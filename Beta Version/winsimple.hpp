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

#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>

#include <iostream>
#include <string>
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
		
		// High-precision timer using QueryPerformanceCounter.
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
		
		// Resets start time and returns elapsed seconds since last restart.
		double restart()
		{
	        double seconds = getSeconds();
            LARGE_INTEGER counter;
            QueryPerformanceCounter(&counter);
            startTime = counter.QuadPart;
            
            return seconds;
		}
		
		
	    // Returns elapsed seconds since start.
	    double getSeconds() const
	    {
	        LARGE_INTEGER currentTime;
	        QueryPerformanceCounter(&currentTime);
	        return static_cast<double>(currentTime.QuadPart - startTime) / frequency;
	      
	    }
	    
	    // Returns elapsed milliseconds since start.
	    double getMilliSeconds() const
	    {
	        return getSeconds() * 1000.0;
	    }
	    
	    // Returns elapsed microseconds since start.
	    double getMicroSeconds() const
	    {
	        return getMilliSeconds() * 1000.0;
	    }
	
		private:
		    LONGLONG startTime = 0;
		    double frequency = 1.0;		
	};
	
	
	
    // String conversion helpers
    // Converts a UTF-8 std::string to std::wstring, falling back to ACP on invalid chars.
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
    // Converts a wide string back to UTF-8.
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
    // Returns the 8.3 short path name for the given long path.
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
    // Retrieves the CLSID of an image encoder based on MIME type.
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
    // Makes a path absolute relative to the executable directory.
    inline bool ResolveRelativePath(std::string& path)
    {
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
		
        // Checks if a point lies inside the rectangle.
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
		
		// Constructs from GDI+ color.
		Hue(Gdiplus::Color &color)
		{
			r = color.GetR();
			g = color.GetG();
			b = color.GetB();
			a = color.GetA();
		}

		// Constructs from COLORREF (alpha forced to 255).
	    Hue(COLORREF color)
	    {
	        r = GetRValue(color);
	        g = GetGValue(color);
	        b = GetBValue(color);
	        a = 255;  // COLORREF doesn't have alpha
	    }
		
		// Constructs from individual components.
		Hue(int r1,int g1,int b1,int a1=255)
		{
			r = r1;
			g = g1;
			b = b1;
			a = a1;
		}
		
		
		// Implicit conversion to Gdiplus::Color.
		operator Gdiplus::Color() const
		{
			return Gdiplus::Color(a,r,g,b); 
		}
		
		// Implicit conversion to COLORREF (drops alpha).
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
		
		
		struct HSV {
			float h;  
			float s; 
			float v; 
			
		};
		
		// Creates an Hue from HSV values (h in degrees, s,v in [0,1]).
		static Hue fromHSV(float h, float s, float v, int alpha = 255) {
			// h: 0..360 degrees, s: 0..1, v: 0..1
			float c = v * s;
			float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
			float m = v - c;

			float r1 = 0, g1 = 0, b1 = 0;
			if (h < 60) {
				r1 = c; g1 = x; b1 = 0;
			} else if (h < 120) {
				r1 = x; g1 = c; b1 = 0;
			} else if (h < 180) {
				r1 = 0; g1 = c; b1 = x;
			} else if (h < 240) {
				r1 = 0; g1 = x; b1 = c;
			} else if (h < 300) {
				r1 = x; g1 = 0; b1 = c;
			} else {
				r1 = c; g1 = 0; b1 = x;
			}

			int r = static_cast<int>((r1 + m) * 255);
			int g = static_cast<int>((g1 + m) * 255);
			int b = static_cast<int>((b1 + m) * 255);

			return Hue(r, g, b, alpha);
		}		
		
		
		static Hue fromHSV(HSV hsv,int alpha = 255)
		{
			return fromHSV(hsv.h,hsv.s,hsv.v,alpha);
		}
		
		// Converts this color to HSV representation.
		HSV toHSV() const {
			float rNorm = r / 255.0f;
			float gNorm = g / 255.0f;
			float bNorm = b / 255.0f;

			float maxVal = std::max({rNorm, gNorm, bNorm});
			float minVal = std::min({rNorm, gNorm, bNorm});
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

			return {hue, saturation, value};
		}	


		// Checks if the given HSV value falls within a hue range (with tolerance and min saturation/value).
		static bool inHueRange(float hue,ws::Hue::HSV hsv,float tolerance = 60,float minSaturation = 0.1,float minValue = 0.2)
		{
			float lower = hue - tolerance;
			float upper = hue + tolerance;

			bool hueInRange = false;
			if (lower < 0) {
				hueInRange = (hsv.h >= (360.0f + lower) || hsv.h <= upper);
			} else if (upper > 360) {
				hueInRange = (hsv.h >= lower || hsv.h <= (upper - 360.0f));
			} else {
				hueInRange = (hsv.h >= lower && hsv.h <= upper);
			}


			return (hueInRange && (hsv.s >= minSaturation) && (hsv.v >= minValue));				
		}
		
		// Replaces a given hue range with a new hue, preserving saturation and value.
		static ws::Hue replaceHue(float hue,float replacement,ws::Hue rgb)
		{
			HSV hsv = rgb.toHSV();
			
			if(ws::Hue::inHueRange(hue,hsv))
			{
				ws::Hue rgb2 = fromHSV(replacement,hsv.s,hsv.v,rgb.a);
				return rgb2;
			}
			return fromHSV(hsv.h,hsv.s,hsv.v,rgb.a);
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


	//KEYBOARD AND MOUSE BUTTON EQUIVALENTS TO WINAPI TYPES - Use these in ws::Global::getButton() or use the winapi equivalents.
	class Key 
	{
		
		public:
		// Letters
		static const int
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
			Z = 'Z';

		// Digits (top row)
		static const int
			Num0 = '0',
			Num1 = '1',
			Num2 = '2',
			Num3 = '3',
			Num4 = '4',
			Num5 = '5',
			Num6 = '6',
			Num7 = '7',
			Num8 = '8',
			Num9 = '9';

		// Numpad
		static const int
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
			Divide = 0x6F;

		// Function keys
		static const int
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
			F24 = 0x87;

		// Modifiers (generic)
		static const int
			Shift = 0x10,
			Control = 0x11,
			Alt = 0x12;

		// Modifiers (left/right specific)
		static const int
			LeftShift = 0xA0,
			RightShift = 0xA1,
			LeftControl = 0xA2,
			RightControl = 0xA3,
			LeftAlt = 0xA4,
			RightAlt = 0xA5;

		// Windows keys & Application key
		static const int
			LeftWin = 0x5B,
			RightWin = 0x5C,
			Application = 0x5D;

		// Navigation & Editing
		static const int
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
			Help = 0x2F;

		// Lock keys
		static const int
			NumLock = 0x90,
			ScrollLock = 0x91;

		// OEM specific keys (US keyboard layout)
		static const int
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
			Oem8 = 0xDF;

		// Browser / Media keys
		static const int
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
			LaunchApp2 = 0xB7;

		static const std::vector<int>& GetAllKeys() 
		{
			static const std::vector<int> keys = {
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

		static std::string GetKeyName(int keyCode) 
		{
			static const std::unordered_map<int, std::string> nameMap = []{
				std::unordered_map<int, std::string> map;
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
			auto it = nameMap.find(keyCode);
			if (it != nameMap.end())
				return it->second;
			return "Unknown(" + std::to_string(keyCode) + ")";
		}
		

		// Prevent instantiation
		Key() = delete;
	};

	class Mouse {
	public:
		static const int
			Left = 0x01,
			Right = 0x02,
			Middle = 0x04,
			XButton1 = 0x05,
			XButton2 = 0x06;

		static const std::vector<int>& GetAllButtons() 
		{
			static const std::vector<int> buttons = { Left, Right, Middle, XButton1, XButton2 };
			return buttons;
		}

		static std::string GetButtonName(int buttonCode) {
			static const std::unordered_map<int, std::string> nameMap = []{
				std::unordered_map<int, std::string> map;
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
			return "UnknownMouseButton(" + std::to_string(buttonCode) + ")";
		}		

		Mouse() = delete;
	};

	

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
		
		
		// Default constructor.
		View()
		{
			
		}
		
		
		// Custom copy constructor because Gdiplus::Matrix lacks a normal one.
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
	    
	  
		// move constructor
		
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
	  
	  
	    
		// Copy assignment operator.
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
		
		
		// Move assignment operator.
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
		
		// Initialises view with a port rectangle, setting world to match.
		void init(int portLeft,int portTop,int portWidth,int portHeight)
		{
			port.left = portLeft;
			port.top = portTop;
			port.width = portWidth;
			port.height = portHeight;
			
			world = port;
		}
		
		// Initialises from an IntRect.
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
		
		
		// Returns the center of the world rectangle.
		[[nodiscard]] ws::Vec2i getCenter()
		{
			return ws::Vec2i(world.left + (world.width/2),world.top + (world.height/2));
		}

		// Sets the center of the world rectangle.
		void setCenter(int cx,int cy)
		{
			world.left = cx - (world.width/2);
			world.top = cy - (world.height/2);
		}

		void setCenter(ws::Vec2i pos)
		{
			setCenter(pos.x,pos.y);
		}


		// Returns the center of the port rectangle.
		[[nodiscard]] ws::Vec2i getPortCenter()
		{
			return ws::Vec2i(port.left + (port.width/2),port.top + (port.height/2));
		}

		// Sets the center of the port rectangle.
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
		
		
		// Sets the point around which the view rotates.
		void setPortRotatePoint(int ox,int oy)
		{
			portOrigin.x = ox;
			portOrigin.y = oy;
		}
		
		void setPortRotatePoint(ws::Vec2i pos)
		{
			setPortRotatePoint(pos.x,pos.y);
		}
		
		// Sets rotation point to the port center.
		void setPortRotatePointCenter()
		{
			portOrigin = ws::Vec2i(port.left + (port.width/2), port.top + (port.height/2));
		}
		
		
		[[nodiscard]] float getRotation() 
		{
		    return rotation;
		}
		
		// Sets the rotation angle in degrees.
		void setRotation(float angle) 
		{
		    rotation = angle;
		}
		
		// Sets the zoom level (exponent: 2^zoom).
		void setZoom(float val)
		{
			zoom = val;
		}
		
		
		[[nodiscard]] float getZoom()
		{
			return zoom;
		}
		
		// Moves the world by a floating-point delta.
		void move(float dx,float dy)
		{
			world.left += static_cast<int>(dx);
			world.top += static_cast<int>(dy);
		}
		
		void move(ws::Vec2f dir)
		{
			move(dir.x,dir.y);
		}

		// Moves the world by an integer delta.
		void move(int dx,int dy)
		{
			world.left += dx;
			world.top += dy;
		}		
		
	    // Copies the internal transformation matrix into the given matrix.
	    void getTransform(Gdiplus::Matrix &m) const
	    {
	        Gdiplus::REAL elements[6];
	        matrix.GetElements(elements);
	        m.SetElements(elements[0], elements[1], elements[2], 
	                     elements[3], elements[4], elements[5]);
	    }
		
		// Sets the internal transformation matrix from an external one.
		void setTransform(const Gdiplus::Matrix &m)
	    {
	        Gdiplus::REAL elements[6];
	        m.GetElements(elements);
	        matrix.SetElements(elements[0], elements[1], elements[2], 
	                          elements[3], elements[4], elements[5]);
	        
	    }
		
		// Converts screen coordinates to world coordinates, accounting for view transform.
		[[nodiscard]] ws::Vec2i toWorld(ws::Vec2i screenPos) 
		{
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
			float worldX = static_cast<float>(screenPos.x);
			float worldY = static_cast<float>(screenPos.y);
			
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
	    
	    [[nodiscard]] ws::Vec2i toWorld(int x,int y) 
	    {
	        return toWorld(ws::Vec2i(x,y));
	    }
	    
	
		// Converts world coordinates to screen coordinates.
	    [[nodiscard]] ws::Vec2i toScreen(ws::Vec2i worldPos) 
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
	        
	        return ws::Vec2i(static_cast<int>(screenX), static_cast<int>(screenY));
	    }
	
	    
	    [[nodiscard]] ws::Vec2i toScreen(int x,int y) 
	    {
	        return toScreen(ws::Vec2i(x,y));
	    }       		

		// Applies the current view transform (matrix, clip) to the given GDI+ graphics.
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
			
		// Recalculates the transformation matrix based on current view settings.
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

		enum class ScaleMode {
			NearestNeighbor,   // sharp edges, pixelated
			Bilinear,          // smooth, linear filter
			Bicubic,           // smoother, slightly more expensive
			HighQualityBicubic // best quality, slowest
		};
		
		ScaleMode scaleMode = ScaleMode::HighQualityBicubic;
		
		
		Gdiplus::Bitmap* bitmap;
		
		
		Texture() : bitmap(nullptr) {}
		
		// Constructs and loads from file.
		Texture(std::string path)
		{
			loadFromFile(path);
		}

		private:

		// Destroys the DIBSection and its associated GDI objects before the GDI+ bitmap is freed.
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

		// Copies the bitmap data from another texture, preserving fast DIB if possible.
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
		
	    // Destructor cleans up GDI+ bitmap and possible DIB.
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
			scaleMode = other.scaleMode;
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
				scaleMode = other.scaleMode;
			}
			return *this;
		}

		
		// Move constructor
		Texture(Texture&& other) noexcept
			: width(other.width),
			  height(other.height),
			  bitmap(other.bitmap),
			  m_hdcMem(other.m_hdcMem),
			  m_hDIB(other.m_hDIB),
			  m_hOldBmp(other.m_hOldBmp),
			  m_dibBits(other.m_dibBits),
			  m_isFast(other.m_isFast),
			  scaleMode(other.scaleMode)
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
				scaleMode = other.scaleMode;

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
		

		// Creates a new texture as a DIBSection, enabling fast pixel access and BitBlt.
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
			
			scaleMode = ScaleMode::HighQualityBicubic;
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
		
		// Loads from a file, creating a DIBSection-backed texture.
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

		// Loads from an in‑memory image buffer (PNG, JPEG, etc.).
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

		// Loads from an existing Gdiplus::Bitmap, converting it to DIBSection.
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
		
		
		
		
		


		
	    // Returns true if a valid bitmap exists.
	    bool isValid() const
	    {
	        return bitmap != nullptr;
	    }		
	    
	    
	    
		void setScaleMode(ScaleMode mode) 
		{ scaleMode = mode; }
		ScaleMode getScaleMode() const 
		{ return scaleMode; }
	    
	    
	    
		// Sets a pixel using 1D index (x = index % width, y = index / width).
		void setPixel(int index, ws::Hue color)
		{
			int x = index % width;
			int y = index / width;
			setPixel(x, y, color);
		}

		// Sets a pixel at (xIndex,yIndex) with color; fast DIB or GDI+ fallback.
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
	    
	    // Gets a pixel using 1D index.
		ws::Hue getPixel(int index)
		{
			int x = index % width;
			int y = index / width;
			return getPixel(x, y);
		}

		// Gets a pixel at (xIndex,yIndex); returns transparent black if out of bounds.
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
	    
	    
	    
	    // Returns size as Vec2i.
	    ws::Vec2i getSize() const 
		{
			return ws::Vec2i(width,height); 
		}
		
		// Resizes the texture to (w,h) using high‑quality interpolation.
		void setSize(int w,int h)
		{
			setSize({w,h});
		}
		
		void setSize(ws::Vec2i s)
		{
			if (s.x < 1 || s.y < 1) return;

			Gdiplus::Bitmap scaled(s.x, s.y, PixelFormat32bppARGB);
			Gdiplus::Graphics g(&scaled);
			g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
			g.DrawImage(bitmap, 0, 0, s.x, s.y);

			loadFromBitmapPlus(scaled);
		}
		
		// Scales the texture uniformly by factor s.
		void setScale(float s)
		{
			if (s <= 0.0f || !bitmap)
				return;

			int newW = static_cast<int>(width  * s);
			int newH = static_cast<int>(height * s);
			if (newW < 1 || newH < 1) return;

			Gdiplus::Bitmap scaled(newW, newH, PixelFormat32bppARGB);
			Gdiplus::Graphics g(&scaled);
			g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
			g.DrawImage(bitmap, 0, 0, newW, newH);

			loadFromBitmapPlus(scaled);
		}
		
		// Saves the texture to a file (format inferred from extension).
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
		
		
		
		// Initialises with default Arial font.
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
		

		
		// Returns true if loaded from a file (custom), false if system.
		bool isSystemFont()
		{ return isCustomFont; }
		
		std::string getFilePath()
		{ return fontFilePath; }
		
		
		std::string getName()
		{ return fontName; }
		
		

		// Loads a system font by name.
		bool loadFromSystem(std::string name)
		{
			fontFilePath.clear();
			isCustomFont = false;
			
			fontCollection.reset(new Gdiplus::PrivateFontCollection());
			
			fontName = name;
			
			return update();
			
		}
		
		// Loads a font from a file (TrueType).
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
		
		
		
		// Returns true if the font object is valid.
		bool isValid()
	    {
	        return gdiFont && gdiFont->GetLastStatus() == Gdiplus::Ok;
	    }

		
		
		private:
		
		// Creates/re‑creates the GDI+ font object from the current family/style/size.
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
	    
	    // Returns visual width after scaling (absolute).
	    int getVisualWidth() const {
	        return static_cast<int>(std::abs(width * scale.x));
	    }
	    
	    // Returns visual height after scaling (absolute).
	    int getVisualHeight() const {
	        return static_cast<int>(std::abs(height * scale.y));
	    }
	    
	    // Computes world-space bounding rectangle of the drawable (after scale, rotation, origin).
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
	    
		
		// Returns bounding rectangle as IntRect.
		ws::IntRect getBounds() const
	    {
	    	int left,top,right,bottom;
			getBounds(left,top,right,bottom);
			return ws::IntRect(left,top,right - left,bottom - top);
	    }
	    
		
		// Tests if a world point is inside the visual shape, accounting for rotation.
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
	    
	    // Draws the object in world space by applying the local transform matrix.
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
		
		
		// Default constructor.
		Sprite()
		{
			
		}
		
		// Constructs with a texture, setting the rect to full texture.
		Sprite(ws::Texture &texture)
		{
			textureRef = &texture;
			setTextureRect({0,0,texture.getSize().x,texture.getSize().y});
		}
		
		
	    // Checks if point lies inside sprite bounds.
	    virtual bool contains(ws::Vec2i pos) override
	    {
	        int left, top, right, bottom;
	        getBounds(left, top, right, bottom);
	        
	        return (pos.x >= left && pos.x < right &&
	                pos.y >= top && pos.y < bottom);
	    }
	    // Draws the sprite texture with the texture's scale mode.
	    virtual void draw(Gdiplus::Graphics* graphics) override
	    {
	        if (!textureRef || !textureRef->isValid()) 
	            return;
	        
	        Gdiplus::Rect destRect(0,0, width, height);
	        Gdiplus::Rect srcRect(texLeft, texTop, texWidth, texHeight);
			
			Gdiplus::InterpolationMode oldMode = graphics->GetInterpolationMode();
			
			switch(textureRef->getScaleMode()) 
			{
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
	    void setTexture(ws::Texture& texture,bool resize = true) {
	        textureRef = &texture;
	        if(resize)
	        	setTextureRect({0,0,texture.getSize().x,texture.getSize().y});
	    }
	    
	    // Sets the source rectangle from the texture.
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
	    
	    // Returns the current source rectangle.
	    ws::IntRect getTextureRect() const {
	        return {texLeft, texTop, texWidth, texHeight};
	    }
		
		// Returns reference to the attached texture.
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
		
	    
	    
	    // Constructs a line with endpoints, width, and color.
	    Line(ws::Vec2i start = {0,0},ws::Vec2i end = {0,0},int thewidth = 2,Gdiplus::Color color = {255,0,0,255})
	    {
	    	this->start = start;
	    	this->end = end;
	    	width = thewidth;
	    	this->color = color;
		}
	    
	    // Draws the line in local coordinates.
		virtual void draw(Gdiplus::Graphics* canvas) override 
		{
	    	Gdiplus::Pen pen(color, static_cast<Gdiplus::REAL>(width));
			canvas->DrawLine(&pen, static_cast<Gdiplus::REAL>(start.x), static_cast<Gdiplus::REAL>(start.y), static_cast<Gdiplus::REAL>(end.x), static_cast<Gdiplus::REAL>(end.y));
	    }
	    
	    
	    private:
	    // Contains always returns false for a line.
	    virtual bool contains(ws::Vec2i pos) override
	    { 
	    	return false;
		}
		
		
		public:
		
		// Helper: checks if point q is on segment pr.
		bool onSegment(ws::Vec2i p, ws::Vec2i q, ws::Vec2i r)
	    {
	        if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
	            q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
	            return true;
	        return false;
	    }
		
		// Returns orientation of triplet: 0 = collinear, 1 = clockwise, 2 = counterclockwise.
	    int orientation(ws::Vec2i p, ws::Vec2i q, ws::Vec2i r)
	    {
	        long long val = (long long)(q.y - p.y) * (r.x - q.x) - 
	                       (long long)(q.x - p.x) * (r.y - q.y);
	        
	        if (val == 0) return 0;  // Collinear
	        return (val > 0) ? 1 : 2; // Clockwise or counterclockwise
	    }			
		
	    // Returns true if this line segment intersects another.
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
	    
	    // Constructs with vertices, colors, and fill settings.
	    Poly(std::vector<ws::Vec2i>& vertices, Gdiplus::Color fillColor = {255,255,0,0}, Gdiplus::Color borderColor = {255,255,0,255}, int borderWidth = 2, bool filled = true)
	    {
	        this->vertices = vertices;
	        this->fillColor = fillColor;
	        this->borderColor = borderColor;
	        this->borderWidth = borderWidth;
	        this->filled = filled;
	    }
	    
	    // Adds a vertex (and a default UV).
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
	    
	    // Removes all vertices.
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
	    
	    // Returns true if at least 3 vertices.
	    bool isValid() 
	    {
	        return vertices.size() >= 3;
	    }
	    
	    // Returns the centroid of the polygon.
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
	    
	    // Point-in-polygon test using ray casting.
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
	    
	    // Checks if any edge intersects the given line (or line endpoints inside).
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
	    
	    // Checks if this polygon intersects another polygon.
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
	    
	    // Returns the bounding rectangle of all vertices.
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
	    
	    // Associates a texture for filling (triggers effect generation).
	    void setTexture(ws::Texture &tex)
	    {
	        textureRef = &tex;
	        textureNeedsUpdate = true;
	    }   
	    
	    // Removes texture mapping.
	    void removeTexture()
	    {
	        textureRef = nullptr;
	    }
	    
	    ws::Texture* getTexture()
	    {
	        return textureRef;
	    }
	    
	    // Sets the UV coordinates for a vertex.
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
	    
	    // Forces a regeneration of the texture effect next draw.
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
	    
	    // Point-in-polygon test used internally.
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
	    
	    // Pre‑computes an effect texture that maps the source texture onto the polygon.
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
	    
	    // Computes UV coordinates for a point inside the polygon using barycentric interpolation.
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
	    
	    // Checks if a point is inside a triangle.
	    bool pointInTriangle(ws::Vec2i p, ws::Vec2i a, ws::Vec2i b, ws::Vec2i c)
	    {
	        float alpha = static_cast<float>(((b.y - c.y)*(p.x - c.x) + (c.x - b.x)*(p.y - c.y))) /
	                     static_cast<float>(((b.y - c.y)*(a.x - c.x) + (c.x - b.x)*(a.y - c.y)));
	        float beta = static_cast<float>(((c.y - a.y)*(p.x - c.x) + (a.x - c.x)*(p.y - c.y))) /
	                    static_cast<float>(((b.y - c.y)*(a.x - c.x) + (c.x - b.x)*(a.y - c.y)));
	        float gamma = 1.0f - alpha - beta;
	        
	        return (alpha >= 0 && beta >= 0 && gamma >= 0);
	    }
	    
	    // Calculates UV via barycentric coordinates for a point inside triangle.
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
	    // Draws the polygon, optionally textured.
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
		
		
		
		// Default constructor.
		Text(){}
		~Text(){}
		
		
		// Constructs with a font.
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
		
		
		

		// Contains test delegates to base class.
		virtual bool contains(ws::Vec2i pos) override
		{
			return Drawable::contains(pos);
		}
		
		// Draws the text with outline and fill.
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

			// Update the offset so that Drawable draws the text at the proper position.
			Gdiplus::Matrix originalTransform;
			canvas->GetTransform(&originalTransform);
			canvas->TranslateTransform(-bounds.X, -bounds.Y);
		    
		    
		    Gdiplus::SolidBrush fillBrush(fillColor);
    
    		// Draw the outline
    		canvas->DrawPath(&outlinePen, &path);
    		
    		// Fill the text
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
		
		// Default radial with 8 points, radius 10.
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
		
		// Regenerates vertices for the radial shape.
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
		
		
		// Sets center position.
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
		
		
		// Moves center by delta.
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
		
		// Sets point count and rebuilds.
		void setPointCount(int count)
		{
			m_points = count;
			make(m_points);
		}
		
		// Sets radius and rebuilds.
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
		
		// Draws via poly.
		virtual void draw(Gdiplus::Graphics* canvas) override
		{
			poly.draw(canvas);
		}
		
		// Contains test using circle equation.
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
			
		// Draws an ellipse filled and outlined.
		virtual void draw(Gdiplus::Graphics* canvas) override
		{
	        Gdiplus::Pen borderPen(m_borderColor, static_cast<Gdiplus::REAL>(m_borderWidth));
	        Gdiplus::SolidBrush fillBrush(m_fillColor);
			
			canvas->DrawEllipse(&borderPen,0.0f,0.0f,static_cast<Gdiplus::REAL>(width),static_cast<Gdiplus::REAL>(height));
			canvas->FillEllipse(&fillBrush,0.0f,0.0f,static_cast<Gdiplus::REAL>(width),static_cast<Gdiplus::REAL>(height));
		}
		
		// Integer overload for contains.
		bool contains(int px,int py)
		{
			return contains(ws::Vec2i(px,py));
		}
		
		// Contains test for ellipse (still not fully accurate after rotation).
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
		
		// Loads a system or OLE cursor.
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

		// Creates a cursor from a Texture.
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

		// Loads a cursor from a .cur or .ani file.
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


		// Destructor destroys the cursor.
		~Cursor()
		{
			if (handle)
				DestroyCursor(handle);
		}

		// Copy constructor.
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

		// Copy assignment operator.
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

		// Move constructor.
		Cursor(Cursor&& other) noexcept
			: handle(other.handle), animated(other.animated), srcPath(std::move(other.srcPath))
		{
			other.handle   = nullptr;
			other.animated = false;
		}

		// Move assignment operator.
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
		
		HWND hwnd;	

		private:

		
		friend class WindowManager;
		
		bool isRunning = false;
		
		std::queue<MSG> msgQ;

		ws::Cursor cursor; 

		// Default notify stub that just passes to DefWindowProc.
		static LRESULT defaultNotifyStub(Window* window, NMHDR* pnmh, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			return DefWindowProc(window->hwnd, uMsg, wParam, lParam);
		}
		
		
		
		public:		
		
		
		ws::View view;
		std::vector<ws::Child*> children;
		ws::Texture backBuffer;
	    Gdiplus::Graphics* canvas;
		
		// Default constructor; creates an empty window.
		Window()
		{
			canvas = nullptr;
        	hwnd = nullptr;
		}
		
		// Convenience constructor that calls create().
		Window(int width,int height,std::string title = "",DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0,const std::string& className = "Window")
		{
			create(width,height,title,style,exStyle,className);
		}
		
		
	
		// Creates the actual window with the given parameters.
		void create(int clientWidth,int clientHeight,std::string title = "",DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0,const std::string& className = "Window")
		{
			if(clientWidth <= 0 || clientHeight <= 0)
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
			
			if(!WindowManager::registerClass(className))
				return;
			
			
			if(style == -1)
				style = WS_OVERLAPPEDWINDOW;
			if(exStyle == -1)
				exStyle = 0;
			
			//Note to self: the style must be set manually this way because hwnd has not been initialized yet!
			style |= WS_CLIPCHILDREN;
			
			
			
			view.init({0,0,clientWidth,clientHeight});


			RECT rect = {0, 0, clientWidth, clientHeight};
			AdjustWindowRectEx(&rect, style, FALSE, exStyle);
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;
			
			
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

		// Destructor – destroys the window and cleans up resources.
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
		
		// Immediately destroys the window.
		void close()
		{
		    if (hwnd && IsWindow(hwnd)) {
		        DestroyWindow(hwnd);
		    }
		    isRunning = false;
		}
		
		// Processes messages; returns false when closed.
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
		
		// Pops the next queued message; returns false if queue empty.
	    bool pollEvent(MSG &message) {
	        if (msgQ.empty()) {
	            return false;
	        }
	        
	        message = msgQ.front();
	        msgQ.pop();
	        return true;
	    }	
		
		// Clears the back buffer to the given color, recreating if needed.
	    void clear(ws::Hue color = ws::Hue::transparent) 
		{
			if (!hwnd) return;

			ws::Vec2i needed = view.getPortSize();;

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
		
		// Draws a Drawable object using the current view transform.
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

		// Updates the layered window for per‑pixel alpha when enabled.
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
		
		// Invalidates and updates the window (and possibly layered window).
	    void display() 
		{
			if(m_perPixelAlpha && hwnd)
				updateLayeredWindow();
			if(!hwnd)
		    	return;
			InvalidateRect(hwnd, NULL, FALSE);
		    UpdateWindow(hwnd);
	    }		
		
		
		
		// Sets a pixel on the back buffer.
		void setPixel(int x,int y,ws::Hue hue)
		{
			backBuffer.setPixel(x,y,hue);
		}
		
		// Gets a pixel from the back buffer.
		ws::Hue getPixel(int x,int y)
		{
			return backBuffer.getPixel(x,y);
		}
		
		// Returns the window title.
		std::string getTitle()
		{
			if(!hwnd)
				return "";
				
			char title[256];
            GetWindowTextA(hwnd, title, sizeof(title));
			return std::string(title);			
		}
		
		// Sets the window title.
		void setTitle(std::string title)
		{
			if(!hwnd)
				return;
			SetWindowTextA(hwnd,ws::TO_LPCSTR(title));
		}
		
		// Replaces the current view.
		void setView(ws::View &v)
		{
			view = v;
		}
		
		ws::View &getView()
		{
			return view;
		}
		
		// Shows or hides the window.
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
		
		// Sets focus to this window.
		void setFocus()
		{
			SetFocus(hwnd);
		}
		
		bool hasFocus()
		{
			HWND focus = GetFocus();
			return (focus == hwnd);
		}
		
		// Places this window after another in Z-order.
		void setLayerAfter(HWND lastHwnd)
		{
			SetWindowPos(hwnd,lastHwnd,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
		}
		
		// Adds a window style.
		void addStyle(DWORD style)
		{
			if(!hwnd)
				return;
			
			DWORD s = getStyle();
			s |= style;
			
				
			SetWindowLongA(hwnd,GWL_STYLE,s);		
		}
		
		// Removes a window style.
		void removeStyle(DWORD style)
		{
			if(!hwnd)
				return;
			
			DWORD s = getStyle();
			s &= ~style;
			
				
			SetWindowLongA(hwnd,GWL_STYLE,s);			
		}
		
		// Sets all window styles at once.
		void setAllStyle(DWORD style)
		{
			if(!hwnd)
				return;
			
			SetWindowLongA(hwnd,GWL_STYLE,0);
			
			SetWindowLongA(hwnd,GWL_STYLE,style);			
		}

		// Adds an extended style.
		void addExStyle(DWORD style)
		{
			if(!hwnd)
				return;
			
			DWORD s = getExStyle();
			s |= style;
			
				
			SetWindowLongA(hwnd,GWL_EXSTYLE,s);		
		}
		
		// Removes an extended style.
		void removeExStyle(DWORD style)
		{
			if(!hwnd)
				return;
			
			DWORD s = getExStyle();
			s &= ~style;
			
				
			SetWindowLongA(hwnd,GWL_EXSTYLE,s);			
		}
		
		// Sets all extended styles.
		void setAllExStyle(DWORD style)
		{
			if(!hwnd)
				return;
			
			SetWindowLongA(hwnd,GWL_EXSTYLE,0);
			
			SetWindowLongA(hwnd,GWL_EXSTYLE,style);			
		}

		DWORD getExStyle() const
		{	
			return GetWindowLong(hwnd, GWL_EXSTYLE);
		}
	    
	    DWORD getStyle() const
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
	    
		// Sets the window size (client area dimensions).
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
		
		// Returns the client area size.
		ws::Vec2i getSize() const
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
		
		// Sets the window position.
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
		
		// Returns the window position.
		ws::Vec2i getPosition() const
		{
			if(!hwnd)
				return ws::Vec2i(0,0);
			
			RECT windowRect;
			GetWindowRect(hwnd, &windowRect);
			return ws::Vec2i(windowRect.left, windowRect.top);		
		}
		
		// Returns the total window rect size (including non-client area).
		ws::Vec2i getTotalSize() const
		{
            RECT rect;
            GetWindowRect(hwnd, &rect);
			
			return ws::Vec2i(rect.right - rect.left,rect.bottom - rect.top);
		}
		
		// Returns the client area position on screen.
		ws::Vec2i getClientPosition() const
		{
			if (!hwnd)
				return ws::Vec2i(0, 0);
			
			POINT clientOrigin = {0, 0};
			ClientToScreen(hwnd, &clientOrigin);
			return ws::Vec2i(clientOrigin.x, clientOrigin.y);
		}		

		// Returns the border width in pixels.
		int getBorderWidth() const
		{
			int border = 0;
			DWORD style = getStyle();
			if(style & WS_THICKFRAME)
				border = GetSystemMetrics(SM_CYFRAME);
			else if(style & WS_BORDER)
				border = GetSystemMetrics(SM_CYEDGE);
			return border;
		}

		// Returns the caption rectangle (title bar area).
		ws::IntRect getCaptionRect(bool excludeBorder = false) const
		{
			if(!hwnd) 
				return {0, 0, 0, 0};

			DWORD style = getStyle();
			if(!(style & WS_CAPTION))
				return {0, 0, 0, 0};

			RECT winRect;
			GetWindowRect(hwnd, &winRect);
			
			int border = 0;
			if(excludeBorder)
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

		//force all clicks to pass through this window as if it does not exist.
		void disableAnyClicks()
		{
			addMessageHandler([](MSG msg) -> LRESULT{
				if(msg.message == WM_NCHITTEST)
				{
					return HTTRANSPARENT;   // mouse passes through
				}
				return 0;
			});			
		}


		//Choose what color should represent emptiness and any click will pass through that color as if nothing is there.
		void disableSomeClicks(ws::Hue hue)
		{
			addMessageHandler([&](MSG msg) -> LRESULT{
				if(msg.message == WM_NCHITTEST)
				{
					POINT p;
					if(!GetCursorPos(&p))
					{
						p.x = -1000;
						p.y = -1000;
					}
					ScreenToClient(hwnd, &p); // Convert to client coordinates
					
					ws::Vec2f MP = toWorld(p);
					if(getPixel(MP.x,MP.y) == hue)
						return HTTRANSPARENT;   // mouse passes through
				}
				return 0;				
			});
		}

		// Enables chroma key transparency (legacy per-pixel or simple).
		void enableChromaKey(ws::Hue hue,bool legacy = false)
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
		
		// Enables alpha-only transparency.
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
		
		// Converts screen coordinates to world.
		ws::Vec2i toWorld(int x,int y)
		{
			return view.toWorld(x,y);
		}
		
		ws::Vec2i toWorld(ws::Vec2i pos)
		{
			return toWorld(pos.x,pos.y);
		}
		
		// Converts world coordinates to screen.
		ws::Vec2i toScreen(int x,int y)
		{
			return view.toScreen(x,y);
		}
		
		ws::Vec2i toScreen(ws::Vec2i pos)
		{
			return toScreen(pos.x,pos.y);
		}

		//Child Management - Declared here but defined in the controls header if included.
		void addChild(ws::Child &child);
		void removeChild(ws::Child &child);
		bool hasChild(ws::Child &child);
		// By default, this points to an empty function. If the child controls header is included, then an alternative definition is provided by the header.
		static inline LRESULT (*s_handleNotifyForChildren)(Window* window, NMHDR* pnmh, UINT uMsg, WPARAM wParam, LPARAM lParam) = &ws::Window::defaultNotifyStub;  
		
		// Sets the window cursor.
		void setCursor(ws::Cursor newcursor)
		{
			cursor = newcursor;
		}
		ws::Cursor getCursor()
		{
			return cursor;
		}
		
		// Adds a custom message handler.
		void addMessageHandler(std::function<LRESULT(MSG msg)> handler)
		{
			customHandlers.push_back(std::move(handler));
		}
		
		std::vector<std::function<LRESULT(MSG msg)>> getMessageHandlers()
		{
			return customHandlers;
		}

		private:
		std::vector<std::function<LRESULT(MSG msg)>> customHandlers;
		
		// Handles messages; called by the global window procedure.
		LRESULT handleMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)
		{
			for(auto& handler : customHandlers)
			{
				MSG msg;
				msg.message = uMsg;
				msg.lParam = lParam;
				msg.wParam = wParam;
				
				LRESULT lresult = handler(msg);
				if(lresult != 0)
					return lresult;//if result is not zero that means that the message was handled.
			}			

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

	std::set<std::wstring> ws::WindowManager::registeredClasses;
	std::map<HWND, ws::Window*> ws::WindowManager::windows;
	std::mutex ws::WindowManager::windowsMutex;
	
	// Implementation of registerClass.
	bool ws::WindowManager::registerClass(const std::string& className)
	{
		std::wstring wclassName = ws::WIDE(className);
		
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

		if (!RegisterClass(&wc))
		{
			std::cerr << "Failed to register window class: " << className << std::endl;
			return false;
		}

		registeredClasses.insert(wclassName);
		return true;
	}



	// Implementation of GlobalProc.
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
	
	// Implementation of addWindow.
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
	
	// Implementation of GetWindow.
	inline ws::Window* ws::WindowManager::GetWindow(HWND hwnd)
	{
		//std::lock_guard<std::mutex> lock(windowsMutex);
		auto it = windows.find(hwnd);
		if (it != windows.end()) {
			return it->second;
		}
		return nullptr;
	}		
	

	

	//=========== GLOBAL INPUT ===========
	namespace Global
	{
		// Returns mouse position relative to the given window's client area.
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
		
		// Returns global mouse position.
		ws::Vec2i getMousePos()
		{
				
			POINT p;
			if(!GetCursorPos(&p))
			{
				return {0,0};
			}
			
			return p;
		
		}
		
		// Checks if a virtual key/button is pressed.
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
		
		// Acquires the desktop HDC.
		Screen()
		{
			hdc = GetDC(NULL);
			if(hdc == NULL)
				valid = false;
		}
		
		// Releases the desktop HDC.
		~Screen()
		{
			ReleaseDC(NULL, hdc);
		}
		
		HDC getHDC()
		{
			return hdc;
		}
		
		// Enumerates all unique display resolutions.
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
		
		// Changes the display resolution; returns status string.
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
		
		// Returns the current screen size.
		ws::Vec2i getSize()
		{
			int width  = GetDeviceCaps(hdc, HORZRES);
			int height = GetDeviceCaps(hdc, VERTRES);
			return ws::Vec2i(width,height);
		}
		
		// Gets a pixel from the screen.
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
		
		// Sets a pixel on the screen.
		void setPixel(int x,int y,ws::Hue hue)
		{
			if(!valid)
				return;
			if(x >= 0 && x < getSize().x && y >= 0 && y < getSize().y)
				SetPixel(hdc,x,y,hue);
		}
		
		// Captures the screen into a ws::Texture.
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
		
		// Returns the system DPI scaling factor.
		float getDPI()
		{
			SetProcessDPIAware();			
			HDC hdc = GetDC(nullptr);
			if (!hdc) return 1.0f;
			int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
			ReleaseDC(nullptr, hdc);
			return static_cast<float>(dpi) / 96.0f;
		}

		// Returns the work area (excluding taskbar).
		ws::IntRect getWorkArea()
		{
			RECT area;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &area, 0);
			return { area.left, area.top, area.right - area.left, area.bottom - area.top };
		}
		
	};


	class GDIPInit
	{
		public:	
		
		//GDI+	
		Gdiplus::GdiplusStartupInput gdiplusstartup;
		ULONG_PTR gdiplustoken;
		
		int maxControlID = 0;
		
		// Initializes GDI+ and sets DPI awareness.
		GDIPInit()
		{
			//GDI+
			Gdiplus::GdiplusStartup(&gdiplustoken,&gdiplusstartup,nullptr);	
			SetProcessDPIAware();
		}
		
		// Shuts down GDI+.
		~GDIPInit()
		{
			//Gdi+
			Gdiplus::GdiplusShutdown(gdiplustoken);
		}
		
	}gdipInit;
	
}

#endif