#ifndef WINAPI_SIMPLE_GRAPHICS
#define WINAPI_SIMPLE_GRAPHICS







#define NOMINMAX        
#define STRICT          
#define WIN32_LEAN_AND_MEAN

 
#include <windows.h>  
#include <windowsx.h>    
#include <iostream>
#include <string>
#include <cstdlib>      
#include <map>

#include <queue>
#include <iomanip>










namespace ws // - Win32 Simple
{

	
	
	
	
	
	
	
	
	
	
	
	
	class Timer
	{
		public:
		
		
		
		Timer()
		{
			LARGE_INTEGER freq;
            QueryPerformanceFrequency(&freq);
            frequency = static_cast<double>(freq.QuadPart);
			restart();
		}
		
		~Timer()
		{
		}
		
		double restart()
		{
	        double seconds = getSeconds();
            LARGE_INTEGER counter;
            QueryPerformanceCounter(&counter);
            startTime = counter.QuadPart;
            
            return seconds;
		}
		
		
	    double getSeconds() const
	    {
	        LARGE_INTEGER currentTime;
	        QueryPerformanceCounter(&currentTime);
	        return static_cast<double>(currentTime.QuadPart - startTime) / frequency;
	      
	    }
	    
	    double getMilliSeconds() const
	    {
	        return getSeconds() * 1000.0;
	    }
	    
	    double getMicroSeconds() const
	    {
	        return getMilliSeconds() * 1000.0;
	    }
	
		private:
		    LONGLONG startTime = 0;
		    double frequency = 1.0;	
		
			
	};
	
	
	
	
	

	
	
	
	struct Vector2f 
	{
		float x,y;
	};
	
	

	
	
	
	class View
	{
		public:
			
		View()
		{
			
		}
		
		void setRect(RECT viewRect)
		{
			world = viewRect;
		}
		
		void setSize(POINT size)
		{
			world.right = size.x;
			world.bottom = size.y;
		}
		
		void setPos(POINT pos)
		{
			world.left = pos.x;
			world.top = pos.y;
		}
		
		
		void setPortRect(RECT portRect)
		{
			port = portRect;
		}
		
		void setPortSize(POINT size)
		{
			port.right = size.x;
			port.bottom = size.y;
		}
		
		void setPortPos(POINT pos)
		{
			port.left = pos.x;
			port.top = pos.y;
		}
		
		
		
		
		RECT getRect()
		{
			return world;
		}
		
		POINT getSize()
		{
			POINT p;
			p.x = world.right;
			p.y = world.bottom;
			
			return p;
		}
		
		POINT getPos()
		{
			POINT p;
			p.x = world.left;
			p.y = world.top;
			
			return p;
		}
		
		RECT getPortRect()
		{
			return port;
		}
		
		POINT getPortSize()
		{
			POINT p;
			p.x = port.right;
			p.y = port.bottom;
			
			return p;
		}
		
		POINT getPortPos()
		{
			POINT p;
			p.x = port.left;
			p.y = port.top;
			
			return p;
		}
		
		
		
		
	    void zoom(float factor)
	    {
	    	if(factor != 0)
	    	{
			
		    	long int x = world.right / factor;
		    	long int y = world.bottom / factor;
				setPortSize({x,y});	// If factor is 2, that means that the visible world area is half as much because it is zooming in and will  be stretching into the viewport.
	    		world.left += x;
	    		world.top += y;
			}
		}


		void move(POINT delta)
		{
			world.left += delta.x;
			world.top += delta.y;
		}
		
		void movePort(POINT delta)
		{
			port.left += delta.x;
			port.top += delta.y;
		}
		
		
		
		
	    POINT toWorld(POINT pos) 
	    {
		    POINT worldSize = getSize();
		    POINT viewSize = getPortSize();
		    POINT viewPos = getPos();
		    
		    POINT worldPoint;
		    
		    float scaleX = static_cast<float>(worldSize.x) / viewSize.x;
		    float scaleY = static_cast<float>(worldSize.y) / viewSize.y;
		    
		    worldPoint.x = static_cast<int>(pos.x * scaleX) + viewPos.x;
		    worldPoint.y = static_cast<int>(pos.y * scaleY) + viewPos.y;
		    
		    return worldPoint;
	    }
	    
	    POINT toWindow(POINT pos) 
	    {
		    POINT worldSize = getSize();
		    POINT viewSize = getPortSize();
		    POINT viewPos = getPos();
		    
		    POINT windowPoint;
		    
		    float scaleX = static_cast<float>(viewSize.x) / worldSize.x;
		    float scaleY = static_cast<float>(viewSize.y) / worldSize.y;
		    
		    windowPoint.x = static_cast<int>((pos.x - viewPos.x) * scaleX);
		    windowPoint.y = static_cast<int>((pos.y - viewPos.y) * scaleY);
		    
		    return windowPoint;
	    }
				
		
		
		
		
		
		
		
		
		private:
		RECT world;
		RECT port;
		
			
	};
	
	

	
	
	
	std::wstring WIDE(std::string str)
	{
	    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	    std::wstring wstr(size, 0);
	    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
	    return wstr;
	}
	
	
	
	LPCSTR TO_LPCSTR(std::string &str)
	{
		return LPCSTR(str.c_str());
	}
		
	

	
	
	
	
	
	class Texture
	{
		public:
		
		HBITMAP bitmap;
		int width = 0;
		int height = 0;
		
		
		Texture() = default;
		
		Texture(std::string path)
		{
			load(path);
		}
		
		
	    ~Texture()
	    {
	        if (bitmap != NULL)
	        {
	            DeleteObject(bitmap);
	            bitmap = NULL;
	        }
	    }		
		




	    Texture(Texture&& other) noexcept
	        : bitmap(other.bitmap), width(other.width), height(other.height)
	    {
	        other.bitmap = NULL;
	        other.width = 0;
	        other.height = 0;
	    }


		
	    Texture& operator=(Texture&& other) noexcept
	    {
	        if (this != &other)
	        {
	            if (bitmap != NULL)
	            {
	                DeleteObject(bitmap);
	            }
	            
	            bitmap = other.bitmap;
	            width = other.width;
	            height = other.height;
	            
	            other.bitmap = NULL;
	            other.width = 0;
	            other.height = 0;
	        }
	        return *this;
	    }		
		
		
	
	
	
	
		bool load(std::string path)
		{


	        if (bitmap != NULL)
	        {
	            DeleteObject(bitmap);
	            bitmap = NULL;
	            width = 0;
	            height = 0;
	        }



			bitmap = (HBITMAP)LoadImageW(
			NULL,
			WIDE(path).c_str(),
			IMAGE_BITMAP,
			0,
			0,
			LR_LOADFROMFILE
			);
			
			
			
			if(bitmap == NULL)
			{
				std::cerr << "Failed to load image at " << std::quoted(path) << ".\n";
				return false;
			}
			
            BITMAP bm;
            GetObject(bitmap, sizeof(BITMAP), &bm);
            width = bm.bmWidth;
            height = bm.bmHeight;		
			
			return true;
		}
		
	    bool isValid() const
	    {
	        return bitmap != NULL;
	    }		
	    
	    
	    
	    COLORREF transparencyColor = CLR_INVALID; // Add this member variable
	
	    void setTransparentMask(COLORREF color) {
	        transparencyColor = color;
	    }	    
	    
	    
			
	};
	
	
		
	
	
	class Drawable
	{
		public:
		
		int x = 0,y = 0,z = 0;
		int width = 1,height = 1;
		
		Vector2f scale = {1,1};		
		
		void setScale(Vector2f s)	
		{
			scale.x = s.x;
			scale.y = s.y;
		}
		
		
		void setOrigin(POINT pos)
		{
			origin.x = pos.x;
			origin.y = pos.y;
		}
	
	
	    int getScaledWidth() const { 
	        return static_cast<int>(width * scale.x); 
	    }
	    
	    int getScaledHeight() const { 
	        return static_cast<int>(height * scale.y); 
	    }
	
		POINT getScaledOrigin()
		{
			long int xo = static_cast<int>(origin.x * scale.x);
			long int yo = static_cast<int>(origin.y * scale.y);
			
			return {xo,yo};
		}
	
		
		virtual void draw(HDC hdc,View &view) = 0;
		virtual bool contains(POINT pos) = 0;
		
		
		virtual ~Drawable() = default;
		
		
		
		
		
		private:
				
			POINT origin = {0,0};	
		
		
	};
	
	
	
	
	class Sprite : public Drawable
	{
		public:
		
		
		
		Sprite()
		{
			
		}
		
		
		
		
		virtual bool contains(POINT pos) override
		{
			
	    	POINT o = getScaledOrigin();
			return (pos.x >= x - o.x && pos.y >= y - o.y && pos.x < x + getScaledWidth() - o.x && pos.y < y + getScaledHeight() - o.y); 
		}
		
		
		virtual void draw(HDC hdc,View &view)  override
		{
			
		    if (!textureRef || !textureRef->isValid()) return;
		    
		    POINT o = getScaledOrigin();
		    
		    // Apply view transformation to position
		    POINT viewPos = view.getPos();
		    int drawX = (x - o.x) - viewPos.x;
		    int drawY = (y - o.y) - viewPos.y;
		    
		    // Only draw if visible in current view
		    if (drawX + getScaledWidth() < 0 || drawY + getScaledHeight() < 0 ||
		        drawX >= view.getPortSize().x || drawY >= view.getPortSize().y) {
		        return; // Culling: skip if not visible
		    }
		    
		    HDC hMemDC = CreateCompatibleDC(hdc);
		    SetLayout(hdc, LAYOUT_BITMAPORIENTATIONPRESERVED);
		    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, textureRef->bitmap);
		    
		    TransparentBlt(hdc,
		        drawX, 
		        drawY, 
		        getScaledWidth(), 
		        getScaledHeight(),
		        hMemDC,
		        rect.left, 
		        rect.top, 
		        rect.right,
		        rect.bottom,
		        textureRef->transparencyColor);
		        
		    SelectObject(hMemDC, hOldBitmap);
		    DeleteDC(hMemDC);
			

		}
		
		
		
		void setTexture(Texture &texture)
		{
			textureRef = &texture;
			rect.left = 0;
			rect.top = 0;
			rect.right = textureRef->width;
			rect.bottom = textureRef->height;	
			
			width = textureRef->width;
			height = textureRef->height;
			
		}
		
		
		void setTextureRect(RECT r)
		{
			rect = r;
			width = r.right;
			height = r.bottom;
		}
		
		
		RECT getTextureRect()
		{
			return rect;
		}
		
		Texture &getTexture()
		{
			return *textureRef;
		}
		
		
	    const Texture* getTexture() const
	    {
	        return textureRef;
	    }		
		
		
	    bool hasTexture() const
	    {
	        return textureRef != nullptr;
	    }		
		
		

		
		
		public:
			friend class Window;
		
		
		private:
			Texture *textureRef = nullptr;
			RECT rect;
		
					
	};
	
	
	
	
	
	class Shape : public Drawable
	{
		public:
		
		COLORREF color = RGB(125,255,255);
		
		Shape()
		{
			width = 10;
			height = 10;
			
		}
		


		virtual bool contains(POINT pos)  override
		{
			
	    	POINT o = getScaledOrigin();
			return (pos.x >= x - o.x && pos.y >= y - o.y && pos.x < x + getScaledWidth() - o.x && pos.y < y + getScaledHeight() - o.y); 
		}



	    virtual void draw(HDC hdc,View &view)  override
	    {
		    POINT o = getScaledOrigin();
		    POINT viewPos = view.getPos();
		    
		    int drawX = (x - o.x) - viewPos.x;
		    int drawY = (y - o.y) - viewPos.y;
		    
		    // Culling check
		    if (drawX + getScaledWidth() < 0 || drawY + getScaledHeight() < 0 ||
		        drawX >= view.getPortSize().x || drawY >= view.getPortSize().y) {
		        return;
		    }
		    
		    HBRUSH brush = CreateSolidBrush(color);
		    RECT rect = {
		        drawX, 
		        drawY, 
		        drawX + getScaledWidth(), 
		        drawY + getScaledHeight()
		    };
		    FillRect(hdc, &rect, brush);
		    DeleteObject(brush);
	    }



		friend class Window;
		
		
	};
	


	
	
	
	class Window
	{
		public:
		
		HWND hwnd;
	    int x,y,width, height;		
		bool isTransparent = false;

		private:
			
		bool isRunning = true;
		std::queue<MSG> msgQ;
		
	    HDC backBufferDC;
	    HBITMAP backBufferBitmap;

				
		public:		
		
		
		View view;
		
		
        HBITMAP stretchBufferBitmap;
        HDC stretchBufferDC;		
		
		public:
		
		Window(int width,int height,std::string title,DWORD style = WS_OVERLAPPEDWINDOW)
		{
			
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
		    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		
		    if (!RegisterClass(&wc)) {
		        std::cerr << "Failed to register window class!" << std::endl;
		        exit(-1);
		    }		
			
			
			
			hwnd = CreateWindowEx(
			0,
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
			
			
			
			
			
			
			
				
	        HDC hdc = GetDC(hwnd);
	        backBufferDC = CreateCompatibleDC(hdc);
	        backBufferBitmap = CreateCompatibleBitmap(hdc, view.getSize().x, view.getSize().y);
	        SelectObject(backBufferDC, backBufferBitmap);
	        
			stretchBufferDC = CreateCompatibleDC(hdc);
            stretchBufferBitmap = CreateCompatibleBitmap(hdc, width, height);
            SelectObject(stretchBufferDC, stretchBufferBitmap);
			
			ReleaseDC(hwnd, hdc);			
			
			
			clear();
			
			
			
			
			
			
            windowInstances[hwnd] = this;			


            ShowWindow(hwnd, SW_SHOW);
            UpdateWindow(hwnd);


			
		}
		
		
		
        ~Window()
        {
            windowInstances.erase(hwnd);
            DeleteObject(backBufferBitmap);
        	DeleteDC(backBufferDC);
            DeleteObject(stretchBufferBitmap);
        	DeleteDC(stretchBufferDC);
        	DeleteObject(LegacyAlpha.hbmp);
		    DeleteDC(LegacyAlpha.hdcMem);
        }
		
		
		
		
		
		void setView(View &v)
		{
			view = v;
		}
		
		
    	
    	private:
		BYTE alpha = 255;
	    COLORREF transparencyColor = RGB(-1,0,0); 
		public:	
			
	    void setChromaKey(COLORREF color = RGB(-1,-1,-1),BYTE alphaValue = 255,std::string type = "modern") 
		{
			
	        transparencyColor = color;
	        alpha = alphaValue;
	        if(type == "legacy")
	        	legacyTransparency = true;
	        else
	        	legacyTransparency = false;
	        
	        
	    	COLORREF c = transparencyColor;
	    	
	    	if(!(GetRValue(c) < 0 || GetRValue(c) > 255 | GetGValue(c) < 0 || GetGValue(c) > 255 | GetBValue(c) < 0 || GetBValue(c) > 255)    ||    alpha < 255)
	    	{
	    		//is valid color or semi-transparent
				
				LONG_PTR style = GetWindowLongPtr(hwnd,GWL_STYLE);
	    		style |= WS_EX_LAYERED;
	    		SetWindowLongPtrA(hwnd,GWL_EXSTYLE,style);
	    		
	    		bool legacy = legacyTransparency;
	    		
	    		if(!legacy)
	    		{
					SetLayeredWindowAttributes(hwnd,c,alpha,LWA_COLORKEY | LWA_ALPHA);
	    		}
	    		else
	    		{
				
	    			//setup for ARGB 32bit transparency - legacy
		    		HDC hdc = GetDC(hwnd);
					SetLayout(hdc, LAYOUT_BITMAPORIENTATIONPRESERVED); // Set layout as left to right
					LegacyAlpha.hdcMem = CreateCompatibleDC(hdc);
			        
					BITMAPINFO bmi = {0};
			        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			        bmi.bmiHeader.biWidth = width;
			        bmi.bmiHeader.biHeight = -height;
			        bmi.bmiHeader.biPlanes = 1;
			        bmi.bmiHeader.biBitCount = 32;
			        bmi.bmiHeader.biCompression = BI_RGB;
			        
			        LegacyAlpha.hbmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &LegacyAlpha.pvBits, NULL, 0);
			        SelectObject(LegacyAlpha.hdcMem, LegacyAlpha.hbmp);

				}


	    		isTransparent = true;

	    		
			}
			else
				isTransparent = false;
	        
	    }	
		
		
		
		
		
		
		
		
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
	        MSG msg;
	        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
	            if (msg.message == WM_QUIT) {
	                isRunning = false;
	                break;
	            }
	            
	            TranslateMessage(&msg);
	            DispatchMessage(&msg);
	            
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
		
		
		
		
		
		
		
		
		
		
		
		
	    void clear(COLORREF color = RGB(255, 255, 255)) {


			//Update back buffer to contain the visible world area. 
			
//	        HDC hdc = GetDC(hwnd);
//			
//			DeleteObject(backBufferBitmap);
//	        backBufferBitmap = CreateCompatibleBitmap(hdc, view.getSize().x, view.getSize().y);
//	        SelectObject(backBufferDC, backBufferBitmap);
//
//	        DeleteObject(hdc);

	    	
	        HBRUSH brush = CreateSolidBrush(color);
	        RECT rect = {0, 0, view.getSize().x, view.getSize().y};
	        FillRect(backBufferDC, &rect, brush);
	        DeleteObject(brush);
	        

	    }
		
		
		
		
		
		
		
		
		
		
		
		
		
		void draw(Drawable &draw)
		{
		    draw.draw(backBufferDC,view);
		}
		
		
		
		
		
		
		
	    void display() 
		{
			
			
			
			
		    HDC hdc = GetDC(hwnd);
		    SetLayout(hdc, LAYOUT_BITMAPORIENTATIONPRESERVED);
		    
		    // Just copy the entire back buffer to stretch buffer
		    StretchBlt(stretchBufferDC, 
		    0, 
			0, 
			width, 
			height,
		    backBufferDC, 
		    0, 
			0, 
			view.getSize().x, 
			view.getSize().y,  // Always copy from (0,0) of world
		    SRCCOPY);
			


			COLORREF c = transparencyColor;
			
		    if(isTransparent && legacyTransparency)
	    	{
		        
		        
		        if (!LegacyAlpha.hdcMem || !LegacyAlpha.hbmp) {
		            setChromaKey(transparencyColor, alpha,"legacy"); // Recreate buffers just in case someone tries to use the isTransparent variable directly instead of using the setTransparency function.
		        }		        
		        
		        
		        // Copy from stretch buffer to ARGB buffer
		        SetLayout(LegacyAlpha.hdcMem, LAYOUT_BITMAPORIENTATIONPRESERVED); // Set layout as left to right
		        BitBlt(LegacyAlpha.hdcMem, 0, 0, width, height, stretchBufferDC, 0, 0, SRCCOPY);
	

		        
		        BLENDFUNCTION blend = {AC_SRC_OVER, 0, alpha, 0};
		        POINT ptDst = {0, 0};
		        SIZE size = {width, height};
		        POINT ptSrc = {0, 0};
		        
		        UpdateLayeredWindow(hwnd, hdc, &ptDst, &size, LegacyAlpha.hdcMem, &ptSrc, transparencyColor, &blend, ULW_ALPHA | ULW_COLORKEY);
		        
		        
		    }
		    else 
		    {
		        BitBlt(hdc, 0, 0, width, height, stretchBufferDC, 0, 0, SRCCOPY);	        
				InvalidateRect(hwnd, NULL, FALSE);
		        UpdateWindow(hwnd);
		    }

			ReleaseDC(hwnd, hdc);

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
	                
	                // Draw the back buffer to the screen
	                BitBlt(hdc, 0, 0, width, height, stretchBufferDC, 0, 0, SRCCOPY);
	                
	                EndPaint(hwnd, &ps);
	                return 0;
	            }
	                
	            case WM_SIZE: {
				
				   
				    
					width = LOWORD(lParam);
	                height = HIWORD(lParam);
	                
	                
	                
	                
	                //viewport size should be updated here to keep proportion even after window has changed size.
	                //for now we will wait to do that. For now we will just set the viewport as the window size.
					
					view.setPortSize({width,height});
					
					
					
	                
			        HDC hdc = GetDC(hwnd);
			        			        
			        
			        DeleteObject(stretchBufferBitmap);
			        stretchBufferBitmap = CreateCompatibleBitmap(hdc, width, height); 	//this will also likely need to change to accomodate the viewport size.
			        SelectObject(stretchBufferDC, stretchBufferBitmap);
			        
			        
				    // RECREATE TRANSPARENCY BUFFERS WHEN SIZE CHANGES
				    if (isTransparent) {
				        if (LegacyAlpha.hbmp) DeleteObject(LegacyAlpha.hbmp);
				        if (LegacyAlpha.hdcMem) DeleteDC(LegacyAlpha.hdcMem);
				        
				        LegacyAlpha.hdcMem = CreateCompatibleDC(hdc);
				        BITMAPINFO bmi = {0};
				        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				        bmi.bmiHeader.biWidth = width;
				        bmi.bmiHeader.biHeight = -height;
				        bmi.bmiHeader.biPlanes = 1;
				        bmi.bmiHeader.biBitCount = 32;
				        bmi.bmiHeader.biCompression = BI_RGB;
				        
				        LegacyAlpha.hbmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &LegacyAlpha.pvBits, NULL, 0);
				        SelectObject(LegacyAlpha.hdcMem, LegacyAlpha.hbmp);
				    }			        
			        
			        
			        
			        ReleaseDC(hwnd, hdc);
						                
	                clear();
	                return 0;
	            	break;
				}
				
				
				case WM_MOVE:
				{
					x = GET_X_LPARAM(lParam);
					y = GET_Y_LPARAM(lParam);
				}
				
	        }
            
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        
        
        
        
				
		
		
		struct LEGACY_ALPHA
		{
		
			void* pvBits = NULL;
			HBITMAP hbmp = NULL;
			HDC hdcMem = NULL;
		}LegacyAlpha;
		
		bool isFullscreen = false;
		bool legacyTransparency = false;
		RECT windowedRect; // Stores window position/size when not fullscreen
    	DWORD windowedStyle; // Stores window style when not fullscreen
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
			
	};
	
	// Initialize the static map
    std::map<HWND, Window*> Window::windowInstances;
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
	namespace Global
	{
	
		POINT getMousePos(Window &window)
		{
			
		    POINT p;
		    if(!GetCursorPos(&p))
		    {
		        return {0,0};
		    }
		    
		    ScreenToClient(window.hwnd, &p); // Convert to client coordinates
		    p = window.view.toWorld(p);
		    return p;
		}
		
		POINT getMousePos()
		{
				
			POINT p;
			if(!GetCursorPos(&p))
			{
				return {0,0};
			}
			
			return p;
		
		}
		
		bool getMouseButton(int vmButton)
		{
			if (GetAsyncKeyState(vmButton) & 0x8000)
				return true;
			
			return false;	
		}
		
		bool getKey(char vmKey)
		{
			if (GetAsyncKeyState(vmKey))
			{
				return true;
			}
			return false;
		}
	}
	
    
    
    
    
	
}


#endif