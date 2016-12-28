#include "Vector2FloatProperty.h"

#include "SceneWindow/SceneEditScreen.h"
#include "UI/EditBox.h"
#include "UI/HorizontalLayout.h"
#include "UI/UIManager.h"

namespace Editor
{
	Vec2FProperty::Vec2FProperty(UIWidget* widget /*= nullptr*/)
	{
		if (widget)
			mWidget = widget;
		else
			mWidget = o2UI.CreateWidget<UIWidget>("vector2 property");

		mXEditBox = dynamic_cast<UIEditBox*>(mWidget->GetChild("x edit"));
		mYEditBox = dynamic_cast<UIEditBox*>(mWidget->GetChild("y edit"));

		mXEditBox->onChangeCompleted = Function<void(const WString&)>(this, &Vec2FProperty::OnXEdited);
		mXEditBox->SetFilterFloat();

		mYEditBox->onChangeCompleted = Function<void(const WString&)>(this, &Vec2FProperty::OnYEdited);
		mYEditBox->SetFilterFloat();
	}

	Vec2FProperty::~Vec2FProperty()
	{
		delete mWidget;
	}

	void Vec2FProperty::SetValue(const Vec2F& value)
	{
		mXValuesDifferent = false;
		mYValuesDifferent = false;
		mCommonValue = value;
		mXEditBox->text = (WString)mCommonValue.x;
		mYEditBox->text = (WString)mCommonValue.y;

		o2EditorSceneScreen.OnSceneChanged();
	}

	void Vec2FProperty::SetUnknownValue()
	{
		mXValuesDifferent = true;
		mYValuesDifferent = true;
		mCommonValue = Vec2F();
		mXEditBox->text = "--";
		mYEditBox->text = "--";

		o2EditorSceneScreen.OnSceneChanged();
	}

	void Vec2FProperty::Setup(const Vector<void*>& targets, bool isProperty)
	{
		if (isProperty)
		{
			mAssignFunc = [](void* ptr, const Vec2F& value) { *((Property<Vec2F>*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return ((Property<Vec2F>*)(ptr))->Get(); };
		}
		else
		{
			mAssignFunc = [](void* ptr, const Vec2F& value) { *((Vec2F*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return *((Vec2F*)(ptr)); };
		}

		mValuesPointers = targets;

		Update();
	}

	void Vec2FProperty::Update()
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
			mCommonValue.x = 0.0f;
		}
		else mXEditBox->text = (WString)mCommonValue.x;

		if (mYValuesDifferent)
		{
			mYEditBox->text = "--";
			mCommonValue.y = 0.0f;
		}
		else mYEditBox->text = (WString)mCommonValue.y;
	}

	UIWidget* Vec2FProperty::GetWidget() const
	{
		return mWidget;
	}

	Vec2F Vec2FProperty::GetCommonValue() const
	{
		return mCommonValue;
	}

	bool Vec2FProperty::IsValuesDifferent() const
	{
		return mXValuesDifferent && mYValuesDifferent;
	}

	const Type* Vec2FProperty::GetFieldType() const
	{
		return &TypeOf(Vec2F);
	}

	void Vec2FProperty::OnXEdited(const WString& data)
	{
		if (mXValuesDifferent && data == "--")
			return;

		mCommonValue.x = (const float)data;
		mXValuesDifferent = false;

		for (auto ptr : mValuesPointers)
			mAssignFunc(ptr, mCommonValue);

		onChanged();

		o2EditorSceneScreen.OnSceneChanged();
	}

	void Vec2FProperty::OnYEdited(const WString& data)
	{
		if (mYValuesDifferent && data == "--")
			return;

		mCommonValue.y = (const float)data;
		mYValuesDifferent = false;

		for (auto ptr : mValuesPointers)
			mAssignFunc(ptr, mCommonValue);

		onChanged();

		o2EditorSceneScreen.OnSceneChanged();
	}
}
 
CLASS_META(Editor::Vec2FProperty)
{
	BASE_CLASS(Editor::IPropertyField);

	PROTECTED_FIELD(mAssignFunc);
	PROTECTED_FIELD(mGetFunc);
	PROTECTED_FIELD(mValuesPointers);
	PROTECTED_FIELD(mCommonValue);
	PROTECTED_FIELD(mXValuesDifferent);
	PROTECTED_FIELD(mYValuesDifferent);
	PROTECTED_FIELD(mWidget);
	PROTECTED_FIELD(mXEditBox);
	PROTECTED_FIELD(mYEditBox);

	PUBLIC_FUNCTION(void, Setup, const Vector<void*>&, bool);
	PUBLIC_FUNCTION(void, Update);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(void, SetValue, const Vec2F&);
	PUBLIC_FUNCTION(void, SetUnknownValue);
	PUBLIC_FUNCTION(Vec2F, GetCommonValue);
	PUBLIC_FUNCTION(bool, IsValuesDifferent);
	PUBLIC_FUNCTION(const Type*, GetFieldType);
	PROTECTED_FUNCTION(void, OnXEdited, const WString&);
	PROTECTED_FUNCTION(void, OnYEdited, const WString&);
}
END_META;
 