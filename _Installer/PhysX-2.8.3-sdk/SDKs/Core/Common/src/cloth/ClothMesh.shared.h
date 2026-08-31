/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
// ClothMesh types used on all cores
#ifndef NX_PHYSICS_CLOTHMESH_SHARED_H
#define NX_PHYSICS_CLOTHMESH_SHARED_H

struct ClothInternalClothConstraint {
	NxI16  vertNr[6];
	NxReal stretchingRestLength;	
	NxReal bendingRestLength;
	NxReal bendingAngle;
	NxU32  _pad1;
	NxU16  flags;
	NxU16  _pad2;
};

struct ClothInternalSoftBodyConstraint {
	NxI16  vertNr[4];
	NxReal restVolume;
	NxU16  compressedRestEdgeLengths[6];
	NxU32  _pad1;
	NxU16  flags;
	NxU16  _pad2;
};

// in contrast to ClothInternalConstraint this constraint contains global particle indices (NxI32)
// not relative indices into a work packet
struct ClothGlobalInternalConstraint {
	void copyTo(ClothInternalClothConstraint &c) const
	{
		for (int i = 0; i < 6; i++)
			c.vertNr[i] = -1; 
		c.stretchingRestLength = stretchingRestLength;
		c.bendingRestLength = bendingRestLength;
		c.bendingAngle = bendingAngle;
		c.flags = flags;
	}

	void copyAndCompressTo(ClothInternalSoftBodyConstraint &c) const
	{
		for (int i = 0; i < 4; i++)
			c.vertNr[i] = -1; 
		
		c.restVolume = restVolume;
		for (int i = 0; i < 6; i++) {
			// throw away the 16 least significant mantissa bits to compress the floats
			NxF32 singleFloat = restEdgeLengths[i];
			NxU32 floatBits = *reinterpret_cast<NxU32*>(&singleFloat);
			c.compressedRestEdgeLengths[i] = (NxU16)(floatBits >> 16);
		}
		c.flags = flags;
	}

	NxI32  vertNr[6];
	NxU32  flags;
	NxU32  Pad0;
	union {
		struct {
			// cloth parameters			
			NxReal stretchingRestLength;	
			NxReal bendingRestLength;
			NxReal bendingAngle;
		};
		struct{
			// soft body parameters			
			NxReal restEdgeLengths[6];
			NxReal restVolume;
			NxU32  Pad1;
		};
	};
	
};

#endif


