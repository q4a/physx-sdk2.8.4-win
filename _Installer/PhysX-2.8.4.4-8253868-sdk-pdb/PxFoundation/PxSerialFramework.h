// This code contains NVIDIA Confidential Information and is disclosed 
// under the Mutual Non-Disclosure Agreement.
//
// Notice
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES 
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO 
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT, 
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable. 
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such 
// information or for any infringement of patents or other rights of third parties that may 
// result from its use. No license is granted by implication or otherwise under any patent 
// or patent rights of NVIDIA Corporation. Details are subject to change without notice. 
// This code supersedes and replaces all information previously supplied. 
// NVIDIA Corporation products are not authorized for use as critical 
// components in life support devices or systems without express written approval of 
// NVIDIA Corporation.
//
// Copyright © 2009 NVIDIA Corporation. All rights reserved.
// Copyright © 2002-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright © 2001-2006 NovodeX. All rights reserved.   

#ifndef PX_SERIAL_FRAMEWORK_H
#define PX_SERIAL_FRAMEWORK_H

#include "Px.h"

// PX_SERIALIZATION

#include "PsArray.h"
#include "PxFields.h"

namespace physx
{
namespace shdfnd2 {
	// todo: shouldn't need to forward-declare from shared foundation
	struct FieldDescriptor;
	class FieldDescriptors;
}
namespace pubfnd2
{
	typedef PxU16 PxType;
	class PxSerializable;
	class PxSerialStream;

	//! Objects are written in a fixed order within a serialized file.
	enum PxSerialOrder
	{
		PX_SERIAL_ORDER_CONVEX		= 20,
		PX_SERIAL_ORDER_TRIMESH		= 20,
		PX_SERIAL_ORDER_DEFMESH		= 20,
		PX_SERIAL_ORDER_HEIGHTFIELD	= 20,
		PX_SERIAL_ORDER_MATERIAL	= 50,
		PX_SERIAL_ORDER_DEFAULT		= 100,
		PX_SERIAL_ORDER_JOINT		= 150,
		PX_SERIAL_ORDER_CONSTRAINT	= 200,
		PX_SERIAL_ORDER_WHEEL		= 250,
	};

	/**
	\brief Class used to "resolve pointers" during deserialization.

	The ref-resolver remaps pointers to PxSerializable objects within a deserialized memory block.
	This class is mainly used by the serialization framework. Users should not have to worry about it.

	@see PxSerializable
	*/
	class PxRefResolver
	{
		public:
		virtual					~PxRefResolver()														{}

		/**
		\brief Retrieves new address of deserialized object

		This is called by the serialization framework.

		\param[in]	oldAddress	Old address of PxSerializable object. See #PxSerializable
		\return		New address of PxSerializable object

		@see PxSerializable
		*/
		virtual	PxSerializable*	newAddress(PxSerializable* oldAddress) const	= 0;

		/**
		\brief Sets new address of deserialized object.

		This is called by the serialization framework.

		\param[in]	oldAddress	Old address of PxSerializable object. See #PxSerializable
		\param[in]	newAddress	New address of PxSerializable object. See #PxSerializable

		@see PxSerializable
		*/
		virtual	void			setNewAddress(PxSerializable* oldAddress, PxSerializable* newAddress)	= 0;
	};

	/**
	\brief Container for user-defined names/references

	This is mainly a "link" object accessed by the framework when serializing and
	deserializing subsets.

	@see PxSerializable
	*/
	class PxUserReferences
	{
		public:
		virtual					~PxUserReferences()														{}

		/**
		\brief Gets PxSerializable object from its user-data/name

		This is called by the framework during deserialization, when import names are
		passed to the deserialize function.

		\param[in]	userData	user-defined name for this object
		\return		Corresponding object, or NULL if not found

		@see PxSerializable
		*/
		virtual	PxSerializable*	getObjectFromID(void* userData) const									= 0;

		/**
		\brief Sets user-data/name for a PxSerializable

		This is called by the framework during deserialization, when export names are
		retrieved from a deserialized collection.

		This can also be called by users to link a subset to a completely different
		subset than the one it was originally linked to.

		\param[in]	object		Serializable object. See #PxSerializable
		\param[in]	userData	user-defined name for this object

		@see PxSerializable
		*/
		virtual	void			setUserData(PxSerializable* object, void* userData)						= 0;
	};

	enum PxSerialTypes
	{
		NXTYPE_UNDEFINED,
		NXTYPE_EXTRA_DATA,

		NXTYPE_NP_HEIGHTFIELD,
		NXTYPE_NP_CONVEX_MESH,
		NXTYPE_NP_TRIANGLE_MESH,
		NXTYPE_NP_DEFORMABLE_MESH,

		NXTYPE_NP_RIGID_DYNAMIC,
		NXTYPE_NP_RIGID_STATIC,
		NXTYPE_NP_SHAPE,
		NXTYPE_NP_MATERIAL,
		NXTYPE_NP_CONSTRAINT,
		NXTYPE_NP_DEFORMABLE,
		NXTYPE_NP_PARTICLE_SYSTEM,
		NXTYPE_NP_PARTICLE_FLUID,
		NXTYPE_NP_WHEEL,
		NXTYPE_NP_ATTACHMENT,

//		NXTYPE_SC_BODY_CORE,
//		NXTYPE_SC_RIGID_CORE,
//		NXTYPE_SC_SHAPE_CORE,
//		NXTYPE_SC_CONSTRAINT_CORE,
//		NXTYPE_SC_DEFORMABLE_CORE,
//		NXTYPE_SC_PARTICLE_SYSTEM_CORE,
//		NXTYPE_SC_WHEEL_CORE,

		NXTYPE_PXS_PARTICLE_SYSTEM_CORE,
		NXTYPE_PXS_DEFORMABLE_CORE,

		// Bad design here: the following types are "user types" and shouldn't be here in the end
		NXTYPE_USER_SPHERICAL_JOINT,
		NXTYPE_USER_REVOLUTE_JOINT,
		NXTYPE_USER_PRISMATIC_JOINT,
		NXTYPE_USER_FIXED_JOINT,
		NXTYPE_USER_DISTANCE_JOINT,
		NXTYPE_USER_D6_JOINT,

		NXTYPE_LAST
	};

	enum PxSerialFlags
	{
		PX_SERIAL_OWNS_MEMORY			= (1<<0),
//		PX_SERIAL_DISABLED				= (1<<1),
		PX_SERIAL_DISABLE_AUTO_RESOLVE	= (1<<1),
		PX_SERIAL_DISABLE_FIELDS		= (1<<2),
	};

/**
\brief Collection class for serialization.

A collection is a container for serializable SDK objects. All serializable SDK objects inherit from PxSerializable.
Serialization and deserialization only work through collections.

A scene is typically serialized using the following steps:

1) create a collection
2) collect objects to serialize
3) serialize collection
4) release collection

For example the code may look like this:

	PxPhysics* physics;	// The physics SDK object
	PxScene* scene;		// The physics scene
	SerialStream s;		// The user-defined stream doing the actual write to disk

	PxCollection* collection = physics->createCollection();	// step 1)
	physics->collectForExport(*collection);					// step 2)
	scene->collectForExport(*collection);					// step 2)
	collection->serialize(s);								// step 3)
	physics->releaseCollection(collection);					// step 4)

A scene is typically deserialized using the following steps:

1) load a serialized block somewhere in memory
2) create a collection object
3) deserialize objects (populate collection with objects from the memory block)
4) add collected objects to scene
5) release collection

For example the code may look like this:

	PxPhysics* physics;	// The physics SDK object
	PxScene* scene;		// The physics scene
	void* memory16;		// a 16-byte aligned buffer previously loaded from disk by the user	- step 1)

	PxCollection* collection = physics->createCollection();	// step 2)
	collection->deserialize(memory16, NULL, NULL);			// step 3)
	physics->addCollection(*collection, scene);				// step 4)
	physics->releaseCollection(collection);					// step 5)

@see PxSerializable
*/
	class PxCollection
	{
		friend class PxSerializable;
		virtual	void						addUnique(PxSerializable*)	= 0;

	public:
											PxCollection()	{}
		virtual								~PxCollection()	{}

		/**
		\brief Serializes a collection.

		Writes out collected objects to a binary stream. Objects are output in the order
		defined by PxSerialOrder, according to their type.

		"Export names" and "import names", as defined by the setUserData and
		addExternalRef functions, are also serialized.

		\param[in]	stream	User-defined serialization stream. See #PxSerialStream

		@see PxSerialStream PxSerialOrder setUserData addExternalRef
		*/
		virtual	void						serialize(PxSerialStream& stream)	= 0;

		/**
		\brief Deserializes a collection.

		Initializes/creates objects within the given input buffer, which must have
		been deserialized from disk already by the user. The input buffer must be
		16-bytes aligned.
		
		Deserialized objects are added to the collection.

		Export names for the collection can be retrieved, if necessary.
		Import names from another collection can be passed, if necesary.

		\param[in]	buffer16		Deserialized input buffer, 16-bytes aligned
		\param[out]	exportNames		Possible export names, or NULL. See #PxUserReferences
		\param[in]	importNames		Possible import names, or NULL. See #PxUserReferences
		\return		True if success

		@see PxUserReferences
		*/
		virtual	bool						deserialize(void* buffer16, PxUserReferences* exportNames, const PxUserReferences* importNames)	= 0;

		/**
		\brief Sets user-data/name for a PxSerializable

		This is used to assign a user-defined data to a PxSerializable. This data is then used
		as the object's name, its reference within other collections. For example when serializing
		subsets, one collection (containing one subset) might have a reference to an object in
		another collection (another subset). The system needs to know about those external references.

		This user-data is also known as an "export name".

		\param[in]	object		Serializable object. See #PxSerializable
		\param[in]	userData	user-defined name for this object

		@see PxSerializable addExternalRef
		*/
		virtual	void						setUserData(PxSerializable* object, void* userData)	= 0;

		/**
		\brief Declares an external reference to the collection

		Some objects in the collection might have pointers/references to objects that are not within
		the same collection. The system needs to know about those external references to properly
		recreate objects during deserialization.

		This user-data is also known as an "import name".

		\param[in]	object		Serializable object. See #PxSerializable
		\param[in]	userData	user-defined name for this object

		@see PxSerializable setUserData
		*/
		virtual	void						addExternalRef(PxSerializable* object, void* userData)	= 0;

		/**
		\brief Gets number of objects in the collection
		\return	Number of objects in the collection
		*/
		virtual	PxU32						getNbObjects()	= 0;

		/**
		\brief Gets object from the collection

		\param[in]	index	Object index, between 0 (incl) and getNbObjects() (excl).
		\return		Desired object from the collection

		@see PxSerializable
		*/
		virtual	PxSerializable*				getObject(PxU32 index)	= 0;
	};

	/**
	\brief Base class for serializable objects

	@see PxRefResolver PxCollection PxSerialStream
	*/
	class PxSerializable
	{
		public:
											PxSerializable(PxRefResolver& v)
											{
												mSerialFlags &= ~PX_SERIAL_OWNS_MEMORY;
												v.setNewAddress(mMyAddress, this);
												mMyAddress = this;
											}
											PxSerializable() : mType(NXTYPE_UNDEFINED), mSerialFlags(PX_SERIAL_OWNS_MEMORY)
											{
												mMyAddress = this;
											}
		virtual								~PxSerializable()											{}

		virtual		PxU32					getOrder()											const	{ return PX_SERIAL_ORDER_DEFAULT;						}
		virtual		void					collectForExport(PxCollection& c)							{ c.addUnique(this);									}

		virtual		bool					getFields(shdfnd2::FieldDescriptors&, PxU32)		const	{ return true;											}
		virtual		bool					getFields(shdfnd2::FieldDescriptors&, PxField::Enum)const	{ return true;											}
		virtual		bool					getFields(shdfnd2::FieldDescriptors&)				const	{ return true;											}
		virtual		const shdfnd2::FieldDescriptor*	getFieldDescriptor(const char*)				const	{ return NULL;											}

		virtual		PxU32					getObjectSize()										const	= 0;

		virtual		void					exportExtraData(PxSerialStream&)							{														}
		virtual		char*					importExtraData(char* address, PxU32&)						{ return address;										}
		virtual		bool					resolvePointers(PxRefResolver&, void*)						{ return true;											}

		virtual		const char*				getClassName()										const	{ return NULL;											}
		virtual		bool					isKindOf(const char*)								const	{ return false;											}

//		PX_INLINE	void					disableSerialization()										{ mSerialFlags |= PX_SERIAL_DISABLED;					}
//		PX_INLINE	void					enableSerialization()										{ mSerialFlags &= ~PX_SERIAL_DISABLED;					}
//		PX_INLINE	PxU16					isSerializationDisabled()							const	{ return PxU16(mSerialFlags & PX_SERIAL_DISABLED);		}

		PX_INLINE	void					disableAutoResolve()										{ mSerialFlags |= PX_SERIAL_DISABLE_AUTO_RESOLVE;					}
		PX_INLINE	void					enableAutoResolve()											{ mSerialFlags &= ~PX_SERIAL_DISABLE_AUTO_RESOLVE;					}
		PX_INLINE	PxU16					isAutoResolveDisabled()								const	{ return PxU16(mSerialFlags & PX_SERIAL_DISABLE_AUTO_RESOLVE);		}

		PX_INLINE	void					disableFields()												{ mSerialFlags |= PX_SERIAL_DISABLE_FIELDS;					}
		PX_INLINE	void					enableFields()												{ mSerialFlags &= ~PX_SERIAL_DISABLE_FIELDS;				}
		PX_INLINE	PxU16					areFieldsDisabled()									const	{ return PxU16(mSerialFlags & PX_SERIAL_DISABLE_FIELDS);	}

		PX_INLINE	PxU16					ownsMemory()										const	{ return PxU16(mSerialFlags & PX_SERIAL_OWNS_MEMORY);	}
		PX_INLINE	PxSerializable*			getAddress()										const	{ return mMyAddress;									}
		PX_INLINE	PxType					getType()											const	{ return mType;											}
		protected:
		PX_INLINE	void					setType(PxType t)											{ mType = t;											}
		private:
					PxType					mType;			// Some kind of class identifier. Could use a string = class name
					PxU16					mSerialFlags;	// Serialization flags
					PxSerializable*			mMyAddress;		// Used for automatic ref-resolving
	};

	#define PX_DECLARE_SERIAL_RTTI(current_class, base_class)									\
	virtual	const char*	getClassName()	const	{ return #current_class;	}					\
	virtual	bool		isKindOf(const char* name)	const										\
						{																		\
							if(strcmp(current_class::getClassName(), name)==0) return true;		\
							else return base_class::isKindOf(name);								\
						}

	#define PX_SERIAL_DYNAMIC_CAST(current_class, class_name)	(!current_class || !current_class->isKindOf(#class_name)) ? NULL : static_cast<##class_name*>(current_class)

//~PX_SERIALIZATION

} // namespace pubfnd2
} // namespace physx

#endif
