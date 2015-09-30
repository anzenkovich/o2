#include "FieldInfo.h"

#include "Utils/Reflection/Type.h"

namespace o2
{
	FieldInfo::FieldInfo():
		mOffset(0), mIsProperty(false), mType(nullptr), mIsPtr(false)
	{
	}

	FieldInfo::FieldInfo(const String& name, UInt offset, bool isProperty, bool isPtr, Type* type):
		mName(name), mOffset(offset), mIsProperty(isProperty), mType(type), mIsPtr(isPtr)
	{
	}

	FieldInfo::FieldInfo(const FieldInfo& other):
		mName(other.mName), mOffset(other.mOffset), mIsProperty(other.mIsProperty), mType(other.mType),
		mIsPtr(mIsPtr)
	{
		for (auto attr : other.mAttributes)
		{
			IAttribute* newAttr = attr->Clone();
			newAttr->mOwnerFieldInfo = this;
			mAttributes.Add(newAttr);
		}
	}

	FieldInfo::~FieldInfo()
	{
		for (auto attr : mAttributes)
			delete attr;
	}

	FieldInfo& FieldInfo::operator=(const FieldInfo& other)
	{
		for (auto attr : mAttributes)
			delete attr;

		for (auto attr : other.mAttributes)
		{
			IAttribute* newAttr = attr->Clone();
			newAttr->mOwnerFieldInfo = this;
			mAttributes.Add(newAttr);
		}

		mName = other.mName;
		mOffset = other.mOffset;
		mIsProperty = other.mIsProperty;
		mIsPtr = other.mIsPtr;
		mType = other.mType;

		return *this;
	}

	bool FieldInfo::operator==(const FieldInfo& other) const
	{
		return mName == other.mName && mOffset == other.mOffset && mIsProperty == other.mIsProperty &&
			mAttributes == other.mAttributes;
	}

	const String& FieldInfo::Name() const
	{
		return mName;
	}

	bool FieldInfo::IsProperty() const
	{
		return mIsProperty;
	}

	const Vector<IAttribute*>& FieldInfo::Attributes() const
	{
		return mAttributes;
	}

}