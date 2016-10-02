#include "Texture.h"

ENUM_META(o2::Texture::Format)
{
	ENUM_ENTRY(Default);
	ENUM_ENTRY(R8G8B8);
	ENUM_ENTRY(R8G8B8A8);
}
END_ENUM_META;

ENUM_META(o2::Texture::Usage)
{
	ENUM_ENTRY(Default);
	ENUM_ENTRY(RenderTarget);
}
END_ENUM_META;
 