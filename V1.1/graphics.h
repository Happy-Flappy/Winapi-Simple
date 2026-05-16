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


namespace ws // - Winsimple
{

	
	
	
	
	
	
	
	struct Vector2f 
	{
		float x,y;
	};
	
	
	
	
	
	
	
	
	
	
	class View
	{
		public:
			
		// Default constructor.
		View()
		{
			
		}
		
		// Sets the world rectangle (visible area).
		void setRect(RECT viewRect)
		{
			world = viewRect;
		}
		
		// Sets world dimensions from a POINT.
		void setSize(POINT size)
		{
			world.right = size.x;
			world.bottom = size.y;
		}
		
		// Sets world position (upper-left corner).
		void setPos(POINT pos)
		{
			world.left = pos.x;
			world.top = pos.y;
		}
		
		
		// Sets the viewport rectangle (screen area).
		void setPortRect(RECT portRect)
		{
			port = portRect;
		}
		
		// Sets viewport size.
		void setPortSize(POINT size)
		{
			port.right = size.x;
			port.bottom = size.y;
		}
		
		// Sets viewport position.
		void setPortPos(POINT pos)
		{
			port.left = pos.x;
			port.top = pos.y;
		}
		
		
		
		
		// Returns the world rectangle.
		RECT getRect()
		{
			return world;
		}
		
		// Returns world size.
		POINT getSize()
		{
			POINT p;
			p.x = world.right;
			p.y = world.bottom;
			
			return p;
		}
		
		// Returns world position.
		POINT getPos()
		{
			POINT p;
			p.x = world.left;
			p.y = world.top;
			
			return p;
		}
		
		// Returns the viewport rectangle.
		RECT getPortRect()
		{
			return port;
		}
		
		// Returns viewport size.
		POINT getPortSize()
		{
			POINT p;
			p.x = port.right;
			p.y = port.bottom;
			
			return p;
		}
		
		// Returns viewport position.
		POINT getPortPos()
		{
			POINT p;
			p.x = port.left;
			p.y = port.top;
			
			return p;
		}
		
		
		
		
	    // Zooms the view by adjusting the viewport size inversely to the factor.
	    void zoom(float factor)
	    {
	    	if(factor != 0)
	    	{
			
		    	long int x = world.right / factor;
		    	long int y = world.bottom / factor;
				setPortSize({x,y});	// If factor is 2, that means that the visible world area is half as much because it is zooming in and will  be stretching into the viewport.
	    	}
		}

		// Moves the world by a delta.
		void move(POINT delta)
		{
			world.left += delta.x;
			world.top += delta.y;
		}
		
		// Moves the viewport by a delta.
		void movePort(POINT delta)
		{
			port.left += delta.x;
			port.top += delta.y;
		}
		
		
		
		
	    // Converts viewport (screen) coordinates to world coordinates.
	    POINT toWorld(POINT pos) 
	    {
	        
			
			POINT worldSize = getSize();
			POINT viewSize = getPortSize();
			
			POINT worldPoint;
	        
	        
	        float scaleX = static_cast<float>(worldSize.x) / viewSize.x;
	        float scaleY = static_cast<float>(worldSize.y) / viewSize.y;
	        
	        worldPoint.x = static_cast<int>(pos.x * scaleX);
	        worldPoint.y = static_cast<int>(pos.y * scaleY);
	        
	        return worldPoint;
	    }
	    
	    // Converts world coordinates to viewport (screen) coordinates.
	    POINT toWindow(POINT pos) 
	    {
	    	POINT worldSize = getSize();
			POINT viewSize = getPortSize();
			
	    	
	    	
	        POINT windowPoint;
	        
	        float scaleX = static_cast<float>(viewSize.x) / worldSize.x;
	        float scaleY = static_cast<float>(viewSize.y) / worldSize.y;
	        
	        windowPoint.x = static_cast<int>(pos.x * scaleX);
	        windowPoint.y = static_cast<int>(pos.y * scaleY);
	        
	        return windowPoint;
	    }
				
		
		
		
		
		
		
		
		
		private:
		RECT world;
		RECT port;
		
			
	};
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	// Converts a UTF-8 std::string to std::wstring.
	std::wstring WIDE(std::string str)
	{
	    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	    std::wstring wstr(size, 0);
	    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
	    return wstr;
	}
	
	
	
	// Returns c_str() cast to LPCSTR.
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
		
		// Constructs by loading image from path.
		Texture(std::string path)
		{
			load(path);
		}
		
		
	    // Frees the bitmap handle.
	    ~Texture()
	    {
	        if (bitmap != NULL)
	        {
	            DeleteObject(bitmap);
	            bitmap = NULL;
	        }
	    }		
		



	    // Move constructor.
	    Texture(Texture&& other) noexcept
	        : bitmap(other.bitmap), width(other.width), height(other.height)
	    {
	        other.bitmap = NULL;
	        other.width = 0;
	        other.height = 0;
	    }


		
	    // Move assignment operator.
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
		
		
	
	
	
	
		// Loads bitmap from file; releases previous image.
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
		
	    // Checks if a valid bitmap is loaded.
	    bool isValid() const
	    {
	        return bitmap != NULL;
	    }		
	    
	    
	    
	    COLORREF transparencyColor = CLR_INVALID; 
		
	    // Sets the color key used for transparent blitting.
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
		
		// Sets the scale factor.
		void setScale(Vector2f s)	
		{
			scale.x = s.x;
			scale.y = s.y;
		}
		
		
		// Sets the origin offset for position calculations.
		void setOrigin(POINT pos)
		{
			origin.x = pos.x;
			origin.y = pos.y;
		}
	
	
	    // Returns width multiplied by scale.
	    int getScaledWidth() const { 
	        return static_cast<int>(width * scale.x); 
	    }
	    
	    // Returns height multiplied by scale.
	    int getScaledHeight() const { 
	        return static_cast<int>(height * scale.y); 
	    }
	
		// Returns the origin multiplied by scale.
		POINT getScaledOrigin()
		{
			long int xo = static_cast<int>(origin.x * scale.x);
			long int yo = static_cast<int>(origin.y * scale.y);
			
			return {xo,yo};
		}
	
		// Pure virtual draw method.
		virtual void draw(HDC hdc) = 0;
		// Pure virtual hit test.
		virtual bool contains(POINT pos) = 0;
		
		
		virtual ~Drawable() = default;
		
		
		
		
		
		private:
				
			POINT origin = {0,0};	
		
		
	};
	
	
	
	
	class Sprite : public Drawable
	{
		public:
		
		
		// Default constructor.
		Sprite()
		{
			
		}
		
		
		
		
		// Checks if point lies inside the scaled sprite bounds.
		virtual bool contains(POINT pos) override
		{
			
	    	POINT o = getScaledOrigin();
			return (pos.x >= x - o.x && pos.y >= y - o.y && pos.x < x + getScaledWidth() - o.x && pos.y < y + getScaledHeight() - o.y); 
		}
		
		
		// Draws the sprite texture with transparency.
		virtual void draw(HDC hdc)  override
		{
			
	    	POINT o = getScaledOrigin();
	    	
	       	if (!textureRef || !textureRef->isValid()) return;
	        
	        HDC hMemDC = CreateCompatibleDC(hdc);
	        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, textureRef->bitmap);
	        
	        TransparentBlt(hdc,
	            x - o.x, 
	            y - o.y, 
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
		
		
		// Attaches a texture and sets rect to full texture size.
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
		
		
		// Sets the source rectangle used from the texture.
		void setTextureRect(RECT r)
		{
			rect = r;
			width = r.right;
			height = r.bottom;
		}
		
		
		// Returns the source rectangle.
		RECT getTextureRect()
		{
			return rect;
		}
		
		// Returns a reference to the attached texture.
		Texture &getTexture()
		{
			return *textureRef;
		}
		
		
	    // Returns const pointer to attached texture.
	    const Texture* getTexture() const
	    {
	        return textureRef;
	    }		
		
		
	    // Checks if a texture is currently assigned.
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
		
		// Initializes a 10x10 shape.
		Shape()
		{
			width = 10;
			height = 10;
			
		}
		


		// Checks if point lies inside the scaled shape bounds.
		virtual bool contains(POINT pos)  override
		{
			
	    	POINT o = getScaledOrigin();
			return (pos.x >= x - o.x && pos.y >= y - o.y && pos.x < x + getScaledWidth() - o.x && pos.y < y + getScaledHeight() - o.y); 
		}



	    // Draws the shape as a solid color rectangle.
	    virtual void draw(HDC hdc)  override
	    {
	    	POINT o = getScaledOrigin();
	        HBRUSH brush = CreateSolidBrush(color);
	        RECT rect = {
	            x - o.x, 
	            y - o.y, 
	            x - o.x + getScaledWidth(), 
	            y - o.y + getScaledHeight()
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
		
		// Creates window, registers class, initialises double buffering with view.
		Window(int width,int height,std::string title)
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
		
		
		
        // Destroys window, frees buffers and removes from instance map.
        ~Window()
        {
            windowInstances.erase(hwnd);
            DeleteObject(backBufferBitmap);
        	DeleteDC(backBufferDC);
            DeleteObject(stretchBufferBitmap);
        	DeleteDC(stretchBufferDC);
        }
		
		
		
		
		
		// Replaces the current view with an external View.
		void setView(View &v)
		{
			view = v;
		}
		
		
    

		
		
			
	    
	    

	    // Toggles fullscreen mode on or off.
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
	    
	    // Switches between fullscreen and windowed.
	    void toggleFullscreen() {
	        setFullscreen(!isFullscreen);
	    }
	    
	    // Returns true if window is fullscreen.
	    bool getFullscreen() const {
	        return isFullscreen;
	    }


				
		
		
		
		// Processes Windows messages; returns false if quit received.
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
		
		
		
		
		
	
		
		
		
	    // Pops the next queued message; returns false if queue empty.
	    bool pollEvent(MSG &message) {
	        if (msgQ.empty()) {
	            return false;
	        }
	        
	        message = msgQ.front();
	        msgQ.pop();
	        return true;
	    }	
		
		
		
		
		
		
		
		
		
		
		
		
	    // Fills the back buffer with a solid color.
	    void clear(COLORREF color = RGB(255, 255, 255)) {
	        HBRUSH brush = CreateSolidBrush(color);
	        RECT rect = {0, 0, view.getSize().x, view.getSize().y};
	        FillRect(backBufferDC, &rect, brush);
	        DeleteObject(brush);
	    }
		
		
		
		
		
		
		
		
		
		
		
		
		
		// Draws any Drawable object onto the back buffer.
		void draw(Drawable &draw)
		{
		    draw.draw(backBufferDC);
		}
		
		
		
		
		
		
		
		
		
	    // Stretches back buffer to window client area and presents.
	    void display() 
		{

			HDC hdc = GetDC(hwnd);
			StretchBlt(stretchBufferDC, 
			0, 
			0, 
			width, 
			height,
        	backBufferDC, 
			0, 
			0, 
			view.getSize().x, 
			view.getSize().y, 
			SRCCOPY);
			
			
			BitBlt(hdc, 0, 0, width, height, stretchBufferDC, 0, 0, SRCCOPY);
			
			ReleaseDC(hwnd, hdc);
			
			InvalidateRect(hwnd, NULL, FALSE);
        	UpdateWindow(hwnd);
	    }		
		
		
		
		
		
		
		
		
		
		private:
			
			
			
		
		

        // Static map to store window instances
        static std::map<HWND, Window*> windowInstances;		

		
        // Static window procedure; forwards messages to the correct instance.
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
		
	
        // Instance window procedure; handles destroy, paint and resize.
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
	                
	                
	                
	                
	                //viewport size should be updated here to keep proportion even after window has changed size.
	                //for now we will wait to do that. For now we will just set the viewport as the window size.
					
					view.setPortSize({width,height});
					
					
					
	                
			        HDC hdc = GetDC(hwnd);
			        
			        DeleteObject(backBufferBitmap);
			        backBufferBitmap = CreateCompatibleBitmap(hdc, view.getSize().x, view.getSize().y);
			        SelectObject(backBufferDC, backBufferBitmap);
			        
			        DeleteObject(stretchBufferBitmap);
			        stretchBufferBitmap = CreateCompatibleBitmap(hdc, width, height); 	//this will also likely need to change to accomodate the viewport size.
			        SelectObject(stretchBufferDC, stretchBufferBitmap);
			        
			        ReleaseDC(hwnd, hdc);
						                
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