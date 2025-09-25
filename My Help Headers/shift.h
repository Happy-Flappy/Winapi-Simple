

#include <vector>
#include <chrono>
#include <optional>


struct ShiftRect
{
	int left,top,width,height;
};


class ShiftData
{


	public:
	int currentframe=0;
	float delay = 0.15;
	float totaltime=0;
	bool ended = false;
	std::vector <ShiftRect> rect;
	
	std::optional<std::chrono::steady_clock::time_point> last_frame_time;
};


ShiftRect Shift(ShiftData &shift) 
{
	
    auto current_time = std::chrono::steady_clock::now();

    if (!shift.last_frame_time.has_value())
    {
        shift.last_frame_time = current_time;
        return shift.rect[shift.currentframe];
    }
    
    std::chrono::duration<float> delta_time = current_time - shift.last_frame_time.value();
    
    shift.last_frame_time = current_time;
    
    shift.totaltime += delta_time.count();	
	
	
	if(shift.totaltime >= shift.delay)
	{
	
	
			
		shift.currentframe++;
		shift.totaltime=0;
			
		if(shift.currentframe >= shift.rect.size())
		{
			shift.currentframe=0;
			shift.ended = true;
		}
	
	
	}
	
	return shift.rect[shift.currentframe];
	
}