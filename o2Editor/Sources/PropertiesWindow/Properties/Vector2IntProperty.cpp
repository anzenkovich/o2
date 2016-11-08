#include "Vector2IntProperty.h"

#include "UI/EditBox.h"
#include "UI/HorizontalLayout.h"
#include "UI/UIManager.h"

namespace Editor
{
	Vec2IProperty::Vec2IProperty(UIWidget* widget /*= nullptr*/)
	{
		if (widget)
			mWidget = widget;
		else
			mWidget = o2UI.CreateWidget<UIWidget>("vector2 property");

		mXEditBox = dynamic_cast<UIEditBox*>(mWidget->GetChild("x edit"));
		mYEditBox = dynamic_cast<UIEditBox*>(mWidget->GetChild("y edit"));

		mXEditBox->onChangeCompleted = Function<void(const WString&)>(this, &Vec2IProperty::OnXEdited);
		mXEditBox->SetFilterInteger();

		mYEditBox->onChangeCompleted = Function<void(const WString&)>(this, &Vec2IProperty::OnYEdited);
		mYEditBox->SetFilterInteger();
	}

	Vec2IProperty::~Vec2IProperty()
	{
		delete mWidget;
	}

	void Vec2IProperty::SetValue(const Vec2I& value)
	{
		mXValuesDifferent = false;
		mYValuesDifferent = false;
		mCommonValue = value;
		mXEditBox->text = (WString)mCommonValue.x;
		mYEditBox->text = (WString)mCommonValue.y;
	}

	void Vec2IProperty::SetUnknownValue()
	{
		mXValuesDifferent = true;
		mYValuesDifferent = true;
		mCommonValue = Vec2I();
		mXEditBox->text = "--";
		mYEditBox->text = "--";
	}

	void Vec2IProperty::Setup(const Vector<void*>& targets, bool isProperty)
	{
		if (isProperty)
		{
			mAssignFunc = [](void* ptr, const Vec2I& value) { *((Property<Vec2I>*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return ((Property<Vec2I>*)(ptr))->Get(); };
		}
		else
		{
			mAssignFunc = [](void* ptr, const Vec2I& value) { *((Vec2I*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return *((Vec2I*)(ptr)); };
		}

		mValuesPointers = targets;

		Update();
	}

	void Vec2IProperty::Update()
	{
		if (mValuesPointers.IsEmpty())
			return;

		mCommonValue = mGetFunc(mValuesPointers[0]);
		mXValuesDifferent = false;
		mYValuesDifferent = false;

		for (int i = 1; i < mValuesPointers.Count(); i++)
		{
			auto value = mGetFunc(mValuesPointers[i]);
			if (!Math::Equals(mCommonValue.x, value.x))
			{
				mXValuesDifferent = true;
				break;
			}

			if (!Math::Equals(mCommonValue.y, value.y))
			{
				mYValuesDifferent = true;
				break;
			}
		}

		if (mXValuesDifferent)
		{
			mXEditBox->text = "--";
			mCommonValue.x = 0;
		}
		else mXEditBox->text = (WString)mCommonValue.x;

		if (mYValuesDifferent)
		{
			mYEditBox->text = "--";
			mCommonValue.y = 0;
		}
		else mYEditBox->text = (WString)mCommonValue.y;
	}

	UIWidget* Vec2IProperty::GetWidget() const
	{
		return mWidget;
	}

	Vec2I Vec2IProperty::GetCommonValue() const
	{
		return mCommonValue;
	}

	bool Vec2IProperty::IsValuesDifferent() const
	{
		return mXValuesDifferent && mYValuesDifferent;
	}

	const Type* Vec2IProperty::GetFieldType() const
	{
		return &TypeOf(Vec2I);
	}

	void Vec2IProperty::OnXEdited(const WString& data)
	{
		if (mXValuesDifferent && data == "--")
			return;

		mCommonValue.x = (const int)data;
		mXValuesDifferent = false;

		for (auto ptr : mValuesPointers)
			mAssignFunc(ptr, mCommonValue);

		onChanged();
	}

	void Vec2IProperty::OnYEdited(const WString& data)
	{
		if (mYValuesDifferent && data == "--")
			return;

		mCommonValue.y = (const int)data;
		mYValuesDifferent = false;

		for (auto ptr : mValuesPointers)
			mAssignFunc(ptr, mCommonValue);

		onChanged();
	}
}
 
CLASS_META(Editor::Vec2IProperty)
{
	BASE_CLASS(Editor::IPropertyField);

	PROTECTED_FIELD(mAssignFunc);
	PROTECTED_FIELD(mGetFunc);
	PROTECTED_FIELD(mValuesPointers);
	PROTECTED_FIELD(mCommonValue);
	PROTECTED_FIELD(mValuesDifferent);
	PROTECTED_FIELD(mXValuesDifferent);
	PROTECTED_FIELD(mYValuesDifferent);
	PROTECTED_FIELD(mWidget);
	PROTECTED_FIELD(mXEditBox);
	PROTECTED_FIELD(mYEditBox);

	PUBLIC_FUNCTION(void, Setup, const Vector<void*>&, bool);
	PUBLIC_FUNCTION(void, Update);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(void, SetValue, const Vec2I&);
	PUBLIC_FUNCTION(void, SetUnknownValue);
	PUBLIC_FUNCTION(Vec2I, GetCommonValue);
	PUBLIC_FUNCTION(bool, IsValuesDifferent);
	PUBLIC_FUNCTION(const Type*, GetFieldType);
	PROTECTED_FUNCTION(void, OnXEdited, const WString&);
	PROTECTED_FUNCTION(void, OnYEdited, const WString&);
}
END_META;
 