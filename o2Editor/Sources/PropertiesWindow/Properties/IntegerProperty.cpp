#include "IntegerProperty.h"

#include "SceneWindow/SceneEditScreen.h"
#include "UI/EditBox.h"
#include "UI/UIManager.h"

namespace Editor
{
	IntegerProperty::IntegerProperty(UIWidget* widget /*= nullptr*/)
	{
		if (widget)
			mEditBox = dynamic_cast<UIEditBox*>(widget);
		else
			mEditBox = o2UI.CreateWidget<UIEditBox>("singleline");

		mEditBox->layout.minHeight = 10;
		mEditBox->onChangeCompleted = Function<void(const WString&)>(this, &IntegerProperty::OnEdited);
		mEditBox->SetFilterInteger();
	}

	IntegerProperty::~IntegerProperty()
	{
		delete mEditBox;
	}

	void IntegerProperty::SetValue(int value)
	{
		mValuesDifferent = false;
		mCommonValue = value;
		mEditBox->text = (WString)mCommonValue;

		o2EditorSceneScreen.OnSceneChanged();
	}

	void IntegerProperty::SetUnknownValue()
	{
		mValuesDifferent = true;
		mCommonValue = 0;
		mEditBox->text = "--";

		o2EditorSceneScreen.OnSceneChanged();
	}

	void IntegerProperty::Setup(const Vector<void*>& targets, bool isProperty)
	{
		if (isProperty)
		{
			mAssignFunc = [](void* ptr, const int& value) { *((Property<int>*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return ((Property<int>*)(ptr))->Get(); };
		}
		else
		{
			mAssignFunc = [](void* ptr, const int& value) { *((int*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return *((int*)(ptr)); };
		}

		mValuesPointers = targets;

		Update();
	}

	void IntegerProperty::Update()
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
			mCommonValue = 0;
		}
		else mEditBox->text = (WString)mCommonValue;
	}

	UIWidget* IntegerProperty::GetWidget() const
	{
		return mEditBox;
	}

	int IntegerProperty::GetCommonValue() const
	{
		return mCommonValue;
	}

	bool IntegerProperty::IsValuesDifferent() const
	{
		return mValuesDifferent;
	}

	const Type* IntegerProperty::GetFieldType() const
	{
		return &TypeOf(int);
	}

	void IntegerProperty::OnEdited(const WString& data)
	{
		if (mValuesDifferent && data == "--")
			return;

		mCommonValue = (const int)data;
		mValuesDifferent = false;

		for (auto ptr : mValuesPointers)
			mAssignFunc(ptr, mCommonValue);

		onChanged();

		o2EditorSceneScreen.OnSceneChanged();
	}
}
 
CLASS_META(Editor::IntegerProperty)
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
	PUBLIC_FUNCTION(void, SetValue, int);
	PUBLIC_FUNCTION(void, SetUnknownValue);
	PUBLIC_FUNCTION(int, GetCommonValue);
	PUBLIC_FUNCTION(bool, IsValuesDifferent);
	PUBLIC_FUNCTION(const Type*, GetFieldType);
	PROTECTED_FUNCTION(void, OnEdited, const WString&);
}
END_META;
 