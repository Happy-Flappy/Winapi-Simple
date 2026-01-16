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
	    
	    if (!std::filesystem::exists(filePath)) {
	        std::cerr << "File not found at: " << filePath.string() << std::endl;
	        return false;
	    }
	    path = filePath.string();
	    return true;
	}

	
	std::wstring WIDE(std::string str)
	{
	    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	    std::wstring wstr(size, 0);
	    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
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
		
		
		
		
		[[nodiscard]] ws::Vec2i toWorld(ws::Vec2i screenPos) const
		{
			
	        Gdiplus::Matrix invMatrix;
	        getTransform(invMatrix);
	        invMatrix.Invert();			
			
	        Gdiplus::PointF point(static_cast<Gdiplus::REAL>(screenPos.x),static_cast<Gdiplus::REAL>(screenPos.y));
	        invMatrix.TransformPoints(&point, 1);
	        
	        return ws::Vec2i(static_cast<int>(point.X), static_cast<int>(point.Y));
		}		
		
		[[nodiscard]] ws::Vec2i toWorld(int x,int y) 
		{
			return toWorld(ws::Vec2i(x,y));
		}
		

	    [[nodiscard]] ws::Vec2i toScreen(ws::Vec2i worldPos) const
	    {
	        Gdiplus::PointF point(static_cast<Gdiplus::REAL>(worldPos.x), 
	                             static_cast<Gdiplus::REAL>(worldPos.y));
	        matrix.TransformPoints(&point, 1);
				        
			return ws::Vec2i(static_cast<int>(point.X), static_cast<int>(point.Y));
	    
		}

		
		[[nodiscard]] ws::Vec2i toScreen(int x,int y) 
		{
			return toScreen(ws::Vec2i(x,y));
		}		
		



		
		
		void apply(Gdiplus::Graphics &graphics)
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
		    
		    graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
		    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		    graphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
		    graphics.SetTransform(&matrix);
		}
			
		private:
			
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
	    	
	};
	
		


	class Animate
	{
		public:
		
		
		int width = 0;
		int height = 0;
		
		
		
		
		
		Animate()
		{
			
		}
		
		~Animate() {
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
//	    virtual void draw(Gdiplus::Graphics* graphics) override
//	    {
//	        if (!textureRef || !textureRef->isValid()) 
//	            return;
//			        
//			        
//			        
//	        int left, top, right, bottom;
//	        getBounds(left, top, right, bottom);
//	        
//	        Gdiplus::Rect destRect(left, top, right - left, bottom - top);
//	        
//	        
//	        
//	        if (scale.x >= 0 && scale.y >= 0)
//	        {
//	            // No flipping
//	            Gdiplus::Rect srcRect(texLeft, texTop, texWidth, texHeight);
//	            graphics->DrawImage(textureRef->bitmap, destRect, 
//	                               srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height,
//	                               Gdiplus::UnitPixel);
//	        }
//	        else
//	        {
//	            // Handle flipping with image attributes
//	            Gdiplus::ImageAttributes attributes;
//	            
//	            if (scale.x < 0) {
//	                attributes.SetWrapMode(Gdiplus::WrapModeTileFlipX);
//	            }
//	            if (scale.y < 0) {
//	                attributes.SetWrapMode(Gdiplus::WrapModeTileFlipY);
//	            }
//	            
//	            Gdiplus::Rect srcRect(texLeft, texTop, texWidth, texHeight);
//	            graphics->DrawImage(textureRef->bitmap, destRect,
//	                               srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height,
//	                               Gdiplus::UnitPixel, &attributes);
//	        }
//		    
//	        
//	    }
//		
//		
		
		
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
		
		public:
			
			
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
	    }
	
	    void addVertex(int x, int y) 
		{
	        vertices.push_back({x, y});
	    }
	
		
	
	    void clear() 
		{
	        vertices.clear();
	    }
	
	    size_t vertexCount() 
		{
	        return vertices.size();
	    }
	
	
	    // Check if enough points to make valid shape. Minimum = 3
	    bool isValid() 
		{
	        return vertices.size() >= 3;
	    }
	
	    // Calculate centroid of the polygon
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
	
	
	
	    // Check if a point is inside the polygon using ray casting algorithm
	    virtual bool contains(ws::Vec2i point) override 
		{
	        if (vertices.size() < 3) return false;
	        
	        int crossings = 0;
	        size_t n = vertices.size();
	        
	        for (size_t a = 0; a < n; a++) 
			{
	            ws::Vec2i p1 = vertices[a];
	            ws::Vec2i p2 = vertices[(a + 1) % n];
	            
	            // Check if point is on vertex
	            if (point.x == p1.x && point.y == p1.y) return true;
	            
	            // Check if point is on horizontal edge
	            if (p1.y == p2.y && point.y == p1.y && 
	                point.x >= std::min(p1.x, p2.x) && point.x <= std::max(p1.x, p2.x)) {
	                return true;
	            }
	            
	            // Check for crossing
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
	        
	        // Check if any edge intersects with the line
	        for (size_t i = 0; i < vertices.size(); i++) {
	            ws::Vec2i p1 = vertices[i];
	            ws::Vec2i p2 = vertices[(i + 1) % vertices.size()];
	            
	            Line edge(p1, p2);
	            if (edge.intersects(line)) {
	                return true;
	            }
	        }
	        
	        // Check if line is completely inside polygon
	        if (contains(line.start) || contains(line.end)) {
	            return true;
	        }
	        
	        return false;
	    }
	
	    
	
	
	
		bool intersects(Poly &other) 
		{
	        // Check if any vertex of other is inside this polygon
	        for (const auto& vertex : other.vertices) 
			{
	            if (contains(vertex)) 
				{
	                return true;
	            }
	        }
	        
	        // Check if any vertex of this polygon is inside the other
	        for (const auto& vertex : vertices) {
	            if (other.contains(vertex)) {
	                return true;
	            }
	        }
	        
	        // Check if any edges intersect
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
	        
	        RECT rect1 = {vertices[0].x, vertices[0].y, vertices[0].x, vertices[0].y};
	        ws::IntRect rect = rect1;
	        rect.width = rect.width - rect.left;
	        rect.height = rect.height - rect.top;
	        
	        
	        
	        for (const auto& vertex : vertices) {
	            rect.left = std::min(rect.left, vertex.x);
	            rect.top = std::min(rect.top, vertex.y);
	            rect.width = std::max(rect.width, vertex.x);
	            rect.height = std::max(rect.height, vertex.y);
	        }
	        
	        return rect;
	    }
	
	
	
	    
	
	
		virtual void draw(Gdiplus::Graphics* canvas) override 
		{
	
	        if (vertices.size() < 2) return;
	        
	        std::vector<Gdiplus::PointF> transformedPoints;
	        
		    // Convert to Gdiplus::REAL
		    for (const auto& vertex : vertices) {
		        transformedPoints.push_back(Gdiplus::PointF(
		            static_cast<Gdiplus::REAL>(vertex.x),
		            static_cast<Gdiplus::REAL>(vertex.y)
		        ));
		    }
	        
	        
		    Gdiplus::Pen borderPen(borderColor, static_cast<Gdiplus::REAL>(borderWidth));
		    Gdiplus::SolidBrush fillBrush(fillColor);
	        
	        
	  
		    // Draw filled polygon
		    if (filled && closed && vertices.size() >= 3) {
		        canvas->FillPolygon(&fillBrush, transformedPoints.data(), 
		                          static_cast<INT>(transformedPoints.size()));
		    }
		    
		    // Draw border/outline
		    if (closed && vertices.size() >= 3) {
		        canvas->DrawPolygon(&borderPen, transformedPoints.data(), 
		                          static_cast<INT>(transformedPoints.size()));
		    } 
		    else if (vertices.size() >= 2) {
		        // Draw as polyline if not closed
		        canvas->DrawLines(&borderPen, transformedPoints.data(), 
		                        static_cast<INT>(transformedPoints.size()));
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
            
//            RECT clientRect;
//            GetClientRect(hwnd, &clientRect);
//            
//            int clientWidth = clientRect.right - clientRect.left;
//            int clientHeight = clientRect.bottom - clientRect.top;

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
	
		ws::Vec2i getMousePos(Window &window)
		{
			
		    POINT p;
		    if(!GetCursorPos(&p))
		    {
		        return {0,0};
		    }
		    
		    ScreenToClient(window.hwnd, &p); // Convert to client coordinates
		    ws::Vec2i p2 = p;
			p2 = window.view.toWorld(p2);
		    return p2;
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