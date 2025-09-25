
#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>
                


class Camera
{
        public:
        double x,y,z;
        
        int visible = 500; 
        int perspective = 240;
        
        int screenWidth = 960;
        int screenHeight = 540;         
        
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