#ifndef WINAPI_SIMPLE_GRAPHICS
#define WINAPI_SIMPLE_GRAPHICS

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
//#ifndef WIN32_LEAN_AND_MEAN
//#define WIN32_LEAN_AND_MEAN
//#endif

 
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

#ifndef SHORT
typedef short SHORT;
#endif

#ifndef PROPID
typedef unsigned long PROPID;
#endif

#include <gdiplus.h>

namespace ws
{
	// Resolves a relative path to an absolute path based on the executable directory.
	bool ResolveRelativePath(std::string path)
	{
	    std::filesystem::path filePath(path);
	    if (filePath.is_relative()) {
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

	
	// Converts a UTF-8 std::string to std::wstring.
	std::wstring WIDE(std::string str)
	{
	    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	    std::wstring wstr(size, 0);
	    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
	    return wstr;
	}
	
	// Converts a wide string to a UTF-8 std::string.
	std::string SHORT(const std::wstring& wstr) {
	    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	    if (bufferSize == 0) {
	        return "";
	    }
	
	    std::string str(bufferSize - 1, '\0');
	    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, str.data(), bufferSize, NULL, NULL);
	
	    return str;
	}
	
	// Converts a narrow string to a wide string and returns its c_str().
	LPCWSTR TO_LPCWSTR(std::string str)
	{
		return WIDE(str).c_str();
	}
	
	// Returns the short path name of a given long path; returns empty on failure.
	std::wstring GetShortPathNameSafe(const std::wstring& longPath) 
	{
	    DWORD bufferSize = GetShortPathNameW(longPath.c_str(), NULL, 0);
	    if (bufferSize == 0) {
	        return L"";
	    }
	
	    std::wstring shortPath(bufferSize, L'\0');
	    bufferSize = GetShortPathNameW(longPath.c_str(), shortPath.data(), bufferSize);
	    if (bufferSize == 0) {
	        return L"";
	    }
	    
	    shortPath.resize(bufferSize);
	    return shortPath;
	}
		
}

#include <type_traits>
#include <utility>

namespace ws
{
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
	
	struct Vec2i {
	    int x, y;
	    
	    Vec2i() = default;
	    
	    // Constructs from arithmetic coordinate values.
	    template<typename T, typename U,
	             typename = std::enable_if_t<std::is_arithmetic_v<T> && 
	                                         std::is_arithmetic_v<U>>>
	    Vec2i(T x_val, U y_val) 
	        : x(static_cast<int>(x_val)), 
	          y(static_cast<int>(y_val)) {}
	    
	    // Constructs from any object that has x,y members.
	    template<typename T,
	             typename = std::enable_if_t<has_xy_members<T>::value>>
	    Vec2i(const T& other) 
	        : x(static_cast<int>(other.x)), 
	          y(static_cast<int>(other.y)) {}
	    
	    // Converts to any type that has x,y members.
	    template<typename T,
	             typename = std::enable_if_t<has_xy_members<T>::value>>
	    operator T() const {
	        T result;
	        result.x = static_cast<decltype(T::x)>(x);
	        result.y = static_cast<decltype(T::y)>(y);
	        return result;
	    }
	    
	    // Cast to POINT*.
	    operator POINT*() { 
	        return reinterpret_cast<POINT*>(this); 
	    }
	    
	    operator const POINT*() const { 
	        return reinterpret_cast<const POINT*>(this); 
	    }
	};
	
	struct Vec2f {
	    float x, y;
	    
	    Vec2f() = default;
	    
	    // Constructs from arithmetic coordinate values.
	    template<typename T, typename U,
	             typename = std::enable_if_t<std::is_arithmetic_v<T> && 
	                                         std::is_arithmetic_v<U>>>
	    Vec2f(T x_val, U y_val) 
	        : x(static_cast<float>(x_val)), 
	          y(static_cast<float>(y_val)) {}
	    
	    // Constructs from any object that has x,y members.
	    template<typename T,
	             typename = std::enable_if_t<has_xy_members<T>::value>>
	    Vec2f(const T& other) 
	        : x(static_cast<float>(other.x)), 
	          y(static_cast<float>(other.y)) {}
	    
	    // Converts to any type that has x,y members.
	    template<typename T,
	             typename = std::enable_if_t<has_xy_members<T>::value>>
	    operator T() const {
	        T result;
	        result.x = static_cast<decltype(T::x)>(x);
	        result.y = static_cast<decltype(T::y)>(y);
	        return result;
	    }
	    
	    operator POINT*() { 
	        return reinterpret_cast<POINT*>(this); 
	    }
	    
	    operator const POINT*() const { 
	        return reinterpret_cast<const POINT*>(this); 
	    }
	};
	
	struct Vec2d {
	    double x, y;
	    
	    Vec2d() = default;
	    
	    // Constructs from arithmetic coordinate values.
	    template<typename T, typename U,
	             typename = std::enable_if_t<std::is_arithmetic_v<T> && 
	                                         std::is_arithmetic_v<U>>>
	    Vec2d(T x_val, U y_val) 
	        : x(static_cast<double>(x_val)), 
	          y(static_cast<double>(y_val)) {}
	    
	    // Constructs from any object that has x,y members.
	    template<typename T,
	             typename = std::enable_if_t<has_xy_members<T>::value>>
	    Vec2d(const T& other) 
	        : x(static_cast<double>(other.x)), 
	          y(static_cast<double>(other.y)) {}
	    
	    // Converts to any type that has x,y members.
	    template<typename T,
	             typename = std::enable_if_t<has_xy_members<T>::value>>
	    operator T() const {
	        T result;
	        result.x = static_cast<decltype(T::x)>(x);
	        result.y = static_cast<decltype(T::y)>(y);
	        return result;
	    }
	    
	    operator POINT*() { 
	        return reinterpret_cast<POINT*>(this); 
	    }
	    
	    operator const POINT*() const { 
	        return reinterpret_cast<const POINT*>(this); 
	    }
	};
	
	struct Vec3i {
	    int x, y, z;
	    
	    Vec3i() = default;
	    
	    // Constructs from arithmetic x,y,z values.
	    template<typename T, typename U, typename V,
	             typename = std::enable_if_t<std::is_arithmetic_v<T> && 
	                                         std::is_arithmetic_v<U> &&
	                                         std::is_arithmetic_v<V>>>
	    Vec3i(T x_val, U y_val, V z_val) 
	        : x(static_cast<int>(x_val)), 
	          y(static_cast<int>(y_val)), 
	          z(static_cast<int>(z_val)) {}
	    
	    // Constructs from any object that has x,y,z members.
	    template<typename T,
	             typename = std::enable_if_t<has_xyz_members<T>::value>>
	    Vec3i(const T& other) 
	        : x(static_cast<int>(other.x)), 
	          y(static_cast<int>(other.y)), 
	          z(static_cast<int>(other.z)) {}
	    
	    // Converts to any type that has x,y,z members.
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
	
	struct Vec3f {
	    float x, y, z;
	    
	    Vec3f() = default;
	    
	    // Constructs from arithmetic x,y,z values.
	    template<typename T, typename U, typename V,
	             typename = std::enable_if_t<std::is_arithmetic_v<T> && 
	                                         std::is_arithmetic_v<U> &&
	                                         std::is_arithmetic_v<V>>>
	    Vec3f(T x_val, U y_val, V z_val) 
	        : x(static_cast<float>(x_val)), 
	          y(static_cast<float>(y_val)), 
	          z(static_cast<float>(z_val)) {}
	    
	    // Constructs from any object that has x,y,z members.
	    template<typename T,
	             typename = std::enable_if_t<has_xyz_members<T>::value>>
	    Vec3f(const T& other) 
	        : x(static_cast<float>(other.x)), 
	          y(static_cast<float>(other.y)), 
	          z(static_cast<float>(other.z)) {}
	    
	    // Converts to any type that has x,y,z members.
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
	
	struct Vec3d {
	    double x, y, z;
	    
	    Vec3d() = default;
	    
	    // Constructs from arithmetic x,y,z values.
	    template<typename T, typename U, typename V,
	             typename = std::enable_if_t<std::is_arithmetic_v<T> && 
	                                         std::is_arithmetic_v<U> &&
	                                         std::is_arithmetic_v<V>>>
	    Vec3d(T x_val, U y_val, V z_val) 
	        : x(static_cast<double>(x_val)), 
	          y(static_cast<double>(y_val)), 
	          z(static_cast<double>(z_val)) {}
	    
	    // Constructs from any object that has x,y,z members.
	    template<typename T,
	             typename = std::enable_if_t<has_xyz_members<T>::value>>
	    Vec3d(const T& other) 
	        : x(static_cast<double>(other.x)), 
	          y(static_cast<double>(other.y)), 
	          z(static_cast<double>(other.z)) {}
	    
	    // Converts to any type that has x,y,z members.
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
	
	struct IntRect {
	    int left, top, width, height;
	    
	    IntRect() = default;
	    
	    // Constructs from left,top,width,height.
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
	    
	    // Constructs from any rectangle-like object.
	    template<typename T,
	             typename = std::enable_if_t<is_rect_like<T>::value>>
	    IntRect(const T& other) {
	        if constexpr (has_width_height_style<T>::value) {
	            left = static_cast<int>(other.left);
	            top = static_cast<int>(other.top);
	            width = static_cast<int>(other.width);
	            height = static_cast<int>(other.height);
	        } else {
	            left = static_cast<int>(other.left);
	            top = static_cast<int>(other.top);
	            width = static_cast<int>(other.right - other.left);
	            height = static_cast<int>(other.bottom - other.top);
	        }
	    }
	    
	    // Converts to any rectangle-like type.
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
	
	struct FloatRect {
	    float left, top, width, height;
	    
	    FloatRect() = default;
	    
	    // Constructs from left,top,width,height.
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
	    
	    // Constructs from any rectangle-like object.
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
	    
	    // Converts to any rectangle-like type.
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
	
	struct DoubleRect {
	    double left, top, width, height;
	    
	    DoubleRect() = default;
	    
	    // Constructs from left,top,width,height.
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
	    
	    // Constructs from any rectangle-like object.
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
	    
	    // Converts to any rectangle-like type.
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
			
		// Cleans up MCI channel if ID is set.
		~Wav()
		{
		    if(!ID.empty())
		    {
		        std::string status = getChannelStatus(channel);
		        if (status != "error") {
		            std::string command = "close " + ID;
		            mciSendStringA(command.c_str(), NULL, 0, NULL);
		        }
			}
		}

		// Initializes Wav object; does not open the file.
		Wav(std::string path = "",int channel = 0,bool blocking = true)
		{
			this->path = path;
			this->channel = channel;
			this->blocking = blocking;
		}

		private:
		
		// Sends an MCI command string; optionally prints error.
		static bool mciSimple(std::string command,bool sendError = true)
		{
			MCIERROR err = mciSendStringA(command.c_str(),NULL,0,NULL);
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
		
		// Checks if file extension is supported by MCI; warns for uncommon types.
		static void isSupported(std::string m_path)
		{
			std::filesystem::path p(m_path);
			std::string ext = p.extension().string();
			
			if(ext == ".wav" || ext == ".mid" || ext == ".midi")
			{
				return;
			}
			if(ext == ".mp3" || ext == ".wma")
			{
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

		// Returns the first available channel (0-99) that is not playing/paused.
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

		// Returns the status string (playing, stopped, etc.) of an MCI channel.
		static std::string getChannelStatus(int m_channel)
		{
			std::string m_ID = std::to_string(m_channel);
		    char returnBuffer[128];
		    std::string command = "status " + m_ID + " mode";
		     memset(returnBuffer, 0, sizeof(returnBuffer));
		    if (mciSendStringA(command.c_str(), returnBuffer, sizeof(returnBuffer), NULL) == 0) {
		        return returnBuffer;
		    } else {
		        return "error";
		    }
		    return "error";
		}
		
		// Opens and plays a sound file on a given channel, returning immediately if non-blocking.
		static bool PlayFree(std::string m_path,int m_channel,bool m_blocking = false)
		{
			isSupported(m_path);
			std::string m_ID = std::to_string(m_channel);
			
			if (!std::filesystem::exists(m_path)) {
	            std::cerr << "Sound file not found: " << m_path << "\n";
	            return false;
	        }

			std::wstring wpath = GetShortPathNameSafe(WIDE(m_path));
			if(!wpath.empty())
				m_path = SHORT(wpath);
			
			std::string command;
			command = "close " + m_ID;
			mciSimple(command,false);
			
			command = "open "+ m_path + " alias "+m_ID;
			if(!mciSimple(command))
				return false;
			
			command = "play "+m_ID;
			
			if(!mciSimple(command))
				return false;
			
			return true;
		}
		
		// Opens a sound file for the Wav object; sets time format to milliseconds.
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
			
	        std::string command = "close " + ID;
	        mciSimple(command,false);
			
			std::wstring wpath = GetShortPathNameSafe(WIDE(path));
			if(!wpath.empty())
				path = SHORT(wpath);
			
			command = "open "+ path + " alias "+ID;
			if(!mciSimple(command))
				return false;

	        command = "set " + ID + " time format milliseconds";
	        if(!mciSimple(command))
				return false;			
			
			return true;
		}
		
		// Starts playback of the loaded sound; non-blocking if blocking==false.
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
	        command = "play " + ID;
	        
	        if(!mciSimple(command))
				return;						
		}
		
		// Pauses playback on the specified MCI channel.
		static void stop(int m_channel)
		{
			std::string m_ID = std::to_string(m_channel);
			std::string command = "pause "+ m_ID;
			mciSimple(command);
		}
		
		// Pauses playback of this Wav's channel.
		void stop()
		{
			std::string command = "pause "+ ID;
			mciSimple(command);
		}
		
	    // Sets volume percentage (0-100) for this sound channel.
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
				    	
	        int volume = (percent * 1000) / 100;
	        
	        if(getChannelStatus(channel) == "error") return false;
	        
	        volume = std::max(0, std::min(1000, volume));
	        
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
	    
	    // Returns current volume percentage (0-100); 0 if error.
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
	        return (volume * 100) / 1000;
	    }
		
		// Seeks to a given position in seconds.
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
		
		// Returns current playback position in seconds.
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
			
			MCIERROR err = mciSendStringA(command.c_str(), returnBuffer, sizeof(returnBuffer), NULL);
	        if (err) 
			{
				char errorBuf[128];
		        mciGetErrorStringA(err, errorBuf, sizeof(errorBuf));
		        std::cerr << "Sound Error of type MCI error: " << errorBuf << " - Command: " << command << "\n";
				return 0;
	        }			
	        
		    char *end_ptr;
		    long result = std::strtol(returnBuffer, &end_ptr, 10);
		    
		    if (returnBuffer == end_ptr) {
		        return 0;
		    }
	        	
	        return result / 1000;
		}	
		
		// Returns total length of the sound in seconds (approximate).
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
			return (atol(returnBuffer) - 10)/1000;		    
		}
		
		// Returns true if playback has reached the end.
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

namespace ws
{
	class View
	{
		public:
		// Default constructor.
		View()
		{
			
		}
		
		// Sets the world rectangle.
		void setRect(ws::IntRect viewRect)
		{
			world = viewRect;
		}
		
		// Sets world dimensions.
		void setSize(ws::Vec2i size)
		{
			world.width = size.x;
			world.height = size.y;
		}
		
		// Sets world position.
		void setPos(ws::Vec2i pos)
		{
			world.left = pos.x;
			world.top = pos.y;
		}
		
		// Sets the viewport rectangle.
		void setPortRect(ws::IntRect portRect)
		{
			port = portRect;
		}
		
		// Sets viewport size.
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
		
		// Sets viewport position.
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
		
		// Returns the world rectangle.
		ws::IntRect getRect()
		{
			return world;
		}
		
		// Returns world size.
		ws::Vec2i getSize()
		{
			ws::Vec2i p;
			p.x = world.width;
			p.y = world.height;
			
			return p;
		}
		
		// Returns world position.
		ws::Vec2i getPos()
		{
			ws::Vec2i p;
			p.x = world.left;
			p.y = world.top;
			
			return p;
		}
		
		// Returns the viewport rectangle.
		ws::IntRect getPortRect()
		{
			return port;
		}
		
		// Returns viewport size.
		ws::Vec2i getPortSize()
		{
			ws::Vec2i p;
			p.x = port.width;
			p.y = port.height;
			
			return p;
		}
		
		// Returns viewport position.
		ws::Vec2i getPortPos()
		{
			ws::Vec2i p;
			p.x = port.left;
			p.y = port.top;
			
			return p;
		}
		
	    // Zooms the view by adjusting viewport size and moving world.
	    void zoom(float factor)
	    {
	    	if(factor != 0)
	    	{
		    	int x = world.width / factor;
		    	int y = world.height / factor;
				setPortSize({x,y});
	    		world.left += x;
	    		world.top += y;
			}
		}

		// Moves the world by delta.
		void move(ws::Vec2i delta)
		{
			world.left += delta.x;
			world.top += delta.y;
		}
		
		// Moves the viewport by delta.
		void movePort(ws::Vec2i delta)
		{
			port.left += delta.x;
			port.top += delta.y;
		}
		
	    // Converts viewport coordinates to world coordinates.
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
	    
	    // Converts world coordinates to viewport coordinates.
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
		
		// Constructs by loading image from path.
		Texture(std::string path)
		{
			load(path);
		}
		
	    // Frees the Gdiplus bitmap.
	    ~Texture()
	    {
	        if (bitmap != nullptr)
	        {
	            delete bitmap;
	            bitmap = nullptr;
	        }
	    }		

	    // Move constructor.
	    Texture(Texture&& other) noexcept
	        : bitmap(other.bitmap), width(other.width), height(other.height)
	    {
	        other.bitmap = NULL;
	        other.width = 0;
	        other.height = 0;
	    }
		
	    // Move assignment operator.
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
		
		// Creates a blank bitmap of given size and optional background color.
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
	
		// Loads bitmap from file; releases previous image.
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
		
	    // Checks if a valid bitmap is loaded.
	    bool isValid() const
	    {
	        return bitmap != nullptr;
	    }		
	    
	    // Sets a pixel color at (xIndex,yIndex).
	    void setPixel(int xIndex,int yIndex,Gdiplus::Color &color)
	    {
			bitmap->SetPixel(xIndex, yIndex, color);
		}
	    
	    // Returns the color of a pixel at (xIndex,yIndex).
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
		
		// Sets the scale factor.
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
		
		// Sets the origin offset.
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
		
	    // Returns scaled width.
	    int getScaledWidth() const {
	        return static_cast<int>(width * fabs(scale.x));
	    }
	    
	    // Returns scaled height.
	    int getScaledHeight() const {
	        return static_cast<int>(height * fabs(scale.y));
	    }
	
	    // Computes world-space bounding rectangle of the drawable (after scaling and origin).
	    void getBounds(int& left, int& top, int& right, int& bottom) const {
	        int localLeft = -origin.x;
	        int localTop = -origin.y;
	        int localRight = width - origin.x;
	        int localBottom = height - origin.y;
	        
	        int scaledLeft = static_cast<int>(localLeft * scale.x);
	        int scaledRight = static_cast<int>(localRight * scale.x);
	        int scaledTop = static_cast<int>(localTop * scale.y);
	        int scaledBottom = static_cast<int>(localBottom * scale.y);
	        
	        if (scaledLeft > scaledRight) std::swap(scaledLeft, scaledRight);
	        if (scaledTop > scaledBottom) std::swap(scaledTop, scaledBottom);
	        
	        left = x + scaledLeft;
	        top = y + scaledTop;
	        right = x + scaledRight;
	        bottom = y + scaledBottom;
	    }

	    struct TransformResult {
	        int drawX, drawY;
	        int visualWidth, visualHeight;
	        int srcX, srcY;
	        int srcWidth, srcHeight;
	        bool visible;
	        
	        // Calculates draw position, visibility, and source rectangle based on view and flip.
	        static TransformResult calculate(const Drawable& drawable, View& view, 
	                                         int texLeft = 0, int texTop = 0,
	                                         int texWidth = 0, int texHeight = 0) {
	            TransformResult result;
	            
	            POINT viewPos = view.getPos();
	            
	            int left, top, right, bottom;
	            drawable.getBounds(left, top, right, bottom);
	            
	            result.visualWidth = right - left;
	            result.visualHeight = bottom - top;
	            
	            result.drawX = left - viewPos.x;
	            result.drawY = top - viewPos.y;
	            
	            result.visible = !(result.drawX + result.visualWidth < 0 || 
	                               result.drawY + result.visualHeight < 0 ||
	                               result.drawX >= view.getPortSize().x || 
	                               result.drawY >= view.getPortSize().y);
	            
	            if (!result.visible) {
	                return result;
	            }
	            
	            if (texWidth > 0 && texHeight > 0) {
	                if (drawable.scale.x >= 0) {
	                    result.srcX = texLeft;
	                    result.srcWidth = texWidth;
	                } else {
	                    result.srcX = texLeft + texWidth - 1;
	                    result.srcWidth = -texWidth;
	                }
	                
	                if (drawable.scale.y >= 0) {
	                    result.srcY = texTop;
	                    result.srcHeight = texHeight;
	                } else {
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
	};
	
	class Sprite : public Drawable
	{
		private:
	    ws::Texture* textureRef = nullptr;
	    int texLeft = 0, texTop = 0;
	    int texWidth = 0, texHeight = 0;			
		
		public:
		// Default constructor.
		Sprite()
		{
			
		}
		
	    // Checks if point lies inside the sprite bounds.
	    virtual bool contains(ws::Vec2i pos) override
	    {
	        int left, top, right, bottom;
	        getBounds(left, top, right, bottom);
	        
	        return (pos.x >= left && pos.x < right &&
	                pos.y >= top && pos.y < bottom);
	    }
	    
	    // Draws the sprite using GDI+ with support for flipping.
	    virtual void draw(Gdiplus::Graphics* graphics, View &view) override
	    {
	        if (!textureRef || !textureRef->isValid()) 
	            return;
	        
	        TransformResult tr = TransformResult::calculate(*this, view, texLeft, texTop, texWidth, texHeight);
	        
	        if (!tr.visible) {
	            return;
	        }
	        
	        Gdiplus::Rect destRect(tr.drawX, tr.drawY, tr.visualWidth, tr.visualHeight);
	        
			if (tr.srcWidth >= 0 && tr.srcHeight >= 0)
			{
				Gdiplus::Rect srcRect(tr.srcX, tr.srcY, abs(tr.srcWidth), abs(tr.srcHeight));
				graphics->DrawImage(textureRef->bitmap, destRect, 
								   srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height,
								   Gdiplus::UnitPixel);
			}
			else
			{
				Gdiplus::ImageAttributes attributes;
				
				if (tr.srcWidth < 0)
				{
					attributes.SetWrapMode(Gdiplus::WrapModeTileFlipX);
				}
				if (tr.srcHeight < 0)
				{
					attributes.SetWrapMode(Gdiplus::WrapModeTileFlipY);
				}
				
				Gdiplus::Rect srcRect(abs(tr.srcX), abs(tr.srcY), 
				abs(tr.srcWidth), abs(tr.srcHeight));
				graphics->DrawImage(textureRef->bitmap, destRect,
				srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height,
				Gdiplus::UnitPixel, &attributes);
			}	        
	    }
		
	    // Sets the texture and optionally resizes sprite to texture dimensions.
	    void setTexture(ws::Texture& texture,bool resize = true) {
	        textureRef = &texture;
	        if(resize)
	        	setTextureRect({0,0,texture.width,texture.height});
	    }
	    
	    // Sets the source rectangle within the texture.
	    void setTextureRect(ws::IntRect rect) {
	        texLeft = rect.left;
	        texTop = rect.top;
	        texWidth = rect.width;
	        texHeight = rect.height;
	        
	        width = texWidth;
	        height = texHeight;
	    }
	    
	    // Returns the source rectangle.
	    ws::IntRect getTextureRect() const {
	        return {texLeft, texTop, texWidth, texHeight};
	    }
		
		// Returns a reference to the attached texture.
		Texture &getTexture()
		{
			return *textureRef;
		}
		
	    // Returns const pointer to attached texture.
	    const Texture* getTexture() const
	    {
	        return textureRef;
	    }		
		
	    // Checks if a texture is currently assigned.
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
		
		// Initializes a 10x10 rectangle.
		Rectangle()
		{
			width = 10;
			height = 10;
			
		}

		// Checks if point lies inside the rectangle.
		virtual bool contains(ws::Vec2i pos)  override
		{
			int left, top, right, bottom;
	        getBounds(left, top, right, bottom);
	        
	        return (pos.x >= left && pos.x < right &&
	                pos.y >= top && pos.y < bottom);
		}

		
	    // Draws filled rectangle.
	    virtual void draw(Gdiplus::Graphics* canvas,View &view)  override
	    {
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
	    
	    // Constructs a line.
	    Line(ws::Vec2i start = {0,0},ws::Vec2i end = {0,0},int thewidth = 2,Gdiplus::Color color = {255,0,0,255})
	    {
	    	this->start = start;
	    	this->end = end;
	    	width = thewidth;
	    	this->color = color;
		}
	    
		// Draws the line with view offset.
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
	    // Hit test always returns false for a line.
	    virtual bool contains(ws::Vec2i pos) override
	    { 
	    	return false;
		}
		
		public:
		// Checks if point q lies on line segment pr.
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
	        
	        if (val == 0) return 0;
	        return (val > 0) ? 1 : 2;
	    }			
		
	    // Returns true if this line segment intersects another.
	    bool intersects(Line &otherLine)
	    {
	        ws::Vec2i p1 = this->start;
	        ws::Vec2i p2 = this->end;
	        ws::Vec2i p3 = otherLine.start;
	        ws::Vec2i p4 = otherLine.end;
	        
	        int o1 = orientation(p1, p2, p3);
	        int o2 = orientation(p1, p2, p4);
	        int o3 = orientation(p3, p4, p1);
	        int o4 = orientation(p3, p4, p2);
	        
	        if (o1 != o2 && o3 != o4)
	            return true;
	        
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
	
	    // Constructs a polygon from a vector of vertices.
	    Poly(std::vector<ws::Vec2i>& vertices, Gdiplus::Color fillColor = {255,255,0,0}, Gdiplus::Color borderColor = {255,255,0,255}, int borderWidth = 2, bool filled = true)
	    {
	        this->vertices = vertices;
	        this->fillColor = fillColor;
	        this->borderColor = borderColor;
	        this->borderWidth = borderWidth;
	        this->filled = filled;
	    }
	
	    // Adds a vertex.
	    void addVertex(ws::Vec2i vertex) 
		{
	        vertices.push_back(vertex);
	    }
	
	    // Adds a vertex from coordinates.
	    void addVertex(int x, int y) 
		{
	        vertices.push_back({x, y});
	    }
	
	    // Removes all vertices.
	    void clear() 
		{
	        vertices.clear();
	    }
	
	    // Returns number of vertices.
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
	
	    // Checks if a point is inside the polygon using ray casting.
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
	    
		// Checks if any edge of the polygon intersects a line, or line endpoints inside.
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
	
		// Returns the bounding rectangle of the polygon.
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
	
		// Draws the polygon (filled or outlined) with view offset.
		virtual void draw(Gdiplus::Graphics* canvas, ws::View &view) override 
		{
	        if (vertices.size() < 2) return;
	        
	        ws::Vec2i viewPos = view.getPos();
		    std::vector<Gdiplus::PointF> transformedPoints;
		    
		    for (const auto& vertex : vertices) {
		        transformedPoints.push_back(Gdiplus::PointF(
		            static_cast<Gdiplus::REAL>(vertex.x - viewPos.x),
		            static_cast<Gdiplus::REAL>(vertex.y - viewPos.y)
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
	};
	
	class Radial : public Drawable
	{
		public:
		Poly poly;
		
		// Constructs a radial circle-like polygon.
		Radial()
		{
			poly.fillColor = Gdiplus::Color(255,100,200,100);
			poly.borderColor = Gdiplus::Color(255,50,255,50);
			poly.borderWidth = 2;
			poly.closed = true;
			poly.filled = true;
			make();
		}
		
		// Regenerates vertices for the radial shape with given point count.
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
		
		// Sets center position and regenerates vertices.
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
		
		// Moves center by delta.
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
		
		// Sets number of vertices and regenerates.
		void setPointCount(int count)
		{
			m_points = count;
			make(m_points);
		}
		
		// Sets radius and regenerates.
		void setRadius(int size)
		{
			radius = size;
			make(m_points);
		}
		
		// Sets fill color.
		void setFillColor(Gdiplus::Color color)
		{
			poly.fillColor = color; 
			make(m_points);
		}
		
		// Sets border color.
		void setBorderColor(Gdiplus::Color color)
		{
			poly.borderColor = color;
			make(m_points);
		}
		
		// Sets border width.
		void setBorderWidth(int size)
		{
			poly.borderWidth = size;
			make(m_points);
		}
		
		// Returns radius.
		int getRadius()
		{
			return radius;
		}
		
		// Returns center position.
		ws::Vec2i getPosition()
		{
			return center;
		}
		
		// Returns number of vertices.
		int getPointCount()
		{
			return m_points;
		}
		
	    // Draws the radial shape (delegates to poly).
	    virtual void draw(Gdiplus::Graphics* canvas, View &view) override
	    {
	    	poly.draw(canvas,view);
	    }
	    
	    // Checks if point is inside (delegates to poly).
	    virtual bool contains(ws::Vec2i pos) override
	    {
	        return poly.contains(pos);
	    }	
		
		private:
		ws::Vec2i center;
		int m_points = 500;
		int radius = 10;
		
		// Updates Drawable position and size from poly's bounding rect.
		void updateDrawableProperties()
	    {
	        ws::IntRect bounds = poly.getBoundingRect();
	        x = bounds.left;
	        y = bounds.top;
	        width = bounds.width;
	        height = bounds.height;
	    }
	};
	
}

namespace ws
{
	class Timer
	{
		public:
		// High-precision timer.
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
	
	class Clipboard
	{
	private:
	    // Creates a GDI+ bitmap copy of a region from a Texture.
	    Gdiplus::Bitmap* copyRectOfBitmap(ws::Texture &texture, ws::IntRect rect)
	    {
	        if (!texture.isValid()) return nullptr;
	        
	        int srcWidth = texture.bitmap->GetWidth();
	        int srcHeight = texture.bitmap->GetHeight();
	        
	        int copyWidth, copyHeight, copyLeft, copyTop;
	        
	        if (rect.width == 0 && rect.height == 0 && rect.left == 0 && rect.top == 0) {
	            copyLeft = 0;
	            copyTop = 0;
	            copyWidth = srcWidth;
	            copyHeight = srcHeight;
	        } else {
	            copyLeft = rect.left;
	            copyTop = rect.top;
	            copyWidth = rect.width;
	            copyHeight = rect.height;
	            
	            if (copyLeft < 0) copyLeft = 0;
	            if (copyTop < 0) copyTop = 0;
	            if (copyLeft + copyWidth > srcWidth) copyWidth = srcWidth - copyLeft;
	            if (copyTop + copyHeight > srcHeight) copyHeight = srcHeight - copyTop;
	            
	            if (copyWidth <= 0 || copyHeight <= 0) return nullptr;
	        }
	        
	        Gdiplus::Bitmap* copyBitmap = new Gdiplus::Bitmap(copyWidth, copyHeight, 
	                                                         PixelFormat32bppARGB);
	        
	        Gdiplus::Graphics graphics(copyBitmap);
	        graphics.DrawImage(texture.bitmap, 
	                          0, 0, copyLeft, copyTop, copyWidth, copyHeight, 
	                          Gdiplus::UnitPixel);
	        
	        return copyBitmap;
	    }
	    
	    // Converts a GDI+ bitmap to HBITMAP.
	    HBITMAP gdipBitmapToHBITMAP(Gdiplus::Bitmap* gdipBitmap)
	    {
	        if (!gdipBitmap) return nullptr;
	        
	        HBITMAP hBitmap = nullptr;
	        Gdiplus::Color color(0, 0, 0, 0);
	        Gdiplus::Status status = gdipBitmap->GetHBITMAP(color, &hBitmap);
	        
	        if (status != Gdiplus::Ok) {
	            return nullptr;
	        }
	        
	        return hBitmap;
	    }
	    
	    // Converts a GDI+ bitmap to a DIB global memory handle.
	    HGLOBAL gdipBitmapToDIB(Gdiplus::Bitmap* gdipBitmap)
	    {
	        if (!gdipBitmap) return nullptr;
	        
	        UINT width = gdipBitmap->GetWidth();
	        UINT height = gdipBitmap->GetHeight();
	        
	        Gdiplus::BitmapData bitmapData;
	        Gdiplus::Rect rect(0, 0, width, height);
	        
	        if (gdipBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, 
	                                PixelFormat32bppARGB, &bitmapData) != Gdiplus::Ok) {
	            return nullptr;
	        }
	        
	        BITMAPINFOHEADER bi = {0};
	        bi.biSize = sizeof(BITMAPINFOHEADER);
	        bi.biWidth = width;
	        bi.biHeight = height;
	        bi.biPlanes = 1;
	        bi.biBitCount = 32;
	        bi.biCompression = BI_RGB;
	        bi.biSizeImage = width * height * 4;
	        
	        DWORD dwSize = sizeof(BITMAPINFOHEADER) + bi.biSizeImage;
	        
	        HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwSize);
	        if (!hGlobal) {
	            gdipBitmap->UnlockBits(&bitmapData);
	            return nullptr;
	        }
	        
	        LPBYTE lpData = (LPBYTE)GlobalLock(hGlobal);
	        if (!lpData) {
	            GlobalFree(hGlobal);
	            gdipBitmap->UnlockBits(&bitmapData);
	            return nullptr;
	        }
	        
	        memcpy(lpData, &bi, sizeof(BITMAPINFOHEADER));
	        
	        LPBYTE pDest = lpData + sizeof(BITMAPINFOHEADER);
	        LPBYTE pSrc = (LPBYTE)bitmapData.Scan0;
	        
	        for (UINT y = 0; y < height; y++) {
	            memcpy(pDest + (y * width * 4), 
	                   pSrc + ((height - 1 - y) * bitmapData.Stride), 
	                   width * 4);
	        }
	        
	        gdipBitmap->UnlockBits(&bitmapData);
	        GlobalUnlock(hGlobal);
	        
	        return hGlobal;
	    }
	    
	    // Converts an HBITMAP to a GDI+ bitmap.
	    Gdiplus::Bitmap* hbitmapToGdipBitmap(HBITMAP hBitmap)
	    {
	        if (!hBitmap) return nullptr;
	        
	        Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromHBITMAP(hBitmap, NULL);
	        
	        if (!bitmap || bitmap->GetLastStatus() != Gdiplus::Ok) {
	            if (bitmap) delete bitmap;
	            
	            BITMAP bm;
	            GetObject(hBitmap, sizeof(BITMAP), &bm);
	            
	            bitmap = new Gdiplus::Bitmap(bm.bmWidth, bm.bmHeight, PixelFormat32bppARGB);
	            
	            Gdiplus::Graphics graphics(bitmap);
	            Gdiplus::Bitmap tempBitmap(hBitmap, NULL);
	            graphics.DrawImage(&tempBitmap, 0, 0, bm.bmWidth, bm.bmHeight);
	        }
	        
	        return bitmap;
	    }

		// Opens clipboard; returns false on failure.
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
	    
	    // Copies a UTF-8 string to clipboard.
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
	    
	    // Pastes text from clipboard.
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
	    
	    // Copies a texture (or part of it) to clipboard.
	    bool copyTexture(ws::Texture &texture, ws::IntRect rect = {0,0,0,0}) 
	    {
	        if (!texture.isValid()) return false;
	        
	        Gdiplus::Bitmap* copyBitmap = copyRectOfBitmap(texture, rect);
	        if (!copyBitmap) return false;
	        
	        if (!OpenClipboardCheck()) {
	            delete copyBitmap;
				return false;
	        }
	        
	        EmptyClipboard();
	        
	        bool success = false;
	        
	        HGLOBAL hDib = gdipBitmapToDIB(copyBitmap);
	        if (hDib && SetClipboardData(CF_DIB, hDib)) {
	            success = true;
	        } else if (hDib) {
	            GlobalFree(hDib);
	        }
	        
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
	    
	    // Pastes a texture from clipboard.
	    ws::Texture pasteTexture(ws::IntRect rect = {0,0,0,0}) 
	    {
	        ws::Texture tex;
	        
	        if (!OpenClipboardCheck()) {
	            return tex;
	        }
	        
	        if (IsClipboardFormatAvailable(CF_DIB)) {
	            HANDLE hData = GetClipboardData(CF_DIB);
	            if (hData) {
	                LPVOID pData = GlobalLock(hData);
	                if (pData) {
	                    BITMAPINFOHEADER* bih = (BITMAPINFOHEADER*)pData;
	                    
	                    LPBYTE pPixels = (LPBYTE)pData + bih->biSize;
	                    
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
	    
	    // Checks if clipboard contains text.
	    bool hasText() 
	    {
	        if (!OpenClipboardCheck()) {
	            return false;
	        }
	        bool hasText = IsClipboardFormatAvailable(CF_UNICODETEXT);
	        CloseClipboard();
	        return hasText;
	    }
	    
	    // Checks if clipboard contains an image.
	    bool hasTexture() 
	    {
	        if (!OpenClipboardCheck()) {
	            return false;
	        }
	        bool hasImage = IsClipboardFormatAvailable(CF_BITMAP);
	        CloseClipboard();
	        return hasImage;
	    }
	    
	    // Clears the clipboard.
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
		
		// Initializes GDI+.
		GDIPLUS()
		{
			Gdiplus::GdiplusStartup(&gdiplustoken,&gdiplusstartup,nullptr);
		}
		
		// Shuts down GDI+.
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
		
		// Creates window, registers class, initialises GDI+ double buffering.
		Window(int width,int height,std::string title,DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = 0)
		{
			style |= WS_CLIPCHILDREN;
			exStyle |= WS_EX_COMPOSITED;
			
			icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
			icex.dwICC = ICC_STANDARD_CLASSES;
			InitCommonControlsEx(&icex);
			
			view.setRect({0,0,width,height});
			view.setPortRect({0,0,width,height});
			
			isRunning = true;
			
			HINSTANCE hInstance = GetModuleHandle(nullptr);
			
		    LPCWSTR CLASS_NAME = L"Window";
		    
		    WNDCLASSW wc = {};
		    wc.lpfnWndProc = ws::Window::StaticWindowProc;
		    wc.hInstance = hInstance;
		    wc.lpszClassName = CLASS_NAME;
		    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		
		    if (!RegisterClassW(&wc)) {
		        std::cerr << "Failed to register window class!" << std::endl;
		        exit(-1);
		    }		
			
			hwnd = CreateWindowExW(
			0,
			CLASS_NAME,
			TO_LPCWSTR(title),
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

        // Destroys window, frees back buffer and graphics.
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
		
		// Replaces the current view with an external View.
		void setView(ws::View &v)
		{
			view = v;
		}
		
		public:	
		
		// Places this window immediately after the specified window in Z-order.
		void setLayerAfter(HWND lastHwnd)
		{
			SetWindowPos(hwnd,lastHwnd,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
		}
	    	    
	    // Toggles fullscreen mode on or off.
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
	    
	    // Switches between fullscreen and windowed.
	    void toggleFullscreen() {
	        setFullscreen(!isFullscreen);
	    }
	    
	    // Returns true if window is fullscreen.
	    bool getFullscreen() const {
	        return isFullscreen;
	    }
				
		// Processes Windows messages; returns false if quit received.
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
		
	    // Pops the next queued message; returns false if queue empty.
	    bool pollEvent(MSG &message) {
	        if (msgQ.empty()) {
	            return false;
	        }
	        
	        message = msgQ.front();
	        msgQ.pop();
	        return true;
	    }	
		
	    // Recreates back buffer and clears it with given color.
	    void clear(Gdiplus::Color color = Gdiplus::Color(255,0,0,0)) 
		{
		    if (canvas) {
		        delete canvas;
		        canvas = nullptr;
		    }
		    if (backBuffer.bitmap) {
		        delete backBuffer.bitmap;
		        backBuffer.bitmap = nullptr;
		    }
		    backBuffer.create(view.getSize().x,view.getSize().y);
		    canvas = new Gdiplus::Graphics(backBuffer.bitmap);
			canvas->Clear(color);
	    }
		
		// Draws a Drawable onto the back buffer.
		void draw(Drawable &draw)
		{
			if(!canvas) return;
			draw.draw(canvas,view);
		}
		
	    // Invalidates window to trigger a paint.
	    void display() 
		{
			InvalidateRect(hwnd, NULL, FALSE);
        	UpdateWindow(hwnd);
	    }		
		
		private:
        static std::map<HWND, Window*> windowInstances;		

        // Static window procedure; forwards messages to the correct instance.
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
		
        // Instance window procedure; handles destroy, paint, resize, move.
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
		RECT windowedRect;
    	DWORD windowedStyle;
	};
	
    std::map<HWND, Window*> Window::windowInstances;
}

namespace ws
{
    namespace Global
    {
		// Returns mouse position relative to the given window, converted to world coordinates.
		ws::Vec2i getMousePos(Window &window)
		{
		    POINT p;
		    if(!GetCursorPos(&p))
		    {
		        return {0,0};
		    }
		    
		    ScreenToClient(window.hwnd, &p);
		    ws::Vec2i p2 = p;
			p2 = window.view.toWorld(p2);
		    return p2;
		}
		
		// Returns global mouse screen position.
		ws::Vec2i getMousePos()
		{
			POINT p;
			if(!GetCursorPos(&p))
			{
				return {0,0};
			}
			
			return p;
		}
		
		// Checks if a virtual mouse button is pressed.
		bool getMouseButton(int vmButton)
		{
			if (GetAsyncKeyState(vmButton) & 0x8000)
				return true;
			
			return false;	
		}
		
		// Checks if a virtual key is pressed.
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

namespace ws
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
		
		// Assigns a unique control ID.
		Child()
		{
			controlID = maxControlID+1;
			maxControlID++;
		}

        // Destroys the child window.
        virtual ~Child()
        {
            if (hwnd && IsWindow(hwnd))
            {
                DestroyWindow(hwnd);
            }
        }				
		
		// Sets position in world coordinates.
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
		
		// Returns world position.
		ws::Vec2i getPosition()
		{
			return {x,y};
		}

		// Returns screen-space position (pixels).
		ws::Vec2i getLiteralPosition()
		{
			return {litX,litY};
		}
		
		// Sets size in world units.
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
		
		// Returns world size.
		ws::Vec2i getSize()
		{
			return {width,height};
		}

		// Returns screen-space size.
		ws::Vec2i getLiteralSize()
		{
			return {litWidth,litHeight};
		}
				
		// Adds a window style.
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
		
		// Removes a window style.
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
		
        // Checks if a style is set.
        bool hasStyle(DWORD checkStyle)
        {
            if (hwnd)
            {
                DWORD currentStyle = GetWindowLong(hwnd, GWL_STYLE);
                return (currentStyle & checkStyle) != 0;
            }
            return (style & checkStyle) != 0;
        }
		
		// Sets the child's text.
		void setText(std::string newText)
		{
			update();
			text = newText;
			if (hwnd)
                SetWindowTextW(hwnd, WIDE(text).c_str());
		}
		
		// Returns the child's text.
		std::string getText()
		{
		    LRESULT textLength = SendMessageW(hwnd, WM_GETTEXTLENGTH, 0, 0);
		
		    if (textLength <= 0)
		        return "";
		    
		    std::vector<wchar_t> buffer(static_cast<size_t>(textLength) + 1);
		
		    SendMessageW(hwnd, WM_GETTEXT, static_cast<WPARAM>(buffer.size()), reinterpret_cast<LPARAM>(buffer.data()));
		
		    return SHORT(std::wstring(buffer.data()));		    
		}
		
		// Tests if a world point is inside the child.
		bool contains(ws::Vec2i point)
		{
			return (point.x >= x  && point.x < x + width && point.y >= 0 && point.y < y + height);
		}
		
		// Updates position and size to match current view scaling.
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
		}
		
		// Sets background color.
		void setFillColor(COLORREF color)
	    {
	        backgroundColor = color;
	        if (hwnd)
	            InvalidateRect(hwnd, NULL, TRUE);
	    }
	    
	    // Sets text color.
	    void setTextColor(COLORREF color)
	    {
	        textColor = color;
	        if (hwnd)
	            InvalidateRect(hwnd, NULL, TRUE);
	    }
		
		// Sets border color.
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
		
		// Initializes the button as a child of the given window.
		bool init(ws::Window &parent)
		{
			if(!parent.hwnd)
			{
				std::cerr << "Child Error: Selected parent is not valid!\n";
				return false;
			}
			
			parentRef = &parent;
			
			hwnd = CreateWindowExW(
			0,
			L"BUTTON",
			TO_LPCWSTR(getText()),
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
		
	    // Checks if the button was clicked.
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
		
		// Initializes the slider.
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
			
			hwnd = CreateWindowExW(
			0,
			TRACKBAR_CLASS,
			TO_LPCWSTR(getText()),
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
		
	    // Retrieves scroll position when slider moved.
	    bool getScroll(MSG &msg)
	    {
	        if((msg.message == WM_HSCROLL || msg.message == WM_VSCROLL) && (HWND)msg.lParam == hwnd)
	        {
	            slidePos = (int)SendMessage(hwnd, TBM_GETPOS, 0, 0);
				return true;
	        }
	        return false;
	    }
	    
	    // Makes slider horizontal.
	    void setHorizontal()
	    {
	    	removeStyle(TBS_VERT);
	    	addStyle(TBS_HORZ);
	    	update();
		}
		
		// Makes slider vertical.
		void setVertical()
		{
	    	removeStyle(TBS_HORZ);
	    	addStyle(TBS_VERT);	
			update();		
		}
		
		// Sets the slider range.
		void setRange(int minimum = 0,int maximum = 100)
		{
			SendMessage(hwnd, TBM_SETRANGEMIN, TRUE, minimum);
			SendMessage(hwnd, TBM_SETRANGEMAX, TRUE, maximum);
		}
		
		// Sets the slide position.
		void setSlidePosition(int pos = 0)
		{
			SendMessage(hwnd, TBM_SETPOS, TRUE, pos);
			slidePos = pos; 
		}
		
		// Returns current slide position.
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
		// Initializes the text box.
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
			
			hwnd = CreateWindowExW(
			WS_EX_CLIENTEDGE,
			L"EDIT",
			TO_LPCWSTR(getText()),
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
		
		// Returns true if the text box has focus.
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
		
		// Initializes the label.
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
			
			hwnd = CreateWindowExW(
			0,
			L"STATIC",
			TO_LPCWSTR(getText()),
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