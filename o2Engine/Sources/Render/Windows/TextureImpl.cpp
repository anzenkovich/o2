#include "stdafx.h"

#ifdef PLATFORM_WINDOWS
#include "Render/Texture.h"
#include "Utils/Debug/Log/LogStream.h"

namespace o2
{
	Texture::~Texture()
	{
		o2Render.mTextures.Remove(this);

		for (auto texRef : mRefs)
			texRef->mTexture = nullptr;

		if (!mReady)
			return;

		if (mUsage == Usage::RenderTarget)
			glDeleteFramebuffersEXT(1, &mFrameBuffer);

		glDeleteTextures(1, &mHandle);
	}

	void Texture::Create(const Vec2I& size, PixelFormat format /*= Format::R8G8B8A8*/, Usage usage /*= Usage::Default*/)
	{
		if (mReady)
		{
			if (mUsage == Usage::RenderTarget)
				glDeleteFramebuffersEXT(1, &mFrameBuffer);

			glDeleteTextures(1, &mHandle);
		}

		mFormat = format;
		mUsage = usage;
		mSize = size;

		glGenTextures(1, &mHandle);
		glBindTexture(GL_TEXTURE_2D, mHandle);

		GLint texFormat = GL_RGB;
		if (format == PixelFormat::R8G8B8A8)
			texFormat = GL_RGBA;
		else if (format == PixelFormat::R8G8B8)
			texFormat = GL_RGB;

		glTexImage2D(GL_TEXTURE_2D, 0, texFormat, (GLsizei)size.x, (GLsizei)size.y, 0, texFormat, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		if (mUsage == Usage::RenderTarget)
		{
			glGenFramebuffersEXT(1, &mFrameBuffer);
			glBindFramebufferEXT(GL_FRAMEBUFFER, mFrameBuffer);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mHandle, 0);

			GLenum DrawBuffers[2] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, DrawBuffers);

			if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				GLenum glError = glGetError();

				o2Render.mLog->Error("Failed to create GL frame buffer object! GL Error %i %cs", glError,
									 GetGLErrorDesc(glError));

				mReady = false;
				return;
			}

			mReady = true;

			glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
		}

		mReady = true;
	}

	void Texture::Create(Bitmap* bitmap)
	{
		if (mReady)
		{
			if (mUsage == Usage::RenderTarget)
				glDeleteFramebuffersEXT(1, &mFrameBuffer);

			glDeleteTextures(1, &mHandle);
		}

		mFormat = bitmap->GetFormat();
		mUsage = Usage::Default;
		mSize = bitmap->GetSize();
		mFileName = bitmap->GetFilename();

		glGenTextures(1, &mHandle);
		glBindTexture(GL_TEXTURE_2D, mHandle);

		GLint texFormat = GL_RGB;
		if (mFormat == PixelFormat::R8G8B8A8)
			texFormat = GL_RGBA;
		else if (mFormat == PixelFormat::R8G8B8)
			texFormat = GL_RGB;

		glTexImage2D(GL_TEXTURE_2D, 0, texFormat, bitmap->GetSize().x, bitmap->GetSize().y, 0, texFormat, GL_UNSIGNED_BYTE,
					 bitmap->GetData());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		mReady = true;
	}

	void Texture::SetData(Bitmap* bitmap)
	{
		glBindTexture(GL_TEXTURE_2D, mHandle);

		GLint texFormat = GL_RGB;
		if (mFormat == PixelFormat::R8G8B8A8)
			texFormat = GL_RGBA;
		else if (mFormat == PixelFormat::R8G8B8)
			texFormat = GL_RGB;

		mSize = bitmap->GetSize();

		glTexImage2D(GL_TEXTURE_2D, 0, texFormat, bitmap->GetSize().x, bitmap->GetSize().y, 0, texFormat, GL_UNSIGNED_BYTE,
					 bitmap->GetData());

		GL_CHECK_ERROR();
	}

	void Texture::SetSubData(const Vec2I& offset, Bitmap* bitmap)
	{
		glBindTexture(GL_TEXTURE_2D, mHandle);

		GLint texFormat = GL_RGB;
		if (mFormat == PixelFormat::R8G8B8A8)
			texFormat = GL_RGBA;
		else if (mFormat == PixelFormat::R8G8B8)
			texFormat = GL_RGB;

		glTexSubImage2D(GL_TEXTURE_2D, 0, offset.x, offset.y, bitmap->GetSize().x, bitmap->GetSize().y, texFormat, GL_UNSIGNED_BYTE,
						bitmap->GetData());

		GL_CHECK_ERROR();
	}

	void Texture::Copy(const Texture& from, const RectI& rect)
	{
		glBindTexture(GL_TEXTURE_2D, from.mHandle);

		GLint texFormat = GL_RGB;
		if (mFormat == PixelFormat::R8G8B8A8)
			texFormat = GL_RGBA;
		else if (mFormat == PixelFormat::R8G8B8)
			texFormat = GL_RGB;

		glCopyTexImage2D(GL_TEXTURE_2D, 0, texFormat, rect.left, rect.top, rect.Width(), rect.Height(), 0);
	}

	Bitmap* Texture::GetData()
	{
		Bitmap* bitmap = mnew Bitmap(mFormat, mSize);

		glBindTexture(GL_TEXTURE_2D, mHandle);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->GetData());

		return bitmap;
	}

	void Texture::SetFilter(Filter filter)
	{
		mFilter = filter;

		GLint type = GL_LINEAR;
		if (mFilter == Filter::Nearest)
			type = GL_NEAREST;

		auto prevTextureHandle = o2Render.mLastDrawTexture ? o2Render.mLastDrawTexture->mHandle : 0;
		o2Render.DrawPrimitives();

		glBindTexture(GL_TEXTURE_2D, mHandle);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, type);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, type);

		glBindTexture(GL_TEXTURE_2D, prevTextureHandle);

		GL_CHECK_ERROR();
	}

	Texture::Filter Texture::GetFilter() const
	{
		return mFilter;
	}
}

#endif //PLATFORM_WINDOWS