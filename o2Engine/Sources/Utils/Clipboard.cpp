#include "stdafx.h"
#include "Clipboard.h"

#include <Windows.h>
#include <shlobj.h>

namespace o2
{

	void Clipboard::SetText(const WString& text)
	{
		if (OpenClipboard(NULL))
		{
			HGLOBAL hgBuffer;
			wchar_t* chBuffer;
			EmptyClipboard();
			hgBuffer = GlobalAlloc(GMEM_DDESHARE, (text.Length() + 1)*sizeof(wchar_t));
			chBuffer = (wchar_t*)GlobalLock(hgBuffer);
			memcpy(chBuffer, text.Data(), (text.Length() + 1)*sizeof(wchar_t));
			GlobalUnlock(hgBuffer);
			SetClipboardData(CF_UNICODETEXT, hgBuffer);
			CloseClipboard();
		}
	}

	WString Clipboard::GetText()
	{
		WString res;

		if (OpenClipboard(NULL))
		{
			HANDLE hData = GetClipboardData(CF_UNICODETEXT);
			wchar_t* chBuffer = (wchar_t*)GlobalLock(hData);
			res = chBuffer;
			GlobalUnlock(hData);
			CloseClipboard();
		}

		return res;
	}

#undef CopyFile

	void Clipboard::CopyFile(const WString& path)
	{
		if (OpenClipboard(NULL))
		{
			EmptyClipboard();

			int size = sizeof(DROPFILES) + (path.Length() + 2)*sizeof(WCHAR);
			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, size);
			DROPFILES *df = (DROPFILES*)GlobalLock(hGlobal);
			ZeroMemory(df, size);
			df->pFiles = sizeof(DROPFILES);
			df->fWide = TRUE;
			LPWSTR ptr = (LPWSTR)(df + 1);
			lstrcpyW(ptr, path.Data());
			GlobalUnlock(hGlobal);
			SetClipboardData(CF_HDROP, hGlobal);
			CloseClipboard();
		}
	}

	void Clipboard::CopyFiles(const Vector<WString>& paths)
	{
		if (OpenClipboard(NULL))
		{
			EmptyClipboard();

			int size = sizeof(DROPFILES) + (paths.Sum<int>([](const WString& x) { return x.Length() + 1; }) + 1)*sizeof(WCHAR);
			HGLOBAL hGlobal = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, size);
			DROPFILES *df = (DROPFILES*)GlobalLock(hGlobal);
			ZeroMemory(df, size);
			df->pFiles = sizeof(DROPFILES);
			df->fWide = TRUE;
			LPWSTR ptr = (LPWSTR)(df + 1);

			for (auto path : paths)
			{
				lstrcpyW(ptr, path.Data());
				ptr += path.Length();
				*ptr = '\0';
				ptr++;
			}

			*ptr = '\0';

			GlobalUnlock(hGlobal);
			SetClipboardData(CF_HDROP, hGlobal);
			CloseClipboard();
		}
	}

	Vector<WString> Clipboard::GetCopyFiles()
	{
		Vector<WString> res;

		if (OpenClipboard(NULL))
		{
			HANDLE hData = GetClipboardData(CF_HDROP);
			if (hData)
			{
				DROPFILES* df = (DROPFILES*)GlobalLock(hData);

				wchar_t* files = (wchar_t*)(df + 1);
				wchar_t buf[MAX_PATH];
				int bufLen = 0;
				int i = 0;

				while (true)
				{
					buf[bufLen++] = files[i];

					if (files[i] == '\0')
					{
						if (bufLen == 1)
							break;

						res.Add(buf);
						bufLen = 0;
					}

					i++;
				}

				GlobalUnlock(hData);
			}

			CloseClipboard();
		}

		return res;
	}

}