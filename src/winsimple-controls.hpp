#ifndef WINSIMPLE_CONTROLS
#define WINSIMPLE_CONTROLS

#include <commctrl.h>   // for common controls (trackbar, etc.)
#include <shlobj.h>     // for folder browser (BROWSEINFO, etc.)
#include <filesystem>

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
	// Forward declaration: handles WM_NOTIFY messages for child controls.
    LRESULT handleNotifyForChildren(Window* window, NMHDR* pnmh, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
	
	class ControlsInit
	{
		public:
		int maxControlID = 1000;
		
		// Constructor: initializes common controls and COM for folder dialogs.
		ControlsInit()
		{
			INITCOMMONCONTROLSEX icex;
			//This is for initialization of winapi child objects sucg as buttons and textboxes.
			icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
			icex.dwICC = ICC_STANDARD_CLASSES | ICC_TAB_CLASSES;  // Enables a set of common controls.
			InitCommonControlsEx(&icex);
			///////////////////////////////
			
			//used for FolderWindowModern
			HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			if (FAILED(hr)){
				MessageBoxA(NULL,"Winsimple Failed to call CoInitializeEx() in winsimple-controls!","Winsimple Failure!",MB_OK);
			}
			
			ws::Window::s_handleNotifyForChildren = &handleNotifyForChildren;
		}
		
		// Destructor: uninitializes COM.
		~ControlsInit()
		{
			CoUninitialize();
		}
		
	}controlsInit;



	class Child
	{
		private:

		HFONT customFont = nullptr;
		std::string text = "";
		int x = 0, y = 0, width = 100, height = 100;
		std::string m_className;               

		std::vector<ws::Child*> children;

		using DestructorCallback = std::function<void(Child*)>;
		std::vector<DestructorCallback> m_destructorCallbacks;

		HWND m_currentParent = nullptr;
		
		public:

		HWND hwnd = NULL;
		DWORD style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
		DWORD exStyle = 0;                     // extended style
		DWORD textStyle = DT_CENTER | DT_VCENTER | DT_SINGLELINE;

		unsigned int controlID = 0;
		COLORREF backgroundColor = RGB(0,0,0);
		COLORREF textColor = RGB(255,255,255);
		COLORREF borderColor = RGB(0,0,0);

		
		// Constructor: creates a child control with a given Win32 class name.
		Child(const std::wstring& className = L"Button") : m_className(ws::SHORT(className))
		{
			controlID = controlsInit.maxControlID++;
		}
		
		// Destructor: destroys the child window and frees the custom font.
		virtual ~Child()
		{
			for(auto& cb : m_destructorCallbacks)
				cb(this);
			
			if (hwnd && IsWindow(hwnd))
				DestroyWindow(hwnd);
			if (customFont)
				DeleteObject(customFont);
		}

		// Initializes the child control and creates its Win32 window.
		virtual bool init(ws::Window& parent)
		{
			return init(parent.hwnd);
		}
		
		// Initializes the child control with a given parent HWND.
		virtual bool init(HWND phwnd)
		{
			if (!phwnd) return false;

			
			if (hwnd && IsWindow(hwnd)) 
			{
				HWND cParent = GetParent(hwnd);
				if(cParent && cParent != phwnd)
				{
					std::cerr << "Error: Child already parented to a different window. "
							  << "Remove it from the current container first.\n";
					MessageBoxA(NULL,"Error: Child already parented to a different window. Remove it from the current container first.\n","Error",MB_OK | MB_ICONINFORMATION);		  
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

			if (!hwnd)
			{
				std::cerr << "Failed to create child control: " << m_className << std::endl;
				return false;
			}

			// Apply stored font if any
			if (customFont)
				SendMessage(hwnd, WM_SETFONT, (WPARAM)customFont, TRUE);

			ShowWindow(hwnd, SW_SHOW);
			UpdateWindow(hwnd);

			for(int a=0;a<children.size();a++)
			{
				if(!children[a]->hwnd)
					children[a]->init(this->hwnd);
				if(GetParent(children[a]->hwnd) != this->hwnd)
					children[a]->init(this->hwnd);
			}

			return true;			
		}

		// Registers a callback to be called when this child is destroyed.
		void registerDestructorCallback(DestructorCallback callback)
		{
			m_destructorCallbacks.push_back(std::move(callback));
		}
		// Clears all registered destructor callbacks.
		void clearDestructorCallbacks() 
		{
			m_destructorCallbacks.clear();
		}

		// Adds another child control as a child of this control.
		void addChild(ws::Child& child) 
		{
			children.push_back(&child);

			
			if(this->hwnd)
				child.init(this->hwnd);  
		}

		// Removes a child control from this control's child list.
		void removeChild(ws::Child &child)
		{
			for(size_t a=0;a<children.size();a++)
			{
				if(&child == children[a])
				{
					children.erase(children.begin() + a);
					break;
				}
			}
		}
		
		// Checks whether a given child is directly owned by this control.
		bool hasChild(ws::Child &child)
		{
			for(size_t a=0;a<children.size();a++)
			{
				if(&child == children[a])
				{
					return true;
				}
			}			
			return false;
		}			
		
		// Sets the Win32 class name of this control (recreates if already created).
		void setClass(const std::wstring& className = L"Button")
		{
			m_className = ws::SHORT(className);
			if(hwnd)
			{
				init(GetParent(hwnd));
			}
		}
		
		// Returns the Win32 class name of this control.
		std::string getClass()
		{
			return m_className;
		}
		
		// Shows or hides the control.
		void setVisible(bool visible)
		{
			ShowWindow(hwnd,(visible) ? SW_SHOW : SW_HIDE);
		}

		// Moves the control to new coordinates.
		void setPosition(int xPos, int yPos) 
		{ 
			x = xPos; 
			y = yPos; 
			if (hwnd) 
				SetWindowPos(hwnd, nullptr, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE); 
		}
		
		// Moves the control to a new position given as a Vec2i.
		void setPosition(ws::Vec2i pos) 
		{ 
			setPosition(pos.x, pos.y); 
		}
		
		// Returns the current position of the control.
		ws::Vec2i getPosition() const 
		{ 
			return {x, y}; 
		}

		// Resizes the control to the given width and height.
		virtual void setSize(int w, int h) 
		{ 
			width = w; 
			height = h; 
			if (hwnd) 
				SetWindowPos(hwnd, nullptr, x, y, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE); 
		}
		
		// Resizes the control using a Vec2i size.
		virtual void setSize(ws::Vec2i size) 
		{ 
			setSize(size.x, size.y); 
		}
		
		// Returns the current size of the control.
		ws::Vec2i getSize() const 
		{ 
			return {width, height}; 
		}
		
		// Returns the handle of the custom font assigned to this control.
		HFONT getFontHandle()
		{
			return customFont;
		}

		// Adds a window style flag to the control.
		void addStyle(DWORD addedStyle) 
		{ 
			style |= addedStyle; 
			if (hwnd) 
			{ 
				SetWindowLong(hwnd, GWL_STYLE, style); 
				SetWindowPos(hwnd, NULL, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED); 
			} 
		}
		
		// Removes a window style flag from the control.
		void removeStyle(DWORD removedStyle) 
		{ 
			style &= ~removedStyle; 
			if (hwnd) 
			{ 
				SetWindowLong(hwnd, GWL_STYLE, style); 
				SetWindowPos(hwnd, NULL, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED); 
			} 
		}
		
		// Checks whether a given style flag is currently set.
		bool hasStyle(DWORD checkStyle) const 
		{ 
			DWORD current = GetWindowLong(hwnd, GWL_STYLE); 
			return (current & checkStyle) != 0; 
		}

		// Adds an extended window style flag.
		void addExStyle(DWORD addedStyle) 
		{ 
			exStyle |= addedStyle; 
			if (hwnd) 
			{ 
				SetWindowLong(hwnd, GWL_EXSTYLE, exStyle); 
				SetWindowPos(hwnd, NULL, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED); 
			} 
		}
		
		// Removes an extended window style flag.
		void removeExStyle(DWORD removedStyle) 
		{ 
			exStyle &= ~removedStyle; 
			if (hwnd) 
			{ 
				SetWindowLong(hwnd, GWL_EXSTYLE, exStyle); 
				SetWindowPos(hwnd, NULL, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED); 
			} 
		}
		
		// Checks whether a given extended style flag is set.
		bool hasExStyle(DWORD checkStyle) const 
		{ 
			DWORD current = GetWindowLong(hwnd, GWL_EXSTYLE); 
			return (current & checkStyle) != 0; 
		}
		
		// Sets the text of the control.
		void setText(const std::string& newText) 
		{ 
			text = newText; 
			if (hwnd) 
				SetWindowTextA(hwnd, text.c_str()); 
		}
		
		// Retrieves the current text of the control.
		std::string getText() const 
		{ 
			if (!hwnd) 
				return text; 
			int len = GetWindowTextLengthW(hwnd); 
			if (len == 0) 
				return ""; 
			std::wstring wbuf(len+1, L'\0'); 
			GetWindowTextW(hwnd, &wbuf[0], len+1); 
			wbuf.resize(len); return ws::SHORT(wbuf); 
		}

		// Applies a ws::Font and ws::Text style to the control.
		void setFont(ws::Font& font, ws::Text& textSettings) 
		{
			if (!font.isValid()) return;
			
			if (customFont)
			{
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
			
			if (customFont && hwnd)
			{
				SendMessage(hwnd, WM_SETFONT, (WPARAM)customFont, TRUE);
			}			
		}

		// Checks if a point (relative to parent) lies inside the control.
		bool contains(ws::Vec2i point) const 
		{ 
			return (point.x >= x && point.x < x+width && point.y >= y && point.y < y+height); 
		}

		// Handles WM_COMMAND messages; override in derived classes.
		virtual bool handleCommand(MSG &msg) { return false; }
		// Handles WM_NOTIFY messages; override in derived classes.
		virtual bool handleNotify(NMHDR* pnmh) { return false; }
	};
	

	// Adds a child control to a ws::Window and initializes it.
	void ws::Window::addChild(ws::Child &child)
	{
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
	
	// Removes a child control from the window.
	void ws::Window::removeChild(ws::Child &child)
	{
		for(size_t a=0;a<children.size();a++)
		{
			if(&child == children[a])
			{
				children.erase(children.begin() + a);
				break;
			}
		}
	}
	
	// Checks whether a child control is registered with the window.
	bool ws::Window::hasChild(ws::Child &child)
	{
		for(size_t a=0;a<children.size();a++)
		{
			if(&child == children[a])
			{
				return true;
			}
		}			
		return false;
	}	

	// Routes WM_NOTIFY messages to child controls; returns 0 if handled.
	LRESULT handleNotifyForChildren(Window* window, NMHDR* pnmh, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        for (Child* child : window->children)
        {
            if (child && child->handleNotify(pnmh))
                return 0;
        }
        return DefWindowProc(window->hwnd, uMsg, wParam, lParam);
    }


	class Tabs : public ws::Child
	{
		private:
		
		struct PendingPage
		{
			std::string title = "";
			ws::Child *child;
		};
		
		std::vector<ws::Child*> pages;
		std::vector<PendingPage> pendingPages;
		int selected = 0;
		
		public:
		
		// Constructor: sets the tab control class and default styles.
		Tabs()
		{
			setClass(L"SysTabControl32");
			addStyle(TCS_FIXEDWIDTH | TCS_RIGHTJUSTIFY  | WS_CLIPSIBLINGS);			
		}
		
		// Initializes the tab control and processes any pending pages.
		virtual bool init(ws::Window& parent) override
		{
			if (!Child::init(parent))
				return false;

			if (!getFontHandle())
			{
				ws::Font font;
				font.loadFromSystem("Segoe UI");
				ws::Text text;
				text.setCharacterSize(14);
				setFont(font, text);
			}

			for (auto& pending : pendingPages) 
			{
				addPage(pending.title,*pending.child);
			}
			pendingPages.clear();

			updatePagePositions();
			
			if (!pages.empty())
				setSelected(selected);

			return true;
		}		
		
		// Adds a new tab page with a title and a child control.
		void addPage(const std::string& title, ws::Child& page)
		{
			if(!hwnd)
			{
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

			
			TCITEM tie = {0};
			tie.mask = TCIF_TEXT;
			
			std::wstring wtitle = ws::WIDE(title);
			tie.pszText = const_cast<LPWSTR>(wtitle.c_str());
			TabCtrl_InsertItem(hwnd, (int)pages.size() - 1, &tie);

			// Reposition all pages to fit the current display area
			updatePagePositions();

			if(pages.size() == 1)
				setSelected(0);
		}

		// Removes a page child from the tab control.
		void removePage(ws::Child &child)
		{
			for(size_t a=0;a<pages.size();a++)
			{
				if(&child == pages[a])
				{
					pages.erase(pages.begin() + a);
					break;
				}
			}
		}
		
		// Checks whether a page child belongs to this tab control.
		bool hasPage(ws::Child &child)
		{
			for(size_t a=0;a<pages.size();a++)
			{
				if(&child == pages[a])
				{
					return true;
				}
			}			
			return false;
		}	
		
		// Selects the tab page at the given index.
		void setSelected(int index)
		{
			if(!hwnd)
			{
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

		// Returns the index of the currently selected tab.
		int getSelected() const
		{
			return TabCtrl_GetCurSel(hwnd);
		}

		// Overrides setSize to also reposition page contents.
		virtual void setSize(int w, int h) override
		{
			Child::setSize(w, h);
			updatePagePositions();
		}
		
		// Handles TCN_SELCHANGE notifications to switch visible pages.
		virtual bool handleNotify(NMHDR* pnmh) override
		{
			if (pnmh->hwndFrom == hwnd && pnmh->code == TCN_SELCHANGE)
			{
				int newSel = TabCtrl_GetCurSel(hwnd);
				
				for(int a=0;a<pages.size();a++)
					pages[a]->setVisible((a == newSel));
				
				
				return true;
			}
			return false;      // not handled, let parent process
		}

		// Updates the position and size of all pages to fit the display area.
		void updatePagePositions()
		{
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
			for(auto* page : pages)
			{
				if(page && page->hwnd)
				{
					SetWindowPos(page->hwnd, NULL,
					rcDisplay.left, rcDisplay.top,
					rcDisplay.right - rcDisplay.left,
					rcDisplay.bottom - rcDisplay.top,
					SWP_NOZORDER | SWP_NOACTIVATE);
				}
			}
		}

		
	};


	class ComboBox : public Child
	{
		private:
		std::vector<std::string> pendingItems;
		
		public:
		
		// Constructor: sets the COMBOBOX class and dropdown style.
		ComboBox()
		{
			setClass(L"COMBOBOX");
	        addStyle(CBS_DROPDOWN);
	        addStyle(WS_VSCROLL);			
		}
		
		// Initializes the combo box and adds any pending items.
		virtual bool init(ws::Window &parent) override
		{
			if (!Child::init(parent)) return false;
			
			if(!getFontHandle())
			{
				ws::Font font;
				font.loadFromSystem("Arial");
				ws::Text text;
				text.setCharacterSize(15);
				setFont(font,text);
			}
	        // set extended UI for better appearance
	        SendMessage(hwnd, CB_SETEXTENDEDUI, (WPARAM)TRUE, 0);

			for (const auto& item : pendingItems)
			{
				addItem(item);
			}
			pendingItems.clear();	

			return true;
		}
		
		// Adds a string item to the combo box drop‑down list.
	    void addItem(const std::string& item)
	    {
			if (!hwnd) {
				// store for later if hwnd doesn't exist yet
				pendingItems.push_back(item);
				return;
			}
	        SendMessageA(hwnd, CB_ADDSTRING, 0, (LPARAM)item.c_str());
	    }
	    
		// Adds multiple string items to the combo box.
	    void addItems(const std::vector<std::string>& items)
	    {
			if (!hwnd) {
				// store for later if hwnd doesn't exist yet
				for(size_t a=0;a<items.size();a++)
					pendingItems.push_back(items[a]);
				return;
			}
	        for (const auto& item : items)
	        {
	            addItem(item);
	        }
	    }
	    
		// Removes the item at the specified index.
	    void removeItem(int index)
	    {
	        if (!hwnd) return;
	        SendMessage(hwnd, CB_DELETESTRING, (WPARAM)index, 0);
	    }
	    
		// Clears all items from the combo box.
	    void clear()
	    {
	        if (!hwnd) return;
	        SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
	    }
	    
		// Returns the index of the currently selected item.
	    int getSelectedIndex()
	    {
	        if (!hwnd) return -1;
	        return (int)SendMessage(hwnd, CB_GETCURSEL, 0, 0);
	    }
	    
		// Selects the item at the given index.
	    void setSelectedIndex(int index)
	    {
	        if (!hwnd) return;
	        SendMessage(hwnd, CB_SETCURSEL, (WPARAM)index, 0);
	    }
	    
		// Returns the text of the currently selected item.
	    std::string getSelectedText()
	    {
	        if (!hwnd) return "";
	        
	        int index = getSelectedIndex();
	        if (index == -1) return "";
	        
	        int length = (int)SendMessage(hwnd, CB_GETLBTEXTLEN, (WPARAM)index, 0);
	        if (length == CB_ERR) return "";
	        
	        std::vector<char> buffer(length + 1);
	        SendMessageA(hwnd, CB_GETLBTEXT, (WPARAM)index, (LPARAM)buffer.data());
	        
	        return std::string(buffer.data());
	    }
	    
		// Returns the total number of items in the combo box.
	    int getItemCount()
	    {
	        if (!hwnd) return 0;
	        return (int)SendMessage(hwnd, CB_GETCOUNT, 0, 0);
	    }
	    
		// Returns the text of the item at the given index.
	    std::string getItemText(int index)
	    {
	        if (!hwnd || index < 0) return "";
	        
	        int length = (int)SendMessage(hwnd, CB_GETLBTEXTLEN, (WPARAM)index, 0);
	        if (length == CB_ERR) return "";
	        
	        std::vector<char> buffer(length + 1);
	        SendMessageA(hwnd, CB_GETLBTEXT, (WPARAM)index, (LPARAM)buffer.data());
	        
	        return std::string(buffer.data());
	    }

		// Checks if a WM_COMMAND message indicates a selection change.
		bool selectionChanged(MSG &msg)
		{
	        if (msg.message == WM_COMMAND && HIWORD(msg.wParam) == CBN_SELCHANGE)
	        {
	            if (LOWORD(msg.wParam) == controlID)
	            {
	                return true;
	            }
	        }
	        return false;			
		}
		
		// Switches between editable and read‑only dropdown styles.
	    void setDropdownStyle(bool allowEdit = true)
	    {
	        if (!hwnd) return;
	        
	        removeStyle(CBS_DROPDOWN);
	        removeStyle(CBS_DROPDOWNLIST);
	        
	        if (allowEdit)
	        {
	            addStyle(CBS_DROPDOWN);  // Editable combo box
	        }
	        else
	        {
	            addStyle(CBS_DROPDOWNLIST);  // Non-editable combo box
	        }
	        
	        SetWindowLong(hwnd, GWL_STYLE, style);
	        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, 
	                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	    }
	    
		// Returns the text currently typed in the editable edit field.
	    std::string getEditText()
	    {
	        if (!hwnd) return "";
	        
	        LRESULT textLength = SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0);
	        if (textLength <= 0)
	            return "";
	        
	        std::vector<char> buffer(static_cast<size_t>(textLength) + 1);
	        SendMessageA(hwnd, WM_GETTEXT, static_cast<WPARAM>(buffer.size()), 
	                    reinterpret_cast<LPARAM>(buffer.data()));
	        
	        return std::string(buffer.data());
	    }
	    
		// Sets the text in the editable edit field.
	    void setEditText(const std::string& text)
	    {
	        if (!hwnd) return;
	        SendMessageA(hwnd, WM_SETTEXT, 0, (LPARAM)text.c_str());
	    }		
	};
	
	
	
	class Button : public Child
	{
		public:
		
		// Constructor: sets the Button class.
		Button()
		{
			setClass(L"Button");
		}
		
		// Initializes the button and sets a default font if needed.
		virtual bool init(ws::Window &parent) override
		{
			if(!Child::init(parent)) return false;
			
			if(!getFontHandle())
			{
				ws::Font font;
				font.loadFromSystem("Arial");
				ws::Text text;
				text.setCharacterSize(15);
				setFont(font,text);
			}
			
		
			return true;				
		}

		// Checks if a WM_COMMAND message indicates this button was clicked.
	    bool isPressed(MSG &msg)
	    {
	        if(msg.message == WM_COMMAND && HIWORD(msg.wParam) == BN_CLICKED)
	        {
	            if(LOWORD(msg.wParam) == controlID)
	            {
	                return true;
	            }
	        }
	        return false;
	    }		
	};
	
	
	
	class Slider : public Child
	{
		public:
		
		// Constructor: sets the trackbar class and horizontal style.
		Slider()
		{
			setClass(TRACKBAR_CLASS);
			addStyle(TBS_HORZ);
			addStyle(TBS_AUTOTICKS);	
			setRange(0,100);			
		}
		
		// Initializes the slider and restores any pending range/position.
		virtual bool init(ws::Window &parent) override
		{
			if(!Child::init(parent)) return false;
			
			if(shouldSetRange)
				setRange(storedMin,storedMax);
			if(shouldSetPos)
				setSlidePosition(slidePos);

			if(!getFontHandle())
			{
				ws::Font font;
				font.loadFromSystem("Arial");
				ws::Text text;
				text.setCharacterSize(15);
				setFont(font,text);
			}
			
			
			return true;
		}
		
		// Checks scroll messages and updates the stored position; returns true if moved.
	    bool getScroll(MSG &msg)
	    {
	    	if(!hwnd)
				return false;
	        if((msg.message == WM_HSCROLL || msg.message == WM_VSCROLL) && (HWND)msg.lParam == hwnd)
	        {
	            slidePos = (int)SendMessage(hwnd, TBM_GETPOS, 0, 0);
	            
				return true;
	        }
	        return false;
	    }
	    
		// Makes the slider horizontal.
	    void setHorizontal()
	    {
	    	removeStyle(TBS_VERT);
	    	addStyle(TBS_HORZ);
		}
		
		// Makes the slider vertical.
		void setVertical()
		{
	    	removeStyle(TBS_HORZ);
	    	addStyle(TBS_VERT);
		}
		
		// Sets the range (minimum and maximum) of the slider.
        void setRange(int minimum = 0,int maximum = 100)
		{
			storedMin = minimum;
			storedMax = maximum;
			if(!hwnd)
			{
				shouldSetRange = true;
				return;
			}
			SendMessage(hwnd, TBM_SETRANGEMIN, TRUE, minimum);
			SendMessage(hwnd, TBM_SETRANGEMAX, TRUE, maximum);
		}
		
		// Sets the current slider position.
		void setSlidePosition(int pos = 0)
		{
			slidePos = pos;
			
			if(!hwnd)
			{
				shouldSetPos = true;
				return;
			}
			SendMessage(hwnd, TBM_SETPOS, TRUE, pos);
			 
		}
		
		// Returns the current slider position.
		int getSlidePosition()
		{
			return slidePos;
		}
		
		
		private:
			int slidePos = 0;
			int storedMin = 0;
			int storedMax = 100;
			bool shouldSetRange = false;
			bool shouldSetPos = false;
		
	};
	
	



	class TextBox : public Child
	{
		public:
		
		// Constructor: sets the EDIT class and multiline styles.
		TextBox()
		{
			setClass(L"EDIT");
			addStyle(ES_AUTOVSCROLL);
			addStyle(ES_AUTOHSCROLL);
			addStyle(ES_MULTILINE);
			addExStyle(WS_EX_CLIENTEDGE);
		}
		
		// Initializes the text box and sets a default font and character limit.
		virtual bool init(ws::Window &parent) override
		{
			
			if(!Child::init(parent)) return false;

			if(!getFontHandle())
			{
				ws::Font font;
				font.loadFromSystem("Arial");
				ws::Text text;
				text.setCharacterSize(15);
				setFont(font,text);
			}
			
			setCharacterLimit(char_limit);
			
			return true;		
		}		
		int char_limit = 0;
		// Sets the maximum number of characters (0 = infinite).
		void setCharacterLimit(int max_chars = 0)
		{
			if(!hwnd)
				char_limit = max_chars;
			else
				SendMessage(hwnd, EM_SETLIMITTEXT, (WPARAM)max_chars, 0);			
		}
		
		// Returns true if this text box currently has keyboard focus.
		bool getFocus()
		{
	        if (!hwnd) return false;
	        return (GetFocus() == hwnd);			
		}
		
	};
	
	
	
	
	class Label : public Child
	{
		public:
		// Constructor: sets the STATIC class and notification style.
		Label()
		{
			setClass(L"STATIC");
			addStyle(SS_NOTIFY);
			addStyle(SS_LEFT);			
		}
		
		// Initializes the label and sets a default font.
		virtual bool init(ws::Window &parent) override
		{
			
			if(!Child::init(parent)) return false;

			if(!getFontHandle())
			{
				ws::Font font;
				font.loadFromSystem("Arial");
				ws::Text text;
				text.setCharacterSize(15);
				setFont(font,text);
			}			

			return true;		
		}
	};


	class ColorDialog
	{
		
		private:
		COLORREF customColors[16]; 
		COLORREF initColor = RGB(0,0,0);
		ws::Hue chosenColor = ws::Hue::black;
		ws::Window *parentRef = nullptr;
		// these flags tell it to show the full dialog and use the initial color.	
		DWORD flags = CC_FULLOPEN | CC_RGBINIT;
		
		public:

		// Sets the parent window for the color dialog.
		void init(ws::Window &newParent)
		{ parentRef = &newParent;}

		// Returns the parent window pointer.
		ws::Window *getParent()
		{ return parentRef;}

		// Adds a flag to the dialog's behaviour.
		void addFlag(DWORD newFlag)
		{ flags |= newFlag; }
		
		// Removes a flag from the dialog's behaviour.
		void removeFlag(DWORD removeFlag)
		{ flags &= ~removeFlag;}
		
		// Returns the current dialog flags.
		DWORD getFlags()
		{ return flags; }

		// Sets the initial color shown when the dialog opens.
		void setInitColor(ws::Hue hue)
		{ initColor = hue; }

		// Returns the initial color value.
		ws::Hue getInitColor()
		{ return initColor; }

		// Returns the color selected by the user after the dialog closes.
		ws::Hue getResult()
		{ return chosenColor; }

		// Opens the color dialog; returns true if a color was chosen.
		bool open()
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
		
	};
	
	
	class Dropdown
	{
		public:
	    
		// Constructor: creates a popup menu for the dropdown.
		Dropdown(int newID, std::string newName)
	    {
	        if (newID != 0) // Leaf items don't need a menu handle
	            handle = CreatePopupMenu();
	        ID = newID;
	        name = newName;
	        isPopup = (newID != 0);
	    }
	    
		// Adds a menu item with an ID, type, and name.
	    void addItem(int id,DWORD type, std::string itemName)
	    {
	        if (isPopup)
	            AppendMenuA(handle, type, id, ws::TO_LPCSTR(itemName));
	    }
	    
		// Adds a submenu (another Dropdown) to this dropdown.
	    void addSubmenu(Dropdown &submenu)
	    {
	        if (isPopup && submenu.isPopup)
	            AppendMenuA(handle, MF_POPUP, (UINT_PTR)submenu.getHandle(), 
	                       ws::TO_LPCSTR(submenu.getName()));
	    }
		
		// Returns the underlying HMENU handle.
		HMENU getHandle()
		{ return handle; }
		
		// Returns the name of the dropdown.
		std::string getName()
		{ return name;}
		
		// Returns the menu item ID.
		int getID()
		{ return ID;}
		
		// Adds another dropdown as a menu item.
		void addItem(Dropdown drop)
		{
			if (isPopup)
				AppendMenuA(this->handle, MF_STRING, drop.getID(), ws::TO_LPCSTR(drop.getName()));
		}
		
		
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

		private:
		ws::Window* windowRef = nullptr;

		public:
		
		// Constructor: creates an empty menu bar.
		Menu()
		{
			bar = CreateMenu();
		}
		
		// Adds a Dropdown (popup menu) to the menu bar.
		void addDropdown(ws::Dropdown &drop)
		{
			AppendMenuA(bar, MF_POPUP, (UINT_PTR)drop.getHandle(), ws::TO_LPCSTR(drop.getName()));
			if(windowRef != nullptr)
				windowRef->setSize(windowRef->getSize());
		}
		
		// Attaches the menu bar to a ws::Window.
		void setWindow(ws::Window &window)
		{
			SetMenu(window.hwnd, bar);	
			windowRef = &window;
			if(windowRef != nullptr)
				windowRef->setSize(windowRef->getSize());
		}
		
		// Extracts the menu command ID from a WM_COMMAND message.
		int getEvent(MSG &m)
		{
			if(m.message == WM_COMMAND)
				return LOWORD(m.wParam);
			return -1;
		}
		
	};


	
	class ClickMenu
	{
		public:
		
		// Adds a flag to the popup menu's behaviour.
		void addFlag(DWORD newFlag)
		{ flags |= newFlag; }
		
		// Removes a flag from the popup menu's behaviour.
		void removeFlag(DWORD removeFlag)
		{ flags &= ~removeFlag;}
		
		// Returns the current flags.
		DWORD getFlags()
		{ return flags; }		
		
		// Retrieves and resets the last selected command ID.
		int getResult()
		{
			int r = command;
			command = 0;
			return r;
		}
		
		// Returns the list of menu item strings.
		std::vector<std::string> getList()
		{ return list; }
		
		// Sets the list of menu item strings.
		void setList(std::vector<std::string> newList)
		{ list = newList; }
		
		// Adds a single menu item string to the list.
		void addItem(std::string item)
		{ list.push_back(item);}
		
		// Removes a menu item string from the list.
		void removeItem(std::string item)
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
		
		// Sets the parent window that will own the context menu.
		void init(ws::Window &newParent)
		{ parentRef = &newParent;}
		
		// Returns the parent window pointer.
		ws::Window *getParent()
		{ return parentRef;}
		
		// Opens the context menu at the given mouse position; returns true.
		bool open(ws::Vec2i mouse)
		{
			if(parentRef == nullptr)
			{
				MessageBoxA(NULL,"Attempted to show a ClickMenu without referencing a parent window! Use Init().","Failed init!",MB_OK | MB_ICONINFORMATION);
				return false;
			}
			HMENU hMenu = CreatePopupMenu();
			if(!hMenu)
				return false;
			
			for(size_t a=0;a<list.size();a++)
			{
				AppendMenu(hMenu, MF_STRING, 1+a, ws::WIDE(list[a]).c_str());
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
		
		private:
		int command = 0;	
		std::vector<std::string> list;	
		ws::Window *parentRef = nullptr;
		DWORD flags = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD;
	};
	
	
	
	
	
	class FileWindow
	{
	    public:
	    
	    // Constructor: initializes a file open/save dialog.
	    FileWindow()
	    {
	        
	    }
	    
		// Sets the initial filename to be shown in the dialog.
	    void setInitResult(std::string file)
	    {
	        fileName = file;
	    }
	    
		// Returns the filename selected by the user.
	    std::string getResult()
	    {
	        return fileName;
	    }
	    
		// Sets the dialog window title.
	    void setTitle(std::string name)
	    { 
	        title = name; 
	    }
	    
		// Returns the dialog window title.
	    std::string getTitle()
	    { 
	        return title; 
	    }
	    
		// Adds a flag to the dialog's behaviour.
	    void addFlag(DWORD newFlag)
	    { 
	        flags |= newFlag; 
	    }
	    
		// Removes a flag from the dialog's behaviour.
	    void removeFlag(DWORD removeFlag)
	    { 
	        flags &= ~removeFlag;
	    }
	    
		// Returns the current dialog flags.
	    DWORD getFlags()
	    { 
	        return flags; 
	    }
	    
		// Opens the file selection dialog; returns true if a file was chosen.
	    bool open(ws::Window *parent = nullptr)
	    {
			
	        std::wstring wtitle = ws::WIDE(title);
	        std::wstring wfileName = ws::WIDE(fileName);
	        
	        wcsncpy(szFile, wfileName.c_str(), MAX_PATH - 1);
	        szFile[MAX_PATH - 1] = L'\0';
	        

	    
	        OPENFILENAMEW ofn = {0};
	        ofn.lStructSize = sizeof(OPENFILENAMEW);
	        ofn.lpstrFilter = L"All Files\0*.*\0";
	        ofn.lpstrFile = szFile;
	        ofn.nMaxFile = MAX_PATH;
	        ofn.lpstrTitle = wtitle.c_str();
	        ofn.Flags = flags;
	        ofn.nFilterIndex = defaultFilter;
	        ofn.lpstrDefExt = L"";


	        if(parent == nullptr)
	            ofn.hwndOwner = NULL;
	        else
	            ofn.hwndOwner = parent->hwnd;

	        
	        if (!fileName.empty()) {
	            std::filesystem::path p(fileName);
	            if (std::filesystem::exists(p)) {
	                std::wstring wdir = ws::WIDE(p.parent_path().string());
	                ofn.lpstrInitialDir = wdir.c_str();
	            }
	        }
	        
	        if(parent)
	        {
	            MSG m;
	            while(parent->pollEvent(m)) {}
	        }
			else
				std::cerr << "Warning: Opening a dialog without specifying a parent window is discouraged due to the fact that dialogs block the message que of a window. \nIf you want to have a window and a dialog, you might want to empty the message queue after opening the dialog.\n";
	        
	        if(GetOpenFileNameW(&ofn))
	        {
	            fileName = ws::SHORT(szFile);
	            return true;
	        }
	        else
	        {
	            fileName.clear();
	            return false;
	        }
	    }
	    
		// Opens the file save dialog; returns true if a file was chosen.
	    bool save(ws::Window *parent = nullptr)
	    {
	        std::wstring wtitle = ws::WIDE(title);
	        std::wstring wfileName = ws::WIDE(fileName);
	        
	        wcsncpy(szFile, wfileName.c_str(), MAX_PATH - 1);
	        szFile[MAX_PATH - 1] = L'\0';
	        

	        OPENFILENAMEW ofn = {0};
	        ofn.lStructSize = sizeof(OPENFILENAMEW);
	        ofn.lpstrFilter = L"All Files\0*.*\0";
	        ofn.lpstrFile = szFile;
	        ofn.nMaxFile = MAX_PATH;
	        ofn.lpstrTitle = wtitle.c_str();
	        ofn.Flags = flags | OFN_OVERWRITEPROMPT;
	        ofn.nFilterIndex = defaultFilter;
	        ofn.lpstrDefExt = L"";


	        if(parent == nullptr)
	            ofn.hwndOwner = NULL;
	        else
	            ofn.hwndOwner = parent->hwnd;
	        

	        
	        if (!fileName.empty()) {
	            std::filesystem::path p(fileName);
	            if (p.has_parent_path()) {
	                std::wstring wdir = ws::WIDE(p.parent_path().string());
	                ofn.lpstrInitialDir = wdir.c_str();
	            }
	        }
	        
	        if(parent)
	        {
	            MSG m;
	            while(parent->pollEvent(m)) {}
	        }
			else
				std::cerr << "Warning: Opening a dialog without specifying a parent window is discouraged due to the fact that dialogs block the message que of a window. \nIf you want to have a window and a dialog, you might want to empty the message queue after opening the dialog.\n";
	        
	        
	        if(GetSaveFileNameW(&ofn))
	        {
	            fileName = ws::SHORT(szFile);
	            return true;
	        }
	        else
	        {
	            fileName.clear();
	            return false;                
	        }
	    }
	    
	    private:
	    
	    DWORD flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NODEREFERENCELINKS | OFN_NOCHANGEDIR | OFN_EXPLORER;    
	    std::string title = "File Explorer";    
	    int defaultFilter = 1;
	    std::string fileName = ""; 
	    
	    //Temporary buffer
	    wchar_t szFile[MAX_PATH] = L"";
	    
	};
	
	
	
	class FolderWindow
	{
	    public:
	    
	    // Constructor: initializes a folder selection dialog.
	    FolderWindow()
	    {
	        
	    }
	    
		// Sets the title of the folder selection dialog.
	    void setTitle(std::string name)
	    {
	        title = name;
	    }
	    
		// Returns the title of the folder selection dialog.
	    std::string getTitle()
	    {
	        return title;
	    }
	    
		// Adds a flag to the dialog's behaviour.
	    void addFlag(DWORD flag)
	    { 
	        flags |= flag;
	    }
	    
		// Replaces all flags with the given value.
	    void setFlags(DWORD allFlags)
	    { 
	        flags = allFlags;
	    }
	    
		// Removes a flag from the dialog's behaviour.
	    void removeFlag(DWORD flag)
	    { 
	        flags &= ~flag;
	    }
	    
		// Returns the current flags.
	    DWORD getFlags()
	    {
	        return flags;
	    }
	    
		// Returns the selected folder path after dialog closes.
	    std::string getResult()
	    { 
	        return folderName;
	    }
	    
		// Opens the folder selection dialog; returns true if a folder was chosen.
	    bool open(ws::Window *parent = nullptr)
	    {
	        std::wstring wtitle = ws::WIDE(title);
	        
	        BROWSEINFOW bi = {0};
	        bi.lpszTitle = wtitle.c_str();
	        bi.ulFlags = flags;
	        
	        if(parent == nullptr)
	            bi.hwndOwner = NULL;
	        else
	            bi.hwndOwner = parent->hwnd;
	        
	        if (!initialFolder.empty()) {
	            std::wstring winitial = ws::WIDE(initialFolder);
	            bi.lParam = (LPARAM)winitial.c_str();
	            bi.lpfn = BrowseCallbackProc;
	            bi.ulFlags |= BIF_NEWDIALOGSTYLE;
	        }
	    
	        if(parent)
	        {
	            MSG m;
	            while(parent->pollEvent(m)) {}
	        }
			else
				std::cerr << "Warning: Opening a dialog without specifying a parent window is discouraged due to the fact that dialogs block the message que of a window. \nIf you want to have a window and a dialog, you might want to empty the message queue after opening the dialog.\n";
	        
	        
	        LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
	        if (pidl != nullptr) {
	            // Get the path of the selected folder
	            wchar_t path[MAX_PATH];
	            if (SHGetPathFromIDListW(pidl, path)) {
	                
					folderName = ws::SHORT(path);
	                
	                // Free the PIDL
	                CoTaskMemFree(pidl);
	                return true;
	            }
	            CoTaskMemFree(pidl);
	        }
	        
	        folderName = "";
	        return false;
	    }
	    
		// Sets the initial folder to be displayed when the dialog opens.
	    void setInitResult(std::string folder)
	    {
	        initialFolder = folder;
	    }
	    
	    private:
	    std::string title = "Select Folder";
	    DWORD flags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	    std::string folderName = "";
	    std::string initialFolder = "";
	    
	    // Callback function used to set the initial folder in the browser dialog.
	    static int __stdcall BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
	    {
	        if (uMsg == BFFM_INITIALIZED) {
	            SendMessageW(hwnd, BFFM_SETSELECTIONW, TRUE, lpData);
	        }
	        return 0;
	    }
	};

	
	class ExploreWindow
	{
		private:
		std::string m_title,m_initialFolder,m_resultName;
		DWORD options;
		HRESULT hr;
		IFileDialog* pfd = nullptr;
		
		public:
		
		// Constructor: creates a modern IFileDialog COM object.
		ExploreWindow()
		{
			
			hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
			
			options = pfd->GetOptions(&options);
			
			addStyle(FOS_NOCHANGEDIR);
		}
		
		// Adds a style flag to the file dialog.
		void addStyle(DWORD style)
		{
			options |= style;
		}
		// Removes a style flag from the file dialog.
		void removeStyle(DWORD style)
		{
			options &= ~style;
		}
		
		// Sets the dialog title.
		void setTitle(std::string title) {m_title = title;}
		// Sets the initial folder or result.
		void setInitResult(std::string folder) {m_initialFolder = folder;}
		
		// Returns the selected file or folder path.
		std::string getResult() {return m_resultName;}
		
		// Opens the modern file dialog; returns true if a file/folder was selected.
		bool open(ws::Window *parent = nullptr)
		{

			bool result = true;
			if(SUCCEEDED(hr))
			{
				pfd->SetOptions(options);				
				
				if (!m_title.empty())
				{
					std::wstring wtitle = ws::WIDE(m_title);
					pfd->SetTitle(wtitle.c_str());
				}				
				
				if (!m_initialFolder.empty())
				{
					IShellItem* psiInitial = nullptr;
					std::wstring wInitial = ws::WIDE(m_initialFolder);
					hr = SHCreateItemFromParsingName(wInitial.c_str(), nullptr, IID_PPV_ARGS(&psiInitial));
					if (SUCCEEDED(hr))
					{
						pfd->SetFolder(psiInitial);
						psiInitial->Release();
					}
				}				
				
				HWND parentHwnd = nullptr;
				if(parent)
					parentHwnd = parent->hwnd;
				
				
				hr = pfd->Show(parentHwnd);
				if (SUCCEEDED(hr))
				{
					IShellItem* psiResult = nullptr;
					hr = pfd->GetResult(&psiResult);
					if (SUCCEEDED(hr))
					{
						PWSTR pszPath = nullptr;
						hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
						if (SUCCEEDED(hr))
						{
							m_resultName = ws::SHORT(pszPath);
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
		
		
	};


    class ListBox : public Child 
	{
        std::vector<std::string> pendingItems;
		public:
        
		// Constructor: sets the LISTBOX class and standard styles.
		ListBox()
		{
            addStyle(LBS_STANDARD | WS_VSCROLL | WS_HSCROLL | LBS_NOTIFY);
			setClass(L"LISTBOX");
		}
        
		// Initializes the list box and adds any pending items.
		virtual bool init(ws::Window &parent) override 
		{
			if(!Child::init(parent)) return false;

			if(!getFontHandle())
			{
				ws::Font font;
				font.loadFromSystem("Arial");
				ws::Text text;
				text.setCharacterSize(15);
				setFont(font,text);
			}
            
			for (const auto& item : pendingItems) addItem(item);
            
			pendingItems.clear();
			
            return true;
        }
		
		// Adds a string item to the list box.
        void addItem(const std::string& item) 
		{
            if (!hwnd) 
			{
				pendingItems.push_back(item); 
				return;
			}
            SendMessageA(hwnd, LB_ADDSTRING, 0, (LPARAM)item.c_str());
        }

		// Adds multiple string items to the list box.
		void addItems(const std::vector<std::string>& items)
		{
			if (!hwnd) {
				for (const auto& item : items) pendingItems.push_back(item);
				return;
			}
			for (const auto& item : items) addItem(item);
		}

		// Removes the item at the given index.
		void removeItem(int index)
		{
			if (!hwnd) return;
			SendMessageA(hwnd, LB_DELETESTRING, (WPARAM)index, 0);
		}
        
		// Clears all items from the list box.
		void clear() 
		{
            if (!hwnd) { pendingItems.clear(); return; }
            SendMessageA(hwnd, LB_RESETCONTENT, 0, 0);
        }
		
		// Returns the index of the currently selected item.
        int getSelectedIndex() 
		{
            return hwnd ? (int)SendMessageA(hwnd, LB_GETCURSEL, 0, 0) : -1;
        }
		
		// Returns the text of the currently selected item.
        std::string getSelectedText() 
		{
            int index = getSelectedIndex();
            return (index == -1) ? "" : getItemText(index);
        }
		
		// Returns the text of the item at the given index.
        std::string getItemText(int index) 
		{
            if (!hwnd || index < 0) return "";
           
			int len = (int)SendMessage(hwnd, LB_GETTEXTLEN, (WPARAM)index, 0);
            
			if (len == LB_ERR) return "";
            
			std::vector<char> buf(len + 1);
            SendMessageA(hwnd, LB_GETTEXT, (WPARAM)index, (LPARAM)buf.data());
            return std::string(buf.data());
        }
        
		// Sets the selected item by index.
		void setSelectedIndex(int index) 
		{
            if (hwnd) 
				SendMessage(hwnd, LB_SETCURSEL, (WPARAM)index, 0);
        }
        
		// Checks if a WM_COMMAND message indicates a selection change.
		bool selectionChanged(MSG &msg) 
		{
            return msg.message == WM_COMMAND && HIWORD(msg.wParam) == LBN_SELCHANGE && LOWORD(msg.wParam) == controlID;
        }
        
		// Returns the total number of items in the list box.
		int getItemCount() 
		{
            return hwnd ? (int)SendMessage(hwnd, LB_GETCOUNT, 0, 0) : 0;
        }
    };	
	

}

#endif