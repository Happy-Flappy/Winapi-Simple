# Winsimple V1.6 API Documentation


<iframe width="560" height="315" src="https://youtube.com/embed/mgtPEukFGro" frameborder="0" allowfullscreen></iframe>

## ws::Timer timer;
* double seconds = timer.getSeconds();
* double milliSeconds = timer.getMilliSeconds();
* double microSeconds = timer.getMicroSeconds();
* double restartSeconds = timer.restart();


## String Conversion Helpers
* std::wstring wideString = ws::WIDE(myShortString);
* std::string shortString = ws::SHORT(wideString);
* LPCSTR myLPCSTR = ws::TO_LPCSTR(shortString);
* LPCWSTR myLPCWSTR = ws::TO_LPCWSTR(shortString);


## Internal Helper Functions
* inline std::wstring GetShortPathNameSafe(const std::wstring& longPath)
* inline int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
* inline bool ResolveRelativePath(std::string& path)




## Data Types & Conversions

### Vector Types

All vector types (`Vec2i`, `Vec2f`, `Vec2d`, `Vec3i`, `Vec3f`, `Vec3d`) support the following generic conversions:

* **Construct from any arithmetic types** – e.g. `ws::Vec2f(10, 3.5)` or `ws::Vec3d(1u, 2.0f, 3L)`.
* **Construct from any type with `.x`, `.y` (and `.z` for 3D)** – works with `POINT`, `Gdiplus::Point`, `Gdiplus::PointF`, custom structs, etc.
* **Implicit conversion to any type with `.x`, `.y` (and `.z`)** – copies components via `static_cast`. Allows direct assignment to `Gdiplus::PointF`, `POINT`, or your own vector types.
* **Implicit conversion to `POINT*` / `const POINT*`** – `Vec2i` (and `Vec2f`, `Vec2d`) are layout‑compatible with `POINT`, so they can be passed directly to Win32 functions expecting `POINT*`.

#### `ws::Vec2i`
* Members: `int x, y`.
* `Vec2i()` – default.
* `Vec2i(T x_val, U y_val)` – arithmetic pair.
* `Vec2i(const T& other)` – from any `.x`/`.y` type.

#### `ws::Vec2f`
* Members: `float x, y`.
* Same construction/conversion patterns as `Vec2i`, using `float`.

#### `ws::Vec2d`
* Members: `double x, y`.
* Same patterns, using `double`.

#### `ws::Vec3i`
* Members: `int x, y, z`.
* `Vec3i()` – default.
* `Vec3i(T x, U y, V z)` – arithmetic triple.
* `Vec3i(const T& other)` – from any `.x`/`.y`/`.z` type.
* Implicit conversion to any `.x`/`.y`/`.z` type.

#### `ws::Vec3f`
* Members: `float x, y, z`.
* Same as `Vec3i`, with `float`.

#### `ws::Vec3d`
* Members: `double x, y, z`.
* Same as `Vec3i`, with `double`.


### Rectangle Types

All rectangle types (`IntRect`, `FloatRect`, `DoubleRect`) use a **left, top, width, height** internal representation but can be constructed from, and converted to, **right/bottom style** rects (like Win32 `RECT`) automatically.

#### `ws::IntRect`
* Members: `int left, top, width, height`.
* Constructors:
  - `IntRect()` – default.
  - `IntRect(T1 l, T2 t, T3 w, T4 h)` – from four arithmetic values.
  - `IntRect(const T& other)` – from **any** rect‑like type:
    - If `other` has `width`/`height`, copies directly.
    - If `other` has `right`/`bottom`, computes `width = right - left`, `height = bottom - top`.
* Conversion operator to **any** rect‑like type:
  - For `width`/`height` style: copies `left, top, width, height`.
  - For `right`/`bottom` style: sets `right = left + width`, `bottom = top + height`.
* Additional methods:
  - `bool contains(const Vec2i& point)` / `const Vec2f& point`
  - `bool operator==(const IntRect&)` / `operator!=`

#### `ws::FloatRect`
* Same interface as `IntRect`, but using `float` members.

#### `ws::DoubleRect`
* Same interface as `IntRect`, using `double` members.


### Color Class `ws::Hue`

#### Members & Constructors
* `int r, g, b, a` (0–255).
* **Default**: `Hue()` → (0,0,0,255).
* **From components**: `Hue(int r, int g, int b, int a = 255)`.
* **From GDI+ color**: `Hue(Gdiplus::Color &color)`.
* **From COLORREF**: `Hue(COLORREF color)` (alpha forced to 255).

#### Implicit Conversions
* `operator Gdiplus::Color()` – returns `Gdiplus::Color(a, r, g, b)`.
* `operator COLORREF()` – returns `RGB(r, g, b)` (drops alpha).

#### Predefined Color Constants
`red`, `green`, `blue`, `orange`, `brown`, `yellow`, `cyan`, `purple`, `pink`, `grey`, `black`, `white`, `transparent`.

#### HSV Support
* Nested struct `HSV { float h, s, v; }` (h in degrees 0–360, s and v in 0..1).
* `static Hue fromHSV(float h, float s, float v, int alpha = 255)` – creates from HSV.
* `static Hue fromHSV(HSV hsv, int alpha = 255)`.
* `HSV toHSV() const` – converts current color to HSV.

#### Hue Manipulation
* `static bool inHueRange(float hue, HSV hsv, float tolerance = 60, float minSaturation = 0.1, float minValue = 0.2)`  
  Checks if `hsv` is within `hue ± tolerance` degrees and meets minimum saturation/value.
* `static Hue replaceHue(float hue, float replacement, Hue rgb)`  
  If `rgb`’s hue falls inside `hue` range, returns a new `Hue` with `replacement` hue but same saturation/value; otherwise returns `rgb` unchanged.


## Class View
* ws::View()
* ws::View(const View& other)
* ws::View(View&& other) 
* operator=(const ws::View &other)
* operator=(ws::View &&other)
* void init(int portLeft,int portTop,int portWidth,int portHeight)
* void init(ws::IntRect rect)
* [[nodiscard]] ws::IntRect getRect()		
* void setRect(ws::IntRect rect)
* void setRect(int left,int top,int width,int height)
* [[nodiscard]] ws::IntRect getPortRect()
* void setPortRect(ws::IntRect rect)
* void setPortRect(int left,int top,int width,int height)
* void setSize(ws::Vec2i size)
* [[nodiscard]] ws::Vec2i getSize()
* void setPortSize(ws::Vec2i size)
* [[nodiscard]] ws::Vec2i getPortSize()
* [[nodiscard]] ws::Vec2i getCenter()
* void setCenter(int cx,int cy)
* void setCenter(ws::Vec2i pos)
* [[nodiscard]] ws::Vec2i getPortCenter()
* void setPortCenter(int cx,int cy)
* void setPortCenter(ws::Vec2i pos)
* void setPortRotatePoint(int ox,int oy)
* void setPortRotatePoint(ws::Vec2i pos)
* void setPortRotatePointCenter()
* [[nodiscard]] float getRotation() 
* void setRotation(float angle) 
* void setZoom(float val)
* [[nodiscard]] float getZoom()
* void move(float dx,float dy)
* void move(ws::Vec2f dir)
* void move(int dx,int dy)
* void getTransform(Gdiplus::Matrix &m) const
* void setTransform(const Gdiplus::Matrix &m)
* [[nodiscard]] ws::Vec2i toWorld(ws::Vec2i screenPos, ws::Vec2i screenSize) 
* [[nodiscard]] ws::Vec2i toWorld(int x,int y,ws::Vec2i screenSize) 
* [[nodiscard]] ws::Vec2i toScreen(ws::Vec2i worldPos,ws::Vec2i screenSize) 
* [[nodiscard]] ws::Vec2i toScreen(int x,int y,ws::Vec2i screenSize) 
* void apply(Gdiplus::Graphics &graphics) - not something you will ever need to use manually.


## Class Texture 
* enum class ScaleMode = { NearestNeighbor, Bilinear, Bicubic, HighQualityBicubic};
* ScaleMode scaleMode = ScaleMode::HighQualityBicubic;
* Gdiplus::Bitmap* bitmap;
* Texture() : bitmap(nullptr) {}
* Texture(std::string path)
* ~Texture()
* Texture(const Texture& other)
* Texture& operator=(const Texture& other)
* Texture(Texture&& other)
* Texture& operator=(Texture&& other) 
* bool create(int w, int h, Gdiplus::Color color = Gdiplus::Color(0,0,0,0))
* Gdiplus::Bitmap* getHandle()
* HDC getHDC() const
* HBITMAP getDIB() const
* HBITMAP getOldBMP() const
* void* getBITS() const
* bool isFastDIB() const
* bool loadFromFile(std::string path)
* bool loadFromMemory(const void* buffer, size_t bufferSize)
* bool loadFromBitmapPlus(Gdiplus::Bitmap& src)
* bool isValid() const
* void setScaleMode(ScaleMode mode) 
* ScaleMode getScaleMode() const 
* void setPixel(int index, ws::Hue color)
* void setPixel(int xIndex, int yIndex, ws::Hue color)
* ws::Hue getPixel(int index)
* ws::Hue getPixel(int xIndex, int yIndex)
* ws::Vec2i getSize() const 
* void setSize(int w,int h)
* void setSize(ws::Vec2i s)
* void setScale(float s)
* bool saveToFile(std::string path)


## Class Font
* Font()
* ~Font()
* Gdiplus::Font* getFontHandle()
* Gdiplus::FontFamily* getFamilyHandle()
* bool isSystemFont()
* std::string getFilePath()
* std::string getName()
* bool loadFromSystem(std::string name)
* bool loadFromFile(std::string path)
* bool isValid()


## Inheritable Base Class Drawable
* float x = 0, y = 0, z = 0;
* int width = 1, height = 1;
* ws::Vec2f scale = {1, 1};
* ws::Vec2i origin = {0, 0};
* float rotation = 0.0f;
* ws::Vec2i getSize() 
* ws::Vec2f getPosition() 
* ws::Vec2f getScale() 
* ws::Vec2i getOrigin()
* float getRotation() 
* void setSize(ws::Vec2i size) 
* void setSize(int w,int h) 
* void setPosition(float xpos, float ypos)
* void setPosition(ws::Vec2f pos)
* void setScale(ws::Vec2f s)
* void setScale(float sx, float sy)
* void setOrigin(ws::Vec2i pos)
* void setOrigin(int posx, int posy)
* void setRotation(float degrees) 
* void move(float dx, float dy)
* void move(const ws::Vec2f& delta)
* int getVisualWidth() const 
* int getVisualHeight() const 
* void getBounds(int& left, int& top, int& right, int& bottom) const
* ws::IntRect getBounds() const
* virtual bool contains(ws::Vec2i point)
* virtual void drawGlobal(Gdiplus::Graphics* graphics)
* virtual void draw(Gdiplus::Graphics* graphics) = 0;
* virtual ~Drawable() = default;



## Class Sprite - Inherits From ws::Drawable
* ws::Sprite()
* ws::Sprite(ws::Texture &texture)
* virtual bool contains(ws::Vec2i pos) override
* virtual void draw(Gdiplus::Graphics* graphics) override
* void setTexture(ws::Texture& texture,bool resize = true) {
* void setTextureRect(ws::IntRect rect) {
* ws::IntRect getTextureRect() const {
* ws::Texture &getTexture()
* const ws::Texture* getTexture() const
* bool hasTexture() const
* friend class Window;



## Class Line - Inherits From ws::Drawable
* ws::Vec2i start;
* ws::Vec2i end;
* Gdiplus::Color color = {255,0,0,255};
* Line(ws::Vec2i start = {0,0},ws::Vec2i end = {0,0},int thewidth = 2,Gdiplus::Color color = {255,0,0,255})
* virtual void draw(Gdiplus::Graphics* canvas) override 
* bool onSegment(ws::Vec2i p, ws::Vec2i q, ws::Vec2i r)
* int orientation(ws::Vec2i p, ws::Vec2i q, ws::Vec2i r)
* bool intersects(Line &otherLine)



## Class Poly - Inherits From ws::Drawable
* std::vector<ws::Vec2i> vertices;
* Poly() = default;
* Poly(std::vector<ws::Vec2i>& vertices, Gdiplus::Color fillColor = {255,255,0,0}, Gdiplus::Color borderColor = {255,255,0,255}, int borderWidth = 2, bool filled = true)
* void addVertex(ws::Vec2i vertex) 
* void addVertex(int x, int y) 
* void clear() 
* size_t vertexCount() 
* bool isValid() 
* ws::Vec2i getCentroid() 
* virtual bool contains(ws::Vec2i point) override 
* bool intersects(Line &line) 
* bool intersects(Poly &other) 
* ws::IntRect getBoundingRect() 
* void setTexture(ws::Texture &tex)
* void removeTexture()
* ws::Texture* getTexture()
* void setUV(size_t vertexIndex, float u, float v)
* void updateTexture()
* void setFillColor(Gdiplus::Color color)    
* void setBorderColor(Gdiplus::Color color)
* Gdiplus::Color getFillColor()
* Gdiplus::Color getBorderColor()
* void setBorderWidth(int w)
* int getBorderWidth()
* void setFilled(bool b = true)
* void setClosed(bool b = true)
* bool getFilled()
* bool getClosed()
* virtual void draw(Gdiplus::Graphics* canvas) override 



## Class Text - Inherits From ws::Drawable
* Text(){}
* ~Text(){}
* Text(ws::Font &newfont)
* void setFont(ws::Font &newFont)
* ws::Font* getFont()
* void setString(std::string str)
* std::string getString()
* void setCharacterSize(int size)
* int getCharacterSize()
* void setStyle(Gdiplus::FontStyle fontStyle)
* Gdiplus::FontStyle getStyle()
* void setFillColor(Gdiplus::Color color)
* Gdiplus::Color getFillColor()
* void setBorderColor(Gdiplus::Color color)
* Gdiplus::Color getBorderColor()
* void setBorderWidth(int w)
* int getBorderWidth()
* virtual bool contains(ws::Vec2i pos) override
* virtual void draw(Gdiplus::Graphics* canvas) override 




## Class Radial - Inherits From ws::Drawable
* Radial()
* void make(int points = 8)
* void setPosition(int posx,int posy)
* void setPosition(ws::Vec2i pos)
* void move(ws::Vec2i delta)
* void move(int deltaX,int deltaY)
* void setPointCount(int count)
* void setRadius(int size)
* void setFillColor(Gdiplus::Color color)
* void setBorderColor(Gdiplus::Color color)
* void setBorderWidth(int size)
* int getRadius()
* ws::Vec2i getPosition()
* int getPointCount()
* virtual void draw(Gdiplus::Graphics* canvas) override
* virtual bool contains(ws::Vec2i pos) override
* ws::Poly poly;


## Class Round - Inherits From ws::Drawable
* virtual void draw(Gdiplus::Graphics* canvas) override
* bool contains(int px,int py)
* void setBorderColor(Gdiplus::Color color)
* void setFillColor(Gdiplus::Color color)
* void setBorderWidth(int w)
* Gdiplus::Color getBorderColor()
* Gdiplus::Color getFillColor()
* int getBorderWidth()


## Class Cursor	
* enum class Type = 
*	// Standard system cursors (IDC_*)
*	Arrow,          // IDC_ARROW
*	IBeam,          // IDC_IBEAM
*	Wait,           // IDC_WAIT
*	Cross,          // IDC_CROSS
*	UpArrow,        // IDC_UPARROW
*	SizeNWSE,       // IDC_SIZENWSE
*	SizeNESW,       // IDC_SIZENESW
*	SizeWE,         // IDC_SIZEWE
*	SizeNS,         // IDC_SIZENS
*	SizeAll,        // IDC_SIZEALL
*	No,             // IDC_NO
*	Hand,           // IDC_HAND
*	AppStarting,    // IDC_APPSTARTING
*	Help,           // IDC_HELP
*	Pin,            // IDC_PIN (Windows 7+)
*	Person,         // IDC_PERSON (Windows 8+)

*	// OLE drag‑and‑drop cursors (from ole32.dll)
*	Copy,           // resource ID 2
*	Move,           // resource ID 3
*	Link            // resource ID 4

* HCURSOR getHandle()
* Cursor()
* Cursor(Type type)
* void loadAs(Type type)
* bool loadFromTexture(const ws::Texture& texture, int hotSpotX = 0, int hotSpotY = 0)
* bool loadFromFile(const std::string& filename)
* ~Cursor()
* Cursor(const Cursor& other) : animated(other.animated), srcPath(other.srcPath)
* Cursor& operator=(const Cursor& other)
* Cursor(Cursor&& other) noexcept
* Cursor& operator=(Cursor&& other) noexcept



## Class Window
* HWND hwnd;	
* ws::View view;
* std::vector<ws::Child*> children;
* ws::Texture backBuffer;
* Gdiplus::Graphics* canvas;
* Window()
* Window(int width,int height,std::string title = "",DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0,const std::string& className = "Window")
* void create(int width,int height,std::string title = "",DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0,const std::string& className = "Window")
* ~Window()
* void close()
* bool isOpen()
* bool pollEvent(MSG &message) {
* void clear(Gdiplus::Color color = Gdiplus::Color(255,0,0,0)) 
* void draw(ws::Drawable &draw)
* void display() 
* void setPixel(int x,int y,ws::Hue hue)
* ws::Hue getPixel(int x,int y)
* std::string getTitle()
* void setTitle(std::string title)
* void setView(ws::View &v)
* ws::View getView()
* void setVisible(bool val)
* bool getVisible()
* void setFocus()
* bool hasFocus()
* void setLayerAfter(HWND lastHwnd)
* void addStyle(DWORD style)
* void removeStyle(DWORD style)
* void setAllStyle(DWORD style)
* void addExStyle(DWORD style)
* void removeExStyle(DWORD style)
* void setAllExStyle(DWORD style)
* DWORD getExStyle() const
* DWORD getStyle() const
* bool hasStyle(DWORD checkStyle)
* bool hasExStyle(DWORD checkStyle)
* void setSize(ws::Vec2i size)
* void setSize(int screenWidth,int screenHeight)
* ws::Vec2i getSize() const
* void setPosition(ws::Vec2i pos)
* void setPosition(int posx,int posy)
* ws::Vec2i getPosition() const
* ws::Vec2i getTotalSize() const
* ws::Vec2i getClientPosition() const
* int getBorderWidth() const
* ws::IntRect getCaptionRect(bool excludeBorder = false) const
* void setFullscreen(bool fullscreen = true) 
* bool getFullscreen() const {
* void enableChromaKey(ws::Hue hue,bool legacy = false)//losing window control for some reason
* void disableChromaKey()
* void enableAlphaOnly(float alpha)
* void disableAlphaOnly()
* ws::Vec2i toWorld(int x,int y)
* ws::Vec2i toWorld(ws::Vec2i pos)
* ws::Vec2i toScreen(int x,int y)
* ws::Vec2i toScreen(ws::Vec2i pos)

* //Child Management Functions are optional and only work if you also include winsimple-controls.hpp
* void addChild(ws::Child &child);
* void removeChild(ws::Child &child);
* bool hasChild(ws::Child &child);

* void setCursor(ws::Cursor newcursor)
* ws::Cursor getCursor()
* void addMessageHandler(std::function<LRESULT(MSG msg)> handler)
* void clearMessageHandlers() {

### Minimal Window
```
ws::Window window;
window.create(960,540,"");
window.isOpen();//as long as this function is called, window will not stall.

//optional
window.clear()
window.draw(sprite);
window.display();
```	


## Global Input Namespace	
* namespace Global
* ws::Vec2i getMousePos(ws::Window &window)
* ws::Vec2i getMousePos()
* bool getButton(int button) - Eg. 'A' or VK_UP



## Class Screen 
* Screen()
* ~Screen()
* HDC getHDC()
* std::vector<ws::Vec2i> getDisplayModes()
* std::string setSize(int x,int y)
* std::string setSize(ws::Vec2i size)
* ws::Vec2i getSize()
* ws::Hue getPixel(int x,int y)
* ws::Hue getPixel(ws::Vec2i pos)
* void setPixel(int x,int y,ws::Hue hue)
* ws::Texture getSnapshot()
* float getDPI()
* ws::IntRect getWorkArea()

## Example

```
#include "winsimple.hpp"

int main()
{
	ws::Window window(960,540,"Window Title");
	
	ws::Texture texture;
	texture.create(960,540,ws::Hue::red);
	ws::Sprite sprite(texture);

	while(window.isOpen())
	{
		MSG msg;
		while(window.pollEvent(msg))
		{
			if(msg.message == WM_SIZE)
				std::cout << "Window resized" << std::endl;
		}
		window.clear();
		window.draw(sprite);
		window.display();
	}
	return 0;
}

```

