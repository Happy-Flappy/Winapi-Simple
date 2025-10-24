
#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>
                
#define PI 3.14159265358979323846

class Camera
{
        public:
        double x,y,z;
        double yaw,pitch,roll;
        
        int visible = 500; 
        int perspective = 900;
        
        int screenWidth = 960;
        int screenHeight = 540;         

	    // Camera coordinate system vectors
	    double forwardX, forwardY, forwardZ;
	    double rightX, rightY, rightZ;
	    double upX, upY, upZ;        
        
        Camera()
        {
        	setHorizontalFOV(60);
        	setRotation(0,0,0);
		}
 
 
 
 
 
 
 
 
 
 
 
 
	    void setRotation(double yaw_degrees, double pitch_degrees, double roll_degrees)
	    {
	        // Convert to radians
	        yaw = (yaw_degrees + 90) * (PI / 180.0);
	        pitch = pitch_degrees * (PI / 180.0);
	        roll = roll_degrees * (PI / 180.0);
	        
	        updateCameraVectors();
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
        
 
 
 
	    void updateCameraVectors()
	    {
	        // Calculate forward vector from yaw and pitch
	        forwardX = cos(yaw) * cos(pitch);
	        forwardY = sin(pitch);
	        forwardZ = sin(yaw) * cos(pitch);
	        
	        // Normalize forward vector
	        double length = sqrt(forwardX * forwardX + forwardY * forwardY + forwardZ * forwardZ);
	        if (length > 0) {
	            forwardX /= length;
	            forwardY /= length;
	            forwardZ /= length;
	        }
	        
	        // Calculate right vector (cross product of forward and world up (0,1,0))
	        rightX = forwardZ;  // cross(forward, (0,1,0))
	        rightY = 0;
	        rightZ = -forwardX;
	        
	        // Normalize right vector
	        length = sqrt(rightX * rightX + rightY * rightY + rightZ * rightZ);
	        if (length > 0) {
	            rightX /= length;
	            rightY /= length;
	            rightZ /= length;
	        }
	        
	        // Calculate up vector (cross product of right and forward)
	        upX = rightY * forwardZ - rightZ * forwardY;
	        upY = rightZ * forwardX - rightX * forwardZ;
	        upZ = rightX * forwardY - rightY * forwardX;
	        
	        // Apply roll to up vector
	        if (roll != 0) {
	            double cosRoll = cos(roll);
	            double sinRoll = sin(roll);
	            
	            // Rotate up vector around forward axis by roll angle
	            double newUpX = upX * cosRoll + rightX * sinRoll;
	            double newUpY = upY * cosRoll + rightY * sinRoll;
	            double newUpZ = upZ * cosRoll + rightZ * sinRoll;
	            
	            upX = newUpX;
	            upY = newUpY;
	            upZ = newUpZ;
	            
	            // Recalculate right vector to maintain orthogonality
	            rightX = forwardY * upZ - forwardZ * upY;
	            rightY = forwardZ * upX - forwardX * upZ;
	            rightZ = forwardX * upY - forwardY * upX;
	        }
	    }
 
        
        
        bool ToScreen(double worldX,double worldY,double worldZ, int &screenX,int &screenY,float &ZScale)
        {

	        // Calculate relative position in world coordinates
	        double worldRelativeX = worldX - x;
	        double worldRelativeY = worldY - y;
	        double worldRelativeZ = worldZ - z;
	        
	        // Transform to camera coordinate system using the camera's rotation matrix
	        double relativeX = worldRelativeX * rightX + worldRelativeY * rightY + worldRelativeZ * rightZ;
	        double relativeY = worldRelativeX * upX + worldRelativeY * upY + worldRelativeZ * upZ;
	        double relativeZ = worldRelativeX * forwardX + worldRelativeY * forwardY + worldRelativeZ * forwardZ;
    
    
    
            if(relativeZ < 100 || relativeZ > visible)
            {
                return false;
            }
            
            
            
            float scale = perspective / relativeZ;
            
            screenX = (relativeX * scale) + (screenWidth/2);
            screenY = (relativeY * scale) + (screenHeight/2);
            
            ZScale = scale;
            
            if(scale < 0)
                    ZScale = 0;
            return true;
        }               
        
	
	    void rotateYaw(double angle_degrees)
	    {
	        yaw += angle_degrees * (PI / 180.0);
	        updateCameraVectors();
	    }
	    
	    void rotatePitch(double angle_degrees)
	    {
	        pitch += angle_degrees * (PI / 180.0);
	        // Clamp pitch to avoid flipping
	        const double maxPitch = PI / 2 - 0.1; // Almost 90 degrees
	        if (pitch > maxPitch) pitch = maxPitch;
	        if (pitch < -maxPitch) pitch = -maxPitch;
	        updateCameraVectors();
	    }
	    
	    void rotateRoll(double angle_degrees)
	    {
	        roll += angle_degrees * (PI / 180.0);
	        updateCameraVectors();
	    }
	    
	    // Move camera relative to its orientation
	    void moveForward(double distance)
	    {
	        x += forwardX * distance;
	        y += forwardY * distance;
	        z += forwardZ * distance;
	    }
	    
	    void moveRight(double distance)
	    {
	        x += rightX * distance;
	        y += rightY * distance;
	        z += rightZ * distance;
	    }
	    
	    void moveUp(double distance)
	    {
	        x += upX * distance;
	        y += upY * distance;
	        z += upZ * distance;
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