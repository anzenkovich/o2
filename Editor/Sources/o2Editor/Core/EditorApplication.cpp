#include "o2Editor/stdafx.h"
#include "EditorApplication.h"

#include "o2/Animation/AnimatedFloat.h"
#include "o2/Animation/AnimatedVector.h"
#include "o2/Application/Input.h"
#include "o2/Assets/Assets.h"
#include "o2/Events/EventSystem.h"
#include "o2/Render/Render.h"
#include "o2/Scene/Actor.h"
#include "o2/Scene/Components/ImageComponent.h"
#include "o2/Scene/Scene.h"
#include "o2/Scene/UI/UIManager.h"
#include "o2/Scene/UI/Widget.h"
#include "o2/Scene/UI/WidgetState.h"
#include "o2/Scene/UI/Widgets/MenuPanel.h"
#include "o2/Scene/UI/Widgets/MenuPanel.h"
#include "o2/Utils/Debug/Debug.h"
#include "o2/Utils/System/Time/Time.h"
#include "o2/Utils/System/Time/Timer.h"
#include "o2/Utils/Tasks/TaskManager.h"
#include "o2Editor/AnimationWindow/AnimationWindow.h"
#include "o2Editor/Core/Actions/IAction.h"
#include "o2Editor/Core/Actions/PropertyChange.h"
#include "o2Editor/Core/EditorScope.h"
#include "o2Editor/Core/MenuPanel.h"
#include "o2Editor/Core/Properties/Properties.h"
#include "o2Editor/Core/ToolsPanel.h"
#include "o2Editor/Core/UIRoot.h"
#include "o2Editor/Core/UIStyle/EditorUIStyle.h"
#include "o2Editor/Core/WindowsSystem/WindowsManager.h"
#include "o2Editor/SceneWindow/SceneEditScreen.h"
#include "o2Editor/TreeWindow/TreeWindow.h"

namespace Editor
{
	EditorApplication::EditorApplication()
	{}

	EditorApplication::~EditorApplication()
	{}

	const String& EditorApplication::GetLoadedSceneName() const
	{
		return mLoadedScene;
	}

	void EditorApplication::LoadScene(const String& name)
	{
		mLoadedScene = name;
		o2Scene.Load(name);

		ResetUndoActions();
	}

	void EditorApplication::SaveScene(const String& name)
	{
		mLoadedScene = name;
		o2Scene.Save(name);
	}

	void EditorApplication::MakeNewScene()
	{
		mLoadedScene = "";
		o2Scene.Clear();

		ResetUndoActions();
	}

	bool EditorApplication::IsSceneChanged() const
	{
		return GetUndoActionsCount() > 0;
	}

	void EditorApplication::OnStarted()
	{
		PushEditorScopeOnStack enterScope;

		o2Application.SetWindowCaption("o2 Editor");

		mUIRoot = mnew UIRoot();

		mBackground = mnew Sprite("ui/UI4_Background.png");
		mBackSign = mnew Sprite("ui/UI4_o2_sign.png");

		mConfig = mnew EditorConfig();
		mConfig->LoadConfigs();

		LoadUIStyle();

		mProperties = mnew Properties();
		mWindowsManager = mnew WindowsManager();
		mMenuPanel = mnew MenuPanel();
		mToolsPanel = mnew ToolsPanel();

		if (mConfig->mProjectConfig.mMaximized)
			o2Application.Maximize();
		else
		{
			Vec2I pos = mConfig->mProjectConfig.mWindowPosition;
			o2Application.SetWindowSize(mConfig->mProjectConfig.mWindowSize);
			o2Application.SetWindowPosition(pos);
			mConfig->mProjectConfig.mWindowPosition = pos;
		}

		OnResizing();

		auto widget = EditorUIRoot.GetRootWidget()->GetChildWidget("tools panel/play panel");
		o2EditorAnimationWindow.SetAnimation(&widget->GetStateObject("playing")->animation);
		o2EditorAnimationWindow.SetTarget(widget);
	}

	void EditorApplication::OnClosing()
	{
		delete mConfig;
		delete mWindowsManager;
		delete mBackground;
		delete mBackSign;
		delete mToolsPanel;
		delete mMenuPanel;
		delete mUIRoot;
	}

	void EditorApplication::OnResizing()
	{
		mBackground->SetSize(o2Render.GetResolution() + Vec2F(20, 20));
		mBackSign->position = (Vec2F)(o2Render.GetResolution()).InvertedX()*0.5f + Vec2F(40.0f, -85.0f);

		mConfig->OnWindowChange();
		mUIRoot->OnApplicationSized();
	}

	void EditorApplication::OnMoved()
	{
		mConfig->OnWindowChange();
	}

	void EditorApplication::ProcessFrame()
	{
		if (!mReady)
			return;

		if (mCursorInfiniteModeEnabled)
			CheckCursorInfiniteMode();

		float maxFPS = 60.0f;
		float maxFPSDeltaTime = 1.0f/maxFPS;

		float realdDt = mTimer->GetDeltaTime();

		if (realdDt < maxFPSDeltaTime)
		{
			Sleep((int)((maxFPSDeltaTime - realdDt)*1000.0f));
			realdDt = maxFPSDeltaTime;
		}

		float dt = Math::Clamp(realdDt, 0.001f, 0.05f);

		mInput->PreUpdate();
		mTime->Update(realdDt);
		o2Debug.Update(dt);
		mTaskManager->Update(dt);
		mEventSystem->Update(dt);

		mRender->Begin();

		OnDraw();
		OnUpdate(dt);

		mUIRoot->Update(dt);
		mEventSystem->PostUpdate();
		mScene->Update(dt);


		OnDraw();
		mUIRoot->Draw();
		mUIManager->Draw();
		o2Debug.Draw();

		mRender->End();

		mInput->Update(dt);

		mDrawCalls = mRender->GetDrawCallsCount();
	}

	void EditorApplication::LoadUIStyle()
	{
		if (mNeedRebuildWndStyle)
		{
			EditorUIStyleBuilder builder;
			builder.RebuildEditorUIManager(false, false);
		}

		//o2UI.LoadStyle("editor_ui_style.xml");
	}

	void EditorApplication::OnUpdate(float dt)
	{
		mWindowsManager->Update(dt);
		o2Application.windowCaption = String("o2 Editor. FPS: ") + (String)((int)o2Time.GetFPS()) +
			" DC: " + (String)mDrawCalls +
			" Cursor: " + (String)o2Input.GetCursorPos();

		if (o2Input.IsKeyPressed('K'))
			o2Memory.DumpInfo();
	}

#undef DrawText

	void EditorApplication::OnDraw()
	{
		o2Render.Clear();
		o2Render.camera = Camera::Default();

		mBackground->Draw();
		mBackSign->Draw();
		mWindowsManager->Draw();

		// Debug draw undo actions
		if (o2Input.IsKeyDown(VK_F6)) 
		{
			for (int i = 0; i < mActions.Count(); i++)
				o2Debug.DrawText(Vec2F(0, (float)(20 * i)), (String)i + mActions[i]->GetName());
		}
	}

	void EditorApplication::OnActivated()
	{
		//o2Assets.RebuildAssets();
	}

	void EditorApplication::OnDeactivated()
	{}

}