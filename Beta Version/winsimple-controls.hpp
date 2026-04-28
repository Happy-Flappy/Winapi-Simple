
#ifndef WINSIMPLE_CONTROLS
#define WINSIMPLE_CONTROLS

#include <commctrl.h>   // for common controls (trackbar, etc.)
#include <shlobj.h>     // for folder browser (BROWSEINFO, etc.)


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

		
		Child(const std::wstring& className = L"Button") : m_className(ws::SHORT(className))
		{
			controlID = controlsInit.maxControlID++;
		}
		
		
		virtual ~Child()
		{
			for(auto& cb : m_destructorCallbacks)
				cb(this);
			
			if (hwnd && IsWindow(hwnd))
				DestroyWindow(hwnd);
			if (customFont)
				DeleteObject(customFont);
		}

		// init – called by ws::Window::addChild()
		virtual bool init(ws::Window& parent)
		{
			return init(parent.hwnd);
		}
		
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

		void registerDestructorCallback(DestructorCallback callback)
		{
			m_destructorCallbacks.push_back(std::move(callback));
		}
		void clearDestructorCallbacks() 
		{
			m_destructorCallbacks.clear();
		}


		void addChild(ws::Child& child) 
		{
			children.push_back(&child);

			
			if(this->hwnd)
				child.init(this->hwnd);  
		}

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
		

		void setClass(const std::wstring& className = L"Button")
		{
			m_className = ws::SHORT(className);
			if(hwnd)
			{
				init(GetParent(hwnd));
			}
		}
		
		std::string getClass()
		{
			return m_className;
		}
		
		void setVisible(bool visible)
		{
			ShowWindow(hwnd,(visible) ? SW_SHOW : SW_HIDE);
		}

		
		void setPosition(int xPos, int yPos) 
		{ 
			x = xPos; 
			y = yPos; 
			if (hwnd) 
				SetWindowPos(hwnd, nullptr, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE); 
		}
		
		void setPosition(ws::Vec2i pos) 
		{ 
			setPosition(pos.x, pos.y); 
		}
		
		ws::Vec2i getPosition() const 
		{ 
			return {x, y}; 
		}

		virtual void setSize(int w, int h) 
		{ 
			width = w; 
			height = h; 
			if (hwnd) 
				SetWindowPos(hwnd, nullptr, x, y, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE); 
		}
		
		virtual void setSize(ws::Vec2i size) 
		{ 
			setSize(size.x, size.y); 
		}
		
		ws::Vec2i getSize() const 
		{ 
			return {width, height}; 
		}
		
		HFONT getFontHandle()
		{
			return customFont;
		}

		void addStyle(DWORD addedStyle) 
		{ 
			style |= addedStyle; 
			if (hwnd) 
			{ 
				SetWindowLong(hwnd, GWL_STYLE, style); 
				SetWindowPos(hwnd, NULL, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED); 
			} 
		}
		
		void removeStyle(DWORD removedStyle) 
		{ 
			style &= ~removedStyle; 
			if (hwnd) 
			{ 
				SetWindowLong(hwnd, GWL_STYLE, style); 
				SetWindowPos(hwnd, NULL, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED); 
			} 
		}
		
		bool hasStyle(DWORD checkStyle) const 
		{ 
			DWORD current = GetWindowLong(hwnd, GWL_STYLE); 
			return (current & checkStyle) != 0; 
		}


		void addExStyle(DWORD addedStyle) 
		{ 
			exStyle |= addedStyle; 
			if (hwnd) 
			{ 
				SetWindowLong(hwnd, GWL_EXSTYLE, exStyle); 
				SetWindowPos(hwnd, NULL, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED); 
			} 
		}
		
		void removeExStyle(DWORD removedStyle) 
		{ 
			exStyle &= ~removedStyle; 
			if (hwnd) 
			{ 
				SetWindowLong(hwnd, GWL_EXSTYLE, exStyle); 
				SetWindowPos(hwnd, NULL, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED); 
			} 
		}
		
		bool hasExStyle(DWORD checkStyle) const 
		{ 
			DWORD current = GetWindowLong(hwnd, GWL_EXSTYLE); 
			return (current & checkStyle) != 0; 
		}
		

		void setText(const std::string& newText) 
		{ 
			text = newText; 
			if (hwnd) 
				SetWindowTextA(hwnd, text.c_str()); 
		}
		
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

		bool contains(ws::Vec2i point) const 
		{ 
			return (point.x >= x && point.x < x+width && point.y >= y && point.y < y+height); 
		}

		// For message handling – derived classes can override if needed
		virtual bool handleCommand(MSG &msg) { return false; }
		virtual bool handleNotify(NMHDR* pnmh) { return false; }
	};
	

	
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
		
		Tabs()
		{
			setClass(L"SysTabControl32");
			addStyle(TCS_FIXEDWIDTH | TCS_RIGHTJUSTIFY  | WS_CLIPSIBLINGS);			
		}
		
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

		int getSelected() const
		{
			return TabCtrl_GetCurSel(hwnd);
		}

		virtual void setSize(int w, int h) override
		{
			Child::setSize(w, h);
			updatePagePositions();
		}
		
		
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
		
		ComboBox()
		{
			setClass(L"COMBOBOX");
	        addStyle(CBS_DROPDOWN);
	        addStyle(WS_VSCROLL);			
		}
		
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
		
	    void addItem(const std::string& item)
	    {
			if (!hwnd) {
				// store for later if hwnd doesn't exist yet
				pendingItems.push_back(item);
				return;
			}
	        SendMessageA(hwnd, CB_ADDSTRING, 0, (LPARAM)item.c_str());
	    }
	    
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
	    
	    void removeItem(int index)
	    {
	        if (!hwnd) return;
	        SendMessage(hwnd, CB_DELETESTRING, (WPARAM)index, 0);
	    }
	    
	    void clear()
	    {
	        if (!hwnd) return;
	        SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
	    }
	    
	    int getSelectedIndex()
	    {
	        if (!hwnd) return -1;
	        return (int)SendMessage(hwnd, CB_GETCURSEL, 0, 0);
	    }
	    
	    void setSelectedIndex(int index)
	    {
	        if (!hwnd) return;
	        SendMessage(hwnd, CB_SETCURSEL, (WPARAM)index, 0);
	    }
	    
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
	    
	    int getItemCount()
	    {
	        if (!hwnd) return 0;
	        return (int)SendMessage(hwnd, CB_GETCOUNT, 0, 0);
	    }
	    
	    std::string getItemText(int index)
	    {
	        if (!hwnd || index < 0) return "";
	        
	        int length = (int)SendMessage(hwnd, CB_GETLBTEXTLEN, (WPARAM)index, 0);
	        if (length == CB_ERR) return "";
	        
	        std::vector<char> buffer(length + 1);
	        SendMessageA(hwnd, CB_GETLBTEXT, (WPARAM)index, (LPARAM)buffer.data());
	        
	        return std::string(buffer.data());
	    }

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
	    
	    void setEditText(const std::string& text)
	    {
	        if (!hwnd) return;
	        SendMessageA(hwnd, WM_SETTEXT, 0, (LPARAM)text.c_str());
	    }		
	};
	
	
	
	class Button : public Child
	{
		public:
		
		Button()
		{
			setClass(L"Button");
		}
		
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
		
		Slider()
		{
			setClass(TRACKBAR_CLASS);
			addStyle(TBS_HORZ);
			addStyle(TBS_AUTOTICKS);	
			setRange(0,100);			
		}
		
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
	    
	    
	    void setHorizontal()
	    {
	    	removeStyle(TBS_VERT);
	    	addStyle(TBS_HORZ);
		}
		
		void setVertical()
		{
	    	removeStyle(TBS_HORZ);
	    	addStyle(TBS_VERT);
		}
		
        
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
		
		TextBox()
		{
			setClass(L"EDIT");
			addStyle(ES_AUTOVSCROLL);
			addStyle(ES_AUTOHSCROLL);
			addStyle(ES_MULTILINE);
			addExStyle(WS_EX_CLIENTEDGE);
		}
		
		
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
		void setCharacterLimit(int max_chars = 0)//0 is infinite
		{
			if(!hwnd)
				char_limit = max_chars;
			else
				SendMessage(hwnd, EM_SETLIMITTEXT, (WPARAM)max_chars, 0);			
		}
		
		
		bool getFocus()
		{
	        if (!hwnd) return false;
	        return (GetFocus() == hwnd);			
		}
		
	};
	
	
	
	
	
	class Label : public Child
	{
		public:
		Label()
		{
			setClass(L"STATIC");
			addStyle(SS_NOTIFY);
			addStyle(SS_LEFT);			
		}
		
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

		void init(ws::Window &newParent)
		{ parentRef = &newParent;}

		ws::Window *getParent()
		{ return parentRef;}

		void addFlag(DWORD newFlag)
		{ flags |= newFlag; }
		
		void removeFlag(DWORD removeFlag)
		{ flags &= ~removeFlag;}
		
		DWORD getFlags()
		{ return flags; }

		void setInitColor(ws::Hue hue)
		{ initColor = hue; }

		ws::Hue getInitColor()
		{ return initColor; }

		ws::Hue getResult()
		{ return chosenColor; }

		
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
	    
		Dropdown(int newID, std::string newName)
	    {
	        if (newID != 0) // Leaf items don't need a menu handle
	            handle = CreatePopupMenu();
	        ID = newID;
	        name = newName;
	        isPopup = (newID != 0);
	    }
	    
	    void addItem(int id,DWORD type, std::string itemName)
	    {
	        if (isPopup)
	            AppendMenuA(handle, type, id, ws::TO_LPCSTR(itemName));
	    }
	    
	    void addSubmenu(Dropdown &submenu)
	    {
	        if (isPopup && submenu.isPopup)
	            AppendMenuA(handle, MF_POPUP, (UINT_PTR)submenu.getHandle(), 
	                       ws::TO_LPCSTR(submenu.getName()));
	    }
		
		HMENU getHandle()
		{ return handle; }
		
		std::string getName()
		{ return name;}
		
		int getID()
		{ return ID;}
		
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
		
		Menu()
		{
			bar = CreateMenu();
		}
		
		void addDropdown(ws::Dropdown &drop)
		{
			AppendMenuA(bar, MF_POPUP, (UINT_PTR)drop.getHandle(), ws::TO_LPCSTR(drop.getName()));
			if(windowRef != nullptr)
				windowRef->setSize(windowRef->getSize());
		}
		
		void setWindow(ws::Window &window)
		{
			SetMenu(window.hwnd, bar);	
			windowRef = &window;
			if(windowRef != nullptr)
				windowRef->setSize(windowRef->getSize());
		}
		
		
		int getEvent(MSG &m)//You can use this for readability or you can use the normal way.
		{
			if(m.message == WM_COMMAND)
				return LOWORD(m.wParam);
			return -1;
		}
		
	};


	
	class ClickMenu
	{
		public:
		
		
		
		void addFlag(DWORD newFlag)
		{ flags |= newFlag; }
		
		void removeFlag(DWORD removeFlag)
		{ flags &= ~removeFlag;}
		
		DWORD getFlags()
		{ return flags; }		
		
		
		int getResult()
		{
			int r = command;
			command = 0;
			return r;
		}
		
		std::vector<std::string> getList()
		{ return list; }
		
		void setList(std::vector<std::string> newList)
		{ list = newList; }
		
		void addItem(std::string item)
		{ list.push_back(item);}
		
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
		
		void init(ws::Window &newParent)
		{ parentRef = &newParent;}
		
		ws::Window *getParent()
		{ return parentRef;}
		
		
		
		
		
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
	    
	    FileWindow()
	    {
	        
	    }
	    
	    void setInitResult(std::string file)
	    {
	        fileName = file;
	    }
	    
	    std::string getResult()
	    {
	        return fileName;
	    }
	    
	    void setTitle(std::string name)
	    { 
	        title = name; 
	    }
	    
	    std::string getTitle()
	    { 
	        return title; 
	    }
	    
	    void addFlag(DWORD newFlag)
	    { 
	        flags |= newFlag; 
	    }
	    
	    void removeFlag(DWORD removeFlag)
	    { 
	        flags &= ~removeFlag;
	    }
	    
	    DWORD getFlags()
	    { 
	        return flags; 
	    }
	    
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
	    
	    FolderWindow()
	    {
	        
	    }
	    
	    void setTitle(std::string name)
	    {
	        title = name;
	    }
	    
	    std::string getTitle()
	    {
	        return title;
	    }
	    
	    void addFlag(DWORD flag)
	    { 
	        flags |= flag;
	    }
	    
	    void setFlags(DWORD allFlags)
	    { 
	        flags = allFlags;
	    }
	    
	    void removeFlag(DWORD flag)
	    { 
	        flags &= ~flag;
	    }
	    
	    DWORD getFlags()
	    {
	        return flags;
	    }
	    
	    std::string getResult()
	    { 
	        return folderName;
	    }
	    
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
	    
	    void setInitResult(std::string folder)
	    {
	        initialFolder = folder;
	    }
	    
	    private:
	    std::string title = "Select Folder";
	    DWORD flags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	    std::string folderName = "";
	    std::string initialFolder = "";
	    
	    // callback function for setting initial folder
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
		
		
		ExploreWindow()
		{
			
			hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
			
			options = pfd->GetOptions(&options);
			
			addStyle(FOS_NOCHANGEDIR);
		}
		
		
		void addStyle(DWORD style)
		{
			options |= style;
		}
		void removeStyle(DWORD style)
		{
			options &= ~style;
		}
		
		void setTitle(std::string title) {m_title = title;}
		void setInitResult(std::string folder) {m_initialFolder = folder;}
		
		std::string getResult() {return m_resultName;}
		
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
        
		ListBox()
		{
            addStyle(LBS_STANDARD | WS_VSCROLL | WS_HSCROLL | LBS_NOTIFY);
			setClass(L"LISTBOX");
		}
        
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
		
        void addItem(const std::string& item) 
		{
            if (!hwnd) 
			{
				pendingItems.push_back(item); 
				return;
			}
            SendMessageA(hwnd, LB_ADDSTRING, 0, (LPARAM)item.c_str());
        }

		void addItems(const std::vector<std::string>& items)
		{
			if (!hwnd) {
				for (const auto& item : items) pendingItems.push_back(item);
				return;
			}
			for (const auto& item : items) addItem(item);
		}

		void removeItem(int index)
		{
			if (!hwnd) return;
			SendMessageA(hwnd, LB_DELETESTRING, (WPARAM)index, 0);
		}
        
		void clear() 
		{
            if (!hwnd) { pendingItems.clear(); return; }
            SendMessageA(hwnd, LB_RESETCONTENT, 0, 0);
        }
		
        int getSelectedIndex() 
		{
            return hwnd ? (int)SendMessageA(hwnd, LB_GETCURSEL, 0, 0) : -1;
        }
		
        std::string getSelectedText() 
		{
            int index = getSelectedIndex();
            return (index == -1) ? "" : getItemText(index);
        }
		
        std::string getItemText(int index) 
		{
            if (!hwnd || index < 0) return "";
           
			int len = (int)SendMessage(hwnd, LB_GETTEXTLEN, (WPARAM)index, 0);
            
			if (len == LB_ERR) return "";
            
			std::vector<char> buf(len + 1);
            SendMessageA(hwnd, LB_GETTEXT, (WPARAM)index, (LPARAM)buf.data());
            return std::string(buf.data());
        }
        
		void setSelectedIndex(int index) 
		{
            if (hwnd) 
				SendMessage(hwnd, LB_SETCURSEL, (WPARAM)index, 0);
        }
        
		bool selectionChanged(MSG &msg) 
		{
            return msg.message == WM_COMMAND && HIWORD(msg.wParam) == LBN_SELCHANGE && LOWORD(msg.wParam) == controlID;
        }
        
		int getItemCount() 
		{
            return hwnd ? (int)SendMessage(hwnd, LB_GETCOUNT, 0, 0) : 0;
        }
    };	
	

}

#endif