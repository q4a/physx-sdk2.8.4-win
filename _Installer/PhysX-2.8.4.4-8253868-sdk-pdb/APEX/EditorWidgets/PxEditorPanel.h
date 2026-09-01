
#ifndef PX_EDITOR_PANEL_H
#define PX_EDITOR_PANEL_H

#include <Px.h>

namespace physx
{
	
	class PxEditorPanel
	{
		protected:
			virtual ~PxEditorPanel(void) {}
			
		public:
			//! call this any time the parent window gets resized...
			virtual void onResize(void) = 0;
			
			//! releases this object.
			virtual void release(void) = 0;
	};
	
} // namespace physx

#endif
