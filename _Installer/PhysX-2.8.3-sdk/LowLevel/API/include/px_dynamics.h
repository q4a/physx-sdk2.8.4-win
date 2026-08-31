#ifndef PXD_DYNAMICS_H
#define PXD_DYNAMICS_H

#include "px_config.h"
#include "px_task.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*!
\file
Dynamics interface.
*/

/************************************************************************/
/* Atoms                                                                */
/************************************************************************/

typedef PxdHandle PxdAtom;
typedef PxdHandle PxdContext;

/*!
Atom types
*/
typedef enum PxdAtomType_
{
	/*!
	Not a valid atom type
	*/
	PXD_ATOM_TYPE_NONE = 0,
	/*!
	1-DOF axle.
	NOT IMPLEMENTED
	*/
	PXD_ATOM_TYPE_AXLE = 1,		
	/*!
	3-DOF atom.
	NOT IMPLEMENTED
	*/
	PXD_ATOM_TYPE_PARTICLE,
	/*! 
	6-DOF rigid body atom
	*/
	PXD_ATOM_TYPE_BODY		
} PxdAtomType;

/*!
Atom properties
*/
typedef enum PxdAtomProperty_
{
	/*!
	Atom inverted mass and inertia (1 PxFloat for PXD_ATOM_TYPE_AXLE
	and PXD_ATOM_TYPE_PARTICLE, 4 PxFloats for PXD_ATOM_TYPE_BODY).
	For body atoms, the order is specified as {mass, inertia.x, 
	inertia.y, inertia.z}
	*/
	PXD_ATOM_INVERTED_MASS,
	/*!
	Atom pose (1 PxFloat for PXD_ATOM_TYPE_AXLE,
	3 PxFloats for PXD_ATOM_TYPE_PARTICLE, 
	1 PxdTransform for PXD_ATOM_TYPE_BODY). 
	For body atoms, the order is specified as {pos.x, pos.y, pos.z, 
	rot.x, rot.y, rot.z, rot.w}
	*/
	PXD_ATOM_POSE,
	/*!
	Atom velocity (1 PxFloat for PXD_ATOM_TYPE_AXLE,
	3 PxFloats for PXD_ATOM_TYPE_PARTICLE, 
	6 PxFloats for PXD_ATOM_TYPE_BODY). 
	For body atoms, the order is  specified as {lin.x, lin.y, lin.z, 
	ang.x, ang.y, ang.z} where lin is linear velocity and ang is 
	angular velocity.
	*/
	PXD_ATOM_VELOCITY,
	/*!
	Atom acceleration (1 PxFloat for PXD_ATOM_TYPE_AXLE,
	3 PxFloats for PXD_ATOM_TYPE_PARTICLE, 
	6 PxFloats for PXD_ATOM_TYPE_BODY). 
	For body atoms, the order is specified as {lin.x, lin.y, lin.z, 
	ang.x, ang.y, ang.z} where lin is linear acceleration and ang is 
	angular acceleration.
	*/
	PXD_ATOM_ACCELERATION,
	/*!
	1 PxFloats for PXD_ATOM_TYPE_PARTICLE: linear damping
	2 PxFloats for PXD_ATOM_TYPE_BODY: linear and angular damping
	*/
	PXD_ATOM_DAMPING,
	/*!
	1 PxFloats for PXD_ATOM_TYPE_PARTICLE: linear velocity cap
	2 PxFloats for PXD_ATOM_TYPE_BODY: linear and angular velocity cap.
	The solver will ensure that the velocity remains below this limit
	*/
	PXD_ATOM_VELOCITY_CAP,
	/*!
	Atom active-state (1 PxInt - flag)
	*/
	PXD_ATOM_ACTIVE,
	/*!
	Hint that an atom may have been falsely activated. (1 PxInt - flag)
	*/
	PXD_ATOM_HINT_RESTING,
	/*!
	Set the number of desired iterations in the solver for this atom. 
	Might or might not be used depending on implementation (1 PxInt).
	Defaults to 4.
	*/
	PXD_ATOM_SOLVER_ITERATION_COUNT,
	/*!
	Set that an atoms rotation should be limited to pi/2 radians per timestep.
	\notice This is neccesary for the HL CCD to work. 
	(1 PxInt - flag)
	*/
	PXD_ATOM_SOLVER_CLAMP_TO_HALFPI,
	/*!
	Atom state with both pose and velocity.
	Combination of PXD_ATOM_POSE and PXD_ATOM_VELOCITY.
	(7+6 PxFloat for PXD_ATOM_TYPE_BODY)
	*/
	PXD_ATOM_POSE_AND_VELOCITY,

	/*!
	Contact reporting threshold for atom (atom pairs whose combined normal force magnitudes exceed this are
	reported
	*/
	PXD_ATOM_REPORT_THRESHOLD


} PxdAtomProperty;

/*!
Descriptor for atoms

\sa PxdAtomProperty
*/
typedef struct PxdAtomDesc_
{
	/*!
	Atom type

	\sa PxdAtomType
	*/
	PxdAtomType		type;

	/*!
	Initial atom position

	\sa PXD_ATOM_POSE
	*/
	PxdVector		position;

	/*!
	Initial atom orientation

	\sa PXD_ATOM_POSE
	*/
	PxdQuaternion	orientation;

	/*!
	Initial linear velocity

	\sa PXD_ATOM_VELOCITY
	*/
	PxdVector		linearVelocity;

	/*!
	Initial angular velocity

	\sa PXD_ATOM_VELOCITY
	*/
	PxdVector		angularVelocity;

	/*!
	Initial linear acceleration

	\sa PXD_ATOM_ACCELERATION
	*/
	PxdVector		linearAcceleration;

	/*!
	Initial angular acceleration

	\sa PXD_ATOM_ACCELERATION
	*/
	PxdVector		angularAcceleration;

	/*!
	Inverse diagonalized inertia. 
	The inertia is always specified in atom frame.

	\sa PXD_ATOM_INVERTED_MASS
	*/
	PxdVector		inverseInertia;

	/*!
	Inverse mass.

	\sa PXD_ATOM_INVERTED_MASS
	*/
	PxFloat			inverseMass;

	/*!
	Linear velocity cap.
	The magnitude of the linear velocity before the constraint solver 
	(the unconstrained velocity) is clamped to this value.

	\sa PXD_ATOM_VELOCITY_CAP
	*/
	PxFloat			linearVelocityCap;

	/*!
	Angular velocity cap.
	The magnitude of the angular velocity before the constraint solver 
	(the unconstrained velocity) is clamped to this value.

	\sa PXD_ATOM_VELOCITY_CAP
	*/
	PxFloat			angularVelocityCap;
	
	/*!
	Linear damping

	\sa PXD_ATOM_DAMPING
	*/
	PxFloat			linearDamping;

	/*!
	Angular damping

	\sa PXD_ATOM_DAMPING
	*/
	PxFloat			angularDamping;

	/*!
	Number of iterations in the solver

	\sa PXD_ATOM_SOLVER_ITERATION_COUNT
	*/
	PxInt			solverIterationCount;

	/*!
	Report threshold for contacts

	\sa PXD_ATOM_REPORT_THRESHOLD
	*/
	PxFloat			reportThreshold;

} PxdAtomDesc;


/*!
Initialize an atom descriptor to default values
\param desc
Atom descriptor to initialize
*/
PXD_EXPORT_FUNCTION void PxdAtomInitDesc(PxdAtomDesc* desc);

/*!
Create atom
\param desc
Atom descriptor.
\return
Constraint identifier. Zero on failure.
*/
PXD_EXPORT_FUNCTION PxdAtom PxdAtomCreate(PxdContext context, PxdAtomDesc* desc);

/*!
Destroy atom
\param atom
Atom identifier
*/
PXD_EXPORT_FUNCTION void PxdAtomDestroy(PxdAtom atom);

/*!
Return the atom type for a specific atom
\param atom
Atom identifier
\return
Atom type identifier. PXD_ATOM_TYPE_NONE if atom identifier is invalid.
*/
PXD_EXPORT_FUNCTION PxdAtomType PxdAtomGetType(PxdAtom atom);


/*!
Activate an atom
\param atom
Atom identifier
*/
PXD_EXPORT_FUNCTION void PxdAtomActivate(PxdAtom atom);
/*!
Deactivate an atom
\param atom
Atom identifier
*/
PXD_EXPORT_FUNCTION void PxdAtomDeactivate(PxdAtom atom);

/*!
Test whether an atom is active
\param atom
Atom identifier
\return

*/
PXD_EXPORT_FUNCTION PxBool PxdAtomIsActive(PxdAtom atom);


/*!
\addtogroup Atom property set/get
@{
*/
PXD_EXPORT_FUNCTION void PxdAtomSetInt(PxdAtom atom, PxdAtomProperty property, PxInt value);
PXD_EXPORT_FUNCTION PxInt PxdAtomGetInt(PxdAtom atom, PxdAtomProperty property);

PXD_EXPORT_FUNCTION void PxdAtomSetFloat(PxdAtom atom, PxdAtomProperty property, PxFloat value);
PXD_EXPORT_FUNCTION PxFloat PxdAtomGetFloat(PxdAtom atom, PxdAtomProperty property);

PXD_EXPORT_FUNCTION void PxdAtomSetProperty(PxdAtom atom, PxdAtomProperty property, void* data, size_t size);
PXD_EXPORT_FUNCTION void PxdAtomGetProperty(PxdAtom atom, PxdAtomProperty property, void* data, size_t size);

PXD_EXPORT_FUNCTION void PxdAtomSetVec(PxdAtom atom, PxdAtomProperty property, PxdVector v);
PXD_EXPORT_FUNCTION PxdVector PxdAtomGetVec(PxdAtom atom, PxdAtomProperty property);

PXD_EXPORT_FUNCTION void PxdAtomSetQuat(PxdAtom atom, PxdAtomProperty property, PxdQuaternion q);
PXD_EXPORT_FUNCTION PxdQuaternion PxdAtomGetQuat(PxdAtom atom, PxdAtomProperty property);

PXD_EXPORT_FUNCTION void PxdAtomSetTransform(PxdAtom atom, PxdAtomProperty property, PxdTransform t);
PXD_EXPORT_FUNCTION PxdTransform PxdAtomGetTransform(PxdAtom atom, PxdAtomProperty property);


/*!
After the solver has run, an array of PxdAtomPair structs such that the sum of contact
normal forces between the atoms has exceeded the reporting threshold of one of the bodies
can be retrieved from the context.
*/


typedef struct PxdAtomPair_
{
	PxdAtom atom0;
	PxdAtom atom1;
} PxdAtomPair;

/*!
Get the number of atom pairs for reporting
\return 
Returns the number of body pairs that are fetchable.

*/

PXD_EXPORT_FUNCTION PxUInt PxdAtomGetReportPairCount(PxdContext context);


/*!
Get the atom pairs for reporting

The caller is responsible for allocating a buffer for pairs of sufficient
size to fit at least the number of overlaps reported by 
PxdContextGetBodyAtomPairCount.
\param pairs
Preallocated buffer to fill with pairs
\return
true on successful filling of the buffer, false on error.
*/

PXD_EXPORT_FUNCTION PxUInt PxdAtomFillReportPairBuffer(PxdContext context, PxdAtomPair* pairBuffer);


/*!
@}
*/



/************************************************************************/
/* Constraints                                                          */
/************************************************************************/

typedef PxdHandle PxdConstraint;

/*!
Constraint types
*/
typedef enum PxdConstraintType_
{
	/*!
	Not a valid constraint type.
	*/
	PXD_CONSTRAINT_TYPE_NONE = 0,
	/*! 
	Constraining linear motion along one axis between two atoms 
	*/
	PXD_CONSTRAINT_TYPE_LINEAR = 1,  
	/*! 
	Constraining angular motion around one axis between two atoms 
	*/
	PXD_CONSTRAINT_TYPE_ANGULAR, 
} PxdConstraintType;

/*!
Constraint properties for custom constraints
*/
typedef enum PxdConstraintProperty_
{
	/*!
	Attachment point in local space of atom 0.

	1 PxdVector
	*/
	PXD_CONSTRAINT_POINT0,

	/*!
	Attachment point in local space of atom 1.

	1 PxdVector
	*/
	PXD_CONSTRAINT_POINT1,

	/*!
	Constraint axis in local space of atom 0.

	1 PxdVector
	*/
	PXD_CONSTRAINT_AXIS0,

	/*!
	Constraint axis in local space of atom 1.

	1 PxdVector
	*/
	PXD_CONSTRAINT_AXIS1,

	/*!
	Geometric constraint error.

	1 PxFloat
	*/
	PXD_CONSTRAINT_ERROR,

	/*!
	Target relative velocity along constraint axis.

	1 PxFloat
	*/
	PXD_CONSTRAINT_TARGET,

	/*!
	Minimum allowed constraint impulse.

	1 PxFloat
	*/
	PXD_CONSTRAINT_IMPULSE_MIN,

	/*!
	Max allowed constraint impulse.

	1 PxFloat
	*/
	PXD_CONSTRAINT_IMPULSE_MAX,

	/*!
	Constraint impulse.
	Only valid if PXD_CONSTRAINT_REPORT_IMPULSE flag is set.

	1 PxFloat, read-only
	*/
	PXD_CONSTRAINT_IMPULSE,

	/*!
	Constraint springiness factor

	1 PxFloat
	*/
	PXD_CONSTRAINT_SPRING,

	/*!
	Constraint damping factor

	1 PxFloat
	*/
	PXD_CONSTRAINT_DAMPER,

	/*!
	Enable/disable constraint impulse reporting.

	1 PxInt - flag
	*/
	PXD_CONSTRAINT_REPORT_IMPULSE,
	/*!
	Constraint dominance.  Use PxdConstraintSetFloat(), PxdConstraintGetFloat() to manipulate.
	A dominance of 0 means the corresp. body will 
	not be pushable by the other body in the constraint.	
	Only value pairs (1.0f, 1.0f)<this is the default>, (0.0f, 1.0f), (1.0f, 0.0f) are permitted for the time being.	
	*/
	PXD_CONSTRAINT_DOMINANCE0,
	
	/*!
	Constraint dominance.  Use PxdConstraintSetFloat(), PxdConstraintGetFloat() to manipulate.
	A dominance of 0 means the corresp. body will 
	not be pushable by the other body in the constraint.	
	Only value pairs (1.0f, 1.0f)<this is the default>, (0.0f, 1.0f), (1.0f, 0.0f) are permitted for the time being.	
	*/
	PXD_CONSTRAINT_DOMINANCE1,	
} PxdConstraintProperty;


/*!
Descriptor for linear and angular custom constraints
*/
typedef struct PxdConstraintDesc_
{
	/*!
	Constraint type
	
	\sa PxdConstraintType
	*/
	PxdConstraintType	type;

	/*!
	Atoms the constraint is connected to. Should be of type PXD_ATOM_TYPE_BODY.
	*/
	PxdAtom				atoms[2];

	/*!
	Attachment point 0

	\sa PXD_CONSTRAINT_POINT0
	*/
	PxdVector			point0;

	/*!
	Attachment point 1

	\sa PXD_CONSTRAINT_POINT1
	*/
	PxdVector			point1;

	/*!
	Constraint axis 0

	\sa PXD_CONSTRAINT_AXIS0
	*/
	PxdVector			axis0;

	/*!
	Constraint axis 1

	\sa PXD_CONSTRAINT_AXIS1
	*/
	PxdVector			axis1;

	/*!
	Geometric error

	\sa PXD_CONSTRAINT_ERROR
	*/
	PxFloat				error;

	/*!
	Target velocity

	\sa PXD_CONSTRAINT_TARGET
	*/
	PxFloat				target;

	/*!
	Min impulse

	\sa PXD_CONSTRAINT_IMPULSE_MIN
	*/
	PxFloat				minImpulse;

	/*!
	Max impulse

	\sa PXD_CONSTRAINT_IMPULSE_MAX
	*/
	PxFloat				maxImpulse;

	/*!
	Springiness factor

	\sa PXD_CONSTRAINT_SPRING
	*/
	PxFloat				spring;

	/*!
	Damping factor

	\sa PXD_CONSTRAINT_DAMPING
	*/
	PxFloat				damper;

	/*!
	Impulse reporting

	\sa PXD_CONSTRAINT_REPORT_IMPULSE
	*/
	PxInt				reportImpulse;

	/*!
	This flag is a hack for 2.5 HL wheelshape to work.
	Indicates that reported impulse should include bias-force
	*/
	PxInt				reportImpulseWithBias;

	/*!
	Dominance setting for one way interactions.
	\sa PXD_CONSTRAINT_DOMINANCE0
	*/
	PxFloat dominance0;
	
	/*!
	Dominance setting for one way interactions.
	\sa PXD_CONSTRAINT_DOMINANCE1
	*/
	PxFloat dominance1;

} PxdConstraintDesc;


/*!
Initialize constraint descriptor
*/
PXD_EXPORT_FUNCTION void PxdConstraintInitDesc(PxdConstraintDesc* desc);

/*!
Create constraint
\param context
Context to create constraint in
\param desc
Constraint descriptor with initial properties
\return
Constraint identifier. Zero on failure.
*/
PXD_EXPORT_FUNCTION PxdConstraint PxdConstraintCreate(PxdContext context, PxdConstraintDesc* desc);

/*!
Destroy constraint
\constraint
Constraint identifier
*/
PXD_EXPORT_FUNCTION void PxdConstraintDestroy(PxdConstraint constraint);


/*!
Fill in the atoms on which this constraint operates.
\param constraint
Constraint identifier
\param atoms
An array of atom identifiers to be filled in by this function
\param atomCount
The number of expected atoms. If this number does not match the actual
number of atoms for the specified constraint an error is reported.
*/
PXD_EXPORT_FUNCTION void PxdConstraintGetAtoms(PxdConstraint constraint, PxdAtom* atoms, PxUInt atomCount);

/*!
Return the constraint type for a specific constraint
\param constraint
Constraint identifier
\return
Constraint type identifier. Zero on failure.
*/
PXD_EXPORT_FUNCTION PxdConstraintType PxdConstraintGetType(PxdConstraint constraint);



/*!
\addtogroup Constraint property set/get
@{
*/
PXD_EXPORT_FUNCTION void PxdConstraintSetFloat(PxdConstraint constraint, PxdConstraintProperty property, PxFloat value);
PXD_EXPORT_FUNCTION PxFloat PxdConstraintGetFloat(PxdConstraint constraint, PxdConstraintProperty property);

PXD_EXPORT_FUNCTION void PxdConstraintSetInt(PxdConstraint constraint, PxdConstraintProperty property, PxInt value);
PXD_EXPORT_FUNCTION PxInt PxdConstraintGetInt(PxdConstraint constraint, PxdConstraintProperty property);

PXD_EXPORT_FUNCTION void PxdConstraintSetProperty(PxdConstraint constraint, PxdConstraintProperty property, void* data, size_t size);
PXD_EXPORT_FUNCTION void PxdConstraintGetProperty(PxdConstraint constraint, PxdConstraintProperty property, void* data, size_t size);

PXD_EXPORT_FUNCTION void PxdConstraintSetVec(PxdConstraint Constraint, PxdConstraintProperty property, PxdVector v);
PXD_EXPORT_FUNCTION PxdVector PxdConstraintGetVec(PxdConstraint Constraint, PxdConstraintProperty property);

PXD_EXPORT_FUNCTION void PxdConstraintSetQuat(PxdConstraint Constraint, PxdConstraintProperty property, PxdQuaternion q);
PXD_EXPORT_FUNCTION PxdQuaternion PxdConstraintGetQuat(PxdConstraint Constraint, PxdConstraintProperty property);

PXD_EXPORT_FUNCTION void PxdConstraintSetTransform(PxdConstraint Constraint, PxdConstraintProperty property, PxdTransform t);
PXD_EXPORT_FUNCTION PxdTransform PxdConstraintGetTransform(PxdConstraint Constraint, PxdConstraintProperty property);

/*!
@}
*/

/************************************************************************/
/* Contact constraint                                                   */
/************************************************************************/
typedef PxdHandle PxdContactConstraint;

/*!
Contact constraint properties
*/
typedef enum PxdContactConstraintProperty_
{
	/*!
	Contact frame in local space of atom 0.

	Contact frame is defined to have normal along x-axis and anisotropic
	major axis along y.

	1 PxdTransform
	*/
	PXD_CONTACT_FRAME0,

	/*!
	Contact frame in local space of atom 1.

	Contact frame is defined to have normal along x-axis and anisotropic
	major axis along y.

	1 PxdTransform
	*/
	PXD_CONTACT_FRAME1,

	/*!
	Geometric constraint error in contact frame

	1 PxdVector
	*/
	PXD_CONTACT_ERROR,

	/*!
	Target relative velocity defined in contact frame

	1 PxdVector
	*/
	PXD_CONTACT_TARGET,

	/*!
	Constraint impulse in contact frame.
	Only valid if PXD_CONTACT_REPORT_IMPULSE is set.

	1 PxdVector, read-only
	*/
	PXD_CONTACT_IMPULSE,

	/*!
	Minimum impulse along normal axis.

	1 PxFloat
	*/
	PXD_CONTACT_IMPULSE_MIN,

	/*!
	Max impulse along normal axis.

	1 PxFloat
	*/
	PXD_CONTACT_IMPULSE_MAX,

	/*!
	Static friction coefficient.
	If anisotropic friction is activated this is along the major axis, otherwise
	it is used for all axes.

	1 PxFloat
	*/
	PXD_CONTACT_STATIC_FRICTION0,
	
	/*!
	Second static friction coefficient.
	If anisotropic friction is activated this is along the minor axis, otherwise
	it is ignored

	1 PxFloat
	*/
	PXD_CONTACT_STATIC_FRICTION1,

	/*!
	Dynamic friction coefficient.
	If anisotropic friction is activated this is along the major axis, otherwise
	it is used for all axes.

	1 PxFloat
	*/
	PXD_CONTACT_DYNAMIC_FRICTION0,

	/*!
	Second dynamic friction coefficient.
	If anisotropic friction is activated this is along the minor axis, otherwise
	it is ignored

	1 PxFloat
	*/
	PXD_CONTACT_DYNAMIC_FRICTION1,

	/*!
	Restitution value.
	If restitution is above zero relative target velocity will be calculated
	from the pre-solver normal velocity and this restitution value.

	1 PxFloat
	*/
	PXD_CONTACT_RESTITUTION,

	/*!
	Enable anisotropic friction

	1 PxInt - flag
	*/
	PXD_CONTACT_ANISOTROPIC,

	/*!
	Enable impulse reporting.

	1 PxInt - flag
	*/
	PXD_CONTACT_REPORT_IMPULSE,

	/*!
	Feature index for internally generated contact constraints.

	1 PxInt, read-only
	*/
	PXD_CONTACT_FEATURE0,

	/*!
	Feature index for internally generated contact constraints.

	1 PxInt, read-only
	*/
	PXD_CONTACT_FEATURE1,

	/*!
	Constraint dominance setting.  Use PxdContactConstraintSetFloat(), PxdContactConstraintGetFloat() to manipulate.
	A dominance of 0 means the corresp. body will 
	not be pushable by the other body in the constraint.	
	Only value pairs (1.0f, 1.0f)<this is the default>, (0.0f, 1.0f), (1.0f, 0.0f) are permitted for the time being.	
	*/
	PXD_CONTACT_DOMINANCE0,

	/*!
	Constraint dominance setting.  Use PxdContactConstraintSetFloat(), PxdContactConstraintGetFloat() to manipulate.
	A dominance of 0 means the corresp. body will 
	not be pushable by the other body in the constraint.	
	Only value pairs (1.0f, 1.0f)<this is the default>, (0.0f, 1.0f), (1.0f, 0.0f) are permitted for the time being.	
	*/
	PXD_CONTACT_DOMINANCE1,
	
} PxdContactConstraintProperty;

/*!
Descriptor for contact constraints
*/
typedef struct PxdContactConstraintDesc_
{
	/*!
	Atoms the constraint is connected to. Should be of type PXD_ATOM_TYPE_BODY.
	*/
	PxdAtom				atoms[2];

	/*!
	Contact frame 0

	\sa PXD_CONTACT_FRAME0
	*/
	PxdTransform		frame0;

	/*!
	Contact frame 1

	\sa PXD_CONTACT_FRAME1
	*/
	PxdTransform		frame1;

	/*!
	Geometric error

	\sa PXD_CONTACT_ERROR
	*/
	PxdVector			error;

	/*!
	Target relative velocity

	\sa PXD_CONTACT_TARGET
	*/
	PxdVector			target;

	/*!
	Min impulse

	\sa PXD_CONTACT_IMPULSE_MIN
	*/
	PxFloat				minImpulse;

	/*!
	Max impulse

	\sa PXD_CONTACT_IMPULSE_MAX
	*/
	PxFloat				maxImpulse;

	/*!
	\sa PXD_CONTACT_STATIC_FRICTION0
	*/
	PxFloat				staticFriction0;

	/*!
	\sa PXD_CONTACT_STATIC_FRICTION1
	*/
	PxFloat				staticFriction1;

	/*!
	\sa PXD_CONTACT_DYNAMIC_FRICTION0
	*/
	PxFloat				dynamicFriction0;

	/*!
	\sa PXD_CONTACT_DYNAMIC_FRICTION1
	*/
	PxFloat				dynamicFriction1;

	/*!
	Restitution coefficient

	\sa PXD_CONTACT_RESTITUTION
	*/
	PxFloat				restitution;

	/*!
	Enable anisotropic friction

	\sa PXD_CONTACT_ANISOTROPIC
	*/
	PxInt				anisotropic;

	/*!
	Enable impulse reporting

	\sa PXD_CONTACT_REPORT_IMPULSE
	*/
	PxInt				reportImpulse;

	/*!
	Dominance setting for one way interactions.
	\sa PXD_CONTACT_DOMINANCE0
	*/
	PxFloat				dominance0;
	/*!
	Dominance setting for one way interactions.
	\sa PXD_CONTACT_DOMINANCE1
	*/
	PxFloat				dominance1;

}PxdContactConstraintDesc;

/*!
Initialize a contact constraint descriptor
*/
PXD_EXPORT_FUNCTION void PxdContactConstraintInitDesc(PxdContactConstraintDesc* desc);


/*!
Create a contact constraint

\param context
Context identifier
\param desc
Constraint descriptor with initial parameters
*/
PXD_EXPORT_FUNCTION PxdContactConstraint PxdContactConstraintCreate(PxdContext context, PxdContactConstraintDesc* desc);

/*!
Destroy a contact constraint

\param contact
Contact constraint identifier
*/
PXD_EXPORT_FUNCTION void PxdContactConstraintDestroy(PxdContactConstraint contact);

/*!
Fill in the atoms on which this constraint operates.
\param constraint
Constraint identifier
\param atoms
An array of atom identifiers to be filled in by this function.
Must have space for two atoms.
*/
PXD_EXPORT_FUNCTION void PxdContactConstraintGetAtoms(PxdContactConstraint constraint, PxdAtom* atoms);


/*!
\addtogroup Contact constraint property set/get
@{
*/
PXD_EXPORT_FUNCTION void PxdContactConstraintSetFloat(PxdContactConstraint constraint, PxdContactConstraintProperty property, PxFloat value);
PXD_EXPORT_FUNCTION PxFloat PxdContactConstraintGetFloat(PxdContactConstraint constraint, PxdContactConstraintProperty property);

PXD_EXPORT_FUNCTION void PxdContactConstraintSetInt(PxdContactConstraint constraint, PxdContactConstraintProperty property, PxInt value);
PXD_EXPORT_FUNCTION PxInt PxdContactConstraintGetInt(PxdContactConstraint constraint, PxdContactConstraintProperty property);

PXD_EXPORT_FUNCTION void PxdContactConstraintSetProperty(PxdContactConstraint constraint, PxdContactConstraintProperty property, void* data, size_t size);
PXD_EXPORT_FUNCTION void PxdContactConstraintGetProperty(PxdContactConstraint constraint, PxdContactConstraintProperty property, void* data, size_t size);

PXD_EXPORT_FUNCTION void PxdContactConstraintSetVec(PxdContactConstraint Constraint, PxdContactConstraintProperty property, PxdVector v);
PXD_EXPORT_FUNCTION PxdVector PxdContactConstraintGetVec(PxdContactConstraint Constraint, PxdContactConstraintProperty property);

PXD_EXPORT_FUNCTION void PxdContactConstraintSetQuat(PxdContactConstraint Constraint, PxdContactConstraintProperty property, PxdQuaternion q);
PXD_EXPORT_FUNCTION PxdQuaternion PxdContactConstraintGetQuat(PxdContactConstraint Constraint, PxdContactConstraintProperty property);

PXD_EXPORT_FUNCTION void PxdContactConstraintSetTransform(PxdContactConstraint Constraint, PxdContactConstraintProperty property, PxdTransform t);
PXD_EXPORT_FUNCTION PxdTransform PxdContactConstraintGetTransform(PxdContactConstraint Constraint, PxdContactConstraintProperty property);

/*!
@}
*/

/************************************************************************/
/* D6 Joint interface                                                   */
/************************************************************************/
typedef PxdHandle PxdD6Joint;

/*!
D6 Joint properties.
*/
typedef enum PxdD6JointProperty_
{
	/*!
	Joint attachment frame 0 in local space of atom 0

	1 PxdTransform
	*/
	PXD_D6JOINT_FRAME0,

	/*!
	Joint attachment frame 1 in local space of atom 1

	1 PxdTransform
	*/
	PXD_D6JOINT_FRAME1,

	/*!
	PxInt (12 bits flags), (locked, limited, free)*[x,y,z,tw,sw1,sw2]

	Encoded LSB to MSB as groups of two bits per axis. Each two bits is interpreted as 
	0-free, 1-limited, 2-locked
	*/
	PXD_D6JOINT_JOINT_TYPE,

	/*!
	PxInt (12 bits flags), (pos,vel)*[x,y,z,tw,sw,slerp]

	Encoded LSB to MSB as groups of two bits per axis. Each two bits is interpreted as 
	0-niether, 1-pos, 2-vel, 3-pos+vel
	*/
	PXD_D6JOINT_DRIVE_TYPE,

	/*!
	Limit value along specific axis.

	1 PxFloat, valid subtype PXD_D6JOINT_LINEAR to PXD_D6JOINT_TWL
	*/
	PXD_D6JOINT_LIMIT_VALUE,
	
	/*!
	Limit springiness along specific axis.

	1 PxFloat, valid subtype PXD_D6JOINT_LINEAR to PXD_D6JOINT_TWL
	*/
	PXD_D6JOINT_LIMIT_SPRING,

	/*!
	Limit damping along specific axis.

	1 PxFloat, valid subtype PXD_D6JOINT_LINEAR to PXD_D6JOINT_TWL
	*/
	PXD_D6JOINT_LIMIT_DAMPING,

	/*!
	Limit restitution along specific axis.

	1 PxFloat, valid subtype PXD_D6JOINT_LINEAR to PXD_D6JOINT_TWL
	*/
	PXD_D6JOINT_LIMIT_RESTITUTION,

	/*!
	Drive springiness for given drive axis

	1 PxFloat, valid subtype PXD_D6JOINT_X to PXD_D6JOINT_SLERP
	*/
	PXD_D6JOINT_DRIVE_SPRING,

	/*!
	Drive damping for given drive axis

	1 PxFloat, valid subtype PXD_D6JOINT_X to PXD_D6JOINT_SLERP
	*/
	PXD_D6JOINT_DRIVE_DAMPING,

	/*!
	Drive impulse limit for given drive axis

	1 PxFloat, valid subtype PXD_D6JOINT_X to PXD_D6JOINT_SLERP
	*/
	PXD_D6JOINT_DRIVE_LIMIT,

	/*!
	Angular drive gear ratio

	1 PxFloat
	*/
	PXD_D6JOINT_DRIVE_GEAR,

	/*!
	Drive target frame. 
	Interpreted as desired location of attachment frame 1 relative to frame 0

	1 PxdTransform
	*/
	PXD_D6JOINT_DRIVE_POSITION,

	/*!
	Drive target velocity in format [lin.x lin.y lin.z ang.x ang.y ang.z] 
	where lin is linear velocity and ang angular velocity.

	6 PxFloat
	*/
	PXD_D6JOINT_DRIVE_VELOCITY,

	/*!
	Joint impulse

	\warning This is currently reported in world-space for linear and angular
	components. Should be reviewed for later revisions.

	6 PxFloat, read-only
	*/
	PXD_D6JOINT_IMPULSE,

	/*!
	Max impulse for linear components

	1 PxFloat
	*/
	PXD_D6JOINT_IMPULSE_MAX_LINEAR,

	/*!
	Max impulse for angular components

	1 PxFloat
	*/
	PXD_D6JOINT_IMPULSE_MAX_ANGULAR,

	/*!
	Enable impulse reporting

	1 PxInt - flag
	*/
	PXD_D6JOINT_REPORT_IMPULSE,

	/*!
	Solver extrapolation factor to increase convergence at the risk of stability

	1 PxFloat, [0.5,2]
	*/
	PXD_D6JOINT_SOLVER_EXTRAPOLATION_FACTOR,

	/*!
	Use acceleration spring model

	1 PxInt - flag
	*/
	PXD_D6JOINT_USE_ACCELERATION_SPRINGS,

	/*!
	Constrain movements along directions perpendicular to the distance vector of the two
	anchor points when all axis are linearly limited.

	1 PxInt - flag
	*/
	PXD_D6JOINT_CONSTRAIN_PERPENDICULAR_DIRS,

	/*!
	Read-only property indicating if joint violated max impulses last 
	simulation-step.

	1 PxInt - flag, read-only
	*/
	PXD_D6JOINT_JOINT_BROKE,

	/*!
	Constraint dominance.  Use PxdD6JointSetFloat(), PxdD6JointGetFloat() to manipulate.
	A dominance of 0 means the corresp. body will 
	not be pushable by the other body in the constraint.	
	Only value pairs (1.0f, 1.0f)<this is the default>, (0.0f, 1.0f), (1.0f, 0.0f) are permitted for the time being.	
	*/
	PXD_D6JOINT_DOMINANCE0,
	
	/*!
	Constraint dominance.  Use PxdD6JointSetFloat(), PxdD6JointGetFloat() to manipulate.
	A dominance of 0 means the corresp. body will 
	not be pushable by the other body in the constraint.	
	Only value pairs (1.0f, 1.0f)<this is the default>, (0.0f, 1.0f), (1.0f, 0.0f) are permitted for the time being.	
	*/
	PXD_D6JOINT_DOMINANCE1,
	
} PxdD6JointProperty;

typedef enum PxdD6JointSubProperty_
{
	// Limit sub-roperties
	PXD_D6JOINT_LINEAR,
	PXD_D6JOINT_SW1,
	PXD_D6JOINT_SW2,
	PXD_D6JOINT_TWH,
	PXD_D6JOINT_TWL,

	// Drive sub-properties
	PXD_D6JOINT_X,
	PXD_D6JOINT_Y,
	PXD_D6JOINT_Z,
	PXD_D6JOINT_TW,
	PXD_D6JOINT_SW,
	PXD_D6JOINT_SLERP
} PxdD6JointSubProperty;

/*!
D6 Joint descriptor
*/
typedef struct PxdD6JointDesc_
{
	/*!
	Atoms the joint is connected to. Should be of type PXD_ATOM_TYPE_BODY.
	*/
	PxdAtom					atoms[2];

	/*!
	Joint attachment frame 0

	\sa PXD_D6JOINT_FRAME0
	*/
	PxdTransform			frame0;

	/*!
	Joint attachment frame 1

	\sa PXD_D6JOINT_FRAME1
	*/
	PxdTransform			frame1;

	/*!
	Joint type definition bit-field

	\sa PXD_D6JOINT_JOINT_TYPE
	*/
	PxInt					jointType;

	/*!
	Joint drive definition bit-field

	\sa PXD_D6JOINT_DRIVE_TYPE
	*/
	PxInt					driveType;

	/*!
	Joint limit value
	Array index PXD_D6JOINT_LINEAR-PXD_D6JOINT_TWL

	\sa PXD_D6JOINT_LIMIT_VALUE
	*/
	PxFloat					limitValue[5];

	/*!
	Joint limit springiness
	Array index PXD_D6JOINT_LINEAR-PXD_D6JOINT_TWL

	\sa PXD_D6JOINT_LIMIT_SPRING
	*/
	PxFloat					limitSpring[5];

	/*!
	Joint limit damping
	Array index PXD_D6JOINT_LINEAR-PXD_D6JOINT_TWL

	\sa PXD_D6JOINT_LIMIT_DAMPING
	*/
	PxFloat					limitDamping[5];

	/*!
	Joint limit restitution
	Array index PXD_D6JOINT_LINEAR-PXD_D6JOINT_TWL

	\sa PXD_D6JOINT_LIMIT_RESTITUTION
	*/
	PxFloat					limitRestitution[5];

	/*!
	Joint drive springiness
	Array index PXD_D6JOINT_X-PXD_D6JOINT_SLERP

	\sa PXD_D6JOINT_DRIVE_SPRING
	*/
	PxFloat					driveSpring[6];
	
	/*!
	Joint drive damping
	Array index PXD_D6JOINT_X-PXD_D6JOINT_SLERP

	\sa PXD_D6JOINT_DRIVE_DAMPING
	*/
	PxFloat					driveDamping[6];

	/*!
	Joint drive impulse limit
	Array index PXD_D6JOINT_X-PXD_D6JOINT_SLERP

	\sa PXD_D6JOINT_DRIVE_LIMIT
	*/
	PxFloat					driveLimit[6];

	/*!
	Joint drive angular gear ratio

	\sa PXD_D6JOINT_DRIVE_GEAR
	*/
	PxFloat					driveGear;

	/*!
	Joint drive position target

	\sa PXD_D6JOINT_DRIVE_POSITION
	*/
	PxdTransform			drivePosition;

	/*!
	Joint drive velocity target

	\sa PXD_D6JOINT_DRIVE_GEAR
	*/
	PxdVector				driveVelocity[2];

	/*!
	\sa PXD_D6JOINT_IMPULSE_MAX_LINEAR
	*/
	PxFloat					maxImpulseLinear;

	/*!
	\sa PXD_D6JOINT_IMPULSE_MAX_ANGULAR
	*/
	PxFloat					maxImpulseAngular;

	/*!
	Enable impulse reporting

	\sa PXD_D6JOINT_REPORT_IMPULSE
	*/
	PxInt					reportImpulse;

	/*!
	\sa PXD_D6JOINT_SOLVER_EXTRAPOLATION_FACTOR
	*/
	PxFloat					solverExtrapolationFactor;

	/*!
	\sa PXD_D6JOINT_USE_ACCELERATION_SPRINGS
	*/
	PxInt					useAccelerationSprings;

	/*!
	\sa PXD_D6JOINT_CONSTRAIN_PERPENDICULAR_DIRS
	*/
	PxInt					constrainPerpendicularDirs;

	/*!
	Dominance setting for one way interactions.
	\sa PXD_D6JOINT_DOMINANCE0
	*/
	PxFloat					dominance0;

	/*!
	Dominance setting for one way interactions.
	\sa PXD_D6JOINT_DOMINANCE1
	*/
	PxFloat					dominance1;

} PxdD6JointDesc;


/*!
Initialize a D6 joint descriptor
*/
PXD_EXPORT_FUNCTION void PxdD6JointInitDesc(PxdD6JointDesc* desc);

/*!
Create a D6 joint

\param context
Context identifier
\param desc
D6 Joint descriptor with initial parameters
*/
PXD_EXPORT_FUNCTION PxdD6Joint PxdD6JointCreate(PxdContext context, PxdD6JointDesc* desc);

/*!
Destroy a D6 joint

\param joint
D6 Joint
*/
PXD_EXPORT_FUNCTION void PxdD6JointDestroy(PxdD6Joint joint);

/*!
Fill in the atoms on which this constraint operates.
\param constraint
Constraint identifier
\param atoms
An array of atom identifiers to be filled in by this function. 
Must have space for two atoms.
*/
PXD_EXPORT_FUNCTION void PxdD6JointGetAtoms(PxdD6Joint joint, PxdAtom* atoms);


/*!
\addtogroup D6 joint property set/get
@{
*/
PXD_EXPORT_FUNCTION void PxdD6JointSetFloat(PxdD6Joint joint, PxdD6JointProperty property, PxFloat value);
PXD_EXPORT_FUNCTION PxFloat PxdD6JointGetFloat(PxdD6Joint joint, PxdD6JointProperty property);

PXD_EXPORT_FUNCTION void PxdD6JointSetInt(PxdD6Joint joint, PxdD6JointProperty property, PxInt value);
PXD_EXPORT_FUNCTION PxInt PxdD6JointGetInt(PxdD6Joint joint, PxdD6JointProperty property);

PXD_EXPORT_FUNCTION void PxdD6JointSetProperty(PxdD6Joint joint, PxdD6JointProperty property, void* data, size_t size);
PXD_EXPORT_FUNCTION void PxdD6JointGetProperty(PxdD6Joint joint, PxdD6JointProperty property, void* data, size_t size);

PXD_EXPORT_FUNCTION void PxdD6JointSetVec(PxdD6Joint joint, PxdD6JointProperty property, PxdVector v);
PXD_EXPORT_FUNCTION PxdVector PxdD6JointGetVec(PxdD6Joint joint, PxdD6JointProperty property);

PXD_EXPORT_FUNCTION void PxdD6JointSetQuat(PxdD6Joint joint, PxdD6JointProperty property, PxdQuaternion q);
PXD_EXPORT_FUNCTION PxdQuaternion PxdD6JointGetQuat(PxdD6Joint joint, PxdD6JointProperty property);

PXD_EXPORT_FUNCTION void PxdD6JointSetTransform(PxdD6Joint joint, PxdD6JointProperty property, PxdTransform t);
PXD_EXPORT_FUNCTION PxdTransform PxdD6JointGetTransform(PxdD6Joint joint, PxdD6JointProperty property);

PXD_EXPORT_FUNCTION void PxdD6JointSetEnumFloat(PxdD6Joint joint, PxdD6JointProperty property, PxdD6JointSubProperty sub, PxFloat value);
PXD_EXPORT_FUNCTION PxFloat PxdD6JointGetEnumFloat(PxdD6Joint joint, PxdD6JointProperty property, PxdD6JointSubProperty sub);

/*!
@}
*/






/************************************************************************/
/* Dynamics control                                                     */
/************************************************************************/

/*!
Dynamics control properties.
*/
typedef enum PxdDynamicsProperty_
{
	/*!
	Bounce threshold.
	For relative velocities below this threshold restitution isn't used
	when computing velocity response.
	1 PxFloat
	*/
	PXD_DYNAMICS_BOUNCE_THRESHOLD,

	/*!
	Number of bodies in an island to batch for dispatch to a separate thread.
	1 PxInt
	*/

	PXD_DYNAMICS_SOLVER_BATCH_SIZE

} PxdDynamicsProperty;

/*!
Allocate task identifier for running the dynamics solver.
After this task has fully executed, resulting atom poses and
constraint magnitudes can be retrieved.
\param dt
Time step
*/
PXD_EXPORT_FUNCTION PxdTask PxdDynamicsUpdate(PxdContext context, PxFloat dt);

/*!
\addtogroup Dynamics control property set/get
@{
*/
PXD_EXPORT_FUNCTION void PxdDynamicsSetInt(PxdContext context, PxdDynamicsProperty property, PxInt value);
PXD_EXPORT_FUNCTION PxInt PxdDynamicsGetInt(PxdContext context, PxdDynamicsProperty property);

PXD_EXPORT_FUNCTION void PxdDynamicsSetFloat(PxdContext context, PxdDynamicsProperty property, PxFloat value);
PXD_EXPORT_FUNCTION PxFloat PxdDynamicsGetFloat(PxdContext context, PxdDynamicsProperty property);

PXD_EXPORT_FUNCTION void PxdDynamicsSetProperty(PxdContext context, PxdDynamicsProperty property, void* value, size_t size);
PXD_EXPORT_FUNCTION void PxdDynamicsGetProperty(PxdContext context, PxdDynamicsProperty property, void* value, size_t size);
/*!
@}
*/

#ifdef __cplusplus
}
#endif

#endif
