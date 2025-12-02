



namespace ws
{
	
	class BGRA 
	{
	    public:
	    BYTE b, g, r, a; 
	    
	    // Constructors
	    BGRA() : b(0), g(0), r(0), a(255) {} // Default: opaque black
	    BGRA(BYTE blue, BYTE green, BYTE red, BYTE alpha = 255) 
	        : b(blue), g(green), r(red), a(alpha) {}
	    BGRA(COLORREF color, BYTE alpha = 255) 
	        : b(GetBValue(color)), g(GetGValue(color)), r(GetRValue(color)), a(alpha) {}
        
		

		

	    COLORREF TO_COLORREF() const {
	        return RGB(r, g, b);
	    }
	    
	    DWORD TO_DWORD() const {
	        // For DIB sections: [BB] [GG] [RR] [AA]
	        return (b << 0) | (g << 8) | (r << 16) | (a << 24);
	    }

	    
	    
	    
	    
	    
	    // Predefined colors
	    static BGRA Transparent() { return BGRA(0, 0, 0, 0); }
	    static BGRA Black() { return BGRA(0, 0, 0, 255); }
	    static BGRA White() { return BGRA(255, 255, 255, 255); }
	    static BGRA Red() { return BGRA(0, 0, 255, 255); }
	    static BGRA Green() { return BGRA(0, 255, 0, 255); }
	    static BGRA Blue() { return BGRA(255, 0, 0, 255); }
	    static BGRA Orange() { return BGRA(0, 150, 255, 255); }
	    
	    // Utility methods
	    bool isTransparent() const { return a == 0; }
	    bool isOpaque() const { return a == 255; }
	    float getOpacity() const { return static_cast<float>(a) / 255.0f; }
	    
	    void setOpacity(float opacity) {
	        a = static_cast<BYTE>(opacity * 255);
	    }
	    
	    // Blend with another color (simple alpha blending)
	    BGRA blend(const BGRA& other) const {
	        if (a == 0) return other;
	        if (other.a == 0) return *this;
	        if (a == 255) return *this;
	        if (other.a == 255) return other;
	        
	        float alpha1 = static_cast<float>(a) / 255.0f;
	        float alpha2 = static_cast<float>(other.a) / 255.0f;
	        float outAlpha = alpha1 + alpha2 * (1 - alpha1);
	        
	        if (outAlpha == 0) return BGRA::Transparent();
	        
	        BYTE outB = static_cast<BYTE>((b * alpha1 + other.b * alpha2 * (1 - alpha1)) / outAlpha);
	        BYTE outG = static_cast<BYTE>((g * alpha1 + other.g * alpha2 * (1 - alpha1)) / outAlpha);
	        BYTE outR = static_cast<BYTE>((r * alpha1 + other.r * alpha2 * (1 - alpha1)) / outAlpha);
	        BYTE outA = static_cast<BYTE>(outAlpha * 255);
	        
	        return BGRA(outB, outG, outR, outA);
	    }
	    
	    void debugPrint() const {
	    	
	    	DWORD test;
	    	test = TO_DWORD();
	    	
	        std::cout << "BGRA(" << (int)b << "," << (int)g << "," << (int)r << "," << (int)a << ") -> DWORD: 0x" 
	                  << std::hex << test << std::dec << " (";
	        
	        BYTE* bytes = (BYTE*)&test;
	        std::cout << "Bytes: ";
	        for(int i = 0; i < 4; i++) {
	            std::cout << (int)bytes[i] << " ";
	        }
	        std::cout << ")" << std::endl;
	    }        
	};


	BGRA TO_BGRA(DWORD word)
	{
		BYTE z,x,c,v;
		z = (word >> 0) & 0xFF;
		x = (word >> 8) & 0xFF;
		c = (word >> 16) & 0xFF;
		v = (word >> 24) & 0xFF;
		return BGRA(z,x,c,v);
	}









	class Texture
	{
		
		
		private:
			

		unsigned int stride = 0;
		

		int calculateStride(int width, int bitsPerPixel) 
		{
		    return ((width * (bitsPerPixel / 8) + 3) & ~3);
		    //A bunch of byte dependent stuff that converts the byte index to integer index.
		}		


		
		public:
		HBITMAP bitmap;
		int width=0,height=0;	
		void* pvBits = NULL;	
		BITMAPINFO bmi;	





		Texture(int wide=1,int high=1)
		{
			create(wide,high);
		}
		
		
		
		~Texture()
		{
			if(bitmap)
			{
	            DeleteObject(bitmap);
	            bitmap = nullptr;
	            pvBits = nullptr;	
	            width = 0;
	            height = 0;
			}
		}




	    bool isValid() const 
	    { 
	        return bitmap != nullptr && pvBits != nullptr; 
	    }


	
	
		void create(int nWidth,int nHeight)
		{
			
			

			
			ZeroMemory(&bmi, sizeof(bmi));
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = nWidth;
			bmi.bmiHeader.biHeight = -nHeight; // A negative height makes the image a top-down DIB
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;	
			bmi.bmiHeader.biSizeImage = nWidth * nHeight * 4; // Important for alpha		
			
	        HDC hdcMem = CreateCompatibleDC(NULL);
	        bitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
	        SelectObject(hdcMem, bitmap);
	        DeleteDC(hdcMem);
			
			if(!bitmap)
			{
				std::cerr << "Failed to Create DIB!\n";
				bitmap = nullptr;
				
				return;
			}
		
			width = nWidth;
			height = nHeight;
			stride = calculateStride(nWidth, 32); 
			
			
			clear(BGRA::Transparent());
			
		}
		




		bool load(std::string path)
		{
			bitmap = (HBITMAP)LoadImageW(
			NULL,
			WIDE(path).c_str(),
			IMAGE_BITMAP,
			0,
			0,
			LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_VGACOLOR
			);
			
			
			
			if(bitmap == NULL)
			{
				std::cerr << "Failed to load image at " << std::quoted(path) << ".\n";
				return false;
			}


			BITMAP bmp;
			int success = GetObject(bitmap, sizeof(BITMAP), &bmp);			
			
			if(success == 0)
				return false;
				
			width = bmp.bmWidth;
			height = bmp.bmHeight;	
			stride = calculateStride(width, bmp.bmBitsPixel); 
			
		    if(bmp.bmBitsPixel == 32) {
		        width = bmp.bmWidth;
		        height = bmp.bmHeight;
		        stride = calculateStride(width, 32);
		        pvBits = bmp.bmBits;
		        return true;
		    }
		    
		    // Convert to 32-bit
		    return convertTo32Bit(bmp);
		}

		
		

		
		
		bool convertTo32Bit(BITMAP& sourceBmp)
		{
		    width = sourceBmp.bmWidth;
		    height = sourceBmp.bmHeight;
		    
		    // Create a new 32-bit DIB section (like in create())
		    HDC hdcMem = CreateCompatibleDC(NULL);
		    
		    ZeroMemory(&bmi, sizeof(bmi));
		    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		    bmi.bmiHeader.biWidth = width;
		    bmi.bmiHeader.biHeight = -height; // Top-down
		    bmi.bmiHeader.biPlanes = 1;
		    bmi.bmiHeader.biBitCount = 32;
		    bmi.bmiHeader.biCompression = BI_RGB;
		    bmi.bmiHeader.biSizeImage = width * height * 4;
		    
		    HBITMAP newBitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
		    SelectObject(hdcMem, newBitmap);
		    
		    if(!newBitmap) {
		        std::cerr << "Failed to create 32-bit DIB section for conversion!\n";
		        DeleteDC(hdcMem);
		        return false;
		    }
		    
		    // Copy and convert the source bitmap
		    HDC hdcSource = CreateCompatibleDC(NULL);
		    SelectObject(hdcSource, bitmap);
		    
		    // Use BitBlt to copy and convert the format
		    BitBlt(hdcMem, 0, 0, width, height, hdcSource, 0, 0, SRCCOPY);
		    
		    // Set alpha channel to 255 (fully opaque) for all pixels
		    DWORD* pixels = static_cast<DWORD*>(pvBits);
		    for(int i = 0; i < width * height; i++) {
		        pixels[i] |= 0xFF000000; // Set alpha to 255
		    }
		    
		    // Clean up old bitmap and use new one
		    DeleteObject(bitmap);
		    bitmap = newBitmap;
		    
		    DeleteDC(hdcSource);
		    DeleteDC(hdcMem);
		    
		    stride = calculateStride(width, 32);
		    std::cout << "Converted to 32-bit ARGB: " << width << "x" << height << std::endl;
		    
		    return true;
		}
		
		
		
		
		
		
		
	    int getPixelIndex(int x, int y)	
	    {
	        // Convert from pixel coordinates to actual memory index
	        return y * (stride / sizeof(DWORD)) + x;
	    }
	    
	    ws::Vec2i getPixelIndex(int index)
	    {
	        // Convert from memory index to pixel coordinates
	        int pixelsPerRow = stride / sizeof(DWORD);
	        int y = index / pixelsPerRow;
	        int x = index % pixelsPerRow;
	        return ws::Vec2i(x, y);
	    }
				
		
		
        bool setPixel(ws::BGRA color,int index)
        {
            if (!pvBits || index < 0 || index >= width * height)
                return false;
                
            DWORD* pPixels = static_cast<DWORD*>(pvBits);
            pPixels[index] = color.TO_DWORD();
            return true;
        }
		
		
        bool setPixel(ws::BGRA color,int x,int y)
        {
            if (!pvBits || x < 0 || x >= width || y < 0 || y >= height)
                return false;
                
            DWORD* pPixels = static_cast<DWORD*>(pvBits);
            int index = getPixelIndex(x, y);
            pPixels[index] = color.TO_DWORD();
            return true;
        }
		
		
        ws::BGRA getPixel(int index)
        {
            if (!pvBits || index < 0 || index >= width * height)
                return BGRA::Transparent();
                
            DWORD* pPixels = static_cast<DWORD*>(pvBits);
            DWORD pixel = pPixels[index];
            
            return TO_BGRA(pixel);
        }
		
		
		
        ws::BGRA getPixel(int x,int y)
        {
            if (!pvBits || x < 0 || x >= width || y < 0 || y >= height)
                return BGRA::Transparent();
                
            DWORD* pPixels = static_cast<DWORD*>(pvBits);
            int index = getPixelIndex(x, y);
            DWORD pixel = pPixels[index];
            
		    return TO_BGRA(pixel);	
        }



        void clear(ws::BGRA color = BGRA::Transparent()) 
        {
            if (!pvBits) return;
            
            DWORD* pPixels = static_cast<DWORD*>(pvBits);
            
            for (int i = 0; i < width * height; i++) {
                pPixels[i] = color.TO_DWORD();
            }
        }
	    
	    
        void testPixelSetting() {
            std::cout << "Testing pixel setting...\n";
            
            // Test red pixel
            setPixel(BGRA(0, 0, 255, 255), 0, 0);
            BGRA result = getPixel(0, 0);
            std::cout << "Set BGRA(0,0,255,255), got: ";
            result.debugPrint();
            
            // Test green pixel  
            setPixel(BGRA(0, 255, 0, 255), 1, 0);
            result = getPixel(1, 0);
            std::cout << "Set BGRA(0,255,0,255), got: ";
            result.debugPrint();
            
            // Test blue pixel
            setPixel(BGRA(255, 0, 0, 255), 2, 0);
            result = getPixel(2, 0);
            std::cout << "Set BGRA(255,0,0,255), got: ";
            result.debugPrint();
            
            // Test semi-transparent pixel
            setPixel(BGRA(0, 0, 255, 128), 3, 0);
            result = getPixel(3, 0);
            std::cout << "Set BGRA(0,0,255,128), got: ";
            result.debugPrint();
        }
	    
	    
	    
		void fillRect(ws::BGRA color, int x, int y, int w, int h) {
	        for (int iy = y; iy < y + h && iy < height; iy++) {
	            for (int ix = x; ix < x + w && ix < width; ix++) {
	                setPixel(color, ix, iy);
	            }
	        }
	    }
		

	};		
	
	
	
	
	
	
	
	
	
	
	
	
	///SPRITE DRAW FUNCTION
	
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


			BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};


	        // AlphaBlend with stretching - same parameters as StretchBlt
	        AlphaBlend(hdc,
	            drawX,                                  // Destination X
	            drawY,                                  // Destination Y  
	            getScaledWidth(),                       // Destination Width (scaled)
	            getScaledHeight(),                      // Destination Height (scaled)
	            hMemDC,
	            rect.left,                              // Source X
	            rect.top,                               // Source Y
	            width,                             // Source Width 
	            height,                            // Source Height
	            blend);                                 // Alpha blending function
	        	    
				        
		    SelectObject(hMemDC, hOldBitmap);
		    DeleteDC(hMemDC);
			

		}
		
	///<><><><><><><><><><><><>><><><><><><><><><><<>
	
	
}