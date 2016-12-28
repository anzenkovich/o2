#include "AssetsPropertiesViewer.h"

#include "Assets/Asset.h"
#include "Assets/Assets.h"
#include "AssetsWindow/AssetsWindow.h"
#include "PropertiesWindow/AssetsViewer/DefaultAssetPropertiesViewer.h"
#include "PropertiesWindow/AssetsViewer/IAssetPropertiesViewer.h"
#include "Render/Sprite.h"
#include "UI/Button.h"
#include "UI/EditBox.h"
#include "UI/Image.h"
#include "UI/Label.h"
#include "UI/UIManager.h"
#include "UI/VerticalLayout.h"
#include "UI/Widget.h"
#include "Utils/FileSystem/FileSystem.h"

namespace Editor
{

	AssetsPropertiesViewer::AssetsPropertiesViewer()
	{
		auto viewersTypes = TypeOf(IAssetPropertiesViewer).GetDerivedTypes();
		for (auto type : viewersTypes)
		{
			if (*type != TypeOf(DefaultAssetPropertiesViewer))
				mAvailableAssetViewers.Add((IAssetPropertiesViewer*)(type->CreateSample()));
		}

		InitializeHeadWidget();
	}

	AssetsPropertiesViewer::~AssetsPropertiesViewer()
	{
		for (auto viewer : mAvailableAssetViewers)
			delete viewer;
	}

	const Type* AssetsPropertiesViewer::GetViewingObjectType() const
	{
		return &TypeOf(Asset);
	}

	void AssetsPropertiesViewer::AddAssetViewer(IAssetPropertiesViewer* viewer)
	{
		mAvailableAssetViewers.Add(viewer);
	}

	void AssetsPropertiesViewer::SetTargets(const Vector<IObject*> targets)
	{
		mTargetAssets = targets.Cast<Asset*>();

		if (mTargetAssets.IsEmpty())
			return;

		UpdateCommonName();
		UpdateCommonIcon();

		bool typesAreSame = true;
		Type* commonType = &mTargetAssets[0]->GetType();
		for (int i = 1; i < mTargetAssets.Count(); i++)
		{
			Type* assetType = &mTargetAssets[i]->GetType();

			if (assetType != commonType)
			{
				typesAreSame = false;
				break;
			}
		}

		if (!typesAreSame)
		{
			if (mCurrentViewer)
				mContentWidget->RemoveChild(mCurrentViewer->GetWidget(), false);

			mCurrentViewer = nullptr;
			return;
		}

		IAssetPropertiesViewer* viewer = mAvailableAssetViewers.FindMatch([&](IAssetPropertiesViewer* x) {
			return commonType->IsBasedOn(*x->GetAssetType());
		});

		if (!viewer)
		{
			auto defaultViewer = mnew DefaultAssetPropertiesViewer();
			defaultViewer->SpecializeAssetType(&mTargetAssets[0]->GetMeta()->GetType());
			mAvailableAssetViewers.Add(defaultViewer);
			viewer = defaultViewer;
		}

		if (viewer != mCurrentViewer)
		{
			if (mCurrentViewer)
				mContentWidget->RemoveChild(mCurrentViewer->GetWidget(), false, false);

			mCurrentViewer = viewer;

			if (mCurrentViewer)
			{
				mCurrentViewer->SetTargetAssets(mTargetAssets);
				mContentWidget->AddChild(mCurrentViewer->GetWidget());
			}
		}
		else if (mCurrentViewer)
			mCurrentViewer->SetTargetAssets(mTargetAssets);
	}

	void AssetsPropertiesViewer::UpdateCommonName()
	{
		if (mTargetAssets.Count() != 1)
		{
			mAssetNameEdit->text = "";
			mAssetPathLabel->text = (String)mTargetAssets.Count() + " assets";

			return;
		}
		else
		{
			mAssetNameEdit->text = FileSystem::GetFileNameWithoutExtension(
				FileSystem::GetPathWithoutDirectories(mTargetAssets.Last()->GetPath()));

			mAssetPathLabel->text = mTargetAssets.Last()->GetPath();
		}
	}

	void AssetsPropertiesViewer::UpdateCommonIcon()
	{
		if (mTargetAssets.IsEmpty())
			return;

		Type& commonType = mTargetAssets[0]->GetType();
		bool iconsAreDifferent = commonType == TypeOf(ImageAsset) && mTargetAssets.Count() > 1;

		for (int i = 1; i < mTargetAssets.Count() && !iconsAreDifferent; i++)
		{
			Type& assetType = mTargetAssets[i]->GetType();
			if (assetType == TypeOf(ImageAsset) || assetType != commonType)
				iconsAreDifferent = true;
		}

		if (iconsAreDifferent)
			mAssetIcon->SetImageName("ui/UI2_big_file_icon.png");
		else
			mAssetIcon->SetImage(AssetsWindow::GetAssetIconSprite(mTargetAssets[0]));

		if (!iconsAreDifferent && commonType == TypeOf(ImageAsset))
		{
			Vec2F iconSize = mAssetIcon->GetImage()->GetOriginalSize();
			auto imageLayer = mAssetIcon->GetLayer("image");
			float previewMaxSize = mAssetIcon->layout.width;

			if (iconSize.x > iconSize.y)
			{
				float cf = iconSize.y / iconSize.x;
				imageLayer->layout = Layout::Based(BaseCorner::Center, Vec2F(previewMaxSize, previewMaxSize*cf));
			}
			else
			{
				float cf = iconSize.x / iconSize.y;
				imageLayer->layout = Layout::Based(BaseCorner::Center, Vec2F(previewMaxSize*cf, previewMaxSize));
			}
		}
		else
		{
			auto imageLayer = mAssetIcon->GetLayer("image");
			imageLayer->layout = Layout::Based(BaseCorner::Center, mAssetIcon->GetImage()->GetOriginalSize());
		}

		mAssetIcon->UpdateLayout();
	}

	void AssetsPropertiesViewer::OnEnabled()
	{}

	void AssetsPropertiesViewer::OnDisabled()
	{
		mTargetAssets.Clear();
	}

	void AssetsPropertiesViewer::Update(float dt)
	{}

	void AssetsPropertiesViewer::Draw()
	{}

	void AssetsPropertiesViewer::InitializeHeadWidget()
	{
		auto contentWidget = o2UI.CreateVerLayout();
		contentWidget->name          = "asset viewer layout";
		contentWidget->spacing       = 0.0f;
		contentWidget->border        = RectF();
		contentWidget->expandHeight  = false;
		contentWidget->expandWidth   = true;
		contentWidget->fitByChildren = true;
		contentWidget->baseCorner    = BaseCorner::Top;
		contentWidget->layout        = UIWidgetLayout::BothStretch();
		mContentWidget = contentWidget;

		mHeadWidget = mnew UIWidget();
		mHeadWidget->layout = UIWidgetLayout::HorStretch(VerAlign::Top, 0, 0, 50, 0);
		mHeadWidget->layout.minHeight = 50;
		mHeadWidget->AddLayer("separator", mnew Sprite("ui/UI_Separator.png"),
							  Layout::HorStretch(VerAlign::Bottom, -6, -7, 5, -2));

		mContentWidget->AddChild(mHeadWidget);

		mAssetIcon = o2UI.CreateImage("ui/UI_o2_sign.png");
		mAssetIcon->layout = UIWidgetLayout::Based(BaseCorner::Left, Vec2F(40, 40), Vec2F(8, 0));
		mHeadWidget->AddChild(mAssetIcon);

		mAssetNameEdit = o2UI.CreateEditBox("actorHeadName");
		mAssetNameEdit->layout = UIWidgetLayout::HorStretch(VerAlign::Top, 50, 10, 17, 5);
		mAssetNameEdit->onChangeCompleted = Function<void(const WString&)>(this, &AssetsPropertiesViewer::OnNameTextEditChanged);
		mHeadWidget->AddChild(mAssetNameEdit);

		mAssetPathLabel = o2UI.CreateLabel("asset/path/xxx/yyy");
		mAssetPathLabel->layout = UIWidgetLayout::HorStretch(VerAlign::Top, 57, 10, 20, 23);
		mAssetPathLabel->horAlign = HorAlign::Left;
		mHeadWidget->AddChild(mAssetPathLabel);
	}

	void AssetsPropertiesViewer::OnNameTextEditChanged(const WString& value)
	{
		if (mTargetAssets.Count() != 1)
			return;

		String ext = FileSystem::GetFileExtension(mTargetAssets.Last()->GetPath());

		if (ext.IsEmpty())
			o2Assets.RenameAsset(mTargetAssets.Last(), value);
		else
			o2Assets.RenameAsset(mTargetAssets.Last(), value + "." + ext);
	}
}
 
CLASS_META(Editor::AssetsPropertiesViewer)
{
	BASE_CLASS(Editor::IObjectPropertiesViewer);

	PROTECTED_FIELD(mHeadWidget);
	PROTECTED_FIELD(mAssetIcon);
	PROTECTED_FIELD(mAssetNameEdit);
	PROTECTED_FIELD(mRemoveButton);
	PROTECTED_FIELD(mAssetPathLabel);
	PROTECTED_FIELD(mTargetAssets);
	PROTECTED_FIELD(mCurrentViewer);
	PROTECTED_FIELD(mAvailableAssetViewers);

	PUBLIC_FUNCTION(const Type*, GetViewingObjectType);
	PUBLIC_FUNCTION(void, AddAssetViewer, IAssetPropertiesViewer*);
	PROTECTED_FUNCTION(void, SetTargets, const Vector<IObject*>);
	PROTECTED_FUNCTION(void, UpdateCommonName);
	PROTECTED_FUNCTION(void, UpdateCommonIcon);
	PROTECTED_FUNCTION(void, OnEnabled);
	PROTECTED_FUNCTION(void, OnDisabled);
	PROTECTED_FUNCTION(void, Update, float);
	PROTECTED_FUNCTION(void, Draw);
	PROTECTED_FUNCTION(void, InitializeHeadWidget);
	PROTECTED_FUNCTION(void, OnNameTextEditChanged, const WString&);
}
END_META;
 