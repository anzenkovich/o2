#include "stdafx.h"
#include "Scene/UI/Widgets/VerticalProgress.h"

#include "Scene/UI/WidgetLayer.h"
#include "Scene/UI/WidgetLayout.h"
#include "Scene/UI/WidgetState.h"

namespace o2
{
	UIVerticalProgress::UIVerticalProgress():
		UIWidget(), DrawableCursorEventsListener(this)
	{}

	UIVerticalProgress::UIVerticalProgress(const UIVerticalProgress& other) :
		UIWidget(other), DrawableCursorEventsListener(this), mValue(other.mValue), mMinValue(other.mMinValue),
		mMaxValue(other.mMaxValue), mOrientation(other.mOrientation), mScrollSense(other.mScrollSense), value(this),
		minValue(this), maxValue(this), scrollSense(this)
	{
		mBarLayer = FindLayer("bar");
		mBackLayer = FindLayer("back");

		RetargetStatesAnimations();
	}

	UIVerticalProgress::~UIVerticalProgress()
	{}

	UIVerticalProgress& UIVerticalProgress::operator=(const UIVerticalProgress& other)
	{
		UIWidget::operator=(other);
		return *this;
	}

	void UIVerticalProgress::Update(float dt)
	{
		UIWidget::Update(dt);

		if (!mResEnabledInHierarchy || mIsClipped)
			return;

		const float threshold = 0.01f;
		const float smoothCoef = 30.0f;

		if (!Math::Equals(mValue, mSmoothValue, threshold))
		{
			mSmoothValue = Math::Clamp(Math::Lerp(mSmoothValue, mValue, dt*smoothCoef), mMinValue, mMaxValue);

			if (Math::Abs(mValue - mSmoothValue) < threshold)
				mSmoothValue = mValue;

			UpdateProgressLayersLayouts();
		}
	}

	void UIVerticalProgress::SetValue(float value)
	{
		mValue = Math::Clamp(value, mMinValue, mMaxValue);
		UpdateProgressLayersLayouts();

		onChange(mValue);
	}

	void UIVerticalProgress::SetValueForcible(float value)
	{
		mValue = Math::Clamp(value, mMinValue, mMaxValue);
		mSmoothValue = mValue;
		UpdateProgressLayersLayouts();

		onChange(mValue);
	}

	float UIVerticalProgress::GetValue() const
	{
		return mValue;
	}

	void UIVerticalProgress::SetMinValue(float minValue)
	{
		mMinValue = minValue;
		mValue = Math::Max(mMinValue, mValue);
		UpdateProgressLayersLayouts();

		onChange(mValue);
	}

	float UIVerticalProgress::GetMinValue() const
	{
		return mMinValue;
	}

	void UIVerticalProgress::SetMaxValue(float maxValue)
	{
		mMaxValue = maxValue;
		mValue = Math::Min(mMaxValue, mValue);
		UpdateProgressLayersLayouts();

		onChange(value);
	}

	float UIVerticalProgress::GetMaxValue() const
	{
		return mMaxValue;
	}

	void UIVerticalProgress::SetValueRange(float minValue, float maxValue)
	{
		mMaxValue = maxValue;
		mValue = Math::Clamp(mValue, mMinValue, mMaxValue);
		UpdateProgressLayersLayouts();

		onChange(value);
	}

	void UIVerticalProgress::SetScrollSense(float coef)
	{
		mScrollSense = coef;
	}

	float UIVerticalProgress::GetScrollSense() const
	{
		return mScrollSense;
	}

	void UIVerticalProgress::SetOrientation(Orientation orientation)
	{
		mOrientation = orientation;
		UpdateLayersLayouts();
	}

	UIVerticalProgress::Orientation UIVerticalProgress::GetOrientation() const
	{
		return mOrientation;
	}

	bool UIVerticalProgress::IsUnderPoint(const Vec2F& point)
	{
		if (mBackLayer)
			return mDrawingScissorRect.IsInside(point) && mBackLayer->IsUnderPoint(point);

		return false;
	}

	bool UIVerticalProgress::IsScrollable() const
	{
		return true;
	}

	void UIVerticalProgress::OnCursorPressed(const Input::Cursor& cursor)
	{
		auto pressedState = state["pressed"];
		if (pressedState)
			*pressedState = true;

		GetValueFromCursor(cursor);
	}

	void UIVerticalProgress::OnCursorReleased(const Input::Cursor& cursor)
	{
		auto pressedState = state["pressed"];
		if (pressedState)
			*pressedState = false;
	}

	void UIVerticalProgress::OnCursorPressBreak(const Input::Cursor& cursor)
	{
		auto pressedState = state["pressed"];
		if (pressedState)
			*pressedState = false;
	}

	void UIVerticalProgress::OnCursorStillDown(const Input::Cursor& cursor)
	{
		GetValueFromCursor(cursor);
	}

	void UIVerticalProgress::GetValueFromCursor(const Input::Cursor &cursor)
	{
		float height = layout->height;
		float d = mMaxValue - mMinValue;
		if (mOrientation == UIVerticalProgress::Orientation::Up)

			SetValue((cursor.position.y - layout->worldBottom)/height*d + mMinValue);
		else
			SetValue((height - (cursor.position.y - layout->worldBottom))/height*d + mMinValue);
	}

	void UIVerticalProgress::OnCursorEnter(const Input::Cursor& cursor)
	{
		auto selectState = state["hover"];
		if (selectState)
			*selectState = true;
	}

	void UIVerticalProgress::OnCursorExit(const Input::Cursor& cursor)
	{
		auto selectState = state["hover"];
		if (selectState)
			*selectState = false;
	}

	void UIVerticalProgress::OnScrolled(float scroll)
	{
		SetValue(mValue + scroll*mScrollSense);
	}

	void UIVerticalProgress::OnDeserialized(const DataNode& node)
	{
		UIWidget::OnDeserialized(node);

		mBarLayer = FindLayer("bar");
		mBackLayer = FindLayer("back");

		RetargetStatesAnimations();
	}

	void UIVerticalProgress::OnResEnableInHierarchyChanged()
	{
		interactable = mResEnabled;
	}

	void UIVerticalProgress::UpdateLayersLayouts()
	{
		UpdateProgressLayersLayouts();
	}

	void UIVerticalProgress::UpdateProgressLayersLayouts()
	{
		if (mBarLayer)
		{
			mBarLayer->layout.offsetMin = Vec2F();
			mBarLayer->layout.offsetMax = Vec2F();

			if (mOrientation == Orientation::Down)
			{
				mBarLayer->layout.anchorMin = Vec2F(0, 1.0f - (mSmoothValue - mMinValue)/(mMaxValue - mMinValue));
				mBarLayer->layout.anchorMax = Vec2F(1, 1);
			}
			else if (mOrientation == Orientation::Up)
			{
				mBarLayer->layout.anchorMin = Vec2F(0, 0);
				mBarLayer->layout.anchorMax = Vec2F(1, (mSmoothValue - mMinValue)/(mMaxValue - mMinValue));
			}
		}

		if (mBackLayer)
			mBackLayer->layout = Layout::BothStretch();

		UIWidget::UpdateLayersLayouts();
	}

	void UIVerticalProgress::CopyData(const Actor& otherActor)
	{
		const UIVerticalProgress& other = dynamic_cast<const UIVerticalProgress&>(otherActor);

		UIWidget::CopyData(other);

		mValue       = other.mValue;
		mMinValue    = other.mMinValue;
		mMaxValue    = other.mMaxValue;
		mOrientation = other.mOrientation;
		mScrollSense = other.mScrollSense;
		mBarLayer    = FindLayer("bar");
		mBackLayer   = FindLayer("back");

		RetargetStatesAnimations();
		SetLayoutDirty();
	}

	void UIVerticalProgress::OnLayerAdded(UIWidgetLayer* layer)
	{
		if (layer->name == "back")
			mBackLayer = layer;
		else if (layer->name == "bar")
			mBarLayer = layer;

		UpdateProgressLayersLayouts();
	}
}

DECLARE_CLASS(o2::UIVerticalProgress);

ENUM_META_(o2::UIVerticalProgress::Orientation, Orientation)
{
	ENUM_ENTRY(Down);
	ENUM_ENTRY(Up);
}
END_ENUM_META;