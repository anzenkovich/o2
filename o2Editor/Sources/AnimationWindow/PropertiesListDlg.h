#pragma once

#include "Utils/Singleton.h"
#include "Scene/UI/Widgets/Tree.h"

namespace o2
{
	class Button;
	class EditBox;
	class Sprite;
	class Text;
	class Window;
}

using namespace o2;

namespace Editor
{
	class AnimationPropertiesTree;

	// ---------------------------------------------------------------------------------------
	// Animation properties list dialog. Shows properties tree with filter for specified actor
	// ---------------------------------------------------------------------------------------
	class PropertiesListDlg : public Singleton<PropertiesListDlg>
	{
	public:
		PropertiesListDlg();
		~PropertiesListDlg();

		// Shows animation properties window for actor and animation
		static void Show(Animation* animation, ActorRef actor);

	private:
		Window*  mWindow = nullptr;
		EditBox* mFilter = nullptr;

		AnimationPropertiesTree* mPropertiesTree = nullptr;

	private:
		// Initializes window and controls
		void InitializeWindow();
	};

	// ------------------------------------------------------------------------------------
	// Animation properties tree. Builds data tree by actor's properties, showing by filter
	// Can add or remove animation values
	// ------------------------------------------------------------------------------------
	class AnimationPropertiesTree : public Tree
	{
	public:
		struct NodeData
		{
			NodeData*         parent = nullptr;
			Vector<NodeData*> children;

			String name;
			String path;

			const Type* type = nullptr;

			bool used = false;

		public:
			~NodeData();

			void Clear();
			NodeData* AddChild(const String& name, const Type* type);
		};

	public:
		// Default constructor
		AnimationPropertiesTree();

		// Copy-constructor
		AnimationPropertiesTree(const AnimationPropertiesTree& other);

		// Copy operator
		AnimationPropertiesTree& operator=(const AnimationPropertiesTree& other);

		// Initializes properties
		void Initialize(Animation* animation, ActorRef actor);

		// Sets filter and refreshes tree
		void SetFilter(const WString& filter);

		SERIALIZABLE(AnimationPropertiesTree);

	private:
		WString mFilterStr; // Filtering string

		Animation* mAnimation = nullptr; // Looking animation
		ActorRef   mActor;               // Looking actor

		NodeData         mRoot;         // Root properties data node
		Vector<IObject*> mPassedObject; // Tree processing passed objects

	private:
		// Initializes parameters tree node by object properties
		void InitializeTreeNode(NodeData* node, IObject* object);

		// Processes object base types and fields
		void ProcessObject(void* object, const ObjectType* type, NodeData* node);

		// Processes tree node property. Checks type
		void ProcessTreeNode(void* object, const Type* type, const String& name, NodeData* node);

		// initializes single property node
		void InitializePropertyNode(NodeData* node, const String& name, const Type* type);

		// Initializes sub tree for object
		void InitializeObjectTreeNode(const ObjectType* fieldObjectType, void* object, const String& name, NodeData* node);

		// Updates visible nodes (calculates range and initializes nodes), enables editor mode
		void UpdateVisibleNodes() override;

		// Gets tree node from pool or creates new, enables editor mode
		TreeNode* CreateTreeNodeWidget() override;

		// Returns object's parent
		UnknownPtr GetObjectParent(UnknownPtr object) override;

		// Returns object's children
		Vector<UnknownPtr> GetObjectChilds(UnknownPtr object) override;

		// Returns debugging string for object
		String GetObjectDebug(UnknownPtr object) override;

		// Sets nodeWidget data by object
		void FillNodeDataByObject(TreeNode* nodeWidget, UnknownPtr object) override;

		// It is called when tree node was double clicked
		void OnNodeDblClick(TreeNode* nodeWidget) override;

		// It is called when list of selected objects was changed
		void OnNodesSelectionChanged(UnknownPtrsVec objects) override;

		friend class AnimationPropertiesTreeNode;
	};

	// -------------------------------------------------------------------------
	// Animation properties tree node. Shows icon, name and add or remove button
	// -------------------------------------------------------------------------
	class AnimationPropertiesTreeNode : public TreeNode
	{
	public:
		// Default constructor
		AnimationPropertiesTreeNode();

		// Copy-constructor
		AnimationPropertiesTreeNode(const AnimationPropertiesTreeNode& other);

		// Copy operator
		AnimationPropertiesTreeNode& operator=(const AnimationPropertiesTreeNode& other);

		// Initializes node by data
		void Setup(AnimationPropertiesTree::NodeData* data, AnimationPropertiesTree* tree);

		SERIALIZABLE(AnimationPropertiesTreeNode);

	private:
		Text*   mName;         // Name of property
		Sprite* mIcon;         // Property icon. Used only for finite properties
		Button* mAddButton;    // Add button, it is enabled when animation value isn't added to animation, adds this value to animation
		Button* mRemoveButton; // Remove button, it is enabled when animation value is added to animation, removes this value to animation

		AnimationPropertiesTree::NodeData* mData = nullptr; // Data node pointer

		AnimationPropertiesTree* mTree = nullptr; // Owner tree

	private:
		// Copies data of actor from other to this
		void CopyData(const Actor& otherActor) override;

		// It is called on deserialization, initializes controls
		void OnDeserialized(const DataNode& node) override;

		// initializes controls and widgets
		void InitializeControls();

		friend class AnimationPropertiesTree;
	};
}

CLASS_BASES_META(Editor::AnimationPropertiesTree)
{
	BASE_CLASS(o2::Tree);
}
END_META;
CLASS_FIELDS_META(Editor::AnimationPropertiesTree)
{
	PRIVATE_FIELD(mFilterStr);
	PRIVATE_FIELD(mAnimation);
	PRIVATE_FIELD(mActor);
	PRIVATE_FIELD(mRoot);
	PRIVATE_FIELD(mPassedObject);
}
END_META;
CLASS_METHODS_META(Editor::AnimationPropertiesTree)
{

	PUBLIC_FUNCTION(void, Initialize, Animation*, ActorRef);
	PUBLIC_FUNCTION(void, SetFilter, const WString&);
	PRIVATE_FUNCTION(void, InitializeTreeNode, NodeData*, IObject*);
	PRIVATE_FUNCTION(void, ProcessObject, void*, const ObjectType*, NodeData*);
	PRIVATE_FUNCTION(void, ProcessTreeNode, void*, const Type*, const String&, NodeData*);
	PRIVATE_FUNCTION(void, InitializePropertyNode, NodeData*, const String&, const Type*);
	PRIVATE_FUNCTION(void, InitializeObjectTreeNode, const ObjectType*, void*, const String&, NodeData*);
	PRIVATE_FUNCTION(void, UpdateVisibleNodes);
	PRIVATE_FUNCTION(TreeNode*, CreateTreeNodeWidget);
	PRIVATE_FUNCTION(UnknownPtr, GetObjectParent, UnknownPtr);
	PRIVATE_FUNCTION(Vector<UnknownPtr>, GetObjectChilds, UnknownPtr);
	PRIVATE_FUNCTION(String, GetObjectDebug, UnknownPtr);
	PRIVATE_FUNCTION(void, FillNodeDataByObject, TreeNode*, UnknownPtr);
	PRIVATE_FUNCTION(void, OnNodeDblClick, TreeNode*);
	PRIVATE_FUNCTION(void, OnNodesSelectionChanged, UnknownPtrsVec);
}
END_META;

CLASS_BASES_META(Editor::AnimationPropertiesTreeNode)
{
	BASE_CLASS(o2::TreeNode);
}
END_META;
CLASS_FIELDS_META(Editor::AnimationPropertiesTreeNode)
{
	PRIVATE_FIELD(mName);
	PRIVATE_FIELD(mIcon);
	PRIVATE_FIELD(mAddButton);
	PRIVATE_FIELD(mRemoveButton);
	PRIVATE_FIELD(mData);
	PRIVATE_FIELD(mTree);
}
END_META;
CLASS_METHODS_META(Editor::AnimationPropertiesTreeNode)
{

	PUBLIC_FUNCTION(void, Setup, AnimationPropertiesTree::NodeData*, AnimationPropertiesTree*);
	PRIVATE_FUNCTION(void, CopyData, const Actor&);
	PRIVATE_FUNCTION(void, OnDeserialized, const DataNode&);
	PRIVATE_FUNCTION(void, InitializeControls);
}
END_META;