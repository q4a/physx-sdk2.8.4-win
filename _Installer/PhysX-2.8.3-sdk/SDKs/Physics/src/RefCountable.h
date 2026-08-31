#ifndef NX_REFCOUNTABLE
#define NX_REFCOUNTABLE

#include "Nxp.h"


class RefCountable
	{
	public:
							RefCountable()	: numRefs(0)	{					}
	NX_INLINE	void		incRef()						{ numRefs++;		}
	NX_INLINE	void		decRef()						{ numRefs--;		}
	NX_INLINE	NxU32		getNumRefs() 			const	{ return numRefs;	}

	private:
				NxU32		numRefs;
	};

#endif

