# Winsimple V1.7 API Documentation
 
Major V1.7 changes:
- Added `ws::Key` and `ws::Mouse` helper classes.
- Added `Vec2u`, `Vec3u`, `UnsignedRect`.
- `View` rect/port APIs now use `ws::FloatRect`.
- `Hue` HSV helpers changed: `HSV::toHue()`, `HSV::inHueRange()`, HSV setters; no `Hue::fromHSV()` or `Hue::replaceHue()`.
- `Texture::editAllPixels()` added.
- `Font::loadFromMemory()` added.
- `Drawable::drawToTexture()`, `Drawable::intersects()` added.
- `Sprite::drawBlend()`, `Sprite::Blt()` added.
- `Window` gained: `disableAnyClicks()`, `disableSomeClicks()`, `setBehindIcons()`, `setIcon()`, `getIcon()`, `setSourcePos()`, `getSourcePos()`, `getMessageHandlers()`. `clearMessageHandlers()` is not present.

---

* Core Linking: gdi32 user32 gdiplus 
* Animation Linking: None
* Audio linking: winmm
* Controls linking: comctl32 uuid cmdlg32 shell32
* Clipboard linking: ole32 uuid


## ws::Timer timer;
* `double seconds = timer.getSeconds();`
* `double milliSeconds = timer.getMilliSeconds();`
* `double microSeconds = timer.getMicroSeconds();`
* `double restartSeconds = timer.restart();`

## String Conversion Helpers
* `std::wstring wideString = ws::WIDE(myShortString);`
* `std::string shortString = ws::SHORT(wideString);`
* `LPCSTR myLPCSTR = ws::TO_LPCSTR(shortString);`
* `LPCWSTR myLPCWSTR = ws::TO_LPCWSTR(shortString);`

## Internal Helper Functions
* `inline std::wstring GetShortPathNameSafe(const std::wstring& longPath)`
* `inline int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)`
* `inline bool ResolveRelativePath(std::string& path)`

---

## Data Types

### Vectors
* `ws::Vec2i = int x,y`
* `ws::Vec2f = float x,y`
* `ws::Vec2d = double x,y`
* `ws::Vec2u = unsigned int x,y`
* `ws::Vec3i = int x,y,z`
* `ws::Vec3f = float x,y,z`
* `ws::Vec3d = double x,y,z`
* `ws::Vec3u = unsigned int x,y,z`

### Rects
* `ws::IntRect = int left,top,width,height`
* `ws::FloatRect = float left,top,width,height`
* `ws::DoubleRect = double left,top,width,height`
* `ws::UnsignedRect = unsigned int left,top,width,height`
* `bool ws::Rect::contains(ws::Vec2i point)`
* `bool ws::Rect::contains(ws::Vec2f point)`
* `bool ws::Rect::intersects(const ws::Rect& other) const`
* `bool ws::Rect::operator==(const ws::Rect& other) const`
* `bool ws::Rect::operator!=(const ws::Rect& other) const`

### Hue
* `ws::Hue()`
* `ws::Hue(Gdiplus::Color &color)`
* `ws::Hue(COLORREF color)`
* `ws::Hue(int r1,int g1,int b1,int a1=255)`
* `ws::Hue::r`
* `ws::Hue::g`
* `ws::Hue::b`
* `ws::Hue::a`
* `const Hue Hue::red = Hue(255, 0, 0, 255);`
* `const Hue Hue::green = Hue(0, 255, 0, 255);`
* `const Hue Hue::blue = Hue(0, 0, 255, 255);`
* `const Hue Hue::orange = Hue(255, 150, 0, 255);`
* `const Hue Hue::brown = Hue(150,100, 50, 255);`
* `const Hue Hue::yellow = Hue(255, 255, 0, 255);`
* `const Hue Hue::cyan = Hue(0, 255, 255, 255);`
* `const Hue Hue::purple = Hue(140, 0, 255, 255);`
* `const Hue Hue::pink = Hue(255, 0, 255, 255);`
* `const Hue Hue::grey = Hue(150, 150, 150, 255);`
* `const Hue Hue::black = Hue(0, 0, 0, 255);`
* `const Hue Hue::white = Hue(255, 255, 255, 255);`
* `const Hue Hue::transparent = Hue(0,0,0,0);`
* `operator Gdiplus::Color() const`
* `operator COLORREF() const`
* `bool operator==(const Hue& other) const`
* `bool operator!=(const Hue& other) const`
* `struct HSV { float h,s,v; }`
* `HSV()`
* `HSV(float h2,float s2,float v2)`
* `ws::Hue ws::Hue::HSV::toHue(int alpha = 255)`
* `bool ws::Hue::HSV::inHueRange(float hue,float tolerance = 60,float minSaturation = 0.1,float minValue = 0.2)`
* `void ws::Hue::HSV::setHue(float h2)`
* `void ws::Hue::HSV::setSaturation(float sat)`
* `void ws::Hue::HSV::setValue(float val)`
* `ws::Hue::HSV ws::Hue::toHSV() const`

### Key
* `ws::Key::A` … `ws::Key::Z`
* `ws::Key::Num0` … `ws::Key::Num9`
* `ws::Key::NumPad0` … `ws::Key::NumPad9`
* `ws::Key::Multiply, Add, Separator, Subtract, Decimal, Divide`
* `ws::Key::F1` … `ws::Key::F24`
* `ws::Key::Shift, Control, Alt`
* `ws::Key::LeftShift, RightShift, LeftControl, RightControl, LeftAlt, RightAlt`
* `ws::Key::LeftWin, RightWin, Application`
* `ws::Key::Backspace, Tab, Clear, Enter, Pause, CapsLock, Escape, Space`
* `ws::Key::PageUp, PageDown, End, Home, Left, Up, Right, Down`
* `ws::Key::Select, Print, Execute, Snapshot, Insert, Delete, Help`
* `ws::Key::NumLock, ScrollLock`
* `ws::Key::OemSemicolon, OemPlus, OemComma, OemMinus, OemPeriod, OemQuestion, OemTilde, OemOpenBrackets, OemBackslash, OemCloseBrackets, OemQuotes, Oem8`
* `ws::Key::BrowserBack, BrowserForward, BrowserRefresh, BrowserStop, BrowserSearch, BrowserFavorites, BrowserHome`
* `ws::Key::VolumeMute, VolumeDown, VolumeUp`
* `ws::Key::MediaNextTrack, MediaPrevTrack, MediaStop, MediaPlayPause`
* `ws::Key::LaunchMail, LaunchMediaSelect, LaunchApp1, LaunchApp2`
* `static const std::vector<int>& ws::Key::GetAllKeys()`
* `static const int ws::Key::GetKeyCount()`
* `static std::string ws::Key::GetKeyName(int keyCode)`

### Mouse
* `static const int ws::Mouse::Left = 0x01`
* `static const int ws::Mouse::Right = 0x02`
* `static const int ws::Mouse::Middle = 0x04`
* `static const int ws::Mouse::XButton1 = 0x05`
* `static const int ws::Mouse::XButton2 = 0x06`
* `static const std::vector<int>& ws::Mouse::GetAllButtons()`
* `static std::string ws::Mouse::GetButtonName(int buttonCode)`

---

## Class View
* `ws::View()`
* `ws::View(const View& other)`
* `ws::View(View&& other)`
* `operator=(const ws::View &other)`
* `operator=(ws::View &&other)`
* `void init(int portLeft,int portTop,int portWidth,int portHeight)`
* `void init(ws::FloatRect rect)`
* `[[nodiscard]] ws::FloatRect getRect()`
* `void setRect(ws::FloatRect rect)`
* `void setRect(int left,int top,int width,int height)`
* `[[nodiscard]] ws::FloatRect getPortRect()`
* `void setPortRect(ws::FloatRect rect)`
* `void setPortRect(int left,int top,int width,int height)`
* `void setSize(ws::Vec2i size)`
* `[[nodiscard]] ws::Vec2i getSize()`
* `void setPortSize(ws::Vec2i size)`
* `[[nodiscard]] ws::Vec2i getPortSize()`
* `[[nodiscard]] ws::Vec2i getCenter()`
* `void setCenter(int cx,int cy)`
* `void setCenter(ws::Vec2i pos)`
* `[[nodiscard]] ws::Vec2i getPortCenter()`
* `void setPortCenter(int cx,int cy)`
* `void setPortCenter(ws::Vec2i pos)`
* `void setPortRotatePoint(int ox,int oy)`
* `void setPortRotatePoint(ws::Vec2i pos)`
* `void setPortRotatePointCenter()`
* `[[nodiscard]] float getRotation()`
* `void setRotation(float angle)`
* `void setZoom(float val)`
* `[[nodiscard]] float getZoom()`
* `void move(float dx,float dy)`
* `void move(ws::Vec2f dir)`
* `void getTransform(Gdiplus::Matrix &m) const`
* `void setTransform(const Gdiplus::Matrix &m)`
* `[[nodiscard]] ws::Vec2i toWorld(ws::Vec2i screenPos, ws::Vec2i screenSize)`
* `[[nodiscard]] ws::Vec2i toWorld(int x,int y,ws::Vec2i screenSize)`
* `[[nodiscard]] ws::Vec2i toScreen(ws::Vec2i worldPos,ws::Vec2i screenSize)`
* `[[nodiscard]] ws::Vec2i toScreen(int x,int y,ws::Vec2i screenSize)`
* `void apply(Gdiplus::Graphics &graphics)` — normally not needed manually.

## Class Texture
* `enum class ScaleMode = { NearestNeighbor, Bilinear, Bicubic, HighQualityBicubic };`
* `ScaleMode scaleMode = ScaleMode::HighQualityBicubic;`
* `Gdiplus::Bitmap* bitmap;`
* `Texture() : bitmap(nullptr) {}`
* `Texture(std::string path)`
* `~Texture()`
* `Texture(const Texture& other)`
* `Texture& operator=(const Texture& other)`
* `Texture(Texture&& other)`
* `Texture& operator=(Texture&& other)`
* `bool create(int w, int h, Gdiplus::Color color = Gdiplus::Color(0,0,0,0))`
* `Gdiplus::Bitmap* getHandle()`
* `HDC getHDC() const`
* `HBITMAP getDIB() const`
* `HBITMAP getOldBMP() const`
* `void* getBITS() const`
* `bool isFastDIB() const`
* `bool loadFromFile(std::string path)`
* `bool loadFromMemory(const void* buffer, size_t bufferSize)`
* `bool loadFromBitmapPlus(Gdiplus::Bitmap& src)`
* `bool isValid() const`
* `void setScaleMode(ScaleMode mode)`
* `ScaleMode getScaleMode() const`
* `void setPixel(int index, ws::Hue color)`
* `void setPixel(int xIndex, int yIndex, ws::Hue color)`
* `ws::Hue getPixel(int index)`
* `ws::Hue getPixel(int xIndex, int yIndex)`
* `template<typename Func> void editAllPixels(Func&& action, int start = 0, int end = -1)`
* `ws::Vec2i getSize() const`
* `void setSize(int w,int h)`
* `void setSize(ws::Vec2i s)`
* `void setScale(float s)`
* `bool saveToFile(std::string path)`

## Class Font
* `Font()`
* `~Font()`
* `Gdiplus::Font* getFontHandle()`
* `Gdiplus::FontFamily* getFamilyHandle()`
* `bool isSystemFont()`
* `std::string getFilePath()`
* `std::string getName()`
* `bool loadFromSystem(std::string name)`
* `bool loadFromFile(std::string path)`
* `bool loadFromMemory(const void* data, size_t size)`
* `bool isValid()`

## Inheritable Base Class Drawable
* `float x = 0, y = 0, z = 0;`
* `int width = 1, height = 1;`
* `ws::Vec2f scale = {1, 1};`
* `ws::Vec2i origin = {0, 0};`
* `float rotation = 0.0f;`
* `ws::Vec2i getSize()`
* `ws::Vec2f getPosition()`
* `ws::Vec2f getScale()`
* `ws::Vec2i getOrigin()`
* `float getRotation()`
* `void setSize(ws::Vec2i size)`
* `void setSize(int w,int h)`
* `void setPosition(float xpos, float ypos)`
* `void setPosition(ws::Vec2f pos)`
* `void setScale(ws::Vec2f s)`
* `void setScale(float sx, float sy)`
* `void setOrigin(ws::Vec2i pos)`
* `void setOrigin(int posx, int posy)`
* `void setRotation(float degrees)`
* `void move(float dx, float dy)`
* `void move(const ws::Vec2f& delta)`
* `int getVisualWidth() const`
* `int getVisualHeight() const`
* `void getBounds(int& left, int& top, int& right, int& bottom) const`
* `ws::IntRect getBounds() const`
* `bool intersects(ws::IntRect box)`
* `virtual bool contains(ws::Vec2i point)`
* `virtual void drawGlobal(Gdiplus::Graphics* graphics)`
* `virtual void draw(Gdiplus::Graphics* graphics) = 0;`
* `void drawToTexture(ws::Texture& target)`
* `virtual ~Drawable() = default;`

## Class Sprite — Inherits From ws::Drawable
* `ws::Sprite()`
* `ws::Sprite(ws::Texture &texture)`
* `virtual bool contains(ws::Vec2i pos) override`
* `virtual void draw(Gdiplus::Graphics* graphics) override`
* `void setTexture(ws::Texture& texture, bool resize = true)`
* `void setTextureRect(ws::IntRect rect)`
* `ws::IntRect getTextureRect() const`
* `ws::Texture &getTexture()`
* `const ws::Texture* getTexture() const`
* `bool hasTexture() const`
* `void drawBlend(ws::Texture &dest, float alphaEffect = 255, DWORD stretchMode = 0)`
* `void Blt(ws::Texture &dest, DWORD stretchMode = COLORONCOLOR)`
* `friend class Window;`

## Class Line — Inherits From ws::Drawable
* `ws::Vec2i start;`
* `ws::Vec2i end;`
* `Gdiplus::Color color = {255,0,0,255};`
* `Line(ws::Vec2i start = {0,0}, ws::Vec2i end = {0,0}, int thewidth = 2, Gdiplus::Color color = {255,0,0,255})`
* `virtual void draw(Gdiplus::Graphics* canvas) override`
* `bool onSegment(ws::Vec2i p, ws::Vec2i q, ws::Vec2i r)`
* `int orientation(ws::Vec2i p, ws::Vec2i q, ws::Vec2i r)`
* `bool intersects(Line &otherLine)`

## Class Poly — Inherits From ws::Drawable
* `std::vector<ws::Vec2i> vertices;`
* `Poly() = default;`
* `Poly(std::vector<ws::Vec2i>& vertices, Gdiplus::Color fillColor = {255,255,0,0}, Gdiplus::Color borderColor = {255,255,0,255}, int borderWidth = 2, bool filled = true)`
* `void addVertex(ws::Vec2i vertex)`
* `void addVertex(int x, int y)`
* `void clear()`
* `size_t vertexCount()`
* `bool isValid()`
* `ws::Vec2i getCentroid()`
* `virtual bool contains(ws::Vec2i point) override`
* `bool intersects(Line &line)`
* `bool intersects(Poly &other)`
* `ws::IntRect getBoundingRect()`
* `void setTexture(ws::Texture &tex)`
* `void removeTexture()`
* `ws::Texture* getTexture()`
* `void setUV(size_t vertexIndex, float u, float v)`
* `void updateTexture()`
* `void setFillColor(Gdiplus::Color color)`
* `void setBorderColor(Gdiplus::Color color)`
* `Gdiplus::Color getFillColor()`
* `Gdiplus::Color getBorderColor()`
* `void setBorderWidth(int w)`
* `int getBorderWidth()`
* `void setFilled(bool b = true)`
* `void setClosed(bool b = true)`
* `bool getFilled()`
* `bool getClosed()`
* `virtual void draw(Gdiplus::Graphics* canvas) override`

## Class Text — Inherits From ws::Drawable
* `Text(){}`
* `~Text(){}`
* `Text(ws::Font &newfont)`
* `void setFont(ws::Font &newFont)`
* `ws::Font* getFont()`
* `void setString(std::string str)`
* `std::string getString()`
* `void setCharacterSize(int size)`
* `int getCharacterSize()`
* `void setStyle(Gdiplus::FontStyle fontStyle)`
* `Gdiplus::FontStyle getStyle()`
* `void setFillColor(Gdiplus::Color color)`
* `Gdiplus::Color getFillColor()`
* `void setBorderColor(Gdiplus::Color color)`
* `Gdiplus::Color getBorderColor()`
* `void setBorderWidth(int w)`
* `int getBorderWidth()`
* `virtual bool contains(ws::Vec2i pos) override`
* `virtual void draw(Gdiplus::Graphics* canvas) override`

## Class Radial — Inherits From ws::Drawable
* `Radial()`
* `void make(int points = 8)`
* `void setPointCount(int count)`
* `void setRadius(int size)`
* `void setFillColor(Gdiplus::Color color)`
* `void setBorderColor(Gdiplus::Color color)`
* `void setBorderWidth(int size)`
* `int getRadius()`
* `int getPointCount()`
* `virtual void draw(Gdiplus::Graphics* canvas) override`
* `virtual bool contains(ws::Vec2i pos) override`
* `ws::Poly poly;`
* Position/move/getPosition come from `ws::Drawable` (`setPosition(float,float)`, `setPosition(ws::Vec2f)`, `move(float,float)`, `move(ws::Vec2f)`, `getPosition() -> ws::Vec2f`).

## Class Round — Inherits From ws::Drawable
* `virtual void draw(Gdiplus::Graphics* canvas) override`
* `bool contains(int px,int py)`
* `void setBorderColor(Gdiplus::Color color)`
* `void setFillColor(Gdiplus::Color color)`
* `void setBorderWidth(int w)`
* `Gdiplus::Color getBorderColor()`
* `Gdiplus::Color getFillColor()`
* `int getBorderWidth()`

## Class Cursor
* `enum class Type = { Arrow, IBeam, Wait, Cross, UpArrow, SizeNWSE, SizeNESW, SizeWE, SizeNS, SizeAll, No, Hand, AppStarting, Help, Pin, Person, Copy, Move, Link };`
* `HCURSOR getHandle()`
* `Cursor()`
* `Cursor(Type type)`
* `void loadAs(Type type)`
* `bool loadFromTexture(const ws::Texture& texture, int hotSpotX = 0, int hotSpotY = 0)`
* `bool loadFromFile(const std::string& filename)`
* `~Cursor()`
* `Cursor(const Cursor& other)`
* `Cursor& operator=(const Cursor& other)`
* `Cursor(Cursor&& other) noexcept`
* `Cursor& operator=(Cursor&& other) noexcept`

## Class Icon
* `Icon()`
* `Icon(std::string path)`
* `Icon(const void* buffer, size_t bufferSize)`
* `Icon(HICON icon)`
* `~Icon()`
* `operator HICON() const`
* `Icon& operator=(HICON icon)`
* `Icon(const Icon& other)`
* `Icon& operator=(const Icon& other)`
* `Icon(Icon&& other) noexcept`
* `Icon& operator=(Icon&& other) noexcept`
* `HICON getHandle()`
* `bool loadFromFile(std::string path)`
* `bool loadFromMemory(const void* buffer, size_t bufferSize)`
* `bool isValid()`
* `ws::Texture getTexture(int width, int height)`
* `ws::Texture getTexture(DWORD size = ICON_SMALL)`

## Class Window
* `HWND hwnd;`
* `ws::View view;`
* `std::vector<ws::Child*> children;`
* `ws::Texture backBuffer;`
* `Gdiplus::Graphics* canvas;`
* `Window()`
* `Window(int width,int height,std::string title = "",DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0,const std::string& className = "Window")`
* `void create(int width,int height,std::string title = "",DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0,const std::string& className = "Window")`
* `~Window()`
* `void close()`
* `bool isOpen()`
* `bool pollEvent(MSG &message)`
* `void clear(ws::Hue color = ws::Hue::transparent)`
* `void draw(ws::Drawable &draw)`
* `void display()`
* `void setPixel(int x,int y,ws::Hue hue)`
* `ws::Hue getPixel(int x,int y)`
* `std::string getTitle()`
* `void setTitle(std::string title)`
* `void setView(ws::View &v)`
* `ws::View getView()`
* `void setVisible(bool val)`
* `bool getVisible()`
* `void setFocus()`
* `bool hasFocus()`
* `void setLayerAfter(HWND lastHwnd)`
* `void addStyle(DWORD style)`
* `void removeStyle(DWORD style)`
* `void setAllStyle(DWORD style)`
* `void addExStyle(DWORD style)`
* `void removeExStyle(DWORD style)`
* `void setAllExStyle(DWORD style)`
* `DWORD getExStyle() const`
* `DWORD getStyle() const`
* `bool hasStyle(DWORD checkStyle)`
* `bool hasExStyle(DWORD checkStyle)`
* `void setSize(ws::Vec2i size)`
* `void setSize(int screenWidth,int screenHeight)`
* `ws::Vec2i getSize() const`
* `void setPosition(ws::Vec2i pos)`
* `void setPosition(int posx,int posy)`
* `ws::Vec2i getPosition() const`
* `ws::Vec2i getTotalSize() const`
* `ws::Vec2i getClientPosition() const`
* `int getBorderWidth() const`
* `ws::IntRect getCaptionRect(bool excludeBorder = false) const`
* `void setFullscreen(bool fullscreen = true)`
* `bool getFullscreen() const`
* `void disableAnyClicks()`
* `void disableSomeClicks(ws::Hue hue)`
* `void enableChromaKey(ws::Hue hue,bool legacy = false)`
* `void disableChromaKey()`
* `void enableAlphaOnly(float alpha)`
* `void disableAlphaOnly()`
* `static bool setBehindIcons(HWND source)`
* `bool setBehindIcons()`
* `ws::Vec2i toWorld(int x,int y)`
* `ws::Vec2i toWorld(ws::Vec2i pos)`
* `ws::Vec2i toScreen(int x,int y)`
* `ws::Vec2i toScreen(ws::Vec2i pos)`
* `void setSourcePos(ws::Vec2i pos)`
* `void setSourcePos(int x,int y)`
* `ws::Vec2i getSourcePos()`
* `void addChild(ws::Child &child);`
* `void removeChild(ws::Child &child);`
* `bool hasChild(ws::Child &child);`
* `static inline LRESULT (*s_handleNotifyForChildren)(Window* window, NMHDR* pnmh, UINT uMsg, WPARAM wParam, LPARAM lParam);`
* `void setCursor(ws::Cursor newcursor)`
* `ws::Cursor getCursor()`
* `bool setIcon(HICON icon,DWORD size = ICON_SMALL)`
* `bool setIcon(std::string file,DWORD size = ICON_SMALL)`
* `HICON getIcon()`
* `void addMessageHandler(std::function<LRESULT(MSG msg)> handler)`
* `std::vector<std::function<LRESULT(MSG msg)>> getMessageHandlers()`

## Global Input Namespace
* `namespace Global`
* `ws::Vec2i getMousePos(ws::Window &window)`
* `ws::Vec2i getMousePos()`
* `bool getButton(int button)` — e.g. `'A'` or `VK_UP`

## Class Screen
* `Screen()`
* `~Screen()`
* `HDC getHDC()`
* `std::vector<ws::Vec2i> getDisplayModes()`
* `std::string setSize(int x,int y)`
* `std::string setSize(ws::Vec2i size)`
* `ws::Vec2i getSize()`
* `ws::Hue getPixel(int x,int y)`
* `ws::Hue getPixel(ws::Vec2i pos)`
* `void setPixel(int x,int y,ws::Hue hue)`
* `ws::Texture getSnapshot()`
* `float getDPI()`
* `ws::IntRect getWorkArea()`

---

# Optional Modules

## winsimple-controls.hpp
Linking: `-lcomctl32 -lcomdlg32 -luuid -lshell32`

### ControlsInit
* `int maxControlID = 1000;`
* `ControlsInit controlsInit;`

### Child
* `HWND hwnd = NULL;`
* `DWORD style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;`
* `DWORD exStyle = 0;`
* `DWORD textStyle = DT_CENTER | DT_VCENTER | DT_SINGLELINE;`
* `unsigned int controlID = 0;`
* `COLORREF backgroundColor = RGB(0,0,0);`
* `COLORREF textColor = RGB(255,255,255);`
* `COLORREF borderColor = RGB(0,0,0);`
* `Child(const std::wstring& className = L"Button")`
* `virtual ~Child()`
* `virtual bool init(ws::Window& parent)`
* `virtual bool init(HWND phwnd)`
* `void registerDestructorCallback(DestructorCallback callback)`
* `void clearDestructorCallbacks()`
* `void addChild(ws::Child& child)`
* `void removeChild(ws::Child &child)`
* `bool hasChild(ws::Child &child)`
* `void setClass(const std::wstring& className = L"Button")`
* `std::string getClass()`
* `void setVisible(bool visible)`
* `void setPosition(int xPos, int yPos)`
* `void setPosition(ws::Vec2i pos)`
* `ws::Vec2i getPosition() const`
* `virtual void setSize(int w, int h)`
* `virtual void setSize(ws::Vec2i size)`
* `ws::Vec2i getSize() const`
* `HFONT getFontHandle()`
* `void addStyle(DWORD addedStyle)`
* `void removeStyle(DWORD removedStyle)`
* `bool hasStyle(DWORD checkStyle) const`
* `void addExStyle(DWORD addedStyle)`
* `void removeExStyle(DWORD removedStyle)`
* `bool hasExStyle(DWORD checkStyle) const`
* `void setText(const std::string& newText)`
* `std::string getText() const`
* `void setFont(ws::Font& font, ws::Text& textSettings)`
* `bool contains(ws::Vec2i point) const`
* `virtual bool handleCommand(MSG &msg)`
* `virtual bool handleNotify(NMHDR* pnmh)`

### Tabs : public ws::Child
* `void addPage(const std::string& title, ws::Child& page)`
* `void removePage(ws::Child &child)`
* `bool hasPage(ws::Child &child)`
* `void setSelected(int index)`
* `int getSelected() const`
* `virtual void setSize(int w, int h) override`
* `virtual bool handleNotify(NMHDR* pnmh) override`
* `void updatePagePositions()`

### ComboBox : public Child
* `void addItem(const std::string& item)`
* `void addItems(const std::vector<std::string>& items)`
* `void removeItem(int index)`
* `void clear()`
* `int getSelectedIndex()`
* `void setSelectedIndex(int index)`
* `std::string getSelectedText()`
* `int getItemCount()`
* `std::string getItemText(int index)`
* `bool selectionChanged(MSG &msg)`
* `void setDropdownStyle(bool allowEdit = true)`
* `std::string getEditText()`
* `void setEditText(const std::string& text)`

### Button : public Child
* `bool isPressed(MSG &msg)`

### Slider : public Child
* `void setHorizontal()`
* `void setVertical()`
* `void setRange(int minimum = 0, int maximum = 100)`
* `void setSlidePosition(int pos = 0)`
* `int getSlidePosition()`

### ScrollBar : public ws::Child
* `ScrollBar(bool vertical = true)`
* `void setVertical()`
* `void setHorizontal()`
* `void setRange(int minVal, int maxVal)`
* `void setPageSize(int page)`
* `void setSlidePos(int pos)`
* `int getSlidePos()`

### TextBox : public Child
* `void setCharacterLimit(int max_chars = 0)`
* `bool getFocus()`

### Label : public Child

### ColorDialog
* `void init(ws::Window &newParent)`
* `ws::Window *getParent()`
* `void addFlag(DWORD newFlag)`
* `void removeFlag(DWORD removeFlag)`
* `DWORD getFlags()`
* `void setInitColor(ws::Hue hue)`
* `ws::Hue getInitColor()`
* `ws::Hue getResult()`
* `bool open()`

### Dropdown
* `Dropdown(int newID, std::string newName)`
* `void addItem(int id, DWORD type, std::string itemName)`
* `void addSubmenu(Dropdown &submenu)`
* `HMENU getHandle()`
* `std::string getName()`
* `int getID()`
* `void addItem(Dropdown drop)`

### Menu
* `HMENU bar;`
* `Menu()`
* `void addDropdown(ws::Dropdown &drop)`
* `void setVisible(bool visible, ws::Window &window)`
* `int getEvent(MSG &m)`
* `ws::Window &getWindow()`

### ClickMenu
* `void addFlag(DWORD newFlag)`
* `void removeFlag(DWORD removeFlag)`
* `DWORD getFlags()`
* `int getResult()`
* `std::vector<std::string> getList()`
* `void setList(std::vector<std::string> newList)`
* `void addItem(std::string item)`
* `void removeItem(std::string item)`
* `void init(ws::Window &newParent)`
* `ws::Window *getParent()`
* `bool open(ws::Vec2i mouse)`

### FileWindow
* `void setInitResult(std::string file)`
* `std::string getResult()`
* `void setTitle(std::string name)`
* `std::string getTitle()`
* `void addFlag(DWORD newFlag)`
* `void removeFlag(DWORD removeFlag)`
* `DWORD getFlags()`
* `bool open(ws::Window *parent = nullptr)`
* `bool save(ws::Window *parent = nullptr)`

### FolderWindow
* `void setTitle(std::string name)`
* `std::string getTitle()`
* `void addFlag(DWORD flag)`
* `void setFlags(DWORD allFlags)`
* `void removeFlag(DWORD flag)`
* `DWORD getFlags()`
* `std::string getResult()`
* `bool open(ws::Window *parent = nullptr)`
* `void setInitResult(std::string folder)`

### ExploreWindow
* `ExploreWindow(std::string mode = "open")`
* `void addStyle(DWORD style)`
* `void removeStyle(DWORD style)`
* `void setTitle(std::string title)`
* `void setInitResult(std::string folder)`
* `std::string getResult()`
* `bool open(ws::Window *parent = nullptr)`

### ListBox : public Child
* `void addItem(const std::string& item)`
* `void addItems(const std::vector<std::string>& items)`
* `void removeItem(int index)`
* `void clear()`
* `int getSelectedIndex()`
* `std::string getSelectedText()`
* `std::string getItemText(int index)`
* `void setSelectedIndex(int index)`
* `bool selectionChanged(MSG &msg)`
* `int getItemCount()`

### Balloon
* `bool Balloon(ws::Window &window, std::string message, std::string title, HICON hIcon, DWORD messageIconType = NIIF_USER, int timeoutMilliseconds = 5000, DWORD styles = NIF_ICON | NIF_TIP | NIF_INFO)`

### TrayIcon
* `bool init(ws::Window &window, std::string title, HICON hIcon, DWORD styles = NIF_ICON | NIF_MESSAGE | NIF_TIP)`
* `bool isPressed(int button)`
* `bool showBalloon(const std::string& message, const std::string& title, DWORD infoFlags = NIIF_USER, int timeoutMs = 5000)`

---

## winsimple-animation.hpp

### GIF
* `int width = 0;`
* `int height = 0;`
* `GIF()`
* `~GIF()`
* `bool loadFromFile(std::string path)`
* `void addFrame(ws::Texture &newFrame, double millisecondDelay)`
* `ws::Texture& getTexture()`
* `ws::Texture* getFrame(int index)`
* `double getFrameDelay(int index)`
* `bool setFrameDelay(int index, double newDelay)`
* `bool setDelay(double newDelay)`
* `bool setFrame(int index, ws::Texture &newTexture)`
* `bool getLoop()`
* `void setLoop(bool trueFalse = true)`
* `int getFrameCount()`
* `int getCurrentFrame()`
* `void play()`
* `void pause()`
* `void stop()`
* `std::string getStatus()`
* `ws::Texture& update()`
* `std::string getPath()`

### ShiftData
* `int currentframe = 0;`
* `float delay = 0.15f;`
* `bool ended = false;`
* `bool start = true;`
* `bool loop = false;`
* `std::vector<ws::IntRect> rect;`
* `ws::Timer timer;`
* `void add(int left, int top, int width, int height)`
* `void add(ws::IntRect r)`

### Shift
* `ws::IntRect Shift(ShiftData &shift)`

---

## winsimple-audio.hpp
Linking: `-lwinmm`

### Wav
* `std::string path;`
* `int channel = 0;`
* `bool blocking = true;`
* `std::string ID = "";`
* `std::string extension = "none";`
* `Wav(std::string path = "", int channel = 0, bool blocking = true)`
* `~Wav()`
* `static int getFreeChannel()`
* `std::string getChannelStatus()`
* `static std::string getChannelStatusMCI(int m_channel)`
* `static bool PlayFree(std::string m_path, int m_channel, bool m_blocking = false)`
* `bool loadFromMemory(const void* data, size_t size, int m_channel, bool m_blocking, std::string extension)`
* `bool open(std::string m_path, int m_channel, bool m_blocking = true)`
* `void play()`
* `static void stop(int m_channel)`
* `void stop()`
* `bool setVolume(int percent)`
* `int getVolume()`
* `bool setProgress(long seconds)`
* `long getProgress()`
* `float getLength()`
* `bool isFinished()`

---

## winsimple-clipboard.hpp
Linking: `-lole32 -luuid`

### ClipboardInit
* `ClipboardInit clipboardInit;`

### ClipData
* `void setTexture(ws::Texture tex)`
* `void setText(std::string str)`
* `void setFiles(std::vector<std::string> filesVec)`
* `ws::Texture getTexture() const`
* `std::string getText() const`
* `const std::vector<std::string>& getFiles() const`

### Clipboard
* `ClipData paste()`
* `bool copyText(const std::string& str)`
* `bool copyFile(const std::string& filePath)`
* `bool copyFiles(const std::vector<std::string>& filePaths)`
* `bool copyTexture(ws::Texture &texture, ws::IntRect rect = {0,0,0,0})`
* `std::vector<std::string> pasteFiles()`
* `std::string pasteFile()`
* `bool hasFiles()`
* `bool hasText()`
* `bool hasTexture()`
* `bool clear()`

### DropEffect
* `enum class DropEffect { None = DROPEFFECT_NONE, Copy = DROPEFFECT_COPY, Move = DROPEFFECT_MOVE, Link = DROPEFFECT_LINK };`

### DropTarget
* `void acceptType(std::string type, ws::DropEffect effect = ws::DropEffect::Copy)`
* `void rejectType(std::string type)`
* `void rejectAll()`
* `void onlyAcceptIf(std::string droptype, std::function<bool()> function)`
* `bool pollDrop(ws::ClipData& outData, DropEffect& outEffect)`
* `void setWindow(ws::Window &window)`
* COM methods: `QueryInterface`, `AddRef`, `Release`, `DragEnter`, `DragOver`, `DragLeave`, `Drop`