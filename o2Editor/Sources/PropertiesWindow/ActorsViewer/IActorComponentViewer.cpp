#include "IActorComponentViewer.h"

#include "UI/Button.h"
#include "UI/Image.h"
#include "UI/Label.h"
#include "UI/Spoiler.h"
#include "UI/UIManager.h"
#include "UI/VerticalLayout.h"
#include "UI/Widget.h"

namespace Editor
{
	IActorComponentViewer::IActorComponentViewer()
	{
		mDataView = mnew UIVerticalLayout();
		mDataView->spacing = 0.0f;
		mDataView->border = RectF(0, 0, 0, 0);
		mDataView->expandHeight = false;
		mDataView->expandWidth = true;
		mDataView->fitByChildren = true;
		mDataView->baseCorner = BaseCorner::RightTop;
		mDataView->layout = UIWidgetLayout::BothStretch();

		mSpoiler = mnew UISpoiler();
		mSpoiler->name = "spoiler";
		mSpoiler->SetExpanded(true);

		UIWidget* componentHead = mnew UIWidget();
		componentHead->name = "head";
		componentHead->layout = UIWidgetLayout::HorStretch(VerAlign::Top, 0, 0, 18);
		componentHead->AddLayer("back", mnew Sprite("ui/UI2_component_head.png"), 
								Layout::BothStretch(-2, -2, -2, -2));
		mDataView->AddChild(componentHead);

		auto separatorImg = o2UI.CreateImage("ui/UI_Separator.png");
		separatorImg->layout = UIWidgetLayout::HorStretch(VerAlign::Top, -1, -1, 5, -2);
		componentHead->AddChild(separatorImg);

		mExpandBtn = o2UI.CreateWidget<UIButton>("expand");
		mExpandBtn->layout = UIWidgetLayout::Based(BaseCorner::Left, Vec2F(15, 15), Vec2F(0, 0));
		mExpandBtn->onClick = [&]() { if (mSpoiler->IsExpanded()) Collapse(); else Expand(); };
		componentHead->AddChild(mExpandBtn);
		
		mNameCaption = o2UI.CreateLabel("Unknown");
		mNameCaption->layout = UIWidgetLayout::HorStretch(VerAlign::Middle, 15, 30, 15, 0);
		mNameCaption->horAlign = HorAlign::Left;
		componentHead->AddChild(mNameCaption);

		mSaveBtn = o2UI.CreateWidget<UIButton>("componentSave");
		mSaveBtn->layout = UIWidgetLayout::Based(BaseCorner::Right, Vec2F(15, 15), Vec2F(-15, 0));
		componentHead->AddChild(mSaveBtn);

		mOptionsBtn = o2UI.CreateWidget<UIButton>("componentOptions");
		mOptionsBtn->layout = UIWidgetLayout::Based(BaseCorner::Right, Vec2F(15, 15), Vec2F(0, 0));
		componentHead->AddChild(mOptionsBtn);

		mPropertiesLayout = mnew UIVerticalLayout();
		mPropertiesLayout->name = "properties";
		mPropertiesLayout->spacing = 5.0f;
		mPropertiesLayout->border = RectF(5, 5, 5, 5);
		mPropertiesLayout->expandHeight = false;
		mPropertiesLayout->expandWidth = true;
		mPropertiesLayout->fitByChildren = true;
		mPropertiesLayout->baseCorner = BaseCorner::Top;
		mPropertiesLayout->layout = UIWidgetLayout::HorStretch(VerAlign::Top, 0, 0, 10);
		mSpoiler->AddChild(mPropertiesLayout);

		mDataView->AddChild(mSpoiler);
	}

	IActorComponentViewer::~IActorComponentViewer()
	{
		delete mDataView;
	}

	UIWidget* IActorComponentViewer::GetWidget() const
	{
		return mDataView;
	}

	void IActorComponentViewer::Expand()
	{
		mExpandBtn->SetState("expanded", true);
		mSpoiler->Expand();
	}

	void IActorComponentViewer::Collapse()
	{
		mExpandBtn->SetState("expanded", false);
		mSpoiler->Collapse();
	}

}
 
CLASS_META(Editor::IActorComponentViewer)
{
	BASE_CLASS(o2::IObject);

	PROTECTED_FIELD(mDataView);
	PROTECTED_FIELD(mPropertiesLayout);
	PROTECTED_FIELD(mExpandBtn);
	PROTECTED_FIELD(mSpoiler);
	PROTECTED_FIELD(mNameCaption);
	PROTECTED_FIELD(mOptionsBtn);
	PROTECTED_FIELD(mSaveBtn);
	PROTECTED_FIELD(mIcon);

	PUBLIC_FUNCTION(void, SetTargetComponents, const Vector<Component*>&);
	PUBLIC_FUNCTION(const Type*, GetComponentType);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(void, Expand);
	PUBLIC_FUNCTION(void, Collapse);
}
END_META;
 