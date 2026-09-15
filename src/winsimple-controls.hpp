
#ifndef WINSIMPLE_CONTROLS
#define WINSIMPLE_CONTROLS

#include "winsimple.hpp"

#include <commctrl.h>   // for common controls (trackbar, etc.)
#include <shlobj.h>     // for folder browser (BROWSEINFO, etc.)
#include <filesystem>
#include <shellapi.h>

//Controls Linking: -lcomctl32 -lcomdlg32

//automated linking for visual studio MSVC
#ifdef _MSC_VER
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "uuid.lib")
#pragma comment(lib, "shell32.lib")
#endif

namespace ws
{
	//this is the real notify handler that ws::Window will point to now that this header is included.
    LRESULT handleNotifyForChildren(Window* window, NMHDR* pnmh, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	class ControlsInit
	{
	public:
		int maxControlID = 1000;
		
		ControlsInit();
		
		~ControlsInit();
	};

	class Child
	{
	public:
		
		using DestructorCallback = std::function<void(Child*)>;
		
		HWND hwnd = NULL;
		DWORD style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
		DWORD exStyle = 0;                     // extended style
		DWORD textStyle = DT_CENTER | DT_VCENTER | DT_SINGLELINE;

		unsigned int controlID = 0;
		COLORREF backgroundColor = RGB(0,0,0);
		COLORREF textColor = RGB(255,255,255);
		COLORREF borderColor = RGB(0,0,0);
		
		Child(const std::wstring& className = L"Button");
		
		virtual ~Child();

		// init – called by ws::Window::addChild()
		virtual bool init(ws::Window& parent);
		
		virtual bool init(HWND phwnd);

		void registerDestructorCallback(DestructorCallback callback);
		void clearDestructorCallbacks();

		void addChild(ws::Child& child);

		void removeChild(ws::Child& child);
		
		bool hasChild(ws::Child& child);

		void setClass(const std::wstring& className = L"Button");
		
		std::string getClass();
		
		void setVisible(bool visible);
		
		void setPosition(int xPos, int yPos);
		
		void setPosition(ws::Vec2i pos);
		
		ws::Vec2i getPosition() const;

		virtual void setSize(int w, int h);
		
		virtual void setSize(ws::Vec2i size);
		
		ws::Vec2i getSize() const;
		
		HFONT getFontHandle();

		void addStyle(DWORD addedStyle);
		
		void removeStyle(DWORD removedStyle);
		
		bool hasStyle(DWORD checkStyle) const;

		void addExStyle(DWORD addedStyle);
		
		void removeExStyle(DWORD removedStyle);
		
		bool hasExStyle(DWORD checkStyle) const;

		void setText(const std::string& newText);
		
		std::string getText() const;

		void setFont(ws::Font& font, ws::Text& textSettings);

		bool contains(ws::Vec2i point) const;

		// For message handling – derived classes can override if needed
		virtual bool handleCommand(MSG& msg);
		virtual bool handleNotify(NMHDR* pnmh);

	private:
		HFONT customFont = nullptr;
		std::string text = "";
		int x = 0, y = 0, width = 100, height = 100;
		std::string m_className;

		std::vector<ws::Child*> children;

		
		std::vector<DestructorCallback> m_destructorCallbacks;

		HWND m_currentParent = nullptr;

	};
	
	class Tabs : public ws::Child
	{
	private:
		struct PendingPage
		{
			std::string title = "";
			ws::Child *child = nullptr;
		};
		
	public:
		
		Tabs();
		
		virtual bool init(ws::Window& parent) override;
		
		void addPage(const std::string& title, ws::Child& page);

		void removePage(ws::Child& child);
		
		bool hasPage(ws::Child& child);
		
		void setSelected(int index);

		int getSelected() const;

		virtual void setSize(int w, int h) override;
		
		virtual bool handleNotify(NMHDR* pnmh) override;

		void updatePagePositions();

	private:
		std::vector<ws::Child*> pages;
		std::vector<PendingPage> pendingPages;
		int selected = 0;
	};

	class ComboBox : public Child
	{
	public:
		ComboBox();
		
		virtual bool init(ws::Window& parent) override;
		
		void addItem(const std::string& item);
	    
		void addItems(const std::vector<std::string>& items);
	    
		void removeItem(int index);
	    
		void clear();
	    
		int getSelectedIndex();
	    
		void setSelectedIndex(int index);
	    
		std::string getSelectedText();
	    
		int getItemCount();
	    
		std::string getItemText(int index);

		bool selectionChanged(MSG& msg);
	    
		void setDropdownStyle(bool allowEdit = true);
	    
		std::string getEditText();
	    
		void setEditText(const std::string& text);
	private:
		std::vector<std::string> pendingItems;
	};
	
	class Button : public Child
	{
	public:
		Button();

		virtual bool init(ws::Window& parent) override;

		bool isPressed(MSG& msg);
	};
	
	class Slider : public Child
	{
	public:
		
		Slider();
		
		virtual bool init(ws::Window& parent) override;
	    
		void setHorizontal();
		
		void setVertical();
        
		void setRange(int minimum = 0, int maximum = 100);
		
		void setSlidePosition(int pos = 0);
		
		int getSlidePosition();
		
	private:
		int slidePos = 0;
		int storedMin = 0;
		int storedMax = 100;
		bool shouldSetRange = false;
		bool shouldSetPos = false;
		
	};

	class ScrollBar : public ws::Child
	{
	public:
		
		ScrollBar(bool vertical = true);

		void setVertical();
		
		void setHorizontal();

		void setRange(int minVal, int maxVal);

		void setPageSize(int page);

		void setSlidePos(int pos);

		int getSlidePos();

		virtual bool init(ws::Window& parent) override;
	};

	class TextBox : public Child
	{
	public:
		
		int char_limit = 0;

		TextBox();
		
		virtual bool init(ws::Window& parent) override;
		
		void setCharacterLimit(int max_chars = 0);
		
		bool getFocus() const;
		
	};
	
	class Label : public Child
	{
	public:
		Label();
		
		virtual bool init(ws::Window& parent) override;
	};

	class ColorDialog
	{
	public:

		void init(ws::Window& newParent);

		ws::Window* getParent();

		void addFlag(DWORD newFlag);
		
		void removeFlag(DWORD removeFlag);
		
		DWORD getFlags();

		void setInitColor(ws::Hue hue);

		ws::Hue getInitColor();

		ws::Hue getResult();

		
		bool open();
	private:
		COLORREF customColors[16];
		COLORREF initColor = RGB(0, 0, 0);
		ws::Hue chosenColor = ws::Hue::black;
		ws::Window* parentRef = nullptr;
		// these flags tell it to show the full dialog and use the initial color.	
		DWORD flags = CC_FULLOPEN | CC_RGBINIT;
	};
	
	class Dropdown
	{
	public:
	    
		Dropdown(int newID, std::string newName);
	    
		void addItem(int id, DWORD type, std::string itemName) const;
	    
		void addSubmenu(Dropdown& submenu) const;
		
		HMENU getHandle() const;
		
		std::string getName() const;
		
		int getID()const;
		
		void addItem(Dropdown drop) const;
		
		
	private:
	    HMENU handle = nullptr;
	    int ID = 0;
	    std::string name = "";
	    bool isPopup = false;
	}; 
	
	class Menu
	{
	public:
		HMENU bar;

		Menu();
		
		void addDropdown(ws::Dropdown& drop);
		
		void setVisible(bool visible, ws::Window& window);
		
		int getEvent(MSG& m);
		
		ws::Window& getWindow();
	private:
		ws::View* oldView = nullptr;
		ws::Vec2f oldSize = { 0,0 };
		ws::Window* windowRef = nullptr;
	};
	
	class ClickMenu
	{
	public:
		
		void addFlag(DWORD newFlag);
		
		void removeFlag(DWORD removeFlag);
		
		DWORD getFlags() const;
		
		int getResult();
		
		std::vector<std::string> getList();
		
		void setList(std::vector<std::string> newList);
		
		void addItem(std::string item);
		
		void removeItem(std::string item);
		
		void init(ws::Window& newParent);
		
		ws::Window* getParent();
		
		bool open(ws::Vec2i mouse);
		
	private:
		int command = 0;	
		std::vector<std::string> list;	
		ws::Window *parentRef = nullptr;
		DWORD flags = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD;
	};
	
	class FileWindow
	{
	public:
	    
		FileWindow();

		void setInitResult(std::string file);
	    
		std::string getResult();
	    
		void setTitle(std::string name);
	    
		std::string getTitle();
	    
		void addFlag(DWORD newFlag);
	    
		void removeFlag(DWORD removeFlag);
	    
		DWORD getFlags() const;
	    
		bool open(ws::Window* parent = nullptr);
	    
		bool save(ws::Window* parent = nullptr);
	    
	private:
		DWORD flags;
	    std::string title;    
	    int defaultFilter;
	    std::string fileName; 
	    
	    //Temporary buffer
	    wchar_t szFile[MAX_PATH];
	};
	
	class FolderWindow
	{
	public:
	    
		FolderWindow();
	    
		void setTitle(std::string name);
	    
		std::string getTitle();
	    
		void addFlag(DWORD flag);
	    
		void setFlags(DWORD allFlags);
	    
		void removeFlag(DWORD flag);
	    
		DWORD getFlags();
	    
		std::string getResult();
	    
		bool open(ws::Window* parent = nullptr);
	    
		void setInitResult(std::string folder);
	    
	    private:
	    std::string title = "Select Folder";
	    DWORD flags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	    std::string folderName = "";
	    std::string initialFolder = "";
	    
	    // callback function for setting initial folder
		static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
	};
	
	class ExploreWindow
	{
	public:
		
		ExploreWindow(std::string mode = "open");
		
		
		void addStyle(DWORD style);
		void removeStyle(DWORD style);
		
		void setTitle(std::string title);
		void setInitResult(std::string folder);
		
		std::string getResult() const;
		
		bool open(ws::Window* parent = nullptr);
	private:
		std::string m_title, m_initialFolder, m_resultName;
		DWORD options;
		HRESULT hr;
		IFileDialog* pfd = nullptr;
	};

    class ListBox : public Child 
	{
	public:
        
		ListBox();
        
		virtual bool init(ws::Window& parent) override;
		
		void addItem(const std::string& item);

		void addItems(const std::vector<std::string>& items);

		void removeItem(int index);
        
		void clear();
		
		int getSelectedIndex();
		
		std::string getSelectedText();
		
		std::string getItemText(int index);
        
		void setSelectedIndex(int index);
        
		bool selectionChanged(MSG& msg);
        
		int getItemCount();

	private:
		std::vector<std::string> pendingItems;
    };	

	bool Balloon(ws::Window& window, std::string message, std::string title, HICON hIcon, DWORD messageIconType = NIIF_USER, int timeoutMilliseconds = 5000, DWORD styles = NIF_ICON | NIF_TIP | NIF_INFO);
	
	class TrayIcon
	{
	public:
		bool init(ws::Window& window, std::string title, HICON hIcon, DWORD styles = NIF_ICON | NIF_MESSAGE | NIF_TIP);
		bool isPressed(int button);

		bool showBalloon(const std::string& message, const std::string& title, DWORD infoFlags = NIIF_USER, int timeoutMs = 5000) const;

	private:
		int myID = -1;
		NOTIFYICONDATA m_nid;
		std::shared_ptr<MSG> msgPtr;
	};

#ifdef WINSIMPLE_CONTROLS_IMPL
	ControlsInit::ControlsInit() {
		INITCOMMONCONTROLSEX icex;
		//This is for initialization of winapi child objects sucg as buttons and textboxes.
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_STANDARD_CLASSES | ICC_TAB_CLASSES;  // Enables a set of common controls.
		InitCommonControlsEx(&icex);
		///////////////////////////////

		//used for FolderWindowModern
		HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(hr)) {
			MessageBoxA(NULL, "Winsimple Failed to call CoInitializeEx() in winsimple-controls!", "Winsimple Failure!", MB_OK);
		}

		ws::Window::s_handleNotifyForChildren = &handleNotifyForChildren;
	}

	ControlsInit::~ControlsInit() {
		CoUninitialize();
	}

	ControlsInit controlsInit;

	Child::Child(const std::wstring& className) : m_className(ws::toUTF8(className)) {
		controlID = controlsInit.maxControlID++;
	}


	Child::~Child() {
		for (auto& cb : m_destructorCallbacks)
			cb(this);

		if (hwnd && IsWindow(hwnd))
			DestroyWindow(hwnd);
		if (customFont)
			DeleteObject(customFont);
	}

	// init – called by ws::Window::addChild()
	bool Child::init(ws::Window& parent) {
		return init(parent.hwnd);
	}

	bool Child::init(HWND phwnd) {
		if (!phwnd) return false;


		if (hwnd && IsWindow(hwnd)) {
			HWND cParent = GetParent(hwnd);
			if (cParent && cParent != phwnd) {
				ws::log("Error: Child already parented to a different window. Remove it from the current container first.\n");
				MessageBoxA(NULL, "Error: Child already parented to a different window. Remove it from the current container first.\n", "Error", MB_OK | MB_ICONINFORMATION);
				return false;
			}

			DestroyWindow(hwnd);
			// force processing of the destruction message
			MSG msg;
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			hwnd = nullptr;
		}

		hwnd = CreateWindowExA(
			exStyle,
			m_className.c_str(),
			text.c_str(),
			style,
			x, y, width, height,
			phwnd,
			(HMENU)(UINT_PTR)controlID,
			GetModuleHandle(nullptr),
			nullptr
		);

		if (!hwnd) {
			ws::log("Failed to create child control: " + m_className);
			return false;
		}

		// Apply stored font if any
		if (customFont)
			SendMessage(hwnd, WM_SETFONT, (WPARAM)customFont, TRUE);

		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);

		for (int a = 0; a < children.size(); a++) {
			if (!children[a]->hwnd)
				children[a]->init(this->hwnd);
			if (GetParent(children[a]->hwnd) != this->hwnd)
				children[a]->init(this->hwnd);
		}

		return true;
	}

	void Child::registerDestructorCallback(DestructorCallback callback) {
		m_destructorCallbacks.push_back(std::move(callback));
	}
	void Child::clearDestructorCallbacks() {
		m_destructorCallbacks.clear();
	}

	void Child::addChild(ws::Child& child) {
		children.push_back(&child);


		if (this->hwnd)
			child.init(this->hwnd);
	}

	void Child::removeChild(ws::Child& child) {
		for (size_t a = 0; a < children.size(); a++) {
			if (&child == children[a]) {
				children.erase(children.begin() + a);
				break;
			}
		}
	}

	bool Child::hasChild(ws::Child& child) {
		for (size_t a = 0; a < children.size(); a++) {
			if (&child == children[a]) {
				return true;
			}
		}
		return false;
	}

	void Child::setClass(const std::wstring& className) {
		m_className = ws::toUTF8(className);
		if (hwnd) {
			init(GetParent(hwnd));
		}
	}

	std::string Child::getClass() {
		return m_className;
	}

	void Child::setVisible(bool visible) {
		ShowWindow(hwnd, (visible) ? SW_SHOW : SW_HIDE);
	}

	void Child::setPosition(int xPos, int yPos) {
		x = xPos;
		y = yPos;
		if (hwnd)
			SetWindowPos(hwnd, nullptr, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	void Child::setPosition(ws::Vec2i pos) {
		setPosition(pos.x, pos.y);
	}

	ws::Vec2i Child::getPosition() const {
		return { x, y };
	}

	void Child::setSize(int w, int h) {
		width = w;
		height = h;
		if (hwnd)
			SetWindowPos(hwnd, nullptr, x, y, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	void Child::setSize(ws::Vec2i size) {
		setSize(size.x, size.y);
	}

	ws::Vec2i Child::getSize() const {
		return { width, height };
	}

	HFONT Child::getFontHandle() {
		return customFont;
	}

	void Child::addStyle(DWORD addedStyle) {
		style |= addedStyle;
		if (hwnd) {
			SetWindowLong(hwnd, GWL_STYLE, style);
			SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
	}

	void Child::removeStyle(DWORD removedStyle) {
		style &= ~removedStyle;
		if (hwnd) {
			SetWindowLong(hwnd, GWL_STYLE, style);
			SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
	}

	bool Child::hasStyle(DWORD checkStyle) const {
		DWORD current = GetWindowLong(hwnd, GWL_STYLE);
		return (current & checkStyle) != 0;
	}

	void Child::addExStyle(DWORD addedStyle) {
		exStyle |= addedStyle;
		if (hwnd) {
			SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
			SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
	}

	void Child::removeExStyle(DWORD removedStyle) {
		exStyle &= ~removedStyle;
		if (hwnd) {
			SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
			SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
	}

	bool Child::hasExStyle(DWORD checkStyle) const {
		DWORD current = GetWindowLong(hwnd, GWL_EXSTYLE);
		return (current & checkStyle) != 0;
	}

	void Child::setText(const std::string& newText) {
		text = newText;
		if (hwnd)
			SetWindowTextA(hwnd, text.c_str());
	}

	std::string Child::getText() const {
		if (!hwnd)
			return text;
		int len = GetWindowTextLengthW(hwnd);
		if (len == 0)
			return "";
		std::wstring wbuf(len + 1, L'\0');
		GetWindowTextW(hwnd, &wbuf[0], len + 1);
		wbuf.resize(len); return ws::toUTF8(wbuf);
	}

	void Child::setFont(ws::Font& font, ws::Text& textSettings) {
		if (!font.isValid()) return;

		if (customFont) {
			DeleteObject(customFont);
			customFont = NULL;
		}

		Gdiplus::Font* gdipFont = font.getFontHandle();
		if (!gdipFont) return;

		Gdiplus::FontFamily family;
		gdipFont->GetFamily(&family);

		WCHAR familyName[LF_FACESIZE];
		family.GetFamilyName(familyName);

		int style = textSettings.getStyle();
		bool isBold = (style & Gdiplus::FontStyleBold) != 0;
		bool isItalic = (style & Gdiplus::FontStyleItalic) != 0;
		bool isUnderline = (style & Gdiplus::FontStyleUnderline) != 0;
		bool isStrikeout = (style & Gdiplus::FontStyleStrikeout) != 0;

		int heightInPixels = textSettings.getCharacterSize();

		customFont = CreateFontW(
			-heightInPixels,
			0,
			0,
			0,
			isBold ? FW_BOLD : FW_NORMAL,
			isItalic ? TRUE : FALSE,
			isUnderline ? TRUE : FALSE,
			isStrikeout ? TRUE : FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE,
			familyName
		);

		if (customFont && hwnd) {
			SendMessage(hwnd, WM_SETFONT, (WPARAM)customFont, TRUE);
		}
	}

	bool Child::contains(ws::Vec2i point) const {
		return (point.x >= x && point.x < x + width && point.y >= y && point.y < y + height);
	}

	// For message handling – derived classes can override if needed
	bool Child::handleCommand(MSG& msg) { return false; }
	bool Child::handleNotify(NMHDR* pnmh) { return false; }

	void ws::Window::addChild(ws::Child& child) {
		//prevent duplicate child addition
		if (std::find(children.begin(), children.end(), &child) != children.end())
			return;

		children.push_back(&child);

		child.registerDestructorCallback([this](Child* dyingChild) {
			auto it = std::find(children.begin(), children.end(), dyingChild);
			if (it != children.end())
				children.erase(it);
			});

		child.init(*this);
	}

	void ws::Window::removeChild(ws::Child& child) {
		for (size_t a = 0; a < children.size(); a++) {
			if (&child == children[a]) {
				children.erase(children.begin() + a);
				break;
			}
		}
	}

	bool ws::Window::hasChild(ws::Child& child) {
		for (size_t a = 0; a < children.size(); a++) {
			if (&child == children[a]) {
				return true;
			}
		}
		return false;
	}

	LRESULT handleNotifyForChildren(Window* window, NMHDR* pnmh, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		for (Child* child : window->children) {
			if (child && child->handleNotify(pnmh))
				return 0;
		}
		return DefWindowProc(window->hwnd, uMsg, wParam, lParam);
	}

	Tabs::Tabs() {
		setClass(L"SysTabControl32");
		addStyle(TCS_FIXEDWIDTH | TCS_RIGHTJUSTIFY | WS_CLIPSIBLINGS);
	}

	bool Tabs::init(ws::Window& parent)  {
		if (!Child::init(parent))
			return false;

		if (!getFontHandle()) {
			ws::Font font;
			font.loadFromSystem("Segoe UI");
			ws::Text text;
			text.setCharacterSize(14);
			setFont(font, text);
		}

		for (auto& pending : pendingPages) {
			addPage(pending.title, *pending.child);
		}
		pendingPages.clear();

		updatePagePositions();

		if (!pages.empty())
			setSelected(selected);

		return true;
	}

	void Tabs::addPage(const std::string& title, ws::Child& page) {
		if (!hwnd) {
			PendingPage p;
			p.title = title;
			p.child = &page;
			pendingPages.push_back(p);
			return;
		}

		HWND parentHwnd = GetParent(hwnd);
		if (!page.hwnd)
			page.init(parentHwnd);
		else
			SetParent(page.hwnd, parentHwnd);

		page.setVisible(false);
		pages.push_back(&page);


		TCITEM tie = { 0 };
		tie.mask = TCIF_TEXT;

		std::wstring wtitle = ws::toUTF16(title);
		tie.pszText = const_cast<LPWSTR>(wtitle.c_str());
		TabCtrl_InsertItem(hwnd, (int)pages.size() - 1, &tie);

		// Reposition all pages to fit the current display area
		updatePagePositions();

		if (pages.size() == 1)
			setSelected(0);
	}

	void Tabs::removePage(ws::Child& child) {
		for (size_t a = 0; a < pages.size(); a++) {
			if (&child == pages[a]) {
				pages.erase(pages.begin() + a);
				break;
			}
		}
	}

	bool Tabs::hasPage(ws::Child& child) {
		for (size_t a = 0; a < pages.size(); a++) {
			if (&child == pages[a]) {
				return true;
			}
		}
		return false;
	}

	void Tabs::setSelected(int index) {
		if (!hwnd) {
			selected = index;
			return;
		}
		if (index < 0 || index >= (int)pages.size())
			return;

		TabCtrl_SetCurSel(hwnd, index);

		for (int i = 0; i < (int)pages.size(); ++i)
			pages[i]->setVisible(i == index);

		updatePagePositions();
	}

	int Tabs::getSelected() const {
		return TabCtrl_GetCurSel(hwnd);
	}

	void Tabs::setSize(int w, int h) {
		Child::setSize(w, h);
		updatePagePositions();
	}

	bool Tabs::handleNotify(NMHDR* pnmh) {
		if (pnmh->hwndFrom == hwnd && pnmh->code == TCN_SELCHANGE) {
			int newSel = TabCtrl_GetCurSel(hwnd);

			for (int a = 0; a < pages.size(); a++)
				pages[a]->setVisible((a == newSel));


			return true;
		}
		return false;      // not handled, let parent process
	}

	void Tabs::updatePagePositions() {
		if (!hwnd) return;

		HWND parentHwnd = GetParent(hwnd);
		if (!parentHwnd) return;

		RECT rcTab;
		GetWindowRect(hwnd, &rcTab);

		MapWindowPoints(HWND_DESKTOP, parentHwnd, (LPPOINT)&rcTab, 2);

		RECT rcDisplay = rcTab;
		MapWindowPoints(parentHwnd, hwnd, (LPPOINT)&rcDisplay, 2);
		TabCtrl_AdjustRect(hwnd, FALSE, &rcDisplay);
		MapWindowPoints(hwnd, parentHwnd, (LPPOINT)&rcDisplay, 2);

		// position each page to exactly fill that display area
		for (auto* page : pages) {
			if (page && page->hwnd) {
				SetWindowPos(page->hwnd, NULL,
					rcDisplay.left, rcDisplay.top,
					rcDisplay.right - rcDisplay.left,
					rcDisplay.bottom - rcDisplay.top,
					SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}
	}

	ComboBox::ComboBox() {
		setClass(L"COMBOBOX");
		addStyle(CBS_DROPDOWN);
		addStyle(WS_VSCROLL);
	}

	bool ComboBox::init(ws::Window& parent) {
		if (!Child::init(parent)) return false;

		if (!getFontHandle()) {
			ws::Font font;
			font.loadFromSystem("Arial");
			ws::Text text;
			text.setCharacterSize(15);
			setFont(font, text);
		}
		// set extended UI for better appearance
		SendMessage(hwnd, CB_SETEXTENDEDUI, (WPARAM)TRUE, 0);

		for (const auto& item : pendingItems) {
			addItem(item);
		}
		pendingItems.clear();

		return true;
	}

	void ComboBox::addItem(const std::string& item) {
		if (!hwnd) {
			// store for later if hwnd doesn't exist yet
			pendingItems.push_back(item);
			return;
		}
		SendMessageA(hwnd, CB_ADDSTRING, 0, (LPARAM)item.c_str());
	}

	void ComboBox::addItems(const std::vector<std::string>& items) {
		if (!hwnd) {
			// store for later if hwnd doesn't exist yet
			for (size_t a = 0; a < items.size(); a++)
				pendingItems.push_back(items[a]);
			return;
		}
		for (const auto& item : items) {
			addItem(item);
		}
	}

	void ComboBox::removeItem(int index) {
		if (!hwnd) return;
		SendMessage(hwnd, CB_DELETESTRING, (WPARAM)index, 0);
	}

	void ComboBox::clear() {
		if (!hwnd) return;
		SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
	}

	int ComboBox::getSelectedIndex() {
		if (!hwnd) return -1;
		return (int)SendMessage(hwnd, CB_GETCURSEL, 0, 0);
	}

	void ComboBox::setSelectedIndex(int index) {
		if (!hwnd) return;
		SendMessage(hwnd, CB_SETCURSEL, (WPARAM)index, 0);
	}

	std::string ComboBox::getSelectedText() {
		if (!hwnd) return "";

		int index = getSelectedIndex();
		if (index == -1) return "";

		int length = (int)SendMessage(hwnd, CB_GETLBTEXTLEN, (WPARAM)index, 0);
		if (length == CB_ERR) return "";

		std::vector<char> buffer(length + 1);
		SendMessageA(hwnd, CB_GETLBTEXT, (WPARAM)index, (LPARAM)buffer.data());

		return std::string(buffer.data());
	}

	int ComboBox::getItemCount() {
		if (!hwnd) return 0;
		return (int)SendMessage(hwnd, CB_GETCOUNT, 0, 0);
	}

	std::string ComboBox::getItemText(int index) {
		if (!hwnd || index < 0) return "";

		int length = (int)SendMessage(hwnd, CB_GETLBTEXTLEN, (WPARAM)index, 0);
		if (length == CB_ERR) return "";

		std::vector<char> buffer(length + 1);
		SendMessageA(hwnd, CB_GETLBTEXT, (WPARAM)index, (LPARAM)buffer.data());

		return std::string(buffer.data());
	}

	bool ComboBox::selectionChanged(MSG& msg) {
		if (msg.message == WM_COMMAND && HIWORD(msg.wParam) == CBN_SELCHANGE) {
			if (LOWORD(msg.wParam) == controlID) {
				return true;
			}
		}
		return false;
	}


	void ComboBox::setDropdownStyle(bool allowEdit) {
		if (!hwnd) return;

		removeStyle(CBS_DROPDOWN);
		removeStyle(CBS_DROPDOWNLIST);

		if (allowEdit) {
			addStyle(CBS_DROPDOWN);  // Editable combo box
		}
		else {
			addStyle(CBS_DROPDOWNLIST);  // Non-editable combo box
		}

		SetWindowLong(hwnd, GWL_STYLE, style);
		SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	std::string ComboBox::getEditText() {
		if (!hwnd) return "";

		LRESULT textLength = SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0);
		if (textLength <= 0)
			return "";

		std::vector<char> buffer(static_cast<size_t>(textLength) + 1);
		SendMessageA(hwnd, WM_GETTEXT, static_cast<WPARAM>(buffer.size()),
			reinterpret_cast<LPARAM>(buffer.data()));

		return std::string(buffer.data());
	}

	void ComboBox::setEditText(const std::string& text) {
		if (!hwnd) return;
		SendMessageA(hwnd, WM_SETTEXT, 0, (LPARAM)text.c_str());
	}

	Button::Button() {
		setClass(L"Button");
	}

	bool Button::init(ws::Window& parent) {
		if (!Child::init(parent)) return false;

		if (!getFontHandle()) {
			ws::Font font;
			font.loadFromSystem("Arial");
			ws::Text text;
			text.setCharacterSize(15);
			setFont(font, text);
		}


		return true;
	}

	bool Button::isPressed(MSG& msg) {
		if (msg.message == WM_COMMAND && HIWORD(msg.wParam) == BN_CLICKED) {
			if (LOWORD(msg.wParam) == controlID) {
				return true;
			}
		}
		return false;
	}

	Slider::Slider() {
		setClass(TRACKBAR_CLASS);
		addStyle(TBS_HORZ);
		addStyle(TBS_AUTOTICKS);
		setRange(0, 100);
	}

	bool Slider::init(ws::Window& parent) {
		if (!Child::init(parent)) return false;

		if (shouldSetRange)
			setRange(storedMin, storedMax);
		if (shouldSetPos)
			setSlidePosition(slidePos);

		if (!getFontHandle()) {
			ws::Font font;
			font.loadFromSystem("Arial");
			ws::Text text;
			text.setCharacterSize(15);
			setFont(font, text);
		}

		parent.addMessageHandler([&](MSG msg) -> LRESULT {
			if (msg.message == WM_HSCROLL && (HWND)msg.lParam == this->hwnd) {
				this->slidePos = (int)SendMessage(this->hwnd, TBM_GETPOS, 0, 0);

			}
			return 0;
			});
		return true;
	}

	void Slider::setHorizontal() {
		removeStyle(TBS_VERT);
		addStyle(TBS_HORZ);
	}

	void Slider::setVertical() {
		removeStyle(TBS_HORZ);
		addStyle(TBS_VERT);
	}


	void Slider::setRange(int minimum, int maximum) {
		storedMin = minimum;
		storedMax = maximum;
		if (!hwnd) {
			shouldSetRange = true;
			return;
		}
		SendMessage(hwnd, TBM_SETRANGEMIN, TRUE, minimum);
		SendMessage(hwnd, TBM_SETRANGEMAX, TRUE, maximum);
	}

	void Slider::setSlidePosition(int pos) {
		slidePos = pos;

		if (!hwnd) {
			shouldSetPos = true;
			return;
		}
		SendMessage(hwnd, TBM_SETPOS, TRUE, pos);

	}

	int Slider::getSlidePosition() {
		return slidePos;
	}

	ScrollBar::ScrollBar(bool vertical) {
		setClass(L"SCROLLBAR");
		if (vertical)
			addStyle(SBS_VERT);
		else
			addStyle(SBS_HORZ);
		setRange(0, 100);
		setPageSize(10);
		setSlidePos(0);
	}

	void ScrollBar::setVertical() {
		removeStyle(SBS_HORZ);
		addStyle(SBS_VERT);
	}

	void ScrollBar::setHorizontal() {
		removeStyle(SBS_VERT);
		addStyle(SBS_HORZ);
	}


	void ScrollBar::setRange(int minVal, int maxVal) {
		SCROLLINFO si = { sizeof(si) };
		si.fMask = SIF_RANGE;
		si.nMin = minVal;
		si.nMax = maxVal;
		SetScrollInfo(hwnd, SB_CTL, &si, TRUE);
	}

	void ScrollBar::setPageSize(int page) {
		SCROLLINFO si = { sizeof(si) };
		si.fMask = SIF_PAGE;
		si.nPage = page;
		SetScrollInfo(hwnd, SB_CTL, &si, TRUE);
	}

	void ScrollBar::setSlidePos(int pos) {
		SCROLLINFO si = { sizeof(si) };
		si.fMask = SIF_POS;
		si.nPos = pos;
		SetScrollInfo(hwnd, SB_CTL, &si, TRUE);
	}

	int ScrollBar::getSlidePos() {
		SCROLLINFO si = { sizeof(si) };
		si.fMask = SIF_POS;
		GetScrollInfo(hwnd, SB_CTL, &si);
		return si.nPos;
	}

	bool ScrollBar::init(ws::Window& parent) {
		if (!Child::init(parent))
			return false;

		return true;
	}

	TextBox::TextBox() {
		setClass(L"EDIT");
		addStyle(ES_AUTOVSCROLL);
		addStyle(ES_AUTOHSCROLL);
		addStyle(ES_MULTILINE);
		addExStyle(WS_EX_CLIENTEDGE);
	}

	bool TextBox::init(ws::Window& parent) {

		if (!Child::init(parent)) return false;

		if (!getFontHandle()) {
			ws::Font font;
			font.loadFromSystem("Arial");
			ws::Text text;
			text.setCharacterSize(15);
			setFont(font, text);
		}

		setCharacterLimit(char_limit);

		return true;
	}
	void TextBox::setCharacterLimit(int max_chars)//0 is infinite
	{
		if (!hwnd)
			char_limit = max_chars;
		else
			SendMessage(hwnd, EM_SETLIMITTEXT, (WPARAM)max_chars, 0);
	}


	bool TextBox::getFocus() const {
		if (!hwnd) return false;
		return (GetFocus() == hwnd);
	}

	Label::Label() {
		setClass(L"STATIC");
		addStyle(SS_NOTIFY);
		addStyle(SS_LEFT);
	}

	bool Label::init(ws::Window& parent) {

		if (!Child::init(parent)) return false;

		if (!getFontHandle()) {
			ws::Font font;
			font.loadFromSystem("Arial");
			ws::Text text;
			text.setCharacterSize(15);
			setFont(font, text);
		}

		return true;
	}

	void ColorDialog::init(ws::Window &newParent)
	{ parentRef = &newParent;}

	ws::Window* ColorDialog::getParent()
	{ return parentRef;}

	void ColorDialog::addFlag(DWORD newFlag)
	{ flags |= newFlag; }
	
	void ColorDialog::removeFlag(DWORD removeFlag)
	{ flags &= ~removeFlag;}
	
	DWORD ColorDialog::getFlags()
	{ return flags; }

	void ColorDialog::setInitColor(ws::Hue hue)
	{ initColor = hue; }

	ws::Hue ColorDialog::getInitColor()
	{ return initColor; }

	ws::Hue ColorDialog::getResult()
	{ return chosenColor; }

	
	bool ColorDialog::open()
	{
		CHOOSECOLOR cc;
		
		ZeroMemory(&cc, sizeof(cc));
		cc.lStructSize = sizeof(cc);
		if(parentRef && parentRef->hwnd)
			cc.hwndOwner = parentRef->hwnd;
		cc.lpCustColors = customColors; // point to the custom colors array
		cc.rgbResult = initColor;      // set the initial color as black
		cc.Flags = flags; 	
		
		//ChooseColor() causes it to show the dialog and it returns whether or not a color was selected.
		if (ChooseColor(&cc) == TRUE) 
		{
			chosenColor = cc.rgbResult;
			return true;
		}
		
		return false;		
	}

	Dropdown::Dropdown(int newID, std::string newName)
	{
	    if (newID != 0) // Leaf items don't need a menu handle
	        handle = CreatePopupMenu();
	    ID = newID;
	    name = newName;
	    isPopup = (newID != 0);
	}
	
	void Dropdown::addItem(int id,DWORD type, std::string itemName) const 
	{
	    if (isPopup)
	        AppendMenuA(handle, type, id, itemName.c_str());
	}
	
	void Dropdown::addSubmenu(Dropdown &submenu) const
	{
	    if (isPopup && submenu.isPopup)
	        AppendMenuA(handle, MF_POPUP, (UINT_PTR)submenu.getHandle(), submenu.getName().c_str());
	}
	
	HMENU Dropdown::getHandle() const
	{ return handle; }
	
	std::string Dropdown::getName() const
	{ return name;}
	
	int Dropdown::getID() const
	{ return ID;}
	
	void Dropdown::addItem(Dropdown drop) const
	{
		if (isPopup)
			AppendMenuA(this->handle, MF_STRING, drop.getID(), drop.getName().c_str());
	}

	Menu::Menu() {
		bar = CreateMenu();
	}

	void Menu::addDropdown(ws::Dropdown& drop) {
		AppendMenuA(bar, MF_POPUP, (UINT_PTR)drop.getHandle(), drop.getName().c_str());
		if (windowRef != nullptr)
			windowRef->setSize(windowRef->getSize());
	}

	void Menu::setVisible(bool visible, ws::Window& window) {
		if (GetMenuItemCount(bar) == 0) {
			//setting the menu visible before adding items will cause the client area to offset in the wrong way and will cause mouse coordinates to be off.
			MessageBoxA(NULL, "Error! You can't use ws::Menu::setVisible() till you add items to the menu!", "Invalid Menu Command Order", MB_OK);
			return;
		}
		if (visible)
			SetMenu(window.hwnd, bar);
		else
			SetMenu(window.hwnd, NULL);

		SetWindowPos(window.hwnd, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

		int menuH = 0;
		if (visible) {
			MENUBARINFO mbi = { sizeof(mbi) };
			if (GetMenuBarInfo(window.hwnd, OBJID_MENU, 0, &mbi))
				menuH = mbi.rcBar.bottom - mbi.rcBar.top;

			if (menuH == 0)
				menuH = GetSystemMetrics(SM_CYMENU);
		}

		window.setSourcePos(window.getSourcePos().x, menuH);

		windowRef = &window;
		InvalidateRect(window.hwnd, NULL, TRUE);
		UpdateWindow(window.hwnd);
	}


	int Menu::getEvent(MSG& m)//You can use this for readability or you can use the normal way.
	{
		if (m.message == WM_COMMAND)
			return LOWORD(m.wParam);
		return -1;
	}

	ws::Window& Menu::getWindow() {
		return *windowRef;
	}

	void ClickMenu::addFlag(DWORD newFlag)
	{ flags |= newFlag; }
	
	void ClickMenu::removeFlag(DWORD removeFlag)
	{ flags &= ~removeFlag;}
	
	DWORD ClickMenu::getFlags() const
	{ return flags; }		
	
	int ClickMenu::getResult()
	{
		int r = command;
		command = 0;
		return r;
	}
	
	std::vector<std::string> ClickMenu::getList()
	{ return list; }
	
	void ClickMenu::setList(std::vector<std::string> newList)
	{ list = newList; }
	
	void ClickMenu::addItem(std::string item)
	{ list.push_back(item);}
	
	void ClickMenu::removeItem(std::string item)
	{	
		for(size_t a=0;a<list.size();a++)
		{
			if(list[a] == item)
			{
				list.erase(list.begin() + a);
				break;
			}
		}
		
	}
	
	void ClickMenu::init(ws::Window &newParent)
	{ parentRef = &newParent;}
	
	ws::Window* ClickMenu::getParent()
	{ return parentRef;}
	
	bool ClickMenu::open(ws::Vec2i mouse)
	{
		if(parentRef == nullptr)
		{
			MessageBoxA(NULL,"Attempted to show a ClickMenu without referencing a parent window! Use Init().","Failed init!",MB_OK | MB_ICONINFORMATION);
			return false;
		}
		HMENU hMenu = CreatePopupMenu();
		if(!hMenu)
			return false;
		
		for (size_t a = 0; a < list.size(); a++) {
			AppendMenu(hMenu, MF_STRING, 1 + a, ws::toUTF16(list[a]).c_str());
		}

        POINT pt = mouse;
        ClientToScreen(parentRef->hwnd, &pt);

        command = TrackPopupMenu(
            hMenu, 
            flags,
            pt.x,
            pt.y,
            0,
            parentRef->hwnd,
            NULL
        );

        DestroyMenu(hMenu); 
        
        return true;
	}


	FileWindow::FileWindow() : 
		  flags(OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NODEREFERENCELINKS | OFN_NOCHANGEDIR | OFN_EXPLORER)
		, title("File Explorer")
		, defaultFilter(1)
		, fileName("") 
		, szFile({ 0 }) {

	}

	void FileWindow::setInitResult(std::string file) {
		fileName = file;
	}

	std::string FileWindow::getResult() {
		return fileName;
	}

	void FileWindow::setTitle(std::string name) {
		title = name;
	}

	std::string FileWindow::getTitle() {
		return title;
	}

	void FileWindow::addFlag(DWORD newFlag) {
		flags |= newFlag;
	}

	void FileWindow::removeFlag(DWORD removeFlag) {
		flags &= ~removeFlag;
	}

	DWORD FileWindow::getFlags() const {
		return flags;
	}

	bool FileWindow::open(ws::Window* parent) {
		std::wstring wtitle = ws::toUTF16(title);
		std::wstring wfileName = ws::toUTF16(fileName);

		wcsncpy(szFile, wfileName.c_str(), MAX_PATH - 1);
		szFile[MAX_PATH - 1] = L'\0';

		OPENFILENAMEW ofn = { 0 };
		ofn.lStructSize = sizeof(OPENFILENAMEW);
		ofn.lpstrFilter = L"All Files\0*.*\0";
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = wtitle.c_str();
		ofn.Flags = flags;
		ofn.nFilterIndex = defaultFilter;
		ofn.lpstrDefExt = L"";

		if (parent == nullptr)
			ofn.hwndOwner = NULL;
		else
			ofn.hwndOwner = parent->hwnd;


		if (!fileName.empty()) {
			std::filesystem::path p(fileName);
			if (std::filesystem::exists(p)) {
				std::wstring wdir = ws::toUTF16(p.parent_path().string());
				ofn.lpstrInitialDir = wdir.c_str();
			}
		}

		if (parent) {
			MSG m;
			while (parent->pollEvent(m)) {}
		}
		else
			ws::log("Warning: Opening a dialog without specifying a parent window is discouraged due to the fact that dialogs block the message que of a window. \nIf you want to have a window and a dialog, you might want to empty the message queue after opening the dialog.\n");

		if (GetOpenFileNameW(&ofn)) {
			fileName = ws::toUTF8(szFile);
			return true;
		}
		else {
			fileName.clear();
			return false;
		}
	}

	bool FileWindow::save(ws::Window* parent) {
		std::wstring wtitle = ws::toUTF16(title);
		std::wstring wfileName = ws::toUTF16(fileName);

		wcsncpy(szFile, wfileName.c_str(), MAX_PATH - 1);
		szFile[MAX_PATH - 1] = L'\0';

		OPENFILENAMEW ofn = { 0 };
		ofn.lStructSize = sizeof(OPENFILENAMEW);
		ofn.lpstrFilter = L"All Files\0*.*\0";
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = wtitle.c_str();
		ofn.Flags = flags | OFN_OVERWRITEPROMPT;
		ofn.nFilterIndex = defaultFilter;
		ofn.lpstrDefExt = L"";

		if (parent == nullptr)
			ofn.hwndOwner = NULL;
		else
			ofn.hwndOwner = parent->hwnd;

		if (!fileName.empty()) {
			std::filesystem::path p(fileName);
			if (p.has_parent_path()) {
				std::wstring wdir = ws::toUTF16(p.parent_path().string());
				ofn.lpstrInitialDir = wdir.c_str();
			}
		}

		if (parent) {
			MSG m;
			while (parent->pollEvent(m)) {}
		}
		else
			ws::log("Warning: Opening a dialog without specifying a parent window is discouraged due to the fact that dialogs block the message que of a window. \nIf you want to have a window and a dialog, you might want to empty the message queue after opening the dialog.\n");

		if (GetSaveFileNameW(&ofn)) {
			fileName = ws::toUTF8(szFile);
			return true;
		}
		else {
			fileName.clear();
			return false;
		}
	}


	FolderWindow::FolderWindow() {}

	void FolderWindow::setTitle(std::string name) {
		title = name;
	}

	std::string FolderWindow::getTitle() {
		return title;
	}

	void FolderWindow::addFlag(DWORD flag) {
		flags |= flag;
	}

	void FolderWindow::setFlags(DWORD allFlags) {
		flags = allFlags;
	}

	void FolderWindow::removeFlag(DWORD flag) {
		flags &= ~flag;
	}

	DWORD FolderWindow::getFlags() {
		return flags;
	}

	std::string FolderWindow::getResult() {
		return folderName;
	}

	bool FolderWindow::open(ws::Window* parent) {
		std::wstring wtitle = ws::toUTF16(title);

		BROWSEINFOW bi = { 0 };
		bi.lpszTitle = wtitle.c_str();
		bi.ulFlags = flags;

		if (parent == nullptr)
			bi.hwndOwner = NULL;
		else
			bi.hwndOwner = parent->hwnd;

		if (!initialFolder.empty()) {
			std::wstring winitial = ws::toUTF16(initialFolder);
			bi.lParam = (LPARAM)winitial.c_str();
			bi.lpfn = BrowseCallbackProc;
			bi.ulFlags |= BIF_NEWDIALOGSTYLE;
		}

		if (parent) {
			MSG m;
			while (parent->pollEvent(m)) {}
		}
		else
			ws::log("Warning: Opening a dialog without specifying a parent window is discouraged due to the fact that dialogs block the message que of a window. \nIf you want to have a window and a dialog, you might want to empty the message queue after opening the dialog.\n");

		LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
		if (pidl != nullptr) {
			// Get the path of the selected folder
			wchar_t path[MAX_PATH];
			if (SHGetPathFromIDListW(pidl, path)) {

				folderName = ws::toUTF8(path);

				// Free the PIDL
				CoTaskMemFree(pidl);
				return true;
			}
			CoTaskMemFree(pidl);
		}

		folderName = "";
		return false;
	}

	void FolderWindow::setInitResult(std::string folder) {
		initialFolder = folder;
	}

	int CALLBACK FolderWindow::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
		if (uMsg == BFFM_INITIALIZED) {
			SendMessageW(hwnd, BFFM_SETSELECTIONW, TRUE, lpData);
		}
		return 0;
	}

	ExploreWindow::ExploreWindow(std::string mode) {
		CLSID clsid = (mode == "save") ? CLSID_FileSaveDialog : CLSID_FileOpenDialog;
		hr = CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

		options = pfd->GetOptions(&options);

		addStyle(FOS_NOCHANGEDIR);
	}

	void ExploreWindow::addStyle(DWORD style) {
		options |= style;
	}
	void ExploreWindow::removeStyle(DWORD style) {
		options &= ~style;
	}

	void ExploreWindow::setTitle(std::string title) { m_title = title; }
	void ExploreWindow::setInitResult(std::string folder) { m_initialFolder = folder; }

	std::string ExploreWindow::getResult() const { return m_resultName; }

	bool ExploreWindow::open(ws::Window* parent) {

		bool result = true;
		if (SUCCEEDED(hr)) {
			pfd->SetOptions(options);

			if (!m_title.empty()) {
				std::wstring wtitle = ws::toUTF16(m_title);
				pfd->SetTitle(wtitle.c_str());
			}

			if (!m_initialFolder.empty()) {
				IShellItem* psiInitial = nullptr;
				std::wstring wInitial = ws::toUTF16(m_initialFolder);
				hr = SHCreateItemFromParsingName(wInitial.c_str(), nullptr, IID_PPV_ARGS(&psiInitial));
				if (SUCCEEDED(hr)) {
					pfd->SetFolder(psiInitial);
					psiInitial->Release();
				}
			}

			HWND parentHwnd = nullptr;
			if (parent)
				parentHwnd = parent->hwnd;

			hr = pfd->Show(parentHwnd);
			if (SUCCEEDED(hr)) {
				IShellItem* psiResult = nullptr;
				hr = pfd->GetResult(&psiResult);
				if (SUCCEEDED(hr)) {
					PWSTR pszPath = nullptr;
					hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
					if (SUCCEEDED(hr)) {
						m_resultName = ws::toUTF8(pszPath);
						CoTaskMemFree(pszPath);
						result = true;
					}
					psiResult->Release();
				}
			}
			pfd->Release();
		}

		return result;
	}

	ListBox::ListBox() {
		addStyle(LBS_STANDARD | WS_VSCROLL | WS_HSCROLL | LBS_NOTIFY);
		setClass(L"LISTBOX");
	}

	bool ListBox::init(ws::Window& parent) {
		if (!Child::init(parent)) return false;

		if (!getFontHandle()) {
			ws::Font font;
			font.loadFromSystem("Arial");
			ws::Text text;
			text.setCharacterSize(15);
			setFont(font, text);
		}

		for (const auto& item : pendingItems) addItem(item);

		pendingItems.clear();

		return true;
	}

	void ListBox::addItem(const std::string& item) {
		if (!hwnd) {
			pendingItems.push_back(item);
			return;
		}
		SendMessageA(hwnd, LB_ADDSTRING, 0, (LPARAM)item.c_str());
	}

	void ListBox::addItems(const std::vector<std::string>& items) {
		if (!hwnd) {
			for (const auto& item : items) pendingItems.push_back(item);
			return;
		}
		for (const auto& item : items) addItem(item);
	}

	void ListBox::removeItem(int index) {
		if (!hwnd) return;
		SendMessageA(hwnd, LB_DELETESTRING, (WPARAM)index, 0);
	}

	void ListBox::clear() {
		if (!hwnd) { pendingItems.clear(); return; }
		SendMessageA(hwnd, LB_RESETCONTENT, 0, 0);
	}

	int ListBox::getSelectedIndex() {
		return hwnd ? (int)SendMessageA(hwnd, LB_GETCURSEL, 0, 0) : -1;
	}

	std::string ListBox::getSelectedText() {
		int index = getSelectedIndex();
		return (index == -1) ? "" : getItemText(index);
	}

	std::string ListBox::getItemText(int index) {
		if (!hwnd || index < 0) return "";

		int len = (int)SendMessage(hwnd, LB_GETTEXTLEN, (WPARAM)index, 0);

		if (len == LB_ERR) return "";

		std::vector<char> buf(len + 1);
		SendMessageA(hwnd, LB_GETTEXT, (WPARAM)index, (LPARAM)buf.data());
		return std::string(buf.data());
	}

	void ListBox::setSelectedIndex(int index) {
		if (hwnd)
			SendMessage(hwnd, LB_SETCURSEL, (WPARAM)index, 0);
	}

	bool ListBox::selectionChanged(MSG& msg) {
		return msg.message == WM_COMMAND && HIWORD(msg.wParam) == LBN_SELCHANGE && LOWORD(msg.wParam) == controlID;
	}

	int ListBox::getItemCount() {
		return hwnd ? (int)SendMessage(hwnd, LB_GETCOUNT, 0, 0) : 0;
	}

	bool Balloon(ws::Window& window, std::string message, std::string title, HICON hIcon, DWORD messageIconType, int timeoutMilliseconds, DWORD styles) {
		NOTIFYICONDATA nid = {};
		ZeroMemory(&nid, sizeof(nid));
		nid.cbSize = sizeof(nid);
		nid.hWnd = window.hwnd;
		static int BalloonID = 0;
		nid.uID = BalloonID++;

		nid.uFlags = styles;
		nid.hIcon = hIcon;
		wcscpy_s(nid.szTip, ws::toUTF16(title).c_str());

		if (!Shell_NotifyIcon(NIM_ADD, &nid))
			return false;

		nid.dwInfoFlags = messageIconType;
		wcscpy_s(nid.szInfo, ws::toUTF16(message).c_str());
		wcscpy_s(nid.szInfoTitle, ws::toUTF16(title).c_str());
		nid.uTimeout = timeoutMilliseconds;

		if (!Shell_NotifyIcon(NIM_MODIFY, &nid)) {
			Shell_NotifyIcon(NIM_DELETE, &nid);
			return false;
		}

		ws::Timer timer;
		window.addMessageHandler([timer, timeoutMilliseconds, nid](MSG msg) mutable -> HRESULT {
			if (timer.getMilliSeconds() >= timeoutMilliseconds)
				Shell_NotifyIcon(NIM_DELETE, &nid);
			return 0;
			});
		return true;
	}

	bool TrayIcon::init(ws::Window& window, std::string title, HICON hIcon, DWORD styles) {
		msgPtr = std::make_shared<MSG>();
		std::weak_ptr<MSG> weakMsg = msgPtr;

		ZeroMemory(&m_nid, sizeof(m_nid));
		m_nid.cbSize = sizeof(NOTIFYICONDATA);
		m_nid.hWnd = window.hwnd;
		static int ID = 0;
		myID = ++ID;
		m_nid.uID = myID;
		m_nid.uFlags = styles;
		m_nid.hIcon = hIcon;
		m_nid.uCallbackMessage = WM_USER + myID;
		lstrcpy(m_nid.szTip, ws::toUTF16(title).c_str());

		if (!Shell_NotifyIcon(NIM_ADD, &m_nid))
			return false;

		//store incoming messages
		int localID = myID;
		window.addMessageHandler([weakMsg, localID](MSG msg) mutable -> LRESULT {
			if (msg.message != WM_USER + localID)
				return 0;
			if (auto sp = weakMsg.lock())
				*sp = msg;
			return 0;
			});


		//destroy handler
		NOTIFYICONDATA localNid = m_nid;
		window.addMessageHandler([localNid](MSG msg) mutable -> LRESULT {
			if (msg.message == WM_DESTROY) {
				Shell_NotifyIcon(NIM_DELETE, &localNid);
				return 0;
			}
			return 0;
			});
		return true;
	}
	bool TrayIcon::isPressed(int button) {
		if (!msgPtr) return false;
		const MSG& msg = *msgPtr;

		if (msg.message != WM_USER + myID)
			return false;

		bool pressed = false;

		if (msg.lParam == button)
			pressed = true;
		if (button == ws::Mouse::Right && msg.lParam == WM_RBUTTONUP)
			pressed = true;
		if (button == ws::Mouse::Left && msg.lParam == WM_LBUTTONUP)
			pressed = true;

		if (pressed)
			*msgPtr = MSG{};
		return pressed;
	}

	bool TrayIcon::showBalloon(const std::string& message, const std::string& title, DWORD infoFlags, int timeoutMs) const {
		if (myID == -1) return false;

		// copy of the nid with info flags set
		NOTIFYICONDATA nid = m_nid;
		nid.uFlags |= NIF_INFO;   // update the info
		nid.dwInfoFlags = infoFlags;
		wcscpy_s(nid.szInfo, ws::toUTF16(message).c_str());
		wcscpy_s(nid.szInfoTitle, ws::toUTF16(title).c_str());
		nid.uTimeout = timeoutMs;

		return Shell_NotifyIcon(NIM_MODIFY, &nid) == TRUE;
	}

#endif
}
#endif