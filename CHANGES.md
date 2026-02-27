# V1.0



* Created ws::Texture class.



# V1.1



* Split shape and sprite functionality into ws::Sprite and ws::Shape.

* Both classes inherit from ws::Drawable and can both be drawn using window.draw(ws::Drawable &draw).







* Fixed Sprite scaling

* Fixed Sprite Origin

* Fixed Sprite setTextureRect()

* Added View class and changed related API.



# V1.2



* Added Global Input System

* ws::Global::getMousePos(ws::Window &window)

* ws::Global::getMousePos()

* ws::Global::getKey(char vmKey)

* ws::Global::getMouseButton(char vmButton)







* Added Window Transparency

* Added ws::View::toWorld()

* Added ws::View::toWindow()

* Added setLayerAfter() for Z positioning of window.

* Added ws::Line class and bool ws::Line::intersects(ws::Line &otherLine).

* Added ws::Timer class.







# V1.3



* Added ws::Poly class that makes a polygon.

* Fixed bug where a transparent window flips in one of the graphical copy operations.

* Added Sound Class

* Added "Radial" polygonal class.

* Added PixelArray class

* Added setPixel and getPixel functions to ws::Texture











# V1.4



* Added Button class.

* Removed redundant display code in window.update() function.

* Added Slider class

* Added Textbox class

* Added Label class

* Added conversion constructors to and from ws::Vec2i and POINT.

* Added RGBA class and modified window class to use it for transparency.

* Modified window class to use ws::Texture instead of lower level winapi.

* Modified ws::Texture to use DIB section instead of basic GDI. Better for transparency and pixel access.

* \*GRAPHICS IS BROKEN\*

* ---Abandoned Project---

* Fallback to V1.3

* Re-added  Button,Slider,Textbox,Label classes.

* Added complete conversion constructors for
  ws::Vec2i,ws::Vec2f,ws::Vec2d,ws::Vec3i,ws::Vec3f,ws::Vec3d,ws::IntRect,ws::FloatRect,and ws:DoubleRect.

* Added flip transformations to drawable scale property. Negative scale on an axis means flipped image on that axis.

* Replaced Most of GDI graphics with GDI+  

* Added Clipboard class for each window.

* Clipboard can copy different types such as bitmap to and from windows.



# V1.5

* Made View class much more functional by using GDI+ Transformations.

* Added set and get functions for Rotate, Zoom, Origin, Center, and Size to View class.

* Added copy constructor and operator to View class. 

* Added many new functions to ws::Window and removed redundant settings variables that normally can be accessed through the windows HWND. These settings are get and set through functions that directly interact with winapi code.

* Added many ws::window constructors and copy constructors and create function.

* Added ws::Window::close(). 

* Added ws::Texture::loadFromMemory()

* Added ws::Animate class for the loading of GIF animations. ws::Animate stores frames as ws::Textures for convenience.

* Added get and set functions to Animate class (with exception in width and height variables for texture consistency)

* Added Documentation for previous versions.

* Added matrix transformation to ws::Drawable to allow any drawable to be transformed by the based class. Rotation, Scaling, Translation, etc.

* Added Demo program "Wheels.exe"

* Added ws::Round (creates ellipses)

* Fixed View::toWorld function by attaching it to window and adjusting for window stretching.

* Changed ws::Global::getMousePos(ws::Window &window) to only return window area mouse coordinates instead of converting to world coordinates.
* Added toWorld,toScreen functions to window class so that window can auto-input the screenSize parameter of the View::toWorld(ws::Vec2i point,ws::Vec2i screenSize) function.

* Renamed ws::Animate to ws::GIF since it only handled GIF animations for the most part.

* Added ws::ShiftData class for handling spritesheet animations.

* Added GDI+ ws::Font class and ws::Text class.

* Added saveToFile(std::string path) to ws::Texture.

* Added ws::FileWindow. For Save and Open file dialogs.

* Added ws::ClickMenu. For right click menu.(can be used with any trigger)

* Added ws::FolderWindow. For opening a folder dialog.

* Added copyFile() pasteFile() copyFiles() pasteFiles() and hasFile() to the clipboard class. This means that files can be copied to and even from the program.

* Added ws::Combobox class. It's a dropdown list that is not the kind that is connected to the window.

* Added ws::Menu and ws::Dropdown for window dropdown menu items.

* Added ws::Hue in place of Gdiplus::Color. Backwards compatible.

* Fixed ws::Window to allow creation of many windows and window polls.

* Fixed ws::View...again.

* Fixed Many Child object bugs.

* Added a few functions to ws::Window such as addChild(), removeChild(), and hasChild().

* Turned Winsimple into UNICODE so that it also works with visual studio.

* Fixed ws::Menu so that it displays immediately upon adding first dropdown.

* Fixed ws::View transformations so that when zooming in, the viewport does not change. 

* Added Cursor class

* Added ws::DropTarget class

* Added ws::DropEffect for ws::DropTarget enums.


# V1.6 - Beta

* Changed Drawable Transform so that position is always translated to origin point.

* Optimized ws::Window::clear() 9x faster.

* Modified ws::Window in responce to modifying ws::Texture to use a DIB section that GDI+ refers to. As opposed to copying every new frame from GDI+ to the backBuffer in ws::Window, which was of Type Gdiplus::bitmap only.

* ws::Texture still uses GDI+ bitmap but now the bitmap points to a GDI regular memory location. This is faster and allows users to BitBlt the contents of ws::Textures.

* Added getter functions for the GDI regular members in ws::Texture.

* ws::Window::Clear = 9x faster
* ws::Window::Draw = 5x faster
* ws::Texture::setPixel() = 30000x faster! (2 million pixels per second) :O - Apparently GDI+ setpixel is pathetically slow.

 



# New major features to be added. 



* Video class

* Texture stretching into polygonal shapes.

* Networking