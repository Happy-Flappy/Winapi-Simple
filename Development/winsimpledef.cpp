#include "..\\src\\\winsimple.hpp"

#define GetAValue(value) (LOBYTE((value)>>24))
#define RGBA(value) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16))|((((DWORD)(BYTE)(b))<<24)))

namespace ws {

#ifdef WINSIMPLE_IMPL
void log(const std::string_view& msg) {
	if (Global::hConsoleOut == INVALID_HANDLE_VALUE) {
		// Get the console output handle
		Global::hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);

		// @CDevJoud
		// If there's an issue of getting the console output handle
		// then the user is compiling the program as window application 
		// this system disables the console window by default. the fix 
		// would be to allocate a console window to log to it. however 
		// if the user is running the program in release mode, we could 
		// consider to output to the IDE output window instead.
		if (Global::hConsoleOut == nullptr) {
			if (AllocConsole() == FALSE) {
				// Allocation of the console window failed miserably. it shouldn't even happen!
				MessageBox(nullptr, TEXT("Failed to allocate console window to debug logging"), TEXT("ERROR"), MB_ICONERROR | MB_OK);
				ExitProcess(EXIT_FAILURE);
			}
			Global::hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
		}
	}
	if (Global::hConsoleOut != INVALID_HANDLE_VALUE && Global::hConsoleOut != nullptr) {
		DWORD w;
		WriteConsoleA(Global::hConsoleOut, msg.data(), msg.length(), &w, 0);
		WriteConsoleA(Global::hConsoleOut, "\n", 1, &w, 0);
	}
}

std::string getWindowsVersion() {
	static std::string windowsVersion = "";

	if (!windowsVersion.empty())
		return windowsVersion;

	HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
	if (!hKernel32) {
		windowsVersion = "UNKNOWN";
		return "UNKNOWN";
	}
	if (!GetProcAddress(hKernel32, "AttachConsole")) {
		windowsVersion = "PRE_XP";
		return "PRE_XP";
	}

	HMODULE hNtdll = LoadLibraryW(L"ntdll.dll");
	if (!hNtdll) {
		windowsVersion = "UNKNOWN";
		return "UNKNOWN";
	}
	typedef LONG(WINAPI* RtlGetVersionFunc)(PRTL_OSVERSIONINFOW);
	RtlGetVersionFunc pRtlGetVersion = (RtlGetVersionFunc)GetProcAddress(hNtdll, "RtlGetVersion");

	if (!pRtlGetVersion) {
		FreeLibrary(hNtdll);
		windowsVersion = "UNKNOWN";
		return "UNKNOWN";
	}


	RTL_OSVERSIONINFOW osvi = {};
	osvi.dwOSVersionInfoSize = sizeof(osvi);

	LONG result = pRtlGetVersion(&osvi);
	FreeLibrary(hNtdll);

	if (result != 0) {
		windowsVersion = "UNKNOWN";
		return "UNKNOWN";
	}
	if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
		windowsVersion = "XP";
		return "XP";
	}

	if (osvi.dwMajorVersion == 6) {
		if (osvi.dwMinorVersion == 0) {
			windowsVersion = "Vista";
			return "Vista";
		}
		if (osvi.dwMinorVersion == 1) {
			windowsVersion = "7";
			return "7";
		}
		if (osvi.dwMinorVersion == 2) {
			windowsVersion = "8";
			return "8";
		}
		if (osvi.dwMinorVersion == 3) {
			windowsVersion = "8.1";
			return "8.1";
		}
	}

	if (osvi.dwMajorVersion == 10) {
		if (osvi.dwBuildNumber >= 22000) {
			windowsVersion = "11";
			return "11";
		}
		else {
			windowsVersion = "10";
			return "10";
		}
	}
	windowsVersion = "10";
	return "10";
}

namespace Global {

	// Returns mouse position relative to the given window's client area.
	ws::Vec2i getMousePosRelativeTo(ws::Window& window) {

		POINT p;
		if (!GetCursorPos(&p)) {
			return { 0,0 };
		}
		ScreenToClient(window.hwnd, &p); // Convert to client coordinates

		p.x += window.getSourcePos().x;
		p.y += window.getSourcePos().y;


		return p;
	}

	// Returns global mouse position.
	ws::Vec2i getMousePos() {

		POINT p;
		if (!GetCursorPos(&p)) {
			return { 0,0 };
		}

		return p;

	}

	// Checks if a virtual key/button is pressed.
	bool getButton(int button) {
		if ((GetAsyncKeyState(button) & 0x8000) != 0)
			return true;
		return false;
	}

}
	
Timer::Timer() {
	LARGE_INTEGER freq;
	QueryPerformanceCounter(&freq);
	Timer::frequency = static_cast<double>(freq.QuadPart);
	Timer::restart();
	}

float Timer::restart() {
	float sec = getSeconds();
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	Timer::startTime = counter.QuadPart;
	return sec;
}

float Timer::getSeconds() const {
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return static_cast<double>(currentTime.QuadPart - Timer::startTime) / Timer::frequency;
}
float Timer::getMilliSeconds() const {
	return Timer::getSeconds() * 1000.f;
}
float Timer::getMicroSeconds() const {
	return Timer::getMilliSeconds() * 1000.f;
}

std::wstring toUTF16(const std::string& str) {
	int size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);
	if (GetLastError() == ERROR_NO_UNICODE_TRANSLATION)
		size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
	std::wstring wstr(size, 0);
	if (size > 0)
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
	return wstr;
}

std::string toUTF8(const std::wstring& wstr) {
	int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	if (size == 0) return "";
	std::string str(size - 1, '\0');
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, str.data(), size, NULL, NULL);
	return str;
}

std::wstring GetShortPathNameSafe(const std::wstring& longPath) {
	DWORD size = GetShortPathNameW(longPath.c_str(), NULL, 0);
	if (size == 0) return L"";
	std::wstring shortPath(size, L'\0');
	size = GetShortPathNameW(longPath.c_str(), shortPath.data(), size);
	if (size == 0) return L"";
	shortPath.resize(size);
	return shortPath;
}
// Retrieves the CLSID of an image encoder based on MIME type.
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
	UINT num = 0, size = 0;
	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;
	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0) return -1;
	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)malloc(size);
	if (!pImageCodecInfo) return -1;
	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j) {
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}
	free(pImageCodecInfo);
	return -1;
}
// Makes a path absolute relative to the executable directory.
bool ResolveRelativePath(std::string& path) {
	if (path.empty()) return false;
	if ((path.size() > 1 && path[1] == ':') || path[0] == '\\' || path[0] == '/')
		return true;
	char exePath[MAX_PATH];
	GetModuleFileNameA(NULL, exePath, MAX_PATH);
	char* lastSlash = strrchr(exePath, '\\');
	if (lastSlash) *lastSlash = '\0';
	std::string dir(exePath);
	path = dir + "\\" + path;
	return true;
}


const Hue Hue::red = Hue(255, 0, 0, 255);
const Hue Hue::green = Hue(0, 255, 0, 255);
const Hue Hue::blue = Hue(0, 0, 255, 255);
const Hue Hue::orange = Hue(255, 150, 0, 255);
const Hue Hue::brown = Hue(150, 100, 50, 255);
const Hue Hue::yellow = Hue(255, 255, 0, 255);
const Hue Hue::cyan = Hue(0, 255, 255, 255);
const Hue Hue::purple = Hue(140, 0, 255, 255);
const Hue Hue::pink = Hue(255, 0, 255, 255);
const Hue Hue::grey = Hue(150, 150, 150, 255);
const Hue Hue::black = Hue(0, 0, 0, 255);
const Hue Hue::white = Hue(255, 255, 255, 255);
const Hue Hue::transparent = Hue(0, 0, 0, 0);

Hue::Hue(Gdiplus::Color& color) : 
	  r(color.GetR()) 
	, g(color.GetG())
	, b(color.GetB()) 
	, a(color.GetA()) {}

Hue::Hue(COLORREF color) : 
	  r(GetRValue(color)) 
	, g(GetGValue(color))
	, b(GetBValue(color)) 
	, a(GetAValue(color)) {}

Hue::Hue(
	unsigned char r, 
	unsigned char g, 
	unsigned char b, 
	unsigned char a) : 
	  r(r)
	, g(g)
	, b(b)
	, a(a) {}

Hue::operator Gdiplus::Color() const {
	return Gdiplus::Color(a, r, g, b);
}
Hue::operator COLORREF() const {
	return RGBA(r, g, b, a);
}

bool Hue::operator==(const Hue& other) const {
	return r == other.r 
		&& g == other.g 
		&& b == other.b 
		&& a == other.a;
}

bool Hue::operator!=(const Hue& other) const {
	return !(*this == other);
}

Hue::HSV::HSV() {
	h = s = v = 0.0f;
}

Hue::HSV::HSV(float h, float s, float v) : 
	  h(h)
	, s(s)
	, v(v) {}

ws::Hue Hue::HSV::toHue(unsigned char alpha) {
	// h: 0..360 degrees, s: 0..1, v: 0..1
	float c = v * s;
	float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
	float m = v - c;

	float r1 = 0, g1 = 0, b1 = 0;
	if (h < 60) {
		r1 = c; g1 = x; b1 = 0;
	}
	else if (h < 120) {
		r1 = x; g1 = c; b1 = 0;
	}
	else if (h < 180) {
		r1 = 0; g1 = c; b1 = x;
	}
	else if (h < 240) {
		r1 = 0; g1 = x; b1 = c;
	}
	else if (h < 300) {
		r1 = x; g1 = 0; b1 = c;
	}
	else {
		r1 = c; g1 = 0; b1 = x;
	}

	int r = static_cast<int>((r1 + m) * 255);
	int g = static_cast<int>((g1 + m) * 255);
	int b = static_cast<int>((b1 + m) * 255);

	return ws::Hue(r, g, b, alpha);
}

bool Hue::HSV::inHueRange(float hue, float tolerance, float minSaturation, float minValue) {
	float lower = hue - tolerance;
	float upper = hue + tolerance;

	bool hueInRange = false;
	if (lower < 0) {
		hueInRange = (h >= (360.0f + lower) || h <= upper);
	}
	else if (upper > 360) {
		hueInRange = (h >= lower || h <= (upper - 360.0f));
	}
	else {
		hueInRange = (h >= lower && h <= upper);
	}


	return (hueInRange && (s >= minSaturation) && (v >= minValue));
}

void Hue::HSV::setHue(float h2) {
	h = fmod(h2, 360.0f);
	if (h < 0) h += 360.0f;
}
void Hue::HSV::setSaturation(float sat) {
	s = sat;
	if (sat < 0.0f) s = 0.0f;
	if (sat > 1.0f) s = 1.0f;
}
void Hue::HSV::setValue(float val) {
	v = val;
	if (val < 0.0f) v = 0.0f;
	if (val > 1.0f) v = 1.0f;
}

Hue::HSV Hue::toHSV() const {
	float rNorm = r / 255.0f;
	float gNorm = g / 255.0f;
	float bNorm = b / 255.0f;

	float maxVal = std::max({ rNorm, gNorm, bNorm });
	float minVal = std::min({ rNorm, gNorm, bNorm });
	float delta = maxVal - minVal;

	float hue = 0.0f;
	if (delta > 0.0f) {
		if (maxVal == rNorm)
			hue = 60.0f * (fmod(((gNorm - bNorm) / delta), 6.0f));
		else if (maxVal == gNorm)
			hue = 60.0f * (((bNorm - rNorm) / delta) + 2.0f);
		else if (maxVal == bNorm)
			hue = 60.0f * (((rNorm - gNorm) / delta) + 4.0f);
	}
	if (hue < 0.0f) hue += 360.0f;

	float saturation = (maxVal == 0.0f) ? 0.0f : delta / maxVal;
	float value = maxVal;

	return { hue, saturation, value };
}

const unsigned char Keyboard::GetKeyCount() {
	return static_cast<unsigned char>(Keyboard::Key::COUNT);
}

const std::string_view Keyboard::GetKeyName(Keyboard::Key code) {
	static const std::unordered_map<Keyboard::Key, std::string> nameMap = [] {
		std::unordered_map<Keyboard::Key, std::string> map;
		// Use a macro or manual entries to avoid repetition.
		// I'll show manual entries for clarity.
#define ADD_KEY(k) map[k] = #k
		ADD_KEY(A); ADD_KEY(B); ADD_KEY(C); ADD_KEY(D); ADD_KEY(E);
		ADD_KEY(F); ADD_KEY(G); ADD_KEY(H); ADD_KEY(I); ADD_KEY(J);
		ADD_KEY(K); ADD_KEY(L); ADD_KEY(M); ADD_KEY(N); ADD_KEY(O);
		ADD_KEY(P); ADD_KEY(Q); ADD_KEY(R); ADD_KEY(S); ADD_KEY(T);
		ADD_KEY(U); ADD_KEY(V); ADD_KEY(W); ADD_KEY(X); ADD_KEY(Y);
		ADD_KEY(Z);
		ADD_KEY(Num0); ADD_KEY(Num1); ADD_KEY(Num2); ADD_KEY(Num3); ADD_KEY(Num4);
		ADD_KEY(Num5); ADD_KEY(Num6); ADD_KEY(Num7); ADD_KEY(Num8); ADD_KEY(Num9);
		ADD_KEY(NumPad0); ADD_KEY(NumPad1); ADD_KEY(NumPad2); ADD_KEY(NumPad3); ADD_KEY(NumPad4);
		ADD_KEY(NumPad5); ADD_KEY(NumPad6); ADD_KEY(NumPad7); ADD_KEY(NumPad8); ADD_KEY(NumPad9);
		ADD_KEY(Multiply); ADD_KEY(Add); ADD_KEY(Separator); ADD_KEY(Subtract);
		ADD_KEY(Decimal); ADD_KEY(Divide);
		ADD_KEY(F1); ADD_KEY(F2); ADD_KEY(F3); ADD_KEY(F4); ADD_KEY(F5);
		ADD_KEY(F6); ADD_KEY(F7); ADD_KEY(F8); ADD_KEY(F9); ADD_KEY(F10);
		ADD_KEY(F11); ADD_KEY(F12); ADD_KEY(F13); ADD_KEY(F14); ADD_KEY(F15);
		ADD_KEY(F16); ADD_KEY(F17); ADD_KEY(F18); ADD_KEY(F19); ADD_KEY(F20);
		ADD_KEY(F21); ADD_KEY(F22); ADD_KEY(F23); ADD_KEY(F24);
		ADD_KEY(Shift); ADD_KEY(Control); ADD_KEY(Alt);
		ADD_KEY(LeftShift); ADD_KEY(RightShift); ADD_KEY(LeftControl);
		ADD_KEY(RightControl); ADD_KEY(LeftAlt); ADD_KEY(RightAlt);
		ADD_KEY(LeftWin); ADD_KEY(RightWin); ADD_KEY(Application);
		ADD_KEY(Backspace); ADD_KEY(Tab); ADD_KEY(Clear); ADD_KEY(Enter);
		ADD_KEY(Pause); ADD_KEY(CapsLock); ADD_KEY(Escape); ADD_KEY(Space);
		ADD_KEY(PageUp); ADD_KEY(PageDown); ADD_KEY(End); ADD_KEY(Home);
		ADD_KEY(Left); ADD_KEY(Up); ADD_KEY(Right); ADD_KEY(Down);
		ADD_KEY(Select); ADD_KEY(Print); ADD_KEY(Execute); ADD_KEY(Snapshot);
		ADD_KEY(Insert); ADD_KEY(Delete); ADD_KEY(Help);
		ADD_KEY(NumLock); ADD_KEY(ScrollLock);
		ADD_KEY(OemSemicolon); ADD_KEY(OemPlus); ADD_KEY(OemComma);
		ADD_KEY(OemMinus); ADD_KEY(OemPeriod); ADD_KEY(OemQuestion);
		ADD_KEY(OemTilde); ADD_KEY(OemOpenBrackets); ADD_KEY(OemBackslash);
		ADD_KEY(OemCloseBrackets); ADD_KEY(OemQuotes); ADD_KEY(Oem8);
		ADD_KEY(BrowserBack); ADD_KEY(BrowserForward); ADD_KEY(BrowserRefresh);
		ADD_KEY(BrowserStop); ADD_KEY(BrowserSearch); ADD_KEY(BrowserFavorites);
		ADD_KEY(BrowserHome); ADD_KEY(VolumeMute); ADD_KEY(VolumeDown);
		ADD_KEY(VolumeUp); ADD_KEY(MediaNextTrack); ADD_KEY(MediaPrevTrack);
		ADD_KEY(MediaStop); ADD_KEY(MediaPlayPause); ADD_KEY(LaunchMail);
		ADD_KEY(LaunchMediaSelect); ADD_KEY(LaunchApp1); ADD_KEY(LaunchApp2);
#undef ADD_KEY
		return map;
		}();
	auto it = nameMap.find(code);
	if (it != nameMap.end())
		return it->second;
	log("Unknown(" + std::to_string(code) + ")");
	//return an empty string if its an invalid keycode
	return "";
}

const std::string_view Mouse::GetButtonName(Mouse::Button buttonCode) {
	static const std::unordered_map<Mouse::Button, std::string> nameMap = [] {
		std::unordered_map<Mouse::Button, std::string> map;
#define ADD_BTN(b) map[b] = #b
		ADD_BTN(Left);
		ADD_BTN(Right);
		ADD_BTN(Middle);
		ADD_BTN(XButton1);
		ADD_BTN(XButton2);
#undef ADD_BTN
		return map;
		}();
	auto it = nameMap.find(buttonCode);
	if (it != nameMap.end())
		return it->second;
	log("UnknownMouseButton(" + std::to_string(buttonCode) + ")");
	return "";
}

//============VIEW==============

View::View() {
	// @CDevJoud
	// init everything to default value which is zero.
	ZeroMemory(this, sizeof(View));
}

// Custom copy constructor because Gdiplus::Matrix lacks a normal one.
View::View(const View& other) :
	rotation(other.rotation),
	port(other.port),
	world(other.world),
	portOrigin(other.portOrigin),
	matrix(),
	zoom(other.zoom) {
	setTransform(other.matrix);
}

// move constructor

View::View(View&& other) noexcept :
	rotation(other.rotation),
	port(std::move(other.port)),
	world(std::move(other.world)),
	portOrigin(std::move(other.portOrigin)),
	matrix(),
	zoom(other.zoom) {
	setTransform(other.matrix);
	other.rotation = 0.0f;
	other.zoom = 0.0f;
}

// Copy assignment operator.
View& View::operator=(const View& other) {
	if (this != &other) {
		world = other.world;
		port = other.port;
		rotation = other.rotation;
		portOrigin = other.portOrigin;
		zoom = other.zoom;
		setTransform(other.matrix);
	}
	return *this;
}

// Move assignment operator.
View& View::operator=(View&& other) noexcept {
	if (this != &other) {
		world = std::move(other.world);
		port = std::move(other.port);
		rotation = other.rotation;
		portOrigin = std::move(other.portOrigin);
		setTransform(other.matrix);
		zoom = other.zoom;

		other.rotation = 0.0f;
		other.zoom = 0.0f;
	}
	return *this;
}

void View::init(int portLeft, int portTop, int portWidth, int portHeight) {
	port.left = portLeft;
	port.top = portTop;
	port.width = portWidth;
	port.height = portHeight;

	world = port;
}

// Initialises from an IntRect.
void View::init(ws::FloatRect rect) {
	init(rect.left, rect.top, rect.width, rect.height);
}


[[nodiscard]] ws::FloatRect View::getRect() {
	return world;
}

void View::setRect(ws::FloatRect rect) {
	world = rect;
}

void View::setRect(int left, int top, int width, int height) {
	setRect(ws::FloatRect(left, top, width, height));
}


[[nodiscard]] ws::FloatRect View::getPortRect() {
	return port;
}

void View::setPortRect(ws::FloatRect rect) {
	port = rect;
}

void View::setPortRect(int left, int top, int width, int height) {
	setPortRect(ws::FloatRect(left, top, width, height));
}


void View::setSize(ws::Vec2i size) {
	world.width = size.x;
	world.height = size.y;
}

[[nodiscard]] ws::Vec2i View::getSize() {
	return ws::Vec2i(world.width, world.height);
}

void View::setPortSize(ws::Vec2i size) {
	port.width = size.x;
	port.height = size.y;
}

[[nodiscard]] ws::Vec2i View::getPortSize() {
	return ws::Vec2i(port.width, port.height);
}


// Returns the center of the world rectangle.
[[nodiscard]] ws::Vec2i View::getCenter() {
	return ws::Vec2i(world.left + (world.width / 2), world.top + (world.height / 2));
}

// Sets the center of the world rectangle.
void View::setCenter(int cx, int cy) {
	world.left = cx - (world.width / 2);
	world.top = cy - (world.height / 2);
}

void View::setCenter(ws::Vec2i pos) {
	setCenter(pos.x, pos.y);
}


// Returns the center of the port rectangle.
[[nodiscard]] ws::Vec2i View::getPortCenter() {
	return ws::Vec2i(port.left + (port.width / 2), port.top + (port.height / 2));
}

// Sets the center of the port rectangle.
void View::setPortCenter(int cx, int cy) {
	ws::Vec2i pos = ws::Vec2i(cx - (port.width / 2), cy - (port.height / 2));
	port.left = pos.x;
	port.top = pos.y;
}


void View::setPortCenter(ws::Vec2i pos) {
	setPortCenter(pos.x, pos.y);
}


// Sets the point around which the view rotates.
void View::setPortRotatePoint(int ox, int oy) {
	portOrigin.x = ox;
	portOrigin.y = oy;
}

void View::setPortRotatePoint(ws::Vec2i pos) {
	setPortRotatePoint(pos.x, pos.y);
}

// Sets rotation point to the port center.
void View::setPortRotatePointCenter() {
	portOrigin = ws::Vec2i(port.left + (port.width / 2), port.top + (port.height / 2));
}


[[nodiscard]] float View::getRotation() {
	return rotation;
}

// Sets the rotation angle in degrees.
void View::setRotation(float angle) {
	rotation = angle;
}

// Sets the zoom level (exponent: 2^zoom).
void View::setZoom(float val) {
	zoom = val;
}

[[nodiscard]] float View::getZoom() {
	return zoom;
}

// Moves the world by a floating-point delta.
void View::move(float dx, float dy) {
	world.left += dx;
	world.top += dy;
}

void View::move(ws::Vec2f dir) {
	move(dir.x, dir.y);
}

// Copies the internal transformation matrix into the given matrix.
void View::getTransform(Gdiplus::Matrix& m) const {
	Gdiplus::REAL elements[6];
	matrix.GetElements(elements);
	m.SetElements(elements[0], elements[1], elements[2],
		elements[3], elements[4], elements[5]);
}

// Sets the internal transformation matrix from an external one.
void View::setTransform(const Gdiplus::Matrix& m) {
	Gdiplus::REAL elements[6];
	m.GetElements(elements);
	matrix.SetElements(elements[0], elements[1], elements[2],
		elements[3], elements[4], elements[5]);

}

// Converts screen coordinates to world coordinates, accounting for view transform.
[[nodiscard]] ws::Vec2i View::toWorld(ws::Vec2i screenPos, ws::Vec2i screenSize) {
	// 1. Map screen (client) coords to viewport (port) local coords
	float portX = static_cast<float>(screenPos.x) * (port.width / static_cast<float>(screenSize.x));
	float portY = static_cast<float>(screenPos.y) * (port.height / static_cast<float>(screenSize.y));

	// 2. Viewport local center
	float portCenterX = port.width / 2.0f;
	float portCenterY = port.height / 2.0f;

	// 3. World center
	float worldCenterX = world.left + world.width / 2.0f;
	float worldCenterY = world.top + world.height / 2.0f;

	// 4. Scale from world to port (including zoom)
	float scaleX = (port.width / world.width) * std::pow(2.0f, zoom);
	float scaleY = (port.height / world.height) * std::pow(2.0f, zoom);

	// 5. Invert the world→port transform (without rotation first)
	float worldX = (portX - portCenterX) / scaleX + worldCenterX;
	float worldY = (portY - portCenterY) / scaleY + worldCenterY;

	// 6. Apply inverse rotation (if any)
	if (rotation != 0.0f) {
		float dx = worldX - worldCenterX;
		float dy = worldY - worldCenterY;
		float rad = -rotation * static_cast<float>(M_PI) / 180.0f;
		float cosA = std::cos(rad);
		float sinA = std::sin(rad);
		worldX = dx * cosA - dy * sinA + worldCenterX;
		worldY = dx * sinA + dy * cosA + worldCenterY;
	}

	return ws::Vec2i(static_cast<int>(worldX), static_cast<int>(worldY));
}

[[nodiscard]] ws::Vec2i View::toWorld(int x, int y, ws::Vec2i screenSize) {
	return toWorld(ws::Vec2i(x, y), screenSize);
}


// Converts world coordinates to screen coordinates.
[[nodiscard]] ws::Vec2i View::toScreen(ws::Vec2i worldPos, ws::Vec2i screenSize) {
	// 1. World center
	float worldCenterX = world.left + world.width / 2.0f;
	float worldCenterY = world.top + world.height / 2.0f;

	// 2. Translate to origin
	float dx = static_cast<float>(worldPos.x) - worldCenterX;
	float dy = static_cast<float>(worldPos.y) - worldCenterY;

	// 3. Apply rotation
	if (rotation != 0.0f) {
		float rad = rotation * static_cast<float>(M_PI) / 180.0f;
		float cosA = std::cos(rad);
		float sinA = std::sin(rad);
		float newX = dx * cosA - dy * sinA;
		float newY = dx * sinA + dy * cosA;
		dx = newX;
		dy = newY;
	}

	// 4. Apply scale (world → port)
	float scaleX = (port.width / world.width) * std::pow(2.0f, zoom);
	float scaleY = (port.height / world.height) * std::pow(2.0f, zoom);
	float portX = dx * scaleX + port.width / 2.0f;
	float portY = dy * scaleY + port.height / 2.0f;

	// 5. Map port → screen (client) coordinates (inverse of the initial stretch)
	float screenX = portX * (static_cast<float>(screenSize.x) / port.width);
	float screenY = portY * (static_cast<float>(screenSize.y) / port.height);

	return ws::Vec2i(static_cast<int>(screenX), static_cast<int>(screenY));
}


[[nodiscard]] ws::Vec2i View::toScreen(int x, int y, ws::Vec2i screenSize) {
	return toScreen(ws::Vec2i(x, y), screenSize);
}

// Applies the current view transform (matrix, clip) to the given GDI+ graphics.
void View::apply(Gdiplus::Graphics& graphics) {
	updateMatrix();

	graphics.SetClip(Gdiplus::Rect(port.left, port.top, port.width, port.height));


	graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
	graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
	graphics.SetTransform(&matrix);
}
void View::updateMatrix() {
	matrix.Reset();

	// Port center
	float portCenterX = static_cast<float>(port.left) + port.width / 2.0f;
	float portCenterY = static_cast<float>(port.top) + port.height / 2.0f;

	// Visible world uses full world dimensions (zoom doesn't change visible area)
	float visibleWorldCenterX = static_cast<float>(world.left) + world.width / 2.0f;
	float visibleWorldCenterY = static_cast<float>(world.top) + world.height / 2.0f;

	// Scale to fit world into port
	float scaleX = static_cast<float>(port.width) / world.width;
	float scaleY = static_cast<float>(port.height) / world.height;

	// Apply zoom as a direct multiplier
	float zoomFactor = std::pow(2.0f, zoom);
	scaleX *= zoomFactor;
	scaleY *= zoomFactor;

	// Transform
	matrix.Translate(portCenterX, portCenterY);

	if (rotation != 0) {
		matrix.Rotate(rotation);
	}

	matrix.Scale(scaleX, scaleY);
	matrix.Translate(-visibleWorldCenterX, -visibleWorldCenterY);
}

//==========TEXTURE===========
Texture::Texture() {
	ZeroMemory(this, sizeof(Texture));
}
#endif
}

