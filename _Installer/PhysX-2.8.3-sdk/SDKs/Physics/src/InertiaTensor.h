#ifndef NX_PHYSICS_INERTIATENSOR
#define NX_PHYSICS_INERTIATENSOR

/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

	class InertiaTensorComputer
	{
		public:
								InertiaTensorComputer(bool initTozero = true);
								InertiaTensorComputer(const NxMat33& inertia, const NxVec3& com, NxReal mass);
								~InertiaTensorComputer();

					void		zero();														//sets to zero mass
					void		setDiagonal(NxReal mass, const NxVec3 & diagonal);			//sets as a diagonal tensor
					void		rotate(const NxMat33 & rot);								//rotates the mass
					void		translate(const NxVec3 & t);								//translates the mass
					void		transform(const NxMat34 & transform);						//transforms the mass
					void		scaleDensity(NxReal densityScale);		//scales by a density factor
					void		add(const InertiaTensorComputer& it);	//adds a mass
					void		center();//recenters inertia around center of mass

					void		setBox(const NxVec3& halfWidths);							//sets as an axis aligned box
					void		setBox(const NxVec3& halfWidths, const NxMat34 * pose);		//sets as an oriented box
					void		addBox(NxReal density, const NxVec3& halfWidths, const NxMat34 * pose = 0);//adds an oriented box mass to the current state

					void		setSphere(NxReal radius);
					void		setSphere(NxReal radius, const NxMat34 * pose);
					void		addSphere(NxReal density, NxReal radius, const NxMat34 * pose = 0);

					void		setCylinder(int dir, NxReal r, NxReal l);
					void		setCylinder(int dir, NxReal r, NxReal l, const NxMat34 * pose);
					void		addCylinder(NxReal density, int dir, NxReal r, NxReal l, const NxMat34 * pose = 0);

					void		setEllipsoid(NxReal rx, NxReal ry, NxReal rz);
					void		setEllipsoid(NxReal rx, NxReal ry, NxReal rz, const NxMat34 * pose);
					void		addEllipsoid(NxReal density, NxReal rx, NxReal ry, NxReal rz, const NxMat34 * pose = 0);

		NX_INLINE	NxVec3		getCenterOfMass()				const	{ return mG;	}
		NX_INLINE	NxReal		getMass()						const	{ return mMass;	}
		NX_INLINE	NxMat33		getInertia()					const	{ return mI;	}

		private:
					NxMat33		mI;
					NxVec3		mG;
					NxReal		mMass;
	};

#endif
