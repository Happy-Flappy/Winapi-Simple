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




namespace ws //DATA TYPES
{
	
	struct Vec2d
	{
		double x,y;	
	};
		
	struct Vec2f
	{
		float x,y;
	};
	
	struct Vec2i
	{
		int x,y;
	};
	
	struct Vec3d
	{
		double x,y,z;
	};
	struct Vec3f
	{
		float x,y,z;
	};
	struct Vec3i
	{
		int x,y,z;
	};
	
	
	struct IntRect
	{
		int left,top,width,height;	
	};
	
	struct FloatRect
	{
		float left,top,width,height;
	};
	
	struct DoubleRect
	{
		double left,top,width,height;
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
		
		void setRect(RECT viewRect)
		{
			world = viewRect;
		}
		
		void setSize(POINT size)
		{
			world.right = size.x;
			world.bottom = size.y;
		}
		
		void setPos(POINT pos)
		{
			world.left = pos.x;
			world.top = pos.y;
		}
		
		
		void setPortRect(RECT portRect)
		{
			port = portRect;
		}
		
		void setPortSize(POINT size)
		{
			port.right = size.x;
			port.bottom = size.y;
		}
		
		void setPortPos(POINT pos)
		{
			port.left = pos.x;
			port.top = pos.y;
		}
		
		
		
		
		RECT getRect()
		{
			return world;
		}
		
		POINT getSize()
		{
			POINT p;
			p.x = world.right;
			p.y = world.bottom;
			
			return p;
		}
		
		POINT getPos()
		{
			POINT p;
			p.x = world.left;
			p.y = world.top;
			
			return p;
		}
		
		RECT getPortRect()
		{
			return port;
		}
		
		POINT getPortSize()
		{
			POINT p;
			p.x = port.right;
			p.y = port.bottom;
			
			return p;
		}
		
		POINT getPortPos()
		{
			POINT p;
			p.x = port.left;
			p.y = port.top;
			
			return p;
		}
		
		
		
		
	    void zoom(float factor)
	    {
	    	if(factor != 0)
	    	{
			
		    	long int x = world.right / factor;
		    	long int y = world.bottom / factor;
				setPortSize({x,y});	// If factor is 2, that means that the visible world area is half as much because it is zooming in and will  be stretching into the viewport.
	    		world.left += x;
	    		world.top += y;
			}
		}


		void move(POINT delta)
		{
			world.left += delta.x;
			world.top += delta.y;
		}
		
		void movePort(POINT delta)
		{
			port.left += delta.x;
			port.top += delta.y;
		}
		
		
		
		
	    POINT toWorld(POINT pos) 
	    {
		    POINT worldSize = getSize();
		    POINT viewSize = getPortSize();
		    POINT viewPos = getPos();
		    
		    POINT worldPoint;
		    
		    float scaleX = static_cast<float>(worldSize.x) / viewSize.x;
		    float scaleY = static_cast<float>(worldSize.y) / viewSize.y;
		    
		    worldPoint.x = static_cast<int>(pos.x * scaleX) + viewPos.x;
		    worldPoint.y = static_cast<int>(pos.y * scaleY) + viewPos.y;
		    
		    return worldPoint;
	    }
	    
	    POINT toWindow(POINT pos) 
	    {
		    POINT worldSize = getSize();
		    POINT viewSize = getPortSize();
		    POINT viewPos = getPos();
		    
		    POINT windowPoint;
		    
		    float scaleX = static_cast<float>(viewSize.x) / worldSize.x;
		    float scaleY = static_cast<float>(viewSize.y) / worldSize.y;
		    
		    windowPoint.x = static_cast<int>((pos.x - viewPos.x) * scaleX);
		    windowPoint.y = static_cast<int>((pos.y - viewPos.y) * scaleY);
		    
		    return windowPoint;
	    }
				
		
		
		
		
		
		
		
		
		private:
		RECT world;
		RECT port;
		
			
	};



	
	
	class Texture
	{
		public:
		
		HBITMAP bitmap;
		int width = 0;
		int height = 0;
		
		
		Texture() = default;
		
		Texture(std::string path)
		{
			load(path);
		}
		
		
	    ~Texture()
	    {
	        if (bitmap != NULL)
	        {
	            DeleteObject(bitmap);
	            bitmap = NULL;
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
	                DeleteObject(bitmap);
	            }
	            
	            bitmap = other.bitmap;
	            width = other.width;
	            height = other.height;
	            
	            other.bitmap = NULL;
	            other.width = 0;
	            other.height = 0;
	        }
	        return *this;
	    }		
		
		
	
	
	
	
		bool load(std::string path)
		{


	        if (bitmap != NULL)
	        {
	            DeleteObject(bitmap);
	            bitmap = NULL;
	            width = 0;
	            height = 0;
	        }


			if(!ResolveRelativePath(path))
				return false;


			bitmap = (HBITMAP)LoadImageW(
			NULL,
			WIDE(path).c_str(),
			IMAGE_BITMAP,
			0,
			0,
			LR_LOADFROMFILE
			);
			
			
			
			if(bitmap == NULL)
			{
				std::cerr << "Failed to load image at " << std::quoted(path) << ".\n";
				return false;
			}
			
            BITMAP bm;
            GetObject(bitmap, sizeof(BITMAP), &bm);
            width = bm.bmWidth;
            height = bm.bmHeight;		
			
			return true;
		}
		
	    bool isValid() const
	    {
	        return bitmap != NULL;
	    }		
	    
	    
	    
	    COLORREF transparencyColor = CLR_INVALID; // Add this member variable
	
	    void setTransparentMask(COLORREF color) {
	        transparencyColor = color;
	    }	    
	    
	    
	    
	    
	    COLORREF getPixel(int Xindex,int Yindex)
	    {
			
			
			HDC hdc = CreateCompatibleDC(NULL);
			if(!hdc)
				return RGB(0,0,0);
			
			
			HBITMAP oldbitmap = (HBITMAP)SelectObject(hdc,bitmap);
			
			
			COLORREF result = GetPixel(hdc,Xindex,Yindex);
			
			
	        SelectObject(hdc, oldbitmap);
	        DeleteDC(hdc);	    	
	        
	        return result;
		}
		
		
		
		
		bool setPixel(COLORREF color,int Xindex,int Yindex)
		{
		
			HDC hdc = CreateCompatibleDC(NULL);
			if(!hdc)
				return false;
			
			
			HBITMAP oldbitmap = (HBITMAP)SelectObject(hdc,bitmap);
			
			color = RGB(GetBValue(color),GetGValue(color),GetRValue(color));//convert to BGR for HBITMAP. COLORREF is RGB. HBITMAP is BGR
			
			SetPixel(hdc,Xindex,Yindex,color);
			
			
	        SelectObject(hdc, oldbitmap);
	        DeleteDC(hdc);	    	
	        
	        return true;			
		}
			
	};
	
	
	
	
	class PixelArray
	{
			
		private:
		
		COLORREF* pixels = nullptr;
		
		int getPixelIndex(int x,int y)	
		{
			return (y * width + x);	
		}
		
		public:	
		int width = 0;
		int height = 0;
		
		
		PixelArray()
		{
			
		}
		
		~PixelArray()
		{
			clear();
		}
		
		
		
	    // Copy constructor
	    PixelArray(const PixelArray& other)
	    {
	        if (other.pixels && other.width > 0 && other.height > 0)
	        {
	            create(other.width, other.height);
	            for (int x = 0; x < width; x++)
	            {
	                for (int y = 0; y < height; y++)
	                {
	                    pixels[getPixelIndex(x,y)] = other.pixels[getPixelIndex(x,y)];
	                }
	            }
	        }
	    }
	    
	    // Assignment operator
	    PixelArray& operator=(const PixelArray& other)
	    {
	        if (this != &other)
	        {
	            clear();
	            if (other.pixels && other.width > 0 && other.height > 0)
	            {
	                create(other.width, other.height);
	                for (int x = 0; x < width; x++)
	                {
	                    for (int y = 0; y < height; y++)
	                    {
	                         pixels[getPixelIndex(x,y)] = other.pixels[getPixelIndex(x,y)];
	                    }
	                }
	            }
	        }
	        return *this;
	    }
	    
	    // Move constructor
	    PixelArray(PixelArray&& other) noexcept
	        : pixels(other.pixels), width(other.width), height(other.height)
	    {
	        other.pixels = nullptr;
	        other.width = 0;
	        other.height = 0;
	    }
	    
	    // Move assignment operator
	    PixelArray& operator=(PixelArray&& other) noexcept
	    {
	        if (this != &other)
	        {
	            clear();
	            pixels = other.pixels;
	            width = other.width;
	            height = other.height;
	            
	            other.pixels = nullptr;
	            other.width = 0;
	            other.height = 0;
	        }
	        return *this;
	    }		
		
		
		
		
		
		
		
		void clear()
		{
			if(pixels)
			{
				delete[] pixels;
				pixels = nullptr;
			}
			width = 0;
			height = 0;
		}
		
		
		
		
		bool create(int w,int h)
		{
			
			clear();
			
			if(w <= 0 || h <= 0)
				return false;
			
			width = w;
			height = h;
			
			
			pixels = new COLORREF[width * height];
			
	
			for(int a=0;a<width * height;a++)
			{
				pixels[a] = RGB(0,0,0);
			}
			
			return true;
		}
		
		
		
		
		
		bool convertToPixel(Texture &texture)
		{
			//convert HBITMAP to COLORREFS
			//INIT PIXELS AS 2D ARRAY USING WIDTH AND HEIGHT FROM HBITMAP.
			
			if(!texture.isValid())
				return false;
			
			width = texture.width;
			height = texture.height;
			
			
			create(width,height);
			
			for(int x=0;x<width;x++)
			{
				for(int y=0;y<height;y++)
				{
					pixels[getPixelIndex(x,y)] = texture.getPixel(x,y);
				}
			}	
			
	        return true;
						
		}
		
		
		bool sendToTexture(Texture &texture)
		{
			
	        if (!pixels || width <= 0 || height <= 0)
	            return false;
	        
	        
	        
	        
	        
	        // Create device context
	        HDC hdc = CreateCompatibleDC(NULL);
	        if (!hdc)
	            return false;
	        
	        
	        // Create bitmap
	        BITMAPINFO bmi = {};
	        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	        bmi.bmiHeader.biWidth = width;
	        bmi.bmiHeader.biHeight = -height;  // Negative for top-down
	        bmi.bmiHeader.biPlanes = 1;
	        bmi.bmiHeader.biBitCount = 32;
	        bmi.bmiHeader.biCompression = BI_RGB;
	        
			
			
			void* pBits = nullptr;
	        HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	        if (!hBitmap)
	        {
	            DeleteDC(hdc);
	            return false;
	        }
	        
	        // Copy pixels to the bitmap (bitmap expects row-major order: [y][x])
	        COLORREF* pPixels = (COLORREF*)pBits;

			//Convert RGB data to BGR for HBITMAP
			for (int i = 0; i < width * height; i++)
		    {
		        COLORREF srcPixel = pixels[i];
		        BYTE r = GetRValue(srcPixel);
		        BYTE g = GetGValue(srcPixel); 
		        BYTE b = GetBValue(srcPixel);
		        
		        // Convert RGB to BGR for bitmap
		        pPixels[i] = RGB(b, g, r);
		    }

			
		    if (texture.bitmap)
		    {
		        DeleteObject(texture.bitmap);
		    }	        
	        
	        // Set up the texture
	        texture.bitmap = hBitmap;
	        texture.width = width;
	        texture.height = height;
	        
	        // Clean up
	        DeleteDC(hdc);
	        
	        return true;
		}
		
		
		
		
		
		void setPixel(COLORREF color,int x,int y)
		{
			if(x >=0 && x < width && y >=0 && y < height)
				pixels[getPixelIndex(x,y)] = color;
			
		}
		
		COLORREF getPixel(int x,int y)
		{
			if(x >=0 && x < width && y >=0 && y < height)
				return pixels[getPixelIndex(x,y)];
			return RGB(0,0,0);
		}
		
		
		
		bool setMask(COLORREF startColor,COLORREF endColor)
		{
			
			if(width <=0 || height <=0)
				return false;
				
			for(int x=0;x<width;x++)
			{
				for(int y=0;y<height;y++)
				{
					COLORREF c = getPixel(x,y);
					if(c == startColor)
					{
						setPixel(endColor,x,y);
					}
					
				}
			}
			
			return true;
		}		
		
		
		
	};
	
	
	
		
	
	
	class Drawable
	{
		public:
		
		int x = 0,y = 0,z = 0;
		int width = 1,height = 1;
		
		Vec2f scale = {1,1};		
		
		void setScale(Vec2f s)	
		{
			scale.x = s.x;
			scale.y = s.y;
		}
		
		
		void setOrigin(POINT pos)
		{
			origin.x = pos.x;
			origin.y = pos.y;
		}
	
	
	    int getScaledWidth() const { 
	        return static_cast<int>(width * scale.x); 
	    }
	    
	    int getScaledHeight() const { 
	        return static_cast<int>(height * scale.y); 
	    }
	
		POINT getScaledOrigin()
		{
			long int xo = static_cast<int>(origin.x * scale.x);
			long int yo = static_cast<int>(origin.y * scale.y);
			
			return {xo,yo};
		}
	
		
		virtual void draw(HDC hdc,View &view) = 0;
		virtual bool contains(POINT pos) = 0;
		
		
		virtual ~Drawable() = default;
		
		
		
		
		
		private:
				
			POINT origin = {0,0};	
		
		
	};
	
	
	
	
	class Sprite : public Drawable
	{
		public:
		
		
		
		Sprite()
		{
			
		}
		
		
		
		
		virtual bool contains(POINT pos) override
		{
			
	    	POINT o = getScaledOrigin();
			return (pos.x >= x - o.x && pos.y >= y - o.y && pos.x < x + getScaledWidth() - o.x && pos.y < y + getScaledHeight() - o.y); 
		}
		
		
		virtual void draw(HDC hdc,View &view)  override
		{
			
		    if (!textureRef || !textureRef->isValid()) return;
		    
		    POINT o = getScaledOrigin();
		    
		    // Apply view transformation to position
		    POINT viewPos = view.getPos();
		    int drawX = (x - o.x) - viewPos.x;
		    int drawY = (y - o.y) - viewPos.y;
		    
		    // Only draw if visible in current view
		    if (drawX + getScaledWidth() < 0 || drawY + getScaledHeight() < 0 ||
		        drawX >= view.getPortSize().x || drawY >= view.getPortSize().y) {
		        return; // Culling: skip if not visible
		    }
		    
		    HDC hMemDC = CreateCompatibleDC(hdc);
		    SetLayout(hdc, LAYOUT_BITMAPORIENTATIONPRESERVED);
		    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, textureRef->bitmap);
		    
		    TransparentBlt(hdc,
		        drawX, 
		        drawY, 
		        getScaledWidth(), 
		        getScaledHeight(),
		        hMemDC,
		        rect.left, 
		        rect.top, 
		        rect.right,
		        rect.bottom,
		        textureRef->transparencyColor);
		        
		    SelectObject(hMemDC, hOldBitmap);
		    DeleteDC(hMemDC);
			

		}
		
		
		
		void setTexture(Texture &texture)
		{
			textureRef = &texture;
			rect.left = 0;
			rect.top = 0;
			rect.right = textureRef->width;
			rect.bottom = textureRef->height;	
			
			width = textureRef->width;
			height = textureRef->height;
			
		}
		
		
		void setTextureRect(RECT r)
		{
			rect = r;
			width = r.right;
			height = r.bottom;
		}
		
		
		RECT getTextureRect()
		{
			return rect;
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
		
		
		private:
			Texture *textureRef = nullptr;
			RECT rect;
		
					
	};
	
	
	
	
	
	class Shape : public Drawable
	{
		public:
		
		COLORREF color = RGB(125,255,255);
		
		Shape()
		{
			width = 10;
			height = 10;
			
		}
		


		virtual bool contains(POINT pos)  override
		{
			
	    	POINT o = getScaledOrigin();
			return (pos.x >= x - o.x && pos.y >= y - o.y && pos.x < x + getScaledWidth() - o.x && pos.y < y + getScaledHeight() - o.y); 
		}



	    virtual void draw(HDC hdc,View &view)  override
	    {
		    POINT o = getScaledOrigin();
		    POINT viewPos = view.getPos();
		    
		    int drawX = (x - o.x) - viewPos.x;
		    int drawY = (y - o.y) - viewPos.y;
		    
		    // Culling check
		    RECT spriteRect = {drawX, drawY, drawX + getScaledWidth(), drawY + getScaledHeight()};
		    RECT viewportRect = {0, 0, view.getPortSize().x, view.getPortSize().y};
		    RECT intersection;

		    if (!IntersectRect(&intersection, &spriteRect, &viewportRect)) {
   		    	return; // No intersection with viewport means the entire sprite is outside of view
		    }
		    
		    HBRUSH brush = CreateSolidBrush(color);
		    RECT rect = {
		        drawX, 
		        drawY, 
		        drawX + getScaledWidth(), 
		        drawY + getScaledHeight()
		    };
		    FillRect(hdc, &rect, brush);
		    DeleteObject(brush);
	    }



		friend class Window;
		
		
	};
	








	class Line : public ws::Drawable 
	{
	
		public:
		
		POINT start;
		POINT end;
		COLORREF color = RGB(0,0,255);
		int width = 2;
	    
	    
	    
	    
	    Line(POINT start = {0,0},POINT end = {0,0},int width = 2,COLORREF color = RGB(0,0,255))
	    {
	    	this->start = start;
	    	this->end = end;
	    	this->width = width;
	    	this->color = color;
		}
	    
	    
		virtual void draw(HDC hdc, ws::View &view) override {
	        // Create and select a blue pen
	        HPEN hPen = CreatePen(PS_SOLID, width, color);
	        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
	        
	        // Apply view transformation
	        POINT viewPos = view.getPos();
	        int drawX1 = start.x - viewPos.x;
	        int drawY1 = start.y - viewPos.y;
	        int drawX2 = end.x - viewPos.x;
	        int drawY2 = end.y - viewPos.y;
	        
	        // Draw a line
	        MoveToEx(hdc, drawX1, drawY1, NULL);
	        LineTo(hdc, drawX2, drawY2);
	        
	        // Restore the old pen and delete the created pen
	        SelectObject(hdc, hOldPen);
	        DeleteObject(hPen);
	        
	        
	
	    }
	    
	    
	    private:
	    virtual bool contains(POINT pos) override
	    { 
	    	return false;
		}
		
		
		public:
			
		bool onSegment(POINT p, POINT q, POINT r)
	    {
	        if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
	            q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
	            return true;
	        return false;
	    }
		
		// Returns: 0 = collinear, 1 = clockwise, 2 = counterclockwise
	    int orientation(POINT p, POINT q, POINT r)
	    {
	        long long val = (long long)(q.y - p.y) * (r.x - q.x) - 
	                       (long long)(q.x - p.x) * (r.y - q.y);
	        
	        if (val == 0) return 0;  // Collinear
	        return (val > 0) ? 1 : 2; // Clockwise or counterclockwise
	    }			
		
	    bool intersects(Line &otherLine)
	    {
	        POINT p1 = this->start;
	        POINT p2 = this->end;
	        POINT p3 = otherLine.start;
	        POINT p4 = otherLine.end;
	        
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
	
	
	
	    std::vector<POINT> vertices;
	    COLORREF fillColor = RGB(255, 0, 0);    
	    COLORREF borderColor = RGB(0, 0, 0);    
	    int borderWidth = 2;
	    bool filled = true;
	    bool closed = true;
	
	
	    Poly() = default;
	
	    
	    Poly(std::vector<POINT>& vertices, COLORREF fillColor = RGB(255, 0, 0), COLORREF borderColor = RGB(0, 0, 0), int borderWidth = 2, bool filled = true)
	    {
	        this->vertices = vertices;
	        this->fillColor = fillColor;
	        this->borderColor = borderColor;
	        this->borderWidth = borderWidth;
	        this->filled = filled;
	    }
	
	
	    void addVertex(POINT vertex) 
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
	    POINT getCentroid() 
		{
	        if (vertices.empty()) return {0, 0};
	        
	        long long sumX = 0, sumY = 0;
	        for (const auto& vertex : vertices) {
	            sumX += vertex.x;
	            sumY += vertex.y;
	        }
	        
	        return {static_cast<LONG>(sumX / vertices.size()), 
	                static_cast<LONG>(sumY / vertices.size())};
	    }
	
	
	
	    // Check if a point is inside the polygon using ray casting algorithm
	    virtual bool contains(POINT point) override 
		{
	        if (vertices.size() < 3) return false;
	        
	        int crossings = 0;
	        size_t n = vertices.size();
	        
	        for (size_t a = 0; a < n; a++) 
			{
	            POINT p1 = vertices[a];
	            POINT p2 = vertices[(a + 1) % n];
	            
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
	            POINT p1 = vertices[i];
	            POINT p2 = vertices[(i + 1) % vertices.size()];
	            
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
	            POINT p1 = vertices[i];
	            POINT p2 = vertices[(i + 1) % vertices.size()];
	            Line edge1(p1, p2);
	            
	            for (size_t j = 0; j < other.vertices.size(); j++) {
	                POINT p3 = other.vertices[j];
	                POINT p4 = other.vertices[(j + 1) % other.vertices.size()];
	                Line edge2(p3, p4);
	                
	                if (edge1.intersects(edge2)) {
	                    return true;
	                }
	            }
	        }
	        
	        return false;
	    }
	
	    
	
		RECT getBoundingRect() 
		{
	        if (vertices.empty()) return {0, 0, 0, 0};
	        
	        RECT rect = {vertices[0].x, vertices[0].y, vertices[0].x, vertices[0].y};
	        
	        for (const auto& vertex : vertices) {
	            rect.left = std::min(rect.left, vertex.x);
	            rect.top = std::min(rect.top, vertex.y);
	            rect.right = std::max(rect.right, vertex.x);
	            rect.bottom = std::max(rect.bottom, vertex.y);
	        }
	        
	        return rect;
	    }
	
	
	
	    
	
	
		virtual void draw(HDC hdc, ws::View &view) override 
		{
	
	        if (vertices.size() < 2) return;
	        
	        POINT viewPos = view.getPos();
	        std::vector<POINT> transformedPoints;
	        
	        // Apply view transformation to all points
	        for (const auto& vertex : vertices) {
	            transformedPoints.push_back({
	                vertex.x - viewPos.x,
	                vertex.y - viewPos.y
	            });
	        }
	        
	        // If filled, create and select brush
	        HBRUSH hBrush = NULL;
	        HBRUSH hOldBrush = NULL;
	        
	        if (filled && vertices.size() >= 3) {
	            hBrush = CreateSolidBrush(fillColor);
	            hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	        }
	        
	        // Create and select pen for border
	        HPEN hPen = CreatePen(PS_SOLID, borderWidth, borderColor);
	        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
	        
	        // Draw the polygon
	        if (closed && vertices.size() >= 3) {
	            Polygon(hdc, transformedPoints.data(), static_cast<int>(transformedPoints.size()));
	        } else {
	            // Draw as polyline if not closed
	            Polyline(hdc, transformedPoints.data(), static_cast<int>(transformedPoints.size()));
	        }
	        
	        // Clean up
	        SelectObject(hdc, hOldPen);
	        DeleteObject(hPen);
	        
	        if (filled && vertices.size() >= 3) {
	            SelectObject(hdc, hOldBrush);
	            DeleteObject(hBrush);
	        }
	    }
	};
	
	
	
	








	
	class Radial : public Drawable
	{
		public:
		Poly poly;
		
		Radial()
		{
			poly.fillColor = RGB(0,0,255);
			poly.borderColor = RGB(0,0,200);
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
		
		void setPosition(POINT pos)
		{
			center = pos;
			make(m_points);
		}
		
		
		void move(POINT delta)
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
		
		void setFillColor(COLORREF color)
		{
			poly.fillColor = color; 
			make(m_points);
		}
		
		void setBorderColor(COLORREF color)
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
		
		POINT getPosition()
		{
			return center;
		}
		
		int getPointCount()
		{
			return m_points;
		}
		
		
	    virtual void draw(HDC hdc, View &view) override
	    {
	        poly.draw(hdc, view);
	    }
	    
	    virtual bool contains(POINT pos) override
	    {
	        return poly.contains(pos);
	    }	
		
		
		private:
		POINT center;
		int m_points = 500;
		int radius = 10;
		
		
		void updateDrawableProperties()
	    {
	        // Set Drawable's position and size based on the poly's bounding rect
	        RECT bounds = poly.getBoundingRect();
	        x = bounds.left;
	        y = bounds.top;
	        width = bounds.right - bounds.left;
	        height = bounds.bottom - bounds.top;
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
	
	

	
	
	
	
	class Window
	{
		public:
		
		HWND hwnd;
	    int x,y,width, height;		
		bool isTransparent = false;

		private:
			
		bool isRunning = true;
		std::queue<MSG> msgQ;
		

				
		public:		
		
		
		View view;
		
		
        HBITMAP stretchBufferBitmap;
        HDC stretchBufferDC;		

	    HDC backBufferDC;
	    HBITMAP backBufferBitmap;

		
		INITCOMMONCONTROLSEX icex;
		
		Window(int width,int height,std::string title,DWORD style = WS_OVERLAPPEDWINDOW)
		{
			
			style |= WS_CLIPCHILDREN;
			
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
			
			
			
			
			
			
			
				
	        HDC hdc = GetDC(hwnd);
	        backBufferDC = CreateCompatibleDC(hdc);
	        backBufferBitmap = CreateCompatibleBitmap(hdc, view.getSize().x, view.getSize().y);
	        SelectObject(backBufferDC, backBufferBitmap);
	        
			stretchBufferDC = CreateCompatibleDC(hdc);
            stretchBufferBitmap = CreateCompatibleBitmap(hdc, width, height);
            SelectObject(stretchBufferDC, stretchBufferBitmap);
			
			ReleaseDC(hwnd, hdc);			
			
			
			clear();
			
			
			
			
			
			
            windowInstances[hwnd] = this;			


            ShowWindow(hwnd, SW_SHOW);
            UpdateWindow(hwnd);


			
		}
		
		
		
        ~Window()
        {
            windowInstances.erase(hwnd);
            DeleteObject(backBufferBitmap);
        	DeleteDC(backBufferDC);
            DeleteObject(stretchBufferBitmap);
        	DeleteDC(stretchBufferDC);
        	DeleteObject(LegacyAlpha.hbmp);
		    DeleteDC(LegacyAlpha.hdcMem);
        }
		
		
		
		
		
		void setView(View &v)
		{
			view = v;
		}
		
		
    	
    	private:
		BYTE alpha = 255;
	    COLORREF transparencyColor = RGB(-1,0,0); 
		public:	
			
	    void setChromaKey(COLORREF color = RGB(-1,-1,-1),BYTE alphaValue = 255,std::string type = "modern") 
		{
			
	        transparencyColor = color;
	        alpha = alphaValue;
	        if(type == "legacy")
	        	legacyTransparency = true;
	        else
	        	legacyTransparency = false;
	        
	        
	    	COLORREF c = transparencyColor;
	    	
	    	if(!(GetRValue(c) < 0 || GetRValue(c) > 255 || GetGValue(c) < 0 || GetGValue(c) > 255 || GetBValue(c) < 0 || GetBValue(c) > 255)    ||    alpha < 255)
	    	{
	    		//is valid color or semi-transparent
				
				LONG_PTR style = GetWindowLongPtr(hwnd,GWL_STYLE);
	    		style |= WS_EX_LAYERED;
	    		SetWindowLongPtrA(hwnd,GWL_EXSTYLE,style);
	    		
	    		bool legacy = legacyTransparency;
	    		
	    		if(!legacy)
	    		{
					SetLayeredWindowAttributes(hwnd,c,alpha,LWA_COLORKEY | LWA_ALPHA);
	    		}
	    		else
	    		{
				
	    			//setup for ARGB 32bit transparency - legacy
		    		HDC hdc = GetDC(hwnd);
					SetLayout(hdc, LAYOUT_BITMAPORIENTATIONPRESERVED); // Set layout as left to right
					LegacyAlpha.hdcMem = CreateCompatibleDC(hdc);
			        
					BITMAPINFO bmi = {0};
			        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			        bmi.bmiHeader.biWidth = width;
			        bmi.bmiHeader.biHeight = -height;
			        bmi.bmiHeader.biPlanes = 1;
			        bmi.bmiHeader.biBitCount = 32;
			        bmi.bmiHeader.biCompression = BI_RGB;
			        
			        LegacyAlpha.hbmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &LegacyAlpha.pvBits, NULL, 0);
			        SelectObject(LegacyAlpha.hdcMem, LegacyAlpha.hbmp);

				}


	    		isTransparent = true;

	    		
			}
			else
				isTransparent = false;
				
				
			view.setPortSize({width,height});
	        
	    }	
		
		
		
		
		
		
		
		
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
		
		
		
		
		
		
		
		
		
		
		
		
	    void clear(COLORREF color = RGB(0,0,0)) {


			//Update back buffer to contain the visible world area. 


	    	
	        HBRUSH brush = CreateSolidBrush(color);
	        RECT rect = {0, 0, view.getSize().x, view.getSize().y};
	        FillRect(backBufferDC, &rect, brush);
	        DeleteObject(brush);
	        

	    }
		
		
		
		
		
		
		
		
		
		
		
		
		
		void draw(Drawable &draw)
		{
			
		    SetLayout(backBufferDC, LAYOUT_BITMAPORIENTATIONPRESERVED);
		    draw.draw(backBufferDC,view);
		}
		
		
		
		
		
		
		
	    void display() 
		{
			
			
			
			
		    HDC hdc = GetDC(hwnd);
		    SetLayout(hdc, LAYOUT_BITMAPORIENTATIONPRESERVED);
		    
		    // Just copy the entire back buffer to stretch buffer
		    StretchBlt(stretchBufferDC, 
		    0, 
			0, 
			width, 
			height,
		    backBufferDC, 
		    0, 
			0, 
			view.getSize().x, 
			view.getSize().y,  
		    SRCCOPY);
			


			
		    if(isTransparent && legacyTransparency)
	    	{
		        
		        
		        if (!LegacyAlpha.hdcMem || !LegacyAlpha.hbmp) {
		            setChromaKey(transparencyColor, alpha,"legacy"); // Recreate buffers just in case someone tries to use the isTransparent variable directly instead of using the setTransparency function.
		        }		        
		        
		        
		        // Copy from stretch buffer to ARGB buffer
		        SetLayout(LegacyAlpha.hdcMem, LAYOUT_BITMAPORIENTATIONPRESERVED); // Set layout as left to right
		        BitBlt(LegacyAlpha.hdcMem, 0, 0, width, height, stretchBufferDC, 0, 0, SRCCOPY);
	

		        
		        BLENDFUNCTION blend = {AC_SRC_OVER, 0, alpha, 0};
		        POINT ptDst = {0, 0};
		        SIZE size = {width, height};
		        POINT ptSrc = {0, 0};
		        
		        UpdateLayeredWindow(hwnd, hdc, &ptDst, &size, LegacyAlpha.hdcMem, &ptSrc, transparencyColor, &blend, ULW_ALPHA | ULW_COLORKEY);
		        
		        
		    }
		    else 
		    {
		        BitBlt(hdc, 0, 0, width, height, stretchBufferDC, 0, 0, SRCCOPY);	        
//				InvalidateRect(hwnd, NULL, FALSE);
//		        UpdateWindow(hwnd);
		    }

			ReleaseDC(hwnd, hdc);

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
	             
	             	SetLayout(hdc, LAYOUT_BITMAPORIENTATIONPRESERVED);
	                // Draw the back buffer to the screen
	                BitBlt(hdc, 0, 0, width, height, stretchBufferDC, 0, 0, SRCCOPY);
	                
	                EndPaint(hwnd, &ps);
	                return 0;
	            }
	                
	            case WM_SIZE: {
				
				   
				    
					width = LOWORD(lParam);
	                height = HIWORD(lParam);
	                
	                
	                
	                
	                //viewport size should be updated here to keep proportion even after window has changed size.
	                //for now we will wait to do that. For now we will just set the viewport as the window size.
					
					view.setPortSize({width,height});
					
					
					
	                
			        HDC hdc = GetDC(hwnd);
			        			        
			        
			        DeleteObject(stretchBufferBitmap);
			        stretchBufferBitmap = CreateCompatibleBitmap(hdc, width, height); 	//this will also likely need to change to accomodate the viewport size.
			        SelectObject(stretchBufferDC, stretchBufferBitmap);
			        
			        
				    // RECREATE TRANSPARENCY BUFFERS WHEN SIZE CHANGES
				    if (isTransparent) {
				        if (LegacyAlpha.hbmp) DeleteObject(LegacyAlpha.hbmp);
				        if (LegacyAlpha.hdcMem) DeleteDC(LegacyAlpha.hdcMem);
				        
				        LegacyAlpha.hdcMem = CreateCompatibleDC(hdc);
				        BITMAPINFO bmi = {0};
				        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				        bmi.bmiHeader.biWidth = width;
				        bmi.bmiHeader.biHeight = -height;
				        bmi.bmiHeader.biPlanes = 1;
				        bmi.bmiHeader.biBitCount = 32;
				        bmi.bmiHeader.biCompression = BI_RGB;
				        
				        LegacyAlpha.hbmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &LegacyAlpha.pvBits, NULL, 0);
				        SelectObject(LegacyAlpha.hdcMem, LegacyAlpha.hbmp);
				    }			        
			        
			        
			        
			        ReleaseDC(hwnd, hdc);
						                
	                clear();
	                return 0;
	            	break;
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
        
        
        
        
				
		
		
		struct LEGACY_ALPHA
		{
		
			void* pvBits = NULL;
			HBITMAP hbmp = NULL;
			HDC hdcMem = NULL;
		}LegacyAlpha;
		
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
	
		POINT getMousePos(Window &window)
		{
			
		    POINT p;
		    if(!GetCursorPos(&p))
		    {
		        return {0,0};
		    }
		    
		    ScreenToClient(window.hwnd, &p); // Convert to client coordinates
		    p = window.view.toWorld(p);
		    return p;
		}
		
		POINT getMousePos()
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
		std::string titleText = "Title";
		int x = 0,y = 0,width = 100,height = 100;
		unsigned int controlID = 0;
		COLORREF backgroundColor = RGB(0,0,0);
		COLORREF textColor = RGB(255,255,255);
		COLORREF borderColor = RGB(0,0,0);
		
		private:
		
		int litX = 0,litY = 0,litWidth = 0,litHeight = 0;
		

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
		
		void setTitle(std::string newText)
		{
			titleText = newText;
			if (hwnd)
                SetWindowTextA(hwnd, titleText.c_str());
            update();
		}
		
		std::string getTitle()
		{
			return titleText;
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

		    POINT originalSize = parentRef->view.getSize();
		    
		    
		    
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
			
			
			if(getMsg != nullptr)
			{
				MSG &msg = *getMsg;
			
				if(msg.message == WM_DRAWITEM)
				{
				    LPDRAWITEMSTRUCT pDrawItem = (LPDRAWITEMSTRUCT)msg.lParam;
				    
				    if (pDrawItem->CtlID == controlID)
				    {
				        HDC hdc = pDrawItem->hDC;
				        RECT rc = pDrawItem->rcItem;
				        
				        // Draw background
				        HBRUSH hBrush = CreateSolidBrush(backgroundColor);
				        FillRect(hdc, &rc, hBrush);
				        DeleteObject(hBrush);
				        
				        // Draw border
				        if (pDrawItem->itemState & ODS_SELECTED)
				        {
				            DrawEdge(hdc, &rc, BDR_SUNKENOUTER, BF_RECT);
				        }
				        else
				        {
				            DrawEdge(hdc, &rc, BDR_RAISEDINNER, BF_RECT);
				        }
				        
				        // Draw text
				        SetBkMode(hdc, TRANSPARENT);
				        SetTextColor(hdc, textColor);
				        
				        DrawTextA(hdc, getTitle().c_str(), -1, &rc, 
				                 textStyle);
				        
				        InvalidateRect(hwnd, NULL, TRUE);
				    }
				}	
				
						
			}
			
			
			
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
			ws::TO_LPCSTR(getTitle()),
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
			LPCSTR(""),//No window text
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

	
	
}


#endif