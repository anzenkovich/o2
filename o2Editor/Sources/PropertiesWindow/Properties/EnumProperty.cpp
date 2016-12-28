#include "EnumProperty.h"

#include "Animation/AnimatedFloat.h"
#include "Animation/AnimatedVector.h"
#include "Animation/Animation.h"
#include "Render/Sprite.h"
#include "SceneWindow/SceneEditScreen.h"
#include "UI/DropDown.h"
#include "UI/UIManager.h"

namespace Editor
{
	EnumProperty::EnumProperty(UIWidget* widget /*= nullptr*/)
	{
		if (widget)
			mDropDown = dynamic_cast<UIDropDown*>(widget);
		else
			mDropDown = o2UI.CreateWidget<UIDropDown>("editor property");

		mDropDown->onSelectedText = Function<void(const WString&)>(this, &EnumProperty::OnSelectedItem);
	}

	EnumProperty::~EnumProperty()
	{
		delete mDropDown;
	}

	void EnumProperty::Setup(const Vector<void*>& targets, bool isProperty)
	{
		if (isProperty)
		{
			mAssignFunc = [](void* ptr, int value) { *((Property<int>*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return ((Property<int>*)(ptr))->Get(); };
		}
		else
		{
			mAssignFunc = [](void* ptr, int value) { *((int*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return *((int*)(ptr)); };
		}

		mValuesPointers = targets;

		Update();
	}

	void EnumProperty::Update()
	{
		if (mValuesPointers.IsEmpty())
			return;

		mCommonValue = mGetFunc(mValuesPointers[0]);
		mValuesDifferent = false;

		for (int i = 1; i < mValuesPointers.Count(); i++)
		{
			if (mCommonValue != mGetFunc(mValuesPointers[i]))
			{
				mValuesDifferent = true;
				break;
			}
		}

		UpdateDropDownValue();
	}

	void EnumProperty::UpdateDropDownValue()
	{
		mUpdatingValue = true;

		if (mValuesDifferent)
		{
			mDropDown->SelectItemAt(-1);
			mDropDown->SetState("undefined", true);
		}
		else
		{
			mDropDown->value = (*mEntries)[mCommonValue];
			mDropDown->SetState("undefined", false);
		}

		mUpdatingValue = false;
	}

	UIWidget* EnumProperty::GetWidget() const
	{
		return mDropDown;
	}

	int EnumProperty::GetCommonValue() const
	{
		return mCommonValue;
	}

	bool EnumProperty::IsValuesDifferent() const
	{
		return mValuesDifferent;
	}

	void EnumProperty::SetValue(int value)
	{
		mCommonValue = value;
		mValuesDifferent = false;

		UpdateDropDownValue();

		o2EditorSceneScreen.OnSceneChanged();
	}

	void EnumProperty::SetUnknownValue()
	{
		mCommonValue = 0;
		mValuesDifferent = true;

		UpdateDropDownValue();

		o2EditorSceneScreen.OnSceneChanged();
	}

	const Type* EnumProperty::GetFieldType() const
	{
		return &TypeOf(void);
	}

	void EnumProperty::SpecializeType(const Type* type)
	{
		if (type->GetUsage() == Type::Usage::Property)
			mEnumType = dynamic_cast<const EnumType*>(((const PropertyType*)type)->GetValueType());
		else
			mEnumType = dynamic_cast<const EnumType*>(type);

		if (mEnumType)
		{
			mEntries = &mEnumType->GetEntries();

			for (auto kv : *mEntries)
				mDropDown->AddItem(kv.Value());
		}
	}

	void EnumProperty::OnSelectedItem(const WString& name)
	{
		if (mUpdatingValue)
			return;

		mCommonValue = mEntries->FindValue(name).Key();
		mValuesDifferent = false;

		for (auto ptr : mValuesPointers)
			mAssignFunc(ptr, mCommonValue);

		onChanged();

		Update();

		o2EditorSceneScreen.OnSceneChanged();
	}
}
 
CLASS_META(Editor::EnumProperty)
{
	BASE_CLASS(Editor::IPropertyField);

	PROTECTED_FIELD(mAssignFunc);
	PROTECTED_FIELD(mGetFunc);
	PROTECTED_FIELD(mEnumType);
	PROTECTED_FIELD(mValuesPointers);
	PROTECTED_FIELD(mCommonValue);
	PROTECTED_FIELD(mValuesDifferent);
	PROTECTED_FIELD(mEntries);
	PROTECTED_FIELD(mDropDown);
	PROTECTED_FIELD(mUpdatingValue);

	PUBLIC_FUNCTION(void, Setup, const Vector<void*>&, bool);
	PUBLIC_FUNCTION(void, Update);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(int, GetCommonValue);
	PUBLIC_FUNCTION(bool, IsValuesDifferent);
	PUBLIC_FUNCTION(void, SetValue, int);
	PUBLIC_FUNCTION(void, SetUnknownValue);
	PUBLIC_FUNCTION(const Type*, GetFieldType);
	PUBLIC_FUNCTION(void, SpecializeType, const Type*);
	PROTECTED_FUNCTION(void, OnSelectedItem, const WString&);
	PROTECTED_FUNCTION(void, UpdateDropDownValue);
}
END_META;
 