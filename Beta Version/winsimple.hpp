#ifndef WINSIMPLE_HPP
#define WINSIMPLE_HPP


// winsimple.hpp – Core Windows GDI+ graphics library (header‑only core)

//Core Winsimple Linking:  -lgdi32 -luser32 -lgdiplus -lole32




// Suppress unnecessary depreciation warnings if any.
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

// Windows headers
#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>

// Standard C++ headers needed for core classes
#include <iostream>      // std::cerr
#include <string>
#include <cstdlib>       // std::atoi, etc.
#include <map>
#include <queue>
#include <iomanip>       // std::quoted
#include <cmath>         // std::cos, std::sin, std::pow
#include <filesystem>    // std::filesystem::path
#include <cwchar>        // wcsrchr, etc.
#include <algorithm>     // std::transform, std::min, std::max
#include <functional>    // std::function
#include <type_traits>   // std::enable_if, std::is_arithmetic, etc.
#include <utility>       // std::move, std::pair
#include <mutex>         // std::mutex

// Define missing Windows types for GDI+
#ifndef SHORT
typedef short SHORT;
#endif
#ifndef PROPID
typedef unsigned long PROPID;
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Some system cursor IDs that are not always defined
#ifndef IDC_PIN
#define IDC_PIN MAKEINTRESOURCE(32671)
#endif
#ifndef IDC_PERSON
#define IDC_PERSON MAKEINTRESOURCE(32672)
#endif


//automated linking for visual studio MSVC
#ifdef _MSC_VER
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "ole32.lib")
#endif



namespace ws
{
	//All forward declares
	class Child;
	class WindowManager;
	class Window;
	
	
	//<><><><><> Core Utilities <><><><><><>
	class Timer
	{
		public:
		
		Timer()
		{
			LARGE_INTEGER freq;
            QueryPerformanceFrequency(&freq);
            frequency = static_cast<double>(freq.QuadPart);
			restart();
		}
		
		~Timer()
		{
		}
		
		double restart()
		{
	        double seconds = getSeconds();
            LARGE_INTEGER counter;
            QueryPerformanceCounter(&counter);
            startTime = counter.QuadPart;
            
            return seconds;
		}
		
		
	    double getSeconds() const
	    {
	        LARGE_INTEGER currentTime;
	        QueryPerformanceCounter(&currentTime);
	        return static_cast<double>(currentTime.QuadPart - startTime) / frequency;
	      
	    }
	    
	    double getMilliSeconds() const
	    {
	        return getSeconds() * 1000.0;
	    }
	    
	    double getMicroSeconds() const
	    {
	        return getMilliSeconds() * 1000.0;
	    }
	
		private:
		    LONGLONG startTime = 0;
		    double frequency = 1.0;		
	};
	
	
	
    // String conversion helpers
    inline std::wstring WIDE(const std::string& str)
    {
        int size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);
        if (GetLastError() == ERROR_NO_UNICODE_TRANSLATION)
            size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
        std::wstring wstr(size, 0);
        if (size > 0)
            MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
        return wstr;
    }
    inline std::string SHORT(const std::wstring& wstr)
    {
        int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
        if (size == 0) return "";
        std::string str(size - 1, '\0');
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, str.data(), size, NULL, NULL);
        return str;
    }
    inline LPCSTR TO_LPCSTR(const std::string& str) { return str.c_str(); }
    inline LPCWSTR TO_LPCWSTR(const std::string& str) { return WIDE(str).c_str(); }

    // Internal helpers (used by Texture)
    inline std::wstring GetShortPathNameSafe(const std::wstring& longPath)
    {
        DWORD size = GetShortPathNameW(longPath.c_str(), NULL, 0);
        if (size == 0) return L"";
        std::wstring shortPath(size, L'\0');
        size = GetShortPathNameW(longPath.c_str(), shortPath.data(), size);
        if (size == 0) return L"";
        shortPath.resize(size);
        return shortPath;
    }
    inline int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
    {
        UINT num = 0, size = 0;
        Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;
        Gdiplus::GetImageEncodersSize(&num, &size);
        if (size == 0) return -1;
        pImageCodecInfo = (Gdiplus::ImageCodecInfo*)malloc(size);
        if (!pImageCodecInfo) return -1;
        Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
        for (UINT j = 0; j < num; ++j)
        {
            if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
            {
                *pClsid = pImageCodecInfo[j].Clsid;
                free(pImageCodecInfo);
                return j;
            }
        }
        free(pImageCodecInfo);
        return -1;
    }
    inline bool ResolveRelativePath(std::string& path)
    {
        std::filesystem::path filePath(path);
        if (filePath.is_relative())
        {
            char exePath[MAX_PATH];
            GetModuleFileNameA(NULL, exePath, MAX_PATH);
            std::filesystem::path exeDir = std::filesystem::path(exePath).parent_path();
            filePath = exeDir / filePath;
        }
        path = filePath.string();
        return true;
    }
	

	// Data Type Conversion Operators
	
	
	// Vector type traits
	template<typename T, typename = void>
	struct has_xy_members : std::false_type {};
	
	template<typename T>
	struct has_xy_members<T, std::void_t<
	    decltype(std::declval<T>().x), 
	    decltype(std::declval<T>().y)>>
	    : std::true_type {};
	
	template<typename T, typename = void>
	struct has_xyz_members : std::false_type {};
	
	template<typename T>
	struct has_xyz_members<T, std::void_t<
	    decltype(std::declval<T>().x), 
	    decltype(std::declval<T>().y),
	    decltype(std::declval<T>().z)>>
	    : std::true_type {};
	
	// Rect type traits
	template<typename T, typename = void>
	struct has_width_height_style : std::false_type {};
	
	template<typename T>
	struct has_width_height_style<T, std::void_t<
	    decltype(std::declval<T>().left), 
	    decltype(std::declval<T>().top),
	    decltype(std::declval<T>().width),
	    decltype(std::declval<T>().height)>> 
	    : std::true_type {};
	
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
	struct is_rect_like : std::integral_constant<bool, 
	    has_width_height_style<T>::value || has_right_bottom_style<T>::value> {};
	
	// ==================== VEC2I ====================
	
	struct Vec2i {
	    int x, y;
	    
	    Vec2i() = default;
	    
	    // Constructor for any two arithmetic types
	    template<typename T, typename U,
	             typename = std::enable_if_t<std::is_arithmetic_v<T> && 
	                                         std::is_arithmetic_v<U>>>
	    Vec2i(T x_val, U y_val) 
	        : x(static_cast<int>(x_val)), 
	          y(static_cast<int>(y_val)) {}
	    
	    // Constructor for any type with .x and .y members
	    template<typename T,
	             typename = std::enable_if_t<has_xy_members<T>::value>>
	    Vec2i(const T& other) 
	        : x(static_cast<int>(other.x)), 
	          y(static_cast<int>(other.y)) {}
	    
	    // Conversion operator to any type with .x and .y members
	    template<typename T,
	             typename = std::enable_if_t<has_xy_members<T>::value>>
	    operator T() const {
	        T result;
	        result.x = static_cast<decltype(T::x)>(x);
	        result.y = static_cast<decltype(T::y)>(y);
	        return result;
	    }
	    
	    // Special POINT pointer conversions (only if layout matches!)
	    operator POINT*() { 
	        return reinterpret_cast<POINT*>(this); 
	    }
	    
	    operator const POINT*() const { 
	        return reinterpret_cast<const POINT*>(this); 
	    }
	};
	
	// ==================== VEC2F ====================
	
	struct Vec2f {
	    float x, y;
	    
	    Vec2f() = default;
	    
	    // Constructor for any two arithmetic types
	    template<typename T, typename U,
	             typename = std::enable_if_t<std::is_arithmetic_v<T> && 
	                                         std::is_arithmetic_v<U>>>
	    Vec2f(T x_val, U y_val) 
	        : x(static_cast<float>(x_val)), 
	          y(static_cast<float>(y_val)) {}
	    
	    // Constructor for any type with .x and .y members
	    template<typename T,
	             typename = std::enable_if_t<has_xy_members<T>::value>>
	    Vec2f(const T& other) 
	        : x(static_cast<float>(other.x)), 
	          y(static_cast<float>(other.y)) {}
	    
	    // Conversion operator to any type with .x and .y members
	    template<typename T,
	             typename = std::enable_if_t<has_xy_members<T>::value>>
	    operator T() const {
	        T result;
	        result.x = static_cast<decltype(T::x)>(x);
	        result.y = static_cast<decltype(T::y)>(y);
	        return result;
	    }
	    
	    // Special POINT pointer conversions
	    operator POINT*() { 
	        return reinterpret_cast<POINT*>(this); 
	    }
	    
	    operator const POINT*() const { 
	        return reinterpret_cast<const POINT*>(this); 
	    }
	};
	
	// ==================== VEC2D ====================
	
	struct Vec2d {
	    double x, y;
	    
	    Vec2d() = default;
	    
	    // Constructor for any two arithmetic types
	    template<typename T, typename U,
	             typename = std::enable_if_t<std::is_arithmetic_v<T> && 
	                                         std::is_arithmetic_v<U>>>
	    Vec2d(T x_val, U y_val) 
	        : x(static_cast<double>(x_val)), 
	          y(static_cast<double>(y_val)) {}
	    
	    // Constructor for any type with .x and .y members
	    template<typename T,
	             typename = std::enable_if_t<has_xy_members<T>::value>>
	    Vec2d(const T& other) 
	        : x(static_cast<double>(other.x)), 
	          y(static_cast<double>(other.y)) {}
	    
	    // Conversion operator to any type with .x and .y members
	    template<typename T,
	             typename = std::enable_if_t<has_xy_members<T>::value>>
	    operator T() const {
	        T result;
	        result.x = static_cast<decltype(T::x)>(x);
	        result.y = static_cast<decltype(T::y)>(y);
	        return result;
	    }
	    
	    // Special POINT pointer conversions
	    operator POINT*() { 
	        return reinterpret_cast<POINT*>(this); 
	    }
	    
	    operator const POINT*() const { 
	        return reinterpret_cast<const POINT*>(this); 
	    }
	};
	
	// ==================== VEC3I ====================
	
	struct Vec3i {
	    int x, y, z;
	    
	    Vec3i() = default;
	    
	    // Constructor for any three arithmetic types
	    template<typename T, typename U, typename V,
	             typename = std::enable_if_t<std::is_arithmetic_v<T> && 
	                                         std::is_arithmetic_v<U> &&
	                                         std::is_arithmetic_v<V>>>
	    Vec3i(T x_val, U y_val, V z_val) 
	        : x(static_cast<int>(x_val)), 
	          y(static_cast<int>(y_val)), 
	          z(static_cast<int>(z_val)) {}
	    
	    // Constructor for any type with .x, .y and .z members
	    template<typename T,
	             typename = std::enable_if_t<has_xyz_members<T>::value>>
	    Vec3i(const T& other) 
	        : x(static_cast<int>(other.x)), 
	          y(static_cast<int>(other.y)), 
	          z(static_cast<int>(other.z)) {}
	    
	    // Conversion operator to any type with .x, .y and .z members
	    template<typename T,
	             typename = std::enable_if_t<has_xyz_members<T>::value>>
	    operator T() const {
	        T result;
	        result.x = static_cast<decltype(T::x)>(x);
	        result.y = static_cast<decltype(T::y)>(y);
	        result.z = static_cast<decltype(T::z)>(z);
	        return result;
	    }
	};
	
	// ==================== VEC3F ====================
	
	struct Vec3f {
	    float x, y, z;
	    
	    Vec3f() = default;
	    
	    // Constructor for any three arithmetic types
	    template<typename T, typename U, typename V,
	             typename = std::enable_if_t<std::is_arithmetic_v<T> && 
	                                         std::is_arithmetic_v<U> &&
	                                         std::is_arithmetic_v<V>>>
	    Vec3f(T x_val, U y_val, V z_val) 
	        : x(static_cast<float>(x_val)), 
	          y(static_cast<float>(y_val)), 
	          z(static_cast<float>(z_val)) {}
	    
	    // Constructor for any type with .x, .y and .z members
	    template<typename T,
	             typename = std::enable_if_t<has_xyz_members<T>::value>>
	    Vec3f(const T& other) 
	        : x(static_cast<float>(other.x)), 
	          y(static_cast<float>(other.y)), 
	          z(static_cast<float>(other.z)) {}
	    
	    // Conversion operator to any type with .x, .y and .z members
	    template<typename T,
	             typename = std::enable_if_t<has_xyz_members<T>::value>>
	    operator T() const {
	        T result;
	        result.x = static_cast<decltype(T::x)>(x);
	        result.y = static_cast<decltype(T::y)>(y);
	        result.z = static_cast<decltype(T::z)>(z);
	        return result;
	    }
	};
	
	// ==================== VEC3D ====================
	
	struct Vec3d {
	    double x, y, z;
	    
	    Vec3d() = default;
	    
	    // Constructor for any three arithmetic types
	    template<typename T, typename U, typename V,
	             typename = std::enable_if_t<std::is_arithmetic_v<T> && 
	                                         std::is_arithmetic_v<U> &&
	                                         std::is_arithmetic_v<V>>>
	    Vec3d(T x_val, U y_val, V z_val) 
	        : x(static_cast<double>(x_val)), 
	          y(static_cast<double>(y_val)), 
	          z(static_cast<double>(z_val)) {}
	    
	    // Constructor for any type with .x, .y and .z members
	    template<typename T,
	             typename = std::enable_if_t<has_xyz_members<T>::value>>
	    Vec3d(const T& other) 
	        : x(static_cast<double>(other.x)), 
	          y(static_cast<double>(other.y)), 
	          z(static_cast<double>(other.z)) {}
	    
	    // Conversion operator to any type with .x, .y and .z members
	    template<typename T,
	             typename = std::enable_if_t<has_xyz_members<T>::value>>
	    operator T() const {
	        T result;
	        result.x = static_cast<decltype(T::x)>(x);
	        result.y = static_cast<decltype(T::y)>(y);
	        result.z = static_cast<decltype(T::z)>(z);
	        return result;
	    }
	};
	
	// ==================== INTRECT ====================
	
	struct IntRect {
	    int left, top, width, height;
	    
	    IntRect() = default;
	    
	    // Constructor for any four arithmetic types
	    template<typename T1, typename T2, typename T3, typename T4,
	             typename = std::enable_if_t<std::is_arithmetic_v<T1> && 
	                                         std::is_arithmetic_v<T2> &&
	                                         std::is_arithmetic_v<T3> &&
	                                         std::is_arithmetic_v<T4>>>
	    IntRect(T1 l, T2 t, T3 w, T4 h) 
	        : left(static_cast<int>(l)), 
	          top(static_cast<int>(t)),
	          width(static_cast<int>(w)), 
	          height(static_cast<int>(h)) {}
	    
	    // Constructor for any rect-like type
	    template<typename T,
	             typename = std::enable_if_t<is_rect_like<T>::value>>
	    IntRect(const T& other) {
	        if constexpr (has_width_height_style<T>::value) {
	            // Width/height style (e.g., sf::IntRect, your own rect types)
	            left = static_cast<int>(other.left);
	            top = static_cast<int>(other.top);
	            width = static_cast<int>(other.width);
	            height = static_cast<int>(other.height);
	        } else {
	            // Right/bottom style (e.g., RECT)
	            left = static_cast<int>(other.left);
	            top = static_cast<int>(other.top);
	            width = static_cast<int>(other.right - other.left);
	            height = static_cast<int>(other.bottom - other.top);
	        }
	    }
	    
	    // Conversion operator to any rect-like type
	    template<typename T,
	             typename = std::enable_if_t<is_rect_like<T>::value>>
	    operator T() const {
	        T result;
	        
	        if constexpr (has_width_height_style<T>::value) {
	            // Convert to width/height style
	            result.left = static_cast<decltype(T::left)>(left);
	            result.top = static_cast<decltype(T::top)>(top);
	            result.width = static_cast<decltype(T::width)>(width);
	            result.height = static_cast<decltype(T::height)>(height);
	        } else {
	            // Convert to right/bottom style
	            result.left = static_cast<decltype(T::left)>(left);
	            result.top = static_cast<decltype(T::top)>(top);
	            result.right = static_cast<decltype(T::right)>(left + width);
	            result.bottom = static_cast<decltype(T::bottom)>(top + height);
	        }
	        
	        return result;
	    }
		
        bool contains(const ws::Vec2i& point) const 
		{
            return point.x >= left && point.x < left + width && point.y >= top && point.y < top + height;
        }
        bool contains(const ws::Vec2f& point) const 
		{
            return point.x >= left && point.x < left + width && point.y >= top && point.y < top + height;
        }
        bool operator==(const IntRect& other) const 
		{
            return left == other.left && top == other.top && width == other.width && height == other.height;
        }
        bool operator!=(const IntRect& other) const 
		{
            return !(*this == other);
        }		
	};
	
	// ==================== FLOATRECT ====================
	
	struct FloatRect {
	    float left, top, width, height;
	    
	    FloatRect() = default;
	    
	    // Constructor for any four arithmetic types
	    template<typename T1, typename T2, typename T3, typename T4,
	             typename = std::enable_if_t<std::is_arithmetic_v<T1> && 
	                                         std::is_arithmetic_v<T2> &&
	                                         std::is_arithmetic_v<T3> &&
	                                         std::is_arithmetic_v<T4>>>
	    FloatRect(T1 l, T2 t, T3 w, T4 h) 
	        : left(static_cast<float>(l)), 
	          top(static_cast<float>(t)),
	          width(static_cast<float>(w)), 
	          height(static_cast<float>(h)) {}
	    
	    // Constructor for any rect-like type
	    template<typename T,
	             typename = std::enable_if_t<is_rect_like<T>::value>>
	    FloatRect(const T& other) {
	        if constexpr (has_width_height_style<T>::value) {
	            left = static_cast<float>(other.left);
	            top = static_cast<float>(other.top);
	            width = static_cast<float>(other.width);
	            height = static_cast<float>(other.height);
	        } else {
	            left = static_cast<float>(other.left);
	            top = static_cast<float>(other.top);
	            width = static_cast<float>(other.right - other.left);
	            height = static_cast<float>(other.bottom - other.top);
	        }
	    }
	    
	    // Conversion operator to any rect-like type
	    template<typename T,
	             typename = std::enable_if_t<is_rect_like<T>::value>>
	    operator T() const {
	        T result;
	        
	        if constexpr (has_width_height_style<T>::value) {
	            result.left = static_cast<decltype(T::left)>(left);
	            result.top = static_cast<decltype(T::top)>(top);
	            result.width = static_cast<decltype(T::width)>(width);
	            result.height = static_cast<decltype(T::height)>(height);
	        } else {
	            result.left = static_cast<decltype(T::left)>(left);
	            result.top = static_cast<decltype(T::top)>(top);
	            result.right = static_cast<decltype(T::right)>(left + width);
	            result.bottom = static_cast<decltype(T::bottom)>(top + height);
	        }
	        
	        return result;
	    }
		
        bool contains(const ws::Vec2i& point) const 
		{
            return point.x >= left && point.x < left + width && point.y >= top && point.y < top + height;
        }
        bool contains(const ws::Vec2f& point) const 
		{
            return point.x >= left && point.x < left + width && point.y >= top && point.y < top + height;
        }
        bool operator==(const FloatRect& other) const 
		{
            return left == other.left && top == other.top && width == other.width && height == other.height;
        }
        bool operator!=(const FloatRect& other) const 
		{
            return !(*this == other);
        }				
	};
	
	// ==================== DOUBLERECT ====================
	
	struct DoubleRect {
	    double left, top, width, height;
	    
	    DoubleRect() = default;
	    
	    // Constructor for any four arithmetic types
	    template<typename T1, typename T2, typename T3, typename T4,
	             typename = std::enable_if_t<std::is_arithmetic_v<T1> && 
	                                         std::is_arithmetic_v<T2> &&
	                                         std::is_arithmetic_v<T3> &&
	                                         std::is_arithmetic_v<T4>>>
	    DoubleRect(T1 l, T2 t, T3 w, T4 h) 
	        : left(static_cast<double>(l)), 
	          top(static_cast<double>(t)),
	          width(static_cast<double>(w)), 
	          height(static_cast<double>(h)) {}
	    
	    // Constructor for any rect-like type
	    template<typename T,
	             typename = std::enable_if_t<is_rect_like<T>::value>>
	    DoubleRect(const T& other) {
	        if constexpr (has_width_height_style<T>::value) {
	            left = static_cast<double>(other.left);
	            top = static_cast<double>(other.top);
	            width = static_cast<double>(other.width);
	            height = static_cast<double>(other.height);
	        } else {
	            left = static_cast<double>(other.left);
	            top = static_cast<double>(other.top);
	            width = static_cast<double>(other.right - other.left);
	            height = static_cast<double>(other.bottom - other.top);
	        }
	    }
	    
	    // Conversion operator to any rect-like type
	    template<typename T,
	             typename = std::enable_if_t<is_rect_like<T>::value>>
	    operator T() const {
	        T result;
	        
	        if constexpr (has_width_height_style<T>::value) {
	            result.left = static_cast<decltype(T::left)>(left);
	            result.top = static_cast<decltype(T::top)>(top);
	            result.width = static_cast<decltype(T::width)>(width);
	            result.height = static_cast<decltype(T::height)>(height);
	        } else {
	            result.left = static_cast<decltype(T::left)>(left);
	            result.top = static_cast<decltype(T::top)>(top);
	            result.right = static_cast<decltype(T::right)>(left + width);
	            result.bottom = static_cast<decltype(T::bottom)>(top + height);
	        }
	        
	        return result;
	    }

        bool contains(const ws::Vec2i& point) const 
		{
            return point.x >= left && point.x < left + width && point.y >= top && point.y < top + height;
        }
        bool contains(const ws::Vec2f& point) const 
		{
            return point.x >= left && point.x < left + width && point.y >= top && point.y < top + height;
        }
        bool operator==(const DoubleRect& other) const 
		{
            return left == other.left && top == other.top && width == other.width && height == other.height;
        }
        bool operator!=(const DoubleRect& other) const 
		{
            return !(*this == other);
        }				
	};


	
	class Hue
	{
		public:
		int r=0,g=0,b=0,a=255;
		
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
		
		Hue(Gdiplus::Color &color)
		{
			r = color.GetR();
			g = color.GetG();
			b = color.GetB();
			a = color.GetA();
		}

	    Hue(COLORREF color)
	    {
	        r = GetRValue(color);
	        g = GetGValue(color);
	        b = GetBValue(color);
	        a = 255;  // COLORREF doesn't have alpha
	    }
		
		Hue(int r1,int g1,int b1,int a1=255)
		{
			r = r1;
			g = g1;
			b = b1;
			a = a1;
		}
		
		
		operator Gdiplus::Color() const
		{
			return Gdiplus::Color(a,r,g,b); 
		}
		
		operator COLORREF() const
		{
			return RGB(r,g,b);
		}

		bool operator==(const Hue& other) const 
		{
			return r == other.r && g == other.g && b == other.b && a == other.a;
		}
		bool operator!=(const Hue& other) const 
		{ 
			return !(*this == other); 
		}		
		
	};
	
	const Hue Hue::red = Hue(255, 0, 0, 255);
	const Hue Hue::green = Hue(0, 255, 0, 255);
	const Hue Hue::blue = Hue(0, 0, 255, 255);
	const Hue Hue::orange = Hue(255, 150, 0, 255);
	const Hue Hue::brown = Hue(150,100, 50, 255);
	const Hue Hue::yellow = Hue(255, 255, 0, 255);
	const Hue Hue::cyan = Hue(0, 255, 255, 255);
	const Hue Hue::purple = Hue(140, 0, 255, 255);
	const Hue Hue::pink = Hue(255, 0, 255, 255);
	const Hue Hue::grey = Hue(150, 150, 150, 255);
	const Hue Hue::black = Hue(0, 0, 0, 255);
	const Hue Hue::white = Hue(255, 255, 255, 255);
	const Hue Hue::transparent = Hue(0,0,0,0);	
	

	//============VIEW==============
	class View
	{
		private:
		
		float rotation = 0.0f;
		ws::IntRect port = {0,0,0,0}; //Port is always in screen coordinates.
		ws::IntRect world = {0,0,0,0}; //World is the world coordinate section of the world that is sent to the view.
		ws::Vec2i portOrigin = {0,0};//This is the point of rotation. It does NOT effect the view position.
		Gdiplus::Matrix matrix;
		float zoom = 0;		


		public:
		
		
		View()
		{
			
		}
		
		
		//Matrix does not have normal copy constructor so the view class has to do this in a custom way so that View can be copied to another view like view = v;
		View(const View& other) : 
			rotation(other.rotation),
			port(other.port),
			world(other.world),
			portOrigin(other.portOrigin),
			matrix(),
			zoom(other.zoom)
		{
			setTransform(other.matrix);
		}
	    
	  
		//move constructor
		
		View(View&& other) noexcept :
	        rotation(other.rotation),
	        port(std::move(other.port)),
	        world(std::move(other.world)),
	        portOrigin(std::move(other.portOrigin)),
	        matrix(),
	        zoom(other.zoom)
	    {
	    	setTransform(other.matrix);
	        other.rotation = 0.0f;
	        other.zoom = 0.0f;
	    }	  
	  
	  
	    
		View& operator=(const View& other)
		{
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
		
		
	    View& operator=(View&& other) noexcept
	    {
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
		
		
		~View() = default;
		
		

		void init(int portLeft,int portTop,int portWidth,int portHeight)
		{
			port.left = portLeft;
			port.top = portTop;
			port.width = portWidth;
			port.height = portHeight;
			
			world = port;
		}
		

		void init(ws::IntRect rect)
		{	
			init(rect.left,rect.top,rect.width,rect.height);
		}
		
		
		
		[[nodiscard]] ws::IntRect getRect()
		{
			return world;
		}
		
		void setRect(ws::IntRect rect)
		{
			world = rect;
		}

		void setRect(int left,int top,int width,int height)
		{
			setRect(ws::IntRect(left,top,width,height));
		}
		
		
		[[nodiscard]] ws::IntRect getPortRect()
		{
			return port;
		}
		
		void setPortRect(ws::IntRect rect)
		{
			port = rect;
		}
		
		void setPortRect(int left,int top,int width,int height)
		{
			setPortRect(ws::IntRect(left,top,width,height));
		}
		
		
		void setSize(ws::Vec2i size)
		{
			world.width = size.x;
			world.height = size.y;
		}
		
		[[nodiscard]] ws::Vec2i getSize()
		{
			return ws::Vec2i(world.width,world.height);
		}
		
		void setPortSize(ws::Vec2i size)
		{
			port.width = size.x;
			port.height = size.y;
		}
		
		[[nodiscard]] ws::Vec2i getPortSize()
		{
			return ws::Vec2i(port.width,port.height);
		}
		
		
		
		[[nodiscard]] ws::Vec2i getCenter()
		{
			return ws::Vec2i(world.left + (world.width/2),world.top + (world.height/2));
		}

		void setCenter(int cx,int cy)
		{
			world.left = cx - (world.width/2);
			world.top = cy - (world.height/2);
		}

		void setCenter(ws::Vec2i pos)
		{
			setCenter(pos.x,pos.y);
		}


		
		[[nodiscard]] ws::Vec2i getPortCenter()
		{
			return ws::Vec2i(port.left + (port.width/2),port.top + (port.height/2));
		}

		void setPortCenter(int cx,int cy)
		{
			ws::Vec2i pos = ws::Vec2i(cx - (port.width/2),cy - (port.height/2));
			port.left = pos.x;
			port.top = pos.y;
		}


		void setPortCenter(ws::Vec2i pos)
		{
			setPortCenter(pos.x,pos.y);
		}
		
		
		
		void setPortRotatePoint(int ox,int oy)
		{
			portOrigin.x = ox;
			portOrigin.y = oy;
		}
		
		void setPortRotatePoint(ws::Vec2i pos)
		{
			setPortRotatePoint(pos.x,pos.y);
		}
		
		
		void setPortRotatePointCenter()
		{
			portOrigin = ws::Vec2i(port.left + (port.width/2), port.top + (port.height/2));
		}
		
		
		
		[[nodiscard]] float getRotation() 
		{
		    return rotation;
		}
		
		
		void setRotation(float angle) 
		{
		    rotation = angle;
		}
		
		
		
		void setZoom(float val)
		{
			zoom = val;
		}
		
		
		[[nodiscard]] float getZoom()
		{
			return zoom;
		}
		
		
		void move(float dx,float dy)
		{
			world.left += static_cast<int>(dx);
			world.top += static_cast<int>(dy);
		}
		
		void move(ws::Vec2f dir)
		{
			move(dir.x,dir.y);
		}

		void move(int dx,int dy)
		{
			world.left += dx;
			world.top += dy;
		}		
		
	    void getTransform(Gdiplus::Matrix &m) const
	    {
	        Gdiplus::REAL elements[6];
	        matrix.GetElements(elements);
	        m.SetElements(elements[0], elements[1], elements[2], 
	                     elements[3], elements[4], elements[5]);
	    }
		
		void setTransform(const Gdiplus::Matrix &m)
	    {
	        Gdiplus::REAL elements[6];
	        m.GetElements(elements);
	        matrix.SetElements(elements[0], elements[1], elements[2], 
	                          elements[3], elements[4], elements[5]);
	        
	    }
		
		
		[[nodiscard]] ws::Vec2i toWorld(ws::Vec2i screenPos, ws::Vec2i screenSize) 
		{
			// First account for window stretching
			ws::Vec2i stretchedPos;
			stretchedPos.x = static_cast<int>(static_cast<float>(screenPos.x) * 
											 (static_cast<float>(world.width) / static_cast<float>(screenSize.x)));
			stretchedPos.y = static_cast<int>(static_cast<float>(screenPos.y) * 
											 (static_cast<float>(world.height) / static_cast<float>(screenSize.y)));
			
			// Calculate the visible world center
			float visibleWorldCenterX = static_cast<float>(world.left) + world.width / 2.0f;
			float visibleWorldCenterY = static_cast<float>(world.top) + world.height / 2.0f;
			
			// Calculate the port center
			float portCenterX = static_cast<float>(port.left) + port.width / 2.0f;
			float portCenterY = static_cast<float>(port.top) + port.height / 2.0f;
			
			// Calculate scale to fit world into port
			float scaleX = static_cast<float>(port.width) / world.width;
			float scaleY = static_cast<float>(port.height) / world.height;
			
			// Apply zoom
			float zoomFactor = std::pow(2.0f, zoom);
			scaleX *= zoomFactor;
			scaleY *= zoomFactor;
			
			// Apply inverse transformation
			float worldX = static_cast<float>(stretchedPos.x);
			float worldY = static_cast<float>(stretchedPos.y);
			
			// Reverse transformations in opposite order
			worldX -= portCenterX;
			worldY -= portCenterY;
			
			if (rotation != 0) {
				Gdiplus::Matrix rotMatrix;
				rotMatrix.Rotate(-rotation);
				Gdiplus::PointF point(worldX, worldY);
				rotMatrix.TransformPoints(&point, 1);
				worldX = point.X;
				worldY = point.Y;
			}
			
			worldX /= scaleX;
			worldY /= scaleY;
			
			worldX += visibleWorldCenterX;
			worldY += visibleWorldCenterY;
			
			return ws::Vec2i(static_cast<int>(worldX), static_cast<int>(worldY));
		}
	    
	    [[nodiscard]] ws::Vec2i toWorld(int x,int y,ws::Vec2i screenSize) 
	    {
	        return toWorld(ws::Vec2i(x,y),screenSize);
	    }
	    
	
	    [[nodiscard]] ws::Vec2i toScreen(ws::Vec2i worldPos,ws::Vec2i screenSize) 
	    {
		        
	        // Apply the transformation (scaled by zoom)
	        float zoomFactor = std::pow(2.0f, zoom);
	        
	        // Calculate the visible world center
	        float visibleWorldCenterX = static_cast<float>(world.left) + world.width / 2.0f;
	        float visibleWorldCenterY = static_cast<float>(world.top) + world.height / 2.0f;
	        
	        // Calculate the port center
	        float portCenterX = static_cast<float>(port.left) + port.width / 2.0f;
	        float portCenterY = static_cast<float>(port.top) + port.height / 2.0f;
	        
	        // Calculate scale to fit visible world into port
	        float visibleWorldWidth = static_cast<float>(world.width) / zoomFactor;
	        float visibleWorldHeight = static_cast<float>(world.height) / zoomFactor;
	        
	        float scaleX = static_cast<float>(port.width) / visibleWorldWidth;
	        float scaleY = static_cast<float>(port.height) / visibleWorldHeight;
	        
	        // Apply transformation
	        float screenX = static_cast<float>(worldPos.x);
	        float screenY = static_cast<float>(worldPos.y);
	        
	        // 1. Translate world center to origin
	        screenX -= visibleWorldCenterX;
	        screenY -= visibleWorldCenterY;
	        
	        // 2. Apply scale
	        screenX *= scaleX;
	        screenY *= scaleY;
	        
	        // 3. Apply rotation
	        if (rotation != 0) {
	            Gdiplus::Matrix rotMatrix;
	            rotMatrix.Rotate(rotation);
	            Gdiplus::PointF point(screenX, screenY);
	            rotMatrix.TransformPoints(&point, 1);
	            screenX = point.X;
	            screenY = point.Y;
	        }
	        
	        // 4. Translate to port center
	        screenX += portCenterX;
	        screenY += portCenterY;
	        
	        // Now account for window stretching
	        screenX *= (static_cast<float>(screenSize.x) / static_cast<float>(world.width));
	        screenY *= (static_cast<float>(screenSize.y) / static_cast<float>(world.height));
	        
	        return ws::Vec2i(static_cast<int>(screenX), static_cast<int>(screenY));
	    }
	
	    
	    [[nodiscard]] ws::Vec2i toScreen(int x,int y,ws::Vec2i screenSize) 
	    {
	        return toScreen(ws::Vec2i(x,y),screenSize);
	    }       		

		
		void apply(Gdiplus::Graphics &graphics)
		{
			updateMatrix();
		    
			graphics.SetClip(Gdiplus::Rect(port.left, port.top, port.width, port.height));

			
		    graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
		    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		    graphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
		    graphics.SetTransform(&matrix);
		}
			
		private:
			
			
		void updateMatrix()			
		{
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
		
	};

	//==========TEXTURE===========
	class Texture
	{
		private:
		int width = 0;
		int height = 0;

		private:
			HDC     m_hdcMem  = nullptr;
			HBITMAP m_hDIB    = nullptr;
			HBITMAP m_hOldBmp = nullptr;
			void*   m_dibBits = nullptr;
			bool    m_isFast  = false;

		public:

		
		Gdiplus::Bitmap* bitmap;
		
		
		Texture() : bitmap(nullptr) {}
		
		
		Texture(std::string path)
		{
			loadFromFile(path);
		}

		private:

		//this function destroys the DIB before destroying any GDI variables it references. If the DIB is not referencing a GDI regular memory section then this function will not remove the DIB section and normal cleanup will occur in the destructor.
		void destroyDIB()
		{
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
				m_hDIB    = nullptr;
				m_dibBits = nullptr;
			}

			if (m_hdcMem) {
				DeleteDC(m_hdcMem);
				m_hdcMem = nullptr;
			}

			m_isFast = false;
		}		

		void copyFrom(const Texture& other)
		{
			if(!other.bitmap || other.width <= 0 || other.height <= 0)
				return;

			if(other.m_isFast)
			{
				if(!create(other.width, other.height))
					return;

				if(m_dibBits && other.m_dibBits)
				{
					memcpy(m_dibBits, other.m_dibBits, other.width * other.height * 4);
				}
			}
			else
			{
				bitmap = other.bitmap->Clone(
					0, 0, other.width, other.height,
					PixelFormat32bppARGB
				);
				if(bitmap && bitmap->GetLastStatus() == Gdiplus::Ok)
				{
					width  = other.width;
					height = other.height;
				}
				else
				{
					delete bitmap;
					bitmap = nullptr;
					width  = height = 0;
				}
			}
		}

		public:
		
	    ~Texture()
	    {
			destroyDIB();
			
			//this will only cleanup if destroyDIB hasn't already cleanedup and made bitmap null.
			if (bitmap) {
				delete bitmap;
				bitmap = nullptr;
			}

			width  = 0;
			height = 0;
	    }		
		




		// Copy constructor
		Texture(const Texture& other)
			: width(0), height(0), bitmap(nullptr),
			  m_hdcMem(nullptr), m_hDIB(nullptr),
			  m_hOldBmp(nullptr), m_dibBits(nullptr),
			  m_isFast(false)
		{
			copyFrom(other);
		}

		// Copy assignment
		Texture& operator=(const Texture& other)
		{
			if (this != &other)
			{
				destroyDIB();
				if (bitmap) { delete bitmap; bitmap = nullptr; }
				width  = 0;
				height = 0;
				copyFrom(other);
			}
			return *this;
		}

		
		//move construct
		Texture(Texture&& other) noexcept
			: width(other.width),
			  height(other.height),
			  bitmap(other.bitmap),
			  m_hdcMem(other.m_hdcMem),
			  m_hDIB(other.m_hDIB),
			  m_hOldBmp(other.m_hOldBmp),
			  m_dibBits(other.m_dibBits),
			  m_isFast(other.m_isFast)
		{
			// Null out the source so its destructor does nothing
			other.bitmap    = nullptr;
			other.m_hdcMem  = nullptr;
			other.m_hDIB    = nullptr;
			other.m_hOldBmp = nullptr;
			other.m_dibBits = nullptr;
			other.m_isFast  = false;
			other.width     = 0;
			other.height    = 0;
		}

		// Move assign
		Texture& operator=(Texture&& other) noexcept
		{
			if (this != &other)
			{
				destroyDIB();
				if (bitmap) { delete bitmap; bitmap = nullptr; }

				width       = other.width;
				height      = other.height;
				bitmap      = other.bitmap;
				m_hdcMem    = other.m_hdcMem;
				m_hDIB      = other.m_hDIB;
				m_hOldBmp   = other.m_hOldBmp;
				m_dibBits   = other.m_dibBits;
				m_isFast    = other.m_isFast;

				// null out the source
				other.bitmap    = nullptr;
				other.m_hdcMem  = nullptr;
				other.m_hDIB    = nullptr;
				other.m_hOldBmp = nullptr;
				other.m_dibBits = nullptr;
				other.m_isFast  = false;
				other.width     = 0;
				other.height    = 0;
			}
			return *this;
		}
		

		//create a dibsection so that the GDI+ bitmap actually points to that dib section. With a dibsection as the memory basis, ws::Textures can be BitBlt using GDI.
		bool create(int w, int h, Gdiplus::Color color = Gdiplus::Color(0,0,0,0))
		{
			destroyDIB();
			if (bitmap) { delete bitmap; bitmap = nullptr; }

			width  = w;
			height = h;

			BITMAPINFO bmi              = {};
			bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth       =  w;
			bmi.bmiHeader.biHeight      = -h;
			bmi.bmiHeader.biPlanes      = 1;
			bmi.bmiHeader.biBitCount    = 32;
			bmi.bmiHeader.biCompression = BI_RGB;

			HDC hdcScreen = GetDC(nullptr);
			m_hDIB = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &m_dibBits, nullptr, 0);
			ReleaseDC(nullptr, hdcScreen);

			if (!m_hDIB) {
				std::cerr << "Texture::create failed to create DIBSection\n";
				return false;
			}

			m_hdcMem  = CreateCompatibleDC(nullptr);
			m_hOldBmp = (HBITMAP)SelectObject(m_hdcMem, m_hDIB);

			bitmap = new Gdiplus::Bitmap(w, h, w * 4, PixelFormat32bppARGB, (BYTE*)m_dibBits);

			if (!bitmap || bitmap->GetLastStatus() != Gdiplus::Ok) {
				std::cerr << "Texture::create failed to create GDI+ wrapper\n";
				destroyDIB();
				return false;
			}

			Gdiplus::Graphics g(m_hdcMem);
			g.Clear(color);

			m_isFast = true;
			return true;
		}
	
	
	
		Gdiplus::Bitmap* getHandle()
		{
			return bitmap;
		}
		
		HDC getHDC() const
		{
			return m_hdcMem;
		}
		
		HBITMAP getDIB() const
		{
			return m_hDIB;
		}
		
		HBITMAP getOldBMP() const
		{
			return m_hOldBmp;
		}
		
		void* getBITS() const
		{
			return m_dibBits;
		}
		
		bool isFastDIB() const
		{
			return m_isFast;
		}	
		
		
		bool loadFromFile(std::string path)
		{
			if (!ResolveRelativePath(path))
				return false;

			// Load into a temporary GDI+ bitmap
			std::wstring wpath = WIDE(path);
			Gdiplus::Bitmap* temp = Gdiplus::Bitmap::FromFile(wpath.c_str());

			if (!temp || temp->GetLastStatus() != Gdiplus::Ok)
			{
				std::cerr << "Failed to load image at " << std::quoted(path) << ".\n";
				if (temp) { delete temp; }
				return false;
			}

			int w = temp->GetWidth();
			int h = temp->GetHeight();

			// Create DIBSection of the same size
			if (!create(w, h))
			{
				delete temp;
				return false;
			}

			// Draw temp into the DIBSection via GDI+
			Gdiplus::Graphics g(m_hdcMem);
			g.DrawImage(temp, 0, 0, w, h);

			delete temp;
			return true;
		}


		bool loadFromMemory(const void* buffer, size_t bufferSize)
		{
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

			if (!temp || temp->GetLastStatus() != Gdiplus::Ok)
			{
				if (temp) { delete temp; }
				return false;
			}

			int w = temp->GetWidth();
			int h = temp->GetHeight();

			// Create DIBSection of the same size
			if (!create(w, h))
			{
				delete temp;
				return false;
			}

			// Draw temp into the DIBSection via GDI+
			Gdiplus::Graphics g(m_hdcMem);
			g.DrawImage(temp, 0, 0, w, h);

			delete temp;
			return true;
		}


		bool loadFromBitmapPlus(Gdiplus::Bitmap& src)
		{
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
		
		
		
		
		


		
	    bool isValid() const
	    {
	        return bitmap != nullptr;
	    }		
	    
	    
	    
	    
	    
	    
	    
	    
		void setPixel(int index, ws::Hue color)
		{
			int x = index % width;
			int y = index / width;
			setPixel(x, y, color);
		}

		void setPixel(int xIndex, int yIndex, ws::Hue color)
		{
			if (xIndex < 0 || xIndex >= width || yIndex < 0 || yIndex >= height)
				return;

			if (m_isFast && m_dibBits)
			{
				// direct DIB access. the pixel format is 32bit BGRA
				BYTE* pixel = static_cast<BYTE*>(m_dibBits) + (yIndex * width + xIndex) * 4;
				pixel[0] = static_cast<BYTE>(color.b); // Blue
				pixel[1] = static_cast<BYTE>(color.g); // Green
				pixel[2] = static_cast<BYTE>(color.r); // Red
				pixel[3] = static_cast<BYTE>(color.a); // Alpha
			}
			else if (bitmap)
			{
				// fallback to GDI+ if this texture does not use the GDI regular DIB method.
				bitmap->SetPixel(xIndex, yIndex, color);
			}
		}
	    
	    
		ws::Hue getPixel(int index)
		{
			int x = index % width;
			int y = index / width;
			return getPixel(x, y);
		}

		ws::Hue getPixel(int xIndex, int yIndex)
		{
			if (xIndex < 0 || xIndex >= width || yIndex < 0 || yIndex >= height)
				return ws::Hue(0,0,0,0);   // return transparent black

			if (m_isFast && m_dibBits)
			{
				BYTE* pixel = static_cast<BYTE*>(m_dibBits) + (yIndex * width + xIndex) * 4;
				return ws::Hue(
				pixel[2],   // Red
				pixel[1],   // Green
				pixel[0],   // Blue
				pixel[3]);  // Alpha
			}
			else if (bitmap)
			{
				//fallback if not a fast DIB.
				Gdiplus::Color color;
				bitmap->GetPixel(xIndex, yIndex, &color);
				return color.GetValue();
			}

			return ws::Hue(0,0,0,0);
		}
	    
	    
	    
	    
	    ws::Vec2i getSize() const 
		{
			return ws::Vec2i(width,height); 
		}
		
		
		bool saveToFile(std::string path)
		{
		    if (!bitmap || width <= 0 || height <= 0)
		    {
		        std::cerr << "Cannot save: Invalid bitmap\n";
		        return false;
		    }
		
		    if (!ResolveRelativePath(path))
		    {
		        std::cerr << "Failed to resolve path: " << path << "\n";
		        return false;
		    }
		    
		    
		
		    
		    // determine the encoder based on the file extension
		    CLSID encoderClsid;
		    std::string ext = path.substr(path.find_last_of(".") + 1);
		    
		    // lowercase extension for compare
		    std::transform(ext.begin(), ext.end(), ext.begin(), [](wint_t c) { return static_cast<char>(::towlower(c)); });
		    
		    // Get encoder CLSID based on file extension
		    if (ext == "png")
		    {
		        GetEncoderClsid(L"image/png", &encoderClsid);
		    }
		    else if (ext == "jpg" || ext == "jpeg")
		    {
		        GetEncoderClsid(L"image/jpeg", &encoderClsid);
		    }
		    else if (ext == "bmp")
		    {
		        GetEncoderClsid(L"image/bmp", &encoderClsid);
		    }
		    else if (ext == "gif")
		    {
		        GetEncoderClsid(L"image/gif", &encoderClsid);
		    }
		    else if (ext == "tiff")
		    {
		        GetEncoderClsid(L"image/tiff", &encoderClsid);
		    }
		    else
		    {
		        // default to PNG if extension not recognized
		        std::cerr << "Unsupported format. Using PNG.\n";
		        GetEncoderClsid(L"image/png", &encoderClsid);
		        path += ".png"; // Add extension
		    }
		
		    // save the image
		    Gdiplus::Status status = bitmap->Save(ws::WIDE(path).c_str(), &encoderClsid, NULL);
		    
		    if (status != Gdiplus::Ok)
		    {
			    std::cerr << "Failed to save image to: " << path << "\n";
				return false;
			}
			return true;
		}
		
	    	
	};

	//==========FONT===========
	class Font
	{

		
		public:
		
		
		
		
		Font()
		{
			family = std::make_unique<Gdiplus::FontFamily>(L"Arial");
			fontCollection = std::make_unique<Gdiplus::PrivateFontCollection>();
			loadFromSystem("Arial");
		}
		
		~Font()
		{
			gdiFont.reset();
		    family.reset();
		    fontCollection.reset();
		}
		
		
		
		Gdiplus::Font* getFontHandle()
		{ return gdiFont.get(); }
		
		Gdiplus::FontFamily* getFamilyHandle()
		{ return family.get(); }
		

		
		
		bool isSystemFont()
		{ return isCustomFont; }
		
		std::string getFilePath()
		{ return fontFilePath; }
		
		
		std::string getName()
		{ return fontName; }
		
		

		
		bool loadFromSystem(std::string name)
		{
			fontFilePath.clear();
			isCustomFont = false;
			
			fontCollection.reset(new Gdiplus::PrivateFontCollection());
			
			fontName = name;
			
			return update();
			
		}
		
		
		bool loadFromFile(std::string path)
		{	
			//reset the font collection to be empty and then add a single font.
			fontFilePath = path;
	        fontCollection.reset(new Gdiplus::PrivateFontCollection());
	        Gdiplus::Status status = fontCollection->AddFontFile(ws::WIDE(fontFilePath).c_str());
	        
	        if(status != Gdiplus::Ok)
	        	return false;
	        
	        //check if the family exists(a family is a font but it is called a family because a font has different versions(bold,italic,etc.))
			int familyCount = fontCollection->GetFamilyCount();	
	        
			if(familyCount <= 0)//Means the family failed to be created.
				return false;
			
			
			//Get the first font family from the collection. (The only one since this is a collection per ws::Font object now).
			foundFamily = 0;
			fontCollection->GetFamilies(1,family.get(),&foundFamily);
			
			
			if(foundFamily == 0)
				return false;
			
			//Get the overall font name. Ignore subnames like (Arial-Bold,Arial-Italic,etc.)
			wchar_t familyName[LF_FACESIZE]; //LF_FACESIZE == 32
			
			if(family->GetFamilyName(familyName) != Gdiplus::Ok)
				return false;
			
			
			std::string name = ws::SHORT(familyName);	
			
			//Remove /0 null terminator from name
			if (!name.empty() && name.back() == '\0')
	        {
	            name.pop_back();
	        }
			
			//Store the font name
			fontName = name;
			fontFilePath = path;
			isCustomFont = true;
			
			//initial update
			return update();
		}
		
		
		
		bool isValid()
	    {
	        return gdiFont && gdiFont->GetLastStatus() == Gdiplus::Ok;
	    }

		
		
		private:
			
		bool update()
		{

			
			if(isCustomFont)
			{
			
	            if(!fontCollection || fontCollection->GetFamilyCount() <= 0)
	                return false;			
			
				if(foundFamily == 0)
					return false;
				
				//Create the font
				gdiFont.reset(new Gdiplus::Font(family.get(),24,Gdiplus::FontStyleRegular,Gdiplus::UnitPixel));
				
			}
			else
			{
				
				gdiFont.reset(new Gdiplus::Font(ws::WIDE(fontName).c_str(), 
	            24, 
	            Gdiplus::FontStyleRegular, 
	            Gdiplus::UnitPixel));
			}
			
			
			return gdiFont->GetLastStatus() == Gdiplus::Ok;			
		}	
			
		
		std::string fontName = "Arial";
	    std::unique_ptr<Gdiplus::PrivateFontCollection> fontCollection;
	    std::unique_ptr<Gdiplus::Font> gdiFont;
		std::unique_ptr<Gdiplus::FontFamily> family;	
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
	    



	    ws::Vec2i getSize() {return ws::Vec2i(width,height);}
	    ws::Vec2f getPosition() {return ws::Vec2f(x,y);}
	    ws::Vec2f getScale() { return scale; }
	    ws::Vec2i getOrigin() { return origin; }
	    float getRotation() { return rotation; }
			    
	    
	    void setSize(ws::Vec2i size) {width = size.x;height = size.y;}
	    void setSize(int w,int h) {width = w;height = h;}
        void setPosition(float xpos, float ypos) { x = xpos; y = ypos; }
        void setPosition(ws::Vec2f pos) { x = pos.x; y = pos.y; }
	    void setScale(ws::Vec2f s) { scale = s; }
	    void setScale(float sx, float sy) { scale.x = sx; scale.y = sy; }
	    void setOrigin(ws::Vec2i pos) { origin = pos; }
	    void setOrigin(int posx, int posy) { origin.x = posx; origin.y = posy; }
	    void setRotation(float degrees) { rotation = degrees; }

        void move(float dx, float dy) { x += dx; y += dy; }
        void move(const ws::Vec2f& delta) { x += delta.x; y += delta.y; }
	    
	    int getVisualWidth() const {
	        return static_cast<int>(std::abs(width * scale.x));
	    }
	    
	    int getVisualHeight() const {
	        return static_cast<int>(std::abs(height * scale.y));
	    }
	    
	    //Visual bounds
	    void getBounds(int& left, int& top, int& right, int& bottom) const
	    {
	    	
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
	    
	    //Visually contains
		virtual bool contains(ws::Vec2i point)
	    {
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
	    
	    
		virtual void drawGlobal(Gdiplus::Graphics* graphics)
	    {
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
	    
	    // Pure virtual - draw the content in local space
	    virtual void draw(Gdiplus::Graphics* graphics) = 0;
	    
	    virtual ~Drawable() = default;
	};

	//============SPRITE=============
	class Sprite : public Drawable
	{
		
		
		private:
			
	    ws::Texture* textureRef = nullptr;
	    int texLeft = 0, texTop = 0;  // Texture coordinates
	    int texWidth = 0, texHeight = 0;  // Texture dimensions			
		
		
		public:
		
		
		
		Sprite()
		{
			
		}
		
		
		Sprite(ws::Texture &texture)
		{
			textureRef = &texture;
			setTextureRect({0,0,texture.getSize().x,texture.getSize().y});
		}
		
		
	    virtual bool contains(ws::Vec2i pos) override
	    {
	        int left, top, right, bottom;
	        getBounds(left, top, right, bottom);
	        
	        return (pos.x >= left && pos.x < right &&
	                pos.y >= top && pos.y < bottom);
	    }
	    virtual void draw(Gdiplus::Graphics* graphics) override
	    {
	        if (!textureRef || !textureRef->isValid()) 
	            return;
	        
	        Gdiplus::Rect destRect(0,0, width, height);
	        Gdiplus::Rect srcRect(texLeft, texTop, texWidth, texHeight);
	        
	        graphics->DrawImage(textureRef->bitmap, destRect,
	                           srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height,
	                           Gdiplus::UnitPixel);
	    }	    

		
		
	    void setTexture(ws::Texture& texture,bool resize = true) {
	        textureRef = &texture;
	        if(resize)
	        	setTextureRect({0,0,texture.getSize().x,texture.getSize().y});
	    }
	    
	    void setTextureRect(ws::IntRect rect) {
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
	    
	    ws::IntRect getTextureRect() const {
	        return {texLeft, texTop, texWidth, texHeight};
	    }
		
		Texture &getTexture()
		{
			return *textureRef;
		}
		
		
	    const Texture* getTexture() const
	    {
	        return textureRef;
	    }		
		
		
	    bool hasTexture() const
	    {
	        return textureRef != nullptr;
	    }		
		
		

		
		
		public:
			friend class Window;	
	};
	
	
	//==========LINE=============
	class Line : public ws::Drawable 
	{
	
		public:
		
		ws::Vec2i start;
		ws::Vec2i end;
		Gdiplus::Color color = {255,0,0,255};
		
	    
	    
	    
	    Line(ws::Vec2i start = {0,0},ws::Vec2i end = {0,0},int thewidth = 2,Gdiplus::Color color = {255,0,0,255})
	    {
	    	this->start = start;
	    	this->end = end;
	    	width = thewidth;
	    	this->color = color;
		}
	    
	    
		virtual void draw(Gdiplus::Graphics* canvas) override 
		{
	    	Gdiplus::Pen pen(color, static_cast<Gdiplus::REAL>(width));
			canvas->DrawLine(&pen, static_cast<Gdiplus::REAL>(start.x), static_cast<Gdiplus::REAL>(start.y), static_cast<Gdiplus::REAL>(end.x), static_cast<Gdiplus::REAL>(end.y));
	    }
	    
	    
	    private:
	    virtual bool contains(ws::Vec2i pos) override
	    { 
	    	return false;
		}
		
		
		public:
			
		bool onSegment(ws::Vec2i p, ws::Vec2i q, ws::Vec2i r)
	    {
	        if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
	            q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
	            return true;
	        return false;
	    }
		
		// Returns: 0 = collinear, 1 = clockwise, 2 = counterclockwise
	    int orientation(ws::Vec2i p, ws::Vec2i q, ws::Vec2i r)
	    {
	        long long val = (long long)(q.y - p.y) * (r.x - q.x) - 
	                       (long long)(q.x - p.x) * (r.y - q.y);
	        
	        if (val == 0) return 0;  // Collinear
	        return (val > 0) ? 1 : 2; // Clockwise or counterclockwise
	    }			
		
	    bool intersects(Line &otherLine)
	    {
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
	    
	};	

	//==============POLY==================
	class Poly : public ws::Drawable 
	{
	    public:
	    std::vector<ws::Vec2i> vertices;
	    
	    
	    private:
	    Gdiplus::Color fillColor = {255,255,0,0};    
	    Gdiplus::Color borderColor = {255,255,0,100};    
	    int borderWidth = 2;
	    bool filled = true;
	    bool closed = true;
	    
	    ws::Texture *textureRef = nullptr;
	    ws::Texture effectTexture;
	    bool textureNeedsUpdate = true;
	    
	    std::vector<ws::Vec2f> uvs;
	    bool hasUVs = false;
	    
	    public:
	        
	    Poly() = default;
	    
	    
	    Poly(std::vector<ws::Vec2i>& vertices, Gdiplus::Color fillColor = {255,255,0,0}, Gdiplus::Color borderColor = {255,255,0,255}, int borderWidth = 2, bool filled = true)
	    {
	        this->vertices = vertices;
	        this->fillColor = fillColor;
	        this->borderColor = borderColor;
	        this->borderWidth = borderWidth;
	        this->filled = filled;
	    }
	    
	    
	    void addVertex(ws::Vec2i vertex) 
	    {
	        vertices.push_back(vertex);
	        uvs.push_back(ws::Vec2f(0, 0));
	        textureNeedsUpdate = true;
	    }
	    
	    void addVertex(int x, int y) 
	    {
	        vertices.push_back({x, y});
	        uvs.push_back(ws::Vec2f(0, 0));
	        textureNeedsUpdate = true;
	    }
	    
	    void clear() 
	    {
	        vertices.clear();
	        uvs.clear();
	        hasUVs = false;
	        textureNeedsUpdate = true;
	    }
	    
	    size_t vertexCount() 
	    {
	        return vertices.size();
	    }
	    
	    
	    bool isValid() 
	    {
	        return vertices.size() >= 3;
	    }
	    
	    
	    ws::Vec2i getCentroid() 
	    {
	        if (vertices.empty()) return {0, 0};
	        
	        long long sumX = 0, sumY = 0;
	        for (const auto& vertex : vertices) {
	            sumX += vertex.x;
	            sumY += vertex.y;
	        }
	        
	        return {static_cast<int>(sumX / vertices.size()), 
	                static_cast<int>(sumY / vertices.size())};
	    }
	    
	    
	    virtual bool contains(ws::Vec2i point) override 
	    {
	        if (vertices.size() < 3) return false;
	        
	        int crossings = 0;
	        size_t n = vertices.size();
	        
	        for (size_t a = 0; a < n; a++) 
	        {
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
	    
	    
	    bool intersects(Line &line) 
	    {
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
	    
	    
	    bool intersects(Poly &other) 
	    {
	        for (const auto& vertex : other.vertices) 
	        {
	            if (contains(vertex)) 
	            {
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
	    
	    
	    ws::IntRect getBoundingRect() 
	    {
	        if (vertices.empty()) return {0, 0, 0, 0};
	        
	        ws::IntRect rect = {vertices[0].x, vertices[0].y, vertices[0].x, vertices[0].y};
	        
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
	    
	    
	    void setTexture(ws::Texture &tex)
	    {
	        textureRef = &tex;
	        textureNeedsUpdate = true;
	    }   
	    
	    void removeTexture()
	    {
	        textureRef = nullptr;
	    }
	    
	    ws::Texture* getTexture()
	    {
	        return textureRef;
	    }
	    
	    void setUV(size_t vertexIndex, float u, float v)
	    {
	        if(vertexIndex >= 0 && vertexIndex < vertices.size())
	        {
	            if(vertexIndex >= uvs.size())
	            {
	                uvs.resize(vertices.size(), ws::Vec2f(0, 0));
	            }
	            uvs[vertexIndex] = ws::Vec2f(u, v);
	            hasUVs = true;
	            textureNeedsUpdate = true;
	        }
	    }
	    
	    void updateTexture()
	    {
	        textureNeedsUpdate = true;
	    }
	    
	    void setFillColor(Gdiplus::Color color)    
	    {
	        fillColor = color;
	    }
	    
	    void setBorderColor(Gdiplus::Color color)
	    {
	        borderColor = color;
	    }
	    
	    Gdiplus::Color getFillColor()
	    {
	        return fillColor;
	    }
	    
	    Gdiplus::Color getBorderColor()
	    {
	        return borderColor;
	    }
	    
	    
	    void setBorderWidth(int w)
	    {
	        borderWidth = w;
	    }
	    
	    int getBorderWidth()
	    {
	        return borderWidth;
	    }
	    
	    void setFilled(bool b = true)
	    {
	        filled = b;
	    }
	    
	    void setClosed(bool b = true)
	    {
	        closed = b;
	    }
	    
	    bool getFilled()
	    {
	        return filled;
	    }
	    
	    bool getClosed()
	    {
	        return closed;
	    }
	    
	    
	    private:
	    
	    bool pointInPolygon(ws::Vec2i p) 
	    {
	        if (vertices.size() < 3) return false;
	        
	        int crossings = 0;
	        size_t n = vertices.size();
	        
	        for (size_t a = 0; a < n; a++) 
	        {
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
	    
	    void generateEffectTexture()
	    {
	        if(!textureRef || !textureRef->isValid()) return;
	        
	        ws::IntRect bounds = getBoundingRect();
	        if(bounds.width <= 0 || bounds.height <= 0) return;
	        
	        effectTexture.create(bounds.width, bounds.height, Gdiplus::Color(0, 0, 0, 0));
	        
	        int texWidth = textureRef->getSize().x;
	        int texHeight = textureRef->getSize().y;
	        
	        if(!hasUVs || uvs.size() != vertices.size())
	        {
	            uvs.clear();
	            for(const auto& vertex : vertices)
	            {
	                float u = static_cast<float>(vertex.x - bounds.left) / static_cast<float>(bounds.width);
	                float v = static_cast<float>(vertex.y - bounds.top) / static_cast<float>(bounds.height);
	                uvs.push_back(ws::Vec2f(u, v));
	            }
	            hasUVs = true;
	        }
	        
	        for(int y = 0; y < bounds.height; y++)
	        {
	            for(int x = 0; x < bounds.width; x++)
	            {
	                ws::Vec2i worldPoint(x + bounds.left, y + bounds.top);
	                
	                if(pointInPolygon(worldPoint))
	                {
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
	    
	    ws::Vec2f getUVForPoint(ws::Vec2i point, ws::IntRect bounds)
	    {
	        if(vertices.size() == 3)
	        {
	            return barycentricUV(point, vertices[0], vertices[1], vertices[2],
	                                uvs[0], uvs[1], uvs[2]);
	        }
	        
	        for(size_t i = 1; i < vertices.size() - 1; i++)
	        {
	            if(pointInTriangle(point, vertices[0], vertices[i], vertices[i + 1]))
	            {
	                return barycentricUV(point, vertices[0], vertices[i], vertices[i + 1],
	                                    uvs[0], uvs[i], uvs[i + 1]);
	            }
	        }
	        
	        return ws::Vec2f(0, 0);
	    }
	    
	    bool pointInTriangle(ws::Vec2i p, ws::Vec2i a, ws::Vec2i b, ws::Vec2i c)
	    {
	        float alpha = static_cast<float>(((b.y - c.y)*(p.x - c.x) + (c.x - b.x)*(p.y - c.y))) /
	                     static_cast<float>(((b.y - c.y)*(a.x - c.x) + (c.x - b.x)*(a.y - c.y)));
	        float beta = static_cast<float>(((c.y - a.y)*(p.x - c.x) + (a.x - c.x)*(p.y - c.y))) /
	                    static_cast<float>(((b.y - c.y)*(a.x - c.x) + (c.x - b.x)*(a.y - c.y)));
	        float gamma = 1.0f - alpha - beta;
	        
	        return (alpha >= 0 && beta >= 0 && gamma >= 0);
	    }
	    
	    ws::Vec2f barycentricUV(ws::Vec2i p, ws::Vec2i a, ws::Vec2i b, ws::Vec2i c,
	                           ws::Vec2f uvA, ws::Vec2f uvB, ws::Vec2f uvC)
	    {
	        float denom = static_cast<float>((b.y - c.y)*(a.x - c.x) + (c.x - b.x)*(a.y - c.y));
	        if(fabs(denom) < 0.0001f) return uvA;
	        
	        float alpha = static_cast<float>(((b.y - c.y)*(p.x - c.x) + (c.x - b.x)*(p.y - c.y))) / denom;
	        float beta = static_cast<float>(((c.y - a.y)*(p.x - c.x) + (a.x - c.x)*(p.y - c.y))) / denom;
	        float gamma = 1.0f - alpha - beta;
	        
	        float u = alpha * uvA.x + beta * uvB.x + gamma * uvC.x;
	        float v = alpha * uvA.y + beta * uvB.y + gamma * uvC.y;
	        
	        return ws::Vec2f(u, v);
	    }
	    
	    public:
	    virtual void draw(Gdiplus::Graphics* canvas) override 
	    {
	        if (vertices.size() < 2) return;
	        
	        if(!textureRef)
	        {
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
	        else
	        {
	            if(textureNeedsUpdate || !effectTexture.isValid())
	            {
	                generateEffectTexture();
	            }
	            
	            if(effectTexture.isValid())
	            {
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
	                
	                if(borderWidth > 0)
	                {
	                    Gdiplus::Pen borderPen(borderColor, static_cast<Gdiplus::REAL>(borderWidth));
	                    canvas->DrawPolygon(&borderPen, transformedPoints.data(), 
	                                      static_cast<INT>(transformedPoints.size()));
	                }
	            }
	        }
	    }
	};	
	
	//===============TEXT==================
	class Text : public ws::Drawable
	{
		public:
		
		
		
		
		Text(){}
		~Text(){}
		
		
		
		
		Text(ws::Font &newfont)
		{ fontRef = &newfont; }
		
		
		
		void setFont(ws::Font &newFont)
		{ fontRef = &newFont; }
		
		ws::Font* getFont()
		{ return fontRef; }
		
		void setString(std::string str)
		{ text = str; }
		
		std::string getString()
		{ return text; }
		
		
		void setCharacterSize(int size)
		{ charSize = size;}
		
		int getCharacterSize()
		{ return charSize; }	
		
		void setStyle(Gdiplus::FontStyle fontStyle)
		{ style = fontStyle;}
		
		Gdiplus::FontStyle getStyle()
		{ return style; }		
		
		void setFillColor(Gdiplus::Color color)
		{ fillColor = color; }
		
		Gdiplus::Color getFillColor()
		{ return fillColor; }
		
		void setBorderColor(Gdiplus::Color color)
		{ borderColor = color; }
		
		Gdiplus::Color getBorderColor()
		{ return borderColor; }
		
		void setBorderWidth(int w)
		{ borderWidth = w; }
		
		int getBorderWidth()
		{ return borderWidth; }
		
		
		

		
		
		
		virtual bool contains(ws::Vec2i pos) override
		{
			return Drawable::contains(pos);
		}
		
		virtual void draw(Gdiplus::Graphics* canvas) override 
		{
			
			Gdiplus::FontFamily &family = *fontRef->getFamilyHandle();
			
			if(!family.IsStyleAvailable(style))
            {
            	std::cerr << "Font style not available! Defaulting to whatever style can be found. If nothing is found, the text will not be displayed."<<std::endl;
                if(family.IsStyleAvailable(Gdiplus::FontStyleRegular))
                    style = Gdiplus::FontStyleRegular;
                else if(family.IsStyleAvailable(Gdiplus::FontStyleBold))
                    style = Gdiplus::FontStyleBold;
                else if(family.IsStyleAvailable(Gdiplus::FontStyleItalic))
                    style = Gdiplus::FontStyleItalic;
                else
                    return;
            }
			
			
			
			Gdiplus::GraphicsPath path;
			
		    Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
			format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsNoFitBlackBox | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
			
			path.AddString(
			ws::WIDE(text).c_str(), 
			static_cast<INT>(text.length()),
			fontRef->getFamilyHandle(), 
			style, 
			static_cast<Gdiplus::REAL>(charSize), 
			Gdiplus::PointF(0,0), 
			&format
			);
    
    		Gdiplus::Pen outlinePen(borderColor, static_cast<Gdiplus::REAL>(borderWidth));
    		outlinePen.SetLineJoin(Gdiplus::LineJoinRound);  
		    
		    Gdiplus::RectF bounds;
		    path.GetBounds(&bounds, NULL, &outlinePen);
		    
		    width = static_cast<int>(bounds.Width);
		    height = static_cast<int>(bounds.Height);
		    
		    
		    Gdiplus::SolidBrush fillBrush(fillColor);
    
    		//drawing the outline
    		canvas->DrawPath(&outlinePen, &path);
    		
    		//filling text
    		canvas->FillPath(&fillBrush, &path);
    		
		} 
		
		
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
		
		Radial()
		{
			poly.setFillColor(Gdiplus::Color(255,100,200,100));
			poly.setBorderColor(Gdiplus::Color(255,50,255,50));
			poly.setBorderWidth(2);
			poly.setClosed();
			poly.setFilled();
			make();
			origin.x = radius;
			origin.y = radius;
		}
		
		void make(int points = 8)
		{
			poly.clear();
			
			double inc = (2 * M_PI)/points; 
			
			for(double a=0;a<(2*M_PI);a+=inc)
			{
				double angle = a;
				int resx = static_cast<int>(std::cos(angle) * static_cast<double>(radius));
				int resy = static_cast<int>(std::sin(angle) * static_cast<double>(radius));
				poly.addVertex(resx + radius, resy + radius);
			}
			m_points = points;
			
			// Update Drawable properties
			width = 2 * radius;
			height = 2 * radius;

			// Update position based on center
			x = static_cast<float>(center.x - origin.x);
			y = static_cast<float>(center.y - origin.y);
		}
		
		
		
		void setPosition(int posx,int posy)
		{
			center = {posx,posy};
			x = static_cast<float>(center.x - origin.x);
			y = static_cast<float>(center.y - origin.y);
		}
		
		void setPosition(ws::Vec2i pos)
		{
			setPosition(pos.x, pos.y);
		}
		
		
		
		void move(ws::Vec2i delta)
		{
			center.x += delta.x;
			center.y += delta.y;
			x = static_cast<float>(center.x - origin.x);
			y = static_cast<float>(center.y - origin.y);
		}
		
		void move(int deltaX,int deltaY)
		{
			move(ws::Vec2i(deltaX, deltaY));
		}
		
		
		void setPointCount(int count)
		{
			m_points = count;
			make(m_points);
		}
		
		void setRadius(int size)
		{
			radius = size;
			make(m_points);
		}
		
		void setFillColor(Gdiplus::Color color)
		{
			poly.setFillColor(color); 
		}
		
		void setBorderColor(Gdiplus::Color color)
		{
			poly.setBorderColor(color);
		}
		
		void setBorderWidth(int size)
		{
			poly.setBorderWidth(size);
		}
		
		
		int getRadius()
		{
			return radius;
		}
		
		ws::Vec2i getPosition()
		{
			return center;
		}
		
		int getPointCount()
		{
			return m_points;
		}
		
		
		virtual void draw(Gdiplus::Graphics* canvas) override
		{
			poly.draw(canvas);
		}
		
		virtual bool contains(ws::Vec2i pos) override
		{
			// Convert to local coordinates
			float localX = static_cast<float>(pos.x - x - origin.x);
			float localY = static_cast<float>(pos.y - y - origin.y);
			
			// Reverse scale
			if (scale.x != 1.0f) localX /= scale.x;
			if (scale.y != 1.0f) localY /= scale.y;
			
			// Check if point is within circle
			return (localX * localX + localY * localY) <= static_cast<float>(radius * radius);
		}	
		
	
		private:
		ws::Vec2i center = {0, 0};
		int m_points = 500;
		int radius = 10;
	};

	//===============ROUND=============
	class Round : public ws::Drawable
	{
		public:
			
		
		virtual void draw(Gdiplus::Graphics* canvas) override
		{
	        Gdiplus::Pen borderPen(m_borderColor, static_cast<Gdiplus::REAL>(m_borderWidth));
	        Gdiplus::SolidBrush fillBrush(m_fillColor);
			
			canvas->DrawEllipse(&borderPen,0.0f,0.0f,static_cast<Gdiplus::REAL>(width),static_cast<Gdiplus::REAL>(height));
			canvas->FillEllipse(&fillBrush,0.0f,0.0f,static_cast<Gdiplus::REAL>(width),static_cast<Gdiplus::REAL>(height));
		}
		
		
		
		bool contains(int px,int py)
		{
			return contains(ws::Vec2i(px,py));
		}
		
		//STILL DOES NOT WORK!!!!
		virtual bool contains(ws::Vec2i p) override
		{
			
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
		
		
		
		void setBorderColor(Gdiplus::Color color){m_borderColor = color;}
		void setFillColor(Gdiplus::Color color){m_fillColor = color;}
		void setBorderWidth(int w){m_borderWidth = w;}
		Gdiplus::Color getBorderColor() {return m_borderColor;}
		Gdiplus::Color getFillColor() {return m_fillColor;}
		int getBorderWidth() {return m_borderWidth;}
		
		
		private:
			Gdiplus::Color m_borderColor = Gdiplus::Color(255,100,200,100);
			Gdiplus::Color m_fillColor = Gdiplus::Color(255,50,150,50);
			int m_borderWidth = 2;
	
		

	};
	
	

	
	//============== CURSOR ==============
	class Cursor
	{
		private:
		HCURSOR handle = nullptr;
		bool animated = false;
		std::wstring srcPath;		
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
		
		HCURSOR getHandle()
		{
			return handle;
		}

		
		Cursor()
		{}
		
		Cursor(Type type)
		{
			loadAs(type);
		}
		
		void loadAs(Type type)
		{
			animated = false;
			srcPath.clear();			
			if (handle)
				DestroyCursor(handle);  

			switch (type)
			{
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

		bool loadFromTexture(const ws::Texture& texture, int hotSpotX = 0, int hotSpotY = 0)
		{
			animated = false;
			srcPath.clear();			
			if (!texture.isValid()) return false;

			Gdiplus::Bitmap* gdipBitmap = texture.bitmap;
			UINT width = gdipBitmap->GetWidth();
			UINT height = gdipBitmap->GetHeight();

			// <><><> 32‑bpp colour DIB section 
			BITMAPINFO bmi = {};
			bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth       = width;
			bmi.bmiHeader.biHeight      = -(LONG)height;          // top‑down
			bmi.bmiHeader.biPlanes      = 1;
			bmi.bmiHeader.biBitCount    = 32;
			bmi.bmiHeader.biCompression = BI_RGB;

			void* colourBits = nullptr;
			HBITMAP hbmColor = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &colourBits, nullptr, 0);
			if (!hbmColor) return false;

			Gdiplus::BitmapData data;
			Gdiplus::Rect rect(0, 0, width, height);
			if (gdipBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data) != Gdiplus::Ok)
			{
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
			maskBmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
			maskBmi.bmiHeader.biWidth       = width;
			maskBmi.bmiHeader.biHeight      = -(LONG)height;
			maskBmi.bmiHeader.biPlanes      = 1;
			maskBmi.bmiHeader.biBitCount    = 1;
			maskBmi.bmiHeader.biCompression = BI_RGB;
			maskBmi.bmiHeader.biSizeImage   = (DWORD)maskBufferSize;

			void* maskBitsPtr = nullptr;
			HBITMAP hbmMask = CreateDIBSection(nullptr, &maskBmi, DIB_RGB_COLORS, &maskBitsPtr, nullptr, 0);
			if (!hbmMask)
			{
				DeleteObject(hbmColor);
				return false;
			}
			memcpy(maskBitsPtr, maskBits.data(), maskBufferSize);

			//<><> Create the cursor
			ICONINFO iconInfo = {};
			iconInfo.fIcon    = FALSE;          // cursor, not icon
			iconInfo.xHotspot = hotSpotX;
			iconInfo.yHotspot = hotSpotY;
			iconInfo.hbmMask  = hbmMask;
			iconInfo.hbmColor = hbmColor;

			HCURSOR newCursor = CreateIconIndirect(&iconInfo);

			// Clean up the temporary bitmaps
			DeleteObject(hbmColor);
			DeleteObject(hbmMask);

			if (newCursor)
			{
				if (handle) DestroyCursor(handle);
				handle = newCursor;
			}
			else
				return false;
			return true;
		}

		bool loadFromFile(const std::string& filename)
		{
			animated = false;
			srcPath.clear();			
			if (handle)
			{
				DestroyCursor(handle);
				handle = nullptr;
			}

			std::wstring wfilename = ws::WIDE(filename);

			// just detect it from the extension, no reason to make the caller do this
			bool isAni = wfilename.size() >= 4 &&
						 _wcsicmp(wfilename.c_str() + wfilename.size() - 4, L".ani") == 0;

			if (isAni) {
				handle = LoadCursorFromFileW(wfilename.c_str());
			} else {
				handle = (HCURSOR)LoadImageW(
					nullptr,
					wfilename.c_str(),
					IMAGE_CURSOR,
					0, 0,
					LR_LOADFROMFILE | LR_DEFAULTSIZE
				);
			}

			if (!handle) {
				std::cerr << "Failed to load cursor from file: " << filename << std::endl;
				animated = false;
				srcPath.clear();
				return false;
			}

			animated = isAni;
			srcPath  = isAni ? wfilename : std::wstring{};
			return true;
		}


		~Cursor()
		{
			if (handle)
				DestroyCursor(handle);
		}

		Cursor(const Cursor& other) : animated(other.animated), srcPath(other.srcPath)
		{
			if (other.handle)
			{
				if (other.animated && !other.srcPath.empty())
				{
					// CopyIcon loses animation frames, reload from file instead
					handle = LoadCursorFromFileW(other.srcPath.c_str());
					if (!handle)
						handle = (HCURSOR)CopyIcon((HICON)other.handle); // fallback just in case
				}
				else
				{
					handle = (HCURSOR)CopyIcon((HICON)other.handle);
				}
			}
		}

		Cursor& operator=(const Cursor& other)
		{
			if (this != &other)
			{
				if (handle) DestroyCursor(handle);
				animated = other.animated;
				srcPath  = other.srcPath;
				if (other.handle)
				{
					if (other.animated && !other.srcPath.empty())
					{
						handle = LoadCursorFromFileW(other.srcPath.c_str());
						if (!handle)
							handle = (HCURSOR)CopyIcon((HICON)other.handle);
					}
					else
					{
						handle = (HCURSOR)CopyIcon((HICON)other.handle);
					}
				}
				else
				{
					handle = nullptr;
				}
			}
			return *this;
		}

		Cursor(Cursor&& other) noexcept
			: handle(other.handle), animated(other.animated), srcPath(std::move(other.srcPath))
		{
			other.handle   = nullptr;
			other.animated = false;
		}

		Cursor& operator=(Cursor&& other) noexcept
		{
			if (this != &other)
			{
				if (handle) DestroyCursor(handle);
				handle         = other.handle;
				animated       = other.animated;
				srcPath        = std::move(other.srcPath);
				other.handle   = nullptr;
				other.animated = false;
			}
			return *this;
		}
	};	


	//=============== WINDOW ==============

	class WindowManager
	{
		public:
		static std::map<HWND, ws::Window*> windows;
		static std::mutex windowsMutex;
		static bool initialized;
		
		
		static void init(const std::string& className = "Window");		
		static LRESULT CALLBACK GlobalProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
		static void addWindow(ws::Window* window);
		static void removeWindow(HWND hwnd);		
		static Window* GetWindow(HWND hwnd);
		
	};

	class Window
	{
		public:
		
		HWND hwnd;	

		private:

		
		friend class WindowManager;
		
		bool isRunning = false;
		
		std::queue<MSG> msgQ;

		ws::Cursor cursor; 


		
		
		public:		
		
		
		ws::View view;
		std::vector<ws::Child*> children;
		ws::Texture backBuffer;
	    Gdiplus::Graphics* canvas;
		
		Window()
		{
			canvas = nullptr;
        	hwnd = nullptr;
		}
		
		
		
		Window(int width,int height,std::string title = "",DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0,const std::string& className = "Window")
		{
			create(width,height,title,style,exStyle,className);
		}
		
		
	

		void create(int width,int height,std::string title = "",DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0,const std::string& className = "Window")
		{
			if(width <= 0 || height <= 0)
			{
				std::cerr << "Error: Attempted to create a window with an invalid size!" << std::endl;
				MessageBoxA(NULL,"Error: Attempted to create a window with an invalid size!","Developer Error",MB_OK);
			}
			
			if (hwnd && IsWindow(hwnd)) 
			{
				DestroyWindow(hwnd);
				// force processing of the destruction message
				MSG msg;
				while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				hwnd = nullptr;
			}
			
			WindowManager::init(className);
			
			
			if(style == -1)
				style = WS_OVERLAPPEDWINDOW;
			if(exStyle == -1)
				exStyle = 0;
			
			//Note to self: the style must be set this way because hwnd has not been initialized yet!
			style |= WS_CLIPCHILDREN;
			
			
			
			view.init({0,0,width,height});
			
			
			hwnd = CreateWindowEx(
			exStyle,
			ws::WIDE(className).c_str(),
			ws::WIDE(title).c_str(),
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
		        std::cerr << "Failed to create window!" << std::endl;
				exit(-1);
		    }			
			
			backBuffer.create(view.getSize().x, view.getSize().y);
			canvas = new Gdiplus::Graphics(backBuffer.getHDC());
			

			isRunning = true;
			setVisible(true);
			UpdateWindow(hwnd);
			setFocus();
			
		}

        ~Window()
        {
			RevokeDragDrop(hwnd);//just in case the window does use dragndrop. You have to call the revoke function for the window before it gets destroyed. Placing revoke elsewhere could result in a crash.


			if (canvas) {
				delete canvas;
				canvas = nullptr;
			}

			if (hwnd && IsWindow(hwnd)) {
				DestroyWindow(hwnd);
			}
        }
		
		
		void close()
		{
		    if (hwnd && IsWindow(hwnd)) {
		        DestroyWindow(hwnd);
		    }
		    isRunning = false;
		}
		
		bool isOpen()
		{
			if(!isRunning || !hwnd)
				return false;
			
			MSG msg;
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) 
			{
				
				if (msg.message == WM_QUIT) {
					isRunning = false;
					return false;
				}
				
				
				bool isOurs = (msg.hwnd == hwnd || IsChild(hwnd, msg.hwnd));
				
				if (isOurs) 
				{
					msgQ.push(msg);
				}
				
				if (isOurs && !IsDialogMessage(hwnd, &msg)) 
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				} 
				else if(!isOurs) 
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}				
			
			return isRunning && hwnd;
		}
		
	    bool pollEvent(MSG &message) {
	        if (msgQ.empty()) {
	            return false;
	        }
	        
	        message = msgQ.front();
	        msgQ.pop();
	        return true;
	    }	
		
	    void clear(Gdiplus::Color color = Gdiplus::Color(255,0,0,0)) 
		{
			if (!hwnd) return;

			ws::Vec2i needed = view.getSize();

			if (!canvas || backBuffer.getSize().x != needed.x || backBuffer.getSize().y != needed.y)
			{
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
		
		void draw(Drawable &draw)
		{
			if(!canvas || !hwnd) return;





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
		
		private:

		bool m_perPixelAlpha = false;

		void updateLayeredWindow()
		{
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
		
		public:
		
	    void display() 
		{
			if(m_perPixelAlpha && hwnd)
				updateLayeredWindow();
			if(!hwnd)
		    	return;
			InvalidateRect(hwnd, NULL, FALSE);
		    UpdateWindow(hwnd);
	    }		
		
		
		
		
		void setPixel(int x,int y,ws::Hue hue)
		{
			backBuffer.setPixel(x,y,hue);
		}
		
		ws::Hue getPixel(int x,int y)
		{
			return backBuffer.getPixel(x,y);
		}
		
		std::string getTitle()
		{
			if(!hwnd)
				return "";
				
			char title[256];
            GetWindowTextA(hwnd, title, sizeof(title));
			return std::string(title);			
		}
		
		void setTitle(std::string title)
		{
			if(!hwnd)
				return;
			SetWindowTextA(hwnd,ws::TO_LPCSTR(title));
		}
		
		void setView(ws::View &v)
		{
			view = v;
		}
		
		ws::View getView()
		{
			return view;
		}
		
		void setVisible(bool val)
		{
			if(!hwnd)
				return;
				
			if(!val)
				ShowWindow(hwnd, SW_HIDE);
			else
				ShowWindow(hwnd, SW_SHOW);
		}
		
		bool getVisible()
		{
			if(!hwnd)
				return false;
			return IsWindowVisible(hwnd);
		}
		
		void setFocus()
		{
			SetFocus(hwnd);
		}
		
		bool hasFocus()
		{
			HWND focus = GetFocus();
			return (focus == hwnd);
		}
		
		void setLayerAfter(HWND lastHwnd)
		{
			SetWindowPos(hwnd,lastHwnd,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
		}
		
		void addStyle(DWORD style)
		{
			if(!hwnd)
				return;
			
			DWORD s = getStyle();
			s |= style;
			
				
			SetWindowLongA(hwnd,GWL_STYLE,s);		
		}
		
		void removeStyle(DWORD style)
		{
			if(!hwnd)
				return;
			
			DWORD s = getStyle();
			s &= ~style;
			
				
			SetWindowLongA(hwnd,GWL_STYLE,s);			
		}
		
		void setAllStyle(DWORD style)
		{
			if(!hwnd)
				return;
			
			SetWindowLongA(hwnd,GWL_STYLE,0);
			
			SetWindowLongA(hwnd,GWL_STYLE,style);			
		}

		void addExStyle(DWORD style)
		{
			if(!hwnd)
				return;
			
			DWORD s = getExStyle();
			s |= style;
			
				
			SetWindowLongA(hwnd,GWL_EXSTYLE,s);		
		}
		
		void removeExStyle(DWORD style)
		{
			if(!hwnd)
				return;
			
			DWORD s = getExStyle();
			s &= ~style;
			
				
			SetWindowLongA(hwnd,GWL_EXSTYLE,s);			
		}
		
		void setAllExStyle(DWORD style)
		{
			if(!hwnd)
				return;
			
			SetWindowLongA(hwnd,GWL_EXSTYLE,0);
			
			SetWindowLongA(hwnd,GWL_EXSTYLE,style);			
		}

		DWORD getExStyle()
		{	
			return GetWindowLong(hwnd, GWL_EXSTYLE);
		}
	    
	    DWORD getStyle()
	    {
	    	return GetWindowLong(hwnd, GWL_STYLE);
            
		}
	    
	    bool hasStyle(DWORD checkStyle)
	    {
	    	return (getStyle() & checkStyle);
		}
		
		bool hasExStyle(DWORD checkStyle)
	    {
	    	return (getExStyle() & checkStyle);
		}
		
		void setSize(ws::Vec2i size)
		{
			setSize(size.x,size.y);
		}
	    
	    void setSize(int screenWidth,int screenHeight)
	    {
	    	if(screenWidth <= 0 || screenHeight <= 0)
			{
				setVisible(false);
				std::cerr << "Warning! You tried to set a window to an invalid size. This has been converted into a safe setVisible(false) command. Try using the setVisible function as a better practice.\n";
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
		
		ws::Vec2i getSize()
		{
			RECT rect;
			GetClientRect(hwnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;


			return ws::Vec2i(width,height);			
		}
		
		void setPosition(ws::Vec2i pos)
		{
			setPosition(pos.x,pos.y);
		}
		
		void setPosition(int posx,int posy)
		{
	    	SetWindowPos(hwnd, 
			nullptr, 
			posx, 
			posy, 
			0, 
			0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);			
		}
		
		ws::Vec2i getPosition()
		{
			if(!hwnd)
				return ws::Vec2i(0,0);
				
            RECT rect;
            GetWindowRect(hwnd, &rect);
						
            // Remove window decorations for size calculation
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            
			return ws::Vec2i(clientRect.left,clientRect.top);			
		}

		void setFullscreen(bool fullscreen = true) 
		{
			if (fullscreen == isFullscreen) return;
			
			if (fullscreen) 
			{
				//save the style
				windowedStyle = getStyle();
				GetWindowRect(hwnd, &windowedRect);
				
				int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
				int screenHeight = GetSystemMetrics(SM_CYSCREEN);
				
				removeStyle(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
				addStyle(WS_POPUP | WS_VISIBLE);
				
				SetWindowPos(hwnd, HWND_TOP, 0, 0, screenWidth, screenHeight,SWP_FRAMECHANGED | SWP_NOACTIVATE);
				
				isFullscreen = true;
			} 
			else 
			{
				setAllStyle(windowedStyle);
				
				SetWindowPos(hwnd, HWND_TOP,
				windowedRect.left, windowedRect.top,
				windowedRect.right  - windowedRect.left,
				windowedRect.bottom - windowedRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);
				
				isFullscreen = false;
			}
		}
	    
	    bool getFullscreen() const {
	        return isFullscreen;
	    }

		void enableChromaKey(ws::Hue hue,bool legacy = false)//losing window control for some reason
		{
			m_perPixelAlpha = legacy;				
			
			addExStyle(WS_EX_LAYERED);
			
			if(!legacy)
				SetLayeredWindowAttributes(hwnd,RGB(hue.r,hue.g,hue.b),static_cast<BYTE>(hue.a),LWA_COLORKEY | LWA_ALPHA);
		}
		
		void disableChromaKey()
		{
			removeExStyle(WS_EX_LAYERED);
		}
		
		void enableAlphaOnly(float alpha)
		{
			addExStyle(WS_EX_LAYERED);
			if(!m_perPixelAlpha)
				SetLayeredWindowAttributes(hwnd,0,static_cast<BYTE>(alpha),LWA_ALPHA);			
		}
		
		void disableAlphaOnly()
		{
			disableChromaKey();
		}
		
		
		ws::Vec2i toWorld(int x,int y)
		{
			return view.toWorld(x,y,getSize());
		}
		
		ws::Vec2i toWorld(ws::Vec2i pos)
		{
			return toWorld(pos.x,pos.y);
		}
		
		ws::Vec2i toScreen(int x,int y)
		{
			return view.toScreen(x,y,getSize());
		}
		
		ws::Vec2i toScreen(ws::Vec2i pos)
		{
			return toScreen(pos.x,pos.y);
		}

		//Child Management - Declared here but defined in the controls header if included.
		void addChild(ws::Child &child);
		void removeChild(ws::Child &child);
		bool hasChild(ws::Child &child);
		
		
		void setCursor(ws::Cursor newcursor)
		{
			cursor = newcursor;
		}
		ws::Cursor getCursor()
		{
			return cursor;
		}
		
		private:

		
		LRESULT handleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)
		{

            switch (uMsg) {
	            case WM_DESTROY:
					WindowManager::removeWindow(hwnd);
					isRunning = false;
					if (WindowManager::windows.empty())
						PostQuitMessage(0);
	                return 0;
	            
				case WM_CLOSE:
					DestroyWindow(hwnd);
					return 0;

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
					if (LOWORD(lParam) == HTCLIENT)
					{
						SetClassLongPtr(hwnd, GCLP_HCURSOR, 
							(LONG_PTR)(cursor.getHandle() ? cursor.getHandle() : LoadCursor(nullptr, IDC_ARROW)));

					}
					return DefWindowProc(hwnd, uMsg, wParam, lParam);
				}				
				
	            case WM_PAINT: 
				{
	            	
					if (m_perPixelAlpha)
					{
						PAINTSTRUCT ps;
						BeginPaint(hwnd, &ps);
						EndPaint(hwnd, &ps);
						return 0;
					}	            	
					else
					{
						
						PAINTSTRUCT ps;
						HDC hdc = BeginPaint(hwnd, &ps);

						if (backBuffer.isFastDIB()) {
							ws::Vec2i screenSize = getSize();
							ws::Vec2i worldSize  = view.getSize();

							if (screenSize.x == worldSize.x && screenSize.y == worldSize.y)
								BitBlt(hdc, 0, 0, backBuffer.getSize().x, backBuffer.getSize().y, backBuffer.getHDC(), 0, 0, SRCCOPY);
							else
							{
								SetStretchBltMode(hdc, COLORONCOLOR);
								StretchBlt(hdc, 0, 0, screenSize.x, screenSize.y, backBuffer.getHDC(), 0, 0, backBuffer.getSize().x, backBuffer.getSize().y, SRCCOPY);
							}
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
		
		bool isFullscreen = false;
		RECT windowedRect; // Stores window position/size when not fullscreen
    	DWORD windowedStyle; // Stores window style when not fullscreen			
	};
	
	
	//Window Manager Stuff
	
	inline void ws::WindowManager::init(const std::string& className)
	{
		if(initialized)
			return;
		HINSTANCE instance = GetModuleHandle(nullptr);
		
		WNDCLASS wc = {};
		wc.lpfnWndProc = WindowManager::GlobalProc;
		wc.hInstance = instance;
		wc.lpszClassName = ws::WIDE(className).c_str();
		wc.hCursor = NULL;
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		
		if(!RegisterClass(&wc))
		{
			std::cerr << "Failed to initialize Window"<<std::endl;
			exit(-1);
		}
		initialized = true;
	}
	
	inline LRESULT CALLBACK ws::WindowManager::GlobalProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{
		//std::lock_guard<std::mutex> lock(windowsMutex);
		
		if(msg == WM_NCCREATE)
		{
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			ws::Window* pWindow = reinterpret_cast<ws::Window*>(pCreate->lpCreateParams);
			
			pWindow->hwnd = hwnd; 
			
			windows[hwnd] = pWindow;
			
			SetWindowLongPtr(hwnd,GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
			
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		
		ws::Window* pWindow = reinterpret_cast<ws::Window*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));
		
		if(pWindow)
		{
			return pWindow->handleMessage(msg,wParam,lParam);
		}
		
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	
	
	inline void ws::WindowManager::addWindow(ws::Window* window)
	{
		//std::lock_guard<std::mutex> lock(windowsMutex);
		windows[window->hwnd] = window;			
	}
	inline void ws::WindowManager::removeWindow(HWND hwnd)
	{
		//std::lock_guard<std::mutex> lock(windowsMutex);
		windows.erase(hwnd);//hwnd is a pointer to a location. Therefore, it may be used to point to my ws::Window.
	}
	
	inline ws::Window* ws::WindowManager::GetWindow(HWND hwnd)
	{
		//std::lock_guard<std::mutex> lock(windowsMutex);
		auto it = windows.find(hwnd);
		if (it != windows.end()) {
			return it->second;
		}
		return nullptr;
	}		
	
	
	
	std::map<HWND, ws::Window*> ws::WindowManager::windows;
	std::mutex ws::WindowManager::windowsMutex;
    bool ws::WindowManager::initialized = false;	
	

	//=========== GLOBAL INPUT ===========
	namespace Global
	{
	
		ws::Vec2i getMousePos(ws::Window &window)
		{
			
		    POINT p;
		    if(!GetCursorPos(&p))
		    {
		        return {0,0};
		    }
		    
		    ScreenToClient(window.hwnd, &p); // Convert to client coordinates


		    return p;
		}
		
		ws::Vec2i getMousePos()
		{
				
			POINT p;
			if(!GetCursorPos(&p))
			{
				return {0,0};
			}
			
			return p;
		
		}
		
		
		bool getButton(int button)
		{
			if ((GetAsyncKeyState(button) & 0x8000) != 0)
				return true;
			return false;			
		}
		
		//depreciated functions. Avoid using these!
		bool getMouseButton(int vmButton)
		{
			return getButton(vmButton);
		}
		bool getKey(int vmKey)
		{
			return getButton(vmKey);
		}
		
		
		
	}

	class Screen
	{
		HDC hdc = nullptr;
		bool valid = true;
		
		public:
		
		Screen()
		{
			hdc = GetDC(NULL);
			if(hdc == NULL)
				valid = false;
		}
		
		~Screen()
		{
			ReleaseDC(NULL, hdc);
		}
		
		HDC getHDC()
		{
			return hdc;
		}
		
		std::vector<ws::Vec2i> getDisplayModes()
		{
			std::vector<ws::Vec2i> modes;
			
			DEVMODE dm = {};
			dm.dmSize = sizeof(dm);
			DWORD modeNum = 0;

			while(EnumDisplaySettings(NULL, modeNum, &dm)) 
			{
				ws::Vec2i res = ws::Vec2i((int)dm.dmPelsWidth,(int)dm.dmPelsHeight);
				
				bool duplicate = false;
				for (const auto& existing : modes) 
				{
					if (existing.x == res.x && existing.y == res.y) 
					{
						duplicate = true;
						break;
					}
				}
				if (!duplicate) 
				{
					modes.push_back(res);
				}
				modeNum++;
			}

			//sort according to size with smallest first.
			std::sort(modes.begin(),modes.end(),[](ws::Vec2i &a,ws::Vec2i &b){
				return a.x * a.y < b.x * b.y; 
			});

			return modes;		
		}
		
		std::string setSize(int x,int y)
		{
			return setSize({x,y});
		}
		
		std::string setSize(ws::Vec2i size)
		{
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
			if(hdc == NULL)
				valid = false;	
			return "SUCCESS";
		}
		
		ws::Vec2i getSize()
		{
			int width  = GetDeviceCaps(hdc, HORZRES);
			int height = GetDeviceCaps(hdc, VERTRES);
			return ws::Vec2i(width,height);
		}
		
		
		ws::Hue getPixel(int x,int y)
		{
			if(!valid)
				return ws::Hue::transparent;
			return getPixel(ws::Vec2i(x,y));
		}

		ws::Hue getPixel(ws::Vec2i pos)
		{
			if(!valid)
				return ws::Hue::transparent;
			
			if(pos.x >= 0 && pos.x < getSize().x && pos.y >= 0 && pos.y < getSize().y)
				return GetPixel(hdc,pos.x,pos.y);
			
			return ws::Hue::transparent;
		}
		
		void setPixel(int x,int y,ws::Hue hue)
		{
			if(!valid)
				return;
			if(x >= 0 && x < getSize().x && y >= 0 && y < getSize().y)
				SetPixel(hdc,x,y,hue);
		}
		
		ws::Texture getSnapshot()
		{
			ws::Texture texture;
			if(!texture.create(getSize().x,getSize().y,ws::Hue::transparent))
				return ws::Texture();
			
			HDC hdcTex = texture.getHDC();
			if(hdcTex)
				BitBlt(hdcTex, 0, 0, getSize().x, getSize().y, hdc, 0, 0, SRCCOPY);			
			
			return texture;
		}
		
		
	};


	class GDIPInit
	{
		public:	
		
		//GDI+	
		Gdiplus::GdiplusStartupInput gdiplusstartup;
		ULONG_PTR gdiplustoken;
		
		int maxControlID = 0;
		
		
		
		GDIPInit()
		{
			//GDI+
			Gdiplus::GdiplusStartup(&gdiplustoken,&gdiplusstartup,nullptr);	
		}
		
		~GDIPInit()
		{
			//Gdi+
			Gdiplus::GdiplusShutdown(gdiplustoken);
		}
		
	}gdipInit;
	
}

#endif