#ifndef PXC_D6JOINTCONSTRAINTS_H
#define PXC_D6JOINTCONSTRAINTS_H

#include "PxcD6Joint.h"

// done with static polymorphism because apparently dynamic polymorphism annoys the
// PS3 people, and in any case dynamic polymorphism doesn't really have any value here
// since there's no real objection to instancing this code once for SW and once for HW
// - the objective is to have only one copy of the source.

template<class ConcreteConstraintGenerator>
void PxcD6Joint::checkSymmetricLimit(ConcreteConstraintGenerator &ccg,
									 const PxcVector & axis, 
									 PxFloat angle, PxFloat limitAngle,
									 PxFloat padding, const PxcD6Joint::AxisLimit &limit) const
{
	// handle symmetric angular limits. Rotations are parameterized by tan(theta/2) 
	// or tan(theta/4) and we 'pad' the limit so that the joint doesn't repeatedly gain 
	// and lose touch - the same thing skinWidth does for contacts.

	PxFloat paddedLimit = limitAngle - padding;
	if(angle < -paddedLimit)	ccg.ang(-axis, -(-limitAngle - angle), &limit);
	if(angle > paddedLimit)		ccg.ang(axis,   (limitAngle - angle),  &limit);
}

template<class ConcreteConstraintGenerator>
void PxcD6Joint::createLinearDriveConstraints(PxcD6JointState &state,
											  ConcreteConstraintGenerator &ccg) const
{
	const PxcMatrix33 &cA2wMat = state.cA2wMat;
	const PxcTransform &cB2cA = state.cB2cA;

	for(PxUInt i = 0; i < 3; i++)
	{
		const AxisDrive& drive = mDrive[i];

		if(mLinMotion[i] == MOTION_LOCKED || drive.mType == 0)
			continue; 
		
		// legacy comment from Pxs: "This check isn't accurate with the limiting done above."

		if(mLinMotion[i] == MOTION_LIMITED &&  
			(mDriveLinearVelocity[i] < 0.0f && cB2cA.translation[i] < -mLimits[0].mValue ||
			 mDriveLinearVelocity[i] > 0.0f && cB2cA.translation[i] > mLimits[0].mValue))
			continue;

		PxFloat sign = -PxcSign(mDriveLinearVelocity[i]);
		PxFloat targetVelocity = drive.mType & DRIVE_VELOCITY ? PxcAbs(mDriveLinearVelocity[i]) : 0;
		PxcVector axis = cA2wMat[i]*sign;
		PxFloat error = mDrivePosition.translation[i] - cB2cA.translation[i];

		if(drive.mType & DRIVE_POSITION)
			ccg.linSpring(axis, targetVelocity, error*sign, drive, (drive.mType & DRIVE_VELOCITY) != 0);
		else
			ccg.linMotor(axis, targetVelocity, 0.0f, -drive.mImpulseLimit, drive.mImpulseLimit, true);
	}
}

template<class ConcreteConstraintGenerator>
void PxcD6Joint::createAngularDriveConstraints(PxcD6JointState &state,
											   ConcreteConstraintGenerator &ccg) const
{
	if(mAngularDrive)
	{
		if(mDrive[5].mType != 0 && mAngularLockedCount == 0)
			createSlerpDriveConstraints(state,ccg);
		else
			createSwingAndTwistDriveConstraints(state,ccg);
	}

	if(mDriveGear != 0.0f)
		ccg.angMotor(state.cA2w.getBasisVector0()*mDriveGear, state.cB2w.getBasisVector0(), 0, 0, 
					  -PXD_FLOAT_MAX, PXD_FLOAT_MAX, false);
}


template<class ConcreteConstraintGenerator>
void PxcD6Joint::createSlerpDriveConstraints(PxcD6JointState &state,
											 ConcreteConstraintGenerator &ccg) const
{
	// the constraint is d2w' * c2Bw = identity, which we implement by constraining the x,y,z 
	// elements of this quaternion to zero. This gives us the JwQ position constraint (see 
	// comments in computeJwQ) on the world-space angular velocity. The velocity target is  
	// transformed into the same constraint space, but starting from the constraint frame of body A

	state.computeDriveQuaternions(mDrivePosition.rotation);

	const PxcMatrix33 &cA2wMat = state.cA2wMat;
	const PxcTransform &cA2w = state.cA2w, &cB2w = state.cB2w, &cB2cA = state.cB2cA;
	const PxcQuaternion &d2cA = state.d2cA, &d2w = state.d2w;

	const AxisDrive &drive = mDrive[5];
	
	if(drive.mType & DRIVE_POSITION)
	{		
		PxcVector JwQd[4];							
		computeJwQ(JwQd, d2w, cB2w.rotation);

		bool hasVelocity = (drive.mType & DRIVE_VELOCITY) != 0;													
		PxcQuaternion targetVelocity = hasVelocity ? d2cA.getConjugate() *
													 PxcQuaternion::create(mDriveAngularVelocity, 0.0f) *
													 cB2cA.rotation
												   : PxcQuaternion::create(0,0,0,1);

		PxcQuaternion posError = d2w.getConjugate() * cB2w.rotation;

		for(PxUInt i = 0; i < 3; i++)
			ccg.angSpring(JwQd[i], targetVelocity[i], -2.0f*posError[i], drive, hasVelocity);
	}

	// dsequeira: why do we allow only one kind of drive for the linear case, but for the angular
	// case possibly generate both position and velocity??

	if(mDrive[5].mType & DRIVE_VELOCITY)
	{
		for(PxUInt i = 0; i < 3; i++)
		{
			PxFloat sign = PxcSign(mDriveAngularVelocity[i]);
			PxcVector axis = cA2wMat[i] * sign;
			ccg.angMotor(axis, axis, mDriveAngularVelocity[i]*sign, 0.0f, 0, drive.mImpulseLimit, true);
		}
	}
}

template<class ConcreteConstraintGenerator>
void PxcD6Joint::createSwingAndTwistDriveConstraints(PxcD6JointState &state,
													 ConcreteConstraintGenerator &ccg) const
{
	state.computeDriveQuaternions(mDrivePosition.rotation);

	const PxcMatrix33 &cA2wMat = state.cA2wMat, &cB2wMat = state.cB2wMat;
	const PxcTransform &cA2w = state.cA2w, &cB2w = state.cB2w, &cB2cA = state.cB2cA;
	const PxcQuaternion &d2cA = state.d2cA, &d2w = state.d2w;

	if(mAngMotion[0] != MOTION_LOCKED)
	{
		const AxisDrive &twistDrive = mDrive[3];

		if(twistDrive.mType & DRIVE_POSITION)
		{
			PxcQuaternion align = rotationArc(d2w.getBasisVector0(), cB2wMat.base0);

			PxcQuaternion deltaTwist = cB2w.rotation.getConjugate() * align * d2w;		
			PxFloat da = (deltaTwist.w < 0 ? -deltaTwist.v.x:deltaTwist.v.x) * 2.0f;	// chord length
			ccg.angSpring(cB2wMat.base0, 0.0f, da, twistDrive, false);
		}

		if(twistDrive.mType & DRIVE_VELOCITY)
		{
			// legacy comment from Pxs: "The following will likely have to be redone:"
			PxFloat sign = PxcSign(mDriveAngularVelocity.x);
			ccg.angMotor(cB2wMat.base0*sign, cB2wMat.base0*sign, mDriveAngularVelocity.x*sign, 0, 
						 -twistDrive.mImpulseLimit, twistDrive.mImpulseLimit, true);
		}
	}

	const AxisDrive &swingDrive = mDrive[4];
	PxcVector target = (cA2w.rotation * d2cA).getBasisVector0();

	for(PxUInt i = 1; i < 3; i++)					// Swing
	{
		if(mAngMotion[i]!=MOTION_LOCKED)
		{
			if(swingDrive.mType & DRIVE_POSITION)
				ccg.angSpring(cB2wMat[i], 0.0f, target.dot(i==1?-cB2wMat[2]:cB2wMat[1]), swingDrive, false);

			if(swingDrive.mType & DRIVE_VELOCITY)
			{
				// legacy comment from Pxs: "this code likely will need to be revised:"
				PxFloat sign = PxcSign(mDriveAngularVelocity[i]);
				ccg.angMotor(cA2wMat[i]*sign, cA2wMat[i]*sign, mDriveAngularVelocity[i]*sign, 0, 
							 -swingDrive.mImpulseLimit, swingDrive.mImpulseLimit, true);
			}
		}
	}
}

template<class ConcreteConstraintGenerator>
void PxcD6Joint::createAngularHardConstraints(PxcD6JointState &state,
											  ConcreteConstraintGenerator &ccg) const
{
	// legacy comment from Pxs: "TODO fix this properly. .really ugly hack to fix TTP 1716"
	// (dsequeira: and from  the look of TTP#1716 no-one knows why this fix works - hmm...
	//  wonder if the sensitivity of the rotation difference quat is to jitter in w is relevant?
	//  why don't we use the dot product of quats to determine, as in slerping?)
	
	if(PxcAbs(state.cB2cA.rotation.v.x) < 0.0001f)	state.cB2cA.rotation.v.x = 0;
	if(PxcAbs(state.cB2cA.rotation.v.y) < 0.0001f)	state.cB2cA.rotation.v.y = 0;
	if(PxcAbs(state.cB2cA.rotation.v.z) < 0.0001f)	state.cB2cA.rotation.v.z = 0;
	if(PxcAbs(state.cB2cA.rotation.w) < 0.0001f)	state.cB2cA.rotation.w = 0;

	state.cB2cA.rotation = state.cB2cA.rotation.getNormalized();	

	bool swapDiff = state.cB2cA.rotation.w < 0.0f;

	if(swapDiff)
		state.cB2cA.rotation = -state.cB2cA.rotation;

	const PxcMatrix33 &cA2wMat = state.cA2wMat, &cB2wMat = state.cB2wMat;
	const PxcTransform &cA2w = state.cA2w, &cB2w = state.cB2w, &cB2cA = state.cB2cA;
	PxcMatrix33 cB2cAMat(cB2cA.rotation);

	if(mAngularLockedCount)
	{
		PxcVector JwQ[4]; 
		computeJwQ(JwQ, cA2w.rotation, swapDiff ? -cB2w.rotation : cB2w.rotation);

		ccg.setNumIdenticalHint(mAngularLockedCount);

		if(mAngMotion[0] == MOTION_LOCKED) ccg.ang(JwQ[0], -2.0f*cB2cA.rotation.v[0]);

		if(mAngularLockedCount>1)
		{
			if(mAngMotion[1] == MOTION_LOCKED) ccg.ang(JwQ[1], -2.0f*cB2cA.rotation.v[1]);
			if(mAngMotion[2] == MOTION_LOCKED) ccg.ang(JwQ[2], -2.0f*cB2cA.rotation.v[2]);
		}
		else if(mAngMotion[1] == MOTION_LOCKED) ccg.ang(cB2wMat.base0.cross(cA2wMat.base2), -cB2cAMat(2,0)); 
		else if(mAngMotion[2] == MOTION_LOCKED) ccg.ang(cB2wMat.base0.cross(cA2wMat.base1), -cB2cAMat(1,0));
	}

	// Angular limited
	PxcQuaternion swing, twist;

	if(mAngularLimited)		
		separateSwingTwist(cB2cA.rotation, swing, twist);

	// If both swing degrees are limited, we use an elliptical limit 

	if( mAngMotion[1] == MOTION_LIMITED && mAngMotion[2] == MOTION_LIMITED)	
	{		
		PxFloat w1 = swing.v.y*swing.v.y;
		PxFloat w2 = swing.v.z*swing.v.z;

		PxFloat r = w1 + w2;
		if(r != 0.0f)
		{
			w1 /= r;
			w2 /= r;

			PxFloat tanQuarterLimit = interpolateEllipse(w1,w2,mTanQuarterSwing1Max,mTanQuarterSwing2Max);
			r = PxcSqrt(r);
			PxFloat tanQuarterPhi = r / (1.0f + swing.w);
			if(tanQuarterPhi > tanQuarterLimit - TANQUARTER_PAD)
			{
				AxisLimit limit;
				limit.mRestitution	= interpolateEllipse(w1,w2,mLimits[1].mRestitution,mLimits[2].mRestitution);
				limit.mSpring		= interpolateEllipse(w1,w2,mLimits[1].mSpring, mLimits[2].mSpring);
				limit.mDamping		= interpolateEllipse(w1,w2,mLimits[1].mDamping, mLimits[2].mDamping);
				PxcVector axis = (cA2wMat.base1*swing.v.y + cA2wMat.base2*swing.v.z).getNormalized();			
				ccg.ang(axis, (tanQuarterLimit - tanQuarterPhi), &limit);
			}
		}
	}

	// Otherwise if we have one swing locked we allow +/- 180 degree limits on the other, but if
	// one is free we only allow +/- 90 degree limits on the other.

	else if(mAngMotion[1] == MOTION_LIMITED)
	{
		if(mAngMotion[2] == MOTION_LOCKED)
		{
			PxFloat tanQuarterPhi = swing.v.y / (1.0f + swing.w);
			checkSymmetricLimit(ccg, cA2wMat.base1, tanQuarterPhi, mTanQuarterSwing1Max, TANQUARTER_PAD, mLimits[1]);
		}
		else
		{	
			PxFloat sin = -cB2cAMat(2,0);
			PxFloat cos = PxcSqrt(cB2cAMat(0,0)*cB2cAMat(0,0) + cB2cAMat(1,0)*cB2cAMat(1,0));
			PxFloat tanHalfPhi = sin / (1.0f + cos);
			PxcVector axis = cA2wMat.base2.cross(cB2wMat.base0);

			checkSymmetricLimit(ccg, axis, tanHalfPhi, mTanHalfSwing1Max, TANHALF_PAD, mLimits[1]);
		}
	}
	else if(mAngMotion[2] == MOTION_LIMITED)
	{
		if(mAngMotion[1] == MOTION_LOCKED)
		{
			PxFloat tanQuarterPhi = swing.v.z / (1.0f + swing.w);
			checkSymmetricLimit(ccg, cA2wMat.base2, tanQuarterPhi, mTanQuarterSwing2Max, TANQUARTER_PAD,mLimits[2]);
		}
		else
		{
			PxFloat sin = cB2cAMat(1,0);
			PxFloat cos = PxcSqrt(cB2cAMat(0,0)*cB2cAMat(0,0) + cB2cAMat(2,0)*cB2cAMat(2,0));
			PxFloat tanHalfPhi = sin / (1.0f + cos);
			PxcVector axis = -cA2wMat.base1.cross(cB2wMat.base0);

			checkSymmetricLimit(ccg, axis, tanHalfPhi, mTanHalfSwing2Max, TANHALF_PAD, mLimits[2]);
		}
	}

	if(mAngMotion[0] == MOTION_LIMITED)
	{
		PxFloat tanQuarterPhi = twist.v.x / (1.0f + twist.w);

		if(tanQuarterPhi < mTanQuarterTwistLow+TANQUARTER_PAD)
			ccg.ang(-cB2wMat.base0, -(mTanQuarterTwistLow - tanQuarterPhi), &mLimits[4]);

		if(tanQuarterPhi > mTanQuarterTwistHigh-TANQUARTER_PAD)
			ccg.ang(cB2wMat.base0, (mTanQuarterTwistHigh - tanQuarterPhi), &mLimits[3]);
	}
}


template<class ConcreteConstraintGenerator>
void PxcD6Joint::createLinearHardConstraints(PxcD6JointState &state,
											 ConcreteConstraintGenerator &ccg) const
{
	const PxcMatrix33 &cA2wMat = state.cA2wMat;
	const PxcTransform &cB2cA = state.cB2cA;

	if(mLinearLimited)
	{
		PxcVector linearLimitDir(0,0,0);

		for(PxUInt i = 0; i < 3; i++)
		{
			if(mLinMotion[i] == MOTION_LIMITED)
				linearLimitDir += cA2wMat[i] * cB2cA.translation[i];
		}

		PxFloat distance = linearLimitDir.magnitude();
		PxFloat error = distance - mLimits[0].mValue;

		if(error > -LINEAR_PAD && distance > 0)
			ccg.lin(linearLimitDir * (1.0f/distance), -error, &mLimits[0]);
	}

	if(mLinearLockedCount)
	{
		ccg.setNumIdenticalHint(mLinearLockedCount);

		if(mLinearLockedCount == 3 && cB2cA.translation.magnitudeSquared() > PXD_FLOAT_EPSILON)
		{		
			const PxcVector worldDiff = cA2wMat.transform(cB2cA.translation);  // use best constraint axis
			const PxcVector linVector = worldDiff.getNormalized();			
			PxcVector tangent0, tangent1;
			PxcNormalToTangents(linVector, tangent0, tangent1);

			ccg.lin(linVector, -linVector.dot(worldDiff));
			ccg.lin(tangent0, 0);
			ccg.lin(tangent1, 0);
		}
		else
		{
			for(PxUInt i = 0; i < 3; i++)
			{
				if(mLinMotion[i] == MOTION_LOCKED)
					ccg.lin(cA2wMat[i], -cB2cA.translation[i]);
			}
		}
	}
}


#endif
