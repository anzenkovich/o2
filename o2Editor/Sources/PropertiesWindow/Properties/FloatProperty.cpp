#include "FloatProperty.h"

#include "SceneWindow/SceneEditScreen.h"
#include "UI/EditBox.h"
#include "UI/UIManager.h"

namespace Editor
{
	FloatProperty::FloatProperty(UIWidget* widget /*= nullptr*/)
	{
		if (widget)
			mEditBox = dynamic_cast<UIEditBox*>(widget);
		else
			mEditBox = o2UI.CreateWidget<UIEditBox>("singleline");

		mEditBox->layout.minHeight = 10;
		mEditBox->onChangeCompleted = Function<void(const WString&)>(this, &FloatProperty::OnEdited);
		mEditBox->SetFilterFloat();
	}

	FloatProperty::~FloatProperty()
	{
		delete mEditBox;
	}

	void FloatProperty::Setup(const Vector<void*>& targets, bool isProperty)
	{
		if (isProperty)
		{
			mAssignFunc = [](void* ptr, const float& value) { *((Property<float>*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return ((Property<float>*)(ptr))->Get(); };
		}
		else
		{
			mAssignFunc = [](void* ptr, const float& value) { *((float*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return *((float*)(ptr)); };
		}

		mValuesPointers = targets;

		Update();
	}

	void FloatProperty::Update()
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

		if (mValuesDifferent)
		{
			mEditBox->text = "--";
			mCommonValue = 0.0f;
		}
		else mEditBox->text = (WString)mCommonValue;
	}

	UIWidget* FloatProperty::GetWidget() const
	{
		return mEditBox;
	}

	float FloatProperty::GetCommonValue() const
	{
		return mCommonValue;
	}

	bool FloatProperty::IsValuesDifferent() const
	{
		return mValuesDifferent;
	}

	void FloatProperty::SetValue(int value)
	{
		mCommonValue = value;
		mValuesDifferent = false;
		mEditBox->text = (WString)mCommonValue;
	}

	void FloatProperty::SetUnknownValue()
	{
		mCommonValue = 0.0f;
		mValuesDifferent = true;
		mEditBox->text = "--";

		o2EditorSceneScreen.OnSceneChanged();
	}

	const Type* FloatProperty::GetFieldType() const
	{
		return &TypeOf(float);
	}

	void FloatProperty::OnEdited(const WString& data)
	{
		if (mValuesDifferent && data == "--")
			return;

		mCommonValue = (const float)data;
		mValuesDifferent = false;

		for (auto ptr : mValuesPointers)
			mAssignFunc(ptr, mCommonValue);

		onChanged();

		o2EditorSceneScreen.OnSceneChanged();
	}
}
 
CLASS_META(Editor::FloatProperty)
{
	BASE_CLASS(Editor::IPropertyField);

	PROTECTED_FIELD(mAssignFunc);
	PROTECTED_FIELD(mGetFunc);
	PROTECTED_FIELD(mValuesPointers);
	PROTECTED_FIELD(mCommonValue);
	PROTECTED_FIELD(mValuesDifferent);
	PROTECTED_FIELD(mEditBox);

	PUBLIC_FUNCTION(void, Setup, const Vector<void*>&, bool);
	PUBLIC_FUNCTION(void, Update);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(float, GetCommonValue);
	PUBLIC_FUNCTION(bool, IsValuesDifferent);
	PUBLIC_FUNCTION(void, SetValue, int);
	PUBLIC_FUNCTION(void, SetUnknownValue);
	PUBLIC_FUNCTION(const Type*, GetFieldType);
	PROTECTED_FUNCTION(void, OnEdited, const WString&);
}
END_META;
 