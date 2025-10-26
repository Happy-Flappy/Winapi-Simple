

#ifndef KEY_EXTERNAL
#define KEY_EXTERNAL


namespace Input
{
	
	
	
	

	struct Variables
	{
		bool keyTapped[256];
		bool keyDown[256];
		int var = 256;
		POINT mousePos = {0,0};
		bool mouseLeft = false;
		bool mouseMiddle = false;
		bool mouseRight = false;
		bool mouseLeftTap = false;
		bool mouseMiddleTap = false;
		bool mouseRightTap = false;		
		
	}var;
    
    

	bool Key(char vmKey,bool tapping=false)
	{
		return tapping ? var.keyTapped[vmKey] : var.keyDown[input];
	}
	
	
	
	
	
	// Reset frame-specific states (call this at start of each frame)
    void beginFrame()
    {
        for(int i = 0; i < var.count; ++i) {
            var.keyTapped[i] = false;
        }
        mouseLeftTap = false;
        mouseRightTap = false;
        mouseMiddleTap = false;
    }
	
	
	
	
	ws::Vec2i getPosition()
	{
		return {var.mousePos.x, var.mousePos.y};
	}
	
	
	bool MouseLeft(bool tapping = false)
	{
		return mouseLeft;
	}
	
	bool MouseRight()
	{
		return mouseRight;
	}
	
	bool MouseMiddle()
	{
		return mouseMiddle;
	}
	
	
	
	
	
	
	
	void update(MSG &m)
	{
		
		
		
		if(m.message == WM_KEYDOWN) 
		{
			char keyCode = m.wParam;
            if (!var.keyDown[keyCode]) // Only trigger once per press 
			{  
            	var.keyTapped[keyCode] = true;
            }
            var.keyDown[keyCode] = true;
                
        }

        if (m.message == WM_KEYUP) 
		{
            var.keyDown[m.wParam] = false;
        }


		if(m.message == WM_MOUSEMOVE)
		{
			int x = GET_X_LPARAM(m.lParam);
			int y = GET_Y_LPARAM(m.lParam);
			var.mousePos.x = x;
			var.mousePos.y = y;
		}


		if(m.message == WM_LBUTTONDOWN)
		{
			if(!var.mouseLeft)
				var.mouseLeftTap = true;
			var.mouseLeft = true;
			
		}
		if(m.message == WM_RBUTTONDOWN)
		{
			if(!var.mouseRight)
				var.mouseRightTap = true;
			var.mouseRight = true;
		}
		if(m.message == WM_MBUTTONDOWN)
		{
			if(!var.mouseRight)
				var.mouseRightTap = true;
			var.mouseMiddle = true;
		}
		
		
		if(m.message == WM_LBUTTONUP)
		{
			var.mouseLeft = false;
		}
		if(m.message == WM_RBUTTONUP)
		{
			var.mouseRight = false;
		}
		if(m.message == WM_MBUTTONUP)
		{
			var.mouseMiddle = false;
		}
		
		
		
	}
	
}



#endif
