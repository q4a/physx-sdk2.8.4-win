
#ifndef PX_GENERIC_PROPERTIES_EDITOR_CALLBACK_H
#define PX_GENERIC_PROPERTIES_EDITOR_CALLBACK_H

namespace NxParameterized
{
	class Handle;
}

namespace physx
{
	
	class PxGenericPropertiesEditorCallback
	{
		public:
			//! called just after a property has been editted by the user.
			virtual void onPostPropertyEdit(NxParameterized::Handle &handle,const char *tweakable,NxParameterized::Interface *parentInterface) = 0;
			
			//! query the application to find a named reference for us.
			virtual const char *onFindNamedReference(const char *variant) = 0;
	};
	
} // namespace physx

#endif
