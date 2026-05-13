#ifndef WINSIMPLE_ANIMATION
#define WINSIMPLE_ANIMATION

namespace ws
{
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
			this->path = path;
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
			if(index < 0 || index > int(textures.size()))
			{
				std::cerr << "Invalid texture frame ID requested in getFrame() from ws::Animate! Returned Invalid!...\n";
				return nullptr;
			}
			
			return &textures[index];
		}
		
		
		double getFrameDelay(int index)
		{
			if(index <= 0 || index > int(textures.size()))
			{
				std::cerr << "Invalid texture frame ID requested in getFrameDelay() from ws::Animate! Returned 0...\n";
				return 0;
			}
			return delays[index];
		}
		
		
		bool setFrameDelay(int index,double newDelay)
		{	
			if(index <= 0 || index > int(textures.size()))
				return false;
			delays[index] = newDelay;		
		}
		
		bool setFrame(int index,ws::Texture &newTexture)
		{
			if(index <= 0 || index > int(textures.size()))
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
				
				if(currentFrame < int(delays.size()) && currentFrame < int(textures.size()) && timer.getMilliSeconds() > delays[currentFrame])
				{	
					currentTexture = textures[currentFrame];	
					currentFrame++;
					timer.restart();
				}	
				
			}
			return currentTexture;
		}
		
		
		
		std::string getPath()
		{
			return path;
		}
		
		
		
		private:
	    std::vector<ws::Texture> textures;
	    ws::Texture currentTexture;
	    std::vector<double> delays;  
		std::string status = "stopped";
		std::string path = "";
		
		int currentFrame = 0;
		int totalFrames = 0;
	    bool loop = false;
		ws::Timer timer;
				
		
	};

	




	//SHIFT ANIMATOR 

    class ShiftData
    {
		public:
        
		int currentframe = 0;
        float delay = 0.15f;
        bool ended = false;
        bool start = true;          // to make timer start at 0
        std::vector<ws::IntRect> rect;
        ws::Timer timer;

        void add(int left, int top, int width, int height)
        {
            rect.push_back({left, top, width, height});
        }

        void add(ws::IntRect r)
        {
            add(r.left, r.top, r.width, r.height);
        }
    };

    ws::IntRect Shift(ShiftData &shift)
    {
        if (shift.start && !shift.ended)
        {
            shift.timer.restart();
            shift.start = false;
        }

        if (!shift.ended && shift.timer.getSeconds() >= shift.delay)
        {
            // Advance to next frame if not at the end
            if (shift.currentframe + 1 < static_cast<int>(shift.rect.size()))
            {
                shift.currentframe++;
            }
            else
            {
                // Reached the end – stay on last frame and mark ended
                shift.ended = true;
            }
            shift.timer.restart();
        }

        return shift.rect[shift.currentframe];
    }
	
	//////////////////		
}

#endif