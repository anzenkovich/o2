#include "IPropertyField.h"

#include "UI/Widget.h"
 
CLASS_META(Editor::IPropertyField)
{
	BASE_CLASS(o2::IObject);

	PUBLIC_FIELD(onChanged);

	PUBLIC_FUNCTION(void, Setup, const Vector<void*>&, bool);
	PUBLIC_FUNCTION(void, Update);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(const Type*, GetFieldType);
	PUBLIC_FUNCTION(void, SpecializeType, const Type*);
}
END_META;
 