
#ifndef PX_GENERIC_PROPERTIES_EDITOR_PANEL_H
#define PX_GENERIC_PROPERTIES_EDITOR_PANEL_H

#include <PxEditorPanel.h>

namespace NxParameterized
{
	class Interface;
}

namespace physx
{
	
	class PxCurveEditorPanel;
	
	class PxGenericPropertiesEditorPanel : public PxEditorPanel
	{
		protected:
			virtual ~PxGenericPropertiesEditorPanel(void) {}
			
		public:
			//! appends an object to the property editor.
			virtual void addObject(const NxParameterized::Interface &object) = 0;
			
			//! removes an object from the property editor.
			virtual void removeObject(const NxParameterized::Interface &object) = 0;
			
			//! clears all properties from the editor.
			virtual void clearObjects(void) = 0;
			
			//! sets of clears the curve editor to use when selecting curve properties.
			virtual void setCurveEditor(PxCurveEditorPanel *curveEditorPanel) = 0;
	};
	
} // namespace physx

#endif
