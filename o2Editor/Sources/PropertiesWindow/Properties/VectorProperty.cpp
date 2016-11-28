#include "VectorProperty.h"

#include "PropertiesWindow/Properties/IntegerProperty.h"
#include "PropertiesWindow/PropertiesWindow.h"
#include "UI/Button.h"
#include "UI/Label.h"
#include "UI/Spoiler.h"
#include "UI/UIManager.h"
#include "UI/VerticalLayout.h"

namespace Editor
{
	VectorProperty::VectorProperty()
	{
		mLayout = mnew UIVerticalLayout();
		mLayout->spacing = 0.0f;
		mLayout->border = RectF(0, 0, 0, 0);
		mLayout->expandHeight = false;
		mLayout->expandWidth = true;
		mLayout->fitByChildren = true;
		mLayout->baseCorner = BaseCorner::RightTop;
		mLayout->layout = UIWidgetLayout::BothStretch();

		auto header = mnew UIWidget();
		header->layout = UIWidgetLayout::BothStretch();
		header->layout.minHeight = 20;

		mExpandButton = o2UI.CreateWidget<UIButton>("expand");
		mExpandButton->layout = UIWidgetLayout::Based(BaseCorner::Left, Vec2F(20, 20), Vec2F(-7, 0));
		mExpandButton->onClick = [&]() { SetExpanded(!IsExpanded()); };
		header->AddChild(mExpandButton, false);

		mNameLabel = o2UI.CreateWidget<UILabel>();
		mNameLabel->layout = UIWidgetLayout::HorStretch(VerAlign::Middle, 10, 0, 20, 0);
		mNameLabel->horAlign = HorAlign::Left;
		header->AddChild(mNameLabel, false);

		mSpoiler = mnew UISpoiler();
		mPropertiesLayout = mnew UIVerticalLayout();
		mPropertiesLayout->spacing = 5.0f;
		mPropertiesLayout->borderLeft = 10;
		mPropertiesLayout->expandHeight = false;
		mPropertiesLayout->expandWidth = true;
		mPropertiesLayout->fitByChildren = true;
		mPropertiesLayout->baseCorner = BaseCorner::RightTop;
		mPropertiesLayout->layout = UIWidgetLayout::BothStretch();
		mSpoiler->AddChild(mPropertiesLayout);

		mLayout->AddChild(header, false);
		mLayout->AddChild(mSpoiler, false);

		auto countPropertyProto = o2EditorProperties.GetFieldPropertyPrototype(&TypeOf(int));
		auto countPropertyWidget = o2EditorProperties.CreateRegularField(countPropertyProto, "Count");

		mCountProperty = (IntegerProperty*)countPropertyWidget.mFirst;
		mPropertiesLayout->AddChild(countPropertyWidget.mSecond);

		mCountProperty->onChanged = Function<void()>(this, &VectorProperty::OnCountChanged);
	}

	VectorProperty::~VectorProperty()
	{
		for (auto x : mValuePropertiesPool)
		{
			delete x.propertyField;
			delete x.widget;
		}

		delete mCountProperty;
		delete mLayout;
	}

	void VectorProperty::Setup(const Vector<void*>& targets, bool isProperty)
	{
		if (isProperty)
		{
			mTargetObjects.Clear();
			return;
		}

		mTargetObjects = targets;
		Update();
	}

	void VectorProperty::Update()
	{
		if (mTargetObjects.IsEmpty())
			return;

		int count = mType->GetObjectVectorSize(mTargetObjects[0]);
		bool isDifferent = false;
		for (auto target : mTargetObjects)
		{
			int targetCount = mType->GetObjectVectorSize(target);
			if (targetCount != count)
			{
				isDifferent = true;
				break;
			}
		}

		if (isDifferent)
		{
			mCountProperty->SetUnknownValue();

			for (auto prop : mValueProperties)
			{
				mPropertiesLayout->RemoveChild(prop.widget, false);
				FreeValueProperty(prop);
			}

			mValueProperties.Clear();

			return;
		}

		mCountProperty->SetValue(count);

		int i = 0;
		for (; i < count; i++)
		{
			Vector<void*> itemTargetValues = mTargetObjects.Select<void*>([&](void* x) { 
				return mType->GetObjectVectorElementPtr(x, i); });

			PropertyDef propertyDef;
			if (i < mValueProperties.Count())
				propertyDef = mValueProperties[i];
			else
			{
				propertyDef = GetFreeValueProperty();
				mValueProperties.Add(propertyDef);
			}

			mPropertiesLayout->AddChild(propertyDef.widget, false);
			propertyDef.nameLabel->text = (String)"Element " + (String)i;
			propertyDef.propertyField->Setup(itemTargetValues, false);
		}

		for (; i < mValueProperties.Count(); i++)
		{
			mPropertiesLayout->RemoveChild(mValueProperties[i].widget, false);
			FreeValueProperty(mValueProperties[i]);
		}

		mPropertiesLayout->UpdateLayout();
	}

	UIWidget* VectorProperty::GetWidget() const
	{
		return mLayout;
	}

	const Type* VectorProperty::GetFieldType() const
	{
		return &TypeOf(void);
	}

	void VectorProperty::SpecializeType(const Type* type)
	{
		mType = (VectorType*)type;
		mNameLabel->text = type->Name();
	}

	void VectorProperty::Expand()
	{
		SetExpanded(true);
	}

	void VectorProperty::Collapse()
	{
		SetExpanded(false);
	}

	void VectorProperty::SetExpanded(bool expanded)
	{
		if (!expanded)
		{
			mSpoiler->Collapse();
			mExpandButton->SetState("expanded", false);
		}
		else
		{
			mSpoiler->Expand();
			mExpandButton->SetState("expanded", true);
		}
	}

	bool VectorProperty::IsExpanded() const
	{
		return mSpoiler->IsExpanded();
	}

	VectorProperty::PropertyDef VectorProperty::GetFreeValueProperty()
	{
		if (!mValuePropertiesPool.IsEmpty())
			return mValuePropertiesPool.PopBack();

		PropertyDef res;
		auto fieldPropertySamplePair = o2EditorProperties.CreateFieldProperty(mType->GetElementType());

		res.propertyField = fieldPropertySamplePair.mFirst;
		res.widget = fieldPropertySamplePair.mSecond;
		res.nameLabel = res.widget->FindChild<UILabel>();

		return res;
	}

	void VectorProperty::FreeValueProperty(PropertyDef def)
	{
		mValuePropertiesPool.Add(def);
	}

	void VectorProperty::OnCountChanged()
	{
		int newCount = mCountProperty->GetCommonValue();
		for (auto target : mTargetObjects)
			mType->SetObjectVectorSize(target, newCount);

		Update();
	}

}
 
CLASS_META(Editor::VectorProperty)
{
	BASE_CLASS(Editor::IPropertyField);

	PROTECTED_FIELD(mType);
	PROTECTED_FIELD(mTargetObjects);
	PROTECTED_FIELD(mLayout);
	PROTECTED_FIELD(mNameLabel);
	PROTECTED_FIELD(mExpandButton);
	PROTECTED_FIELD(mSpoiler);
	PROTECTED_FIELD(mPropertiesLayout);
	PROTECTED_FIELD(mValueProperties);
	PROTECTED_FIELD(mValuePropertiesPool);
	PROTECTED_FIELD(mCountProperty);

	PUBLIC_FUNCTION(void, Setup, const Vector<void*>&, bool);
	PUBLIC_FUNCTION(void, Update);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(const Type*, GetFieldType);
	PUBLIC_FUNCTION(void, SpecializeType, const Type*);
	PUBLIC_FUNCTION(void, Expand);
	PUBLIC_FUNCTION(void, Collapse);
	PUBLIC_FUNCTION(void, SetExpanded, bool);
	PUBLIC_FUNCTION(bool, IsExpanded);
	PROTECTED_FUNCTION(PropertyDef, GetFreeValueProperty);
	PROTECTED_FUNCTION(void, FreeValueProperty, PropertyDef);
}
END_META;
 