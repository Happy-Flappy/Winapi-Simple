



class ShiftData
{


	public:
	int currentframe=0;
	float delay = 0.15;
	float totaltime=0;
	bool ended = false;
	bool start = true;//To make sure that timer is 0 when starting.
	std::vector <ws::IntRect> rect;
	ws::Timer timer;
	
	
};


ws::IntRect Shift(ShiftData &shift) 
{
	if(start && !ended)
	{
		shift.timer.restart();
		shift.start = false;
	}
	
	if(shift.timer.getSeconds() >= shift.delay)
	{
	
	
			
		shift.currentframe++;
		shift.totaltime=0;
			
		if(shift.currentframe >= shift.rect.size())
		{
			shift.currentframe=0;
			shift.ended = true;
			shift.start = true;
		}
	
	
	}
	
	return shift.rect[shift.currentframe];
	
}