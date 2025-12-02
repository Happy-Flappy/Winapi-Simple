


namespace ws
{
	
	class Window
	{
		public:
		
		HWND hwnd;
	    int x,y,width, height;		
		bool isTransparent = false;

		ws::Input input;


		private:
			
		bool isRunning = true;
		std::queue<MSG> msgQ;
		DWORD style = WS_OVERLAPPEDWINDOW;
		
		
	    bool legacyTransparency = false;
				
		public:		
		
		
		View view;
		
		
        ws::Texture backBuffer;
		ws::Texture displayBuffer;
		HDC backBufferDC;
		HDC displayBufferDC;
		
		INITCOMMONCONTROLSEX icex;
		
		Window(int width,int height,std::string title="",DWORD newStyle = WS_OVERLAPPEDWINDOW)
		{
			
			style = newStyle;
			
			addStyle(WS_CLIPCHILDREN);
			
			
			DWORD exStyle = 0;
		    exStyle |= WS_EX_LAYERED;
		    
			
			
			
			
			//This is for initialization of winapi child objects sucg as buttons and textboxes.
			icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
			icex.dwICC = ICC_STANDARD_CLASSES;  // Enables a set of common controls.
			InitCommonControlsEx(&icex);
			///////////////////////////////
			
			
			
			view.setRect({0,0,width,height});//Sets world rect
			view.setPortRect({0,0,width,height});//Sets viewport rect
			
			
			
			isRunning = true;
			
			
			HINSTANCE hInstance = GetModuleHandle(nullptr);
			
		    LPCSTR CLASS_NAME = "Window";
		    
		    WNDCLASS wc = {};
		    wc.lpfnWndProc = ws::Window::StaticWindowProc;
		    wc.hInstance = hInstance;
		    wc.lpszClassName = CLASS_NAME;
		    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		    wc.hbrBackground = nullptr;
		
		    if (!RegisterClass(&wc)) {
		        std::cerr << "Failed to register window class!" << std::endl;
		        exit(-1);
		    }		
			
			
			
			hwnd = CreateWindowEx(
			exStyle,
			CLASS_NAME,
			TO_LPCSTR(title),
			style,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			width,
			height,
			nullptr,
			nullptr,
			hInstance,
			this
			);
			
			
		    if (hwnd == nullptr) {
		        std::cerr << "Failed to create window!" << std::endl;
		        exit(-1);
		    }			
			
			
			
			
			
			
			
			
			backBuffer.create(view.getSize().x,view.getSize().y);
			displayBuffer.create(width,height);

	        HDC hdc = GetDC(hwnd);
	        backBufferDC = CreateCompatibleDC(hdc);
	        SelectObject(backBufferDC, backBuffer.bitmap);
	        
			displayBufferDC = CreateCompatibleDC(hdc);
            SelectObject(displayBufferDC, displayBuffer.bitmap);
			
			ReleaseDC(hwnd, hdc);			
						
					
			
			
			clear();
			
			
			
			
			
			
            windowInstances[hwnd] = this;			


            ShowWindow(hwnd, SW_SHOW);
            UpdateWindow(hwnd);


			//enable anti-aliasing
	        SetStretchBltMode(backBufferDC, HALFTONE);
	        SetGraphicsMode(backBufferDC, GM_ADVANCED);
	        //Force original orientation to avoid rotations in copy operations.
			SetLayout(backBufferDC, LAYOUT_BITMAPORIENTATIONPRESERVED);


		    POINT ptZero = {0, 0};
		    SIZE size = {width, height};
		    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
		    
		    HDC hdcScreen = GetDC(NULL);
		    UpdateLayeredWindow(hwnd, hdcScreen, &ptZero, &size, 
		                      backBufferDC, &ptZero, 0, &blend, ULW_ALPHA);
		    ReleaseDC(NULL, hdcScreen);
			
		}
		
		
		
        ~Window()
        {
            windowInstances.erase(hwnd);
        	DeleteDC(backBufferDC);
        	DeleteDC(displayBufferDC);    
        }
		
		
		
		
		
		void setView(View &v)
		{
			view = v;
			backBuffer.create(view.getSize().x,view.getSize().y);
		}
		
		
		
		void addStyle(DWORD addedStyle)
		{
            style |= addedStyle;
			if (hwnd)
            {
                SetWindowLong(hwnd, GWL_STYLE, style);
                SetWindowPos(hwnd, NULL, 0, 0, 0, 0, 
                           SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
		}
		
		void removeStyle(DWORD removedStyle)
		{
			
			style &= ~removedStyle;
			if (hwnd)
            {
                SetWindowLong(hwnd, GWL_STYLE, style);
                SetWindowPos(hwnd, NULL, 0, 0, 0, 0, 
                           SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }		
		}
		
        bool hasStyle(DWORD checkStyle)
        {
            if (hwnd)
            {
                DWORD currentStyle = GetWindowLong(hwnd, GWL_STYLE);
                return (currentStyle & checkStyle) != 0;
            }
        }
        
        
        DWORD getStyle()
        {
        	return style;
		}
        
        
        
    	
		public:	
			

		
		
		
		
		
		
		
		void setLayerAfter(HWND lastHwnd)
		{
			SetWindowPos(hwnd,lastHwnd,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
		}
		    	    
	    


	    void setFullscreen(bool fullscreen) {
	        if (fullscreen == isFullscreen) return;
	        
	        if (fullscreen) {
	            windowedStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	            GetWindowRect(hwnd, &windowedRect);
	            
	            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	            int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	            
	            SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	            SetWindowPos(hwnd, 
				HWND_TOP, 
				0, 
				0, 
				screenWidth, 
				screenHeight,
				SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	            
	            isFullscreen = true;
	        } else {
	            SetWindowLong(hwnd, GWL_STYLE, windowedStyle);
	            SetWindowPos(hwnd, HWND_TOP, 
	            windowedRect.left, 
				windowedRect.top,
	            windowedRect.right - windowedRect.left,
	            windowedRect.bottom - windowedRect.top,
	            SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	            
	            isFullscreen = false;
	        }
	    }
	    
	    void toggleFullscreen() {
	        setFullscreen(!isFullscreen);
	    }
	    
	    bool getFullscreen() const {
	        return isFullscreen;
	    }


				
		
		
		
		bool isOpen()
		{
			
			input.beginFrame();
			
			MSG msg;
	        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
	            if (msg.message == WM_QUIT) {
	                isRunning = false;
	                break;
	            }
	            
	            TranslateMessage(&msg);
	            DispatchMessage(&msg);
	            
	            input.update(msg);
	            msgQ.push(msg);
	            
	        }
            
			return isRunning;

		}
		
		
		
		
		
	
		
		
		
	    bool pollEvent(MSG &message) {
	        if (msgQ.empty()) {
	            return false;
	        }
	        
	        message = msgQ.front();
	        msgQ.pop();
	        
	        
	        
	        return true;
	    }	
		
		
		
		
		
		
		
		
		
		
		
		
	    void clear(ws::BGRA color = ws::BGRA(0,0,0,255)) 
		{
			
	        if (backBuffer.isValid()) {
	            backBuffer.clear(color);
	        }
			
	    }
		
		
		
		
		
		
	    void drawPixel(ws::BGRA color,int x, int y)
	    {
	        backBuffer.setPixel(color,x, y);
	    }		
		
		
		
		
		
		
		void draw(Drawable &draw)
		{
		    draw.draw(backBufferDC,view);
		}
		
		
		
		
		
	
		
		
		void display() 
		{


			///METHOD THAT WORKS BUT DOES NOT SEPARATE SCALING FROM UPDATELAYEREDWINDOW
		    POINT ptDst = {x, y};
		    SIZE size = {width, height};
		    POINT ptSrc = {0, 0};
		    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};


			HDC hdcScreen = GetDC(NULL);
		    
		    // Get directly from displayBuffer to screen
		    UpdateLayeredWindow(hwnd, hdcScreen, &ptDst, &size, 
		                      backBufferDC, &ptSrc, 
		                      0, &blend, ULW_ALPHA);
			

		    ReleaseDC(NULL, hdcScreen);
		    ////////////////////////

	

		}	
				
		
		
		
		
		private:
			
			
			
		
		

        // Static map to store window instances
        static std::map<HWND, Window*> windowInstances;		

		
        static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            Window* pWindow = nullptr;
            
            if (uMsg == WM_NCCREATE) {
                CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
                pWindow = reinterpret_cast<Window*>(pCreate->lpCreateParams);
                SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
            } else {
                pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            }
            
            if (pWindow) {
                return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
            }
            
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }	
		
	
        LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {


            switch (uMsg) {
	            case WM_DESTROY:
	                PostQuitMessage(0);
	                isRunning = false;
	                return 0;
	                
	            case WM_PAINT: {
	            	
	            	
	                PAINTSTRUCT ps;
	                HDC hdc = BeginPaint(hwnd, &ps);
	             	
	                
	                EndPaint(hwnd, &ps);
	                return 0;
	            }


				case WM_SIZE:	                
				{
				
				   
				    
					width = LOWORD(lParam);
	                height = HIWORD(lParam);
	                
	                
	                
	                
	                //viewport size should be updated here to keep proportion even after window has changed size.
	                //for now we will wait to do that. For now we will just set the viewport as the window size.
					
					view.setPortSize({width,height});
					
					displayBuffer.create(width, height);
						                
	                clear();
	                
	                
	            	return 0;
				}
				
				
				case WM_MOVE:
				{
					x = GET_X_LPARAM(lParam);
					y = GET_Y_LPARAM(lParam);
				}
				
				
	        }
	        
	        
	        //Secondary message adding because not all messages are received always.
		    MSG msg;
		    msg.hwnd = hwnd;
		    msg.lParam = lParam;
		    msg.wParam = wParam;
		    msg.message = uMsg;
		    msgQ.push(msg);
            
            
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        
        
        
        
				
		
		
		
		bool isFullscreen = false;
		RECT windowedRect; // Stores window position/size when not fullscreen
    	DWORD windowedStyle; // Stores window style when not fullscreen
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
			
	};
	
	// Initialize the static map
    std::map<HWND, Window*> Window::windowInstances;
    
    	
	
	
}