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

	
	
	
	
	
	
	
	
	
	
	
	
	class Timer
	{
		public:
		
		
		// High-precision timer using QueryPerformanceCounter.
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
		
		// Resets start time and returns elapsed seconds since last restart.
		double restart()
		{
	        double seconds = getSeconds();
            LARGE_INTEGER counter;
            QueryPerformanceCounter(&counter);
            startTime = counter.QuadPart;
            
            return seconds;
		}
		
		
	    // Returns elapsed seconds since start.
	    double getSeconds() const
	    {
	        LARGE_INTEGER currentTime;
	        QueryPerformanceCounter(&currentTime);
	        return static_cast<double>(currentTime.QuadPart - startTime) / frequency;
	      
	    }
	    
	    // Returns elapsed milliseconds since start.
	    double getMilliSeconds() const
	    {
	        return getSeconds() * 1000.0;
	    }
	    
	    // Returns elapsed microseconds since start.
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
	    		world.left += x;
	    		world.top += y;
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
		    POINT viewPos = getPos();
		    
		    POINT worldPoint;
		    
		    float scaleX = static_cast<float>(worldSize.x) / viewSize.x;
		    float scaleY = static_cast<float>(worldSize.y) / viewSize.y;
		    
		    worldPoint.x = static_cast<int>(pos.x * scaleX) + viewPos.x;
		    worldPoint.y = static_cast<int>(pos.y * scaleY) + viewPos.y;
		    
		    return worldPoint;
	    }
	    
	    // Converts world coordinates to viewport (screen) coordinates.
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
	    
	    
	    
	    COLORREF transparencyColor = CLR_INVALID; // Add this member variable
	
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
	
		// Pure virtual draw method; view is used for coordinate transformation.
		virtual void draw(HDC hdc,View &view) = 0;
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
		
		
		// Draws the sprite texture with view culling and transparency.
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



	    // Draws the shape as a solid color rectangle with view culling.
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
	








	class Line : public ws::Drawable 
	{
	
		public:
		
		POINT start;
		POINT end;
		COLORREF color = RGB(0,0,255);
		int width = 2;
	    
	    
	    
	    // Constructs a line with given endpoints, width and color.
	    Line(POINT start = {0,0},POINT end = {0,0},int width = 2,COLORREF color = RGB(0,0,255))
	    {
	    	this->start = start;
	    	this->end = end;
	    	this->width = width;
	    	this->color = color;
		}
	    
	    
	    private:
		// Draws the line with view offset.
		virtual void draw(HDC hdc, ws::View &view) override {
	        // Create and select a blue pen
	        HPEN hPen = CreatePen(PS_SOLID, width, color);
	        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
	        
	        // Apply view transformation
	        POINT viewPos = view.getPos();
	        int drawX1 = start.x - viewPos.x;
	        int drawY1 = start.y - viewPos.y;
	        int drawX2 = end.x - viewPos.x;
	        int drawY2 = end.y - viewPos.y;
	        
	        // Draw a line
	        MoveToEx(hdc, drawX1, drawY1, NULL);
	        LineTo(hdc, drawX2, drawY2);
	        
	        // Restore the old pen and delete the created pen
	        SelectObject(hdc, hOldPen);
	        DeleteObject(hPen);
	        
	        
	
	    }
	    
	    
	    private:
	    // Hit test always returns false for a line.
	    virtual bool contains(POINT pos) override
	    { 
	    	return false;
		}
		
		
		public:
			
		// Checks if point q lies on line segment pr.
		bool onSegment(POINT p, POINT q, POINT r)
	    {
	        if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
	            q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
	            return true;
	        return false;
	    }
		
		// Returns orientation of triplet: 0 = collinear, 1 = clockwise, 2 = counterclockwise.
		int orientation(POINT p, POINT q, POINT r)
	    {
	        long long val = (long long)(q.y - p.y) * (r.x - q.x) - 
	                       (long long)(q.x - p.x) * (r.y - q.y);
	        
	        if (val == 0) return 0;  // Collinear
	        return (val > 0) ? 1 : 2; // Clockwise or counterclockwise
	    }			
		
	    // Returns true if this line segment intersects another.
	    bool intersects(Line &otherLine)
	    {
	        POINT p1 = this->start;
	        POINT p2 = this->end;
	        POINT p3 = otherLine.start;
	        POINT p4 = otherLine.end;
	        
	        // Calculate orientation values
	        int o1 = orientation(p1, p2, p3);
	        int o2 = orientation(p1, p2, p4);
	        int o3 = orientation(p3, p4, p1);
	        int o4 = orientation(p3, p4, p2);
	        
	        // General case: lines intersect if orientations are different
	        if (o1 != o2 && o3 != o4)
	            return true;
	        
	        // Special cases: check if points are collinear and lie on segments
	        if (o1 == 0 && onSegment(p1, p3, p2)) return true;
	        if (o2 == 0 && onSegment(p1, p4, p2)) return true;
	        if (o3 == 0 && onSegment(p3, p1, p4)) return true;
	        if (o4 == 0 && onSegment(p3, p2, p4)) return true;
	        
	        return false;
	    }
	    
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
		

				
		public:		
		
		
		View view;
		
		
        HBITMAP stretchBufferBitmap;
        HDC stretchBufferDC;		

	    HDC backBufferDC;
	    HBITMAP backBufferBitmap;

		// Creates window, registers class, initialises double buffering and transparency support.
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
		
		
		
        // Destroys window, frees buffers and transparency resources.
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
		
		
		
		
		
		// Replaces the current view with an external View.
		void setView(View &v)
		{
			view = v;
		}
		
		
    	
    	private:
		BYTE alpha = 255;
	    COLORREF transparencyColor = RGB(-1,0,0); 
		public:	
			
	    // Sets chroma key and alpha transparency; 'type' can be "modern" or "legacy".
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
		
		
		
		
		
		
		
		
		// Places this window immediately after the specified window in the Z-order.
		void setLayerAfter(HWND lastHwnd)
		{
			SetWindowPos(hwnd,lastHwnd,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
		}
		    	    
	    


	    // Toggles fullscreen mode on or off.
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
	    void clear(COLORREF color = RGB(0,0,0)) {


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
		
		
		
		
		
		
		
		
		
		
		
		
		
		// Draws any Drawable object onto the back buffer, applying view transformation.
		void draw(Drawable &draw)
		{
		    draw.draw(backBufferDC,view);
		}
		
		
		
		
		
		
		
	    // Presents the back buffer to the screen, handling transparency and stretching.
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
		
	
        // Instance window procedure; handles destroy, paint, resize, move.
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
	
		// Returns mouse position relative to the given window, converted to world coordinates.
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
		
		// Returns global mouse screen position.
		POINT getMousePos()
		{
				
			POINT p;
			if(!GetCursorPos(&p))
			{
				return {0,0};
			}
			
			return p;
		
		}
		
		// Checks if a virtual mouse button is pressed.
		bool getMouseButton(int vmButton)
		{
			if (GetAsyncKeyState(vmButton) & 0x8000)
				return true;
			
			return false;	
		}
		
		// Checks if a virtual key is pressed.
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