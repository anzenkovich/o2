#pragma once

#include "Animation/AnimatedFloat.h"
#include "Animation/AnimatedVector.h"
#include "Animation/Animation.h"
#include "Assets/Asset.h"
#include "Assets/Assets.h"
#include "AssetsWindow/AssetsWindow.h"
#include "Events/DrawableCursorEventsListener.h"
#include "Events/KeyboardEventsListener.h"
#include "PropertiesWindow/Properties/IPropertyField.h"
#include "Render/Sprite.h"
#include "Render/Text.h"
#include "UI/Button.h"
#include "UI/UIManager.h"
#include "UI/Widget.h"
#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Property.h"

using namespace o2;

namespace Editor
{
	// ------------------------------
	// Asset property field interface
	// ------------------------------
	class IAssetProperty: public IPropertyField, public DrawableCursorEventsListener, public KeyboardEventsListener
	{
	public:
		// Sets value asset id
		virtual void SetAssetId(UID id) {}

		IOBJECT(IAssetProperty);
	};

	// -------------------------
	// Editor asset property box
	// -------------------------
	template<typename _type>
	class AssetProperty: public IAssetProperty
	{
	public:
		// Default constructor
		AssetProperty(UIWidget* widget = nullptr);

		// Destructor
		~AssetProperty();

		// Sets fields
		void Setup(const Vector<void*>& targets, bool isProperty);

		// Updates and checks value
		void Update();

		// Returns root widget
		UIWidget* GetWidget() const;

		// Returns value
		const _type& GetCommonValue() const;

		// Returns is values different
		bool IsValuesDifferent() const;

		// Returns editing by this field type
		const Type* GetFieldType() const;

		// Sets value asset id
		void SetAssetId(UID id);

		// Sets value as unknown
		void SetUnknownValue();

		IOBJECT(AssetProperty);

	protected:
		Function<void(void*, const _type&)> mAssignFunc; // Value assign function
		Function<_type(void*)>              mGetFunc;    // Get value function

		Vector<void*> mValuesPointers;  // Fields' pointers
		_type         mCommonValue;     // Common field value (if not different)
		bool          mValuesDifferent; // Are values different

		UIWidget*     mBox;             // Edit box 
		Text*         mNameText;        // Asset name text

	protected:
		// Calls when cursor enters this object
		void OnCursorEnter(const Input::Cursor& cursor);

		// Calls when cursor exits this object
		void OnCursorExit(const Input::Cursor& cursor);

		// Calls when cursor pressed on this
		void OnCursorPressed(const Input::Cursor& cursor);

		// Calls when key was pressed
		void OnKeyPressed(const Input::Key& key);
	};

	template<typename _type>
	AssetProperty<_type>::AssetProperty(UIWidget* widget /*= nullptr*/)
	{
		if (!widget)
			widget = o2UI.CreateWidget<UIWidget>("assetProperty");
		
		mBox = widget;
		mNameText = widget->GetLayerDrawable<Text>("caption");

		SetEventHandleDrawable(mBox);
	}

	template<typename _type>
	AssetProperty<_type>::~AssetProperty()
	{
		SetEventHandleDrawable(nullptr);
		delete mBox;
	}

	template<typename _type>
	void AssetProperty<_type>::Setup(const Vector<void*>& targets, bool isProperty)
	{
		if (isProperty)
		{
			mAssignFunc = [](void* ptr, const _type& value) { *((Property<_type>*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return ((Property<_type>*)(ptr))->Get(); };
		}
		else
		{
			mAssignFunc = [](void* ptr, const _type& value) { *((_type*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return *((_type*)(ptr)); };
		}

		mValuesPointers = targets;

		Update();
	}

	template<typename _type>
	void AssetProperty<_type>::Update()
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
			if (!o2Assets.IsAssetExist(mCommonValue.GetAssetId()))
			{
				mNameText->text = "Null:" + TypeOf(_type).Name().SubStr(4);
				mBox->layer["caption"]->transparency = 0.5f;
			}
			else
			{
				mNameText->text = o2FileSystem.GetFileNameWithoutExtension(
					o2FileSystem.GetPathWithoutDirectories(mCommonValue.GetPath()));
				mBox->layer["caption"]->transparency = 1.0f;
			}
		}
	}

	template<typename _type>
	UIWidget* AssetProperty<_type>::GetWidget() const
	{
		return mBox;
	}

	template<typename _type>
	const _type& AssetProperty<_type>::GetCommonValue() const
	{
		return mCommonValue;
	}

	template<typename _type>
	bool AssetProperty<_type>::IsValuesDifferent() const
	{
		return mValuesDifferent;
	}

	template<typename _type>
	const Type* AssetProperty<_type>::GetFieldType() const
	{
		return &TypeOf(_type);
	}

	template<typename _type>
	void AssetProperty<_type>::SetAssetId(UID id)
	{
		mCommonValue = id == 0 ? _type() : _type(id);
		mValuesDifferent = false;

		for (auto ptr : mValuesPointers)
			mAssignFunc(ptr, mCommonValue);

		onChanged();

		Update();
	}

	template<typename _type>
	void Editor::AssetProperty<_type>::SetUnknownValue()
	{
		mValuesDifferent = true;
		mCommonValue = _type();
		mNameText->text = "--";
		mBox->layer["caption"]->transparency = 1.0f;
	}

	template<typename _type>
	void AssetProperty<_type>::OnCursorPressed(const Input::Cursor& cursor)
	{
		o2UI.FocusWidget(mBox);
		o2EditorAssets.ShowAssetIcon(mCommonValue.GetPath());
	}

	template<typename _type>
	void AssetProperty<_type>::OnCursorExit(const Input::Cursor& cursor)
	{
		mBox->SetState("select", false);
	}

	template<typename _type>
	void AssetProperty<_type>::OnCursorEnter(const Input::Cursor& cursor)
	{
		mBox->SetState("select", true);
	}

	template<typename _type>
	void AssetProperty<_type>::OnKeyPressed(const Input::Key& key)
	{
		if (mBox->IsFocused() && key == VK_DELETE || key == VK_BACK)
			SetAssetId(0);
	}
}
 
META_TEMPLATES(typename _type)
CLASS_TEMPLATE_META(Editor::AssetProperty<typename _type>)
{
	BASE_CLASS(Editor::IAssetProperty);

	PROTECTED_FIELD(mAssignFunc);
	PROTECTED_FIELD(mGetFunc);
	PROTECTED_FIELD(mValuesPointers);
	PROTECTED_FIELD(mCommonValue);
	PROTECTED_FIELD(mValuesDifferent);
	PROTECTED_FIELD(mBox);
	PROTECTED_FIELD(mNameText);

	PUBLIC_FUNCTION(void, Setup, const Vector<void*>&, bool);
	PUBLIC_FUNCTION(void, Update);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(const _type&, GetCommonValue);
	PUBLIC_FUNCTION(bool, IsValuesDifferent);
	PUBLIC_FUNCTION(const Type*, GetFieldType);
	PUBLIC_FUNCTION(void, SetAssetId, UID);
	PUBLIC_FUNCTION(void, SetUnknownValue);
	PROTECTED_FUNCTION(void, OnCursorEnter, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorExit, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorPressed, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnKeyPressed, const Input::Key&);
}
END_META;
 