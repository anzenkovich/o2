#pragma once

#include "Core/Properties/IObjectPropertiesViewer.h"
#include "Core/Properties/FieldPropertiesInfo.h"

namespace Editor
{
	// ------------------------------------------------------------
	// Default object properties viewer. Builds field by reflection
	// ------------------------------------------------------------
	class DefaultObjectViewer : public IObjectPropertiesViewer
	{
	public:
		// Default constructor, creates view widget as vertical layout
		DefaultObjectViewer();

		// Refreshing controls and properties by target objects
		void Refresh(const TargetsVec& targetObjets) override;

		// Returns viewing objects type
		const Type* GetViewingObjectType() const override;

		IOBJECT(DefaultObjectViewer);

	private:
		const Type*           mRealObjectType = nullptr; // Specialized viewing object type
		FieldPropertiesInfo   mFieldProperties; // Field properties information
	};
}

CLASS_BASES_META(Editor::DefaultObjectViewer)
{
	BASE_CLASS(Editor::IObjectPropertiesViewer);
}
END_META;
CLASS_FIELDS_META(Editor::DefaultObjectViewer)
{
	PRIVATE_FIELD(mRealObjectType);
	PRIVATE_FIELD(mFieldProperties);
}
END_META;
CLASS_METHODS_META(Editor::DefaultObjectViewer)
{

	PUBLIC_FUNCTION(void, Refresh, const TargetsVec&);
	PUBLIC_FUNCTION(const Type*, GetViewingObjectType);
}
END_META;
