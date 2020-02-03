#pragma once

#include "o2/Assets/Asset.h"
#include "o2/Render/FontRef.h"

namespace o2
{
	class Font;

	// ----------
	// Font asset
	// ----------
	class FontAsset : public Asset
	{
	public:
		PROPERTIES(FontAsset);
		GETTER(FontRef, font, GetFont);   // Font getter

	public:
		// Check equals operator
		FontAsset& operator=(const FontAsset& asset);

		// Returns font pointer
		virtual FontRef GetFont() const;

		SERIALIZABLE(FontAsset);

	protected:
		FontRef mFont;

	protected:
		// Default constructor
		FontAsset();

		// Copy-constructor
		FontAsset(const FontAsset& asset);

		friend class Assets;
	};

	typedef Ref<FontAsset> FontAssetRef;
}

CLASS_BASES_META(o2::FontAsset)
{
	BASE_CLASS(o2::Asset);
}
END_META;
CLASS_FIELDS_META(o2::FontAsset)
{
	PUBLIC_FIELD(font);
	PROTECTED_FIELD(mFont);
}
END_META;
CLASS_METHODS_META(o2::FontAsset)
{

	PUBLIC_FUNCTION(FontRef, GetFont);
}
END_META;
