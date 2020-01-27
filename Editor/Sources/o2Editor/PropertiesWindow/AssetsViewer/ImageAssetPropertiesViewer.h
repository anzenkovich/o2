#pragma once

#include "o2/Assets/AtlasAsset.h"
#include "o2/Assets/ImageAsset.h"
#include "o2Editor/Core/Properties/Basic/AssetProperty.h"
#include "o2/Events/CursorEventsArea.h"
#include "o2Editor/PropertiesWindow/AssetsViewer/IAssetPropertiesViewer.h"
#include "o2/Scene/UI/Widgets/Image.h"
#include "o2/Utils/Editor/DragHandle.h"

using namespace o2;

namespace o2
{
	class Sprite;
	class VerticalLayout;
}

namespace Editor
{
	class BorderIProperty;
	class EnumProperty;
	class ObjectProperty;

	class ImageAssetPropertiesViewer: public IAssetPropertiesViewer
	{
	public:
		// Default constructor
		ImageAssetPropertiesViewer();

		// Destructor
		~ImageAssetPropertiesViewer();

		// Sets target actors
		void SetTargetAssets(const Vector<AssetRef*>& assets);

		// Returns viewing asset type 
		const Type* GetAssetType() const;

		// Returns data widget
		Widget* GetWidget() const;

		IOBJECT(ImageAssetPropertiesViewer);

	protected:
		class PreviewImage : public Image
		{
		public:
			// Sets texture filter to nearest and draws it
			void Draw() override;

			SERIALIZABLE(PreviewImage);
		};

	protected:
		Vector<ImageAssetRef*> mTargetAssets;
								      
		VerticalLayout* mContent = nullptr;
								      
		Widget*       mPreviewImageContent;
		PreviewImage* mPreviewImage = nullptr;
		Image*        mPreviewImageBack = nullptr;
								      
		WidgetDragHandle* mBorderLeftHandle;
		WidgetDragHandle* mBorderRightHandle;
		WidgetDragHandle* mBorderTopHandle;
		WidgetDragHandle* mBorderBottomHandle;
		BorderF           mBordersSmoothValue;

		AssetProperty<AtlasAssetRef>* mAtlasProperty = nullptr;

		BorderIProperty* mBorderProperty = nullptr;
		EnumProperty*    mDefaultTypeProperty = nullptr;
		ObjectProperty*  mWindowsProperties = nullptr;
		ObjectProperty*  mOSXProperties = nullptr;
		ObjectProperty*  mAndroidProperties = nullptr;
		ObjectProperty*  mIOSProperties = nullptr;

	protected:
		// Initializes image preview widgets and border handles
		void InitializeImagePreview();

		// Initializes image slices handles
		void InitializeSliceHandles();

		// Initializes other properties
		void InitializeProperties();

		// Fits image by size
		void FitImage();

		// Updates borders anchors layouts
		void UpdateBordersAnchors();

		// Updates targets assets borders values by floored mBordersSmoothValue
		void UpdateBordersValue();

		// Sets common value for atlas property
		void SetupAtlasProperty();

		// It is called when atlas property was changed
		void OnAtlasPropertyChanged(IPropertyField* field);

		// Creates grid sprite
		Sprite* CreateGridSprite();
	};
}

CLASS_BASES_META(Editor::ImageAssetPropertiesViewer)
{
	BASE_CLASS(Editor::IAssetPropertiesViewer);
}
END_META;
CLASS_FIELDS_META(Editor::ImageAssetPropertiesViewer)
{
	PROTECTED_FIELD(mTargetAssets);
	PROTECTED_FIELD(mContent);
	PROTECTED_FIELD(mPreviewImageContent);
	PROTECTED_FIELD(mPreviewImage);
	PROTECTED_FIELD(mPreviewImageBack);
	PROTECTED_FIELD(mBorderLeftHandle);
	PROTECTED_FIELD(mBorderRightHandle);
	PROTECTED_FIELD(mBorderTopHandle);
	PROTECTED_FIELD(mBorderBottomHandle);
	PROTECTED_FIELD(mBordersSmoothValue);
	PROTECTED_FIELD(mAtlasProperty);
	PROTECTED_FIELD(mBorderProperty);
	PROTECTED_FIELD(mDefaultTypeProperty);
	PROTECTED_FIELD(mWindowsProperties);
	PROTECTED_FIELD(mOSXProperties);
	PROTECTED_FIELD(mAndroidProperties);
	PROTECTED_FIELD(mIOSProperties);
}
END_META;
CLASS_METHODS_META(Editor::ImageAssetPropertiesViewer)
{

	PUBLIC_FUNCTION(void, SetTargetAssets, const Vector<AssetRef*>&);
	PUBLIC_FUNCTION(const Type*, GetAssetType);
	PUBLIC_FUNCTION(Widget*, GetWidget);
	PROTECTED_FUNCTION(void, InitializeImagePreview);
	PROTECTED_FUNCTION(void, InitializeSliceHandles);
	PROTECTED_FUNCTION(void, InitializeProperties);
	PROTECTED_FUNCTION(void, FitImage);
	PROTECTED_FUNCTION(void, UpdateBordersAnchors);
	PROTECTED_FUNCTION(void, UpdateBordersValue);
	PROTECTED_FUNCTION(void, SetupAtlasProperty);
	PROTECTED_FUNCTION(void, OnAtlasPropertyChanged, IPropertyField*);
	PROTECTED_FUNCTION(Sprite*, CreateGridSprite);
}
END_META;

CLASS_BASES_META(Editor::ImageAssetPropertiesViewer::PreviewImage)
{
	BASE_CLASS(o2::Image);
}
END_META;
CLASS_FIELDS_META(Editor::ImageAssetPropertiesViewer::PreviewImage)
{
}
END_META;
CLASS_METHODS_META(Editor::ImageAssetPropertiesViewer::PreviewImage)
{

	PUBLIC_FUNCTION(void, Draw);
}
END_META;
