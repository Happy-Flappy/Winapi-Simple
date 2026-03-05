* Changed Drawable Transform so that position is always translated to origin point.
* Optimized ws::Window::clear() 9x faster.
* Modified ws::Window in responce to modifying ws::Texture to use a DIB section that GDI+ refers to. As opposed to copying every new frame from GDI+ to the backBuffer in ws::Window, which was of Type Gdiplus::bitmap only.
* ws::Texture still uses GDI+ bitmap but now the bitmap points to a GDI regular memory location. This is faster and allows users to BitBlt the contents of ws::Textures.
* Added getter functions for the GDI regular members in ws::Texture.

ws::Window::Clear = 9x faster
ws::Window::Draw = 5x faster
ws::Texture::setPixel() = 30000x faster! (2 million pixels per second) :O - Apparently GDI+ setpixel is pathetically slow.

 
