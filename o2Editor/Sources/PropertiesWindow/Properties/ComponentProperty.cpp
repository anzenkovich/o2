#include "ComponentProperty.h"

#include "Animation/AnimatedFloat.h"
#include "Animation/AnimatedVector.h"
#include "Animation/Animation.h"
#include "Assets/Assets.h"
#include "AssetsWindow/AssetsWindow.h"
#include "Render/Sprite.h"
#include "Scene/Actor.h"
#include "SceneWindow/SceneEditScreen.h"
#include "TreeWindow/TreeWindow.h"
#include "UI/Button.h"
#include "UI/UIManager.h"
#include "UI/Widget.h"

namespace Editor
{
	ComponentProperty::ComponentProperty(UIWidget* widget /*= nullptr*/)
	{
		if (!widget)
			widget = o2UI.CreateWidget<UIWidget>("componentProperty");

		mBox = widget;
		mNameText = widget->GetLayerDrawable<Text>("caption");

		SetEventHandleDrawable(mBox);
		mComponentType = &TypeOf(Component);
	}

	ComponentProperty::~ComponentProperty()
	{
		SetEventHandleDrawable(nullptr);
		delete mBox;
	}

	void ComponentProperty::Setup(const Vector<void*>& targets, bool isProperty)
	{
		if (isProperty)
		{
			mAssignFunc = [](void* ptr, Component* value) { *((Property<Component*>*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return ((Property<Component*>*)(ptr))->Get(); };
		}
		else
		{
			mAssignFunc = [](void* ptr, Component* value) { *((Component**)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return *((Component**)(ptr)); };
		}

		mValuesPointers = targets;

		Update();
	}

	void ComponentProperty::Update()
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
			mNameText->text = "--";
			mBox->layer["caption"]->transparency = 1.0f;
		}
		else
		{
			if (!mCommonValue)
			{
				mNameText->text = "Null:" + mComponentType->Name();
				mBox->layer["caption"]->transparency = 0.5f;
			}
			else
			{
				mNameText->text = mCommonValue->GetOwnerActor()->GetName();
				mBox->layer["caption"]->transparency = 1.0f;
			}
		}
	}

	UIWidget* ComponentProperty::GetWidget() const
	{
		return mBox;
	}

	Component* ComponentProperty::GetCommonValue() const
	{
		return mCommonValue;
	}

	bool ComponentProperty::IsValuesDifferent() const
	{
		return mValuesDifferent;
	}

	const Type* ComponentProperty::GetFieldType() const
	{
		return &TypeOf(Component*);
	}

	void ComponentProperty::SpecializeType(const Type* type)
	{
		mComponentType = type;
	}

	const Type* ComponentProperty::GetSpecializedType() const
	{
		return mComponentType;
	}

	void ComponentProperty::SetValue(Component* value)
	{
		mCommonValue = value;
		mValuesDifferent = false;

		for (auto ptr : mValuesPointers)
			mAssignFunc(ptr, mCommonValue);

		onChanged();

		Update();
	}

	void ComponentProperty::SetUnknownValue()
	{
		mCommonValue = nullptr;
		mValuesDifferent = true;
		mNameText->text = "--";
		mBox->layer["caption"]->transparency = 1.0f;
	}

	void ComponentProperty::OnCursorEnter(const Input::Cursor& cursor)
	{
		mBox->SetState("select", true);
	}

	void ComponentProperty::OnCursorExit(const Input::Cursor& cursor)
	{
		mBox->SetState("select", false);
	}

	void ComponentProperty::OnCursorPressed(const Input::Cursor& cursor)
	{
		o2UI.FocusWidget(mBox);

		if (mCommonValue)
		{
			auto ownerActor = mCommonValue->GetOwnerActor();

			if (ownerActor)
			{
				if (ownerActor->IsAsset())
				{
					o2EditorAssets.ShowAssetIcon(o2Assets.GetAssetPath(ownerActor->GetAssetId()));
				}
				else if (ownerActor->IsOnScene())
				{
					o2EditorTree.HightlightActorsTreeNode(ownerActor);
					o2EditorSceneScreen.SelectActor(ownerActor, false);
				}
			}
		}
	}

	void ComponentProperty::OnKeyPressed(const Input::Key& key)
	{
		if (mBox->IsFocused() && key == VK_DELETE || key == VK_BACK)
			SetValue(nullptr);
	}

}
 
CLASS_META(Editor::ComponentProperty)
{
	BASE_CLASS(Editor::IPropertyField);
	BASE_CLASS(o2::DrawableCursorEventsListener);
	BASE_CLASS(o2::KeyboardEventsListener);

	PROTECTED_FIELD(mAssignFunc);
	PROTECTED_FIELD(mGetFunc);
	PROTECTED_FIELD(mValuesPointers);
	PROTECTED_FIELD(mCommonValue);
	PROTECTED_FIELD(mValuesDifferent);
	PROTECTED_FIELD(mComponentType);
	PROTECTED_FIELD(mBox);
	PROTECTED_FIELD(mNameText);

	PUBLIC_FUNCTION(void, Setup, const Vector<void*>&, bool);
	PUBLIC_FUNCTION(void, Update);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(Component*, GetCommonValue);
	PUBLIC_FUNCTION(bool, IsValuesDifferent);
	PUBLIC_FUNCTION(const Type*, GetFieldType);
	PUBLIC_FUNCTION(void, SpecializeType, const Type*);
	PUBLIC_FUNCTION(const Type*, GetSpecializedType);
	PUBLIC_FUNCTION(void, SetValue, Component*);
	PUBLIC_FUNCTION(void, SetUnknownValue);
	PROTECTED_FUNCTION(void, OnCursorEnter, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorExit, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorPressed, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnKeyPressed, const Input::Key&);
}
END_META;
 