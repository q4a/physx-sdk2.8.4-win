#ifndef PX_HARDWARE_H
#define PX_HARDWARE_H

#include "px_config.h"

#ifdef PXD_SUPPORT_PPU

#ifdef __cplusplus
extern "C" 
{
#endif

#pragma pack(push,1)
#pragma pack(pop)


/*!
Purge a page from the PhysX card. This causes references to the page in question to be
removed from the PhysX card. Caching allows the page to remain on the card even when
it has been deleted until the space is needed for a fresh page, so that if the page
is mapped again, it does not need to be copied to the card. This means that unless
references to a page are purged when the mesh is deleted, such a page could alias with
a new page.

\param a pointer to the host side copy of the mesh page
*/

PXD_EXPORT_FUNCTION void PxdHardwarePurgeMeshPage(void* pageAddr);

#ifdef __cplusplus
}
#endif

#endif //PXD_SUPPORT_PPU

#endif
