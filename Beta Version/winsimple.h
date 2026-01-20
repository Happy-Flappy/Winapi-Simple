#ifndef WINAPI_SIMPLE_GRAPHICS
#define WINAPI_SIMPLE_GRAPHICS






#ifndef NOMINMAX
#define NOMINMAX
#endif        
#ifndef STRICT
#define STRICT
#endif          
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

 
#include <windows.h>  
#include <windowsx.h>  
#include <commctrl.h>  
#include <commdlg.h>
#include <iostream>
#include <string>
#include <cstdlib>      
#include <map>

#include <queue>
#include <iomanip>
#include <cmath>

#include <mmsystem.h>
#include <filesystem>
#include <cwchar>
#include <algorithm>


// Define missing Windows types BEFORE including GDI+
#ifndef SHORT
typedef short SHORT;
#endif

#ifndef PROPID
typedef unsigned long PROPID;
#endif

#include <gdiplus.h>
#include <shlwapi.h>
#include <objbase.h>
#include <Shlobj.h>
#include <shellapi.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ========== CORE UTILITIES ==========
namespace ws
{
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
	
	
	
	bool ResolveRelativePath(std::string path)
	{
		// Convert to absolute path to ensure proper resolution
	    std::filesystem::path filePath(path);
	    if (filePath.is_relative()) {
	        // Get executable directory and resolve relative path
	        char exePath[MAX_PATH];
	        GetModuleFileNameA(NULL, exePath, MAX_PATH);
	        std::filesystem::path exeDir = std::filesystem::path(exePath).parent_path();
	        filePath = exeDir / filePath;
	    }
	    
//	    if (!std::filesystem::exists(filePath)) {
//	        std::cerr << "File not found at: " << filePath.string() << std::endl;
//	        return false;
//	    }
	    path = filePath.string();
	    return true;
	}


	std::wstring WIDE(std::string str)
	{
	    // Try UTF-8 first
	    int size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);
	    if (GetLastError() == ERROR_NO_UNICODE_TRANSLATION) {
	        // Fallback to ANSI (system default code page)
	        size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
	    }
	    
	    std::wstring wstr(size, 0);
	    if (size > 0) {
	        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
	    }
	    return wstr;
	}

	
	std::string SHORT(const std::wstring& wstr) {
	    // Determine the size of the required buffer
	    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	    if (bufferSize == 0) {
	        return "";
	    }
	
	    // Create the string and perform the conversion
	    std::string str(bufferSize - 1, '\0'); // Subtract 1 for the null terminator
	    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, str.data(), bufferSize, NULL, NULL);
	
	    return str;
	}
	
	
	
	
	LPCSTR TO_LPCSTR(std::string str)
	{
		return LPCSTR(str.c_str());
	}
		


	std::wstring GetShortPathNameSafe(const std::wstring& longPath) 
	{
	    // Pass NULL for the buffer to get the required size.
	    DWORD bufferSize = GetShortPathNameW(longPath.c_str(), NULL, 0);
	    if (bufferSize == 0) {
	        return L""; // Return empty on failure
	    }
	
	    // Allocate the buffer
	    std::wstring shortPath(bufferSize, L'\0');
	
	    // Call again with the allocated buffer
	    bufferSize = GetShortPathNameW(longPath.c_str(), shortPath.data(), bufferSize);
	    if (bufferSize == 0) {
	        return L""; // Return empty on failure
	    }
	    
	    // Resize to the actual length and return
	    shortPath.resize(bufferSize);
	    return shortPath;
	}


	//A microsoft function that I just copied. It is required for the saving of images to file.
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
	    UINT  num = 0;          // number of image encoders
	    UINT  size = 0;         // size of the image encoder array in bytes
	
	    Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;
	
	    Gdiplus::GetImageEncodersSize(&num, &size);
	    if(size == 0)
	        return -1;  // Failure
	
	    pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	    if(pImageCodecInfo == NULL)
	        return -1;  // Failure
	
	    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
	
	    for(UINT j = 0; j < num; ++j)
	    {
	        if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
	        {
	            *pClsid = pImageCodecInfo[j].Clsid;
	            free(pImageCodecInfo);
	            return j;  // Success
	        }    
	    }
	
	    free(pImageCodecInfo);
	    return -1;  // Failure
	}


		
}



#include <type_traits>
#include <utility>


// ========== DATA TYPES ==========
namespace ws
{

	// I MUST ADMIT! I did use AI to make these data types support template construction. 
	// When it comes to templates and type constructors, these are not my strongsuits. 
	// I just don't know it well enough to do it on my own. 
	// I did not use AI for anything else in the library. Only for adding comments and for doing repetitive tasks such as replacing POINT with ws::Vec2f. 
	

	
	// ==================== TYPE TRAITS ====================
	
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
	};
		
}





// ========== SOUND & VIDEO ==========
namespace ws 
{
	
	class Wav
	{
		public:
			
			
		std::string path;
		int channel = 0;
		bool blocking = true;
		std::string ID = "";
		std::string extension = "none";
			
			
			
		~Wav()
		{
		    // Clean up
		    if(!ID.empty())
		    {
		        std::string status = getChannelStatus(channel);
		        if (status != "error") {
		            std::string command = "close " + ID;
		            mciSendStringA(command.c_str(), NULL, 0, NULL);
		        }
			}
		}

		Wav(std::string path = "",int channel = 0,bool blocking = true)
		{
			this->path = path;
			this->channel = channel;
			this->blocking = blocking;
		}




		private:
		
		static bool mciSimple(std::string command,bool sendError = true)
		{
			MCIERROR err = mciSendStringA(TO_LPCSTR(command),NULL,0,NULL);
		    if (err) 
			{
		        if(sendError)
				{
					char errorBuf[128];
			        mciGetErrorStringA(err, errorBuf, sizeof(errorBuf));
			        std::cerr << "Sound Error of type MCI error: " << errorBuf << " - Command: " << command << "\n";
		        }
				return false;
		    }	
			return true;		
		}
		
		
		
		
		
		static void isSupported(std::string m_path)
		{
			std::filesystem::path p(m_path);
			std::string ext = p.extension().string();
			
			if(ext == ".wav" || ext == ".mid" || ext == ".midi")
			{
				//always supported
				return;
			}
			if(ext == ".mp3" || ext == ".wma")
			{
				//supported by most modern windows
				return;
			}
			if(ext == ".au" || ext == ".aif" || ext == ".aiff" || ext == ".snd")
			{
				std::cerr << "Warning! "<<ext << " is less commonly supported and might not work on some computers! Safest options are: (wav,mid,midi,mp3,wma).\n";
				return;
			}
			
			std::cerr << "Warning! "<< ext << " probably wont be supported on most computers! Safest options are: (wav,mid,midi,mp3,wma).\n ";
		}
		
		public:


		static int getFreeChannel()
		{
			int channel = -1;
			for(int a=0;a<100;a++)
			{
				if(getChannelStatus(a) != "playing" && getChannelStatus(a) != "paused")
				{
					if(a != -1)
					{
						channel = a;
						break;
					}
				} 
			}
			if(channel == -1)
			{
				std::cerr << "A sound channel could not be auto detected! \nThis means that either the maximum number of device\n sound contexts have been created OR that the range of channel\n ID's from 0 to 100 are all in use.\n Try using a number below or above the minimum and maximum attempted values.\n";
			}
			
			return channel;
		}


		static std::string getChannelStatus(int m_channel)
		{
			
			std::string m_ID = std::to_string(m_channel);
			
			
			
		    char returnBuffer[128]; // Buffer to store the status string
		    std::string command = "status " + m_ID + " mode";
		    
		     memset(returnBuffer, 0, sizeof(returnBuffer));
		   
		    
		    // Send the command and check for errors
		    if (mciSendStringA(command.c_str(), returnBuffer, sizeof(returnBuffer), NULL) == 0) {
		        return returnBuffer; // Returns "playing", "stopped", etc.
		    } else {
		        return "error";
		    }
		    return "error";
		}
		

		
		static bool PlayFree(std::string m_path,int m_channel,bool m_blocking = false)
		{
			
			isSupported(m_path);
			
			
			
			std::string m_ID = std::to_string(m_channel);
			
			if (!std::filesystem::exists(m_path)) {
	            std::cerr << "Sound file not found: " << m_path << "\n";
	            return false;
	        }
			


			//Get shortened path name because mciSendStringA does not support long paths.
			std::wstring wpath = GetShortPathNameSafe(WIDE(m_path));
			if(!wpath.empty())
				m_path = SHORT(wpath);
			
			
			std::string command;
			//Make command to close this old channel if it was already open
			command = "close " + m_ID;
			mciSimple(command,false);
			
			
			//open the file and give it an alias - users do not have to see this. All they have to remember is the channel ID since this is technically different device context channels.
			command = "open "+ m_path + " alias "+m_ID;
			if(!mciSimple(command))
				return false;
			
			
			//Make command for playing the file.
			command = "play "+m_ID;
			
			//play the file
			if(!mciSimple(command))
				return false;
			
			return true;
		}
		
		
		
		
		
		
		bool open(std::string m_path,int m_channel,bool m_blocking = true)
		{
			isSupported(m_path);
			
			std::filesystem::path p(m_path);
			extension = p.extension().string();
			
			if (!std::filesystem::exists(m_path)) {
	            std::cerr << "Sound file not found: " << m_path << "\n";
	            return false;
	        }
			this->path = m_path;
			this->channel = m_channel;
			this->blocking = m_blocking;
			this->ID = std::to_string(m_channel);
			
			
			
			//close it no matter what
	        std::string command = "close " + ID;
	        mciSimple(command,false);//close channel without error report.			
			
			

			//Get shortened path name because mciSendStringA does not support long paths.
			std::wstring wpath = GetShortPathNameSafe(WIDE(path));
			if(!wpath.empty())
				path = SHORT(wpath);
			
			
			
			//open channel
			command = "open "+ path + " alias "+ID;
			if(!mciSimple(command))
				return false;
			
			

			// Set time format to milliseconds for better control
	        command = "set " + ID + " time format milliseconds";
	        if(!mciSimple(command))
				return false;			
			
			
			
			return true;
		}
		
		void play()
		{

			if(getChannelStatus(channel) == "error")
			{
				std::cerr<<"Sound attempted to play but was not initialized! Use open() before play().\n";
				return;
			}
			
			
			if(getChannelStatus(channel) == "playing" && blocking)
				return;
			
			std::string command = "";
			
	        // Start playback
	        command = "play " + ID;
	        
	        if(!mciSimple(command))
				return;
			
						
		}
		
		
		
		static void stop(int m_channel)
		{
			std::string m_ID = std::to_string(m_channel);
			
			std::string command = "pause "+ m_ID;
			
			mciSimple(command);
		}
		
		void stop()
		{
			std::string command = "pause "+ ID;
			
			mciSimple(command);
		}
		
		
		
		
	    bool setVolume(int percent)
	    {
	    	
	    	if(getChannelStatus(channel) == "error")
	    	{
	    		std::cerr << "Unsupported Audio Action: setVolume cannot be used before using open()\n";
	    		return false;
			}
			
			if(extension == ".mid" || extension == ".midi")
			{
				std::cerr << "Unsupported Audio Action: An attempt was made to set the volume of a midi file!\n";
			}
				    	
	    	
	        // Convert percentage back to MCI volume (0-1000)
	        int volume = (percent * 1000) / 100;
	        
	        if(getChannelStatus(channel) == "error") return false;
	        
	        // Clamp volume to valid range
	        volume = std::max(0, std::min(1000, volume));
	        
	        // Try different MCI volume commands in order
	        std::string commands[] = {
	            "set " + ID + " audio volume to " + std::to_string(volume),
	            "setaudio " + ID + " volume to " + std::to_string(volume), 
	            "set " + ID + " volume " + std::to_string(volume)
	        };
	        
	        for (const auto& cmd : commands) {
	            if (mciSimple(cmd, false)) {  
	                return true;
	            }
	        }
	        
	        std::cerr << "Volume control not supported on this system\n";
	        return false;
	    }
	    
	    int getVolume()
	    {

	    	if(getChannelStatus(channel) == "error")
	    	{
	    		std::cerr << "Unsupported Audio Action: getVolume cannot be used before using open()\n";
	    		return false;
			}
	    	
			if(extension == ".mid" || extension == ".midi")
			{
				std::cerr << "Unsupported Action: An attempt was made to get the volume of a midi file!\n";
			}
			
	        if(getChannelStatus(channel) == "error") return 0;
	        
	        char returnBuffer[128];
	        memset(returnBuffer, 0, sizeof(returnBuffer));
	        
	        std::string command = "status " + ID + " volume";
	        MCIERROR err = mciSendStringA(command.c_str(), returnBuffer, sizeof(returnBuffer), NULL);
	        
	        if(err)
	        	return 0;
	        	
	        int volume = atoi(returnBuffer);			
			
			
			
	        if(volume == 0)
	        	return 0;
	        // MCI volume range is 0 - 1000 but this returns percentage of that.
	        return (volume * 100) / 1000;
	    }
		
		
		bool setProgress(long seconds)
		{
			std::string status = getChannelStatus(channel);
	    	if(status == "error")
	    	{
	    		std::cerr << "Unsupported Audio Action: setProgress cannot be used before using open()\n";
	    		return false;
			}
						
		    std::string command;
		    if (status == "playing") {
		        command = "play " + ID + " from " + std::to_string(seconds * 1000);
		    } else {
		        command = "seek " + ID + " to " + std::to_string(seconds * 1000);
		    }
		    
			return mciSimple(command);
		}
		
		
		long getProgress()
		{
	    	if(getChannelStatus(channel) == "error")
	    	{
	    		std::cerr << "Unsupported Audio Action: getProgress cannot be used before using open()\n";
	    		return false;
			}
						
			char returnBuffer[128];
			memset(returnBuffer, 0, sizeof(returnBuffer));
			
			std::string command = "status "+ ID + " position";
			
			MCIERROR err = mciSendStringA(TO_LPCSTR(command), returnBuffer, sizeof(returnBuffer), NULL);
	        if (err) 
			{
				char errorBuf[128];
		        mciGetErrorStringA(err, errorBuf, sizeof(errorBuf));
		        std::cerr << "Sound Error of type MCI error: " << errorBuf << " - Command: " << command << "\n";
	            
				return 0;
	        }			
	        
		    // Convert string to long
		    char *end_ptr;
		    long result = std::strtol(returnBuffer, &end_ptr, 10);
		    
		    // Check if conversion was successful
		    if (returnBuffer == end_ptr) {
		        return 0; // conversion error
		    }
	        	
	        return result / 1000;//return in seconds
		}	
		
		
		
		long getLength()
		{
	    	if(getChannelStatus(channel) == "error")
	    	{
	    		std::cerr << "Unsupported Audio Action: getLength() cannot be used before using open()\n";
	    		return 0;
			}
		    
		    char returnBuffer[128];
		    memset(returnBuffer, 0, sizeof(returnBuffer));
		    
		    std::string command = "status " + ID + " length";
		    MCIERROR err = mciSendStringA(command.c_str(), returnBuffer, sizeof(returnBuffer), NULL);
			
			if(err)
			{
				return 0;
			}
			return (atol(returnBuffer) - 10)/1000; // return in seconds
		    
		}
		
		
		
		bool isFinished()
		{
			if(getChannelStatus(channel) == "error")
	    	{
	    		std::cerr << "Unsupported Audio Action: isFinished() cannot be used before using open()\n";
	    		return false;
			}
			return (getProgress() >= getLength());
		}
		
		
		
		
		
		
		
	};
	
	
	
	
	
	
		
}





// ========== GRAPHICS ==========
namespace ws 
{



	class View
	{
		public:
		
		
		View()
		{
			
		}
		
		
		//Matrix does not have normal copy constructor so the view class has to do this in a custom way so that View can be copied to another view like view = v;
	    View(const View& other) : 
	        world(other.world), 
	        port(other.port), 
	        rotation(other.rotation),
	        portOrigin(other.portOrigin),
	        matrix(),
	        zoom(other.zoom)
	    {
	        setTransform(other.matrix);
	    }	
	    
	  
		//move constructor
		
		View(View&& other) noexcept :
	        world(std::move(other.world)),
	        port(std::move(other.port)),
	        rotation(other.rotation),
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
			world.left += dx;
			world.top += dy;
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
		
		
		
		
	    [[nodiscard]] ws::Vec2i toWorld(ws::Vec2i screenPos,ws::Vec2i screenSize) 
	    {
		    
			
			    
	        // First account for window stretching
	        ws::Vec2i stretchedPos;
	        stretchedPos.x = static_cast<int>(static_cast<float>(screenPos.x) * 
	                                         (static_cast<float>(world.width) / static_cast<float>(screenSize.x)));
	        stretchedPos.y = static_cast<int>(static_cast<float>(screenPos.y) * 
	                                         (static_cast<float>(world.height) / static_cast<float>(screenSize.y)));
	        
	        
	        // Now apply the transformation (scaled by zoom)
	        float zoomFactor = std::pow(2.0f, zoom);
	        
	        // Calculate the visible world center
	        float visibleWorldCenterX = world.left + world.width / 2.0f;
	        float visibleWorldCenterY = world.top + world.height / 2.0f;
	        
	        // Calculate the port center
	        float portCenterX = port.left + port.width / 2.0f;
	        float portCenterY = port.top + port.height / 2.0f;
	        
	        // Calculate scale to fit visible world into port
	        float visibleWorldWidth = static_cast<float>(world.width) / zoomFactor;
	        float visibleWorldHeight = static_cast<float>(world.height) / zoomFactor;
	        
	        float scaleX = static_cast<float>(port.width) / visibleWorldWidth;
	        float scaleY = static_cast<float>(port.height) / visibleWorldHeight;
	        
	        // Apply inverse transformation
	        float worldX = static_cast<float>(stretchedPos.x);
	        float worldY = static_cast<float>(stretchedPos.y);
	        
	        // Reverse the transformation steps
	        // 1. Translate from port center to origin
	        worldX -= portCenterX;
	        worldY -= portCenterY;
	        
	        // 2. Reverse rotation
	        if (rotation != 0) {
	            Gdiplus::Matrix rotMatrix;
	            rotMatrix.Rotate(-rotation);
	            Gdiplus::PointF point(worldX, worldY);
	            rotMatrix.TransformPoints(&point, 1);
	            worldX = point.X;
	            worldY = point.Y;
	        }
	        
	        // 3. Reverse scale
	        worldX /= scaleX;
	        worldY /= scaleY;
	        
	        // 4. Translate back to world center
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
	        float visibleWorldCenterX = world.left + world.width / 2.0f;
	        float visibleWorldCenterY = world.top + world.height / 2.0f;
	        
	        // Calculate the port center
	        float portCenterX = port.left + port.width / 2.0f;
	        float portCenterY = port.top + port.height / 2.0f;
	        
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
		    
		    graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
		    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		    graphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
		    graphics.SetTransform(&matrix);
		}
			
		private:
			
			
		void updateMatrix()			
		{
		    matrix.Reset();
		    
		    float zoomFactor = std::pow(2.0f, zoom);
		    
		    // The VISIBLE portion of the world (after zoom)
		    float visibleWorldWidth = static_cast<float>(world.width) / zoomFactor;
		    float visibleWorldHeight = static_cast<float>(world.height) / zoomFactor;
		    
		    // We assume we're viewing the CENTER of the world
		    float visibleWorldCenterX = static_cast<float>(world.left) + static_cast<float>(world.width) / 2.0f;
		    float visibleWorldCenterY = static_cast<float>(world.top) + static_cast<float>(world.height) / 2.0f;
		    
		    // But the visible rectangle should be centered at this point
		    float visibleWorldLeft = visibleWorldCenterX - visibleWorldWidth / 2.0f;
		    float visibleWorldTop = visibleWorldCenterY - visibleWorldHeight / 2.0f;
		    
		    // Calculate scale to fit visible world into port
		    float scaleX = static_cast<float>(port.width) / visibleWorldWidth;
		    float scaleY = static_cast<float>(port.height) / visibleWorldHeight;
		    
		    float portCenterX = static_cast<float>(port.left) + port.width / 2.0f;
		    float portCenterY = static_cast<float>(port.top) + port.height / 2.0f;
		    
		    // Transform from visible world to port
		    matrix.Translate(portCenterX, portCenterY);
		    
		    if (rotation != 0) {
		        matrix.Rotate(rotation);
		    }
		    
		    matrix.Scale(scaleX, scaleY);
		    matrix.Translate(-visibleWorldCenterX, -visibleWorldCenterY);			




		}
			
			
			
		float rotation = 0;
		ws::IntRect port; //Port is always in screen coordinates.
		ws::IntRect world; //World is the world coordinate section of the world that is sent to the view.
		ws::Vec2i portOrigin;//This is the point of rotation. It does NOT effect the view position.
		Gdiplus::Matrix matrix;
		float zoom = 0;
		
		
		
		 	
	};




	
	
	class Texture
	{
		private:
		int width = 0;
		int height = 0;

		public:

		
		Gdiplus::Bitmap* bitmap;
		
		
		
		
		Texture() : bitmap(nullptr) {}
		
		
		Texture(std::string path)
		{
			loadFromFile(path);
		}
		
		
	    ~Texture()
	    {
	        if (bitmap != nullptr)
	        {
	            delete bitmap;
	            bitmap = nullptr;
	        }
	    }		
		


		//move
	    Texture(Texture&& other) noexcept
	        : bitmap(other.bitmap), width(other.width), height(other.height)
	    {
	        other.bitmap = NULL;
	        other.width = 0;
	        other.height = 0;
	    }

		//copy
	    Texture(const Texture& other) : bitmap(nullptr), width(0), height(0)
	    {
	        if (other.bitmap && other.width > 0 && other.height > 0) {
	            bitmap = other.bitmap->Clone(0, 0, other.width, other.height, PixelFormat32bppARGB);
	            width = other.width;
	            height = other.height;
	        }
	    }


		//Copy assignment
	    Texture& operator=(const Texture& other)
	    {
	        if (this != &other)
	        {
	            // Clean up existing bitmap
	            if (bitmap != nullptr)
	            {
	                delete bitmap;
	                bitmap = nullptr;
	            }
	            
	            // Copy from other if it has valid bitmap
	            if (other.bitmap && other.width > 0 && other.height > 0)
	            {
	                bitmap = other.bitmap->Clone(0, 0, other.width, other.height, PixelFormat32bppARGB);
	                width = other.width;
	                height = other.height;
	            }
	            else
	            {
	                bitmap = nullptr;
	                width = 0;
	                height = 0;
	            }
	        }
	        return *this;
	    }


	    Texture& operator=(Texture&& other) noexcept
	    {
	        if (this != &other)
	        {
	            if (bitmap != NULL)
	            {
	                delete bitmap;
	            }
	            
	            bitmap = other.bitmap;
	            width = other.width;
	            height = other.height;
	            
	            other.bitmap = nullptr;
	            other.width = 0;
	            other.height = 0;
	        }
	        return *this;
	    }		
		

	
		bool create(int w,int h,Gdiplus::Color color = {0,0,0,0})
		{
			
			bitmap = new Gdiplus::Bitmap(w, h, PixelFormat32bppARGB);
			
			Gdiplus::Graphics* gr;
			gr = new Gdiplus::Graphics(bitmap);
			gr->Clear(color);
			
			
			if(gr)
			{
				delete gr;
				gr = nullptr;
			}
			if(!bitmap)
				return false;
				
			width = w;
			height = h;
			return true;
		}
	
		
		
		bool loadFromFile(std::string path)
		{


	        if (bitmap != NULL)
	        {
	            delete bitmap;
	            bitmap = nullptr;
	            width = 0;
	            height = 0;
	        }


			if(!ResolveRelativePath(path))
				return false;


			std::wstring wpath = WIDE(path);
			bitmap = Gdiplus::Bitmap::FromFile(wpath.c_str());
			
			
			
			if(bitmap == nullptr || bitmap->GetLastStatus() != Gdiplus::Ok)
			{
				std::cerr << "Failed to load image at " << std::quoted(path) << ".\n";
				if (bitmap != nullptr)
				{
					delete bitmap;
					bitmap = nullptr;
				}
				return false;
			}
			
			width = bitmap->GetWidth();
			height = bitmap->GetHeight();		
			
			return true;
		}


		bool loadFromMemory(const void* buffer,size_t bufferSize)
		{
		    // create a global memory object
		    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, bufferSize);
		    if (!hGlobal) {
		        return false;
		    }
		    
		    // copy the data to the global memory
		    void* pData = GlobalLock(hGlobal);
		    if (!pData) {
		        GlobalFree(hGlobal);
		        return false;
		    }
		    
		    memcpy(pData, buffer, bufferSize);
		    GlobalUnlock(hGlobal);
		    
		    // Create IStream from the global memory
		    IStream* pStream = nullptr;
		    HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
		    if (FAILED(hr)) {
		        GlobalFree(hGlobal);
		        return false;
		    }
		    
		    bitmap = Gdiplus::Bitmap::FromStream(pStream);
		    pStream->Release();
		    
		    if (bitmap == nullptr || bitmap->GetLastStatus() != Gdiplus::Ok) {
		        if (bitmap) {
		            delete bitmap;
		            bitmap = nullptr;
		        }
		        return false;
		    }
		    
		    width = bitmap->GetWidth();
		    height = bitmap->GetHeight();
		    return true;
		}

		
		
		bool loadFromBitmapPlus(Gdiplus::Bitmap &pBitmap)
		{
			UINT width = pBitmap.GetWidth();
			UINT height = pBitmap.GetHeight();
			bitmap = pBitmap.Clone(0, 0, width, height, PixelFormat32bppARGB);
			return true;
		}
		
		
		
		
		
		


		
	    bool isValid() const
	    {
	        return bitmap != nullptr;
	    }		
	    
	    
	    
	    
	    
	    
	    
	    
		void setPixel(int index,Gdiplus::Color color)
		{
			int x = index % width;
		    int y = index / width;
			setPixel(x,y,color);
		}
	    
	    void setPixel(int xIndex,int yIndex,Gdiplus::Color color)
	    {
			bitmap->SetPixel(xIndex, yIndex, color);
		}
	    
	    
	    Gdiplus::Color getPixel(int index)
		{
			int x = index % width;
		    int y = index / width;
			return getPixel(x,y);
		}
	    
	    
	    Gdiplus::Color getPixel(int xIndex,int yIndex)
	    {
	    	Gdiplus::Color color;
			bitmap->GetPixel(xIndex, yIndex, &color);
			return color;
		}
	    
	    
	    
	    
	    ws::Vec2i getSize()
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
		    std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
		    
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
	
		


	class GIF
	{
		public:
		
		
		int width = 0;
		int height = 0;
		
		
		
		
		
		GIF()
		{
			
		}
		
		~GIF() {
		    textures.clear();
		    delays.clear();
		}
		
		
		
		bool loadFromFile(std::string path)
		{
			
			
	        Gdiplus::Image* gif = Gdiplus::Image::FromFile(ws::WIDE(path).c_str(), FALSE);
			
			
			if(!gif || gif->GetLastStatus() != Gdiplus::Ok) 
				return false;
			
			
	        UINT dimensions = 0;
	        GUID dimensionID = Gdiplus::FrameDimensionTime;
	        dimensions = gif->GetFrameDimensionsCount();
			
			if(dimensions == 0)
			{
				delete gif;
				return false;
			}
			
	        std::vector<GUID> dims(dimensions);
	        gif->GetFrameDimensionsList(dims.data(), dimensions);
	        
	        totalFrames = static_cast<int>(gif->GetFrameCount(&dims[0]));
			
			
			if(totalFrames <= 0)
			{
				delete gif;
				return false;
			}
			
			
	        int width = gif->GetWidth();
	        int height = gif->GetHeight();			
			
			
		    textures.clear();
		    delays.clear();
		    textures.reserve(totalFrames);
		    delays.reserve(totalFrames);
		    
		    
		    std::unique_ptr<Gdiplus::PropertyItem> pItem;
		    
		    
		    
		    
	        UINT totalBufferSize = gif->GetPropertyItemSize(PropertyTagFrameDelay);
	        if (totalBufferSize > 0)
	        {
	            pItem.reset(static_cast<Gdiplus::PropertyItem*>(std::malloc(totalBufferSize)));
	            if (pItem && gif->GetPropertyItem(PropertyTagFrameDelay, totalBufferSize, pItem.get()) == Gdiplus::Ok)
	            {
	                UINT* uintDelays = static_cast<UINT*>(pItem->value);
	                for (int a = 0; a < totalFrames; a++)
	                {
	                    delays.push_back(static_cast<double>(uintDelays[a] * 10));// Was in 100ths of a second. Now is in milliseconds.
	                }
	            }
	        }		    
	        
	        //Load the frames into textures.
	        for (int a = 0; a < totalFrames; a++)
	        {
	            GUID pageID = Gdiplus::FrameDimensionTime;
	            gif->SelectActiveFrame(&pageID, a);
	            
	            ws::Texture newtex;
	            if (!newtex.create(width, height))
	                return false;
	            
	            // Draw frame to the bitmap
	            Gdiplus::Graphics graphics(newtex.bitmap);
	            
				graphics.DrawImage(gif,0, 0, width, height);
	            
	            textures.push_back(std::move(newtex));
	        }
	        
	        delete gif;
	        
	        
	        currentFrame = 0;
			currentTexture = textures[0];
			return true;
		}
		
		
		
		void addFrame(ws::Texture &newFrame,double millisecondDelay)
		{
			
			if(newFrame.getSize().x > width)
				width = newFrame.getSize().x;
			if(newFrame.getSize().y > height)
				height = newFrame.getSize().y;
			
			textures.push_back(std::move(newFrame));
			delays.push_back(millisecondDelay);
				
			totalFrames++;
		}
		
		
		
		
		ws::Texture& getTexture()
		{
			return currentTexture;
		}
		
		
		ws::Texture* getFrame(int index)
		{
			if(index < 0 || index > textures.size())
			{
				std::cerr << "Invalid texture frame ID requested in getFrame() from ws::Animate! Returned Invalid!...\n";
				return nullptr;
			}
			
			return &textures[index];
		}
		
		
		double getFrameDelay(int index)
		{
			if(index <= 0 || index > textures.size())
			{
				std::cerr << "Invalid texture frame ID requested in getFrameDelay() from ws::Animate! Returned 0...\n";
				return 0;
			}
			return delays[index];
		}
		
		
		bool setFrameDelay(int index,double newDelay)
		{	
			if(index <= 0 || index > textures.size())
				return false;
			delays[index] = newDelay;		
		}
		
		bool setFrame(int index,ws::Texture &newTexture)
		{
			if(index <= 0 || index > textures.size())
				return false;
			textures[index] = newTexture;
			return true;
		}
		
		
		
		bool getLoop()
		{
			return loop;
		}
		
		void setLoop(bool trueFalse = true)
		{
			loop = trueFalse;
		}
		
		
		int getFrameCount()
		{
			return totalFrames;
		}
		
		int getCurrentFrame()
		{
			return currentFrame;
		}
		
		
		void play()
		{
			if(status == "stopped")
			{
				timer.restart();
				currentFrame = 0;
				status = "playing";
			}
			if(status == "paused")
			{
				status = "playing";
			}
			
		}
		
		
		void pause()
		{
			status = "paused";
		}
		
		
		void stop()
		{
			currentFrame = 0;
			status = "stopped";
		}
		
		
		std::string getStatus()
		{
			return status;
		}
		
		
		
		
		ws::Texture& update()
		{
			
			if(status == "playing")
			{
			
				if(currentFrame >= totalFrames)
				{
					if(loop)
					{
						timer.restart();
						currentFrame = 0;
					}
					else
						status = "stopped";
				}
				
				if(currentFrame < delays.size() && currentFrame < textures.size() && timer.getMilliSeconds() > delays[currentFrame])
				{	
					currentTexture = textures[currentFrame];	
					currentFrame++;
					timer.restart();
				}	
				
			}
			return currentTexture;
		}
		
		
		
		
		
		private:
	    std::vector<ws::Texture> textures;
	    ws::Texture currentTexture;
	    std::vector<double> delays;  
		std::string status = "stopped";
		
		int currentFrame = 0;
		int totalFrames = 0;
	    bool loop = false;
		ws::Timer timer;
				
		
	};

	




	//SHIFT ANIMATOR 

	class ShiftData
	{
	
	
		public:
		int currentframe=0;
		float delay = 0.15;
		bool ended = false;
		bool start = true;//To make sure that timer is 0 when starting.
		std::vector <ws::IntRect> rect;
		ws::Timer timer;
		
		void add(int left,int top,int width,int height)
		{
			rect.push_back({left,top,width,height});
		}
		
		void add(ws::IntRect rect)
		{
			add(rect.left,rect.top,rect.width,rect.height);
		}
		
	};
	
	
	ws::IntRect Shift(ShiftData &shift) 
	{
		if(shift.start && !shift.ended)
		{
			shift.timer.restart();
			shift.start = false;
		}
		
		if(shift.timer.getSeconds() >= shift.delay)
		{
		
		
				
			shift.currentframe++;
				
			if(shift.currentframe >= shift.rect.size())
			{
				shift.currentframe=0;
				shift.ended = true;
				shift.start = true;
			}
			shift.timer.restart();
		
		}
		
		return shift.rect[shift.currentframe];
		
	}
	
	//////////////////	
		
		
		
		
		
		
		
		
		
		
		
		
	
	
	class Drawable
	{
	public:
	    //Coordinates and sizes are Local 
		
		int x = 0, y = 0, z = 0;
	    int width = 1, height = 1;
	    ws::Vec2f scale = {1, 1};
	    ws::Vec2i origin = {0, 0};
	    //Degrees
	    float rotation = 0.0f;
	    



	    ws::Vec2i getSize() {return ws::Vec2i(width,height);}
	    ws::Vec2i getPosition() {return ws::Vec2i(x,y);}
	    ws::Vec2f getScale() { return scale; }
	    ws::Vec2i getOrigin() { return origin; }
	    float getRotation() { return rotation; }
			    
	    
	    void setSize(ws::Vec2i size) {width = size.x;height = size.y;}
	    void setSize(int w,int h) {width = w;height = h;}
	    void setPosition(ws::Vec2i pos) {x = pos.x;y = pos.y;}
	    void setPosition(int xpos,int ypos) {x = xpos;y = ypos;}
	    void setScale(ws::Vec2f s) { scale = s; }
	    void setScale(float sx, float sy) { scale.x = sx; scale.y = sy; }
	    void setOrigin(ws::Vec2i pos) { origin = pos; }
	    void setOrigin(int posx, int posy) { origin.x = posx; origin.y = posy; }
	    void setRotation(float degrees) { rotation = degrees; }
	    
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
	            float rad = rotation * M_PI / 180.0f;
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
	            float localX = (point.x - x) / scale.x + origin.x;
	            float localY = (point.y - y) / scale.y + origin.y;
	            return (localX >= 0 && localX < width && 
	                    localY >= 0 && localY < height);
	        }
	        
	        // For rotated objects, do proper transform
	        float localX = static_cast<float>(point.x - x);
	        float localY = static_cast<float>(point.y - y);
	        
	        // Reverse rotation
	        float rad = -rotation * M_PI / 180.0f;
	        float cosA = std::cos(rad);
	        float sinA = std::sin(rad);
	        float rotX = localX * cosA - localY * sinA;
	        float rotY = localX * sinA + localY * cosA;
	        
	        // Reverse scale and adjust for origin
	        rotX = rotX / scale.x + origin.x;
	        rotY = rotY / scale.y + origin.y;
	        
	        return (rotX >= 0 && rotX < width && 
	                rotY >= 0 && rotY < height);
	    }
	    
	    
		virtual void drawGlobal(Gdiplus::Graphics* graphics)
	    {
	        // Save current state
	        Gdiplus::GraphicsState state = graphics->Save();
	        
	        Gdiplus::Matrix transform;
	        


		    transform.Translate(static_cast<Gdiplus::REAL>(x), 
		                       static_cast<Gdiplus::REAL>(y));
		    
		    // Apply rotation around the origin point
		    if (rotation != 0.0f) {
		        transform.Translate(static_cast<Gdiplus::REAL>(origin.x * scale.x), 
		                           static_cast<Gdiplus::REAL>(origin.y * scale.y));
		        transform.Rotate(rotation);
		        transform.Translate(static_cast<Gdiplus::REAL>(-origin.x * scale.x), 
		                           static_cast<Gdiplus::REAL>(-origin.y * scale.y));
		    }


		    
		    // Apply scale
		    if (scale.x != 1.0f || scale.y != 1.0f) {        	
		        transform.Scale(scale.x, scale.y);	            
		    }
		    
	        
	        
	        graphics->SetTransform(&transform);
	        
	        // Draw the actual content
	        draw(graphics);
	        
	        
	        graphics->Restore(state);
	    }
	    
	    // Pure virtual - draw the content in local space
	    virtual void draw(Gdiplus::Graphics* graphics) = 0;
	    
	    virtual ~Drawable() = default;
	};
	
	
	
	
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
	    	Gdiplus::Pen pen(color);
			canvas->DrawLine(&pen,start.x,start.y,end.x,end.y);
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
			int(text.length()),
			fontRef->getFamilyHandle(), 
			style, 
			charSize, 
			Gdiplus::PointF(0,0), 
			&format
			);
    
    		Gdiplus::Pen outlinePen(borderColor, borderWidth);
    		outlinePen.SetLineJoin(Gdiplus::LineJoinRound);  
		    
		    Gdiplus::RectF bounds;
		    path.GetBounds(&bounds, NULL, &outlinePen);
		    
		    width = bounds.Width;
		    height = bounds.Height;
		    
		    
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
	                double xIntersection = (p2.x - p1.x) * (point.y - p1.y) / (double)(p2.y - p1.y) + p1.x;
	                
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
	    
	    void setUV(int vertexIndex, float u, float v)
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
	                double xIntersection = (p2.x - p1.x) * (p.y - p1.y) / (double)(p2.y - p1.y) + p1.x;
	                
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
	                float u = static_cast<float>(vertex.x - bounds.left) / bounds.width;
	                float v = static_cast<float>(vertex.y - bounds.top) / bounds.height;
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
	                    
	                    int texX = static_cast<int>(uv.x * (texWidth - 1));
	                    int texY = static_cast<int>(uv.y * (texHeight - 1));
	                    
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
	        float alpha = ((b.y - c.y)*(p.x - c.x) + (c.x - b.x)*(p.y - c.y)) /
	                     ((b.y - c.y)*(a.x - c.x) + (c.x - b.x)*(a.y - c.y));
	        float beta = ((c.y - a.y)*(p.x - c.x) + (a.x - c.x)*(p.y - c.y)) /
	                    ((b.y - c.y)*(a.x - c.x) + (c.x - b.x)*(a.y - c.y));
	        float gamma = 1.0f - alpha - beta;
	        
	        return (alpha >= 0 && beta >= 0 && gamma >= 0);
	    }
	    
	    ws::Vec2f barycentricUV(ws::Vec2i p, ws::Vec2i a, ws::Vec2i b, ws::Vec2i c,
	                           ws::Vec2f uvA, ws::Vec2f uvB, ws::Vec2f uvC)
	    {
	        float denom = (b.y - c.y)*(a.x - c.x) + (c.x - b.x)*(a.y - c.y);
	        if(fabs(denom) < 0.0001f) return uvA;
	        
	        float alpha = ((b.y - c.y)*(p.x - c.x) + (c.x - b.x)*(p.y - c.y)) / denom;
	        float beta = ((c.y - a.y)*(p.x - c.x) + (a.x - c.x)*(p.y - c.y)) / denom;
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
				int resx = static_cast<int>(std::cos(angle) * radius);
				int resy = static_cast<int>(std::sin(angle) * radius);
				poly.addVertex(resx + radius, resy + radius);
			}
			m_points = points;
			
			// Update Drawable properties
			width = 2 * radius;
			height = 2 * radius;

			// Update position based on center
			x = center.x - origin.x;
			y = center.y - origin.y;
		}
		
		
		
		void setPosition(int posx,int posy)
		{
			center = {posx,posy};
			x = center.x - origin.x;
			y = center.y - origin.y;
		}
		
		void setPosition(ws::Vec2i pos)
		{
			setPosition(pos.x, pos.y);
		}
		
		
		
		void move(ws::Vec2i delta)
		{
			center.x += delta.x;
			center.y += delta.y;
			x = center.x - origin.x;
			y = center.y - origin.y;
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
			return (localX * localX + localY * localY) <= (radius * radius);
		}	
		
	
		private:
		ws::Vec2i center = {0, 0};
		int m_points = 500;
		int radius = 10;
	};






	class Round : public ws::Drawable
	{
		public:
			
		
		virtual void draw(Gdiplus::Graphics* canvas) override
		{
	        Gdiplus::Pen borderPen(m_borderColor, static_cast<Gdiplus::REAL>(m_borderWidth));
	        Gdiplus::SolidBrush fillBrush(m_fillColor);
			
			canvas->DrawEllipse(&borderPen,0,0,width,height);
			canvas->FillEllipse(&fillBrush,0,0,width,height);
			
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
	        localX += origin.x;
	        localY += origin.y;
	        
	        // Ellipse equation check
	        float centerX = width / 2.0f;
	        float centerY = height / 2.0f;
	        float radiusX = width / 2.0f;
	        float radiusY = height / 2.0f;
	        
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
	


		
}




// ========== SYSTEM ==========
namespace ws 
{
	


	
	
	
	
	
	class Clipboard
	{
	private:
	    // Helper function to copy a rectangular region from a GDI+ Bitmap
	    Gdiplus::Bitmap* copyRectOfBitmap(ws::Texture &texture, ws::IntRect rect)
	    {
	        if (!texture.isValid()) return nullptr;
	        
	        // Get bitmap dimensions
	        int srcWidth = texture.bitmap->GetWidth();
	        int srcHeight = texture.bitmap->GetHeight();
	        
	        // Determine copy area
	        int copyWidth, copyHeight, copyLeft, copyTop;
	        
	        if (rect.width == 0 && rect.height == 0 && rect.left == 0 && rect.top == 0) {
	            // Copy entire bitmap
	            copyLeft = 0;
	            copyTop = 0;
	            copyWidth = srcWidth;
	            copyHeight = srcHeight;
	        } else {
	            // Copy specified rectangle
	            copyLeft = rect.left;
	            copyTop = rect.top;
	            copyWidth = rect.width;
	            copyHeight = rect.height;
	            
	            // Clamp to bitmap bounds
	            if (copyLeft < 0) copyLeft = 0;
	            if (copyTop < 0) copyTop = 0;
	            if (copyLeft + copyWidth > srcWidth) copyWidth = srcWidth - copyLeft;
	            if (copyTop + copyHeight > srcHeight) copyHeight = srcHeight - copyTop;
	            
	            // Check if rectangle is valid
	            if (copyWidth <= 0 || copyHeight <= 0) return nullptr;
	        }
	        
	        // Create a new bitmap for the copied region
	        Gdiplus::Bitmap* copyBitmap = new Gdiplus::Bitmap(copyWidth, copyHeight, 
	                                                         PixelFormat32bppARGB);
	        
	        // Create graphics context for the new bitmap
	        Gdiplus::Graphics graphics(copyBitmap);
	        
	        // Draw the specified region from source to destination
	        graphics.DrawImage(texture.bitmap, 
	                          0, 0, copyLeft, copyTop, copyWidth, copyHeight, 
	                          Gdiplus::UnitPixel);
	        
	        return copyBitmap;
	    }
	    
	    // Convert GDI+ Bitmap to HBITMAP for clipboard (CF_BITMAP format)
	    HBITMAP gdipBitmapToHBITMAP(Gdiplus::Bitmap* gdipBitmap)
	    {
	        if (!gdipBitmap) return nullptr;
	        
	        HBITMAP hBitmap = nullptr;
	        Gdiplus::Color color(0, 0, 0, 0); // Transparent background
	        Gdiplus::Status status = gdipBitmap->GetHBITMAP(color, &hBitmap);
	        
	        if (status != Gdiplus::Ok) {
	            return nullptr;
	        }
	        
	        return hBitmap;
	    }
	    
	    // Convert GDI+ Bitmap to DIB for clipboard (CF_DIB format)
	    HGLOBAL gdipBitmapToDIB(Gdiplus::Bitmap* gdipBitmap)
	    {
	        if (!gdipBitmap) return nullptr;
	        
	        UINT width = gdipBitmap->GetWidth();
	        UINT height = gdipBitmap->GetHeight();
	        
	        // Lock the bitmap bits
	        Gdiplus::BitmapData bitmapData;
	        Gdiplus::Rect rect(0, 0, width, height);
	        
	        if (gdipBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, 
	                                PixelFormat32bppARGB, &bitmapData) != Gdiplus::Ok) {
	            return nullptr;
	        }
	        
	        // Prepare BITMAPINFOHEADER
	        BITMAPINFOHEADER bi = {0};
	        bi.biSize = sizeof(BITMAPINFOHEADER);
	        bi.biWidth = width;
	        bi.biHeight = height;  // Positive for top-down DIB
	        bi.biPlanes = 1;
	        bi.biBitCount = 32;
	        bi.biCompression = BI_RGB;
	        bi.biSizeImage = width * height * 4;
	        
	        // Calculate total size needed
	        DWORD dwSize = sizeof(BITMAPINFOHEADER) + bi.biSizeImage;
	        
	        // Allocate global memory
	        HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwSize);
	        if (!hGlobal) {
	            gdipBitmap->UnlockBits(&bitmapData);
	            return nullptr;
	        }
	        
	        // Lock the global memory
	        LPBYTE lpData = (LPBYTE)GlobalLock(hGlobal);
	        if (!lpData) {
	            GlobalFree(hGlobal);
	            gdipBitmap->UnlockBits(&bitmapData);
	            return nullptr;
	        }
	        
	        // Copy BITMAPINFOHEADER
	        memcpy(lpData, &bi, sizeof(BITMAPINFOHEADER));
	        
	        // Copy pixel data (convert from bottom-up to top-down if needed)
	        LPBYTE pDest = lpData + sizeof(BITMAPINFOHEADER);
	        LPBYTE pSrc = (LPBYTE)bitmapData.Scan0;
	        
	        // Copy scanline by scanline (bitmapData is bottom-up, DIB we want top-down)
	        for (UINT y = 0; y < height; y++) {
	            // Copy entire scanline
	            memcpy(pDest + (y * width * 4), 
	                   pSrc + ((height - 1 - y) * bitmapData.Stride), 
	                   width * 4);
	        }
	        
	        // Cleanup
	        gdipBitmap->UnlockBits(&bitmapData);
	        GlobalUnlock(hGlobal);
	        
	        return hGlobal;
	    }
	    
	    // Convert HBITMAP to GDI+ Bitmap
	    Gdiplus::Bitmap* hbitmapToGdipBitmap(HBITMAP hBitmap)
	    {
	        if (!hBitmap) return nullptr;
	        
	        // Create GDI+ bitmap from HBITMAP
	        Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromHBITMAP(hBitmap, NULL);
	        
	        // If FromHBITMAP fails, try creating a new bitmap and copying
	        if (!bitmap || bitmap->GetLastStatus() != Gdiplus::Ok) {
	            if (bitmap) delete bitmap;
	            
	            // Get bitmap info
	            BITMAP bm;
	            GetObject(hBitmap, sizeof(BITMAP), &bm);
	            
	            // Create a new GDI+ bitmap
	            bitmap = new Gdiplus::Bitmap(bm.bmWidth, bm.bmHeight, PixelFormat32bppARGB);
	            
	            // Create graphics and draw the HBITMAP
	            Gdiplus::Graphics graphics(bitmap);
	            Gdiplus::Bitmap tempBitmap(hBitmap, NULL);
	            graphics.DrawImage(&tempBitmap, 0, 0, bm.bmWidth, bm.bmHeight);
	        }
	        
	        return bitmap;
	    }
	
	
	
	
	
	
		bool OpenClipboardCheck()
		{
	        if (!OpenClipboard(NULL)) {
	            return false;
	        }
			return true;
		}
	
	
	
	
	
	public:
	    Clipboard()
		{}
	    
	    bool copyText(const std::string& str) 
	    {
	        const std::wstring text = ws::WIDE(str);
	        
	        if (!OpenClipboardCheck()) {
	            return false;
	        }
	        
	        EmptyClipboard();
	        
	        size_t length = (text.length() + 1) * sizeof(wchar_t);
	        HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, length);
	        if (!hGlobal) {
	            CloseClipboard();
	            return false;
	        }
	        
	        LPWSTR pGlobal = (LPWSTR)GlobalLock(hGlobal);
	        if (!pGlobal) {
	            GlobalFree(hGlobal);
	            CloseClipboard();
	            return false;
	        }
	        
	        wcsncpy_s(pGlobal, length / sizeof(wchar_t), text.c_str(), text.length());
	        GlobalUnlock(hGlobal);
	        
	        SetClipboardData(CF_UNICODETEXT, hGlobal);
	        CloseClipboard();
	        return true;
	    }
	    
	    std::string pasteText() 
	    {
	        if (!OpenClipboardCheck()) {
	            return "";
	        }
				        
	        std::string result;
	        
	        if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
	            HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	            if (hData) {
	                LPCWSTR pGlobal = (LPCWSTR)GlobalLock(hData);
	                if (pGlobal) {
	                    result = ws::SHORT(pGlobal);
	                    GlobalUnlock(hData);
	                }
	            }
	        }
	        
	        CloseClipboard();
	        return result;
	    }
	    
	    bool copyTexture(ws::Texture &texture, ws::IntRect rect = {0,0,0,0}) 
	    {
	        if (!texture.isValid()) return false;
	        
	        // Create a copy of the specified region
	        Gdiplus::Bitmap* copyBitmap = copyRectOfBitmap(texture, rect);
	        if (!copyBitmap) return false;
	        
	        
	        if (!OpenClipboardCheck()) {
	            delete copyBitmap;
				return false;
	        }
	        
	        
	        EmptyClipboard();
	        
	        bool success = false;
	        
	        // Try DIB format first (most compatible)
	        HGLOBAL hDib = gdipBitmapToDIB(copyBitmap);
	        if (hDib && SetClipboardData(CF_DIB, hDib)) {
	            success = true;
	        } else if (hDib) {
	            GlobalFree(hDib);
	        }
	        
	        // Also try BITMAP format
	        HBITMAP hBitmap = gdipBitmapToHBITMAP(copyBitmap);
	        if (hBitmap && SetClipboardData(CF_BITMAP, hBitmap)) {
	            success = true;
	        } else if (hBitmap) {
	            DeleteObject(hBitmap);
	        }
	        
	        delete copyBitmap;
	        CloseClipboard();
	        
	        return success;
	    }
	    
	    
	    ws::Texture pasteTexture(ws::IntRect rect = {0,0,0,0}) 
	    {
	        ws::Texture tex;
	        
	        if (!OpenClipboardCheck()) {
	            return tex;
	        }
	        
	        // Try to get DIB first (better quality)
	        if (IsClipboardFormatAvailable(CF_DIB)) {
	            HANDLE hData = GetClipboardData(CF_DIB);
	            if (hData) {
	                LPVOID pData = GlobalLock(hData);
	                if (pData) {
	                    BITMAPINFOHEADER* bih = (BITMAPINFOHEADER*)pData;
	                    
	                    // Calculate pixel data offset
	                    LPBYTE pPixels = (LPBYTE)pData + bih->biSize;
	                    
	                    // Create GDI+ bitmap from DIB
	                    Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(
	                        bih->biWidth, 
	                        abs(bih->biHeight), 
	                        bih->biWidth * 4, 
	                        PixelFormat32bppARGB, 
	                        pPixels);
	                    
	                    if (bitmap && bitmap->GetLastStatus() == Gdiplus::Ok) {
	                        tex.create(bitmap->GetWidth(),bitmap->GetHeight());
	                        tex.bitmap = bitmap;
	                    } else {
	                        delete bitmap;
	                    }
	                    
	                    GlobalUnlock(hData);
	                }
	            }
	        }
	        // Fall back to BITMAP format
	        else if (IsClipboardFormatAvailable(CF_BITMAP)) {
	            HBITMAP hClipboardBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
	            if (hClipboardBitmap) {
	                Gdiplus::Bitmap* clipboardBitmap = hbitmapToGdipBitmap(hClipboardBitmap);
	                if (clipboardBitmap && clipboardBitmap->GetLastStatus() == Gdiplus::Ok) {
	                    
						tex.create(clipboardBitmap->GetWidth(),clipboardBitmap->GetHeight());
						tex.bitmap = clipboardBitmap;
	                } else {
	                    delete clipboardBitmap;
	                }
	            }
	        }
	        
	        CloseClipboard();
	        
	        // Apply rectangle cropping if needed and we have a valid texture
	        if (tex.isValid() && rect.width > 0 && rect.height > 0) {
	            Gdiplus::Bitmap* croppedBitmap = copyRectOfBitmap(tex, rect);
	            if (croppedBitmap) {
	                delete tex.bitmap;
	                tex.create(croppedBitmap->GetWidth(),croppedBitmap->GetHeight());
					tex.bitmap = croppedBitmap;
	                
	            }
	        }
	        
	        return tex;
	    }


	    bool copyFile(const std::string& filePath)
	    {
	        std::wstring widePath = ws::WIDE(filePath);
	        
	        if (!OpenClipboardCheck()) {
	            return false;
	        }
	        
	        EmptyClipboard();
	        
	        // Calculate required memory size
	        size_t pathSize = (widePath.length() + 1) * sizeof(wchar_t);
	        size_t dropFilesSize = sizeof(DROPFILES) + pathSize + sizeof(wchar_t); // +1 wchar_t for double null terminator
	        
	        // Allocate global memory
	        HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, dropFilesSize);
	        if (!hGlobal) {
	            CloseClipboard();
	            return false;
	        }
	        
	        // Lock and prepare DROPFILES structure
	        DROPFILES* pDropFiles = (DROPFILES*)GlobalLock(hGlobal);
	        if (!pDropFiles) {
	            GlobalFree(hGlobal);
	            CloseClipboard();
	            return false;
	        }
	        
	        // Initialize DROPFILES structure
	        pDropFiles->pFiles = sizeof(DROPFILES);  // Offset to file list
	        pDropFiles->pt = { 0, 0 };              // Drop point (unused)
	        pDropFiles->fNC = FALSE;                 // Client area
	        pDropFiles->fWide = TRUE;                // Unicode strings
	        
	        // Copy file path(s) after the DROPFILES structure
	        wchar_t* pFileList = (wchar_t*)((BYTE*)pDropFiles + sizeof(DROPFILES));
	        wcscpy_s(pFileList, widePath.length() + 1, widePath.c_str());
	        
	        // Double null-terminate the file list
	        pFileList[widePath.length() + 1] = L'\0';
	        
	        GlobalUnlock(hGlobal);
	        
	        // Set clipboard data
	        bool success = SetClipboardData(CF_HDROP, hGlobal) != NULL;
	        CloseClipboard();
	        
	        if (!success) {
	            GlobalFree(hGlobal);
	        }
	        
	        return success;
	    }
	    

	    bool copyFiles(const std::vector<std::string>& filePaths)
	    {
	        if (filePaths.empty()) {
	            return false;
	        }
	        
	        if (!OpenClipboardCheck()) {
	            return false;
	        }
	        
	        EmptyClipboard();
	        
	        // Calculate total size needed
	        size_t totalPathsSize = 0;
	        std::vector<std::wstring> widePaths;
	        for (const auto& path : filePaths) {
	            std::wstring widePath = ws::WIDE(path);
	            widePaths.push_back(widePath);
	            totalPathsSize += (widePath.length() + 1) * sizeof(wchar_t);
	        }
	        
	        size_t dropFilesSize = sizeof(DROPFILES) + totalPathsSize + sizeof(wchar_t); // +1 for final null
	        
	        // Allocate global memory
	        HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE, dropFilesSize);
	        if (!hGlobal) {
	            CloseClipboard();
	            return false;
	        }
	        
	        // Lock and prepare DROPFILES structure
	        DROPFILES* pDropFiles = (DROPFILES*)GlobalLock(hGlobal);
	        if (!pDropFiles) {
	            GlobalFree(hGlobal);
	            CloseClipboard();
	            return false;
	        }
	        
	        // Initialize DROPFILES structure
	        pDropFiles->pFiles = sizeof(DROPFILES);
	        pDropFiles->pt = { 0, 0 };
	        pDropFiles->fNC = FALSE;
	        pDropFiles->fWide = TRUE;
	        
	        // Copy all file paths
	        wchar_t* pFileList = (wchar_t*)((BYTE*)pDropFiles + sizeof(DROPFILES));
	        size_t offset = 0;
	        
	        for (const auto& widePath : widePaths) {
	            wcscpy_s(pFileList + offset, widePath.length() + 1, widePath.c_str());
	            offset += widePath.length() + 1;  // +1 for null terminator
	        }
	        
	        // Final null terminator
	        pFileList[offset] = L'\0';
	        
	        GlobalUnlock(hGlobal);
	        
	        // Set clipboard data
	        bool success = SetClipboardData(CF_HDROP, hGlobal) != NULL;
	        CloseClipboard();
	        
	        if (!success) {
	            GlobalFree(hGlobal);
	        }
	        
	        return success;
	    }
	    

	    std::vector<std::string> pasteFiles()
	    {
	        std::vector<std::string> filePaths;
	        
	        if (!OpenClipboardCheck()) {
	            return filePaths;
	        }
	        
	        if (IsClipboardFormatAvailable(CF_HDROP)) {
	            HANDLE hData = GetClipboardData(CF_HDROP);
	            if (hData) {
	                HDROP hDrop = (HDROP)GlobalLock(hData);
	                if (hDrop) {
	                    // Get number of files
	                    UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
	                    
	                    // Get each file path
	                    for (UINT i = 0; i < fileCount; i++) {
	                        // Get required buffer size
	                        UINT bufferSize = DragQueryFileW(hDrop, i, NULL, 0);
	                        
	                        if (bufferSize > 0) {
	                            std::wstring widePath(bufferSize + 1, L'\0');
	                            DragQueryFileW(hDrop, i, &widePath[0], bufferSize + 1);
	                            filePaths.push_back(ws::SHORT(widePath));
	                        }
	                    }
	                    
	                    GlobalUnlock(hData);
	                }
	            }
	        }
	        
	        CloseClipboard();
	        return filePaths;
	    }
	    
	    std::string pasteFile()
	    {
	        auto files = pasteFiles();
	        if (!files.empty()) {
	            return files[0];
	        }
	        return "";
	    }
	    

	    bool hasFiles()
	    {
	        if (!OpenClipboardCheck()) {
	            return false;
	        }
	        bool hasFiles = IsClipboardFormatAvailable(CF_HDROP);
	        CloseClipboard();
	        return hasFiles;
	    }


	    
	    bool hasText() 
	    {
	        if (!OpenClipboardCheck()) {
	            return false;
	        }
	        bool hasText = IsClipboardFormatAvailable(CF_UNICODETEXT);
	        CloseClipboard();
	        return hasText;
	    }
	    
	    bool hasTexture() 
	    {
	        if (!OpenClipboardCheck()) {
	            return false;
	        }
	        bool hasImage = IsClipboardFormatAvailable(CF_BITMAP);
	        CloseClipboard();
	        return hasImage;
	    }
	    
	    bool clear() 
	    {
	        if (!OpenClipboardCheck()) {
	            return false;
	        }
	        bool success = EmptyClipboard();
	        CloseClipboard();
	        return success;
	    }
	    
	}clipboard;
	
	
	
	
	
	class GDIPLUS
	{
		public:
			
		Gdiplus::GdiplusStartupInput gdiplusstartup;
		ULONG_PTR gdiplustoken;
		
		GDIPLUS()
		{
			Gdiplus::GdiplusStartup(&gdiplustoken,&gdiplusstartup,nullptr);
		}
		
		~GDIPLUS()
		{
			Gdiplus::GdiplusShutdown(gdiplustoken);
		}
		
	}gdi;




	
	
		
	class Window
	{
		public:
		
		HWND hwnd;	

		private:
			
		bool isRunning = false;
		
		std::queue<MSG> msgQ;
		
		INITCOMMONCONTROLSEX icex;
		
				
		public:		
		
		
		ws::View view;
		
		ws::Texture backBuffer;
	    Gdiplus::Graphics* canvas;

		
		Window()
		{
			canvas = nullptr;
        	hwnd = nullptr;
		}
		
		
		
		Window(int width,int height,std::string title,DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0)
		{
			create(width,height,title,style,exStyle);
		}
		
		
		
		Window(HWND otherHwnd)
		{
			createFromHwnd(otherHwnd);	
		}
		
		
		
		bool createFromHwnd(HWND otherHwnd)
		{
			if(!otherHwnd)
			{
				std::cerr << "Failed to create window from HWND!\n";
				return false;
			}
			
			
			WINDOWINFO winInfo;
            winInfo.cbSize = sizeof(WINDOWINFO);
            
            if (!GetWindowInfo(otherHwnd, &winInfo))
            {
            	std::cerr << "Failed to create window from HWND!\n";
				return false;
			}
			
			
			
			char title[256];
            GetWindowTextA(otherHwnd, title, sizeof(title));
			
			
			int width = winInfo.rcClient.right - winInfo.rcClient.left;
			int height = winInfo.rcClient.bottom - winInfo.rcClient.top;
			create(width,height,std::string(title),winInfo.dwStyle,winInfo.dwExStyle);
			if(winInfo.dwWindowStatus == WS_ACTIVECAPTION)
				setVisible(true);
			else
				setVisible(false);	
			
			return true;
		}
		
		
		
		Window(Window &other)
		{
			isRunning = other.isRunning;
	        msgQ = other.msgQ;
	        
	        view = other.view;
	        
	        isFullscreen = other.isFullscreen;
	        windowedRect = other.windowedRect;
	        
			

	        
	        // Copy icex
	        icex = other.icex;
	        
	        
		    if (other.hwnd) 
		    {
		        // Get the other window's properties
		        RECT rect;
		        GetWindowRect(other.hwnd, &rect);
		        
		        char title[256];
		        GetWindowTextA(other.hwnd, title, sizeof(title));
		        
		        DWORD style = GetWindowLong(other.hwnd, GWL_STYLE);
		        DWORD exStyle = GetWindowLong(other.hwnd, GWL_EXSTYLE);
		        
		        // Create a new window
		        create(
		            other.getSize().x,
		            other.getSize().y,
		            std::string(title),
		            style,
		            exStyle
		        );
		        
		        // Copy the view
		        view = other.view;
		        
		        backBuffer = other.backBuffer;
		        
		        setVisible(other.getVisible());
		        if(other.hasFocus())
					setFocus();
		        
		        
		        if(other.getFullscreen())
		        	setFullscreen(true);
		        
//		        // Copy backbuffer
//		        if (other.backBuffer.isValid() && backBuffer.isValid()) {
//		            delete backBuffer.bitmap;
//		            backBuffer.bitmap = other.backBuffer.bitmap->Clone(
//		                0, 0, other.backBuffer.width, other.backBuffer.height, 
//		                PixelFormat32bppARGB
//		            );
//		            backBuffer.width = other.backBuffer.width;
//		            backBuffer.height = other.backBuffer.height;
//		        }
		    }
		}
		



		void create(int width,int height,std::string title,DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0)
		{
			//Note to self: the style must be set this way because hwnd has not been initialized yet!
			style |= WS_CLIPCHILDREN;
			exStyle |= WS_EX_COMPOSITED;
			
			
			//This is for initialization of winapi child objects sucg as buttons and textboxes.
			icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
			icex.dwICC = ICC_STANDARD_CLASSES;  // Enables a set of common controls.
			InitCommonControlsEx(&icex);
			///////////////////////////////
			
			
			view.init({0,0,width,height});
			
			
			isRunning = true;
			
			
			HINSTANCE hInstance = GetModuleHandle(nullptr);
			
		    LPCSTR CLASS_NAME = "Window";
		    
		    WNDCLASS wc = {};
		    wc.lpfnWndProc = ws::Window::StaticWindowProc;
		    wc.hInstance = hInstance;
		    wc.lpszClassName = CLASS_NAME;
		    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		
		    if (!RegisterClass(&wc)) {
		        std::cerr << "Failed to register window class!" << std::endl;
		        exit(-1);
		    }		
			
			
			
			hwnd = CreateWindowEx(
			exStyle,
			CLASS_NAME,
			TO_LPCSTR(title),
			style,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			width,
			height,
			nullptr,
			nullptr,
			hInstance,
			this
			);
			
			
		    if (hwnd == nullptr) {
		        std::cerr << "Failed to create window!" << std::endl;
		        exit(-1);
		    }			
			
				
			
			
			backBuffer.create(view.getSize().x,view.getSize().y); 
			canvas = new Gdiplus::Graphics(backBuffer.bitmap);
			

            ShowWindow(hwnd, SW_SHOW);
            UpdateWindow(hwnd);

			
			windowInstances[hwnd] = this;			


			isRunning = true;
			
			setVisible(true);
			setFocus();
						
		}


		
		

        ~Window()
        {
		    if (canvas) {
		        delete canvas;
		        canvas = nullptr;
		    }
		    if (backBuffer.bitmap) {
		        delete backBuffer.bitmap;
		        backBuffer.bitmap = nullptr;
		    }
        }
		
		
		void close()
		{
		    if (hwnd && IsWindow(hwnd)) {
		        DestroyWindow(hwnd);
		    }
		    isRunning = false;
		    
		    
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
			0, 
			0, 
			0, 
			screenWidth, 
			screenHeight,
			SWP_FRAMECHANGED | SWP_SHOWWINDOW);
            
		}
		
		
		ws::Vec2i getSize()
		{
  
            RECT rect;
            GetWindowRect(hwnd, &rect);  
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
			0, 
			posx, 
			posy, 
			0, 
			0,
			SWP_FRAMECHANGED | SWP_SHOWWINDOW);			
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
		
	    


	    void setFullscreen(bool fullscreen) 
		{
	        if (fullscreen == isFullscreen) return;
	        
	        if (fullscreen) 
			{
	            windowedStyle = getExStyle();
	            
				GetWindowRect(hwnd, &windowedRect);
	            
	            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	            int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	            
	            
	            removeStyle(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
	            addStyle(WS_POPUP | WS_VISIBLE);
	            setSize(screenWidth,screenHeight);
	            
	            isFullscreen = true;
	        } else {
	            setAllStyle(windowedStyle);
	            
	            setPosition(windowedRect.left,windowedRect.top);
	            setSize(windowedRect.right - windowedRect.left,
	            windowedRect.bottom - windowedRect.top);
	            
	            isFullscreen = false;
	        }
	    }
	    
	    
	    bool getFullscreen() const {
	        return isFullscreen;
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
		
		
		
				
		
		
		
		bool isOpen()
		{
			
			if(!isRunning || !hwnd)
				return false;
				
	        MSG msg;
	        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
	            if (msg.message == WM_QUIT) {
	                isRunning = false;
	                break;
	            }
	            
	            TranslateMessage(&msg);
	            DispatchMessage(&msg);
	            
	            msgQ.push(msg);
	            
	        }
            
			return isRunning;

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
			if(!hwnd)
				return;
				
			
		    // Clean up old resources
		    if (canvas) {
		        delete canvas;
		        canvas = nullptr;
		    }
		    //Do the cleanup here or suffer memory overload
		    if (backBuffer.bitmap) {
		        delete backBuffer.bitmap;
		        backBuffer.bitmap = nullptr;
		    }
		    backBuffer.create(view.getSize().x,view.getSize().y);
		    canvas = new Gdiplus::Graphics(backBuffer.bitmap);
			canvas->Clear(color);
	    }
		
		
		
		
		
		
		
		
		
		
		
		
		void draw(Drawable &draw)
		{
			if(!canvas || !hwnd) return;





			Gdiplus::Matrix originalMatrix; //Get the original untransformed matrix so that the drawable can be drawn in world coordinates. 
        	canvas->GetTransform(&originalMatrix);
			
			//Apply the transformation
			view.apply(*canvas);
			
			//draw the object in world coords.
			draw.drawGlobal(canvas);
			
			//Restore the original transformation so that the transform can be applied again next time. 
			//This is because changes occur and need to be transformed too.
			canvas->SetTransform(&originalMatrix);
		}
		
		
		
		
		
		
		
	    void display() 
		{
			if(!hwnd)
		    	return;
			InvalidateRect(hwnd, NULL, FALSE);
		    UpdateWindow(hwnd);
	    }		
		
		
		
	
		
		
		
		
		
		private:
						
			
		
		

        // Static map to store window instances
        static std::map<HWND, Window*> windowInstances;		

		
        static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            Window* pWindow = nullptr;
            
            if (uMsg == WM_NCCREATE) {
                CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
                pWindow = reinterpret_cast<Window*>(pCreate->lpCreateParams);
                SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
            } else {
                pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            }
            
            if (pWindow) {
                return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
            }
            
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }	
		
	
        LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {


            switch (uMsg) {
	            case WM_DESTROY:
	                PostQuitMessage(0);
	                isRunning = false;
	                return 0;
	                
	            case WM_PAINT: {
	            	
	            	
	                PAINTSTRUCT ps;
	                HDC hdc = BeginPaint(hwnd, &ps);

					if (backBuffer.bitmap) {
				        
						if (canvas) {
					        delete canvas;
					        canvas = nullptr;
					    }
					    
					    //faster to use bitbltting
						if(hdc)
						{
							HBITMAP hBitmap;
							backBuffer.bitmap->GetHBITMAP(Gdiplus::Color(0, 0, 0), &hBitmap);
							HDC hdcMem = CreateCompatibleDC(hdc);
							HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

				            //SetStretchBltMode(hdc, HALFTONE); //For better quality stretching - Causes blur though! (NOT GOOD FOR CLEAN STRETCHING)
				            SetBrushOrgEx(hdc, 0, 0, NULL);
							
							StretchBlt(hdc,0,0,getSize().x,getSize().y,hdcMem,0,0,view.getSize().x,view.getSize().y,SRCCOPY);
							
							
							SelectObject(hdcMem, hbmOld);
							DeleteDC(hdcMem);
							DeleteObject(hBitmap); 
						}
						
						//much slower when using graphics.drawimage 
//						if (hdc) {
//				            Gdiplus::Graphics graphics(hdc);
//				            graphics.DrawImage(backBuffer.bitmap, 0, 0, width, height);
//				        }
				    }

	              
	                
	                EndPaint(hwnd, &ps);
	                return 0;

	            }
	            

				
				
				
				
	        }
	        
	        
	        //Secondary message adding because not all messages are received always.
		    MSG msg;
		    msg.hwnd = hwnd;
		    msg.lParam = lParam;
		    msg.wParam = wParam;
		    msg.message = uMsg;
		    msgQ.push(msg);
            
            
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        
        
        
        
				
		
		bool isFullscreen = false;
		RECT windowedRect; // Stores window position/size when not fullscreen
    	DWORD windowedStyle; // Stores window style when not fullscreen
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
			
	};
	
	// Initialize the static map
    std::map<HWND, Window*> Window::windowInstances;
    
    

		
}



namespace ws //GLOBAL INPUT
{
    
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
		
		bool getMouseButton(int vmButton)
		{
			if (GetAsyncKeyState(vmButton) & 0x8000)
				return true;
			
			return false;	
		}
		
		bool getKey(char vmKey)
		{
			if (GetAsyncKeyState(vmKey))
			{
				return true;
			}
			return false;
		}
	}
	
}





namespace ws //CHILD WINDOW API
{
	



	class Dropdown
	{
		public:
	    
		Dropdown(int newID, std::string newName)
	    {
	        if (newID != 0) // Leaf items don't need a menu handle
	            handle = CreatePopupMenu();
	        ID = newID;
	        name = newName;
	        isPopup = (newID != 0);
	    }
	    
	    void addItem(int id,DWORD type, std::string itemName)
	    {
	        if (isPopup)
	            AppendMenuA(handle, type, id, ws::TO_LPCSTR(itemName));
	    }
	    
	    void addSubmenu(Dropdown &submenu)
	    {
	        if (isPopup && submenu.isPopup)
	            AppendMenuA(handle, MF_POPUP, (UINT_PTR)submenu.getHandle(), 
	                       ws::TO_LPCSTR(submenu.getName()));
	    }
		
		HMENU getHandle()
		{ return handle; }
		
		std::string getName()
		{ return name;}
		
		int getID()
		{ return ID;}
		
		void addItem(Dropdown drop)
		{
			if (isPopup)
				AppendMenuA(this->handle, MF_STRING, drop.getID(), ws::TO_LPCSTR(drop.getName()));
		}
		
		
		private:
	    HMENU handle = nullptr;
	    int ID = 0;
	    std::string name = "";
	    bool isPopup = false;
			
	};
	
	class Menu
	{
		public:
		HMENU bar;
		
		Menu()
		{
			bar = CreateMenu();
		}
		
		void addDropdown(ws::Dropdown &drop)
		{
			AppendMenuA(bar, MF_POPUP, (UINT_PTR)drop.getHandle(), ws::TO_LPCSTR(drop.getName()));
			
		}
		
		void setWindow(ws::Window &window)
		{
			SetMenu(window.hwnd, bar);	
		}
		
		
		int getEvent(MSG &m)//You can use this for readability or you can use the normal way.
		{
			if(m.message == WM_COMMAND)
				return LOWORD(m.wParam);
			return -1;
		}
		
	};


	
	
	
	int maxControlID = 0;
	
	
	
	
	class Child
	{
		public:
		
		ws::Window *parentRef = nullptr;
		HWND hwnd = NULL;
		DWORD style = WS_TABSTOP | WS_VISIBLE | WS_CHILD;
		DWORD textStyle = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		

		unsigned int controlID = 0;
		COLORREF backgroundColor = RGB(0,0,0);
		COLORREF textColor = RGB(255,255,255);
		COLORREF borderColor = RGB(0,0,0);
		
		private:
		
		int litX = 0,litY = 0,litWidth = 0,litHeight = 0;
		std::string text = "";
		int x = 0,y = 0,width = 100,height = 100;		

		void setPosLit()
		{
			SetWindowPos(hwnd, nullptr, litX, litY, litWidth, litHeight, SWP_NOZORDER | SWP_NOACTIVATE);
		}

		public:
		
		Child()
		{
			controlID = maxControlID+1;
			maxControlID++;
		}

        virtual ~Child()
        {
            if (hwnd && IsWindow(hwnd))
            {
                DestroyWindow(hwnd);
            }
        }				
		
		void setPosition(int xPos,int yPos)
		{
			x = xPos;
			y = yPos;
			
			update();
		}		
		
		void setPosition(ws::Vec2i pos)
		{
			x = pos.x;
			y = pos.y;
			update();
		}
		
		ws::Vec2i getPosition()
		{
			return {x,y};
		}

		ws::Vec2i getLiteralPosition()
		{
			return {litX,litY};
		}
		
		void setSize(ws::Vec2i size)
		{
			width = size.x;
			height = size.y;
			update();
		}
		
		void setSize(int w,int h)
		{
			width = w;
			height = h;
			update();
		}
		
		ws::Vec2i getSize()
		{
			return {width,height};
		}

		ws::Vec2i getLiteralSize()
		{
			return {litWidth,litHeight};
		}

				
		void addStyle(DWORD addedStyle)
		{
            style |= addedStyle;
			if (hwnd)
            {
                SetWindowLong(hwnd, GWL_STYLE, style);
                SetWindowPos(hwnd, NULL, 0, 0, 0, 0, 
                           SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
            update();
		}
		
		void removeStyle(DWORD removedStyle)
		{
			
			style &= ~removedStyle;
			if (hwnd)
            {
                SetWindowLong(hwnd, GWL_STYLE, style);
                SetWindowPos(hwnd, NULL, 0, 0, 0, 0, 
                           SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }	
			update();		
		}
		
        bool hasStyle(DWORD checkStyle)
        {
            if (hwnd)
            {
                DWORD currentStyle = GetWindowLong(hwnd, GWL_STYLE);
                return (currentStyle & checkStyle) != 0;
            }
            return (style & checkStyle) != 0;
        }
		
		void setText(std::string newText)
		{
			update();
			text = newText;
			if (hwnd)
                SetWindowTextA(hwnd, text.c_str());
            
		}
		
		std::string getText()
		{
		    LRESULT textLength = SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0);
		
		    if (textLength <= 0)
		        return "";
		    
		    std::vector<char> buffer(static_cast<size_t>(textLength) + 1);
		
		    SendMessage(hwnd, WM_GETTEXT, static_cast<WPARAM>(buffer.size()), reinterpret_cast<LPARAM>(buffer.data()));
		
		    return std::string(buffer.data());		    
		    
		}
		
		
		
		bool contains(ws::Vec2i point)
		{
			return (point.x >= x  && point.x < x + width && point.y >= 0 && point.y < y + height);
		}
		
		
		
		void update(MSG *getMsg = nullptr)
		{
			
			if(!parentRef)
				return;
			if(!hwnd)
				return;


		    ws::Vec2i windowSize = parentRef->getSize(); 
		    int windowWidth = windowSize.x;
		    int windowHeight = windowSize.y;

		    ws::Vec2i originalSize = parentRef->view.getSize();
		    
		    
		    
		    if (originalSize.x <= 0 || originalSize.y <= 0) 
		        return;
		        
		    if (windowWidth <= 0 || windowHeight <= 0) 
		        return;
		        

			
		    float wScale = static_cast<float>(windowWidth) / originalSize.x;
		    float hScale = static_cast<float>(windowHeight) / originalSize.y;
			
			
			litX = static_cast<int>(x * wScale);
			litY = static_cast<int>(y * hScale);
			litWidth = static_cast<int>(width * wScale);
			litHeight = static_cast<int>(height * hScale);
			
			setPosLit();
			
			
			
			
			
//			
//			if(getMsg != nullptr)
//			{
//				MSG &msg = *getMsg;
//			
//				if(msg.message == WM_DRAWITEM)
//				{
//				    LPDRAWITEMSTRUCT pDrawItem = (LPDRAWITEMSTRUCT)msg.lParam;
//				    
//				    if (pDrawItem->CtlID == controlID)
//				    {
//				        HDC hdc = pDrawItem->hDC;
//				        RECT rc = pDrawItem->rcItem;
//				        
//				        // Draw background
//				        HBRUSH hBrush = CreateSolidBrush(backgroundColor);
//				        FillRect(hdc, &rc, hBrush);
//				        DeleteObject(hBrush);
//				        
//				        // Draw border
//				        if (pDrawItem->itemState & ODS_SELECTED)
//				        {
//				            DrawEdge(hdc, &rc, BDR_SUNKENOUTER, BF_RECT);
//				        }
//				        else
//				        {
//				            DrawEdge(hdc, &rc, BDR_RAISEDINNER, BF_RECT);
//				        }
//				        
//				        // Draw text
//				        SetBkMode(hdc, TRANSPARENT);
//				        SetTextColor(hdc, textColor);
//				        
//				        DrawTextA(hdc, getText().c_str(), -1, &rc, 
//				                 textStyle);
//				        
//				        InvalidateRect(hwnd, NULL, TRUE);
//				    }
//				}	
//			
//						
//			}
				
			
		}
		
		
		void setFillColor(COLORREF color)
	    {
	        backgroundColor = color;
	        if (hwnd)
	            InvalidateRect(hwnd, NULL, TRUE);
	    }
	    
	    void setTextColor(COLORREF color)
	    {
	        textColor = color;
	        if (hwnd)
	            InvalidateRect(hwnd, NULL, TRUE);
	    }
		
		void setBorderColor(COLORREF color)
		{
			borderColor = color;
			if (hwnd)
			    InvalidateRect(hwnd, NULL, TRUE);
		}
		
		
		
		
	};
	







	
	class ClickMenu
	{
		public:
		
		
		
		void addFlag(DWORD newFlag)
		{ flags |= newFlag; }
		
		void removeFlag(DWORD removeFlag)
		{ flags &= ~removeFlag;}
		
		DWORD getFlags()
		{ return flags; }		
		
		
		int getCommand()
		{return command;}
		
		std::vector<std::string> getList()
		{ return list; }
		
		void setList(std::vector<std::string> newList)
		{ list = newList; }
		
		void addItem(std::string item)
		{ list.push_back(item);}
		
		void removeItem(std::string item)
		{	
			for(int a=0;a<list.size();a++)
			{
				if(list[a] == item)
				{
					list.erase(list.begin() + a);
					break;
				}
			}
			
		}
		
		void init(ws::Window &newParent)
		{ parentRef = &newParent;}
		
		ws::Window *getParent()
		{ return parentRef;}
		
		
		
		
		
		bool show(ws::Vec2i mouse)
		{
			if(parentRef == nullptr)
			{
				std::cerr << "Attempted to show a ClickMenu without referencing a parent window! Use Init().\n";
				return false;
			}
			HMENU hMenu = CreatePopupMenu();
			if(!hMenu)
				return false;
			
			for(int a=0;a<list.size();a++)
			{
				AppendMenu(hMenu, MF_STRING, 1+a, list[a].c_str());
            }
            

            POINT pt = mouse;
            ClientToScreen(parentRef->hwnd, &pt);


            // Show context menu and get selection
            command = TrackPopupMenu(
                hMenu, 
                flags,
                pt.x,
                pt.y,
                0,
                parentRef->hwnd,
                NULL
            );

			

            DestroyMenu(hMenu); // Cleanup
            
            
            return true;
		}	
		
		private:
		int command = 0;	
		std::vector<std::string> list;	
		ws::Window *parentRef = nullptr;
		DWORD flags = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD;
	};
	
	
	
	
	
	class FileWindow
	{
		public:
		
		FileWindow()
		{
			
		}
		
		
		void setFileName(std::string file)
		{
			const char* initialText = file.c_str();
		    strncpy(szFile, initialText, sizeof(szFile) - 1);
		    szFile[sizeof(szFile) - 1] = '\0'; 
		}
		
		std::string getFileName()
		{
			std::string file(szFile);
			return file;
		}
		
		
		void setTitle(std::string name)
		{ title = name; }
		std::string getTitle()
		{ return title; }
		
		void addFlag(DWORD newFlag)
		{ flags |= newFlag; }
		
		void removeFlag(DWORD removeFlag)
		{ flags &= ~removeFlag;}
		
		DWORD getFlags()
		{ return flags; }
		
		
		
		
		
		bool open(ws::Window *parent = nullptr)
		{
			if(parent == nullptr)
				ofn.hwndOwner = NULL;
			else
				ofn.hwndOwner = parent->hwnd;
		
			setFileName("");
		    szFile[0] = '\0';
		    ofn.lStructSize = sizeof(OPENFILENAME);
		    ofn.lpstrFilter = "All Files\0*.*\0";
		    ofn.lpstrInitialDir = ws::TO_LPCSTR(getFileName());
		    ofn.lpstrFile = szFile;
		    ofn.nMaxFile = sizeof(szFile);
		    ofn.lpstrTitle = ws::TO_LPCSTR(title);
		    //OFN_NOCHANGEDIR is essential to avoid having the dialog change the program's executable directory.
		    ofn.Flags = flags;
		    ofn.nFilterIndex = defaultFilter;
			ofn.lpstrDefExt = "";

			if(parent)
			{
				MSG m;
		        while(parent->pollEvent(m)) {} // Clear event queue because of the blocking nature of this dialog
			}
			
			if(GetOpenFileName(&ofn))
				return true;
			else
			{
				setFileName("");
				return false;
			}
		}
		
		bool save(ws::Window *parent = nullptr)
		{
			if(parent == nullptr)
				ofn.hwndOwner = NULL;
			else
				ofn.hwndOwner = parent->hwnd;
			
			setFileName("");
		    szFile[0] = '\0';
		    ofn.lStructSize = sizeof(OPENFILENAME);
		    ofn.lpstrFilter = "All Files\0*.*\0";
		    ofn.lpstrInitialDir = ws::TO_LPCSTR(getFileName());
		    ofn.lpstrFile = szFile;
		    ofn.nMaxFile = sizeof(szFile);
		    ofn.lpstrTitle = ws::TO_LPCSTR(title);
		    
		    //OFN_NOCHANGEDIR is essential to avoid having the dialog change the program's executable directory.
		    ofn.Flags = flags;//OFN_NODEREFERENCELINKS allows shortcuts to stay shortcuts
		    ofn.lpstrDefExt = "";
			
			
			if(parent)
			{
				MSG m;
		        while(parent->pollEvent(m)) {} // Clear event queue because of the blocking nature of this dialog
			}
			
			
			
			if(GetSaveFileName(&ofn))
			{
				return true;
			}
			else
			{
				setFileName("");
				return false;				
			}
		}
		
		private:
		
		DWORD flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NODEREFERENCELINKS | OFN_NOCHANGEDIR | OFN_EXPLORER;	
		std::string title = "File Explorer";	
		int defaultFilter = 1;
		char szFile[260];
		
		OPENFILENAME ofn = {0};
	};
	
	class FolderWindow
	{
		public:
		
		FolderWindow()
		{
			
		}
		
		void setTitle(std::string name)
		{title = name;}
		std::string getTitle()
		{return title;}
		void addFlag(DWORD flag)
		{ flags |= flag;}
		void setFlags(DWORD allFlags)
		{ flags = allFlags;}
		void removeFlag(DWORD flag)
		{ flags &= ~flag;}
		DWORD getFlags()
		{return flags;}
		
		std::string getFolderName()
		{ return folderName;}
		
		
		
		
		bool open(ws::Window *parent = nullptr)
		{
		
		    bi.lpszTitle = ws::TO_LPCSTR(title);
		    
		    //OFN_NOCHANGEDIR is essential to avoid having the dialog change the program's executable directory.
		    bi.ulFlags = flags;
			if(parent == nullptr)
				bi.hwndOwner = NULL;
			else
				bi.hwndOwner = parent->hwnd;
		
			if(parent)
			{
				MSG m;
		        while(parent->pollEvent(m)) {} // Clear event queue because of the blocking nature of this dialog
			}
			
		
		    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		    if (pidl != 0) {
		        // Get the name of the folder
		        char path[MAX_PATH];
		        if (SHGetPathFromIDList(pidl, path)) {
		            // Free memory used
		            IMalloc* imalloc = 0;
		            if (SUCCEEDED(SHGetMalloc(&imalloc))) {
		                imalloc->Free(pidl);
		                imalloc->Release();
		            }
		            folderName = std::string(path);
		            return true;
			
		        }
		    }
		    folderName = "";
		    return false;
			
		}		
		
		
		private:
		BROWSEINFO bi = {0};	
		std::string title = "Open Folder";
		DWORD flags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_RETURNFSANCESTORS | OFN_NOCHANGEDIR;	
		std::string folderName = "";
	};
	
	


	class ComboBox : public Child
	{
	    public:
	    ComboBox()
	    {
	    }
	    
	    bool init(ws::Window &parent)
	    {
	        if(!parent.hwnd)
	        {
	            std::cerr << "Child Error: Selected parent is not valid!\n";
	            return false;
	        }
	        
	        parentRef = &parent;
	        
	        // Add combo box specific styles
	        addStyle(CBS_DROPDOWN);
	        addStyle(WS_VSCROLL);
	        
	        hwnd = CreateWindowEx(
	            0,
	            TEXT("COMBOBOX"),
	            NULL,
	            style,
	            getPosition().x,
	            getPosition().y,
	            getSize().x,
	            getSize().y,  // Dropdown height
	            parent.hwnd,
	            (HMENU)(UINT_PTR)controlID,
	            GetModuleHandle(nullptr),
	            nullptr);
	            
	        if (!hwnd)
	        {
	            std::cerr << "Child Error: Failed to create ComboBox!\n";
	            return false;
	        }
	        
	        // Set extended UI for better appearance
	        SendMessage(hwnd, CB_SETEXTENDEDUI, (WPARAM)TRUE, 0);
	        
	        ShowWindow(hwnd, SW_SHOW);
	        UpdateWindow(hwnd);
	        
	        return true;
	    }
	    
	    // Add an item to the dropdown
	    void addItem(const std::string& item)
	    {
	        if (!hwnd) return;
	        SendMessageA(hwnd, CB_ADDSTRING, 0, (LPARAM)item.c_str());
	    }
	    
	    // Add multiple items
	    void addItems(const std::vector<std::string>& items)
	    {
	        if (!hwnd) return;
	        for (const auto& item : items)
	        {
	            addItem(item);
	        }
	    }
	    
	    // Remove an item by index
	    void removeItem(int index)
	    {
	        if (!hwnd) return;
	        SendMessage(hwnd, CB_DELETESTRING, (WPARAM)index, 0);
	    }
	    
	    // Clear all items
	    void clear()
	    {
	        if (!hwnd) return;
	        SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
	    }
	    
	    // Get selected index
	    int getSelectedIndex()
	    {
	        if (!hwnd) return -1;
	        return (int)SendMessage(hwnd, CB_GETCURSEL, 0, 0);
	    }
	    
	    // Set selected index
	    void setSelectedIndex(int index)
	    {
	        if (!hwnd) return;
	        SendMessage(hwnd, CB_SETCURSEL, (WPARAM)index, 0);
	    }
	    
	    // Get selected text
	    std::string getSelectedText()
	    {
	        if (!hwnd) return "";
	        
	        int index = getSelectedIndex();
	        if (index == -1) return "";
	        
	        int length = (int)SendMessage(hwnd, CB_GETLBTEXTLEN, (WPARAM)index, 0);
	        if (length == CB_ERR) return "";
	        
	        std::vector<char> buffer(length + 1);
	        SendMessageA(hwnd, CB_GETLBTEXT, (WPARAM)index, (LPARAM)buffer.data());
	        
	        return std::string(buffer.data());
	    }
	    
	    // Get item count
	    int getItemCount()
	    {
	        if (!hwnd) return 0;
	        return (int)SendMessage(hwnd, CB_GETCOUNT, 0, 0);
	    }
	    
	    // Get item text at index
	    std::string getItemText(int index)
	    {
	        if (!hwnd || index < 0) return "";
	        
	        int length = (int)SendMessage(hwnd, CB_GETLBTEXTLEN, (WPARAM)index, 0);
	        if (length == CB_ERR) return "";
	        
	        std::vector<char> buffer(length + 1);
	        SendMessageA(hwnd, CB_GETLBTEXT, (WPARAM)index, (LPARAM)buffer.data());
	        
	        return std::string(buffer.data());
	    }
	    
	    // Check if selection changed in a message
	    bool selectionChanged(MSG &msg)
	    {
	        if (msg.message == WM_COMMAND && HIWORD(msg.wParam) == CBN_SELCHANGE)
	        {
	            if (LOWORD(msg.wParam) == controlID)
	            {
	                return true;
	            }
	        }
	        return false;
	    }
	    
	    // Set dropdown style (CBS_DROPDOWN or CBS_DROPDOWNLIST)
	    void setDropdownStyle(bool allowEdit = true)
	    {
	        if (!hwnd) return;
	        
	        removeStyle(CBS_DROPDOWN);
	        removeStyle(CBS_DROPDOWNLIST);
	        
	        if (allowEdit)
	        {
	            addStyle(CBS_DROPDOWN);  // Editable combo box
	        }
	        else
	        {
	            addStyle(CBS_DROPDOWNLIST);  // Non-editable combo box
	        }
	        
	        SetWindowLong(hwnd, GWL_STYLE, style);
	        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, 
	                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	    }
	    
	    // Get text from edit control (when using CBS_DROPDOWN)
	    std::string getEditText()
	    {
	        if (!hwnd) return "";
	        
	        LRESULT textLength = SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0);
	        if (textLength <= 0)
	            return "";
	        
	        std::vector<char> buffer(static_cast<size_t>(textLength) + 1);
	        SendMessageA(hwnd, WM_GETTEXT, static_cast<WPARAM>(buffer.size()), 
	                    reinterpret_cast<LPARAM>(buffer.data()));
	        
	        return std::string(buffer.data());
	    }
	    
	    // Set edit text (when using CBS_DROPDOWN)
	    void setEditText(const std::string& text)
	    {
	        if (!hwnd) return;
	        SendMessageA(hwnd, WM_SETTEXT, 0, (LPARAM)text.c_str());
	    }
	};


	
	
	class Button : public Child
	{
		public:
		
		
		Button()
		{	
		}
		
		
		bool init(ws::Window &parent)
		{
			
			if(!parent.hwnd)
			{
				std::cerr << "Child Error: Selected parent is not valid!\n";
				return false;
			}
			
			parentRef = &parent;
			
			
			
			
			hwnd = CreateWindowEx(
			0,
			ws::TO_LPCSTR("BUTTON"),
			ws::TO_LPCSTR(getText()),
			style,
			getPosition().x,
			getPosition().y,
			getSize().x,
			getSize().y,
			parent.hwnd,
            (HMENU)(UINT_PTR)controlID,
            GetModuleHandle(nullptr),
            nullptr);			
			
		    if (!hwnd)
	        {
	            std::cerr << "Child Error: Failed to create Button!\n";
	            return false;
	        }	
		
			ShowWindow(hwnd,SW_SHOW);
			UpdateWindow(hwnd);
		
			return true;		
		}
		
		
		
	    bool isPressed(MSG &msg)
	    {
	    	
	    	
	        if(msg.message == WM_COMMAND && HIWORD(msg.wParam) == BN_CLICKED)
	        {
	            if(LOWORD(msg.wParam) == controlID)
	            {
	                return true;
	            }
	        }
	        return false;
	    }

	};
	



	class Slider : public Child
	{
		public:
		
		
		Slider()
		{
		}
		
		
		bool init(ws::Window &parent)
		{
			
			if(!parent.hwnd)
			{
				std::cerr << "Child Error: Selected parent is not valid!\n";
				return false;
			}
			
			parentRef = &parent;
			
			addStyle(TBS_HORZ);
			addStyle(TBS_AUTOTICKS);
			
			
			hwnd = CreateWindowEx(
			0,
			TRACKBAR_CLASS,
			ws::TO_LPCSTR(getText()),
			style,
			getPosition().x,
			getPosition().y,
			getSize().x,
			getSize().y,
			parent.hwnd,
            (HMENU)(UINT_PTR)controlID,
            GetModuleHandle(nullptr),
            nullptr);			
			
		    if (!hwnd)
	        {
	            std::cerr << "Child Error: Failed to create Slider!\n";
	            return false;
	        }	
		
			ShowWindow(hwnd,SW_SHOW);
			UpdateWindow(hwnd);
			
			setRange(0,100);
		
			return true;		
		}
		
		
		
	    bool getScroll(MSG &msg)
	    {
	    	
	        if((msg.message == WM_HSCROLL || msg.message == WM_VSCROLL) && (HWND)msg.lParam == hwnd)
	        {
	            slidePos = (int)SendMessage(hwnd, TBM_GETPOS, 0, 0);
	            
				return true;
	        }
	        return false;
	    }
	    
	    
	    void setHorizontal()
	    {
	    	removeStyle(TBS_VERT);
	    	addStyle(TBS_HORZ);
	    	update();
		}
		
		void setVertical()
		{
	    	removeStyle(TBS_HORZ);
	    	addStyle(TBS_VERT);	
			update();		
		}
		
        
		void setRange(int minimum = 0,int maximum = 100)
		{
			SendMessage(hwnd, TBM_SETRANGEMIN, TRUE, minimum);
			SendMessage(hwnd, TBM_SETRANGEMAX, TRUE, maximum);
		}
		
		void setSlidePosition(int pos = 0)
		{
			SendMessage(hwnd, TBM_SETPOS, TRUE, pos);
			slidePos = pos; 
		}
		
		int getSlidePosition()
		{
			return slidePos;
		}
		
		
		private:
			int slidePos = 0;
		
	};



	class TextBox : public Child
	{
		public:
		
		bool init(ws::Window &parent)
		{
			
			if(!parent.hwnd)
			{
				std::cerr << "Child Error: Selected parent is not valid!\n";
				return false;
			}
			
			parentRef = &parent;
			
			
			addStyle(ES_AUTOVSCROLL);
			addStyle(ES_AUTOHSCROLL);
			addStyle(ES_MULTILINE);
			
			
			hwnd = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			TEXT("EDIT"),
			ws::TO_LPCSTR(getText()),
			style,
			getPosition().x,
			getPosition().y,
			getSize().x,
			getSize().y,
			parent.hwnd,
            (HMENU)(UINT_PTR)controlID,
            GetModuleHandle(nullptr),
            nullptr);			
			
		    if (!hwnd)
	        {
	            std::cerr << "Child Error: Failed to create Textbox!\n";
	            return false;
	        }	
		
			ShowWindow(hwnd,SW_SHOW);
			UpdateWindow(hwnd);
			
		
			return true;		
		}		
		
		
		bool getFocus()
		{
	        if (!hwnd) return false;
	        return (GetFocus() == hwnd);			
		}
		
	};
	
	
	
	
	
	
	class Label : public Child
	{
		public:
		Label()
		{
			
		}
		
		bool init(ws::Window &parent)
		{
			
			if(!parent.hwnd)
			{
				std::cerr << "Child Error: Selected parent is not valid!\n";
				return false;
			}
			
			addStyle(SS_NOTIFY);
			addStyle(SS_LEFT);
			
			parentRef = &parent;
			
			
			hwnd = CreateWindowEx(
			0,
			TEXT("STATIC"),
			ws::TO_LPCSTR(getText()),
			style,
			getPosition().x,
			getPosition().y,
			getSize().x,
			getSize().y,
			parent.hwnd,
            (HMENU)(UINT_PTR)controlID,
            GetModuleHandle(nullptr),
            nullptr);			
			
		    if (!hwnd)
	        {
	            std::cerr << "Child Error: Failed to create Label!\n";
	            return false;
	        }	
		
			ShowWindow(hwnd,SW_SHOW);
			UpdateWindow(hwnd);
		
			return true;		
		}
				
		
		
		
	};
	
	
	
	
}


#endif