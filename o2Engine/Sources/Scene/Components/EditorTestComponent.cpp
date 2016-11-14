#include "EditorTestComponent.h"
 
 
 
CLASS_META(o2::EditorTestComponent)
{
	BASE_CLASS(o2::Component);

	PUBLIC_FIELD(mInteger).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mFloat).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mString).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mWString).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mBool).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mImageAsset).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mActorAsset).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mDataAsset).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mActor).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mTags).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mLayer).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mComponent).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mImageComponent).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mColor).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mVec2F).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mVec2I).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mVertex).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mRectF).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mRectI).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mCurve).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mTestInside).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mVecs).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mIntVector).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mTestInsideVector).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mActorVector).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mDictionary).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mFloat2).SERIALIZABLE_ATTRIBUTE();

	PUBLIC_FUNCTION(void, DeserializeBasic, const DataNode&, const void*);
}
END_META;

CLASS_META(o2::EditorTestComponent::TestInside)
{
	BASE_CLASS(o2::ISerializable);

	PUBLIC_FIELD(mFloat).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mString).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mWString).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mBool).SERIALIZABLE_ATTRIBUTE();
}
END_META;
 