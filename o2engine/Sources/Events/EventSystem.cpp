#include "EventSystem.h"

#include "Events/ApplicationEventsListener.h"
#include "Events/CursorEventsListener.h"
#include "Events/DragEventsListener.h"
#include "Events/KeyboardEventsListener.h"

namespace o2
{
	DECLARE_SINGLETON(EventSystem);

	EventSystem::EventSystem()
	{}

	EventSystem::~EventSystem()
	{}

	void EventSystem::Update(float dt)
	{
		mCursorListeners.Sort([](Ptr<CursorEventsListener> a, Ptr<CursorEventsListener> b) { return a->Depth() > b->Depth(); });
		mDragListeners.Sort([](Ptr<DragEventsListener> a, Ptr<DragEventsListener> b) { return a->Depth() > b->Depth(); });

		mLastUnderCursorListeners = mUnderCursorListeners;
		mUnderCursorListeners.Clear();

		for (const Input::Cursor& cursor : o2Input.GetCursors())
			ProcessCursorTracing(cursor);

		ProcessCursorEnter();
		ProcessCursorExit();

		for (const Input::Cursor& cursor : o2Input.GetCursors())
		{
			if (cursor.mPressedTime < FLT_EPSILON && cursor.mPressed)
				ProcessCursorPressed(cursor);
			else
				ProcessCursorDown(cursor);
		}

		for (const Input::Cursor& cursor : o2Input.GetReleasedCursors())
		{
			ProcessCursorReleased(cursor);
			mPressedListeners.Remove(cursor.mId);
		}

		for (const Input::Key& key : o2Input.GetPressedKeys())
		{
			if (key.mKey == -1)
				ProcessRBPressed();
			else if (key.mKey == -2)
				ProcessMBPressed();
			else
				ProcessKeyPressed(key);
		}

		for (const Input::Key& key : o2Input.GetDownKeys())
		{
			if (key.mKey == -1)
				ProcessRBDown();
			else if (key.mKey == -2)
				ProcessMBDown();
			else
				ProcessKeyDown(key);
		}

		for (const Input::Key& key : o2Input.GetReleasedKeys())
		{
			if (key.mKey == -1)
				ProcessRBReleased();
			else if (key.mKey == -2)
				ProcessMBReleased();
			else
				ProcessKeyReleased(key);
		}
	}

	void EventSystem::OnApplicationStarted()
	{
		for (auto listener : mApplicationListeners)
			listener->OnApplicationStarted();
	}

	void EventSystem::OnApplicationClosing()
	{
		for (auto listener : mApplicationListeners)
			listener->OnApplicationClosing();
	}

	void EventSystem::OnApplicationActivated()
	{
		for (auto listener : mApplicationListeners)
			listener->OnApplicationActivated();
	}

	void EventSystem::OnApplicationDeactivated()
	{
		for (auto listener : mApplicationListeners)
			listener->OnApplicationDeactivated();
	}

	void EventSystem::OnApplicationSized()
	{
		for (auto listener : mApplicationListeners)
			listener->OnApplicationSized();
	}

	void EventSystem::ProcessCursorTracing(const Input::Cursor& cursor)
	{
		for (auto listener : mCursorListeners)
		{
			if (!listener->IsUnderPoint(cursor.mPosition))
				continue;

			auto drag = dynamic_cast<DragEventsListener*>(listener.Get());
			if (drag && drag->IsDragging())
				continue;

			mUnderCursorListeners.Add(cursor.mId, listener);

			return;
		}
	}

	void EventSystem::ProcessCursorEnter()
	{
		for (auto underCursor : mUnderCursorListeners)
		{
			if (!(mLastUnderCursorListeners.ContainsKey(underCursor.Key()) &&
				  mLastUnderCursorListeners[underCursor.Key()] == underCursor.Value()))
			{
				underCursor.Value()->OnCursorEnter(*o2Input.GetCursor(underCursor.Key()));
			}
		}
	}

	void EventSystem::ProcessCursorExit()
	{
		for (auto lastUnderCursor : mLastUnderCursorListeners)
		{
			if (!(mUnderCursorListeners.ContainsKey(lastUnderCursor.Key()) &&
				  mUnderCursorListeners[lastUnderCursor.Key()] == lastUnderCursor.Value()))
			{
				lastUnderCursor.Value()->OnCursorExit(*o2Input.GetCursor(lastUnderCursor.Key()));
			}
		}
	}

	Ptr<DragEventsListener> EventSystem::GetListenerUnderCursor(CursorId cursorId) const
	{
		for (auto listener : mDragListeners)
		{
			if (listener->IsDragging())
				continue;

			if (!listener->IsUnderPoint(o2Input.GetCursorPos(cursorId)))
				continue;

			return listener;

			break;
		}

		return nullptr;
	}

	void EventSystem::ProcessCursorPressed(const Input::Cursor& cursor)
	{
		if (!mUnderCursorListeners.ContainsKey(cursor.mId))
			return;

		auto listener = mUnderCursorListeners[cursor.mId];

		mPressedListeners.Add(cursor.mId, listener);

		listener->OnCursorPressed(cursor);
		listener->mIsPressed = true;
	}

	void EventSystem::ProcessCursorDown(const Input::Cursor& cursor)
	{
		if (mPressedListeners.ContainsKey(cursor.mId))
			mPressedListeners[cursor.mId]->OnCursorStayDown(cursor);

		if (cursor.mDelta.Length() > FLT_EPSILON)
		{
			if (mUnderCursorListeners.ContainsKey(cursor.mId))
				mUnderCursorListeners[cursor.mId]->OnCursorMoved(cursor);
		}
	}

	void EventSystem::ProcessCursorReleased(const Input::Cursor& cursor)
	{
		if (mPressedListeners.ContainsKey(cursor.mId))
		{
			mPressedListeners[cursor.mId]->OnCursorReleased(cursor);
			mPressedListeners[cursor.mId]->mIsPressed = false;
			mPressedListeners.Remove(cursor.mId);
		}
	}

	void EventSystem::ProcessRBPressed()
	{
		const Input::Cursor& cursor = *o2Input.GetCursor(0);

		if (!mUnderCursorListeners.ContainsKey(cursor.mId))
			return;

		auto listener = mUnderCursorListeners[cursor.mId];

		mRightButtonPressedListener = listener;

		listener->OnCursorRightMousePressed(cursor);
		listener->mIsRightMousePressed = true;
	}

	void EventSystem::ProcessRBDown()
	{
		const Input::Cursor& cursor = *o2Input.GetCursor(0);

		if (mRightButtonPressedListener)
			mRightButtonPressedListener->OnCursorRightMouseStayDown(cursor);
	}

	void EventSystem::ProcessRBReleased()
	{
		const Input::Cursor& cursor = *o2Input.GetCursor(0);

		if (mRightButtonPressedListener)
		{
			mRightButtonPressedListener->OnCursorRightMouseReleased(cursor);
			mRightButtonPressedListener->mIsRightMousePressed = false;
		}
	}

	void EventSystem::ProcessMBPressed()
	{
		const Input::Cursor& cursor = *o2Input.GetCursor(0);

		if (!mUnderCursorListeners.ContainsKey(cursor.mId))
			return;

		auto listener = mUnderCursorListeners[cursor.mId];

		mMiddleButtonPressedListener = listener;

		listener->OnCursorMiddleMousePressed(cursor);
		listener->mIsMiddleMousePressed = true;
	}

	void EventSystem::ProcessMBDown()
	{
		const Input::Cursor& cursor = *o2Input.GetCursor(0);

		if (mMiddleButtonPressedListener)
			mMiddleButtonPressedListener->OnCursorMiddleMouseStayDown(cursor);
	}

	void EventSystem::ProcessMBReleased()
	{
		const Input::Cursor& cursor = *o2Input.GetCursor(0);

		if (mMiddleButtonPressedListener)
		{
			mMiddleButtonPressedListener->OnCursorMiddleMouseReleased(cursor);
			mMiddleButtonPressedListener->mIsMiddleMousePressed = false;
		}
	}

	void EventSystem::ProcessScrolling()
	{
		float scroll = o2Input.GetMouseWheelDelta();
		if (!Math::Equals(scroll, 0.0f))
		{
			for (auto listener : mCursorListeners)
				listener->OnScrolled(scroll);
		}
	}

	void EventSystem::ProcessKeyPressed(const Input::Key& key)
	{
		for (auto listener : mKeyboardListeners)
			listener->OnKeyPressed(key);
	}

	void EventSystem::ProcessKeyDown(const Input::Key& key)
	{
		for (auto listener : mKeyboardListeners)
			listener->OnKeyStayDown(key);
	}

	void EventSystem::ProcessKeyReleased(const Input::Key& key)
	{
		for (auto listener : mKeyboardListeners)
			listener->OnKeyReleased(key);
	}

}