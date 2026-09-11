#ifndef WINSIMPLE_AUDIO
#define WINSIMPLE_AUDIO

//Audio Linking: -lwinmm

//automated linking for visual studio MSVC
#ifdef _MSC_VER
#pragma comment(lib, "winmm.lib")
#endif

#include <filesystem>
#include <fstream>

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
		
		private:
		
		bool isMemoryLoaded = false;
		std::string tempFilePath = "";	
		ws::Timer progressTimer;
		float progress = 0;
		float totalLength = 0;
		bool isPlaying = false;
		int volume = 100;
		
		public:
		
		~Wav()
		{
			// Clean up
		    if(!ID.empty())
		    {
		        std::string status = getChannelStatus();
		        if(status != "error") {
		            std::string command = "close " + ID;
		            mciSendStringA(command.c_str(), NULL, 0, NULL);
		        }
			}			
			
			if(!tempFilePath.empty())
				std::filesystem::remove(tempFilePath);
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
				std::string status = getChannelStatusMCI(a);
				if(status == "playing" || status == "paused")
					continue;				
				channel = a;
				break;
			}
			if(channel == -1)
				std::cerr << "A sound channel could not be auto detected! \nThis means that either the maximum number of device\n sound contexts have been created OR that the range of channel\n ID's from 0 to 100 are all in use.\n Try using a number below or above the minimum and maximum attempted values.\n";
			
			return channel;
		}


		std::string getChannelStatus()
		{
			if(ID.empty()) return "error";
			return isPlaying ? "playing" : "stopped";
		}
		
		
		static std::string getChannelStatusMCI(int m_channel)
		{
			
			std::string m_ID = std::to_string(m_channel);
			
			
		    char returnBuffer[128]; // Buffer to store the status string
		    std::string command = "status " + m_ID + " mode";
		    
		    memset(returnBuffer, 0, sizeof(returnBuffer));
		   
		    
		    // Send the command and check for errors
		    if (mciSendStringA(command.c_str(), returnBuffer, sizeof(returnBuffer), NULL) == 0) 
		        return returnBuffer; // Returns "playing", "stopped", etc.
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
			
			
			command = "play " + m_ID;
			if(!mciSimple(command))
				return false;
			
			return true;
		}

		//saves a temporary file so that the program can open and play that file as sound. - Extension must be given!
		bool loadFromMemory(const void* data, size_t size, int m_channel, bool m_blocking,std::string extension) 
		{
			std::filesystem::path tempDir = std::filesystem::current_path();
			tempFilePath = (tempDir / ("ws_audio_" + std::to_string(rand()) + "_" + std::to_string(clock()) + extension)).string();

			std::ofstream file(tempFilePath, std::ios::binary);
			if(!file.write(static_cast<const char*>(data), size)) 
			{
				std::cerr << "Failed to write audio buffer to temporary file.\n";
				return false;
			}
			file.close();

			if(!open(tempFilePath, m_channel, m_blocking)) 
			{
				std::cerr << "Failed to open temporary audio file.\n";
				std::filesystem::remove(tempFilePath);
				return false;
			}

			isMemoryLoaded = true;
			
			char buf[128];
			mciSendStringA(("status " + ID + " length").c_str(), buf, sizeof(buf), NULL);
			totalLength = atof(buf);   
			progress = 0.0f;
			isPlaying = false;			
			
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
			
			
			char buf[128];
			mciSendStringA(("status " + ID + " length").c_str(), buf, sizeof(buf), NULL);
			totalLength = atof(buf);   
			progress = 0.0f;
			isPlaying = false;			
			
			return true;
		}
		
		void play()
		{
			if(ID.empty())
			{
				std::cerr<<"Sound attempted to play but was not initialized! Use open() before play().\n";
				return;
			}
			
			
			if(isPlaying && blocking)
				return;
			
			std::string command = "play " + ID;
			if(!mciSimple(command))
				return;		
			
			// Restart the timer to measure from this moment
			progressTimer.restart();
			isPlaying = true;
		}
		
		
		
		static void stop(int m_channel)
		{
			std::string m_ID = std::to_string(m_channel);
			std::string command = "pause "+ m_ID;
			mciSimple(command);
		}
		
		void stop()
		{
			if(isPlaying) 
			{
				//accumilate the previous elapsed time so that the true time will be backed up.
				progress += progressTimer.restart();   
				isPlaying = false;
			}
			
			std::string command = "pause "+ ID;
			mciSimple(command);
		}
		
		
		
		
	    bool setVolume(int percent)
	    {
	    	
	    	if(ID.empty())
	    	{
	    		std::cerr << "Unsupported Audio Action: setVolume cannot be used before using open()\n";
	    		return false;
			}
			
			if(extension == ".mid" || extension == ".midi")
			{
				std::cerr << "Unsupported Audio Action: An attempt was made to set the volume of a midi file!\n";
				return false;
			}
			
			
	        // Convert percentage back to MCI volume (0-1000)
	        int volume2 = (percent * 1000) / 100;
	        volume2 = std::max(0, std::min(1000, volume2));
	        
	        // Try different MCI volume commands in order
	        std::string commands[] = {
	            "set " + ID + " audio volume to " + std::to_string(volume2),
	            "setaudio " + ID + " volume to " + std::to_string(volume2), 
	            "set " + ID + " volume " + std::to_string(volume2)
	        };
	        
	        for(const auto& cmd : commands) 
	        if(mciSimple(cmd, false))   
	        {
				volume = percent;
				return true;
			}
	        
	        std::cerr << "Volume control not supported on this system\n";
	        return false;
	    }
	    
	    int getVolume()
	    {

	    	if(ID.empty())
	    	{
	    		std::cerr << "Unsupported Audio Action: getVolume cannot be used before using open()\n";
	    		return 0;
			}
	    	
			if(extension == ".mid" || extension == ".midi")
			{
				std::cerr << "Unsupported Action: An attempt was made to get the volume of a midi file!\n";
				return 100;
			}
			
			return volume;
	    }
		
		
		bool setProgress(long seconds)
		{
			if(ID.empty())
	    	{
	    		std::cerr << "Unsupported Audio Action: setProgress cannot be used before using open()\n";
	    		return false;
			}
			
			std::string command = "";
		    if(isPlaying) 
		        command = "play " + ID + " from " + std::to_string(seconds * 1000);
		    else 
		        command = "seek " + ID + " to " + std::to_string(seconds * 1000);
		    
		    
			if(!mciSimple(command))
				return false;
			
			progress = static_cast<float>(seconds * 1000);
			
			if(isPlaying)     
				progressTimer.restart();
			return true;
		}
		
		
		long getProgress()
		{
			if(isPlaying)
				return progress + static_cast<long>(progressTimer.getMilliSeconds());
			else
				return static_cast<long>(progress);
		}	
		
		
		
		float getLength()
		{
	    	return totalLength;
		}
	
		bool isFinished()
		{
			if(ID.empty())
	    	{
	    		std::cerr << "Unsupported Audio Action: isFinished() cannot be used before using open()\n";
	    		return false;
			}
			if(getProgress() >= getLength())
			{
				if(isPlaying) stop();
				return true;
			}
			return false;
		}
		
	};		
}

#endif