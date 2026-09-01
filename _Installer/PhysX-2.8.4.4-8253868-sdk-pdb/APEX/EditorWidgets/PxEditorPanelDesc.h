
#ifndef PX_EDITOR_PANEL_DESC_H
#define PX_EDITOR_PANEL_DESC_H

#include <Px.h>
#include <PxSimpleTypes.h>

#if defined(PX_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <windows.h> // for HWND and IsWindow()
#endif

namespace physx
{
	
	class PxEditorPanelDesc
	{
		public:
			PxEditorPanelDesc(void)
			{
			#if defined(PX_WINDOWS)
				parentWindow = 0;
			#endif
				GameScale = 1.0f; // the default game scale for units
			}
			
			virtual bool isValid(void) const
			{
				bool ok = true;
			#if defined(PX_WINDOWS)
				if(!parentWindow || !IsWindow(parentWindow)) ok = false;
			#endif
				return ok;
			}
			
		public:
		#if defined(PX_WINDOWS)
			//! REQUIRED: parent window or panel place the widgets in.
			HWND parentWindow;
		#endif
			physx::pubfnd2::PxF32	GameScale;
	};
	
} // namespace physx

#endif
