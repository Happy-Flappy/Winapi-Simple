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






namespace ws // WINAPI CONVERTERS
{
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


namespace ws //DATA TYPES
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






namespace ws //SOUND AND VIDEO ENTITIES
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



namespace ws //GRAPHICS ENTITIES
{


	class View
	{
		public:
			
		View()
		{
			
		}
		
		void setRect(ws::IntRect viewRect)
		{
			world = viewRect;
		}
		
		void setSize(ws::Vec2i size)
		{
			world.width = size.x;
			world.height = size.y;
		}
		
		void setPos(ws::Vec2i pos)
		{
			world.left = pos.x;
			world.top = pos.y;
		}
		
		
		void setPortRect(ws::IntRect portRect)
		{
			port = portRect;
		}
		
		
		void setPortSize(ws::Vec2i size)
		{
			port.width = size.x;
			port.height = size.y;
		}
		
		
		void setPortSize(int x,int y)
		{
			port.width = x;
			port.height = y;
		}
		
		
		void setPortPos(ws::Vec2i pos)
		{
			port.left = pos.x;
			port.top = pos.y;
		}
		
		void setPortPos(int x,int y)
		{
			port.left = x;
			port.top = y;
		}
		
		
		
		
		ws::IntRect getRect()
		{
			return world;
		}
		
		ws::Vec2i getSize()
		{
			ws::Vec2i p;
			p.x = world.width;
			p.y = world.height;
			
			return p;
		}
		
		ws::Vec2i getPos()
		{
			ws::Vec2i p;
			p.x = world.left;
			p.y = world.top;
			
			return p;
		}
		
		ws::IntRect getPortRect()
		{
			return port;
		}
		
		ws::Vec2i getPortSize()
		{
			ws::Vec2i p;
			p.x = port.width;
			p.y = port.height;
			
			return p;
		}
		
		ws::Vec2i getPortPos()
		{
			ws::Vec2i p;
			p.x = port.left;
			p.y = port.top;
			
			return p;
		}
		
		
		
		
	    void zoom(float factor)
	    {
	    	if(factor != 0)
	    	{
			
		    	int x = world.width / factor;
		    	int y = world.height / factor;
				setPortSize({x,y});	// If factor is 2, that means that the visible world area is half as much because it is zooming in and will  be stretching into the viewport.
	    		world.left += x;
	    		world.top += y;
			}
		}


		void move(ws::Vec2i delta)
		{
			world.left += delta.x;
			world.top += delta.y;
		}
		
		void movePort(ws::Vec2i delta)
		{
			port.left += delta.x;
			port.top += delta.y;
		}
		
		
		
		
	    ws::Vec2i toWorld(ws::Vec2i pos) 
	    {
		    ws::Vec2i worldSize = getSize();
		    ws::Vec2i viewSize = getPortSize();
		    ws::Vec2i viewPos = getPos();
		    
		    ws::Vec2i worldPoint;
		    
		    float scaleX = static_cast<float>(worldSize.x) / viewSize.x;
		    float scaleY = static_cast<float>(worldSize.y) / viewSize.y;
		    
		    worldPoint.x = static_cast<int>(pos.x * scaleX) + viewPos.x;
		    worldPoint.y = static_cast<int>(pos.y * scaleY) + viewPos.y;
		    
		    return worldPoint;
	    }
	    
	    ws::Vec2i toWindow(ws::Vec2i pos) 
	    {
		    ws::Vec2i worldSize = getSize();
		    ws::Vec2i viewSize = getPortSize();
		    ws::Vec2i viewPos = getPos();
		    
		    ws::Vec2i windowPoint;
		    
		    float scaleX = static_cast<float>(viewSize.x) / worldSize.x;
		    float scaleY = static_cast<float>(viewSize.y) / worldSize.y;
		    
		    windowPoint.x = static_cast<int>((pos.x - viewPos.x) * scaleX);
		    windowPoint.y = static_cast<int>((pos.y - viewPos.y) * scaleY);
		    
		    return windowPoint;
	    }
				
		
		
		
		
		
		
		
		
		private:
		ws::IntRect world;
		ws::IntRect port;
		
			
	};



	
	
	class Texture
	{
		public:
		
		Gdiplus::Bitmap* bitmap;
		int width = 0;
		int height = 0;
		
		
		
		Texture() : bitmap(nullptr) {}
		
		
		Texture(std::string path)
		{
			load(path);
		}
		
		
	    ~Texture()
	    {
	        if (bitmap != nullptr)
	        {
	            delete bitmap;
	            bitmap = nullptr;
	        }
	    }		
		




	    Texture(Texture&& other) noexcept
	        : bitmap(other.bitmap), width(other.width), height(other.height)
	    {
	        other.bitmap = NULL;
	        other.width = 0;
	        other.height = 0;
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
	
	
	
	
		bool load(std::string path)
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
		
	    bool isValid() const
	    {
	        return bitmap != nullptr;
	    }		
	    
	    
	    
	    
	    
	    
	    
	    
	    
	    void setPixel(int xIndex,int yIndex,Gdiplus::Color &color)
	    {
			bitmap->SetPixel(xIndex, yIndex, color);
		}
	    
	    Gdiplus::Color getPixel(int xIndex,int yIndex)
	    {
	    	Gdiplus::Color color;
			bitmap->GetPixel(xIndex, yIndex, &color);
			return color;
		}
	    
	    	
	};
	
	
	
	
	
		
	
	
	class Drawable
	{
		public:
		
		int x = 0,y = 0,z = 0;
		int width = 1,height = 1;
		ws::Vec2f scale = {1,1};		
		ws::Vec2i origin = {0,0};
		
		void setScale(ws::Vec2f s)	
		{
			scale.x = s.x;
			scale.y = s.y;
		}
		
		
		void setScale(float sx,float sy)	
		{
			scale.x = sx;
			scale.y = sy;
		}
		
		
		
		void setOrigin(ws::Vec2i pos)
		{
			origin.x = pos.x;
			origin.y = pos.y;
		}
		
		void setOrigin(int posx,int posy)
		{
			origin.x = posx;
			origin.y = posy;
		}
		
		
		
	
	    // Get the visual width after scaling (always positive)
	    int getScaledWidth() const {
	        return static_cast<int>(width * fabs(scale.x));
	    }
	    
	    // Get the visual height after scaling (always positive)
	    int getScaledHeight() const {
	        return static_cast<int>(height * fabs(scale.y));
	    }
	
	    // Get the visual bounds of the object
	    void getBounds(int& left, int& top, int& right, int& bottom) const {
	        // Calculate the transformation of the local bounds
	        // Local coordinates relative to origin
	        int localLeft = -origin.x;
	        int localTop = -origin.y;
	        int localRight = width - origin.x;
	        int localBottom = height - origin.y;
	        
	        // Apply scale (can be negative for flipping)
	        int scaledLeft = static_cast<int>(localLeft * scale.x);
	        int scaledRight = static_cast<int>(localRight * scale.x);
	        int scaledTop = static_cast<int>(localTop * scale.y);
	        int scaledBottom = static_cast<int>(localBottom * scale.y);
	        
	        // Normalize (swap if negative scale flipped the bounds)
	        if (scaledLeft > scaledRight) std::swap(scaledLeft, scaledRight);
	        if (scaledTop > scaledBottom) std::swap(scaledTop, scaledBottom);
	        
	        // Translate to world coordinates
	        left = x + scaledLeft;
	        top = y + scaledTop;
	        right = x + scaledRight;
	        bottom = y + scaledBottom;
	    }

	    // Consolidated transformation function
	    struct TransformResult {
	        int drawX, drawY;          // Screen coordinates to draw at
	        int visualWidth, visualHeight; // Visual dimensions on screen
	        int srcX, srcY;            // Source texture coordinates (for Sprite)
	        int srcWidth, srcHeight;   // Source dimensions (for Sprite, may be negative)
	        bool visible;              // Whether the object is visible in view
	        
	        // Common transformation that applies to all Drawables
	        static TransformResult calculate(const Drawable& drawable, View& view, 
	                                         int texLeft = 0, int texTop = 0,
	                                         int texWidth = 0, int texHeight = 0) {
	            TransformResult result;
	            
	            // Get view position for culling
	            POINT viewPos = view.getPos();
	            
	            // Get the visual bounds
	            int left, top, right, bottom;
	            drawable.getBounds(left, top, right, bottom);
	            
	            result.visualWidth = right - left;
	            result.visualHeight = bottom - top;
	            
	            // Calculate draw position (top-left of visual bounds)
	            result.drawX = left - viewPos.x;
	            result.drawY = top - viewPos.y;
	            
	            // Culling check
	            result.visible = !(result.drawX + result.visualWidth < 0 || 
	                               result.drawY + result.visualHeight < 0 ||
	                               result.drawX >= view.getPortSize().x || 
	                               result.drawY >= view.getPortSize().y);
	            
	            if (!result.visible) {
	                return result;
	            }
	            
	            // Texture source calculations (only for sprites)
	            if (texWidth > 0 && texHeight > 0) {
	                // Determine source rectangle based on flipping
	                if (drawable.scale.x >= 0) {
	                    // No horizontal flip
	                    result.srcX = texLeft;
	                    result.srcWidth = texWidth;
	                } else {
	                    // Horizontal flip: start from right edge, use negative width
	                    result.srcX = texLeft + texWidth - 1;
	                    result.srcWidth = -texWidth;
	                }
	                
	                if (drawable.scale.y >= 0) {
	                    // No vertical flip
	                    result.srcY = texTop;
	                    result.srcHeight = texHeight;
	                } else {
	                    // Vertical flip: start from bottom edge, use negative height
	                    result.srcY = texTop + texHeight - 1;
	                    result.srcHeight = -texHeight;
	                }
	            }
	            
	            return result;
	        }
	    };
	
		
		virtual void draw(Gdiplus::Graphics* graphics,View &view) = 0;
		virtual bool contains(ws::Vec2i pos) = 0;
		
		
		virtual ~Drawable() = default;
		
		
		
		
		
		
		private:
				
				
		
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
	    
	    virtual void draw(Gdiplus::Graphics* graphics, View &view) override
	    {
	        if (!textureRef || !textureRef->isValid()) 
	            return;
	        
	        // Use the common transformation calculation
	        TransformResult tr = TransformResult::calculate(*this, view, texLeft, texTop, texWidth, texHeight);
	        
	        if (!tr.visible) {
	            return;
	        }
	        
	        
	        
	        Gdiplus::Rect destRect(tr.drawX, tr.drawY, tr.visualWidth, tr.visualHeight);
	        
			if (tr.srcWidth >= 0 && tr.srcHeight >= 0)
			{
				// No flipping
				Gdiplus::Rect srcRect(tr.srcX, tr.srcY, abs(tr.srcWidth), abs(tr.srcHeight));
				graphics->DrawImage(textureRef->bitmap, destRect, 
								   srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height,
								   Gdiplus::UnitPixel);
			}
			else
			{
				// Handle flipping with image attributes
				Gdiplus::ImageAttributes attributes;
				
				if (tr.srcWidth < 0)
				{
					// Horizontal flip
					attributes.SetWrapMode(Gdiplus::WrapModeTileFlipX);
				}
				if (tr.srcHeight < 0)
				{
					// Vertical flip
					attributes.SetWrapMode(Gdiplus::WrapModeTileFlipY);
				}
				
				Gdiplus::Rect srcRect(abs(tr.srcX), abs(tr.srcY), 
				abs(tr.srcWidth), abs(tr.srcHeight));
				graphics->DrawImage(textureRef->bitmap, destRect,
				srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height,
				Gdiplus::UnitPixel, &attributes);
			}	        
	        
	        
	    }
		
		
		
		
	    void setTexture(ws::Texture& texture,bool resize = true) {
	        textureRef = &texture;
	        if(resize)
	        	setTextureRect({0,0,texture.width,texture.height});
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
	
	
	
	
	
	class Rectangle : public Drawable
	{
		public:
		
		Gdiplus::Color color = {255,100,200,100};
		
		Rectangle()
		{
			width = 10;
			height = 10;
			
		}
		


		virtual bool contains(ws::Vec2i pos)  override
		{
			int left, top, right, bottom;
	        getBounds(left, top, right, bottom);
	        
	        return (pos.x >= left && pos.x < right &&
	                pos.y >= top && pos.y < bottom);
		}


		
	    virtual void draw(Gdiplus::Graphics* canvas,View &view)  override
	    {
	    	

	        // Use the common transformation calculation
	        TransformResult tr = TransformResult::calculate(*this, view, x, y, width, height);
	        
	        if (!tr.visible) {
	            return;
	        }			

	    	
			
			Gdiplus::SolidBrush brush(color);
			Gdiplus::Rect rect(tr.drawX, tr.drawY, tr.visualWidth, tr.visualHeight);
			canvas->FillRectangle(&brush,rect);
		    
	    }




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
	    
	    
		virtual void draw(Gdiplus::Graphics* canvas, ws::View &view) override 
		{
	        TransformResult tr = TransformResult::calculate(*this, view, start.x, start.y, end.x, end.y);
	        
	        if (!tr.visible) {
	            return;
	        }
	
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
	    Gdiplus::Color fillColor = {255,255,0,0};    
	    Gdiplus::Color borderColor = {255,255,0,100};    
	    int borderWidth = 2;
	    bool filled = true;
	    bool closed = true;
	
	
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
	
	
	
	    
	
	
		virtual void draw(Gdiplus::Graphics* canvas, ws::View &view) override 
		{
	
	        if (vertices.size() < 2) return;
	        
	        ws::Vec2i viewPos = view.getPos();
		    std::vector<Gdiplus::PointF> transformedPoints;
		    
		    // Apply view transformation to all points
		    for (const auto& vertex : vertices) {
		        transformedPoints.push_back(Gdiplus::PointF(
		            static_cast<Gdiplus::REAL>(vertex.x - viewPos.x),
		            static_cast<Gdiplus::REAL>(vertex.y - viewPos.y)
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
			poly.fillColor = Gdiplus::Color(255,100,200,100);
			poly.borderColor = Gdiplus::Color(255,50,255,50);
			poly.borderWidth = 2;
			poly.closed = true;
			poly.filled = true;
			make();
		}
		
		void make(int points = 500)
		{
			poly.clear();
			
			double inc = (2*3.14)/points; 
			
			for(double a=0;a<(2*3.14);a+=inc)
			{
				int resx = static_cast<int>(std::cos(a) * getRadius());
				int resy = static_cast<int>(std::sin(a) * getRadius());
				poly.addVertex(center.x + resx,center.y + resy);
			}
			m_points = points;
			
			updateDrawableProperties();
		}
		
		
		
		void setPosition(int posx,int posy)
		{
			center = {posx,posy};
			make(m_points);
		}
		
		void setPosition(ws::Vec2i pos)
		{
			center = pos;
			make(m_points);
		}
		
		
		void move(ws::Vec2i delta)
		{
			setPosition(center.x + delta.x,center.y + delta.y);
			make(m_points);
		}
		
		void move(int deltaX,int deltaY)
		{
			setPosition(center.x + deltaX,center.y + deltaY);
			make(m_points);
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
			poly.fillColor = color; 
			make(m_points);
		}
		
		void setBorderColor(Gdiplus::Color color)
		{
			poly.borderColor = color;
			make(m_points);
		}
		
		void setBorderWidth(int size)
		{
			poly.borderWidth = size;
			make(m_points);
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
		
		
	    virtual void draw(Gdiplus::Graphics* canvas, View &view) override
	    {
	    	poly.draw(canvas,view);
	    }
	    
	    virtual bool contains(ws::Vec2i pos) override
	    {
	        return poly.contains(pos);
	    }	
		
		
		private:
		ws::Vec2i center;
		int m_points = 500;
		int radius = 10;
		
		
		void updateDrawableProperties()
	    {
	        // Set Drawable's position and size based on the poly's bounding rect
	        ws::IntRect bounds = poly.getBoundingRect();
	        x = bounds.left;
	        y = bounds.top;
	        width = bounds.width;
	        height = bounds.height;
	    }
		
	};

	
}





namespace ws //SYSTEM ENTITIES
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
	                        tex.bitmap = bitmap;
	                        tex.width = bitmap->GetWidth();
	                        tex.height = bitmap->GetHeight();
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
	                    tex.bitmap = clipboardBitmap;
	                    tex.width = clipboardBitmap->GetWidth();
	                    tex.height = clipboardBitmap->GetHeight();
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
	                tex.bitmap = croppedBitmap;
	                tex.width = croppedBitmap->GetWidth();
	                tex.height = croppedBitmap->GetHeight();
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
	    int x,y,width, height;		
		bool isTransparent = false;

		private:
			
		bool isRunning = true;
		std::queue<MSG> msgQ;
		
		INITCOMMONCONTROLSEX icex;
		
				
		public:		
		
		
		View view;
		
		ws::Texture backBuffer;
	    Gdiplus::Graphics* canvas;

		

		
		
		
		Window(int width,int height,std::string title,DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0)
		{
			
			style |= WS_CLIPCHILDREN;
			
			exStyle |= WS_EX_COMPOSITED;
			
			//This is for initialization of winapi child objects sucg as buttons and textboxes.
			icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
			icex.dwICC = ICC_STANDARD_CLASSES;  // Enables a set of common controls.
			InitCommonControlsEx(&icex);
			///////////////////////////////
			
			
			
			view.setRect({0,0,width,height});//Sets world rect
			view.setPortRect({0,0,width,height});//Sets viewport rect
			
			
			
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
			0,
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

			
		}
		
		
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

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
		
		
		
		
		
		void setView(ws::View &v)
		{
			view = v;
		}
		
		
    	
		public:	
			

		
		
		
		
		
		
		
		
		void setLayerAfter(HWND lastHwnd)
		{
			SetWindowPos(hwnd,lastHwnd,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
		}
		    	    
	    


	    void setFullscreen(bool fullscreen) {
	        if (fullscreen == isFullscreen) return;
	        
	        if (fullscreen) {
	            windowedStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	            GetWindowRect(hwnd, &windowedRect);
	            
	            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	            int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	            
	            SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	            SetWindowPos(hwnd, 
				HWND_TOP, 
				0, 
				0, 
				screenWidth, 
				screenHeight,
				SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	            
	            isFullscreen = true;
	        } else {
	            SetWindowLong(hwnd, GWL_STYLE, windowedStyle);
	            SetWindowPos(hwnd, HWND_TOP, 
	            windowedRect.left, 
				windowedRect.top,
	            windowedRect.right - windowedRect.left,
	            windowedRect.bottom - windowedRect.top,
	            SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	            
	            isFullscreen = false;
	        }
	    }
	    
	    void toggleFullscreen() {
	        setFullscreen(!isFullscreen);
	    }
	    
	    bool getFullscreen() const {
	        return isFullscreen;
	    }


				
		
		
		
		bool isOpen()
		{
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
			if(!canvas) return;

			draw.draw(canvas,view);
		}
		
		
		
		
		
		
		
	    void display() 
		{
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
					    
						if(hdc)
						{
							HBITMAP hBitmap;
							backBuffer.bitmap->GetHBITMAP(Gdiplus::Color(0, 0, 0), &hBitmap);
							HDC hdcMem = CreateCompatibleDC(hdc);
							HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);
							
							StretchBlt(hdc,0,0,width,height,hdcMem,0,0,view.getSize().x,view.getSize().y,SRCCOPY);
							SelectObject(hdcMem, hbmOld);
							DeleteDC(hdcMem);
							DeleteObject(hBitmap); 
						}
						
//						if (hdc) {
//				            Gdiplus::Graphics graphics(hdc);
//				            graphics.DrawImage(backBuffer.bitmap, 0, 0, width, height);
//				        }
				    }

	              
	                
	                EndPaint(hwnd, &ps);
	                return 0;

	            }
	                
	            case WM_SIZE: {
				
				   
				    
					width = LOWORD(lParam);
	                height = HIWORD(lParam);

	
	        		view.setPortSize({width, height});

	                
	                return 0;
	            	
				}
				
				
				case WM_MOVE:
				{
					x = GET_X_LPARAM(lParam);
					y = GET_Y_LPARAM(lParam);
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
		bool legacyTransparency = false;
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



			int windowWidth = parentRef->width;
			int windowHeight = parentRef->height;

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