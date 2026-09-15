//WINSIMPLE VERSION 1.7
#ifndef WINSIMPLE_HPP
#define WINSIMPLE_HPP

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef STRICT
#define STRICT
#endif

#define GetAValue(value) (LOBYTE((value)>>24))
#define RGBA(value) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16))|((((DWORD)(BYTE)(b))<<24)))

#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>

#include <iostream>
#include <string>
#include <string_view>
#include <cstdlib>
#include <map>
#include <queue>
#include <iomanip>
#include <cmath>
#include <cwchar>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <utility>
#include <mutex>
#include <set>
#include <memory>
#include <vector>

#ifndef SHORT
typedef short SHORT;
#endif
#ifndef PROPID
typedef unsigned long PROPID;
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef IDC_PIN
#define IDC_PIN MAKEINTRESOURCE(32671)
#endif
#ifndef IDC_PERSON
#define IDC_PERSON MAKEINTRESOURCE(32672)
#endif

#ifdef _MSC_VER
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdiplus.lib")
#endif

namespace ws
{

	//////////////////////////////////////////////////////////////////////////////
	//                              DEFINITIONS
	//////////////////////////////////////////////////////////////////////////////

	//For ws::Vec2
	template<typename T, typename = void>
	struct has_xy_members : std::false_type {};

	template<typename T>
	struct has_xy_members<T, std::void_t<
		decltype(std::declval<T>().x),
		decltype(std::declval<T>().y)>>
		: std::true_type {};

	//For ws::Vec3
	template<typename T, typename = void>
	struct has_xyz_members : std::false_type {};

	template<typename T>
	struct has_xyz_members<T, std::void_t<
		decltype(std::declval<T>().x),
		decltype(std::declval<T>().y),
		decltype(std::declval<T>().z)>>
		: std::true_type {};

	//ws::Rect 
	template<typename T, typename = void>
	struct has_width_height_style : std::false_type {};

	template<typename T>
	struct has_width_height_style<T, std::void_t<
		decltype(std::declval<T>().left),
		decltype(std::declval<T>().top),
		decltype(std::declval<T>().width),
		decltype(std::declval<T>().height)>>
		: std::true_type {};
	//Also for ws::Rect
	template<typename T, typename = void>
	struct has_right_bottom_style : std::false_type {};

	template<typename T>
	struct has_right_bottom_style<T, std::void_t<
		decltype(std::declval<T>().left),
		decltype(std::declval<T>().top),
		decltype(std::declval<T>().right),
		decltype(std::declval<T>().bottom)>>
		: std::true_type {};

	template<typename T>
	struct Vec2 {
		T x, y;

		Vec2() = default;

		// From two arithmetic values
		template<typename U, typename V, typename = std::enable_if_t<std::is_arithmetic_v<U>&& std::is_arithmetic_v<V>>>
		Vec2(U x_, V y_) : x(static_cast<T>(x_)), y(static_cast<T>(y_)) {}

		// From any type with .x and .y members (conversion constructor)
		template<typename U, typename = std::enable_if_t<ws::has_xy_members<U>::value>>
		Vec2(const U& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

		// ----- Conversion to other types with .x and .y members -----
		template<typename U, typename = std::enable_if_t<ws::has_xy_members<U>::value>>
		operator U() const {
			U result;
			result.x = static_cast<decltype(U::x)>(x);
			result.y = static_cast<decltype(U::y)>(y);
			return result;
		}

		// Special POINT pointer conversions
		operator POINT* () { return reinterpret_cast<POINT*>(this); }
		operator const POINT* () const { return reinterpret_cast<const POINT*>(this); }

		// ----- Compound assignments -----
		Vec2& operator+=(const Vec2& rhs) { x += rhs.x; y += rhs.y; return *this; }
		Vec2& operator-=(const Vec2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
		Vec2& operator*=(T scalar) { x *= scalar; y *= scalar; return *this; }
		Vec2& operator/=(T scalar) { if (scalar != 0) { x /= scalar; y /= scalar; } return *this; }
		Vec2& operator*=(const Vec2& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
		Vec2& operator/=(const Vec2& rhs) {
			if (rhs.x != 0) x /= rhs.x;
			if (rhs.y != 0) y /= rhs.y;
			return *this;
		}

		// ----- Binary operators
		Vec2 operator+(const Vec2& rhs) const { return Vec2(x + rhs.x, y + rhs.y); }
		Vec2 operator-(const Vec2& rhs) const { return Vec2(x - rhs.x, y - rhs.y); }
		Vec2 operator*(T scalar) const { return Vec2(x * scalar, y * scalar); }
		Vec2 operator/(T scalar) const { if (scalar == 0) return *this; return Vec2(x / scalar, y / scalar); }
		Vec2 operator*(const Vec2& rhs) const { return Vec2(x * rhs.x, y * rhs.y); }
		Vec2 operator/(const Vec2& rhs) const {
			if (rhs.x == 0 || rhs.y == 0) return *this;
			return Vec2(x / rhs.x, y / rhs.y);
		}

		// ----- Unary operators -----
		Vec2 operator+() const { return *this; }
		Vec2 operator-() const { return Vec2(-x, -y); }
	};

	using Vec2i = Vec2<int>;
	using Vec2f = Vec2<float>;
	using Vec2d = Vec2<double>;
	using Vec2u = Vec2<unsigned int>;


	template<typename T>
	struct Vec3 {
		T x, y, z;

		Vec3() = default;

		// From three arithmetic values
		template<typename U, typename V, typename W, typename = std::enable_if_t<std::is_arithmetic_v<U>&& std::is_arithmetic_v<V>&& std::is_arithmetic_v<W>>>
		Vec3(U x_, V y_, W z_) : x(static_cast<T>(x_)), y(static_cast<T>(y_)), z(static_cast<T>(z_)) {}

		// From any type with .x, .y and .z members (conversion constructor)
		template<typename U, typename = std::enable_if_t<ws::has_xyz_members<U>::value>>
		Vec3(const U& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)) {}

		// ----- Conversion to other types with .x, .y and .z members -----
		template<typename U, typename = std::enable_if_t<ws::has_xyz_members<U>::value>>
		operator U() const {
			U result;
			result.x = static_cast<decltype(U::x)>(x);
			result.y = static_cast<decltype(U::y)>(y);
			result.z = static_cast<decltype(U::z)>(z);
			return result;
		}

		// ----- Compound assignments -----
		Vec3& operator+=(const Vec3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
		Vec3& operator-=(const Vec3& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
		Vec3& operator*=(T scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
		Vec3& operator/=(T scalar) { if (scalar != 0) { x /= scalar; y /= scalar; z /= scalar; } return *this; }
		Vec3& operator*=(const Vec3& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
		Vec3& operator/=(const Vec3& rhs) {
			if (rhs.x != 0) x /= rhs.x;
			if (rhs.y != 0) y /= rhs.y;
			if (rhs.z != 0) z /= rhs.z;
			return *this;
		}

		// ----- Binary operators
		Vec3 operator+(const Vec3& rhs) const { return Vec3(x + rhs.x, y + rhs.y, z + rhs.z); }
		Vec3 operator-(const Vec3& rhs) const { return Vec3(x - rhs.x, y - rhs.y, z - rhs.z); }
		Vec3 operator*(T scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
		Vec3 operator/(T scalar) const { if (scalar == 0) return *this; return Vec3(x / scalar, y / scalar, z / scalar); }
		Vec3 operator*(const Vec3& rhs) const { return Vec3(x * rhs.x, y * rhs.y, z * rhs.z); }
		Vec3 operator/(const Vec3& rhs) const {
			if (rhs.x == 0 || rhs.y == 0 || rhs.z == 0) return *this;
			return Vec3(x / rhs.x, y / rhs.y, z / rhs.z);
		}

		// ----- Unary operators -----
		Vec3 operator+() const { return *this; }
		Vec3 operator-() const { return Vec3(-x, -y, -z); }


	};

	using Vec3i = Vec3<int>;
	using Vec3f = Vec3<float>;
	using Vec3d = Vec3<double>;
	using Vec3u = Vec3<unsigned int>;


	template<typename T>
	struct Rect {
		T left, top, width, height;

		Rect() = default;

		template<typename U, typename V, typename W, typename X,
			typename = std::enable_if_t<std::is_arithmetic_v<U>&&
			std::is_arithmetic_v<V>&&
			std::is_arithmetic_v<W>&&
			std::is_arithmetic_v<X>>>
			Rect(U l, V t, W w, X h)
			: left(static_cast<T>(l)), top(static_cast<T>(t)),
			width(static_cast<T>(w)), height(static_cast<T>(h)) {}

		template<typename U,
			typename = std::enable_if_t<ws::has_width_height_style<U>::value ||
			ws::has_right_bottom_style<U>::value>>
			Rect(const U& other) {
			if constexpr (ws::has_width_height_style<U>::value) {
				left = static_cast<T>(other.left);
				top = static_cast<T>(other.top);
				width = static_cast<T>(other.width);
				height = static_cast<T>(other.height);
			}
			else {
				left = static_cast<T>(other.left);
				top = static_cast<T>(other.top);
				width = static_cast<T>(other.right - other.left);
				height = static_cast<T>(other.bottom - other.top);
			}
		}

		template<typename U,
			typename = std::enable_if_t<ws::has_width_height_style<U>::value ||
			ws::has_right_bottom_style<U>::value>>
			operator U() const {
			U result;
			if constexpr (ws::has_width_height_style<U>::value) {
				result.left = static_cast<decltype(U::left)>(left);
				result.top = static_cast<decltype(U::top)>(top);
				result.width = static_cast<decltype(U::width)>(width);
				result.height = static_cast<decltype(U::height)>(height);
			}
			else {
				result.left = static_cast<decltype(U::left)>(left);
				result.top = static_cast<decltype(U::top)>(top);
				result.right = static_cast<decltype(U::right)>(left + width);
				result.bottom = static_cast<decltype(U::bottom)>(top + height);
			}
			return result;
		}

		bool intersects(const Rect& other) const {
			return !(other.left > left + width || other.top > top + height || other.left + other.width < left || other.top + other.height < top);
		}

		bool contains(const ws::Vec2i& point) const {
			return point.x >= left && point.x < left + width &&
				point.y >= top && point.y < top + height;
		}
		bool contains(const ws::Vec2f& point) const {
			return point.x >= left && point.x < left + width &&
				point.y >= top && point.y < top + height;
		}

		bool operator==(const Rect& other) const {
			return left == other.left && top == other.top &&
				width == other.width && height == other.height;
		}
		bool operator!=(const Rect& other) const {
			return !(*this == other);
		}
	};

	using IntRect = Rect<int>;
	using FloatRect = Rect<float>;
	using DoubleRect = Rect<double>;
	using UnsignedRect = Rect<unsigned int>;

	//All forward declares
	class Child;
	class WindowManager;
	class Window;

	void log(const std::string_view& msg);

	namespace Global {
		//////////////////////////////////////////////////////////////////////////////
		// Store global variables that will be accessed by the user or the library
		//////////////////////////////////////////////////////////////////////////////

		// Returns mouse position relative to the given window's client area.
		ws::Vec2i getMousePos(ws::Window& window);

		// Returns global mouse position.
		ws::Vec2i getMousePos();

		// Checks if a virtual key/button is pressed.
		bool getButton(int button);

		// hConsoleOut will store the handle output for the console window
#ifdef WINSIMPLE_IMPL
		HANDLE hConsoleOut = INVALID_HANDLE_VALUE;
#endif
	}

	// @CDevJoud: replaced warning() with log()
	/*static bool debugMode = true;
	void warning(std::string str)
	{
		if(debugMode)
			std::cerr << str << std::endl;
	}*/
	
	std::string getWindowsVersion();
	
	//<><><><><> Core Utilities <><><><><><>
	class Timer
	{
	public:
		// High-precision timer using QueryPerformanceCounter.
		Timer();
		~Timer() = default;
		
		//@CDevJoud: changed return type to float. 4 Byte IEEE754 is enough!
		// Resets start time and returns elapsed seconds since last restart.
		float restart();
		
	    // Returns elapsed seconds since start.
		float getSeconds() const;
	    
	    // Returns elapsed milliseconds since start.
		float getMilliSeconds() const;
	    
	    // Returns elapsed microseconds since start.
		float getMicroSeconds() const;
	private:
		LONGLONG startTime = 0;
		double frequency = 1.0;		
	};
	
	
	//@CDevJoud: renamed WIDE and SHORT
	
    // String conversion helpers
    // Converts a UTF-8 std::string to std::wstring, falling back to ACP on invalid chars.
	std::wstring toUTF16(const std::string& str);

    // Converts a wide string back to UTF-8.
	std::string toUTF8(const std::wstring& wstr);

    /*inline LPCSTR TO_LPCSTR(const std::string& str) { return str.c_str(); }
    inline LPCWSTR TO_LPCWSTR(const std::string& str) { return WIDE(str).c_str(); }*/

    // Internal helpers (used by Texture)
    // Returns the 8.3 short path name for the given long path.
	std::wstring GetShortPathNameSafe(const std::wstring& longPath);
    // Retrieves the CLSID of an image encoder based on MIME type.
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
    // Makes a path absolute relative to the executable directory.
	bool ResolveRelativePath(std::string& path);
	
	class Hue
	{
	public:
		unsigned char r = 0x00, g = 0x00, b = 0x00, a = 0xFF;
		
	    static const Hue red;
	    static const Hue green;
	    static const Hue blue;
	    static const Hue orange;
	    static const Hue brown;
	    static const Hue yellow;
	    static const Hue cyan;
	    static const Hue purple;
	    static const Hue pink;
	    static const Hue grey;
	    static const Hue black;
	    static const Hue white;
		static const Hue transparent;
		
		Hue() = default;
		
		// Constructs from GDI+ color.
		Hue(Gdiplus::Color& color);

		// Constructs from COLORREF (alpha forced to 255).
		Hue(COLORREF color);
		
		// Constructs from individual components.
		Hue(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
		
		// Implicit conversion to Gdiplus::Color.
		operator Gdiplus::Color() const;
		
		// Implicit conversion to COLORREF (drops alpha).
		operator COLORREF() const;

		bool operator==(const Hue& other) const;

		bool operator!=(const Hue& other) const;
		
		struct HSV {
			
			HSV(float h2, float s2, float v2);
			HSV();
			float h;  
			float s; 
			float v; 
		
			// Creates a Hue from HSV values (h in degrees, s,v in [0,1]).
			ws::Hue toHue(unsigned char alpha = 255);
			
			// Checks if the given HSV value falls within a hue range (with tolerance and min saturation/value).
			bool inHueRange(float hue, float tolerance = 60, float minSaturation = 0.1, float minValue = 0.2);

			void setHue(float h2);
			void setSaturation(float sat);
			void setValue(float val);
			
		};
		// Converts this color to HSV representation.
		HSV toHSV() const;
	};

	//KEYBOARD AND MOUSE BUTTON EQUIVALENTS TO WINAPI TYPES - Use these in ws::Global::getButton() or use the winapi equivalents.
	class Keyboard
	{
	public:
		enum Key : unsigned char {
			A = 'A',
			B = 'B',
			C = 'C',
			D = 'D',
			E = 'E',
			F = 'F',
			G = 'G',
			H = 'H',
			I = 'I',
			J = 'J',
			K = 'K',
			L = 'L',
			M = 'M',
			N = 'N',
			O = 'O',
			P = 'P',
			Q = 'Q',
			R = 'R',
			S = 'S',
			T = 'T',
			U = 'U',
			V = 'V',
			W = 'W',
			X = 'X',
			Y = 'Y',
			Z = 'Z',
			Num0 = '0',
			Num1 = '1',
			Num2 = '2',
			Num3 = '3',
			Num4 = '4',
			Num5 = '5',
			Num6 = '6',
			Num7 = '7',
			Num8 = '8',
			Num9 = '9',
			NumPad0 = 0x60,
			NumPad1 = 0x61,
			NumPad2 = 0x62,
			NumPad3 = 0x63,
			NumPad4 = 0x64,
			NumPad5 = 0x65,
			NumPad6 = 0x66,
			NumPad7 = 0x67,
			NumPad8 = 0x68,
			NumPad9 = 0x69,
			Multiply = 0x6A,
			Add = 0x6B,
			Separator = 0x6C,
			Subtract = 0x6D,
			Decimal = 0x6E,
			Divide = 0x6F,
			F1 = 0x70,
			F2 = 0x71,
			F3 = 0x72,
			F4 = 0x73,
			F5 = 0x74,
			F6 = 0x75,
			F7 = 0x76,
			F8 = 0x77,
			F9 = 0x78,
			F10 = 0x79,
			F11 = 0x7A,
			F12 = 0x7B,
			F13 = 0x7C,
			F14 = 0x7D,
			F15 = 0x7E,
			F16 = 0x7F,
			F17 = 0x80,
			F18 = 0x81,
			F19 = 0x82,
			F20 = 0x83,
			F21 = 0x84,
			F22 = 0x85,
			F23 = 0x86,
			F24 = 0x87,
			Shift = 0x10,
			Control = 0x11,
			Alt = 0x12,
			LeftShift = 0xA0,
			RightShift = 0xA1,
			LeftControl = 0xA2,
			RightControl = 0xA3,
			LeftAlt = 0xA4,
			RightAlt = 0xA5,
			LeftWin = 0x5B,
			RightWin = 0x5C,
			Application = 0x5D,
			Backspace = 0x08,
			Tab = 0x09,
			Clear = 0x0C,
			Enter = 0x0D,
			Pause = 0x13,
			CapsLock = 0x14,
			Escape = 0x1B,
			Space = 0x20,
			PageUp = 0x21,
			PageDown = 0x22,
			End = 0x23,
			Home = 0x24,
			Left = 0x25,
			Up = 0x26,
			Right = 0x27,
			Down = 0x28,
			Select = 0x29,
			Print = 0x2A,
			Execute = 0x2B,
			Snapshot = 0x2C,
			Insert = 0x2D,
			Delete = 0x2E,
			Help = 0x2F,
			NumLock = 0x90,
			ScrollLock = 0x91,
			OemSemicolon = 0xBA,
			OemPlus = 0xBB,
			OemComma = 0xBC,
			OemMinus = 0xBD,
			OemPeriod = 0xBE,
			OemQuestion = 0xBF,
			OemTilde = 0xC0,
			OemOpenBrackets = 0xDB,
			OemBackslash = 0xDC,
			OemCloseBrackets = 0xDD,
			OemQuotes = 0xDE,
			Oem8 = 0xDF,
			BrowserBack = 0xA6,
			BrowserForward = 0xA7,
			BrowserRefresh = 0xA8,
			BrowserStop = 0xA9,
			BrowserSearch = 0xAA,
			BrowserFavorites = 0xAB,
			BrowserHome = 0xAC,
			VolumeMute = 0xAD,
			VolumeDown = 0xAE,
			VolumeUp = 0xAF,
			MediaNextTrack = 0xB0,
			MediaPrevTrack = 0xB1,
			MediaStop = 0xB2,
			MediaPlayPause = 0xB3,
			LaunchMail = 0xB4,
			LaunchMediaSelect = 0xB5,
			LaunchApp1 = 0xB6,
			LaunchApp2 = 0xB7,

			COUNT = 159
		};

		static const std::vector<Keyboard::Key>& GetAllKeys();
		
		static const unsigned char GetKeyCount();

		static const std::string_view GetKeyName(Keyboard::Key keyCode);

		// Prevent instantiation
		Keyboard() = delete;
	};
	using Key = Keyboard;

	class Mouse {
	public:
		enum Button : unsigned char {
			Left = 0x01,
			Right = 0x02,
			Middle = 0x04,
			XButton1 = 0x05,
			XButton2 = 0x06,
			
			COUNT = 0x07
		};

		// @CDevJoud no need for this method
		//static const std::vector<int>& GetAllButtons();
		static const std::string_view GetButtonName(Mouse::Button buttonCode);

		Mouse() = delete;
	};

	//============VIEW==============
	class View
	{
	
	public:
		// Default constructor.
		View();
		
		// Custom copy constructor because Gdiplus::Matrix lacks a normal one.
		View(const View& other);
	  
		// move constructor
		View(View&& other) noexcept;
	    
		// Copy assignment operator.
		View& operator=(const View& other);
		
		// Move assignment operator.
		View& operator=(View&& other) noexcept;
		
		~View() = default;
		
		// Initialises view with a port rectangle, setting world to match.
		void init(int portLeft, int portTop, int portWidth, int portHeight);
		
		// Initialises from an IntRect.
		void init(ws::FloatRect rect);

		// Moves the world by a floating-point delta.
		void move(float dx, float dy);

		void move(ws::Vec2f dir);

		// Converts screen coordinates to world coordinates, accounting for view transform.
		[[nodiscard]] ws::Vec2i toWorld(ws::Vec2i screenPos, ws::Vec2i screenSize);
	    
		[[nodiscard]] ws::Vec2i toWorld(int x, int y, ws::Vec2i screenSize);
	    
	
		// Converts world coordinates to screen coordinates.
		[[nodiscard]] ws::Vec2i toScreen(ws::Vec2i worldPos, ws::Vec2i screenSize);
	    
		[[nodiscard]] ws::Vec2i toScreen(int x, int y, ws::Vec2i screenSize);

		// Applies the current view transform (matrix, clip) to the given GDI+ graphics.
		void apply(Gdiplus::Graphics& graphics);
			
		void setRect(ws::FloatRect rect);
		void setRect(int left, int top, int width, int height);

		void setPortRect(ws::FloatRect rect);
		void setPortRect(int left, int top, int width, int height);
		
		void setSize(ws::Vec2i size);
		void setPortSize(ws::Vec2i size);
		
		void setCenter(int cx, int cy);
		void setCenter(ws::Vec2i pos);
		
		void setPortCenter(int cx, int cy);
		void setPortCenter(ws::Vec2i pos);
		void setPortRotatePoint(int ox, int oy);
		void setPortRotatePoint(ws::Vec2i pos);
		void setPortRotatePointCenter();
		
		void setRotation(float angle);
		void setZoom(float val);
		
		void setTransform(const Gdiplus::Matrix& m);

		[[nodiscard]] ws::FloatRect getRect();
		[[nodiscard]] ws::FloatRect getPortRect();
		[[nodiscard]] ws::Vec2i getSize();
		[[nodiscard]] ws::Vec2i getPortSize();
		[[nodiscard]] ws::Vec2i getCenter();
		[[nodiscard]] ws::Vec2i getPortCenter();
		[[nodiscard]] float getRotation();
		[[nodiscard]] float getZoom();
		void getTransform(Gdiplus::Matrix& m) const;

	private:
		// Recalculates the transformation matrix based on current view settings.
		void updateMatrix();
		
	private:
		float rotation;
		ws::FloatRect port; //Port is always in screen coordinates.
		ws::FloatRect world; //World is the world coordinate section of the world that is sent to the view.
		ws::Vec2i portOrigin;//This is the point of rotation. It does NOT effect the view position.
		Gdiplus::Matrix matrix;
		float zoom;
	};

	//==========TEXTURE===========
	class Texture
	{
	public:
		
		enum class ScaleMode {
			NearestNeighbor,   // sharp edges, pixelated
			Bilinear,          // smooth, linear filter
			Bicubic,           // smoother, slightly more expensive
			HighQualityBicubic // best quality, slowest
		};

		Texture();

		// Constructs and loads from file.
		Texture(const std::string& path);

	    // Destructor cleans up GDI+ bitmap and possible DIB.
		~Texture();

		// Copy constructor
		Texture(const Texture& other);

		// Copy assignment
		Texture& operator=(const Texture& other);
		
		// Move constructor
		Texture(Texture&& other) noexcept;

		// Move assign
		Texture& operator=(Texture&& other) noexcept;

		// Creates a new texture as a DIBSection, enabling fast pixel access and BitBlt.
		bool create(int w, int h, Gdiplus::Color color = Gdiplus::Color(0, 0, 0, 0));
	
		Gdiplus::Bitmap* getHandle();
		
		HDC getHDC() const;

		HBITMAP getDIB() const;

		HBITMAP getOldBMP() const;

		void* getBITS() const;
		
		//returns whether this is a fast DIB section under the hood or is a GDI HBITMAP only.
		bool isFastDIB() const;

		// Loads from a file, creating a DIBSection-backed texture.
		bool loadFromFile(const std::string& path);

		// Loads from an in‑memory image buffer (PNG, JPEG, etc.).
		bool loadFromMemory(const void* buffer, size_t bufferSize);

		// Loads from an existing Gdiplus::Bitmap, converting it to DIBSection.
		bool loadFromBitmapPlus(Gdiplus::Bitmap& src);
		
	    // Returns true if a valid bitmap exists.
		bool isValid() const;
	    
		void setScaleMode(ScaleMode mode);
		ScaleMode getScaleMode() const;
	    
		// Sets a pixel using 1D index (x = index % width, y = index / width).
		void setPixel(int index, ws::Hue color);
		// Sets a pixel at (xIndex,yIndex) with color; fast DIB or GDI+ fallback.
		void setPixel(int xIndex, int yIndex, ws::Hue color);

		template<typename Func>
		void editAllPixels(Func&& action, int start = 0,int end = -1)
		{
			start = std::max(0,start);
			
			if(end <= 0)
				end = width * height;
			
			end = std::min(end,width * height);
			
			if(end <= start)
				return;
			
			if(!m_isFast || !m_dibBits) 
				return;

			uint8_t* pixel = static_cast<uint8_t*>(m_dibBits) + start * 4;
			for(int a = start; a < end; a++) {
				action(pixel,a);          
				pixel += 4;
			}
		}

	    
	    // Gets a pixel using 1D index.
		ws::Hue getPixel(int index);
		// Gets a pixel at (xIndex,yIndex); returns transparent black if out of bounds.
		ws::Hue getPixel(int xIndex, int yIndex);
	    
	    // Returns size as Vec2i.
		ws::Vec2i getSize() const;
		// Resizes the texture to (w,h) using high‑quality interpolation.
		void setSize(int w, int h);

		void setSize(ws::Vec2i s);

		// Scales the texture uniformly by factor s.
		void setScale(float s);

		// Saves the texture to a file (format inferred from extension).
		bool saveToFile(const std::string& path);

	private:
		// Destroys the DIBSection and its associated GDI objects before the GDI+ bitmap is freed.
		void destroyDIB();

		// Copies the bitmap data from another texture, preserving fast DIB if possible.
		void copyFrom(const Texture& other);
	private:
		int width;
		int height;

	private:
		HDC     m_hdcMem;
		HBITMAP m_hDIB;
		HBITMAP m_hOldBmp;
		void* m_dibBits;
		bool    m_isFast;
		public:

		

		ScaleMode scaleMode = ScaleMode::HighQualityBicubic;

		Gdiplus::Bitmap* bitmap;

	};

	//==========FONT===========
	class Font
	{
	public:
		// Initialises with default Arial font.
		Font();
		
		~Font();
		
		Gdiplus::Font* getFontHandle() const;
		
		Gdiplus::FontFamily* getFamilyHandle() const;
		
		// Returns true if loaded from a file (custom), false if system.
		bool isSystemFont() const;

		std::string getFilePath() const;
		
		std::string getName() const;

		// Loads a system font by name.
		bool loadFromSystem(const std::string& name);
		
		// Loads a font from a file (TrueType).
		bool loadFromFile(const std::string& path);
		
		//Load a font from memory.
		bool loadFromMemory(const void* data, size_t size);
		
		// Returns true if the font object is valid.
		bool isValid();
		
	private:
		// Creates/re‑creates the GDI+ font object from the current family/style/size.
		bool update();
		std::string fontName = "Arial";
	    Gdiplus::PrivateFontCollection* fontCollection;
	    Gdiplus::Font* gdiFont;
		Gdiplus::FontFamily* family;
		int foundFamily = 0;
		bool isCustomFont = false;
		std::string fontFilePath;
	};

	//=============DRAWABLE=============
	class Drawable
	{
	public:
	    //Coordinates and sizes are Local 
		
		float x = 0, y = 0, z = 0;
	    int width = 1, height = 1;
	    ws::Vec2f scale = {1, 1};
	    ws::Vec2i origin = {0, 0};
	    //Degrees
	    float rotation = 0.0f;

	    ws::Vec2i getSize() const;
	    ws::Vec2f getPosition() const;
	    ws::Vec2f getScale() const;
	    ws::Vec2i getOrigin() const;
	    float getRotation() const;
	    
	    void setSize(ws::Vec2i size);
	    void setSize(int w,int h);
        void setPosition(float xpos, float ypos);
        void setPosition(ws::Vec2f pos);
	    void setScale(ws::Vec2f s);
	    void setScale(float sx, float sy);
	    void setOrigin(ws::Vec2i pos);
	    void setOrigin(int posx, int posy);
	    void setRotation(float degrees);

		void move(float dx, float dy);
		void move(const ws::Vec2f& delta);
	    
	    // Returns visual width after scaling (absolute).
		int getVisualWidth() const;
	    
	    // Returns visual height after scaling (absolute).
		int getVisualHeight() const;
	    
	    // Computes world-space bounding rectangle of the drawable (after scale, rotation, origin).
		void getBounds(int& left, int& top, int& right, int& bottom) const;

		// Returns bounding rectangle as IntRect.
		ws::IntRect getBounds() const;
		
		// Tests if a world point is inside the visual shape, accounting for rotation.
		virtual bool contains(ws::Vec2i point);
	    
		bool intersects(ws::IntRect box);
		
	    // Draws the object in world space by applying the local transform matrix.
		virtual void drawGlobal(Gdiplus::Graphics* graphics);
	    
	    // Pure virtual - draw the content in local space
	    virtual void draw(Gdiplus::Graphics* graphics) = 0;
		
		//draws to a texture with full transforms using GDI+ DrawImage. - ignores window view transforms!
		void drawToTexture(ws::Texture& target);
		
	    virtual ~Drawable() = default;
	};

	//dynamically loaded function for AlphaBlend drawing.
    typedef BOOL (WINAPI *AlphaBlendFunc)(
        HDC hdcDest,
        int xDest, int yDest,
        int cxDest, int cyDest,
        HDC hdcSrc,
        int xSrc, int ySrc,
        int cxSrc, int cySrc,
        BLENDFUNCTION blendFunction
    );
	//============SPRITE=============
	class Sprite : public Drawable
	{
	public:
		
		// Default constructor.
		Sprite();
		
		// Constructs with a texture, setting the rect to full texture.
		Sprite(ws::Texture& texture);
	    
		// Checks if point lies inside sprite bounds.
		virtual bool contains(ws::Vec2i pos) override;
	    // Draws the sprite texture with the texture's scale mode.
		virtual void draw(Gdiplus::Graphics* graphics) override;
		// Sets the texture and optionally resizes sprite to texture size.
		void setTexture(ws::Texture& texture, bool resize = true);
	    
	    // Sets the source rectangle from the texture.
		void setTextureRect(ws::IntRect rect);
	    
	    // Returns the current source rectangle.
		ws::IntRect getTextureRect() const;
		
		// Returns reference to the attached texture.
		Texture& getTexture();
		
		const Texture* getTexture() const;
		
		bool hasTexture() const;
		//Draws to a texture using AlphaBlend function -  this does not support complex transforms. - ignores window view transforms!
		void drawBlend(ws::Texture& dest, float alphaEffect = 255, DWORD stretchMode = 0);
		//Draws sprite data to a texture but discards all rotation transforms and negative scale. - ignores window view transforms!
		void Blt(ws::Texture& dest, DWORD stretchMode = COLORONCOLOR);
	private:
		friend class Window;

		ws::Texture* textureRef;
		int texLeft, texTop;  // Texture coordinates
		int texWidth, texHeight;  // Texture dimensions	
	};
	
	
	//==========LINE=============
	class Line : public ws::Drawable 
	{
	public:
		ws::Vec2i start;
		ws::Vec2i end;
		Gdiplus::Color color = {255,0,0,255};
	    
	    // Constructs a line with endpoints, width, and color.
		Line(ws::Vec2i start = { 0, 0 }, ws::Vec2i end = { 0, 0 }, int thewidth = 2, Gdiplus::Color color = { 255, 0, 0, 255 });
	    // Draws the line in local coordinates.
		virtual void draw(Gdiplus::Graphics* canvas) override;

		// Helper: checks if point q is on segment pr.
		bool onSegment(ws::Vec2i p, ws::Vec2i q, ws::Vec2i r);
		
		// Returns orientation of triplet: 0 = collinear, 1 = clockwise, 2 = counterclockwise.
		int orientation(ws::Vec2i p, ws::Vec2i q, ws::Vec2i r);
		
	    // Returns true if this line segment intersects another.
		bool intersects(Line& otherLine);

	private:
		// Contains always returns false for a line.
		virtual bool contains(ws::Vec2i pos) override;
	};	

	//==============POLY==================
	class Poly : public ws::Drawable 
	{
	public:
	    std::vector<ws::Vec2i> vertices;
	    Poly() = default;
	    
	    // Constructs with vertices, colors, and fill settings.
		Poly(std::vector<ws::Vec2i>& vertices, Gdiplus::Color fillColor = { 255,255,0,0 }, Gdiplus::Color borderColor = { 255,255,0,255 }, int borderWidth = 2, bool filled = true);
	    
	    // Adds a vertex (and a default UV).
		void addVertex(ws::Vec2i vertex);
	    
		void addVertex(int x, int y);
	    
	    // Removes all vertices.
		void clear();
	    
		size_t vertexCount();
	    
	    // Returns true if at least 3 vertices.
		bool isValid();
	    
	    // Returns the centroid of the polygon.
		ws::Vec2i getCentroid();
	    
	    // Point-in-polygon test using ray casting.
		virtual bool contains(ws::Vec2i point) override;
	    
	    // Checks if any edge intersects the given line (or line endpoints inside).
		bool intersects(Line& line);
	    
	    // Checks if this polygon intersects another polygon.
		bool intersects(Poly& other);
	    
	    // Returns the bounding rectangle of all vertices.
		ws::IntRect getBoundingRect();
	    
	    // Associates a texture for filling (triggers effect generation).
		void setTexture(ws::Texture& tex);
	    
	    // Removes texture mapping.
		void removeTexture();
	    
		ws::Texture* getTexture();
	    
	    // Sets the UV coordinates for a vertex.
		void setUV(size_t vertexIndex, float u, float v);
	    
	    // Forces a regeneration of the texture effect next draw.
		void updateTexture();
	    
		void setFillColor(Gdiplus::Color color);
	    
		void setBorderColor(Gdiplus::Color color);
	    
		Gdiplus::Color getFillColor();
	    
		Gdiplus::Color getBorderColor();
	    
		void setBorderWidth(int w);
	    
		int getBorderWidth();
	    
	    void setFilled(bool b = true);
	    
		void setClosed(bool b = true);
	    
		bool getFilled();
	    
		bool getClosed();
	    
	    
	private:
	    
	    // Point-in-polygon test used internally.
		bool pointInPolygon(ws::Vec2i p);
	    
	    // Pre‑computes an effect texture that maps the source texture onto the polygon.
		void generateEffectTexture();
	    
	    // Computes UV coordinates for a point inside the polygon using barycentric interpolation.
		ws::Vec2f getUVForPoint(ws::Vec2i point, ws::IntRect bounds);
	    
	    // Checks if a point is inside a triangle.
		bool pointInTriangle(ws::Vec2i p, ws::Vec2i a, ws::Vec2i b, ws::Vec2i c);
	    
	    // Calculates UV via barycentric coordinates for a point inside triangle.
		ws::Vec2f barycentricUV(ws::Vec2i p, ws::Vec2i a, ws::Vec2i b, ws::Vec2i c,
			ws::Vec2f uvA, ws::Vec2f uvB, ws::Vec2f uvC);
	    
	public:
	    // Draws the polygon, optionally textured.
		virtual void draw(Gdiplus::Graphics* canvas) override;
	private:
		Gdiplus::Color fillColor = { 255,255,0,0 };
		Gdiplus::Color borderColor = { 255,255,0,100 };
		int borderWidth = 2;
		bool filled = true;
		bool closed = true;

		ws::Texture* textureRef = nullptr;
		ws::Texture effectTexture;
		bool textureNeedsUpdate = true;

		std::vector<ws::Vec2f> uvs;
		bool hasUVs = false;
	};	
	
	
	class Rectangle : public ws::Drawable
	{
	public:
		Rectangle();
		Rectangle(int w, int h);
		//Setters
		void setFillColor(ws::Hue color);
		void setBorderColor(ws::Hue color);
		void setBorderWidth(int width);
		void setBorderStyle(Gdiplus::DashStyle style);	
		
		//Getters
		ws::Hue getFillColor() const;
		ws::Hue getBorderColor() const;
		int getBorderWidth() const;
		Gdiplus::DashStyle getBorderStyle() const;
		
		//draw the rectangle in local coordinates
		virtual void draw(Gdiplus::Graphics* canvas) override;
		
	private:
		Gdiplus::Color m_fillColor = Gdiplus::Color(255, 255, 255, 255); // white
		Gdiplus::Color m_borderColor = Gdiplus::Color(255, 0, 0, 0);      // black
		int m_borderWidth = 1;
		Gdiplus::DashStyle m_borderStyle = Gdiplus::DashStyleSolid;		
	};
	
	//===============TEXT==================
	class Text : public ws::Drawable
	{
	public:
		
		// Default constructor.
		Text() = default;
		~Text() = default;
		
		// Constructs with a font.
		Text(ws::Font& newfont);
		
		void setFont(ws::Font& newFont);
		
		ws::Font* getFont();
		
		void setString(std::string str);
		
		std::string getString();
		
		void setCharacterSize(int size);
		
		int getCharacterSize();
		
		void setStyle(Gdiplus::FontStyle fontStyle);
		
		Gdiplus::FontStyle getStyle();
		
		void setFillColor(Gdiplus::Color color);
		
		Gdiplus::Color getFillColor();
		
		void setBorderColor(Gdiplus::Color color);
		
		Gdiplus::Color getBorderColor();
		
		void setBorderWidth(int w);
		
		int getBorderWidth();

		// Contains test delegates to base class.
		virtual bool contains(ws::Vec2i pos) override;
		
		// Draws the text with outline and fill.
		virtual void draw(Gdiplus::Graphics* canvas) override;

		ws::IntRect getLocalBounds() const;
		
		
	private:
		Gdiplus::Color fillColor = Gdiplus::Color(255,0,0,0);	
		Gdiplus::Color borderColor = Gdiplus::Color(255,255,0,0);	
		ws::Font *fontRef = nullptr;	
		std::string text = "";
		int borderWidth = 0;
		int charSize = 12;	
		Gdiplus::FontStyle style = Gdiplus::FontStyleRegular;
	};	
	
	//==============RADIAL==============
	class Radial : public Drawable
	{
	public:
		Poly poly;
		
		// Default radial with 8 points, radius 10.
		Radial();
		
		// Regenerates vertices for the radial shape.
		void make(int points = 8);
		
		// Sets point count and rebuilds.
		void setPointCount(int count);
		
		// Sets radius and rebuilds.
		void setRadius(int size);
		
		void setFillColor(Gdiplus::Color color);
		
		void setBorderColor(Gdiplus::Color color);
		
		void setBorderWidth(int size);

		int getRadius();
		
		int getPointCount();
		
		// Draws via poly.
		virtual void draw(Gdiplus::Graphics* canvas) override;
		
		// Contains test using circle equation.
		virtual bool contains(ws::Vec2i pos) override;
	private:
		int m_points = 500;
		int radius = 10;
	};

	//===============ROUND=============
	class Round : public ws::Drawable {
	public:

		// Draws an ellipse filled and outlined.
		virtual void draw(Gdiplus::Graphics* canvas) override;

		// Integer overload for contains.
		bool contains(int px, int py);

		// Contains test for ellipse (still not fully accurate after rotation).
		virtual bool contains(ws::Vec2i p) override;

		void setBorderColor(Gdiplus::Color color);
		void setFillColor(Gdiplus::Color color);
		void setBorderWidth(int w);
		Gdiplus::Color getBorderColor();
		Gdiplus::Color getFillColor();
		int getBorderWidth();

	private:
		Gdiplus::Color m_borderColor = Gdiplus::Color(255, 100, 200, 100);
		Gdiplus::Color m_fillColor = Gdiplus::Color(255, 50, 150, 50);
		int m_borderWidth = 2;
	};
	
	//============== CURSOR ==============
	class Cursor
	{
	public:
		enum class Type
		{
			// Standard system cursors (IDC_*)
			Arrow,          // IDC_ARROW
			IBeam,          // IDC_IBEAM
			Wait,           // IDC_WAIT
			Cross,          // IDC_CROSS
			UpArrow,        // IDC_UPARROW
			SizeNWSE,       // IDC_SIZENWSE
			SizeNESW,       // IDC_SIZENESW
			SizeWE,         // IDC_SIZEWE
			SizeNS,         // IDC_SIZENS
			SizeAll,        // IDC_SIZEALL
			No,             // IDC_NO
			Hand,           // IDC_HAND
			AppStarting,    // IDC_APPSTARTING
			Help,           // IDC_HELP
			Pin,            // IDC_PIN (Windows 7+)
			Person,         // IDC_PERSON (Windows 8+)

			// OLE drag‑and‑drop cursors (from ole32.dll)
			Copy,           // resource ID 2
			Move,           // resource ID 3
			Link            // resource ID 4
		};
		
		Cursor() = default;
		
		// Loads a system or OLE cursor.
		Cursor(Type type);

		// Destructor destroys the cursor.
		~Cursor();

		// Copy constructor.
		Cursor(const Cursor& other);

		// Copy assignment operator.
		Cursor& operator=(const Cursor& other);

		// Move constructor.
		Cursor(Cursor&& other) noexcept;

		// Move assignment operator.
		Cursor& operator=(Cursor&& other) noexcept;
		
		void loadAs(Type type);

		// Creates a cursor from a Texture.
		bool loadFromTexture(const ws::Texture& texture, int hotSpotX = 0, int hotSpotY = 0);

		// Loads a cursor from a .cur or .ani file.
		bool loadFromFile(const std::string& filename);

		HCURSOR getHandle();
	private:
		HCURSOR handle = nullptr;
		bool animated = false;
		std::wstring srcPath;
	};	

	class Icon
	{
	public:
		Icon() = default;
		//constructor loads from file
		Icon(std::string path);
		//contructor loads from memory
		Icon(const void* buffer, size_t bufferSize);
		//copy constructor that makes this icon the same as another icon - NOT A POINTER.
		Icon(HICON icon);
		~Icon();
		//operator that makes an HICON take on pointer address of this icons HICON.
		operator HICON() const;
		
		//copy operation from HICON replaces this icon.
		Icon& operator=(HICON icon);

		// Copy constructor – copies the icon from another Icon
		Icon(const Icon& other);

		// Copy assignment – replaces current icon with a copy of the other's icon
		Icon& operator=(const Icon& other);

		// Move constructor – steals the handle from other; other becomes empty
		Icon(Icon&& other);

		// Move assignment – replaces current icon with other's, leaves other empty
		Icon& operator=(Icon&& other);
		
		HICON getHandle();
		
		//load the icon from an .ico file.
		bool loadFromFile(std::string path);
		//load the icon from a block of memory.
		bool loadFromMemory(const void* buffer, size_t bufferSize);
		
		bool isValid();
		
		//get a texture copy of the icon at a specific size.
		ws::Texture getTexture(int width, int height);
		
		//get a texture copy of the icon at a specific size. - if you pass in a single number it will be treated as a square size.
		ws::Texture getTexture(DWORD size = ICON_SMALL);
	private:
		HICON hIcon = nullptr;
	};

	//=============== WINDOW ==============

	class WindowManager
	{
		public:
		static std::set<std::wstring> registeredClasses; 
		static std::map<HWND, ws::Window*> windows;
		static std::mutex windowsMutex;
		
		// Registers a window class if not already done.
		static bool registerClass(const std::string& className);
				
		// Global window procedure; routes messages to the correct Window instance.
		static LRESULT CALLBACK GlobalProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
		static void addWindow(ws::Window* window);
		static void removeWindow(HWND hwnd);		
		static Window* GetWindow(HWND hwnd);
		
	};

	class Window
	{
	public:		
		ws::View view;
		std::vector<ws::Child*> children;
		ws::Texture backBuffer;
	    Gdiplus::Graphics* canvas;
		HWND hwnd;
		
		// Default constructor; creates an empty window.
		Window();
		
		// Convenience constructor that calls create().
		Window(int width, int height, std::string title = "", DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0, const std::string& className = "Window");
	
		// Creates the actual window with the given parameters.
		void create(int clientWidth, int clientHeight, std::string title = "", DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0, const std::string& className = "Window");

		// Destructor – destroys the window and cleans up resources.
		~Window();
		
		// Immediately destroys the window.
		void close();
		
		// Processes messages; returns false when closed.
		bool isOpen();
		
		// Pops the next queued message; returns false if queue empty.
		bool pollEvent(MSG& message);
		
		// Clears the back buffer to the given color, recreating if needed.
		void clear(ws::Hue color = ws::Hue::transparent);
		
		// Draws a Drawable object using the current view transform.
		void draw(Drawable& draw);
		
		// Invalidates and updates the window
		void display();
		
		// Sets a pixel on the back buffer.
		void setPixel(int x, int y, ws::Hue hue);
		
		// Gets a pixel from the back buffer.
		ws::Hue getPixel(int x, int y);
		
		// Returns the window title.
		std::string getTitle();
		
		// Sets the window title.
		void setTitle(std::string title);
		
		// Replaces the current view.
		void setView(ws::View& v);
		
		ws::View& getView();
		
		// Shows or hides the window.
		void setVisible(bool val);
		
		bool getVisible();
		
		// Sets focus to this window.
		void setFocus();
		
		bool hasFocus();
		
		// Places this window after another in Z-order.
		void setLayerAfter(HWND lastHwnd);
		
		// Adds a window style.
		void addStyle(DWORD style);
		
		// Removes a window style.
		void removeStyle(DWORD style);
		
		// Sets all window styles at once.
		void setAllStyle(DWORD style);

		// Adds an extended style.
		void addExStyle(DWORD style);
		
		// Removes an extended style.
		void removeExStyle(DWORD style);
		
		// Sets all extended styles.
		void setAllExStyle(DWORD style);

		//returns the extended window styles for this window
		DWORD getExStyle() const;
	    
		//returns the non-extended window styles for this window.
		DWORD getStyle() const;
	    
		//checks if a certain individual style exists for this window.
		bool hasStyle(DWORD checkStyle);
		//checks if a certain individual extended style exists for this window.
		bool hasExStyle(DWORD checkStyle);
		// Sets the window size (client area dimensions). - Overload for ws::Vec2i
		void setSize(ws::Vec2i size);
	    
		// Sets the window size (client area dimensions).
		void setSize(int screenWidth, int screenHeight);
		
		// Returns the client area size.
		ws::Vec2i getSize() const;
		
		//sets the window position - overload
		void setPosition(ws::Vec2i pos);
		
		// Sets the window position.
		void setPosition(int posx, int posy);
		
		// Returns the window position.
		ws::Vec2i getPosition() const;
		
		// Returns the total window rect size (including non-client area).
		ws::Vec2i getTotalSize() const;
		
		// Returns the client area position on screen.
		ws::Vec2i getClientPosition() const;

		// Returns the border width in pixels.
		int getBorderWidth() const;

		// Returns the caption rectangle (title bar area).
		ws::IntRect getCaptionRect(bool excludeBorder = false) const;
		
		// Toggles fullscreen mode.
		void setFullscreen(bool fullscreen = true);
		
		//checks if window is in fullscreen styling.
		bool getFullscreen() const;

		//force all clicks to pass through this window as if it does not exist.
		void disableAnyClicks();

		//Choose what color should represent emptiness and any click will pass through that color as if nothing is there.
		void disableSomeClicks(ws::Hue hue);
		
		// Enables chroma key transparency (legacy per-pixel or simple).
		void enableChromaKey(ws::Hue hue, bool legacy = false);
		
		//disables the chroma keying that causes window transparency. - also disables alpha only.
		void disableChromaKey();
		
		// Enables alpha-only transparency.
		void enableAlphaOnly(float alpha);
		
		//Disables alpha only transparency - also disables chroma key.
		void disableAlphaOnly();
		
		//Sets the window behind the desktop icons. (WARNING -  this will override many window settings and styles to produce a functional desktop window.)
		static bool setBehindIcons(HWND source);
		
		//overload that sets this window behind the icons.
		bool setBehindIcons();
		
		// Converts screen coordinates to world.
		ws::Vec2i toWorld(int x, int y);
		// Converts screen coordinates to world. - overload
		ws::Vec2i toWorld(ws::Vec2i pos);
		
		// Converts world coordinates to screen.
		ws::Vec2i toScreen(int x, int y);
		// Converts world coordinates to screen. - overload
		ws::Vec2i toScreen(ws::Vec2i pos);
		
		//You really shouldn't mess with this.
		// Sets where in texture coordinates the backbuffer is drawn on the window client area. 
		void setSourcePos(ws::Vec2i pos);
		// Sets where in texture coordinates the backbuffer is drawn on the window client area. - overload
		void setSourcePos(int x, int y);
		//returns the position where the backbuffer is drawn to the window client area.
		ws::Vec2i getSourcePos();

		// Default notify stub that just passes to DefWindowProc.
		static LRESULT defaultNotifyStub(Window* window, NMHDR* pnmh, UINT uMsg, WPARAM wParam, LPARAM lParam);

		//Child Management - Declared here but defined in the controls header if included.
		void addChild(ws::Child &child);
		void removeChild(ws::Child &child);
		bool hasChild(ws::Child &child);
		// By default, this points to an empty function. If the child controls header is included, then an alternative definition is provided by the header.
		static inline LRESULT(*s_handleNotifyForChildren)(Window* window, NMHDR* pnmh, UINT uMsg, WPARAM wParam, LPARAM lParam) = &Window::defaultNotifyStub;
		
		// Sets the window cursor.
		void setCursor(ws::Cursor newcursor);
		ws::Cursor getCursor();
		
		//Sets the window icon to a given HICON. - This function is not responsible for cleanup of your HICON handle.
		bool setIcon(HICON icon, DWORD size = ICON_SMALL);
		
		//Sets the window icon to a given .ico file.
		bool setIcon(std::string file, DWORD size = ICON_SMALL);
		
		HICON getIcon();
		
		// Adds a custom message handler.
		void addMessageHandler(std::function<LRESULT(MSG msg)> handler);
		
		std::vector<std::function<LRESULT(MSG msg)>> getMessageHandlers();

	private:
		// Updates the layered window for per‑pixel alpha when enabled.
		void updateLayeredWindow();

		//cleanup dragndrop whether it was used or not.
		void pRevokeDragDrop(HWND& targetHwnd);
		
		
		// Handles messages; called by the global window procedure.
		LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		
		
	private:
		friend class WindowManager;

		bool m_perPixelAlpha = false;

		std::vector<std::function<LRESULT(MSG msg)>> customHandlers;

		bool isRunning = false;

		std::queue<MSG> msgQ;

		ws::Cursor cursor;
		HICON hIcon = nullptr;


		ws::Vec2i srcPos = { 0,0 };
		bool isFullscreen = false;
		RECT windowedRect; // Stores window position/size when not fullscreen
		DWORD windowedStyle; // Stores window style when not fullscreen			

	};
	
	//Window Manager Stuff

	//=========== GLOBAL INPUT ===========

	class Screen
	{
	public:
		
		// Acquires the desktop HDC.
		Screen();
		
		// Releases the desktop HDC.
		~Screen();
		
		HDC getHDC();
		
		// Enumerates all unique display resolutions.
		std::vector<ws::Vec2i> getDisplayModes();
		
		// Changes the display resolution; returns status string.
		std::string setSize(int x, int y);
		
		std::string setSize(ws::Vec2i size);
		
		// Returns the current screen size.
		ws::Vec2i getSize();
		
		// Gets a pixel from the screen.
		ws::Hue getPixel(int x, int y);

		ws::Hue getPixel(ws::Vec2i pos);
		
		// Sets a pixel on the screen.
		void setPixel(int x, int y, ws::Hue hue);
		
		// Captures the screen into a ws::Texture.
		ws::Texture getSnapshot();
		
		// Returns the system DPI scaling factor.
		float getDPI();

		// Returns the work area (excluding taskbar).
		ws::IntRect getWorkArea();
	private:
		HDC hdc = nullptr;
		bool valid = true;
	};

	typedef BOOL (WINAPI *SetDPIAwareProc)(void);
	class GDIPInit
	{
	public:	
		//GDI+	
		Gdiplus::GdiplusStartupInput gdiplusstartup;
		ULONG_PTR gdiplustoken;
		
		int maxControlID = 0;
		
		// Initializes GDI+ and sets DPI awareness.
		GDIPInit();
		
		// Shuts down GDI+.
		~GDIPInit();
	};
	
	//////////////////////////////////////////////////////////////////////////////
	//							  DECLERATIONS
	//////////////////////////////////////////////////////////////////////////////
#ifdef WINSIMPLE_IMPL
	GDIPInit gdipInit;
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
		ws::Vec2i getMousePos(ws::Window& window) {

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

	const std::vector<Keyboard::Key>& Keyboard::GetAllKeys() {
		static const std::vector<Keyboard::Key> keys = {
			// Letters
			A, B, C, D, E, F, G, H, I, J, K, L, M,
			N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
			// Digits
			Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
			// Numpad
			NumPad0, NumPad1, NumPad2, NumPad3, NumPad4,
			NumPad5, NumPad6, NumPad7, NumPad8, NumPad9,
			Multiply, Add, Separator, Subtract, Decimal, Divide,
			// Function keys
			F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
			F11, F12, F13, F14, F15, F16, F17, F18, F19, F20,
			F21, F22, F23, F24,
			// Modifiers
			Shift, Control, Alt,
			LeftShift, RightShift, LeftControl, RightControl, LeftAlt, RightAlt,
			// Windows / Application
			LeftWin, RightWin, Application,
			// Navigation & Editing
			Backspace, Tab, Clear, Enter, Pause, CapsLock, Escape, Space,
			PageUp, PageDown, End, Home, Left, Up, Right, Down,
			Select, Print, Execute, Snapshot, Insert, Delete, Help,
			// Lock keys
			NumLock, ScrollLock,
			// OEM specific
			OemSemicolon, OemPlus, OemComma, OemMinus, OemPeriod,
			OemQuestion, OemTilde, OemOpenBrackets, OemBackslash,
			OemCloseBrackets, OemQuotes, Oem8,
			// Browser / Media
			BrowserBack, BrowserForward, BrowserRefresh, BrowserStop,
			BrowserSearch, BrowserFavorites, BrowserHome,
			VolumeMute, VolumeDown, VolumeUp,
			MediaNextTrack, MediaPrevTrack, MediaStop, MediaPlayPause,
			LaunchMail, LaunchMediaSelect, LaunchApp1, LaunchApp2
		};
		return keys;
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

	Texture::Texture(const std::string& fileName) {
		Texture::loadFromFile(fileName);
	}

	void Texture::destroyDIB() {
		if (bitmap && m_isFast) {
			delete bitmap;
			bitmap = nullptr;
		}

		if (m_hOldBmp && m_hdcMem) {
			SelectObject(m_hdcMem, m_hOldBmp);
			m_hOldBmp = nullptr;
		}

		if (m_hDIB) {
			DeleteObject(m_hDIB);
			m_hDIB = nullptr;
			m_dibBits = nullptr;
		}

		if (m_hdcMem) {
			DeleteDC(m_hdcMem);
			m_hdcMem = nullptr;
		}

		m_isFast = false;
	}

	void Texture::copyFrom(const Texture& other) {
		if (!other.bitmap || other.width <= 0 || other.height <= 0)
			return;

		if (other.m_isFast) {
			if (!create(other.width, other.height))
				return;

			if (m_dibBits && other.m_dibBits) {
				memcpy(m_dibBits, other.m_dibBits, other.width * other.height * 4);
			}
		}
		else {
			bitmap = other.bitmap->Clone(
				0, 0, other.width, other.height,
				PixelFormat32bppARGB
			);
			if (bitmap && bitmap->GetLastStatus() == Gdiplus::Ok) {
				width = other.width;
				height = other.height;
			}
			else {
				delete bitmap;
				bitmap = nullptr;
				width = height = 0;
			}
		}
	}

	Texture::~Texture() {
		destroyDIB();

		//this will only cleanup if destroyDIB hasn't already cleanedup and made bitmap null.
		if (bitmap) {
			delete bitmap;
			bitmap = nullptr;
		}

		width = 0;
		height = 0;
	}

	Texture::Texture(const Texture& other) {
		ZeroMemory(this, sizeof(Texture));
		copyFrom(other);
		scaleMode = other.scaleMode;
	}

	Texture& Texture::operator=(const Texture& other) {
		if (this != &other) {
			destroyDIB();
			if (bitmap) { delete bitmap; bitmap = nullptr; }
			width = 0;
			height = 0;
			copyFrom(other);
			scaleMode = other.scaleMode;
		}
		return *this;
	}

	Texture::Texture(Texture&& other) noexcept {

		memcpy(this, &other, sizeof(Texture));
		// Null out the source so its destructor does nothing
		ZeroMemory(&other, sizeof(Texture));
	}

	// Move assign
	Texture& Texture::operator=(Texture&& other) noexcept {
		if (this != &other) {
			destroyDIB();
			if (bitmap) { delete bitmap; bitmap = nullptr; }

			memcpy(this, &other, sizeof(Texture));

			// null out the source
			ZeroMemory(&other, sizeof(Texture));
		}
		return *this;
	}

	bool Texture::create(int w, int h, Gdiplus::Color color) {
		destroyDIB();
		if (bitmap) { delete bitmap; bitmap = nullptr; }

		width = w;
		height = h;

		BITMAPINFO bmi = {};
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = w;
		bmi.bmiHeader.biHeight = -h;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		HDC hdcScreen = GetDC(nullptr);
		m_hDIB = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &m_dibBits, nullptr, 0);
		ReleaseDC(nullptr, hdcScreen);

		if (!m_hDIB) {
			ws::log("Texture::create failed to create DIBSection");
			return false;
		}

		m_hdcMem = CreateCompatibleDC(nullptr);
		m_hOldBmp = (HBITMAP)SelectObject(m_hdcMem, m_hDIB);

		bitmap = new Gdiplus::Bitmap(w, h, w * 4, PixelFormat32bppARGB, (BYTE*)m_dibBits);

		if (!bitmap || bitmap->GetLastStatus() != Gdiplus::Ok) {
			ws::log("Texture::create failed to create GDI+ wrapper");
			destroyDIB();
			return false;
		}

		Gdiplus::Graphics g(m_hdcMem);
		g.Clear(color);

		m_isFast = true;

		scaleMode = ScaleMode::HighQualityBicubic;
		return true;
	}

	Gdiplus::Bitmap* Texture::getHandle() {
		return bitmap;
	}

	HDC Texture::getHDC() const {
		return m_hdcMem;
	}

	HBITMAP Texture::getDIB() const {
		return m_hDIB;
	}

	HBITMAP Texture::getOldBMP() const {
		return m_hOldBmp;
	}

	void* Texture::getBITS() const {
		return m_dibBits;
	}

	//returns whether this is a fast DIB section under the hood or is a GDI HBITMAP only.
	bool Texture::isFastDIB() const {
		return m_isFast;
	}

	bool Texture::loadFromFile(const std::string& _path) {
		std::string path = _path;
		if (!ResolveRelativePath(path))
			return false;

		// Load into a temporary GDI+ bitmap
		std::wstring wpath = toUTF16(path);
		Gdiplus::Bitmap* temp = Gdiplus::Bitmap::FromFile(wpath.c_str());

		if (!temp || temp->GetLastStatus() != Gdiplus::Ok) {
			ws::log("Failed to load image at (" + path + ")");
			if (temp) { delete temp; }
			return false;
		}

		int w = temp->GetWidth();
		int h = temp->GetHeight();

		// Create DIBSection of the same size
		if (!create(w, h)) {
			delete temp;
			return false;
		}

		// Draw temp into the DIBSection via GDI+
		Gdiplus::Graphics g(m_hdcMem);
		g.DrawImage(temp, 0, 0, w, h);

		delete temp;
		return true;
	}

	bool Texture::loadFromMemory(const void* buffer, size_t bufferSize) {
		// Copy buffer into global memory for IStream
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, bufferSize);
		if (!hGlobal) return false;

		void* pData = GlobalLock(hGlobal);
		if (!pData) { GlobalFree(hGlobal); return false; }
		memcpy(pData, buffer, bufferSize);
		GlobalUnlock(hGlobal);

		IStream* pStream = nullptr;
		HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
		if (FAILED(hr)) { GlobalFree(hGlobal); return false; }

		// Load into a temporary GDI+ bitmap
		Gdiplus::Bitmap* temp = Gdiplus::Bitmap::FromStream(pStream);
		pStream->Release();

		if (!temp || temp->GetLastStatus() != Gdiplus::Ok) {
			if (temp) { delete temp; }
			return false;
		}

		int w = temp->GetWidth();
		int h = temp->GetHeight();

		// Create DIBSection of the same size
		if (!create(w, h)) {
			delete temp;
			return false;
		}

		// Draw temp into the DIBSection via GDI+
		Gdiplus::Graphics g(m_hdcMem);
		g.DrawImage(temp, 0, 0, w, h);

		delete temp;
		return true;
	}

	bool Texture::loadFromBitmapPlus(Gdiplus::Bitmap& src) {
		int w = src.GetWidth();
		int h = src.GetHeight();

		if (w <= 0 || h <= 0) return false;

		// Create DIBSection of the same size
		if (!create(w, h))
			return false;

		// Draw source bitmap into the DIBSection via GDI+
		Gdiplus::Graphics g(m_hdcMem);
		g.DrawImage(&src, 0, 0, w, h);

		return true;
	}

	bool Texture::isValid() const {
		return bitmap != nullptr;
	}



	void Texture::setScaleMode(ScaleMode mode) {
		scaleMode = mode;
	}
	Texture::ScaleMode Texture::getScaleMode() const {
		return scaleMode;
	}

	// Sets a pixel using 1D index (x = index % width, y = index / width).
	void Texture::setPixel(int index, ws::Hue color) {
		int x = index % width;
		int y = index / width;
		setPixel(x, y, color);
	}
	// Sets a pixel at (xIndex,yIndex) with color; fast DIB or GDI+ fallback.
	void Texture::setPixel(int xIndex, int yIndex, ws::Hue color) {
		if (xIndex < 0 || xIndex >= width || yIndex < 0 || yIndex >= height)
			return;
		if (m_isFast && m_dibBits) {
			// direct DIB access. the pixel format is 32bit BGRA
			BYTE* pixel = static_cast<BYTE*>(m_dibBits) + (yIndex * width + xIndex) * 4;
			pixel[0] = static_cast<BYTE>(color.b); // Blue
			pixel[1] = static_cast<BYTE>(color.g); // Green
			pixel[2] = static_cast<BYTE>(color.r); // Red
			pixel[3] = static_cast<BYTE>(color.a); // Alpha
		}
		else if (bitmap) {
			// fallback to GDI+ if this texture does not use the GDI regular DIB method.
			bitmap->SetPixel(xIndex, yIndex, color);
		}
	}

	// Gets a pixel using 1D index.
	ws::Hue Texture::getPixel(int index) {
		int x = index % width;
		int y = index / width;
		return getPixel(x, y);
	}
	// Gets a pixel at (xIndex,yIndex); returns transparent black if out of bounds.
	ws::Hue Texture::getPixel(int xIndex, int yIndex) {
		if (xIndex < 0 || xIndex >= width || yIndex < 0 || yIndex >= height)
			return ws::Hue(0, 0, 0, 0);   // return transparent black
		if (m_isFast && m_dibBits) {
			BYTE* pixel = static_cast<BYTE*>(m_dibBits) + (yIndex * width + xIndex) * 4;
			return ws::Hue(
				pixel[2],   // Red
				pixel[1],   // Green
				pixel[0],   // Blue
				pixel[3]);  // Alpha
		}
		else if (bitmap) {
			//fallback if not a fast DIB.
			Gdiplus::Color color;
			bitmap->GetPixel(xIndex, yIndex, &color);
			return color.GetValue();
		}
		return ws::Hue(0, 0, 0, 0);
	}
	// Returns size as Vec2i.
	ws::Vec2i Texture::getSize() const {
		return ws::Vec2i(width, height);
	}
	// Resizes the texture to (w,h) using high‑quality interpolation.
	void Texture::setSize(int w, int h) {
		setSize({ w,h });
	}
	void Texture::setSize(ws::Vec2i s) {
		if (s.x < 1 || s.y < 1) return;
		Gdiplus::Bitmap scaled(s.x, s.y, PixelFormat32bppARGB);
		Gdiplus::Graphics g(&scaled);
		g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		g.DrawImage(bitmap, 0, 0, s.x, s.y);
		loadFromBitmapPlus(scaled);
	}
	// Scales the texture uniformly by factor s.
	void Texture::setScale(float s) {
		if (s <= 0.0f || !bitmap)
			return;
		int newW = static_cast<int>(width * s);
		int newH = static_cast<int>(height * s);
		if (newW < 1 || newH < 1) return;
		Gdiplus::Bitmap scaled(newW, newH, PixelFormat32bppARGB);
		Gdiplus::Graphics g(&scaled);
		g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		g.DrawImage(bitmap, 0, 0, newW, newH);
		loadFromBitmapPlus(scaled);
	}
	// Saves the texture to a file (format inferred from extension).
	bool Texture::saveToFile(const std::string& _path) {
		std::string path = _path;
		if (!bitmap || width <= 0 || height <= 0) {
			ws::log("Cannot save: Invalid bitmap");
			return false;
		}
		if (!ResolveRelativePath(path)) {
			ws::log("Failed to resolve path (" + path + ")");
			return false;
		}
		// determine the encoder based on the file extension
		CLSID encoderClsid;
		std::string ext = path.substr(path.find_last_of(".") + 1);
		// lowercase extension for compare
		std::transform(ext.begin(), ext.end(), ext.begin(), [](wint_t c) { return static_cast<char>(::towlower(c)); });
		// Get encoder CLSID based on file extension
		if (ext == "png") {
			GetEncoderClsid(L"image/png", &encoderClsid);
		}
		else if (ext == "jpg" || ext == "jpeg") {
			GetEncoderClsid(L"image/jpeg", &encoderClsid);
		}
		else if (ext == "bmp") {
			GetEncoderClsid(L"image/bmp", &encoderClsid);
		}
		else if (ext == "gif") {
			GetEncoderClsid(L"image/gif", &encoderClsid);
		}
		else if (ext == "tiff") {
			GetEncoderClsid(L"image/tiff", &encoderClsid);
		}
		else {
			// default to PNG if extension not recognized
			ws::log("Unsupported format. Using PNG.");
			GetEncoderClsid(L"image/png", &encoderClsid);
			path += ".png"; // Add extension
		}
		// save the image
		Gdiplus::Status status = bitmap->Save(ws::toUTF16(path).c_str(), &encoderClsid, NULL);
		if (status != Gdiplus::Ok) {
			ws::log("Failed to save image to (" + path + ")");
			return false;
		}
		return true;
	}

	//==========FONT===========

	// Initialises with default Arial font.
	Font::Font() {
		family = new Gdiplus::FontFamily(L"Arial");
		fontCollection = new Gdiplus::PrivateFontCollection();
		loadFromSystem("Arial");
	}

	Font::~Font() {
		delete gdiFont;
		delete family;
		delete fontCollection;
		ZeroMemory(this, sizeof(Font));
	}

	Gdiplus::Font* Font::getFontHandle() const {
		return gdiFont;
	}

	Gdiplus::FontFamily* Font::getFamilyHandle() const {
		return family;
	}
	// Returns true if loaded from a file (custom), false if system.
	bool Font::isSystemFont() const {
		return isCustomFont;
	}
	std::string Font::getFilePath() const {
		return fontFilePath;
	}
	std::string Font::getName() const {
		return fontName;
	}
	// Loads a system font by name.
	bool Font::loadFromSystem(const std::string& name) {
		fontFilePath.clear();
		isCustomFont = false;

		delete fontCollection; fontCollection = new Gdiplus::PrivateFontCollection();

		fontName = name;

		delete family;
		family = new Gdiplus::FontFamily(ws::toUTF16(name).c_str());

		if (family->GetLastStatus() != Gdiplus::Ok) {
			delete family; family = new Gdiplus::FontFamily(L"Arial");
			fontName = "Arial";
		}

		return update();
	}

	// Loads a font from a file (TrueType).
	bool Font::loadFromFile(const std::string& path) {
		//reset the font collection to be empty and then add a single font.
		fontFilePath = path;
		delete fontCollection; fontCollection = new Gdiplus::PrivateFontCollection();
		Gdiplus::Status status = fontCollection->AddFontFile(ws::toUTF16(fontFilePath).c_str());

		if (status != Gdiplus::Ok)
			return false;

		//check if the family exists(a family is a font but it is called a family because a font has different versions(bold,italic,etc.))
		int familyCount = fontCollection->GetFamilyCount();

		if (familyCount <= 0)//Means the family failed to be created.
			return false;


		//Get the first font family from the collection. (The only one since this is a collection per ws::Font object now).
		foundFamily = 0;
		fontCollection->GetFamilies(1, family, &foundFamily);


		if (foundFamily == 0)
			return false;

		//Get the overall font name. Ignore subnames like (Arial-Bold,Arial-Italic,etc.)
		wchar_t familyName[LF_FACESIZE]; //LF_FACESIZE == 32

		if (family->GetFamilyName(familyName) != Gdiplus::Ok)
			return false;

		std::string name = ws::toUTF8(familyName);

		//Remove /0 null terminator from name
		if (!name.empty() && name.back() == '\0') {
			name.pop_back();
		}

		//Store the font name
		fontName = name;
		fontFilePath = path;
		isCustomFont = true;

		//initial update
		return update();
	}

	//Load a font from memory.
	bool Font::loadFromMemory(const void* data, size_t size) {
		delete fontCollection; fontCollection = new Gdiplus::PrivateFontCollection();

		Gdiplus::Status status = fontCollection->AddMemoryFont(data, static_cast<UINT>(size));
		if (status != Gdiplus::Ok)
			return false;

		int familyCount = fontCollection->GetFamilyCount();
		if (familyCount <= 0)
			return false;

		foundFamily = 0;
		fontCollection->GetFamilies(1, family, &foundFamily);
		if (foundFamily == 0)
			return false;

		wchar_t familyName[LF_FACESIZE];
		if (family->GetFamilyName(familyName) != Gdiplus::Ok)
			return false;

		std::string name = ws::toUTF8(familyName);
		if (!name.empty() && name.back() == '\0')
			name.pop_back();

		fontName = name;
		fontFilePath.clear();
		isCustomFont = true;

		return update();
	}

	// Returns true if the font object is valid.
	bool Font::isValid() {
		return gdiFont && gdiFont->GetLastStatus() == Gdiplus::Ok;
	}

	// Creates/re‑creates the GDI+ font object from the current family/style/size.
	bool Font::update() {
		if (isCustomFont) {
			if (!fontCollection || fontCollection->GetFamilyCount() <= 0)
				return false;

			if (foundFamily == 0)
				return false;

			//Create the font
			delete gdiFont; gdiFont = new Gdiplus::Font(family, 24, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		}
		else {
			delete gdiFont; gdiFont = new Gdiplus::Font(family,
				24,
				Gdiplus::FontStyleRegular,
				Gdiplus::UnitPixel);
		}
		return gdiFont->GetLastStatus() == Gdiplus::Ok;
	}

	//=============DRAWABLE=============
	ws::Vec2i Drawable::getSize() const { return ws::Vec2i(width, height); }
	ws::Vec2f Drawable::getPosition() const { return ws::Vec2f(x, y); }
	ws::Vec2f Drawable::getScale() const { return scale; }
	ws::Vec2i Drawable::getOrigin() const { return origin; }
	float Drawable::getRotation() const { return rotation; }

	void Drawable::setSize(ws::Vec2i size) { width = size.x; height = size.y; }
	void Drawable::setSize(int w, int h) { width = w; height = h; }
	void Drawable::setPosition(float xpos, float ypos) { x = xpos; y = ypos; }
	void Drawable::setPosition(ws::Vec2f pos) { x = pos.x; y = pos.y; }
	void Drawable::setScale(ws::Vec2f s) { scale = s; }
	void Drawable::setScale(float sx, float sy) { scale.x = sx; scale.y = sy; }
	void Drawable::setOrigin(ws::Vec2i pos) { origin = pos; }
	void Drawable::setOrigin(int posx, int posy) { origin.x = posx; origin.y = posy; }
	void Drawable::setRotation(float degrees) { rotation = degrees; }

	void Drawable::move(float dx, float dy) { x += dx; y += dy; }
	void Drawable::move(const ws::Vec2f& delta) { x += delta.x; y += delta.y; }

	// Returns visual width after scaling (absolute).
	int Drawable::getVisualWidth() const {
		return static_cast<int>(std::abs(width * scale.x));
	}

	// Returns visual height after scaling (absolute).
	int Drawable::getVisualHeight() const {
		return static_cast<int>(std::abs(height * scale.y));
	}

	// Computes world-space bounding rectangle of the drawable (after scale, rotation, origin).
	void Drawable::getBounds(int& left, int& top, int& right, int& bottom) const {

		float corners[4][2] = {
			{static_cast<float>(-origin.x), static_cast<float>(-origin.y)},
			{static_cast<float>(width - origin.x), static_cast<float>(-origin.y)},
			{static_cast<float>(width - origin.x), static_cast<float>(height - origin.y)},
			{static_cast<float>(-origin.x), static_cast<float>(height - origin.y)}
		};

		// Apply scale
		for (int i = 0; i < 4; i++) {
			corners[i][0] *= scale.x;
			corners[i][1] *= scale.y;
		}

		// Apply rotation 
		if (rotation != 0.0f) {
			float rad = static_cast<float>(rotation * M_PI / 180.0);
			float cosA = std::cos(rad);
			float sinA = std::sin(rad);

			for (int i = 0; i < 4; i++) {
				float x = corners[i][0];
				float y = corners[i][1];
				corners[i][0] = x * cosA - y * sinA;
				corners[i][1] = x * sinA + y * cosA;
			}
		}

		// Apply translation to world position and find bounds
		float minX = corners[0][0] + x;
		float maxX = corners[0][0] + x;
		float minY = corners[0][1] + y;
		float maxY = corners[0][1] + y;

		for (int i = 1; i < 4; i++) {
			float wx = corners[i][0] + x;
			float wy = corners[i][1] + y;

			if (wx < minX) minX = wx;
			if (wx > maxX) maxX = wx;
			if (wy < minY) minY = wy;
			if (wy > maxY) maxY = wy;
		}

		left = static_cast<int>(minX);
		top = static_cast<int>(minY);
		right = static_cast<int>(maxX);
		bottom = static_cast<int>(maxY);
	}


	// Returns bounding rectangle as IntRect.
	ws::IntRect Drawable::getBounds() const {
		int left, top, right, bottom;
		getBounds(left, top, right, bottom);
		return ws::IntRect(left, top, right - left, bottom - top);
	}


	// Tests if a world point is inside the visual shape, accounting for rotation.
	bool Drawable::contains(ws::Vec2i point) {
		int left, top, right, bottom;
		getBounds(left, top, right, bottom);

		if (point.x < left || point.x > right || point.y < top || point.y > bottom)
			return false;

		if (rotation == 0.0f) {
			float localX = (static_cast<float>(point.x - x)) / scale.x + static_cast<float>(origin.x);
			float localY = (static_cast<float>(point.y - y)) / scale.y + static_cast<float>(origin.y);
			return (localX >= 0 && localX < width &&
				localY >= 0 && localY < height);
		}

		// For rotated objects, do proper transform
		float localX = static_cast<float>(point.x - x);
		float localY = static_cast<float>(point.y - y);

		// Reverse rotation
		float rad = static_cast<float>(-rotation * M_PI / 180.0);
		float cosA = std::cos(rad);
		float sinA = std::sin(rad);
		float rotX = localX * cosA - localY * sinA;
		float rotY = localX * sinA + localY * cosA;

		// Reverse scale and adjust for origin
		rotX = rotX / scale.x + static_cast<float>(origin.x);
		rotY = rotY / scale.y + static_cast<float>(origin.y);

		return (point.x >= left && point.x <= right && point.y >= top && point.y <= bottom);
	}

	bool Drawable::intersects(ws::IntRect box) {
		int left, top, right, bottom;
		getBounds(left, top, right, bottom);

		int boxLeft = box.left;
		int boxRight = box.left + box.width;
		int boxTop = box.top;
		int boxBottom = box.top + box.height;

		if (boxRight < left || boxLeft > right || boxTop > bottom || boxBottom < top)
			return false;
		return true;
	}

	// Draws the object in world space by applying the local transform matrix.
	void Drawable::drawGlobal(Gdiplus::Graphics* graphics) {
		// Save current state
		Gdiplus::GraphicsState state = graphics->Save();

		Gdiplus::Matrix transform;


		//move to world position.
		transform.Translate(
			static_cast<Gdiplus::REAL>(x),
			static_cast<Gdiplus::REAL>(y)
		);

		//rotate around (0,0) which is now the origin point
		if (rotation != 0.0f) {
			transform.Rotate(rotation);
		}

		//scale in local space
		if (scale.x != 1.0f || scale.y != 1.0f) {
			transform.Scale(scale.x, scale.y);
		}

		transform.Translate(
			static_cast<Gdiplus::REAL>(-origin.x),
			static_cast<Gdiplus::REAL>(-origin.y)
		);


		//graphics->SetTransform(&transform);
		graphics->MultiplyTransform(&transform, Gdiplus::MatrixOrderPrepend);

		// Draw the actual content
		draw(graphics);


		graphics->Restore(state);
	}

	//draws to a texture with full transforms using GDI+ DrawImage. - ignores window view transforms!
	void Drawable::drawToTexture(ws::Texture& target) {
		if (!target.isValid()) return;
		Gdiplus::Graphics graphics(target.getHDC());
		drawGlobal(&graphics);
	}
	//============SPRITE=============

	Sprite::Sprite() {
		ZeroMemory(this, sizeof(Sprite));
	}

	Sprite::Sprite(ws::Texture& texture) {
		ZeroMemory(this, sizeof(Sprite));
		textureRef = &texture;
		setTextureRect({ 0,0,texture.getSize().x,texture.getSize().y });
	}

	// Checks if point lies inside sprite bounds.
	bool Sprite::contains(ws::Vec2i pos) {
		int left, top, right, bottom;
		getBounds(left, top, right, bottom);

		return (pos.x >= left && pos.x < right &&
			pos.y >= top && pos.y < bottom);
	}
	// Draws the sprite texture with the texture's scale mode.
	void Sprite::draw(Gdiplus::Graphics* graphics) {
		if (!textureRef || !textureRef->isValid())
			return;

		Gdiplus::Rect destRect(0, 0, width, height);
		Gdiplus::Rect srcRect(texLeft, texTop, texWidth, texHeight);

		Gdiplus::InterpolationMode oldMode = graphics->GetInterpolationMode();

		switch (textureRef->getScaleMode()) {
		case Texture::ScaleMode::NearestNeighbor:
			graphics->SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
			break;
		case Texture::ScaleMode::Bilinear:
			graphics->SetInterpolationMode(Gdiplus::InterpolationModeBilinear);
			break;
		case Texture::ScaleMode::Bicubic:
			graphics->SetInterpolationMode(Gdiplus::InterpolationModeBicubic);
			break;
		case Texture::ScaleMode::HighQualityBicubic:
			graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
			break;
		}

		graphics->DrawImage(textureRef->bitmap, destRect,
			srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height,
			Gdiplus::UnitPixel);

		graphics->SetInterpolationMode(oldMode);
	}

	// Sets the texture and optionally resizes sprite to texture size.
	void Sprite::setTexture(ws::Texture& texture, bool resize) {
		textureRef = &texture;
		if (resize)
			setTextureRect({ 0,0,texture.getSize().x,texture.getSize().y });
	}

	// Sets the source rectangle from the texture.
	void Sprite::setTextureRect(ws::IntRect rect) {
		// Assuming rect.left, rect.top are coordinates
		// rect.right is width, rect.bottom is height
		texLeft = rect.left;
		texTop = rect.top;
		texWidth = rect.width;
		texHeight = rect.height;

		// Set Drawable dimensions to match texture rectangle
		width = texWidth;
		height = texHeight;
	}

	// Returns the current source rectangle.
	ws::IntRect Sprite::getTextureRect() const {
		return { texLeft, texTop, texWidth, texHeight };
	}

	// Returns reference to the attached texture.
	Texture& Sprite::getTexture() {
		return *textureRef;
	}

	const Texture* Sprite::getTexture() const {
		return textureRef;
	}

	bool Sprite::hasTexture() const {
		return textureRef != nullptr;
	}

	//Draws to a texture using AlphaBlend function -  this does not support complex transforms. - ignores window view transforms!
	void Sprite::drawBlend(ws::Texture& dest, float alphaEffect, DWORD stretchMode) {
		if (!textureRef || !textureRef->isValid()) return;

		if (scale.x < 0 || scale.y < 0) {
			ws::log("Warning! Attempted to drawBlend with negative scale. Defaulting to GDI+ draw for negative scaling support.(This will be slower)");
			drawToTexture(dest);
			return;
		}

		int destX = static_cast<int>(x - origin.x * scale.x);
		int destY = static_cast<int>(y - origin.y * scale.y);
		int destW = static_cast<int>(texWidth * scale.x);
		int destH = static_cast<int>(texHeight * scale.y);

		int srcX = texLeft, srcY = texTop;
		int srcW = texWidth, srcH = texHeight;

		BLENDFUNCTION blend = { AC_SRC_OVER, 0, static_cast<unsigned char>(alphaEffect), AC_SRC_ALPHA };

		static AlphaBlendFunc pAlphaBlend = []() -> AlphaBlendFunc {
			HMODULE hMsimg32 = LoadLibraryW(L"msimg32.dll");
			if (hMsimg32) {
				return (AlphaBlendFunc)GetProcAddress(hMsimg32, "AlphaBlend");
			}
			return nullptr;
			}();

		if (stretchMode != 0)
			SetStretchBltMode(dest.getHDC(), stretchMode);

		if (pAlphaBlend) {
			pAlphaBlend(dest.getHDC(), destX, destY, destW, destH,
				textureRef->getHDC(), srcX, srcY, srcW, srcH,
				blend);
		}
	}
	//Draws sprite data to a texture but discards all rotation transforms and negative scale. - ignores window view transforms!
	void Sprite::Blt(ws::Texture& dest, DWORD stretchMode) {
		if (!textureRef || !textureRef->isValid()) return;

		int destX = static_cast<int>(x - origin.x * scale.x);
		int destY = static_cast<int>(y - origin.y * scale.y);
		int destW = static_cast<int>(texWidth * scale.x);
		int destH = static_cast<int>(texHeight * scale.y);

		int srcX = texLeft, srcY = texTop;
		int srcW = texWidth, srcH = texHeight;

		// (COLORONCOLOR is fastest, HALFTONE gives better quality)
		SetStretchBltMode(dest.getHDC(), stretchMode);

		StretchBlt(
			dest.getHDC(),
			destX, destY, destW, destH,
			textureRef->getHDC(),
			srcX, srcY, srcW, srcH,
			SRCCOPY
		);
	}
	//==========LINE=============
	Line::Line(ws::Vec2i start, ws::Vec2i end, int thewidth, Gdiplus::Color color) {
		this->start = start;
		this->end = end;
		width = thewidth;
		this->color = color;
	}

	// Draws the line in local coordinates.
	void Line::draw(Gdiplus::Graphics* canvas) {
		Gdiplus::Pen pen(color, static_cast<Gdiplus::REAL>(width));
		canvas->DrawLine(&pen, static_cast<Gdiplus::REAL>(start.x), static_cast<Gdiplus::REAL>(start.y), static_cast<Gdiplus::REAL>(end.x), static_cast<Gdiplus::REAL>(end.y));
	}

	bool Line::contains(ws::Vec2i pos) {
		return false;
	}

	// Helper: checks if point q is on segment pr.
	bool Line::onSegment(ws::Vec2i p, ws::Vec2i q, ws::Vec2i r) {
		if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
			q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
			return true;
		return false;
	}

	// Returns orientation of triplet: 0 = collinear, 1 = clockwise, 2 = counterclockwise.
	int Line::orientation(ws::Vec2i p, ws::Vec2i q, ws::Vec2i r) {
		long long val = (long long)(q.y - p.y) * (r.x - q.x) -
			(long long)(q.x - p.x) * (r.y - q.y);

		if (val == 0) return 0;  // Collinear
		return (val > 0) ? 1 : 2; // Clockwise or counterclockwise
	}

	// Returns true if this line segment intersects another.
	bool Line::intersects(Line& otherLine) {
		ws::Vec2i p1 = this->start;
		ws::Vec2i p2 = this->end;
		ws::Vec2i p3 = otherLine.start;
		ws::Vec2i p4 = otherLine.end;

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
	//==============POLY==================
	Poly::Poly(std::vector<ws::Vec2i>& vertices, Gdiplus::Color fillColor, Gdiplus::Color borderColor, int borderWidth, bool filled) {
		this->vertices = vertices;
		this->fillColor = fillColor;
		this->borderColor = borderColor;
		this->borderWidth = borderWidth;
		this->filled = filled;
	}

	void Poly::addVertex(ws::Vec2i vertex) {
		vertices.push_back(vertex);
		uvs.push_back(ws::Vec2f(0, 0));
		textureNeedsUpdate = true;
	}

	void Poly::addVertex(int x, int y) {
		vertices.push_back({ x, y });
		uvs.push_back(ws::Vec2f(0, 0));
		textureNeedsUpdate = true;
	}

	// Removes all vertices.
	void Poly::clear() {
		vertices.clear();
		uvs.clear();
		hasUVs = false;
		textureNeedsUpdate = true;
	}

	size_t Poly::vertexCount() {
		return vertices.size();
	}

	// Returns true if at least 3 vertices.
	bool Poly::isValid() {
		return vertices.size() >= 3;
	}

	// Returns the centroid of the polygon.
	ws::Vec2i Poly::getCentroid() {
		if (vertices.empty()) return { 0, 0 };

		long long sumX = 0, sumY = 0;
		for (const auto& vertex : vertices) {
			sumX += vertex.x;
			sumY += vertex.y;
		}

		return { static_cast<int>(sumX / vertices.size()),
				static_cast<int>(sumY / vertices.size()) };
	}

	// Point-in-polygon test using ray casting.
	bool Poly::contains(ws::Vec2i point) {
		if (vertices.size() < 3) return false;

		int crossings = 0;
		size_t n = vertices.size();

		for (size_t a = 0; a < n; a++) {
			ws::Vec2i p1 = vertices[a];
			ws::Vec2i p2 = vertices[(a + 1) % n];

			if (point.x == p1.x && point.y == p1.y) return true;

			if (p1.y == p2.y && point.y == p1.y &&
				point.x >= std::min(p1.x, p2.x) && point.x <= std::max(p1.x, p2.x)) {
				return true;
			}

			if ((p1.y > point.y) != (p2.y > point.y)) {
				double xIntersection = static_cast<double>(p2.x - p1.x) * static_cast<double>(point.y - p1.y) / static_cast<double>(p2.y - p1.y) + static_cast<double>(p1.x);

				if (point.x <= xIntersection) {
					crossings++;
				}
			}
		}

		return (crossings % 2 == 1);
	}

	// Checks if any edge intersects the given line (or line endpoints inside).
	bool Poly::intersects(Line& line) {
		if (vertices.size() < 2) return false;

		for (size_t i = 0; i < vertices.size(); i++) {
			ws::Vec2i p1 = vertices[i];
			ws::Vec2i p2 = vertices[(i + 1) % vertices.size()];

			Line edge(p1, p2);
			if (edge.intersects(line)) {
				return true;
			}
		}

		if (contains(line.start) || contains(line.end)) {
			return true;
		}

		return false;
	}

	// Checks if this polygon intersects another polygon.
	bool Poly::intersects(Poly& other) {
		for (const auto& vertex : other.vertices) {
			if (contains(vertex)) {
				return true;
			}
		}

		for (const auto& vertex : vertices) {
			if (other.contains(vertex)) {
				return true;
			}
		}

		for (size_t i = 0; i < vertices.size(); i++) {
			ws::Vec2i p1 = vertices[i];
			ws::Vec2i p2 = vertices[(i + 1) % vertices.size()];
			Line edge1(p1, p2);

			for (size_t j = 0; j < other.vertices.size(); j++) {
				ws::Vec2i p3 = other.vertices[j];
				ws::Vec2i p4 = other.vertices[(j + 1) % other.vertices.size()];
				Line edge2(p3, p4);

				if (edge1.intersects(edge2)) {
					return true;
				}
			}
		}

		return false;
	}

	// Returns the bounding rectangle of all vertices.
	ws::IntRect Poly::getBoundingRect() {
		if (vertices.empty()) return { 0, 0, 0, 0 };

		ws::IntRect rect = { vertices[0].x, vertices[0].y, vertices[0].x, vertices[0].y };

		for (const auto& vertex : vertices) {
			rect.left = std::min(rect.left, vertex.x);
			rect.top = std::min(rect.top, vertex.y);
			rect.width = std::max(rect.width, vertex.x);
			rect.height = std::max(rect.height, vertex.y);
		}

		rect.width = rect.width - rect.left;
		rect.height = rect.height - rect.top;

		return rect;
	}

	// Associates a texture for filling (triggers effect generation).
	void Poly::setTexture(ws::Texture& tex) {
		textureRef = &tex;
		textureNeedsUpdate = true;
	}

	// Removes texture mapping.
	void Poly::removeTexture() {
		textureRef = nullptr;
	}

	ws::Texture* Poly::getTexture() {
		return textureRef;
	}

	// Sets the UV coordinates for a vertex.
	void Poly::setUV(size_t vertexIndex, float u, float v) {
		if (vertexIndex >= 0 && vertexIndex < vertices.size()) {
			if (vertexIndex >= uvs.size()) {
				uvs.resize(vertices.size(), ws::Vec2f(0, 0));
			}
			uvs[vertexIndex] = ws::Vec2f(u, v);
			hasUVs = true;
			textureNeedsUpdate = true;
		}
	}

	// Forces a regeneration of the texture effect next draw.
	void Poly::updateTexture() {
		textureNeedsUpdate = true;
	}

	void Poly::setFillColor(Gdiplus::Color color) {
		fillColor = color;
	}

	void Poly::setBorderColor(Gdiplus::Color color) {
		borderColor = color;
	}

	Gdiplus::Color Poly::getFillColor() {
		return fillColor;
	}

	Gdiplus::Color Poly::getBorderColor() {
		return borderColor;
	}


	void Poly::setBorderWidth(int w) {
		borderWidth = w;
	}

	int Poly::getBorderWidth() {
		return borderWidth;
	}

	void Poly::setFilled(bool b) {
		filled = b;
	}

	void Poly::setClosed(bool b) {
		closed = b;
	}

	bool Poly::getFilled() {
		return filled;
	}

	bool Poly::getClosed() {
		return closed;
	}

	// Point-in-polygon test used internally.
	bool Poly::pointInPolygon(ws::Vec2i p) {
		if (vertices.size() < 3) return false;

		int crossings = 0;
		size_t n = vertices.size();

		for (size_t a = 0; a < n; a++) {
			ws::Vec2i p1 = vertices[a];
			ws::Vec2i p2 = vertices[(a + 1) % n];

			if (p.x == p1.x && p.y == p1.y) return true;

			if (p1.y == p2.y && p.y == p1.y &&
				p.x >= std::min(p1.x, p2.x) && p.x <= std::max(p1.x, p2.x)) {
				return true;
			}

			if ((p1.y > p.y) != (p2.y > p.y)) {
				double xIntersection = static_cast<double>(p2.x - p1.x) * static_cast<double>(p.y - p1.y) / static_cast<double>(p2.y - p1.y) + static_cast<double>(p1.x);

				if (p.x <= xIntersection) {
					crossings++;
				}
			}
		}

		return (crossings % 2 == 1);
	}

	// Pre‑computes an effect texture that maps the source texture onto the polygon.
	void Poly::generateEffectTexture() {
		if (!textureRef || !textureRef->isValid()) return;

		ws::IntRect bounds = getBoundingRect();
		if (bounds.width <= 0 || bounds.height <= 0) return;

		effectTexture.create(bounds.width, bounds.height, Gdiplus::Color(0, 0, 0, 0));

		int texWidth = textureRef->getSize().x;
		int texHeight = textureRef->getSize().y;

		if (!hasUVs || uvs.size() != vertices.size()) {
			uvs.clear();
			for (const auto& vertex : vertices) {
				float u = static_cast<float>(vertex.x - bounds.left) / static_cast<float>(bounds.width);
				float v = static_cast<float>(vertex.y - bounds.top) / static_cast<float>(bounds.height);
				uvs.push_back(ws::Vec2f(u, v));
			}
			hasUVs = true;
		}

		for (int y = 0; y < bounds.height; y++) {
			for (int x = 0; x < bounds.width; x++) {
				ws::Vec2i worldPoint(x + bounds.left, y + bounds.top);

				if (pointInPolygon(worldPoint)) {
					ws::Vec2f uv = getUVForPoint(worldPoint, bounds);

					uv.x = std::max(0.0f, std::min(1.0f, uv.x));
					uv.y = std::max(0.0f, std::min(1.0f, uv.y));

					int texX = static_cast<int>(uv.x * static_cast<float>(texWidth - 1));
					int texY = static_cast<int>(uv.y * static_cast<float>(texHeight - 1));

					Gdiplus::Color texColor = textureRef->getPixel(texX, texY);
					effectTexture.setPixel(x, y, texColor);
				}
			}
		}

		textureNeedsUpdate = false;
	}

	// Computes UV coordinates for a point inside the polygon using barycentric interpolation.
	ws::Vec2f Poly::getUVForPoint(ws::Vec2i point, ws::IntRect bounds) {
		if (vertices.size() == 3) {
			return barycentricUV(point, vertices[0], vertices[1], vertices[2],
				uvs[0], uvs[1], uvs[2]);
		}

		for (size_t i = 1; i < vertices.size() - 1; i++) {
			if (pointInTriangle(point, vertices[0], vertices[i], vertices[i + 1])) {
				return barycentricUV(point, vertices[0], vertices[i], vertices[i + 1],
					uvs[0], uvs[i], uvs[i + 1]);
			}
		}

		return ws::Vec2f(0, 0);
	}

	// Checks if a point is inside a triangle.
	bool Poly::pointInTriangle(ws::Vec2i p, ws::Vec2i a, ws::Vec2i b, ws::Vec2i c) {
		float alpha = static_cast<float>(((b.y - c.y) * (p.x - c.x) + (c.x - b.x) * (p.y - c.y))) /
			static_cast<float>(((b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y)));
		float beta = static_cast<float>(((c.y - a.y) * (p.x - c.x) + (a.x - c.x) * (p.y - c.y))) /
			static_cast<float>(((b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y)));
		float gamma = 1.0f - alpha - beta;

		return (alpha >= 0 && beta >= 0 && gamma >= 0);
	}

	// Calculates UV via barycentric coordinates for a point inside triangle.
	ws::Vec2f Poly::barycentricUV(ws::Vec2i p, ws::Vec2i a, ws::Vec2i b, ws::Vec2i c,
		ws::Vec2f uvA, ws::Vec2f uvB, ws::Vec2f uvC) {
		float denom = static_cast<float>((b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y));
		if (fabs(denom) < 0.0001f) return uvA;

		float alpha = static_cast<float>(((b.y - c.y) * (p.x - c.x) + (c.x - b.x) * (p.y - c.y))) / denom;
		float beta = static_cast<float>(((c.y - a.y) * (p.x - c.x) + (a.x - c.x) * (p.y - c.y))) / denom;
		float gamma = 1.0f - alpha - beta;

		float u = alpha * uvA.x + beta * uvB.x + gamma * uvC.x;
		float v = alpha * uvA.y + beta * uvB.y + gamma * uvC.y;

		return ws::Vec2f(u, v);
	}

	void Poly::draw(Gdiplus::Graphics* canvas) {
		if (vertices.size() < 2) return;

		if (!textureRef) {
			std::vector<Gdiplus::PointF> transformedPoints;

			for (const auto& vertex : vertices) {
				transformedPoints.push_back(Gdiplus::PointF(
					static_cast<Gdiplus::REAL>(vertex.x),
					static_cast<Gdiplus::REAL>(vertex.y)
				));
			}

			Gdiplus::Pen borderPen(borderColor, static_cast<Gdiplus::REAL>(borderWidth));
			Gdiplus::SolidBrush fillBrush(fillColor);

			if (filled && closed && vertices.size() >= 3) {
				canvas->FillPolygon(&fillBrush, transformedPoints.data(),
					static_cast<INT>(transformedPoints.size()));
			}

			if (closed && vertices.size() >= 3) {
				canvas->DrawPolygon(&borderPen, transformedPoints.data(),
					static_cast<INT>(transformedPoints.size()));
			}
			else if (vertices.size() >= 2) {
				canvas->DrawLines(&borderPen, transformedPoints.data(),
					static_cast<INT>(transformedPoints.size()));
			}
		}
		else {
			if (textureNeedsUpdate || !effectTexture.isValid()) {
				generateEffectTexture();
			}

			if (effectTexture.isValid()) {
				ws::IntRect bounds = getBoundingRect();

				Gdiplus::TextureBrush textureBrush(effectTexture.bitmap);

				Gdiplus::Matrix transform;
				transform.Translate(static_cast<Gdiplus::REAL>(bounds.left),
					static_cast<Gdiplus::REAL>(bounds.top));
				textureBrush.SetTransform(&transform);

				std::vector<Gdiplus::PointF> transformedPoints;
				for (const auto& vertex : vertices) {
					transformedPoints.push_back(Gdiplus::PointF(
						static_cast<Gdiplus::REAL>(vertex.x),
						static_cast<Gdiplus::REAL>(vertex.y)
					));
				}

				canvas->FillPolygon(&textureBrush, transformedPoints.data(),
					static_cast<INT>(transformedPoints.size()));

				if (borderWidth > 0) {
					Gdiplus::Pen borderPen(borderColor, static_cast<Gdiplus::REAL>(borderWidth));
					canvas->DrawPolygon(&borderPen, transformedPoints.data(),
						static_cast<INT>(transformedPoints.size()));
				}
			}
		}
	}
	//=====RECTANGLE======
	Rectangle::Rectangle() {
		width = 1;
		height = 1;
	}
	Rectangle::Rectangle(int w, int h) {
		setSize(w, h);
	}

	//Setters
	void Rectangle::setFillColor(ws::Hue color) { m_fillColor = color; }
	void Rectangle::setBorderColor(ws::Hue color) { m_borderColor = color; }
	void Rectangle::setBorderWidth(int width) { m_borderWidth = width; }
	void Rectangle::setBorderStyle(Gdiplus::DashStyle style) { m_borderStyle = style; }

	//Getters
	ws::Hue Rectangle::getFillColor() const { return m_fillColor.GetValue(); }
	ws::Hue Rectangle::getBorderColor() const { return m_borderColor.GetValue(); }
	int Rectangle::getBorderWidth() const { return m_borderWidth; }
	Gdiplus::DashStyle Rectangle::getBorderStyle() const { return m_borderStyle; }

	//draw the rectangle in local coordinates
	void Rectangle::draw(Gdiplus::Graphics* canvas) {
		Gdiplus::RectF rect(0.0f, 0.0f,
			static_cast<Gdiplus::REAL>(width),
			static_cast<Gdiplus::REAL>(height));

		if (m_fillColor.GetA() > 0) {
			Gdiplus::SolidBrush fillBrush(m_fillColor);
			canvas->FillRectangle(&fillBrush, rect);
		}

		if (m_borderWidth > 0 && m_borderColor.GetA() > 0) {
			Gdiplus::Pen borderPen(m_borderColor,
				static_cast<Gdiplus::REAL>(m_borderWidth));
			borderPen.SetDashStyle(m_borderStyle);
			canvas->DrawRectangle(&borderPen, rect);
		}
	}
	//===============TEXT==================

	Text::Text(ws::Font& newfont) {
		fontRef = &newfont;
	}

	void Text::setFont(ws::Font& newFont) {
		fontRef = &newFont;
		getLocalBounds();
	}

	ws::Font* Text::getFont() { return fontRef; }

	void Text::setString(std::string str) {
		text = str;
		getLocalBounds();
	}

	std::string Text::getString() { return text; }

	void Text::setCharacterSize(int size) {
		charSize = size;
		getLocalBounds();
	}

	int Text::getCharacterSize() { return charSize; }

	void Text::setStyle(Gdiplus::FontStyle fontStyle) {
		style = fontStyle;
		getLocalBounds();
	}

	Gdiplus::FontStyle Text::getStyle() { return style; }

	void Text::setFillColor(Gdiplus::Color color) { fillColor = color; }

	Gdiplus::Color Text::getFillColor() { return fillColor; }

	void Text::setBorderColor(Gdiplus::Color color) { borderColor = color; }

	Gdiplus::Color Text::getBorderColor() { return borderColor; }

	void Text::setBorderWidth(int w) {
		borderWidth = w;
		getLocalBounds();
	}

	int Text::getBorderWidth() { return borderWidth; }

	// Contains test delegates to base class.
	bool Text::contains(ws::Vec2i pos) {
		return Drawable::contains(pos);
	}

	// Draws the text with outline and fill.
	void Text::draw(Gdiplus::Graphics* canvas) {
		if (!fontRef)
			return;
		Gdiplus::FontFamily* familyPtr = fontRef->getFamilyHandle();
		if (!familyPtr) return;
		Gdiplus::FontFamily& family = *familyPtr;

		if (!family.IsStyleAvailable(style)) {
			ws::log("Font style not available! Defaulting to whatever style can be found. If nothing is found, the text will not be displayed.");
			if (family.IsStyleAvailable(Gdiplus::FontStyleRegular))
				style = Gdiplus::FontStyleRegular;
			else if (family.IsStyleAvailable(Gdiplus::FontStyleBold))
				style = Gdiplus::FontStyleBold;
			else if (family.IsStyleAvailable(Gdiplus::FontStyleItalic))
				style = Gdiplus::FontStyleItalic;
			else
				return;
		}

		Gdiplus::GraphicsPath path;

		Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
		format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsNoFitBlackBox | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);

		path.AddString(
			ws::toUTF16(text).c_str(),
			static_cast<INT>(text.length()),
			fontRef->getFamilyHandle(),
			style,
			static_cast<Gdiplus::REAL>(charSize),
			Gdiplus::PointF(0, 0),
			&format
		);

		Gdiplus::Pen outlinePen(borderColor, static_cast<Gdiplus::REAL>(borderWidth));
		outlinePen.SetLineJoin(Gdiplus::LineJoinRound);

		Gdiplus::RectF bounds;
		path.GetBounds(&bounds, NULL, &outlinePen);

		width = static_cast<int>(bounds.Width);
		height = static_cast<int>(bounds.Height);

		// Update the offset so that Drawable draws the text at the proper position.
		Gdiplus::Matrix originalTransform;
		canvas->GetTransform(&originalTransform);
		canvas->TranslateTransform(-bounds.X, -bounds.Y);

		Gdiplus::SolidBrush fillBrush(fillColor);

		// Draw the outline
		if (borderWidth > 0)
			canvas->DrawPath(&outlinePen, &path);

		// Fill the text
		canvas->FillPath(&fillBrush, &path);
	}

	ws::IntRect Text::getLocalBounds() const {
		if (!fontRef) return { 0,0,0,0 };

		Gdiplus::GraphicsPath path;
		Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
		format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsNoFitBlackBox | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);

		path.AddString(
			ws::toUTF16(text).c_str(),
			static_cast<INT>(text.length()),
			fontRef->getFamilyHandle(),
			style,
			static_cast<Gdiplus::REAL>(charSize),
			Gdiplus::PointF(0, 0),
			&format
		);

		Gdiplus::RectF bounds;
		path.GetBounds(&bounds, NULL, NULL);

		// Update the drawable's width and height
		const_cast<ws::Text*>(this)->width = static_cast<int>(bounds.Width);
		const_cast<ws::Text*>(this)->height = static_cast<int>(bounds.Height);

		return ws::IntRect(
			static_cast<int>(bounds.X),
			static_cast<int>(bounds.Y),
			static_cast<int>(bounds.Width),
			static_cast<int>(bounds.Height)
		);
	}
	//==============RADIAL==============

	// Default radial with 8 points, radius 10.
	Radial::Radial() {
		poly.setFillColor(Gdiplus::Color(255, 100, 200, 100));
		poly.setBorderColor(Gdiplus::Color(255, 50, 255, 50));
		poly.setBorderWidth(2);
		poly.setClosed();
		poly.setFilled();
		make();
		origin.x = 0;
		origin.y = 0;
	}

	// Regenerates vertices for the radial shape.
	void Radial::make(int points) {
		poly.clear();

		double inc = (2 * M_PI) / points;

		for (double a = 0; a < (2 * M_PI); a += inc) {
			double angle = a;
			int resx = static_cast<int>(std::cos(angle) * static_cast<double>(radius));
			int resy = static_cast<int>(std::sin(angle) * static_cast<double>(radius));
			poly.addVertex(resx, resy);
		}
		m_points = points;

		// Update Drawable properties
		width = 2 * radius;
		height = 2 * radius;
	}

	// Sets point count and rebuilds.
	void Radial::setPointCount(int count) {
		m_points = count;
		make(m_points);
	}

	// Sets radius and rebuilds.
	void Radial::setRadius(int size) {
		radius = size;
		make(m_points);
	}

	void Radial::setFillColor(Gdiplus::Color color) {
		poly.setFillColor(color);
	}

	void Radial::setBorderColor(Gdiplus::Color color) {
		poly.setBorderColor(color);
	}

	void Radial::setBorderWidth(int size) {
		poly.setBorderWidth(size);
	}

	int Radial::getRadius() {
		return radius;
	}

	int Radial::getPointCount() {
		return m_points;
	}

	// Draws via poly.
	void Radial::draw(Gdiplus::Graphics* canvas) {
		poly.draw(canvas);
	}

	// Contains test using circle equation.
	bool Radial::contains(ws::Vec2i pos) {
		// Convert to local coordinates
		float localX = static_cast<float>(pos.x - x - origin.x);
		float localY = static_cast<float>(pos.y - y - origin.y);

		// Reverse scale
		if (scale.x != 1.0f) localX /= scale.x;
		if (scale.y != 1.0f) localY /= scale.y;

		// Check if point is within circle
		return (localX * localX + localY * localY) <= static_cast<float>(radius * radius);
	}

	//===============ROUND=============
	// Draws an ellipse filled and outlined.
	void Round::draw(Gdiplus::Graphics* canvas) {
		Gdiplus::Pen borderPen(m_borderColor, static_cast<Gdiplus::REAL>(m_borderWidth));
		Gdiplus::SolidBrush fillBrush(m_fillColor);

		canvas->DrawEllipse(&borderPen, 0.0f, 0.0f, static_cast<Gdiplus::REAL>(width), static_cast<Gdiplus::REAL>(height));
		canvas->FillEllipse(&fillBrush, 0.0f, 0.0f, static_cast<Gdiplus::REAL>(width), static_cast<Gdiplus::REAL>(height));
	}

	// Integer overload for contains.
	bool Round::contains(int px, int py) {
		return contains(ws::Vec2i(px, py));
	}

	// Contains test for ellipse (still not fully accurate after rotation).
	bool Round::contains(ws::Vec2i p) {

		// Convert to local coordinates
		float localX = static_cast<float>(p.x - x);
		float localY = static_cast<float>(p.y - y);

		// Reverse scale
		if (scale.x != 1.0f) localX /= scale.x;
		if (scale.y != 1.0f) localY /= scale.y;

		// Adjust for origin
		localX += static_cast<float>(origin.x);
		localY += static_cast<float>(origin.y);

		// Ellipse equation check
		float centerX = static_cast<float>(width) / 2.0f;
		float centerY = static_cast<float>(height) / 2.0f;
		float radiusX = static_cast<float>(width) / 2.0f;
		float radiusY = static_cast<float>(height) / 2.0f;

		if (radiusX <= 0 || radiusY <= 0) return false;

		float normalizedX = (localX - centerX) / radiusX;
		float normalizedY = (localY - centerY) / radiusY;

		return (normalizedX * normalizedX + normalizedY * normalizedY) <= 1.0f;
	}

	void Round::setBorderColor(Gdiplus::Color color) { m_borderColor = color; }
	void Round::setFillColor(Gdiplus::Color color) { m_fillColor = color; }
	void Round::setBorderWidth(int w) { m_borderWidth = w; }
	Gdiplus::Color Round::getBorderColor() { return m_borderColor; }
	Gdiplus::Color Round::getFillColor() { return m_fillColor; }
	int Round::getBorderWidth() { return m_borderWidth; }

	//============== CURSOR ==============
	Cursor::Cursor(Type type) {
		loadAs(type);
	}

	// Destructor destroys the cursor.
	Cursor::~Cursor() {
		if (handle)
			DestroyCursor(handle);
	}

	// Copy constructor.
	Cursor::Cursor(const Cursor& other) : animated(other.animated), srcPath(other.srcPath) {
		if (other.handle) {
			if (other.animated && !other.srcPath.empty()) {
				// CopyIcon loses animation frames, reload from file instead
				handle = LoadCursorFromFileW(other.srcPath.c_str());
				if (!handle)
					handle = (HCURSOR)CopyIcon((HICON)other.handle); // fallback just in case
			}
			else
				handle = (HCURSOR)CopyIcon((HICON)other.handle);
		}
	}

	// Copy assignment operator.
	Cursor& Cursor::operator=(const Cursor& other) {
		if (this != &other) {
			if (handle) DestroyCursor(handle);
			animated = other.animated;
			srcPath = other.srcPath;
			if (other.handle) {
				if (other.animated && !other.srcPath.empty()) {
					handle = LoadCursorFromFileW(other.srcPath.c_str());
					if (!handle)
						handle = (HCURSOR)CopyIcon((HICON)other.handle);
				}
				else
					handle = (HCURSOR)CopyIcon((HICON)other.handle);
			}
			else
				handle = nullptr;
		}
		return *this;
	}

	// Move constructor.
	Cursor::Cursor(Cursor&& other) noexcept
		: handle(other.handle), animated(other.animated), srcPath(std::move(other.srcPath)) {
		other.handle = nullptr;
		other.animated = false;
	}

	// Move assignment operator.
	Cursor& Cursor::operator=(Cursor&& other) noexcept {
		if (this != &other) {
			if (handle) DestroyCursor(handle);
			handle = other.handle;
			animated = other.animated;
			srcPath = std::move(other.srcPath);
			other.handle = nullptr;
			other.animated = false;
		}
		return *this;
	}

	HCURSOR Cursor::getHandle() {
		return handle;
	}



	// Loads a system or OLE cursor.

	void Cursor::loadAs(Type type) {
		animated = false;
		srcPath.clear();
		if (handle)
			DestroyCursor(handle);

		switch (type) {
		case Type::Arrow:        handle = LoadCursor(nullptr, IDC_ARROW); break;
		case Type::IBeam:        handle = LoadCursor(nullptr, IDC_IBEAM); break;
		case Type::Wait:         handle = LoadCursor(nullptr, IDC_WAIT); break;
		case Type::Cross:        handle = LoadCursor(nullptr, IDC_CROSS); break;
		case Type::UpArrow:      handle = LoadCursor(nullptr, IDC_UPARROW); break;
		case Type::SizeNWSE:     handle = LoadCursor(nullptr, IDC_SIZENWSE); break;
		case Type::SizeNESW:     handle = LoadCursor(nullptr, IDC_SIZENESW); break;
		case Type::SizeWE:       handle = LoadCursor(nullptr, IDC_SIZEWE); break;
		case Type::SizeNS:       handle = LoadCursor(nullptr, IDC_SIZENS); break;
		case Type::SizeAll:      handle = LoadCursor(nullptr, IDC_SIZEALL); break;
		case Type::No:           handle = LoadCursor(nullptr, IDC_NO); break;
		case Type::Hand:         handle = LoadCursor(nullptr, IDC_HAND); break;
		case Type::AppStarting:  handle = LoadCursor(nullptr, IDC_APPSTARTING); break;
		case Type::Help:         handle = LoadCursor(nullptr, IDC_HELP); break;
		case Type::Pin:          handle = LoadCursor(nullptr, IDC_PIN); break;
		case Type::Person:       handle = LoadCursor(nullptr, IDC_PERSON); break;

			// OLE drag‑and‑drop cursors – requires loading from ole32.dll
		case Type::Copy:
		{
			HMODULE ole = GetModuleHandle(L"ole32.dll");
			if (ole)
				handle = (HCURSOR)LoadImage(ole, MAKEINTRESOURCE(2), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
			if (!handle)
				handle = LoadCursor(nullptr, IDC_HAND);  // fallback
			break;
		}
		case Type::Move:
		{
			HMODULE ole = GetModuleHandle(L"ole32.dll");
			if (ole)
				handle = (HCURSOR)LoadImage(ole, MAKEINTRESOURCE(3), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
			if (!handle)
				handle = LoadCursor(nullptr, IDC_ARROW);  // fallback
			break;
		}
		case Type::Link:
		{
			HMODULE ole = GetModuleHandle(L"ole32.dll");
			if (ole)
				handle = (HCURSOR)LoadImage(ole, MAKEINTRESOURCE(4), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
			if (!handle)
				handle = LoadCursor(nullptr, IDC_HAND);   // fallback
			break;
		}
		}
	}

	// Creates a cursor from a Texture.
	bool Cursor::loadFromTexture(const ws::Texture& texture, int hotSpotX, int hotSpotY) {
		animated = false;
		srcPath.clear();
		if (!texture.isValid()) return false;

		Gdiplus::Bitmap* gdipBitmap = texture.bitmap;
		UINT width = gdipBitmap->GetWidth();
		UINT height = gdipBitmap->GetHeight();

		// <><><> 32‑bpp colour DIB section 
		BITMAPINFO bmi = {};
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -(LONG)height;          // top‑down
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		void* colourBits = nullptr;
		HBITMAP hbmColor = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &colourBits, nullptr, 0);
		if (!hbmColor) return false;

		Gdiplus::BitmapData data;
		Gdiplus::Rect rect(0, 0, width, height);
		if (gdipBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data) != Gdiplus::Ok) {
			DeleteObject(hbmColor);
			return false;
		}
		memcpy(colourBits, data.Scan0, width * height * 4);
		gdipBitmap->UnlockBits(&data);

		// <><><> 1‑bpp mask DIB section
		size_t maskStride = ((width + 31) / 32) * 4;               // DWORD‑aligned stride in bytes
		size_t maskBufferSize = maskStride * height;
		std::vector<BYTE> maskBits(maskBufferSize, 0xFF);          // all bits = 1 (opaque mask)

		BITMAPINFO maskBmi = {};
		maskBmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		maskBmi.bmiHeader.biWidth = width;
		maskBmi.bmiHeader.biHeight = -(LONG)height;
		maskBmi.bmiHeader.biPlanes = 1;
		maskBmi.bmiHeader.biBitCount = 1;
		maskBmi.bmiHeader.biCompression = BI_RGB;
		maskBmi.bmiHeader.biSizeImage = (DWORD)maskBufferSize;

		void* maskBitsPtr = nullptr;
		HBITMAP hbmMask = CreateDIBSection(nullptr, &maskBmi, DIB_RGB_COLORS, &maskBitsPtr, nullptr, 0);
		if (!hbmMask) {
			DeleteObject(hbmColor);
			return false;
		}
		memcpy(maskBitsPtr, maskBits.data(), maskBufferSize);

		//<><> Create the cursor
		ICONINFO iconInfo = {};
		iconInfo.fIcon = FALSE;          // cursor, not icon
		iconInfo.xHotspot = hotSpotX;
		iconInfo.yHotspot = hotSpotY;
		iconInfo.hbmMask = hbmMask;
		iconInfo.hbmColor = hbmColor;

		HCURSOR newCursor = CreateIconIndirect(&iconInfo);

		// Clean up the temporary bitmaps
		DeleteObject(hbmColor);
		DeleteObject(hbmMask);

		if (newCursor) {
			if (handle) DestroyCursor(handle);
			handle = newCursor;
		}
		else
			return false;
		return true;
	}

	// Loads a cursor from a .cur or .ani file.
	bool Cursor::loadFromFile(const std::string& filename) {
		animated = false;
		srcPath.clear();
		if (handle) {
			DestroyCursor(handle);
			handle = nullptr;
		}

		std::wstring wfilename = ws::toUTF16(filename);

		// just detect it from the extension, no reason to make the caller do this
		bool isAni = wfilename.size() >= 4 && _wcsicmp(wfilename.c_str() + wfilename.size() - 4, L".ani") == 0;

		if (isAni)
			handle = LoadCursorFromFileW(wfilename.c_str());
		else {
			handle = (HCURSOR)LoadImageW(
				nullptr,
				wfilename.c_str(),
				IMAGE_CURSOR,
				0, 0,
				LR_LOADFROMFILE | LR_DEFAULTSIZE
			);
		}
		if (!handle) {
			ws::log("Failed to load cursor from file (" + filename + ")");
			animated = false;
			srcPath.clear();
			return false;
		}

		animated = isAni;
		srcPath = isAni ? wfilename : std::wstring{};
		return true;
	}
	//ICON
	Icon::Icon(std::string path) {
		loadFromFile(path);
	}
	//contructor loads from memory
	Icon::Icon(const void* buffer, size_t bufferSize) {
		loadFromMemory(buffer, bufferSize);
	}
	//copy constructor that makes this icon the same as another icon - NOT A POINTER.
	Icon::Icon(HICON icon) : hIcon(nullptr) {
		if (icon)
			hIcon = CopyIcon(icon);
	}
	Icon::~Icon() {
		if (hIcon) {
			DestroyIcon(hIcon);
			hIcon = nullptr;
		}
	}
	//operator that makes an HICON take on pointer address of this icons HICON.
	Icon::operator HICON() const { return hIcon; }

	//copy operation from HICON replaces this icon.
	Icon& Icon::operator=(HICON icon) {
		if (hIcon) {
			DestroyIcon(hIcon);
			hIcon = nullptr;
		}
		if (icon)
			hIcon = CopyIcon(icon);
		return *this;
	}

	// Copy constructor – copies the icon from another Icon
	Icon::Icon(const Icon& other) : hIcon(nullptr) {
		if (other.hIcon)
			hIcon = CopyIcon(other.hIcon);
	}

	// Copy assignment – replaces current icon with a copy of the other's icon
	Icon& Icon::operator=(const Icon& other) {
		if (this != &other) {
			if (hIcon) {
				DestroyIcon(hIcon);
				hIcon = nullptr;
			}
			if (other.hIcon)
				hIcon = CopyIcon(other.hIcon);
		}
		return *this;
	}

	//// Move constructor – steals the handle from other; other becomes empty
	//Icon::Icon(Icon&& other) noexcept : hIcon(other.hIcon) {
	//	other.hIcon = nullptr;
	//}

	// Move assignment – replaces current icon with other's, leaves other empty
	//Icon& Icon::operator=(Icon&& other) noexcept {
	//	if (this != &other) {
	//		if (hIcon) {
	//			DestroyIcon(hIcon);
	//			hIcon = nullptr;
	//		}
	//		// Steal the handle
	//		hIcon = other.hIcon;
	//		other.hIcon = nullptr;
	//	}
	//	return *this;
	//}

	HICON Icon::getHandle() {
		return hIcon;
	}

	//load the icon from an .ico file.
	bool Icon::loadFromFile(std::string path) {
		if (hIcon) {
			DestroyIcon(hIcon);
			hIcon = nullptr;
		}

		hIcon = (HICON)LoadImageA(
			NULL,
			path.c_str(),
			IMAGE_ICON,
			0,
			0,
			LR_LOADFROMFILE | LR_SHARED
		);
		if (!hIcon)
			return false;
		return true;
	}
	//load the icon from a block of memory.
	bool Icon::loadFromMemory(const void* buffer, size_t bufferSize) {
		if (hIcon) {
			DestroyIcon(hIcon);
			hIcon = nullptr;
		}


		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, bufferSize);
		if (!hGlobal)
			return false;

		void* pData = GlobalLock(hGlobal);
		if (!pData) {
			GlobalFree(hGlobal);
			return false;
		}
		memcpy(pData, buffer, bufferSize);
		GlobalUnlock(hGlobal);

		IStream* pStream = nullptr;
		HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream); // TRUE = free on release
		if (FAILED(hr)) {
			GlobalFree(hGlobal);
			return false;
		}


		Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromStream(pStream);
		if (!pBitmap || pBitmap->GetLastStatus() != Gdiplus::Ok)
			return false;

		pBitmap->GetHICON(&hIcon);
		delete pBitmap;

		pStream->Release();

		if (!hIcon)
			return false;
		return true;
	}

	bool Icon::isValid() {
		if (!hIcon)
			return false;
		return true;
	}

	//get a texture copy of the icon at a specific size.
	ws::Texture Icon::getTexture(int width, int height) {
		ws::Texture tex;
		if (!tex.create(width, height))
			return tex;
		HDC hdc = tex.getHDC();
		if (!hdc)
			return tex;
		BOOL result = DrawIconEx(
			hdc,
			0, 0,
			hIcon,
			width, height,
			0,
			NULL,
			DI_NORMAL
		);

		if (!result)
			return ws::Texture();
		return tex;
	}


	//get a texture copy of the icon at a specific size. - if you pass in a single number it will be treated as a square size.
	ws::Texture Icon::getTexture(DWORD size) {
		if (!isValid())
			return ws::Texture();
		int width, height;
		if (size == ICON_SMALL) {
			width = GetSystemMetrics(SM_CXSMICON);
			height = GetSystemMetrics(SM_CYSMICON);
		}
		else if (size == ICON_BIG) {
			width = GetSystemMetrics(SM_CXICON);
			height = GetSystemMetrics(SM_CYICON);
		}
		else
			width = height = static_cast<int>(size);

		return getTexture(width, height);
	}
	//=============== WINDOW ==============
	std::set<std::wstring> ws::WindowManager::registeredClasses;
	std::map<HWND, ws::Window*> ws::WindowManager::windows;
	std::mutex ws::WindowManager::windowsMutex;

	// Implementation of registerClass.
	bool ws::WindowManager::registerClass(const std::string& className) {
		std::wstring wclassName = ws::toUTF16(className);

		// Check if already registered
		if (registeredClasses.find(wclassName) != registeredClasses.end())
			return true;   // already exists, nothing to do

		HINSTANCE instance = GetModuleHandle(nullptr);
		WNDCLASS wc = {};
		wc.lpfnWndProc = WindowManager::GlobalProc;
		wc.hInstance = instance;
		wc.lpszClassName = wclassName.c_str();
		wc.hCursor = NULL;
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

		if (!RegisterClass(&wc)) {
			ws::log("Failed to register window class " + className);
			return false;
		}

		registeredClasses.insert(wclassName);
		return true;
	}



	// Implementation of GlobalProc.
	LRESULT CALLBACK ws::WindowManager::GlobalProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		//std::lock_guard<std::mutex> lock(windowsMutex);

		if (msg == WM_NCCREATE) {
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			ws::Window* pWindow = reinterpret_cast<ws::Window*>(pCreate->lpCreateParams);

			pWindow->hwnd = hwnd;

			windows[hwnd] = pWindow;

			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));

			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		ws::Window* pWindow = reinterpret_cast<ws::Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		if (pWindow) {
			return pWindow->handleMessage(msg, wParam, lParam);
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	// Implementation of addWindow.
	void ws::WindowManager::addWindow(ws::Window* window) {
		//std::lock_guard<std::mutex> lock(windowsMutex);
		windows[window->hwnd] = window;
	}
	void ws::WindowManager::removeWindow(HWND hwnd) {
		//std::lock_guard<std::mutex> lock(windowsMutex);
		windows.erase(hwnd);//hwnd is a pointer to a location. Therefore, it may be used to point to my ws::Window.
	}

	// Implementation of GetWindow.
	ws::Window* ws::WindowManager::GetWindow(HWND hwnd) {
		//std::lock_guard<std::mutex> lock(windowsMutex);
		auto it = windows.find(hwnd);
		if (it != windows.end()) {
			return it->second;
		}
		return nullptr;
	}



	// Default constructor; creates an empty window.
	Window::Window() {
		canvas = nullptr;
		hwnd = nullptr;
	}

	// Convenience constructor that calls create().
	Window::Window(int width, int height, std::string title, DWORD style, DWORD exStyle, const std::string& className) {
		create(width, height, title, style, exStyle, className);
	}

	// Creates the actual window with the given parameters.
	void Window::create(int clientWidth, int clientHeight, std::string title, DWORD style, DWORD exStyle, const std::string& className) {
		if (clientWidth <= 0 || clientHeight <= 0) {
			ws::log("Error: Attempted to create a window with an invalid size!");
		}

		if (hwnd && IsWindow(hwnd)) {
			DestroyWindow(hwnd);
			// force processing of the destruction message
			MSG msg;
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			hwnd = nullptr;
		}

		if (!WindowManager::registerClass(className))
			return;

		if (style == -1)
			style = WS_OVERLAPPEDWINDOW;
		if (exStyle == -1)
			exStyle = 0;

		//Note to self: the style must be set manually this way because hwnd has not been initialized yet!
		style |= WS_CLIPCHILDREN;

		view.init({ 0,0,clientWidth,clientHeight });

		RECT rect = { 0, 0, clientWidth, clientHeight };
		AdjustWindowRectEx(&rect, style, FALSE, exStyle);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		hwnd = CreateWindowEx(
			exStyle,
			ws::toUTF16(className).c_str(),
			ws::toUTF16(title).c_str(),
			style,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			width,
			height,
			nullptr,
			nullptr,
			GetModuleHandle(nullptr),
			this
		);


		if (hwnd == nullptr) {
			ws::log("Failed to create window!");
			exit(-1);
		}

		backBuffer.create(view.getSize().x, view.getSize().y);
		canvas = new Gdiplus::Graphics(backBuffer.getHDC());

		isRunning = true;
		setVisible(true);
		UpdateWindow(hwnd);
		setFocus();

	}

	// Destructor – destroys the window and cleans up resources.
	Window::~Window() {
		if (hwnd && IsWindow(hwnd)) {
			pRevokeDragDrop(hwnd);//just in case the window does use dragndrop. You have to call the revoke function for the window before it gets destroyed. Placing revoke elsewhere could result in a crash.
			DestroyWindow(hwnd);
		}

		if (canvas) {
			delete canvas;
			canvas = nullptr;
		}

		if (hIcon) {
			DestroyIcon(hIcon);
			hIcon = nullptr;
		}
	}

	// Immediately destroys the window.
	void Window::close() {
		if (hwnd && IsWindow(hwnd)) {
			DestroyWindow(hwnd);
		}
		isRunning = false;
	}

	// Processes messages; returns false when closed.
	bool Window::isOpen() {
		if (!isRunning || !hwnd)
			return false;

		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {

			if (msg.message == WM_QUIT) {
				isRunning = false;
				return false;
			}


			bool isOurs = (msg.hwnd == hwnd || IsChild(hwnd, msg.hwnd));

			if (isOurs)
				msgQ.push(msg);

			if (isOurs && !IsDialogMessage(hwnd, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else if (!isOurs) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		return isRunning && hwnd;
	}

	// Pops the next queued message; returns false if queue empty.
	bool Window::pollEvent(MSG& message) {
		if (msgQ.empty())
			return false;

		message = msgQ.front();
		msgQ.pop();
		return true;
	}

	// Clears the back buffer to the given color, recreating if needed.
	void Window::clear(ws::Hue color) {
		if (!hwnd) return;

		ws::Vec2i needed = view.getPortSize();;

		if (!canvas || backBuffer.getSize().x != needed.x || backBuffer.getSize().y != needed.y) {
			delete canvas;
			canvas = nullptr;
			backBuffer.create(needed.x, needed.y);
			canvas = new Gdiplus::Graphics(backBuffer.getHDC());
			canvas->SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
			canvas->SetPixelOffsetMode(Gdiplus::PixelOffsetModeNone);
			canvas->SetSmoothingMode(Gdiplus::SmoothingModeNone);
		}

		if (canvas)
			canvas->Clear(color);
	}

	// Draws a Drawable object using the current view transform.
	void Window::draw(Drawable& draw) {
		if (!canvas || !hwnd) return;



		Gdiplus::Matrix originalMatrix; //Get the original untransformed matrix so that the drawable can be drawn in world coordinates. 
		canvas->GetTransform(&originalMatrix);

		// save clip region
		Gdiplus::Region originalClip;
		canvas->GetClip(&originalClip);

		//Apply the transformation
		view.apply(*canvas);

		//draw the object in world coords.
		draw.drawGlobal(canvas);

		//Restore the original transformation so that the transform can be applied again next time. 
		//This is because changes occur and need to be transformed too.
		canvas->SetTransform(&originalMatrix);
		canvas->SetClip(&originalClip);//restore the original clip boundary.
	}

	// Invalidates and updates the window
	void Window::display() {
		if (m_perPixelAlpha && hwnd)
			updateLayeredWindow();
		if (!hwnd)
			return;
		InvalidateRect(hwnd, NULL, FALSE);
		UpdateWindow(hwnd);
	}

	// Sets a pixel on the back buffer.
	void Window::setPixel(int x, int y, ws::Hue hue) {
		backBuffer.setPixel(x, y, hue);
	}

	// Gets a pixel from the back buffer.
	ws::Hue Window::getPixel(int x, int y) {
		return backBuffer.getPixel(x, y);
	}

	// Returns the window title.
	std::string Window::getTitle() {
		if (!hwnd)
			return "";

		char title[256];
		GetWindowTextA(hwnd, title, sizeof(title));
		return std::string(title);
	}

	// Sets the window title.
	void Window::setTitle(std::string title) {
		if (!hwnd)
			return;
		SetWindowTextA(hwnd, title.c_str());
	}

	// Replaces the current view.
	void Window::setView(ws::View& v) {
		view = v;
	}

	ws::View& Window::getView() {
		return view;
	}

	// Shows or hides the window.
	void Window::setVisible(bool val) {
		if (!hwnd)
			return;

		if (!val)
			ShowWindow(hwnd, SW_HIDE);
		else
			ShowWindow(hwnd, SW_SHOW);
	}

	bool Window::getVisible() {
		if (!hwnd)
			return false;
		return IsWindowVisible(hwnd);
	}

	// Sets focus to this window.
	void Window::setFocus() {
		SetFocus(hwnd);
	}

	bool Window::hasFocus() {
		HWND focus = GetFocus();
		return (focus == hwnd);
	}

	// Places this window after another in Z-order.
	void Window::setLayerAfter(HWND lastHwnd) {
		SetWindowPos(hwnd, lastHwnd, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	}

	// Adds a window style.
	void Window::addStyle(DWORD style) {
		if (!hwnd)
			return;

		DWORD s = getStyle();
		s |= style;

		SetWindowLongA(hwnd, GWL_STYLE, s);
	}

	// Removes a window style.
	void Window::removeStyle(DWORD style) {
		if (!hwnd)
			return;

		DWORD s = getStyle();
		s &= ~style;


		SetWindowLongA(hwnd, GWL_STYLE, s);
	}

	// Sets all window styles at once.
	void Window::setAllStyle(DWORD style) {
		if (!hwnd)
			return;

		SetWindowLongA(hwnd, GWL_STYLE, 0);

		SetWindowLongA(hwnd, GWL_STYLE, style);
	}

	// Adds an extended style.
	void Window::addExStyle(DWORD style) {
		if (!hwnd)
			return;

		DWORD s = getExStyle();
		s |= style;


		SetWindowLongA(hwnd, GWL_EXSTYLE, s);
	}

	// Removes an extended style.
	void Window::removeExStyle(DWORD style) {
		if (!hwnd)
			return;

		DWORD s = getExStyle();
		s &= ~style;


		SetWindowLongA(hwnd, GWL_EXSTYLE, s);
	}

	// Sets all extended styles.
	void Window::setAllExStyle(DWORD style) {
		if (!hwnd)
			return;

		SetWindowLongA(hwnd, GWL_EXSTYLE, 0);

		SetWindowLongA(hwnd, GWL_EXSTYLE, style);
	}

	//returns the extended window styles for this window
	DWORD Window::getExStyle() const {
		return GetWindowLong(hwnd, GWL_EXSTYLE);
	}

	//returns the non-extended window styles for this window.
	DWORD Window::getStyle() const {
		return GetWindowLong(hwnd, GWL_STYLE);

	}

	//checks if a certain individual style exists for this window.
	bool Window::hasStyle(DWORD checkStyle) {
		return (getStyle() & checkStyle);
	}
	//checks if a certain individual extended style exists for this window.
	bool Window::hasExStyle(DWORD checkStyle) {
		return (getExStyle() & checkStyle);
	}
	// Sets the window size (client area dimensions). - Overload for ws::Vec2i
	void Window::setSize(ws::Vec2i size) {
		setSize(size.x, size.y);
	}

	// Sets the window size (client area dimensions).
	void Window::setSize(int screenWidth, int screenHeight) {
		if (screenWidth <= 0 || screenHeight <= 0) {
			setVisible(false);
			ws::log("Warning! You tried to set a window to an invalid size. This has been converted into a safe setVisible(false) command. Try using the setVisible function as a better practice.");
			return;
		}
		SetWindowPos(hwnd,
			nullptr,
			0,
			0,
			screenWidth,
			screenHeight,
			SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);

	}

	// Returns the client area size.
	ws::Vec2i Window::getSize() const {
		RECT rect;
		GetClientRect(hwnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;


		return ws::Vec2i(width, height);
	}

	//sets the window position - overload
	void Window::setPosition(ws::Vec2i pos) {
		setPosition(pos.x, pos.y);
	}

	// Sets the window position.
	void Window::setPosition(int posx, int posy) {
		SetWindowPos(hwnd,
			nullptr,
			posx,
			posy,
			0,
			0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	// Returns the window position.
	ws::Vec2i Window::getPosition() const {
		if (!hwnd)
			return ws::Vec2i(0, 0);

		RECT windowRect;
		GetWindowRect(hwnd, &windowRect);
		return ws::Vec2i(windowRect.left, windowRect.top);
	}

	// Returns the total window rect size (including non-client area).
	ws::Vec2i Window::getTotalSize() const {
		RECT rect;
		GetWindowRect(hwnd, &rect);

		return ws::Vec2i(rect.right - rect.left, rect.bottom - rect.top);
	}

	// Returns the client area position on screen.
	ws::Vec2i Window::getClientPosition() const {
		if (!hwnd)
			return ws::Vec2i(0, 0);

		POINT clientOrigin = { 0, 0 };
		ClientToScreen(hwnd, &clientOrigin);
		return ws::Vec2i(clientOrigin.x, clientOrigin.y);
	}

	// Returns the border width in pixels.
	int Window::getBorderWidth() const {
		int border = 0;
		DWORD style = getStyle();
		if (style & WS_THICKFRAME)
			border = GetSystemMetrics(SM_CYFRAME);
		else if (style & WS_BORDER)
			border = GetSystemMetrics(SM_CYEDGE);
		return border;
	}

	// Returns the caption rectangle (title bar area).
	ws::IntRect Window::getCaptionRect(bool excludeBorder) const {
		if (!hwnd)
			return { 0, 0, 0, 0 };

		DWORD style = getStyle();
		if (!(style & WS_CAPTION))
			return { 0, 0, 0, 0 };

		RECT winRect;
		GetWindowRect(hwnd, &winRect);

		int border = 0;
		if (excludeBorder)
			border = getBorderWidth();

		int captionHeight = GetSystemMetrics(SM_CYCAPTION);
		return ws::IntRect(
			winRect.left,
			winRect.top + border,
			winRect.right - winRect.left,
			captionHeight - border
		);
	}

	// Toggles fullscreen mode.
	void Window::setFullscreen(bool fullscreen) {
		if (fullscreen == isFullscreen) return;

		if (fullscreen) {
			//save the style
			windowedStyle = getStyle();
			GetWindowRect(hwnd, &windowedRect);

			int screenWidth = GetSystemMetrics(SM_CXSCREEN);
			int screenHeight = GetSystemMetrics(SM_CYSCREEN);

			removeStyle(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
			addStyle(WS_POPUP | WS_VISIBLE);

			SetWindowPos(hwnd, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_FRAMECHANGED | SWP_NOACTIVATE);

			isFullscreen = true;
		}
		else {
			setAllStyle(windowedStyle);

			SetWindowPos(hwnd, HWND_TOP,
				windowedRect.left, windowedRect.top,
				windowedRect.right - windowedRect.left,
				windowedRect.bottom - windowedRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			isFullscreen = false;
		}
	}

	//checks if window is in fullscreen styling.
	bool Window::getFullscreen() const {
		return isFullscreen;
	}

	//force all clicks to pass through this window as if it does not exist.
	void Window::disableAnyClicks() {
		addMessageHandler([](MSG msg) -> LRESULT {
			if (msg.message == WM_NCHITTEST) {
				return HTTRANSPARENT;   // mouse passes through
			}
			return 0;
			});
	}

	//Choose what color should represent emptiness and any click will pass through that color as if nothing is there.
	void Window::disableSomeClicks(ws::Hue hue) {
		addMessageHandler([this, hue](MSG msg) -> LRESULT {
			if (msg.message == WM_NCHITTEST) {

				POINT p;
				if (!GetCursorPos(&p)) {
					p.x = -1000;
					p.y = -1000;
				}
				ScreenToClient(hwnd, &p); // Convert to client coordinates

				ws::Vec2i MP = toWorld(p);
				if (getPixel(MP.x, MP.y) == hue)
					return HTTRANSPARENT;   // mouse passes through
			}
			return 0;
			});
	}

	// Enables chroma key transparency (legacy per-pixel or simple).
	void Window::enableChromaKey(ws::Hue hue, bool legacy) {
		m_perPixelAlpha = legacy;

		addExStyle(WS_EX_LAYERED);

		if (!legacy)
			SetLayeredWindowAttributes(hwnd, RGB(hue.r, hue.g, hue.b), static_cast<BYTE>(hue.a), LWA_COLORKEY | LWA_ALPHA);
	}

	//disables the chroma keying that causes window transparency. - also disables alpha only.
	void Window::disableChromaKey() {
		removeExStyle(WS_EX_LAYERED);
	}

	// Enables alpha-only transparency.
	void Window::enableAlphaOnly(float alpha) {
		addExStyle(WS_EX_LAYERED);
		if (!m_perPixelAlpha)
			SetLayeredWindowAttributes(hwnd, 0, static_cast<BYTE>(alpha), LWA_ALPHA);
	}

	//Disables alpha only transparency - also disables chroma key.
	void Window::disableAlphaOnly() {
		disableChromaKey();
	}

	//Sets the window behind the desktop icons. (WARNING -  this will override many window settings and styles to produce a functional desktop window.)
	bool Window::setBehindIcons(HWND source) {
		HWND progman = FindWindowW(L"Progman", nullptr);
		HWND workerW = FindWindowExW(progman, nullptr, L"WorkerW", nullptr);
		HWND shellView = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
		if (!progman || !workerW || !shellView) {
			ws::log("Could not locate Progman / WorkerW / SHELLDLL_DefView. Failed to set Window behind icons!");
			return false;
		}

		//for windows 11

#ifndef WS_EX_NOREDIRECTIONBITMAP
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif

		bool raised = (GetWindowLongPtr(progman, GWL_EXSTYLE) & WS_EX_NOREDIRECTIONBITMAP) != 0;
		if (!raised) {
			ws::log("Can't set window behind icons on this computer due to imcompatible desktop design.");
			return false;
		}


		//SetAllStyle
		SetWindowLongA(source, GWL_STYLE, 0);
		SetWindowLongA(source, GWL_STYLE, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);

		SetParent(source, workerW);

		//add WS_EX_LAYERED
		DWORD exStyle = GetWindowLong(source, GWL_EXSTYLE);
		exStyle |= WS_EX_LAYERED;
		SetWindowLongA(source, GWL_EXSTYLE, exStyle);


		//enable alpha only
		SetLayeredWindowAttributes(source, 0, 255, LWA_ALPHA);

		ShowWindow(source, SW_SHOW);

		//setLayerAfter shellView
		SetWindowPos(source, shellView, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
		return true;
	}

	//overload that sets this window behind the icons.
	bool Window::setBehindIcons() {
		return setBehindIcons(hwnd);
	}

	// Converts screen coordinates to world.
	ws::Vec2i Window::toWorld(int x, int y) {
		return view.toWorld(x, y, getSize());
	}
	// Converts screen coordinates to world. - overload
	ws::Vec2i Window::toWorld(ws::Vec2i pos) {
		return toWorld(pos.x, pos.y);
	}

	// Converts world coordinates to screen.
	ws::Vec2i Window::toScreen(int x, int y) {
		return view.toScreen(x, y, getSize());
	}
	// Converts world coordinates to screen. - overload
	ws::Vec2i Window::toScreen(ws::Vec2i pos) {
		return toScreen(pos.x, pos.y);
	}

	//You really shouldn't mess with this.
	// Sets where in texture coordinates the backbuffer is drawn on the window client area. 
	void Window::setSourcePos(ws::Vec2i pos) {
		srcPos = pos;
	}
	// Sets where in texture coordinates the backbuffer is drawn on the window client area. - overload
	void Window::setSourcePos(int x, int y) {
		srcPos = { x,y };
	}
	//returns the position where the backbuffer is drawn to the window client area.
	ws::Vec2i Window::getSourcePos() {
		return srcPos;
	}

	// Sets the window cursor.
	void Window::setCursor(ws::Cursor newcursor) {
		cursor = newcursor;
	}
	ws::Cursor Window::getCursor() {
		return cursor;
	}

	//Sets the window icon to a given HICON. - This function is not responsible for cleanup of your HICON handle.
	bool Window::setIcon(HICON icon, DWORD size) {

		if (!icon)
			return false;

		if (hIcon) {
			DestroyIcon(hIcon);
			hIcon = nullptr;
		}

		hIcon = icon;
		SendMessage(hwnd, WM_SETICON, size, (LPARAM)hIcon);
		return true;
	}

	//Sets the window icon to a given .ico file.
	bool Window::setIcon(std::string file, DWORD size) {

		HICON m_hIcon = (HICON)LoadImageA(
			NULL,
			file.c_str(),
			IMAGE_ICON,
			0, 0,
			LR_LOADFROMFILE | LR_DEFAULTSIZE
		);
		bool res = setIcon(m_hIcon, size);
		DestroyIcon(m_hIcon);
		m_hIcon = nullptr;
		return res;
	}

	HICON Window::getIcon() {
		return hIcon;
	}

	// Adds a custom message handler.
	void Window::addMessageHandler(std::function<LRESULT(MSG msg)> handler) {
		customHandlers.push_back(std::move(handler));
	}

	std::vector<std::function<LRESULT(MSG msg)>> Window::getMessageHandlers() {
		return customHandlers;
	}

	// Updates the layered window for per‑pixel alpha when enabled.
	void Window::updateLayeredWindow() {
		if (!m_perPixelAlpha || !hwnd) return;

		HDC hdc = backBuffer.getHDC();
		if (!hdc) return;

		SIZE size = { backBuffer.getSize().x, backBuffer.getSize().y };
		POINT ptSrc = { 0, 0 };
		POINT ptDst = { 0, 0 };
		RECT rect;
		GetWindowRect(hwnd, &rect);
		ptDst.x = rect.left;
		ptDst.y = rect.top;

		BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		UpdateLayeredWindow(hwnd, nullptr, &ptDst, &size, hdc, &ptSrc, 0, &blend, ULW_ALPHA);
	}

	//cleanup dragndrop whether it was used or not.
	void Window::pRevokeDragDrop(HWND& targetHwnd) {
		typedef HRESULT(WINAPI* Ptr)(HWND);

		HMODULE hOle32 = LoadLibrary(TEXT("Ole32.dll"));
		if (hOle32 == nullptr)
			return;
		Ptr ptr = (Ptr)GetProcAddress(hOle32, "RevokeDragDrop");
		if (ptr == nullptr) {
			FreeLibrary(hOle32);
			return;
		}
		ptr(targetHwnd);
		FreeLibrary(hOle32);
	}


	// Handles messages; called by the global window procedure.
	LRESULT Window::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		for (auto& handler : customHandlers) {
			MSG msg;
			msg.message = uMsg;
			msg.lParam = lParam;
			msg.wParam = wParam;

			LRESULT lresult = handler(msg);
			if (lresult != 0)
				return lresult;//if result is not zero that means that the message was handled.
		}

		switch (uMsg) {

		case WM_NCLBUTTONDBLCLK:
			if (wParam == HTSYSMENU)
				return 0;
			break;

		case WM_DESTROY:
			WindowManager::removeWindow(hwnd);
			isRunning = false;
			if (WindowManager::windows.empty())
				PostQuitMessage(0);
			return 0;

		case WM_SYSCOMMAND:
			if (wParam == SC_CLOSE && HIWORD(lParam) == 0 /* from menu */)
				return 0;
			break;

		case WM_CLOSE:
			DestroyWindow(hwnd);
			return 0;

		case WM_NOTIFY:
		{
			NMHDR* pnmh = reinterpret_cast<NMHDR*>(lParam);
			return s_handleNotifyForChildren(this, pnmh, uMsg, wParam, lParam);
		}

		case WM_COMMAND:
		{
			MSG msg = {};
			msg.hwnd = hwnd;
			msg.message = WM_COMMAND;
			msg.wParam = wParam;
			msg.lParam = lParam;
			msgQ.push(msg);
			return 0;
		}

		case WM_SETCURSOR:
		{
			if (LOWORD(lParam) == HTCLIENT) {
				SetClassLongPtr(hwnd, GCLP_HCURSOR, (LONG_PTR)(cursor.getHandle() ? cursor.getHandle() : LoadCursor(nullptr, IDC_ARROW)));
			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		case WM_PAINT:
		{

			if (m_perPixelAlpha) {
				PAINTSTRUCT ps;
				BeginPaint(hwnd, &ps);
				EndPaint(hwnd, &ps);
				return 0;
			}
			else {

				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);

				if (backBuffer.isFastDIB()) {

					RECT clientRect;
					GetClientRect(hwnd, &clientRect);
					int dstW = clientRect.right - clientRect.left;
					int dstH = clientRect.bottom - clientRect.top;

					int bufW = backBuffer.getSize().x;
					int bufH = backBuffer.getSize().y;

					int srcH = bufH - srcPos.y;
					int srcW = bufW - srcPos.x;
					if (srcH < 1) srcH = 1;   // safety

					SetStretchBltMode(hdc, COLORONCOLOR);
					StretchBlt(
						hdc,
						0, 0, dstW, dstH,               // destination = full client area
						backBuffer.getHDC(),
						srcPos.x, srcPos.y, bufW, srcH,            // source = everything below the menu
						SRCCOPY
					);
				}
				EndPaint(hwnd, &ps);
			}
			return 0;

		}
		case WM_ERASEBKGND:
			return 1;

		default:
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	// Default notify stub that just passes to DefWindowProc.
	LRESULT Window::defaultNotifyStub(Window* window, NMHDR* pnmh, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		return DefWindowProc(window->hwnd, uMsg, wParam, lParam);
	}
	//=====SCREEN=====

			// Acquires the desktop HDC.
	Screen::Screen() {
		hdc = GetDC(NULL);
		if (hdc == NULL)
			valid = false;
	}

	// Releases the desktop HDC.
	Screen::~Screen() {
		ReleaseDC(NULL, hdc);
	}

	HDC Screen::getHDC() {
		return hdc;
	}

	// Enumerates all unique display resolutions.
	std::vector<ws::Vec2i> Screen::getDisplayModes() {
		std::vector<ws::Vec2i> modes;

		DEVMODE dm = {};
		dm.dmSize = sizeof(dm);
		DWORD modeNum = 0;

		while (EnumDisplaySettings(NULL, modeNum, &dm)) {
			ws::Vec2i res = ws::Vec2i((int)dm.dmPelsWidth, (int)dm.dmPelsHeight);

			bool duplicate = false;
			for (const auto& existing : modes) {
				if (existing.x == res.x && existing.y == res.y) {
					duplicate = true;
					break;
				}
			}
			if (!duplicate) {
				modes.push_back(res);
			}
			modeNum++;
		}

		//sort according to size with smallest first.
		std::sort(modes.begin(), modes.end(), [](ws::Vec2i& a, ws::Vec2i& b) {
			return a.x * a.y < b.x * b.y;
			});

		return modes;
	}

	// Changes the display resolution; returns status string.
	std::string Screen::setSize(int x, int y) {
		return setSize({ x,y });
	}

	std::string Screen::setSize(ws::Vec2i size) {
		DEVMODE dm;
		ZeroMemory(&dm, sizeof(dm));
		dm.dmSize = sizeof(dm);
		dm.dmPelsWidth = size.x;
		dm.dmPelsHeight = size.y;
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

		LONG result = ChangeDisplaySettings(&dm, CDS_UPDATEREGISTRY);
		switch (result) {
		case DISP_CHANGE_SUCCESSFUL:   break;
		case DISP_CHANGE_BADMODE:      /* resolution not supported */
			return "INVALID";
		case DISP_CHANGE_RESTART:      /* need restart */
			return "NEEDRESTART";
		default:
			return "ERROR";
		}

		hdc = GetDC(NULL);
		if (hdc == NULL)
			valid = false;
		return "SUCCESS";
	}

	// Returns the current screen size.
	ws::Vec2i Screen::getSize() {
		int width = GetDeviceCaps(hdc, HORZRES);
		int height = GetDeviceCaps(hdc, VERTRES);
		return ws::Vec2i(width, height);
	}

	// Gets a pixel from the screen.
	ws::Hue Screen::getPixel(int x, int y) {
		if (!valid)
			return ws::Hue::transparent;
		return getPixel(ws::Vec2i(x, y));
	}

	ws::Hue Screen::getPixel(ws::Vec2i pos) {
		if (!valid)
			return ws::Hue::transparent;

		if (pos.x >= 0 && pos.x < getSize().x && pos.y >= 0 && pos.y < getSize().y)
			return GetPixel(hdc, pos.x, pos.y);

		return ws::Hue::transparent;
	}

	// Sets a pixel on the screen.
	void Screen::setPixel(int x, int y, ws::Hue hue) {
		if (!valid)
			return;
		if (x >= 0 && x < getSize().x && y >= 0 && y < getSize().y)
			SetPixel(hdc, x, y, hue);
	}

	// Captures the screen into a ws::Texture.
	ws::Texture Screen::getSnapshot() {
		ws::Texture texture;
		if (!texture.create(getSize().x, getSize().y, ws::Hue::transparent))
			return ws::Texture();

		HDC hdcTex = texture.getHDC();
		if (hdcTex)
			BitBlt(hdcTex, 0, 0, getSize().x, getSize().y, hdc, 0, 0, SRCCOPY);

		return texture;
	}

	// Returns the system DPI scaling factor.
	float Screen::getDPI() {
		HDC hdc = GetDC(nullptr);
		if (!hdc) return 1.0f;
		int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
		ReleaseDC(nullptr, hdc);
		return static_cast<float>(dpi) / 96.0f;
	}

	// Returns the work area (excluding taskbar).
	ws::IntRect Screen::getWorkArea() {
		RECT area;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &area, 0);
		return { area.left, area.top, area.right - area.left, area.bottom - area.top };
	}

	// Initializes GDI+ and sets DPI awareness.
	GDIPInit::GDIPInit() {
		//GDI+
		Gdiplus::GdiplusStartup(&gdiplustoken, &gdiplusstartup, nullptr);
		//get a handle to the already loaded user32.dll
		HMODULE hUser32 = LoadLibraryW(L"user32.dll");
		if (hUser32) {
			SetDPIAwareProc pSetDPIAware = (SetDPIAwareProc)GetProcAddress(hUser32, "SetProcessDPIAware");
			//only call the function if it exists. - On windows xp or below, this does not exist but is also not needed.
			if (pSetDPIAware != NULL)
				pSetDPIAware();
		}
	}

	// Shuts down GDI+.
	GDIPInit::~GDIPInit() {
		//Gdi+
		Gdiplus::GdiplusShutdown(gdiplustoken);
	}

#endif

}

#endif