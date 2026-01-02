#ifndef WINAPI_SIMPLE_GRAPHICS
#define WINAPI_SIMPLE_GRAPHICS


//Linking = -lgdi32 -luser32 -lmsimg32 -lkernel32 



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
	
	
	
	
	class Shape
	{
		public:
		
		int x,y,z;
		float scaleX = 1.0f,scaleY = 1.0f;
		int originX = 0,originY = 0;
		COLORREF color = RGB(255,255,255);
		RECT rect;
		
		
		Shape()
		{
			x = 0;
			y = 0;
			z = 0;
			
			rect.right = 10;
			rect.bottom = 10;
		}
		
		
		
		
		bool contains(int posx,int posy)
		{
			return (posx >= x - originX && posy >= y - originY && posx < x + rect.right - originX && posy < y + rect.bottom - originY); 
		}
		
		
		
		
		
		
		void setTexture(Texture &texture)
		{
			textureRef = &texture;
			rect.left = 0;
			rect.top = 0;
			rect.right = textureRef->width;
			rect.bottom = textureRef->height;			
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
		
		
		
		void fitTexture()
		{
			rect.left = 0;
			rect.top = 0;
			rect.right = textureRef->width;
			rect.bottom = textureRef->height;
		}		
		
		
		
		void setOrigin(int posx,int posy)
		{
			originX = posx;
			originY = posy;
		}
		
		
		
		
		private:
			Texture *textureRef = nullptr;
		
			
	};
	

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	class Window
	{
		public:
		
		HWND hwnd;
	    int x,y,width, height;		


		private:
			
		bool isRunning = true;
		std::queue<MSG> msgQ;
		
	    HDC backBufferDC;
	    HBITMAP backBufferBitmap;

		private:
		struct World
		{
			int x,y,width,height;
		};
				
		public:		
		struct View
		{
			int x,y,width,height;

			World world;
				
		}view;
		
        HBITMAP stretchBufferBitmap;
        HDC stretchBufferDC;		
		
		public:
		
		Window(int width,int height,std::string title)
		{
		
			view.width = width;
			view.height = height;
			view.x = 0;
			view.y = 0;
			
			view.world.x = 0;
			view.world.y = 0;
			view.world.width = view.width;
			view.world.height = view.height;
			
			
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
			WS_OVERLAPPEDWINDOW,
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
	        backBufferBitmap = CreateCompatibleBitmap(hdc, view.world.width, view.world.height);
	        SelectObject(backBufferDC, backBufferBitmap);
	        
			stretchBufferDC = CreateCompatibleDC(hdc);
            stretchBufferBitmap = CreateCompatibleBitmap(hdc, width, height);
            SelectObject(stretchBufferDC, stretchBufferBitmap);
			
			ReleaseDC(hwnd, hdc);			
			
			
			clear();
			
			
			
			
			
			
            // Store this instance in a map for later retrieval
            windowInstances[hwnd] = this;			


            ShowWindow(hwnd, SW_SHOW);
            UpdateWindow(hwnd);


			
		}
		
		
		
        ~Window()
        {
            windowInstances.erase(hwnd);
            DeleteObject(backBufferBitmap);
        	DeleteDC(backBufferDC);
        }
		




	    void setFullscreen(bool fullscreen) {
	        if (fullscreen == isFullscreen) return;
	        
	        if (fullscreen) {
	            windowedStyle = GetWindowLong(hwnd, GWL_STYLE);
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
	        HBRUSH brush = CreateSolidBrush(color);
	        RECT rect = {0, 0, view.world.width, view.world.height};
	        FillRect(backBufferDC, &rect, brush);
	        DeleteObject(brush);
	    }
		
		
		
		
		
		
		
		
		
		
		
		
		
		void draw(Shape &shape)
		{
			if(shape.getTexture().bitmap != NULL)
			{
               	HDC hMemDC = CreateCompatibleDC(backBufferDC);
                HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, shape.getTexture().bitmap);
                
			    BITMAP bm;
			    GetObject(shape.getTexture().bitmap, sizeof(BITMAP), &bm);
			
			    TransparentBlt(backBufferDC,
                shape.x - shape.originX, 
                shape.y - shape.originY, 
                shape.rect.right, 
                shape.rect.bottom,
                hMemDC,
                shape.rect.left, 
                shape.rect.top, 
                shape.rect.right, 
                shape.rect.bottom,
                shape.getTexture().transparencyColor);

				SelectObject(hMemDC, hOldBitmap);
                DeleteDC(hMemDC);
				
			}
			else
			{
			
				HBRUSH brush = CreateSolidBrush(shape.color);
		        RECT rect = {shape.x - shape.originX, shape.y - shape.originY, shape.x + shape.rect.right - shape.originX, shape.y + shape.rect.bottom - shape.originY};
		        FillRect(backBufferDC, &rect, brush);
		        DeleteObject(brush);
		    }
		}
		
		
		
		
		
		
		
		
		
	    void display() 
		{

			HDC hdc = GetDC(hwnd);
			StretchBlt(stretchBufferDC, 
			0, 
			0, 
			width, 
			height,
        	backBufferDC, 
			view.world.x, 
			view.world.y, 
			view.world.width, 
			view.world.height, 
			SRCCOPY);
			
			
			BitBlt(hdc, 0, 0, width, height, stretchBufferDC, 0, 0, SRCCOPY);
			
			ReleaseDC(hwnd, hdc);
			
			InvalidateRect(hwnd, NULL, FALSE);
        	UpdateWindow(hwnd);
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
	                
	            case WM_SIZE:
				   
				    width = LOWORD(lParam);
	                height = HIWORD(lParam);
	                
	                // Recreate back buffer
                    HDC hdc = GetDC(hwnd);
                    DeleteObject(stretchBufferBitmap);
                    stretchBufferBitmap = CreateCompatibleBitmap(hdc, width, height);
                    SelectObject(stretchBufferDC, stretchBufferBitmap);
                    ReleaseDC(hwnd, hdc);
	                
	                // Clear the new back buffer
	                clear();
	                return 0;
	        }
            
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        
        
        
        
				
		
		
		
		bool isFullscreen = false;
	    RECT windowedRect; // Stores window position/size when not fullscreen
    	DWORD windowedStyle; // Stores window style when not fullscreen
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
			
	};
	
	// Initialize the static map
    std::map<HWND, Window*> Window::windowInstances;
    





































	
	
}


#endif