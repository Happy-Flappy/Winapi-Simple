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


## V1.6

* Changed Drawable Transform so that position is always translated to origin point.
* Changed ws::Texture to use ws::Hue in the set and get pixel functions instead of Gdiplus::Color. 
* ws::Texture still uses GDI+ bitmap but now the bitmap points to a GDI regular memory location. This is faster and allows users to BitBlt the contents of ws::Textures. Outcome - 30000x performance increase
* Added getter functions for the handles to the GDI regular members in ws::Texture.
* Modified ws::Window in responce to modifying ws::Texture to use a DIB section that GDI+ refers to. As opposed to copying every new frame from GDI+ to the backBuffer in ws::Window, which was of Type Gdiplus::bitmap only. Outcome - 5x speed increase
* Optimized ws::Window::clear() 9x faster.

### Total optimizations

* ws::Window::Clear = 9x faster
* ws::Window::Draw = 5x faster
* ws::Texture::setPixel() = 30000x faster! (2 million pixels per second) :O - Apparently GDI+ setpixel is pathetically slow.


* Split winsimple.h into separate files:
* winsimple.hpp - Core
* winsimple-clipboard.hpp - clipboard and dragdrop
* winsimple-controls.hpp - winapi controls such as buttons and save dialog
* winsimple-animate.hpp - animation helper class and ws::GIF.
* winsimple-audio.hpp

* Added new control child - ws::ListBox
* Added legacy transparency option in ws::Window::setChromaKey(ws::Hue hue,bool legacy = false) - Legacy uses per pixel alpha and the winapi UpdateLayeredWindow() function.
* Added Contains function to IntRect,FloatRect,and DoubleRect.
* Changed ws::Drawable coordinates from int to float. 
* Added move() to ws::Drawable.
* Changed ws::Shift() to stay on last frame till restarted instead of always auto looping.
* Added ws::ExploreWindow, which has many more options than FileWindow and supports both folder opening and file opening. The original classes are retained for compatibility.
* Added ws::ColorDialog for choosing a color.
* Added ws::Screen for screen related stuff.
* Added getPixel() to ws::Screen
* Added setPixel() and getSnapshot() to ws::Screen
* Added setSize() to ws::Screen - Affects resolution 
* Added 4 new Demos to  Beta Version 1.6
* Added automatic linking for visual studio. If you use visual studio with winsimple, you wont have to worry about linking.
* Fixed warnings of incorrect type conversions in winsimple headers.
* Fixed Faulty window cleanup in ws::Window::create()
* Major upgrades to ws::Child so that it can be used to easily do any win32 child control existing. 
* Fixed issue where winsimple-controls.hpp was required due to ws::Window::handleNotifyForChildren.
* Added Steering Behavior Demo
* Added ws::Texture::setSize() and ws::Texture::setScale for image pixel data resizing.
* Fixed Custom window class crashing.
* Added option to insert custom handling for internal window messages. 
* window.addMessageHandler([&window](MSG msg) -> LRESULT{//handle a message here and return LRESULT});
* Added ws::Hue::toHSV()
* Added ws::Hue::fromHSV()
* Fixed ws::Window::getPosition() to actually return the screen position.
* Added ws::Window::getBorderWidth()
* Added ws::Window::getCaptionRect(bool excludeBorder = false)
* Added SetProcessDPIAware() to the GDI+ initializer. With DPI awareness, screen.getSize() will be accurate.
* Fixed incorrect window size creation logic.
* Fixed ws::Text offset


## V1.7

* Removed void clearMessageHandlers()
* Added std::vector<std::function<LRESULT(MSG msg)>> getMessageHandlers() - allows erasure of a custom handler.
* Added void disableSomeClicks(ws::Hue hue) - Choose what color should represent emptiness and any click will pass through that color as if nothing is there.
* Added void disableAnyClicks() - force all clicks to pass through this window as if it does not exist.
* Added ws::Key button types that are equivalent in value and type to Winapi virtual key codes.
* Same with ws::Mouse
* Added GetAllButtons() to ws::Key and ws::Mouse - returns a vector of buttons that can be iterated.
* Added GetButtonName(int buttonCode) to ws::Key and ws::Mouse - returns string name of button.
* Fixed bug in ws::Window::clear() where the backbuffer is created with the world size when it should be viewport size.
* Changed ws::Window::getView() to return a pointer.
* Fixed ws::View::toWorld() and toScreen() by removing the secondary screensize parameter.
* Removed ws::Slider::getScroll and added under-the-hood messageHandler to ws::Slider constructor.(rare case where message not received by window poller.)
* Replaced ws::Menu::setWindow(ws::Window &window) with ws::Menu::setVisible(bool visible,ws::Window &window).
* Fixed issue where ws::Menu adjusted the window view due to the offset. Now using menu.setVisible() corrects that.
* Added math operators for vec2i,vec2f,vec2d,vec3i,vec3f,vec3d. Eg: ws::Vec2f AtimesB = vectorA * vector;
* Added bool ws::Wav::loadFromMemory(const void* data, size_t size, int m_channel, bool m_blocking,std::string extension)  
* Finally fixed an offset that was caused by ws::Menu
* Added ws::Window::setSourcePos(ws::Vec2i &pos) as part of the fix for the ws::Menu offset.
* Modified ws::Global::getMousePos(ws::Window &window) to consider ws::Menu offset.
* Modified ExploreWindow() to be ExploreWindow(std::string mode = "open") - can also use mode "save".
* Changed SetProcessDPIAware function to be dynamically loaded - Safety Procedure since windows xp does not have that function.
* Modified default behavior for client area icon double click to be ignored - otherwise default window behavior closes immediately.
* Added bool ws::Balloon(ws::Window &window,std::string message,std::string title,HICON hIcon,DWORD messageIconType = NIIF_USER,int timeoutMilliseconds = 5000,DWORD styles = NIF_ICON | NIF_TIP | NIF_INFO)
* Added bool ws::Balloon(ws::Window &window,std::string message,std::string title,std::string iconPath = "GETFROMWINDOW",DWORD messageIconType = NIIF_USER,int timeoutMilliseconds = 5000)
* Added std::string ws::getWindowsVersion(); - return strings are: PRE_XP - XP - vista - 7 - 8 - 8.1 - 10 - 11
* Added bool ws::Window::setIcon(HICON icon,DWORD size = ICON_SMALL)
* Added bool ws::Window::setIcon(std::string file,DWORD size = ICON_SMALL)
* Added ws::Icon - fully backwards compatible and swappable with HICON.
* Icon() = default;
* Icon(std::string path)
* Icon(const void* buffer,size_t bufferSize)
* Icon(HICON icon) : hIcon(nullptr) 
* ~Icon()
* operator HICON() const {return hIcon;}
* Icon& operator=(HICON icon) 
* Icon(const Icon& other) : hIcon(nullptr) {
* Icon& operator=(const Icon& other) {
* Icon(Icon&& other) noexcept : hIcon(other.hIcon) {
* Icon& operator=(Icon&& other) noexcept {
* HICON getHandle()
* bool loadFromFile(std::string path)
* bool loadFromMemory(const void* buffer,size_t bufferSize)
* bool isValid()
* ws::Texture getTexture(int width,int height)
* ws::Texture getTexture(DWORD size = ICON_SMALL)
* Added ws::Sprite::draw(ws::Texture &dest)
* Added ws::Sprite::drawBlend(ws::Texture &dest,DWORD stretchMode = 0)
* Added ws::Sprite::Blt(ws::Texture &dest,DWORD stretchMode = COLORONCOLOR)
* Fixed many ws::Wav bottlenecks by storing data members instead of calling MCI for everything.
* Added ws::Drawable::drawToTexture(ws::Texture &target)
* Added ws::Text::getLocalBounds() and had it called per every size changing ws::Text function call.
* Added borderWidth check to ws::Text so that it will not draw any border if the border has zero width.
* Added middle parameter to ws::Sprite::drawBlend(ws::Texture &dest,int alphaEffect = 255,DWORD stretchMode = 0)
* Removed Depreciated getMouseButton and getKey. Use getButton for both mouse and keyboard.
* Fixed ws::View::toWorld again since I accidentally broke it in this beta version.(nothing major - just coordinate offset)
* Fixed ws::Font::loadFromSystem(). It now updates to system fonts properly.
* Dynamically loaded RevokeDragDrop() function in ws::Window so that compiling no longer requires -lole32.
* Removed ws::Balloon() overload that loaded the icon from file. Using ws::Icon is the safer memory managed alternative.
* Modified ws::Balloon to remove it's own tray icon after the delay.
* Added ws::TrayIcon 
* Added ws::TrayIcon::showBalloon(const std::string& message, const std::string& title,DWORD infoFlags = NIIF_USER, int timeoutMs = 5000)
* Added bool ws::Drawable::intersects(ws::IntRect box)
* Added bool loop to ws::ShiftData
* Added ws::Font::loadFromMemory(const void* data, size_t size)
* Changed ws::View to use ws::FloatRect instead of ws::IntRect to avoid jitter.
* Changed ws types to have base templates. ws::Vec2<>,ws::Vec3<>,and ws::Rect<>.
* Added bool intersects to ws::Rect
* Added ws::Texture::editAllPixels(Func&& func,int start = 0,int end = -1) - Provides faster iteration.
* Fixed ws::View::toWorld again. Like seriously, how many times have I had to fix this again???
* Added Kinematics Demo
* Added SWP_NOACTIVATE to the setLayerAfter() method in ws::Window to avoid automatic activation.
* Added setBehindIcons() to set window behind desktop icons. (only works on windows 11 for now)




# New major features to be added. 


* More Winapi Controls

* Video class

* Texture stretching into polygonal shapes.

* Networking

* ProgressBar

* RadioButton & CheckBox – basic form elements.

* GroupBox – organizes forms visually.

* RichEdit

* TabControl – for multi-page setups without creating new windows.

* ListView (with report view) – for file lists, data grids.

* FontDialog

* NotifyIcon

* Searchbox

