#pragma once

#include "ImageComponent.h"
#include "o2/Assets/Asset.h"
#include "o2/Assets/Types/ActorAsset.h"
#include "o2/Assets/Types/AnimationAsset.h"
#include "o2/Assets/Types/DataAsset.h"
#include "o2/Assets/Types/ImageAsset.h"
#include "o2/Render/Sprite.h"
#include "o2/Scene/Component.h"
#include "o2/Scene/Tags.h"
#include "o2/Utils/Editor/Attributes/DontDeleteAttribute.h"
#include "o2/Utils/Editor/Attributes/InvokeOnChangeAttribute.h"
#include "o2/Utils/Math/Curve.h"

namespace o2
{
	class EditorTestComponent: public Component
	{
	public:
		Vector<Vec2I> mVecs; // @SERIALIZABLE @INVOKE_ON_CHANGE(Test)

		enum class TestEnum { A, B, C, D, E, F, G, H, K, L, M, N };
		 
		class TestInside: public ISerializable
		{
		public:
			float mFloat;	  // @SERIALIZABLE
			String mString;	  // @SERIALIZABLE
			WString mWString; // @SERIALIZABLE
			bool mBool;		  // @SERIALIZABLE

			bool operator==(const TestInside& other) const { return false; }

			SERIALIZABLE(TestInside);
		};

		void SetSpritePtr(Sprite* sprite) { mSprite = sprite; }
		Sprite* GetSpritePtr() const { return mSprite; }

		void SetSprite(const Sprite& sprite) { *mSprite = sprite; }
		Sprite GetSprite() const { return *mSprite; }

		void SetArray(const Vector<Vec2I>& arr) { mVecs = arr; }
		const Vector<Vec2I>& GetArray() const { return mVecs; }

		PROPERTIES(EditorTestComponent);
		PROPERTY(Sprite*, spritePropPtr, SetSpritePtr, GetSpritePtr);
		PROPERTY(Sprite, spriteProp, SetSprite, GetSprite);
		PROPERTY(Vector<Vec2I>, arr, SetArray, GetArray);

		int mInteger;					           // @SERIALIZABLE @INVOKE_ON_CHANGE(Test)
		float mFloat;					           // @SERIALIZABLE
		String mString;					           // @SERIALIZABLE
		WString mWString;				           // @SERIALIZABLE
		bool mBool;						           // @SERIALIZABLE
		ImageAssetRef mImageAsset;			       // @SERIALIZABLE
		ActorAssetRef mActorAsset;			       // @SERIALIZABLE
		DataAssetRef mDataAsset;                   // @SERIALIZABLE
		AnimationAssetRef mAnimationAsset;         // @SERIALIZABLE
		Sprite spritex;                            // @SERIALIZABLE
		Sprite* mSprite = mnew Sprite();           // @SERIALIZABLE @DONT_DELETE
		Actor* mActor = nullptr;                   // @SERIALIZABLE
		TagGroup mTags;                            // @SERIALIZABLE
		SceneLayer* mLayer;                        // @SERIALIZABLE
		Component* mComponent = nullptr;           // @SERIALIZABLE
		ImageComponent* mImageComponent = nullptr; // @SERIALIZABLE
		Color4 mColor;					           // @SERIALIZABLE
		Vec2F mVec2F;					           // @SERIALIZABLE
		Vec2I mVec2I;					           // @SERIALIZABLE
		Vertex2 mVertex;				           // @SERIALIZABLE
		RectF mRectF;					           // @SERIALIZABLE
		RectI mRectI;					           // @SERIALIZABLE
		BorderF mBorderF;					       // @SERIALIZABLE
		BorderI mBorderI;					       // @SERIALIZABLE
		Curve mCurve = Curve::EaseInOut();         // @SERIALIZABLE
		TestInside mTestInside;                    // @SERIALIZABLE
		TestEnum mTestEnum;                        // @SERIALIZABLE
		TestInside* mTestInsidePtr = nullptr;      // @SERIALIZABLE

		Vector<int> mIntVector; // @SERIALIZABLE
		Vector<TestInside> mTestInsideVector;      // @SERIALIZABLE @INVOKE_ON_CHANGE(Test)
		Vector<TestInside*> mTestInsideptrsVector;      // @SERIALIZABLE @INVOKE_ON_CHANGE(Test)
		Vector<Actor*> mActorVector;               // @SERIALIZABLE
		Vector<AnimationAssetRef> mAssetsVector;               // @SERIALIZABLE

		Vector<Vector<TestInside*>> mVectorOfVector; // @SERIALIZABLE

		Map<String, String> mDictionary;    // @SERIALIZABLE
		float mFloat2;					           // @SERIALIZABLE
		float mFloat3;					           // @SERIALIZABLE
		float mFloat4;					           // @SERIALIZABLE
		float mFloat5;					           // @SERIALIZABLE
		float mFloat6;					           // @SERIALIZABLE
		float mFloat7;					           // @SERIALIZABLE
		float mFloat8;					           // @SERIALIZABLE
		float mFloat9;					           // @SERIALIZABLE

		static String GetCategory();

		void Test();

		SERIALIZABLE(EditorTestComponent);
	};
}

PRE_ENUM_META(o2::EditorTestComponent::TestEnum);

CLASS_BASES_META(o2::EditorTestComponent)
{
	BASE_CLASS(o2::Component);
}
END_META;
CLASS_FIELDS_META(o2::EditorTestComponent)
{
	PUBLIC_FIELD(mVecs).INVOKE_ON_CHANGE_ATTRIBUTE(Test).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(spritePropPtr);
	PUBLIC_FIELD(spriteProp);
	PUBLIC_FIELD(arr);
	PUBLIC_FIELD(mInteger).INVOKE_ON_CHANGE_ATTRIBUTE(Test).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mFloat).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mString).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mWString).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mBool).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mImageAsset).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mActorAsset).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mDataAsset).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mAnimationAsset).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(spritex).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mSprite).DEFAULT_VALUE(mnew Sprite()).DONT_DELETE_ATTRIBUTE().SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mActor).DEFAULT_VALUE(nullptr).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mTags).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mLayer).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mComponent).DEFAULT_VALUE(nullptr).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mImageComponent).DEFAULT_VALUE(nullptr).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mColor).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mVec2F).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mVec2I).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mVertex).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mRectF).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mRectI).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mBorderF).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mBorderI).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mCurve).DEFAULT_VALUE(Curve::EaseInOut()).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mTestInside).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mTestEnum).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mTestInsidePtr).DEFAULT_VALUE(nullptr).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mIntVector).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mTestInsideVector).INVOKE_ON_CHANGE_ATTRIBUTE(Test).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mTestInsideptrsVector).INVOKE_ON_CHANGE_ATTRIBUTE(Test).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mActorVector).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mAssetsVector).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mVectorOfVector).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mDictionary).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mFloat2).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mFloat3).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mFloat4).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mFloat5).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mFloat6).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mFloat7).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mFloat8).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mFloat9).SERIALIZABLE_ATTRIBUTE();
}
END_META;
CLASS_METHODS_META(o2::EditorTestComponent)
{

	PUBLIC_FUNCTION(void, SetSpritePtr, Sprite*);
	PUBLIC_FUNCTION(Sprite*, GetSpritePtr);
	PUBLIC_FUNCTION(void, SetSprite, const Sprite&);
	PUBLIC_FUNCTION(Sprite, GetSprite);
	PUBLIC_FUNCTION(void, SetArray, const Vector<Vec2I>&);
	PUBLIC_FUNCTION(const Vector<Vec2I>&, GetArray);
	PUBLIC_STATIC_FUNCTION(String, GetCategory);
	PUBLIC_FUNCTION(void, Test);
}
END_META;

CLASS_BASES_META(o2::EditorTestComponent::TestInside)
{
	BASE_CLASS(o2::ISerializable);
}
END_META;
CLASS_FIELDS_META(o2::EditorTestComponent::TestInside)
{
	PUBLIC_FIELD(mFloat).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mString).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mWString).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mBool).SERIALIZABLE_ATTRIBUTE();
}
END_META;
CLASS_METHODS_META(o2::EditorTestComponent::TestInside)
{
}
END_META;
