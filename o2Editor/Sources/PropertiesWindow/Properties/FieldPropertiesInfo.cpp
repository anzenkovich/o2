#include "FieldPropertiesInfo.h"

#include "PropertiesWindow/Properties/IPropertyField.h"
#include "Utils/IObject.h"

namespace Editor
{
	void FieldPropertiesInfo::Set(const Vector<IObject*>& targets)
	{
		if (targets.IsEmpty())
			return;

		auto fields = targets[0]->GetType().Fields();
		for (auto fieldInfo : fields)
		{
			Vector<void*> fieldPointers = targets.Select<void*>(
				[&](IObject* x) { return fieldInfo->GetValuePtrStrong<char>(x); });

			IPropertyField* propertyField;
			if (properties.TryGetValue(fieldInfo, propertyField))
				propertyField->Setup(fieldPointers, fieldInfo->IsProperty());
		}
	}
}