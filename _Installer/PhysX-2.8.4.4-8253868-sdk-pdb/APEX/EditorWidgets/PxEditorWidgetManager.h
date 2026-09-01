
#ifndef PX_EDITOR_WIDGET_MANAGER_H
#define PX_EDITOR_WIDGET_MANAGER_H

#include <Px.h>

namespace physx
{
	
	class PxEditorPanelDesc;
	class PxGenericPropertiesEditorPanel;
	class PxGenericPropertiesEditorPanelDesc;
	class PxCurveEditorPanel;
	
	class PxEditorWidgetManager
	{
		protected:
			virtual ~PxEditorWidgetManager(void) {}
			
		public:
			//! release this object.
			virtual void release(void) = 0;
			
			//! call this somewhat frequently as its used for executing cleanup code.
			virtual void update(void) = 0;
			
			//! creates a widget inside a panel that is capable of editing any type of NxParameterized object.
			virtual PxGenericPropertiesEditorPanel *createGenericPropertiesEditor(const PxGenericPropertiesEditorPanelDesc &desc) = 0;
			
			//! creates a curve editor widget.
			virtual PxCurveEditorPanel             *createCurveEditor(const PxEditorPanelDesc &desc) = 0;
	};
	
} // namespace physx


#if defined(PX_VC)
	#if defined(PX_EDITOR_EXPORT_DLL)
		#define PX_EDITOR_DLL_API __declspec(dllexport)
	#else
		#define PX_EDITOR_DLL_API __declspec(dllimport)
	#endif
#endif

typedef physx::PxEditorWidgetManager *(*PxCreateEditorWidgetManagerPROC)(void);
PX_C_EXPORT PX_EDITOR_DLL_API physx::PxEditorWidgetManager *PxCreateEditorWidgetManager(void);

#endif
