#include "FloatProperty.h"

#include "Application/Application.h"
#include "SceneWindow/SceneEditScreen.h"
#include "UI/Button.h"
#include "UI/EditBox.h"
#include "UI/UIManager.h"
#include "UI/WidgetLayer.h"
#include "UI/WidgetLayout.h"
#include "UI/WidgetState.h"

namespace Editor
{
	FloatProperty::FloatProperty(UIWidget* widget /*= nullptr*/)
	{
		if (widget)
			mPropertyWidget = widget;
		else
			mPropertyWidget = o2UI.CreateWidget<UIWidget>("singleline edit property");

		mEditBox = mPropertyWidget->GetChildByType<UIEditBox>();
		mEditBox->onChangeCompleted = THIS_FUNC(OnEdited);
		mEditBox->text = "--";
		mEditBox->SetFilterFloat();

		mRevertBtn = mPropertyWidget->GetChildByType<UIButton>();
		if (mRevertBtn)
			mRevertBtn->onClick = THIS_FUNC(Revert);

		auto handleLayer = mEditBox->FindLayer("arrows");

		if (handleLayer)
		{
			mEditBox->onDraw += [&]() { mDragHangle.OnDrawn(); };

			mDragHangle.cursorType = CursorType::SizeNS;
			mDragHangle.isUnderPoint = [=](const Vec2F& point) { return handleLayer->IsUnderPoint(point); };
			mDragHangle.onMoved = THIS_FUNC(OnDragHandleMoved);
			mDragHangle.onCursorPressed = THIS_FUNC(OnMoveHandlePressed);
			mDragHangle.onCursorReleased = THIS_FUNC(OnMoveHandleReleased);
		}
	}

	FloatProperty::~FloatProperty()
	{
		delete mPropertyWidget;
	}

	void FloatProperty::SetValueAndPrototypePtr(const TargetsVec& targets, bool isProperty)
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

		Refresh();
	}

	void FloatProperty::Refresh()
	{
		if (mValuesPointers.IsEmpty())
			return;

		auto lastCommonValue = mCommonValue;
		auto lastDifferent = mValuesDifferent;

		auto newCommonValue = mGetFunc(mValuesPointers[0].first);
		auto newDifferent = false;

		for (int i = 1; i < mValuesPointers.Count(); i++)
		{
			if (newCommonValue != mGetFunc(mValuesPointers[i].first))
			{
				newDifferent = true;
				break;
			}
		}

		if (newDifferent)
		{
			if (!lastDifferent)
				SetUnknownValue(newCommonValue);
		}
		else if (!Math::Equals(lastCommonValue, newCommonValue) || lastDifferent)
			SetCommonValue(newCommonValue);

		CheckRevertableState();
	}

	void FloatProperty::Revert()
	{
		for (auto ptr : mValuesPointers)
		{
			if (ptr.second)
			{
				mAssignFunc(ptr.first, mGetFunc(ptr.second));
			}
		}

		Refresh();
	}

	UIWidget* FloatProperty::GetWidget() const
	{
		return mPropertyWidget;
	}

	float FloatProperty::GetCommonValue() const
	{
		return mCommonValue;
	}

	bool FloatProperty::IsValuesDifferent() const
	{
		return mValuesDifferent;
	}

	void FloatProperty::SetValue(float value)
	{
		for (auto ptr : mValuesPointers)
			mAssignFunc(ptr.first, value);

		SetCommonValue(value);
	}

	void FloatProperty::SetUnknownValue(float defaultValue /*= 0.0f*/)
	{
		mCommonValue = defaultValue;
		mValuesDifferent = true;
		mEditBox->text = "--";

		OnChanged();
	}

	const Type* FloatProperty::GetFieldType() const
	{
		return &TypeOf(float);
	}

	void FloatProperty::SetCommonValue(float value)
	{
		mCommonValue = value;
		mValuesDifferent = false;
		mEditBox->text = (WString)mCommonValue;

		OnChanged();
	}

	void FloatProperty::CheckRevertableState()
	{
		bool revertable = false;

		for (auto ptr : mValuesPointers)
		{
			if (ptr.second && !Math::Equals(mGetFunc(ptr.first), mGetFunc(ptr.second)))
			{
				revertable = true;
				break;
			}
		}

		if (auto state = mPropertyWidget->state["revert"])
			*state = revertable;
	}

	void FloatProperty::OnEdited(const WString& data)
	{
		if (mValuesDifferent && data == "--")
			return;

		SetValueByUser((const float)data);
	}

	void FloatProperty::OnDragHandleMoved(const Input::Cursor& cursor)
	{
		float multiplier = Math::Abs(cursor.delta.y) < 20 ? 0.01f : 0.05f;
		SetValue(mCommonValue + cursor.delta.y*multiplier);
	}

	void FloatProperty::OnKeyReleased(const Input::Key& key)
	{
		if (!mEditBox->IsFocused())
			return;

		if (key == VK_UP)
		{
			SetValueByUser(Math::Ceil(mCommonValue + 0.01f));
			mEditBox->SelectAll();
		}

		if (key == VK_DOWN)
		{
			SetValueByUser(Math::Floor(mCommonValue - 0.01f));
			mEditBox->SelectAll();
		}
	}

	void FloatProperty::OnMoveHandlePressed(const Input::Cursor& cursor)
	{
		StoreValues(mBeforeChangeValues);
		o2Application.SetCursorInfiniteMode(true);
	}

	void FloatProperty::OnMoveHandleReleased(const Input::Cursor& cursor)
	{
		o2Application.SetCursorInfiniteMode(false);
		CheckValueChangeCompleted();
	}

	void FloatProperty::SetValueByUser(float value)
	{
		StoreValues(mBeforeChangeValues);
		SetValue(value);
		CheckValueChangeCompleted();
	}

	void FloatProperty::CheckValueChangeCompleted()
	{
		Vector<DataNode> valuesData;
		StoreValues(valuesData);

		if (mBeforeChangeValues != valuesData)
			onChangeCompleted(mValuesPath, mBeforeChangeValues, valuesData);
	}

	void FloatProperty::StoreValues(Vector<DataNode>& data) const
	{
		data.Clear();
		for (auto ptr : mValuesPointers)
		{
			data.Add(DataNode());
			data.Last() = mGetFunc(ptr.first);
		}
	}

}

DECLARE_CLASS(Editor::FloatProperty);
