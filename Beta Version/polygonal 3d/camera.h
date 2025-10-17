
#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>
                
#define PI 3.14159265358979323846

class Camera
{
        public:
        double x,y,z;
        
        int visible = 500; 
        int perspective = 900;
        
        int screenWidth = 960;
        int screenHeight = 540;         
        
        
        Camera()
        {
        	setHorizontalFOV(60);
		}
        
	    void setHorizontalFOV(double fov_degrees)
	    {
	        //Degrees to radians
	        double fov_radians = fov_degrees * (PI / 180.0);
	
	        perspective = static_cast<int>( (screenWidth / 2.0) / tan(fov_radians / 2.0) );
	    }
	    
	    void setVerticalFOV(double fov_degrees)
	    {
	        // Degrees to radians
	        double fov_radians = fov_degrees * (PI / 180.0);
	
	        // Calculate and set the new perspective value
	        perspective = static_cast<int>( (screenHeight / 2.0) / tan(fov_radians / 2.0) );
	    }        
        
        
        
        void ToScreen(double worldX,double worldY,double worldZ, int &screenX,int &screenY,float ZScale)
        {
            
            double relativeX = worldX - x;
            double relativeY = worldY - y;
            double relativeZ = worldZ - z;
            
    
    
    
            if(relativeZ < 0)
            {
                screenX = -1000000;
                screenY = -1000000;
                ZScale = 0;
                return;
            }
            
            
            
            
            float scale = perspective / relativeZ;
            
            screenX = (relativeX * scale) + (screenWidth/2);
            screenY = (relativeY * scale) + (screenHeight/2);
            
            ZScale = scale;
            
            if(scale < 0)
                    ZScale = 0;
        }               
        

        
        
        float getHillHeight(double x, double z) {
            double amplitude1 = 30.0f;
            double frequency1 = 0.01f;
            double amplitude2 = 30.0f;
            double frequency2 = 0.01f;
        
            float y = amplitude1 * std::sin(frequency1 * x) +
                      amplitude2 * std::cos(frequency2 * z) +
                      amplitude1 * std::sin(frequency1 * (x + z) * 0.7f);
            y += 480.0f;
            return y;
        
        
        
        }
                
        
                
}camera;



#endif