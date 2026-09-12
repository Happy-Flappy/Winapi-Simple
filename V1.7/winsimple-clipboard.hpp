#ifndef WINSIMPLE_CLIPBOARD
#define WINSIMPLE_CLIPBOARD

#include <mutex>
#include <shellapi.h>
#include <shlobj.h>
#include <cstring>
#include <vector>
#include <string>
#include <queue>
#include <functional>

//Clipboard And DragNDrop Linking: -lole32 -luuid

//automated linking for visual studio MSVC
#ifdef _MSC_VER
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "uuid.lib")
#endif


namespace ws
{
	class ClipboardInit
	{
	public:
		bool alreadyInit = false;
		ClipboardInit()
		{
			HRESULT hr = OleInitialize(NULL);
			if (hr == S_FALSE)
				alreadyInit = true;					
		}
		~ClipboardInit()
		{
			if(!alreadyInit)
				OleUninitialize();
		}
	} clipboardInit;

	class ClipData
	{
	private:
		std::vector<std::string> files;
		std::string text = "";
		ws::Texture texture;
		
	public:
		ClipData() = default;
		
		void setTexture(ws::Texture tex) { texture = std::move(tex); }
		void setText(std::string str)    { text = std::move(str); }
		void setFiles(std::vector<std::string> filesVec) { files = std::move(filesVec); }
		
		ws::Texture getTexture() const { return texture; }
		std::string getText() const    { return text; }
		const std::vector<std::string>& getFiles() const { return files; }
	};


	static bool CopyDIBToTexture(LPVOID pDIB, ws::Texture& tex) {
		if (!pDIB) return false;

		BITMAPINFOHEADER* bih = (BITMAPINFOHEADER*)pDIB;
		DWORD offset = bih->biSize;
		if (bih->biBitCount <= 8) {
			offset += (1ULL << bih->biBitCount) * sizeof(RGBQUAD);
		}
		BYTE* bits = (BYTE*)pDIB + offset;

		int width  = bih->biWidth;
		int height = abs(bih->biHeight);  // negative height means top‑down DIB
		bool topDown = (bih->biHeight < 0);

		if (!tex.create(width, height))
			return false;

		BYTE* dst = (BYTE*)tex.getBITS();
		int dstStride = width * 4;  

		int srcStride = ((width * bih->biBitCount + 31) / 32) * 4;

		for (int y = 0; y < height; ++y) {
			int srcY = topDown ? y : (height - 1 - y);
			BYTE* srcRow = bits + srcY * srcStride;
			BYTE* dstRow = dst + y * dstStride;

			if (bih->biBitCount == 32) {
				memcpy(dstRow, srcRow, width * 4);
			} else if (bih->biBitCount == 24) {
				for (int x = 0; x < width; ++x) {
					dstRow[x*4 + 0] = srcRow[x*3 + 0]; // B
					dstRow[x*4 + 1] = srcRow[x*3 + 1]; // G
					dstRow[x*4 + 2] = srcRow[x*3 + 2]; // R
					dstRow[x*4 + 3] = 255;             // A
				}
			} 
			else 
			{
				return false;
			}
		}
		return true;
	}	

	
	class Clipboard
	{
		private:
		

		bool copyHBITMAPToTexture(HBITMAP hbm, ws::Texture& outTex) const
		{
			BITMAP bm;
			GetObject(hbm, sizeof(bm), &bm);
			
			BITMAPINFO bi = {};
			bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
			bi.bmiHeader.biWidth       = bm.bmWidth;
			bi.bmiHeader.biHeight      = -bm.bmHeight;
			bi.bmiHeader.biPlanes      = 1;
			bi.bmiHeader.biBitCount    = 32;
			bi.bmiHeader.biCompression = BI_RGB;
			
			HDC hdc = GetDC(NULL);
			GetDIBits(hdc, hbm, 0, bm.bmHeight, NULL, &bi, DIB_RGB_COLORS);
			
			std::vector<BYTE> bits(bi.bmiHeader.biSizeImage);
			if (!GetDIBits(hdc, hbm, 0, bm.bmHeight, bits.data(), &bi, DIB_RGB_COLORS)) {
				ReleaseDC(NULL, hdc);
				return false;
			}
			ReleaseDC(NULL, hdc);
			
			if (!outTex.create(bm.bmWidth, bm.bmHeight))
				return false;
			
			BYTE* dstBits = static_cast<BYTE*>(outTex.getBITS());
			int dstStride = bm.bmWidth * 4;
			memcpy(dstBits, bits.data(), dstStride * bm.bmHeight);
			return true;
		}
		
		HGLOBAL textureToDIB(const ws::Texture& tex) const
		{
			if (!tex.isValid() || !tex.isFastDIB())
				return nullptr;
			
			int width  = tex.getSize().x;
			int height = tex.getSize().y;
			
			BITMAPINFOHEADER bih = {};
			bih.biSize        = sizeof(BITMAPINFOHEADER);
			bih.biWidth       = width;
			bih.biHeight      = height;
			bih.biPlanes      = 1;
			bih.biBitCount    = 32;
			bih.biCompression = BI_RGB;
			bih.biSizeImage   = width * height * 4;
			
			DWORD totalSize = sizeof(BITMAPINFOHEADER) + bih.biSizeImage;
			HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, totalSize);
			if (!hGlobal) return nullptr;
			
			BYTE* pData = (BYTE*)GlobalLock(hGlobal);
			if (!pData) {
				GlobalFree(hGlobal);
				return nullptr;
			}
			
			memcpy(pData, &bih, sizeof(BITMAPINFOHEADER));
			
			BYTE* dstBits = pData + sizeof(BITMAPINFOHEADER);
			BYTE* srcBits = (BYTE*)tex.getBITS();
			int stride = width * 4;
			
			for (int y = 0; y < height; ++y) {
				BYTE* srcRow = srcBits + y * stride;
				BYTE* dstRow = dstBits + (height - 1 - y) * stride;
				memcpy(dstRow, srcRow, stride);
			}
			
			GlobalUnlock(hGlobal);
			return hGlobal;
		}
		
		Gdiplus::Bitmap* copyRectOfBitmap(ws::Texture &texture, ws::IntRect rect)
		{
			if (!texture.isValid()) return nullptr;
			
			int srcWidth = texture.bitmap->GetWidth();
			int srcHeight = texture.bitmap->GetHeight();
			
			int copyWidth, copyHeight, copyLeft, copyTop;
			if (rect.width == 0 && rect.height == 0 && rect.left == 0 && rect.top == 0) {
				copyLeft = 0;
				copyTop = 0;
				copyWidth = srcWidth;
				copyHeight = srcHeight;
			} else {
				copyLeft = rect.left;
				copyTop = rect.top;
				copyWidth = rect.width;
				copyHeight = rect.height;
				if (copyLeft < 0) copyLeft = 0;
				if (copyTop < 0) copyTop = 0;
				if (copyLeft + copyWidth > srcWidth) copyWidth = srcWidth - copyLeft;
				if (copyTop + copyHeight > srcHeight) copyHeight = srcHeight - copyTop;
				if (copyWidth <= 0 || copyHeight <= 0) return nullptr;
			}
			
			Gdiplus::Bitmap* copyBitmap = new Gdiplus::Bitmap(copyWidth, copyHeight, PixelFormat32bppARGB);
			Gdiplus::Graphics graphics(copyBitmap);
			graphics.DrawImage(texture.bitmap, 0, 0, copyLeft, copyTop, copyWidth, copyHeight, Gdiplus::UnitPixel);
			return copyBitmap;
		}
		
		bool OpenClipboardCheck()
		{
			return OpenClipboard(NULL) != FALSE;
		}
		
	public:
		Clipboard() = default;
		
		ClipData paste()
		{
			if (!OpenClipboardCheck())
				return ClipData();
			
			ClipData data;
			
			if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
				HANDLE hData = GetClipboardData(CF_UNICODETEXT);
				if (hData) {
					LPCWSTR pGlobal = (LPCWSTR)GlobalLock(hData);
					if (pGlobal) {
						data.setText(ws::SHORT(pGlobal));
						GlobalUnlock(hData);
					}
				}
			}
			
			data.setTexture(pasteTexture());
			data.setFiles(pasteFiles());
			
			CloseClipboard();
			return data;
		}
		
		bool copyText(const std::string& str)
		{
			const std::wstring text = ws::WIDE(str);
			if (!OpenClipboardCheck()) return false;
			EmptyClipboard();
			
			size_t length = (text.length() + 1) * sizeof(wchar_t);
			HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, length);
			if (!hGlobal) {
				CloseClipboard();
				return false;
			}
			LPWSTR pGlobal = (LPWSTR)GlobalLock(hGlobal);
			if (!pGlobal) {
				GlobalFree(hGlobal);
				CloseClipboard();
				return false;
			}
			wcsncpy_s(pGlobal, length / sizeof(wchar_t), text.c_str(), text.length());
			GlobalUnlock(hGlobal);
			
			SetClipboardData(CF_UNICODETEXT, hGlobal);
			CloseClipboard();
			return true;
		}
		
		bool copyFile(const std::string& filePath)
		{
			std::wstring widePath = ws::WIDE(filePath);
			if (!OpenClipboardCheck()) return false;
			EmptyClipboard();
			
			size_t pathSize = (widePath.length() + 1) * sizeof(wchar_t);
			size_t dropFilesSize = sizeof(DROPFILES) + pathSize + sizeof(wchar_t);
			
			HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, dropFilesSize);
			if (!hGlobal) {
				CloseClipboard();
				return false;
			}
			
			DROPFILES* pDropFiles = (DROPFILES*)GlobalLock(hGlobal);
			if (!pDropFiles) {
				GlobalFree(hGlobal);
				CloseClipboard();
				return false;
			}
			
			pDropFiles->pFiles = sizeof(DROPFILES);
			pDropFiles->pt = { 0, 0 };
			pDropFiles->fNC = FALSE;
			pDropFiles->fWide = TRUE;
			
			wchar_t* pFileList = (wchar_t*)((BYTE*)pDropFiles + sizeof(DROPFILES));
			wcscpy_s(pFileList, widePath.length() + 1, widePath.c_str());
			pFileList[widePath.length() + 1] = L'\0';
			
			GlobalUnlock(hGlobal);
			
			bool success = (SetClipboardData(CF_HDROP, hGlobal) != NULL);
			CloseClipboard();
			if (!success) GlobalFree(hGlobal);
			return success;
		}
		
		bool copyFiles(const std::vector<std::string>& filePaths)
		{
			if (filePaths.empty()) return false;
			if (!OpenClipboardCheck()) return false;
			EmptyClipboard();
			
			std::vector<std::wstring> widePaths;
			size_t totalPathsSize = 0;
			for (const auto& path : filePaths) {
				std::wstring widePath = ws::WIDE(path);
				widePaths.push_back(widePath);
				totalPathsSize += (widePath.length() + 1) * sizeof(wchar_t);
			}
			
			size_t dropFilesSize = sizeof(DROPFILES) + totalPathsSize + sizeof(wchar_t);
			HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, dropFilesSize);
			if (!hGlobal) {
				CloseClipboard();
				return false;
			}
			
			DROPFILES* pDropFiles = (DROPFILES*)GlobalLock(hGlobal);
			if (!pDropFiles) {
				GlobalFree(hGlobal);
				CloseClipboard();
				return false;
			}
			
			pDropFiles->pFiles = sizeof(DROPFILES);
			pDropFiles->pt = { 0, 0 };
			pDropFiles->fNC = FALSE;
			pDropFiles->fWide = TRUE;
			
			wchar_t* pFileList = (wchar_t*)((BYTE*)pDropFiles + sizeof(DROPFILES));
			size_t offset = 0;
			for (const auto& widePath : widePaths) {
				wcscpy_s(pFileList + offset, widePath.length() + 1, widePath.c_str());
				offset += widePath.length() + 1;
			}
			pFileList[offset] = L'\0';
			
			GlobalUnlock(hGlobal);
			
			bool success = (SetClipboardData(CF_HDROP, hGlobal) != NULL);
			CloseClipboard();
			if (!success) GlobalFree(hGlobal);
			return success;
		}
		
		bool copyTexture(ws::Texture &texture, ws::IntRect rect = {0,0,0,0})
		{
			if (!texture.isValid()) return false;
			
			ws::Texture* src = &texture;
			ws::Texture cropped;
			if (rect.width > 0 && rect.height > 0) {
				Gdiplus::Bitmap* croppedBitmap = copyRectOfBitmap(texture, rect);
				if (!croppedBitmap) return false;
				cropped.loadFromBitmapPlus(*croppedBitmap);
				delete croppedBitmap;
				if (!cropped.isValid()) return false;
				src = &cropped;
			}
			
			if (!OpenClipboardCheck()) return false;
			EmptyClipboard();
			
			bool success = false;
			HGLOBAL hDib = textureToDIB(*src);
			if (hDib && SetClipboardData(CF_DIB, hDib))
				success = true;
			else if (hDib)
				GlobalFree(hDib);
			
			CloseClipboard();
			return success;
		}
		
	private:
		ws::Texture pasteTexture(ws::IntRect rect = {0,0,0,0})
		{
			ws::Texture tex;
			if (!OpenClipboardCheck())
				return tex;
			
			if (IsClipboardFormatAvailable(CF_DIB)) {
				HANDLE hData = GetClipboardData(CF_DIB);
				if (hData) {
					LPVOID pData = GlobalLock(hData);
					if (pData) {
						CopyDIBToTexture(pData, tex);
						GlobalUnlock(hData);
					}
				}
			}
			else if (IsClipboardFormatAvailable(CF_DIBV5)) {
				HANDLE hData = GetClipboardData(CF_DIBV5);
				if (hData) {
					LPVOID pData = GlobalLock(hData);
					if (pData) {
						const BITMAPV5HEADER* bih = (const BITMAPV5HEADER*)pData;
						if (bih->bV5BitCount == 32 && bih->bV5Compression == BI_RGB) {
							CopyDIBToTexture(pData, tex);
						}
					}
					GlobalUnlock(hData);
				}
			}
			else if (IsClipboardFormatAvailable(CF_BITMAP)) {
				HBITMAP hbm = (HBITMAP)GetClipboardData(CF_BITMAP);
				if (hbm) {
					copyHBITMAPToTexture(hbm, tex);
				}
			}
			
			CloseClipboard();
			
			if (tex.isValid() && rect.width > 0 && rect.height > 0) {
				Gdiplus::Bitmap* croppedBitmap = copyRectOfBitmap(tex, rect);
				if (croppedBitmap) {
					tex.loadFromBitmapPlus(*croppedBitmap);
					delete croppedBitmap;
				}
			}
			return tex;
		}
		
	public:
		std::vector<std::string> pasteFiles()
		{
			std::vector<std::string> filePaths;
			if (!OpenClipboardCheck())
				return filePaths;
			
			if (IsClipboardFormatAvailable(CF_HDROP)) {
				HANDLE hData = GetClipboardData(CF_HDROP);
				if (hData) {
					HDROP hDrop = (HDROP)GlobalLock(hData);
					if (hDrop) {
						UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
						for (UINT i = 0; i < fileCount; i++) {
							UINT bufferSize = DragQueryFileW(hDrop, i, NULL, 0);
							if (bufferSize > 0) {
								std::wstring widePath(bufferSize + 1, L'\0');
								DragQueryFileW(hDrop, i, &widePath[0], bufferSize + 1);
								filePaths.push_back(ws::SHORT(widePath));
							}
						}
						GlobalUnlock(hData);
					}
				}
			}
			
			CloseClipboard();
			return filePaths;
		}
		
		std::string pasteFile()
		{
			auto files = pasteFiles();
			if (!files.empty()) return files[0];
			return "";
		}
		
		bool hasFiles()
		{
			if (!OpenClipboardCheck()) return false;
			bool has = IsClipboardFormatAvailable(CF_HDROP) != FALSE;
			CloseClipboard();
			return has;
		}
		
		bool hasText()
		{
			if (!OpenClipboardCheck()) return false;
			bool has = IsClipboardFormatAvailable(CF_UNICODETEXT) != FALSE;
			CloseClipboard();
			return has;
		}
		
		bool hasTexture()
		{
			if (!OpenClipboardCheck()) return false;
			bool has = (IsClipboardFormatAvailable(CF_DIB) != FALSE) ||
					   (IsClipboardFormatAvailable(CF_DIBV5) != FALSE) ||
					   (IsClipboardFormatAvailable(CF_BITMAP) != FALSE);
			CloseClipboard();
			return has;
		}
		
		bool clear()
		{
			if (!OpenClipboardCheck()) return false;
			bool success = EmptyClipboard() != FALSE;
			CloseClipboard();
			return success;
		}
		
	} clipboard;

	enum class DropEffect {
		None = DROPEFFECT_NONE,
		Copy = DROPEFFECT_COPY,
		Move = DROPEFFECT_MOVE,
		Link = DROPEFFECT_LINK
	};

	class DropTarget : public IDropTarget
	{
	private:
		std::mutex m_queueMutex;
		ws::Window *windowRef = nullptr;
		DWORD m_lastEffect = DROPEFFECT_NONE;
		
		enum class AcceptedType { None, Files, Images, Text };
		AcceptedType m_acceptedType = AcceptedType::None;
		
		struct TypeSetting {
			bool enabled = false;
			DWORD effect = DROPEFFECT_COPY;
			std::function<bool()> condition = [](){ return true; };
		} m_files, m_text, m_images;
		
		std::queue<std::pair<ws::ClipData, DWORD>> m_dropQueue;
		
		bool hasFiles(IDataObject* pData) const {
			if (!pData) return false;
			FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
			return SUCCEEDED(pData->QueryGetData(&fmt));
		}
		
		bool hasText(IDataObject* pData) const {
			if (!pData) return false;
			FORMATETC fmt = { CF_UNICODETEXT, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
			return SUCCEEDED(pData->QueryGetData(&fmt));
		}
		
		bool hasImage(IDataObject* pData) const {
			if (!pData) return false;
			
			FORMATETC fmt = { CF_DIB, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
			if (SUCCEEDED(pData->QueryGetData(&fmt))) return true;
			
			fmt.cfFormat = CF_DIBV5;
			if (SUCCEEDED(pData->QueryGetData(&fmt))) return true;
			
			static CLIPFORMAT pngFormat = (CLIPFORMAT)RegisterClipboardFormatW(L"PNG");
			if (pngFormat) {
				fmt.cfFormat = pngFormat;
				if (SUCCEEDED(pData->QueryGetData(&fmt))) return true;
			}
			
			static CLIPFORMAT fileGroupDescW = (CLIPFORMAT)RegisterClipboardFormatW(L"FileGroupDescriptorW");
			if (fileGroupDescW) {
				fmt.cfFormat = fileGroupDescW;
				if (SUCCEEDED(pData->QueryGetData(&fmt))) return true;
			}
			
			static CLIPFORMAT urlFormat = (CLIPFORMAT)RegisterClipboardFormatW(L"UniformResourceLocatorW");
			if (urlFormat) {
				fmt.cfFormat = urlFormat;
				fmt.tymed = TYMED_HGLOBAL;
				if (SUCCEEDED(pData->QueryGetData(&fmt))) return true;
			}
			
			fmt.cfFormat = CF_HDROP;
			fmt.tymed = TYMED_HGLOBAL;
			if (SUCCEEDED(pData->QueryGetData(&fmt))) {
			}
			
			return false;
		}
		
	public:
		void acceptType(std::string type, ws::DropEffect effect = ws::DropEffect::Copy)
		{
			DWORD dwEffect = static_cast<DWORD>(effect);
			if (type == "images") {
				m_images.enabled = true;
				m_images.effect = dwEffect;
				m_images.condition = [](){ return true; };
			}
			if (type == "text") {
				m_text.enabled = true;
				m_text.effect = dwEffect;
				m_text.condition = [](){ return true; };
			}
			if (type == "files") {
				m_files.enabled = true;
				m_files.effect = dwEffect;
				m_files.condition = [](){ return true; };
			}
		}
		
		void rejectType(std::string type)
		{
			if (type == "images") m_images.enabled = false;
			if (type == "text")   m_text.enabled = false;
			if (type == "files")  m_files.enabled = false;
		}
		
		void rejectAll()
		{
			m_files.enabled = m_text.enabled = m_images.enabled = false;
			m_files.effect = m_text.effect = m_images.effect = static_cast<DWORD>(ws::DropEffect::Copy);
		}
		
		void onlyAcceptIf(std::string droptype, std::function<bool()> function)
		{
			if (droptype == "images") {
				m_images.enabled = true;
				m_images.condition = function;
			}
			if (droptype == "text") {
				m_text.enabled = true;
				m_text.condition = function;
			}
			if (droptype == "files") {
				m_files.enabled = true;
				m_files.condition = function;
			}
		}
		
		bool pollDrop(ws::ClipData& outData, DropEffect& outEffect)
		{
			if (!windowRef) return false;
			std::lock_guard<std::mutex> lock(m_queueMutex);
			if (m_dropQueue.empty()) return false;
			auto& front = m_dropQueue.front();
			outData = std::move(front.first);
			outEffect = static_cast<DropEffect>(front.second);
			m_dropQueue.pop();
			return true;
		}
		
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override
		{
			if (!windowRef) return S_OK;
			if (riid == IID_IUnknown || riid == IID_IDropTarget) {
				*ppv = this;
				AddRef();
				return S_OK;
			}
			*ppv = nullptr;
			return E_NOINTERFACE;
		}
		ULONG STDMETHODCALLTYPE AddRef()  override { return 1; }
		ULONG STDMETHODCALLTYPE Release() override { return 1; }
		
		HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override
		{
			if (!windowRef) return S_OK;
			m_lastEffect = DROPEFFECT_NONE;
			m_acceptedType = AcceptedType::None;
			
			if (m_images.enabled && hasImage(pDataObj)) {
				m_acceptedType = AcceptedType::Images;
				if (!m_images.condition || m_images.condition())
					m_lastEffect = m_images.effect;
			}
			else if (m_files.enabled && hasFiles(pDataObj)) {
				m_acceptedType = AcceptedType::Files;
				if (!m_files.condition || m_files.condition())
					m_lastEffect = m_files.effect;
			}
			else if (m_text.enabled && hasText(pDataObj)) {
				m_acceptedType = AcceptedType::Text;
				if (!m_text.condition || m_text.condition())
					m_lastEffect = m_text.effect;
			}
			
			*pdwEffect = m_lastEffect;
			
			ws::Cursor c;
			if (m_lastEffect & DROPEFFECT_COPY)
				c.loadAs(ws::Cursor::Type::Copy);
			else if (m_lastEffect & DROPEFFECT_MOVE)
				c.loadAs(ws::Cursor::Type::Move);
			else
				c.loadAs(ws::Cursor::Type::Arrow);
			windowRef->setCursor(c);
			
			return S_OK;
		}
		
		HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override
		{
			if (!windowRef) return S_OK;
			DWORD newEffect = DROPEFFECT_NONE;
			
			switch (m_acceptedType) {
				case AcceptedType::Files:
					if (!m_files.condition || m_files.condition())
						newEffect = m_files.effect;
					break;
				case AcceptedType::Images:
					if (!m_images.condition || m_images.condition())
						newEffect = m_images.effect;
					break;
				case AcceptedType::Text:
					if (!m_text.condition || m_text.condition())
						newEffect = m_text.effect;
					break;
				default: break;
			}
			
			*pdwEffect = newEffect;
			m_lastEffect = newEffect;
			
			ws::Cursor c;
			if (m_lastEffect & DROPEFFECT_COPY)
				c.loadAs(ws::Cursor::Type::Copy);
			else if (m_lastEffect & DROPEFFECT_MOVE)
				c.loadAs(ws::Cursor::Type::Move);
			else
				c.loadAs(ws::Cursor::Type::Arrow);
			windowRef->setCursor(c);
			
			return S_OK;
		}
		
		HRESULT STDMETHODCALLTYPE DragLeave() override
		{
			if (!windowRef) return S_OK;
			m_acceptedType = AcceptedType::None;
			ws::Cursor c;
			c.loadAs(ws::Cursor::Type::Arrow);
			windowRef->setCursor(c);
			return S_OK;
		}
		
		HRESULT STDMETHODCALLTYPE Drop(IDataObject* obj, DWORD, POINTL, DWORD* e) override
		{
			if (!windowRef) return S_OK;
			*e = m_lastEffect;
			
			ws::ClipData data;
			
			FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
			STGMEDIUM stg = {};
			if (SUCCEEDED(obj->GetData(&fmt, &stg))) {
				HDROP hDrop = (HDROP)GlobalLock(stg.hGlobal);
				UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
				std::vector<std::string> files;
				for (UINT i = 0; i < count; i++) {
					wchar_t path[MAX_PATH] = {};
					DragQueryFileW(hDrop, i, path, MAX_PATH);
					files.push_back(ws::SHORT(path));
				}
				data.setFiles(files);
				GlobalUnlock(stg.hGlobal);
				ReleaseStgMedium(&stg);
			}
			
			FORMATETC descFmt = { (CLIPFORMAT)RegisterClipboardFormatW(L"FileGroupDescriptorW"), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
			STGMEDIUM descStg = {};
			if (SUCCEEDED(obj->GetData(&descFmt, &descStg))) {
				FILEGROUPDESCRIPTORW* fgd = (FILEGROUPDESCRIPTORW*)GlobalLock(descStg.hGlobal);
				if (fgd && fgd->cItems > 0) {
					GlobalUnlock(descStg.hGlobal);
					ReleaseStgMedium(&descStg);
					
					// Request both ISTREAM and HGLOBAL so the source picks whichever it supports.
					// NEVER call pstm->Read without first confirming tymed == TYMED_ISTREAM:
					// if a source returns S_OK with TYMED_HGLOBAL, pstm holds the raw HGLOBAL
					// value misread as a vtable pointer -> instant access violation -> COM
					// catches the SEH fault, Drop() never reaches the queue push, nothing received.
					FORMATETC contentFmt = { (CLIPFORMAT)RegisterClipboardFormatW(L"FileContents"), nullptr, DVASPECT_CONTENT, 0, TYMED_ISTREAM | TYMED_HGLOBAL };
					STGMEDIUM contentStg = {};
					if (SUCCEEDED(obj->GetData(&contentFmt, &contentStg))) {
						if (contentStg.tymed == TYMED_ISTREAM && contentStg.pstm) {
							std::vector<BYTE> bytes;
							BYTE buf[4096];
							ULONG bytesRead;
							while (SUCCEEDED(contentStg.pstm->Read(buf, sizeof(buf), &bytesRead)) && bytesRead > 0)
								bytes.insert(bytes.end(), buf, buf + bytesRead);
							if (!bytes.empty()) {
								ws::Texture tex;
								if (tex.loadFromMemory(bytes.data(), bytes.size()))
									data.setTexture(tex);
							}
						} else if (contentStg.tymed == TYMED_HGLOBAL && contentStg.hGlobal) {
							LPVOID pData = GlobalLock(contentStg.hGlobal);
							SIZE_T size = GlobalSize(contentStg.hGlobal);
							if (pData && size > 0) {
								ws::Texture tex;
								if (tex.loadFromMemory(pData, size))
									data.setTexture(tex);
							}
							GlobalUnlock(contentStg.hGlobal);
						}
						ReleaseStgMedium(&contentStg);
					}
				} else {
					GlobalUnlock(descStg.hGlobal);
					ReleaseStgMedium(&descStg);
				}
			}
			
			if (!data.getTexture().isValid()) {
				FORMATETC pngFmt = { (CLIPFORMAT)RegisterClipboardFormatW(L"PNG"), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
				STGMEDIUM pngStg = {};
				if (SUCCEEDED(obj->GetData(&pngFmt, &pngStg))) {
					LPVOID pData = GlobalLock(pngStg.hGlobal);
					SIZE_T size = GlobalSize(pngStg.hGlobal);
					if (pData && size > 0) {
						ws::Texture tex;
						if (tex.loadFromMemory(pData, size))
							data.setTexture(tex);
					}
					GlobalUnlock(pngStg.hGlobal);
					ReleaseStgMedium(&pngStg);
				}
			}
			
			if (!data.getTexture().isValid()) {
				CLIPFORMAT dibFormats[] = { CF_DIBV5, CF_DIB };
				for (CLIPFORMAT cf : dibFormats) {
					FORMATETC dibFmt = { cf, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
					STGMEDIUM dibStg = {};
					if (SUCCEEDED(obj->GetData(&dibFmt, &dibStg))) {
						LPVOID pData = GlobalLock(dibStg.hGlobal);
						if (pData) {
							ws::Texture tex;
							if (CopyDIBToTexture(pData, tex))
								data.setTexture(tex);
							GlobalUnlock(dibStg.hGlobal);
						}
						ReleaseStgMedium(&dibStg);
						if (data.getTexture().isValid()) break;
					}
				}
			}
			
			FORMATETC textFmt = { CF_UNICODETEXT, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
			STGMEDIUM textStg = {};
			if (SUCCEEDED(obj->GetData(&textFmt, &textStg))) {
				LPCWSTR p = (LPCWSTR)GlobalLock(textStg.hGlobal);
				if (p) data.setText(ws::SHORT(p));
				GlobalUnlock(textStg.hGlobal);
				ReleaseStgMedium(&textStg);
			}
			
			if (data.getText().empty()) {
				FORMATETC urlFmt = { (CLIPFORMAT)RegisterClipboardFormatW(L"UniformResourceLocatorW"), nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
				STGMEDIUM urlStg = {};
				if (SUCCEEDED(obj->GetData(&urlFmt, &urlStg))) {
					LPCWSTR p = (LPCWSTR)GlobalLock(urlStg.hGlobal);
					if (p) data.setText(ws::SHORT(p));
					GlobalUnlock(urlStg.hGlobal);
					ReleaseStgMedium(&urlStg);
				}
			}
			
			std::lock_guard<std::mutex> lock(m_queueMutex);
			m_dropQueue.push({ std::move(data), m_lastEffect });
			
			return S_OK;
		}
		
		void setWindow(ws::Window &window)
		{
			RegisterDragDrop(window.hwnd, this);
			windowRef = &window;
			
			ChangeWindowMessageFilterEx(window.hwnd, WM_DROPFILES,    MSGFLT_ALLOW, nullptr);
			ChangeWindowMessageFilterEx(window.hwnd, WM_COPYDATA,     MSGFLT_ALLOW, nullptr);
			ChangeWindowMessageFilterEx(window.hwnd, 0x0049,          MSGFLT_ALLOW, nullptr);
		}
		
		~DropTarget()
		{
			if (windowRef && windowRef->hwnd)
				RevokeDragDrop(windowRef->hwnd);
		}
	};
}

#endif