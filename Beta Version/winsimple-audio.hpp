#ifndef WINSIMPLE_AUDIO
#define WINSIMPLE_AUDIO

//Audio Linking: -lwinmm

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

#endif