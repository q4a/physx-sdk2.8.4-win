
#ifndef PX_GENERIC_PROPERTIES_EDITOR_PANEL_DESC_H
#define PX_GENERIC_PROPERTIES_EDITOR_PANEL_DESC_H

#include <PxEditorPanelDesc.h>

namespace physx
{
	
	class PxGenericPropertiesEditorCallback;
	
	class PxGenericPropertiesEditorPanelDesc : public PxEditorPanelDesc
	{
		public:
			PxGenericPropertiesEditorPanelDesc(void)
			{
				callback = 0;
			}
			
			virtual bool isValid(void) const
			{
				bool ok = PxEditorPanelDesc::isValid();
				if(!callback) ok = false;
				return ok;
			}
			
		public:
			PxGenericPropertiesEditorCallback *callback;
	};
	
} // namespace physx

#endif
