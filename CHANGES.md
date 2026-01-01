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



# V1.5 - BETA 

* Made View class much more functional by using GDI+ Transformations.

* Added set and get functions for Rotate, Zoom, Origin, Center, and Size to View class.

* Added copy constructor and operator to View class. 

* Added many new functions to ws::Window and removed redundant settings variables that normally can be accessed through the windows HWND. These settings are get and set through functions that directly interact with winapi code.

* Added many ws::window constructors and copy constructors and create function.

* Added ws::Window::close(). 

* Added ws::Texture::loadFromMemory()

* Added ws::Animate class for the loading of GIF animations. ws::Animate stores frames as ws::Textures for convenience.

* Added get and set functions to Animate class (with exception in width and height variables for texture consistency)






# New major features to be added. 



* Video class

* Texture stretching into polygonal shapes.

* Mesh3D

* Transformation functions at a drawables level (rotate,scale,color functions,etc.)

* More child classes and features for them.(right click menu)

* (Figure out how to fix random flicker that happens to child objects)

* Add more available types to Clipboard class.

