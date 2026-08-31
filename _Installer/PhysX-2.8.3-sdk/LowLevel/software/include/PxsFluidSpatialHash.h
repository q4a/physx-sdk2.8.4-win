#ifndef PXS_FLUID_SPATIAL_HASH_H
#define PXS_FLUID_SPATIAL_HASH_H

#include "PxsFluidConfig.h"
#ifdef PXS_ENABLE_FLUIDS

#include "PxcArray.h"
#include "PxcGridCellVector.h"

class PxsFluidParticleArray;
struct PxsFluidParticle;

// Use hardware hash function (Note: Hardware hash function is much slower)
#define PXS_FLUID_USE_HW_HASH_FUNCTION	0

// Packet hash entry flags
#define PXS_FLUID_PACKET_INVALID		0xffff	// Marks empty entry

// Cell hash entry flags
#define PXS_FLUID_CELL_INVALID			PXS_FLUID_PACKET_INVALID	// Marks empty entry


// Structure describing a fluid packet hash entry.
struct PxsFluidPacket
{
	PxcGridCellVector coords;	//! The packet coordinates
	PxU16 numParticles;			//! Number of particles in the packet
	PxU16 firstParticle;		//! Start index of the associated particle interval 
};

// Structure describing a fluid cell hash entry. Same structure as a packet for now.
typedef PxsFluidPacket PxsFluidCell;


// Structure describing boundary section (plus inner section) of a fluid packet.
// This will be used for halo optimization, i.e., to reduce the number of particles
// that have to be tested in neighboring packets.
#define PXS_FLUID_PACKET_SECTIONS 27
struct PxsFluidPacketSections
{
	PxU16 numParticles[PXS_FLUID_PACKET_SECTIONS];		//! Number of particles in each packet section
	PxU16 firstParticle[PXS_FLUID_PACKET_SECTIONS];		//! Start index of the associated particle interval for each packet section
};

// Structure describing the regions around a packet which are relevant for particle interactions.
// A packet has 26 neighbor packets:
// - 6 of these neighbors share a face with the packet. Each of these neighbors provide 9 halo regions.
// - 12 of these neighbors share an edge with the packet. Each of these neighbors provides 3 halo region.
// - 8 of these neighbors share a corner with the packet. Each of these neighbors provide 1 halo region.
//
// -> Number of halo regions for a packet:  6*9 + 12*3 + 8*1 = 98
#define PXS_FLUID_PACKET_HALO_REGIONS 98
struct PxsFluidPacketHaloRegions
{
	PxU16 numParticles[PXS_FLUID_PACKET_HALO_REGIONS];	//! Number of particles in each halo region
	PxU16 firstParticle[PXS_FLUID_PACKET_HALO_REGIONS];	//! Start index of the associated particle interval for each halo region
	PxU16 maxNumParticles;								//! Maximum number of particles among all halo regions
};


class PxsFluidSpatialHash
{
public:

	PxsFluidSpatialHash(PxU16 particlesMax, PxdF32 cellSizeInv, PxU32 packetMultLog);
	~PxsFluidSpatialHash();

	void destroy();

	//
	// Hash functions related stuff
	//
	static PXD_FORCE_INLINE void ccittcrc(PxU32 *crc, PxU8 ser_data);
	static PXD_FORCE_INLINE PxU16 crc(PxU32 a, PxU32 b);
	static PXD_FORCE_INLINE PxU16 hashFunction(const PxcGridCellVector& packetCoord, PxU32 numHashBuckets);

#ifdef __CELLOS_LV2__
	PXD_FORCE_INLINE PxdF32 getCellSizeInv() { return mCellSizeInv; }
#endif
	PXD_FORCE_INLINE PxU32 getPacketMultLog() { return mPacketMultLog; }

	PXD_FORCE_INLINE const PxsFluidPacket* getPackets() { return mPackets; }
	PXD_FORCE_INLINE const PxsFluidPacketSections* getPacketSections() { return mPacketSections; }

	PXD_FORCE_INLINE		const PxsFluidPacket*	findPacket(PxU16& packetIndex, const PxcGridCellVector& coord);
	static PXD_FORCE_INLINE	const PxsFluidPacket*	findPacket(PxU16& packetIndex, const PxsFluidPacket* packets, const PxcGridCellVector& coord);
	static PXD_FORCE_INLINE const PxsFluidCell*		findCell(const PxcGridCellVector& coord, const PxsFluidCell* cells, PxU32 numHashBuckets);

	/*!
	Given the coordinates of a specific packet, the packet table and the packet sections, this function builds
	the halo region structure for the packet. The halo region specifies the relevant particles of neighboring
	packets.
	*/
	static void getHaloRegions(	PxsFluidPacketHaloRegions& packetHalo, const PxcGridCellVector& packetCoords,
								const PxsFluidPacket* packets, const PxsFluidPacketSections* packetSections);

	/*!
	Build local hash table for cells within a packet. Reorders a particle index array according to particle cells.

	The cell entry array must have more entries than the number of particles passed. The particle index
	table must have the size of the number of particles passed. The particle array is not declared const 
	because hash keys might get stored temporarily in the particles.
	*/
	static void buildLocalHash(	PxsFluidParticle* particles, PxU32 numParticles, PxsFluidCell* cells,
								PxU16* particleIndices, PxU32 numHashBuckets, bool storeHashKeyInParticle);

	/*!
	Builds the packet hash and reorders particles according to packets. Input particles are not declared const since
	each particle hash key gets precomputet.

	Returns number of deleted particles if the maximum number of packets was reached.
	*/
	void updatePacketHash(	PxsFluidParticleArray& outParticles, PxsFluidParticleArray& inParticles,
							PxcArray<PxU16>& deletedParticleIds);

	/*!
	Divides each fluid packet into sections and reorders particles according to sections.
	Input particles are not declared const since for each particle the section index gets precomputet.
	*/
	void updatePacketSections(PxsFluidParticleArray& outParticles, PxsFluidParticleArray& inParticles);

private:

	void reorderParticlesToPackets(PxsFluidParticleArray& outParticles, PxsFluidParticleArray& inParticles);

	static void reorderParticleIndicesToCells(	const PxsFluidParticle* particles, PxU32 numParticles, PxsFluidCell* cells,
												PxU16* particleIndices, PxU32 numHashBuckets, bool hashKeyStoredInParticle);

	/*!
	Splits the specified packet into 26 boundary sections (plus one inner section) and reorders the particles
	according to sections.
	*/
	void buildPacketSections(	const PxsFluidPacket& packet, PxsFluidPacketSections& sections,
								PxsFluidParticleArray& outParticles, PxsFluidParticleArray& inParticles,
								PxU32 packetMultLog);

	void reorderParticlesToPacketSections(	const PxsFluidPacket& packet, PxsFluidPacketSections& sections,
											PxsFluidParticleArray& outParticles, PxsFluidParticleArray& inParticles);

	PxU32 getPacketSectionIndex(const PxcGridCellVector& cellCoords, const PxcGridCellVector& packetMinCellCoords,
								PxU32 packetMult);


	PXD_FORCE_INLINE		PxsFluidPacket*			getPacket(PxU16& packetIndex, const PxcGridCellVector& coord);
	static PXD_FORCE_INLINE	PxsFluidPacket*			getPacket(PxU16& packetIndex, PxsFluidPacket* packets, const PxcGridCellVector& coord);

	static PXD_FORCE_INLINE	PxsFluidCell*			getCell(PxU16& cellIndex, const PxcGridCellVector& coord, PxsFluidCell* cells, PxU32 numHashBuckets);


private:
						PxdF32					mCellSizeInv;

						//Packet Hash data
						PxU32					mPacketMultLog;
						PxsFluidPacket*			mPackets;
						PxsFluidPacketSections*	mPacketSections;
};


PXD_FORCE_INLINE PxsFluidPacket* PxsFluidSpatialHash::getPacket(PxU16& packetIndex, PxsFluidPacket* packets,
																const PxcGridCellVector& coord)
{
	PxsFluidPacket *packet;
	PxU32 tries = 0;
	PxU16 key = hashFunction(coord, PXD_FLUID_PACKET_HASH_SIZE);
	packet = &packets[key];

	while ( (packet->numParticles != PXS_FLUID_PACKET_INVALID) && (coord != packet->coords) )
	{
		key = (key+1) & (PXD_FLUID_PACKET_HASH_SIZE-1);
		packet = &packets[key];

		PXN_ASSERT(tries++ < PXD_FLUID_PACKET_HASH_SIZE);
	}

	packetIndex = key;
	return(packet);
}

PXD_FORCE_INLINE PxsFluidPacket* PxsFluidSpatialHash::getPacket(PxU16& packetIndex, const PxcGridCellVector& coord)
{
	return getPacket(packetIndex, mPackets, coord);
}

PXD_FORCE_INLINE const PxsFluidPacket* PxsFluidSpatialHash::findPacket(PxU16& packetIndex, const PxcGridCellVector& coord)
{
	return findPacket(packetIndex, mPackets, coord);
}

PXD_FORCE_INLINE const PxsFluidPacket* PxsFluidSpatialHash::findPacket(PxU16& packetIndex, const PxsFluidPacket* packets,
																	   const PxcGridCellVector& coord)
{
	const PxsFluidPacket* packet = getPacket(packetIndex, const_cast<PxsFluidPacket*>(packets), coord);

	if(packet->numParticles == PXS_FLUID_PACKET_INVALID)
		return NULL;
	else
		return packet;
}

// MS: This is basically the same as getPacket(...). Might consider generating a base class for hash entries
//     based on vector coordinates and then this could be unified.
PXD_FORCE_INLINE PxsFluidCell* PxsFluidSpatialHash::getCell(PxU16& cellIndex, const PxcGridCellVector& coord,
															PxsFluidCell* cells, PxU32 numHashBuckets)
{
	PxsFluidCell *cell;
	PxU32 tries = 0;
	PxU16 key = hashFunction(coord, numHashBuckets);
	cell = &cells[key];

	while ( (cell->numParticles != PXS_FLUID_CELL_INVALID) && (coord != cell->coords) )
	{
		key = (key+1) & (numHashBuckets-1);
		cell = &cells[key];

		PXN_ASSERT(tries++ < numHashBuckets);
	}

	cellIndex = key;
	return(cell);
}

PXD_FORCE_INLINE const PxsFluidCell* PxsFluidSpatialHash::findCell(const PxcGridCellVector& coord, const PxsFluidCell* cells,
																   PxU32 numHashBuckets)
{
	PxU16 cellIndex;
	const PxsFluidCell* cell = getCell(cellIndex, coord, const_cast<PxsFluidCell*>(cells), numHashBuckets);

	if(cell->numParticles == PXS_FLUID_CELL_INVALID)
		return NULL;
	else
		return cell;
}

PXD_FORCE_INLINE void PxsFluidSpatialHash::ccittcrc(PxU32 *crc, PxU8 ser_data)
{
    PxUInt i;
    PxU32 uch = (PxU32) ser_data;

    *crc ^= (uch << 8);

    for (i=0; i<8; i++)
    {
        if (*crc & 0x8000L)
            *crc = (*crc << 1) ^ 0x1021L;
        else
            *crc <<= 1;

        *crc &= 0xffffL;
    }
}

// Stateless crc-ccitt on a 64-bit input (a is high, b is low)
PXD_FORCE_INLINE PxU16 PxsFluidSpatialHash::crc(PxU32 a, PxU32 b)
{
    PxU32 result = 0;

    ccittcrc( &result, (PxU8)(a>>24) & 0xFF );
    ccittcrc( &result, (PxU8)(a>>16) & 0xFF );
    ccittcrc( &result, (PxU8)(a>>8) & 0xFF );
    ccittcrc( &result, (PxU8)(a>>0) & 0xFF );
    
    ccittcrc( &result, (PxU8)(b>>24) & 0xFF );
    ccittcrc( &result, (PxU8)(b>>16) & 0xFF );
    ccittcrc( &result, (PxU8)(b>>8) & 0xFF );
    ccittcrc( &result, (PxU8)(b>>0) & 0xFF );

    return (PxU16)result;
}

PXD_FORCE_INLINE PxU16 PxsFluidSpatialHash::hashFunction(const PxcGridCellVector& coord, PxU32 numHashBuckets)
{
#if PXS_FLUID_USE_HW_HASH_FUNCTION
	PxU32 result;
    PxU32 a, b;
	PxU16 ux;
	PxU16 uy;
	PxU16 uz;

    // Make sure we don't have more than 64K buckets because we're using
    // a CRC 16 for the hash.  Also, make shure that the number of
    // buckets is a power of two!

    PXN_ASSERT(numHashBuckets <= 0x10000);
    PXN_ASSERT((((numHashBuckets-1)^numHashBuckets)+1) == (2*numHashBuckets));

	ux = *(PxU16 *)&coord.x;
	uy = *(PxU16 *)&coord.y;
	uz = *(PxU16 *)&coord.z;

    a = ux | (uy << 16);
    b = uz;

    result = crc(a, b) & (numHashBuckets - 1);
    return result;
#else
	PXN_ASSERT((((numHashBuckets-1)^numHashBuckets)+1) == (2*numHashBuckets));
	return ( ((PxU16)(coord.x + 101*coord.y + 7919*coord.z)) & (numHashBuckets - 1) );
#endif	// PXS_FLUID_USE_HW_HASH_FUNCTION
}


#endif	// PXS_ENABLE_FLUIDS

#endif
