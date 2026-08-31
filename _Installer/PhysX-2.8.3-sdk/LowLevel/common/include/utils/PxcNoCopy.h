#ifndef PXC_NOCOPY_H
#define PXC_NOCOPY_H

namespace PxcNoCopy_ //prevent unintentional dependent type-lookups
{
	class NoCopy
	{
	protected:
		NoCopy()
		{
		}

		~NoCopy()
		{
		}
	private: //make private and really hide
		NoCopy(const NoCopy& c);
		const NoCopy& operator=(const NoCopy& c);
	};
}

/*!
Non-copyable object semantics. 
Derive your class from this one to make it non-copyable.
*/
typedef PxcNoCopy_::NoCopy PxcNoCopy; 

#endif
