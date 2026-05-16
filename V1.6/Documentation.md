# Winsimple V1.6 API Documentation


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




## Data Types
* ws::Vec2i = int x,y
* ws::Vec2f = float x,y
* ws::Vec2d = double x,y
* ws::Vec3i = int x,y,z
* ws::Vec3f = float x,y,z
* ws::Vec3d = double x,y,z
* ws::IntRect = int left,top,width,height
* bool ws::IntRect::contains(ws::Vec2i point)
* ws::FloatRect = float left,top,width,height
* bool ws::FloatRect::contains(ws::Vec2i point)
* ws::DoubleRect = double left,top,width,height
* bool ws::DoubleRect::contains(ws::Vec2i point)

* ws::Hue()
* ws::Hue(Gdiplus::Color &color)
* ws::Hue(COLORREF color)
* ws::Hue(int r1,int g1,int b1,int a1=255)
* ws::Hue::r
* ws::Hue::g
* ws::Hue::b
* const Hue Hue::red = Hue(255, 0, 0, 255);
* const Hue Hue::green = Hue(0, 255, 0, 255);
* const Hue Hue::blue = Hue(0, 0, 255, 255);
* const Hue Hue::orange = Hue(255, 150, 0, 255);
* const Hue Hue::brown = Hue(150,100, 50, 255);
* const Hue Hue::yellow = Hue(255, 255, 0, 255);
* const Hue Hue::cyan = Hue(0, 255, 255, 255);
* const Hue Hue::purple = Hue(140, 0, 255, 255);
* const Hue Hue::pink = Hue(255, 0, 255, 255);
* const Hue Hue::grey = Hue(150, 150, 150, 255);
* const Hue Hue::black = Hue(0, 0, 0, 255);
* const Hue Hue::white = Hue(255, 255, 255, 255);
* const Hue Hue::transparent = Hue(0,0,0,0);	
* operator Gdiplus::Color() const
* operator COLORREF() const
* bool operator==(const Hue& other) const 
* bool operator!=(const Hue& other) const 
* struct HSV = float h,s,v
* ws::Hue ws::Hue::fromHSV(float h, float s, float v, int alpha = 255)
* ws::Hue ws::Hue::fromHSV(HSV hsv,int alpha = 255)	
* ws::Hue::HSV toHSV() const
* bool ws::Hue::inHueRange(float hue,ws::Hue::HSV hsv,float tolerance = 60,float minSaturation = 0.1,float minValue = 0.2)
* ws::Hue ws::Hue::replaceHue(float hue,float replacement,ws::Hue rgb)
	


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


