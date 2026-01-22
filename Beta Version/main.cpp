#include "winsimple.h"
#include <map>
#include <string>

namespace ws
{
    namespace Input
    {
        // Key states
        inline bool keyDown[256] = {false};
        inline bool keyTapped[256] = {false};
        inline bool keyReleased[256] = {false};  // New: track key releases
        
        // Mouse states
        inline ws::Vec2i mousePos;
        inline ws::Vec2i mouseDelta;  // Movement since last frame
        inline ws::Vec2i mousePrevPos;  // Previous position
        inline bool mouseButtons[5] = {false};  // Left, Right, Middle, X1, X2
        inline bool mouseButtonsTapped[5] = {false};
        inline bool mouseButtonsReleased[5] = {false};
        inline int mouseWheelDelta = 0;  // Wheel scrolling
        
        // Internal tracking
        inline bool anyKeyPressed = false;
        inline bool anyMouseButtonPressed = false;
        
        // Map Windows virtual keys to more readable names
        std::map<std::string, int> keyNames = {
            {"A", 'A'}, {"B", 'B'}, {"C", 'C'}, {"D", 'D'}, {"E", 'E'},
            {"F", 'F'}, {"G", 'G'}, {"H", 'H'}, {"I", 'I'}, {"J", 'J'},
            {"K", 'K'}, {"L", 'L'}, {"M", 'M'}, {"N", 'N'}, {"O", 'O'},
            {"P", 'P'}, {"Q", 'Q'}, {"R", 'R'}, {"S", 'S'}, {"T", 'T'},
            {"U", 'U'}, {"V", 'V'}, {"W", 'W'}, {"X", 'X'}, {"Y", 'Y'},
            {"Z", 'Z'}, {"Space", VK_SPACE}, {"Enter", VK_RETURN},
            {"Escape", VK_ESCAPE}, {"Shift", VK_SHIFT}, {"Control", VK_CONTROL},
            {"Alt", VK_MENU}, {"Tab", VK_TAB}, {"Left", VK_LEFT},
            {"Right", VK_RIGHT}, {"Up", VK_UP}, {"Down", VK_DOWN}
        };
        
        // Reset frame-specific states
        void beginFrame()
        {
            // Reset tapped and released states
            for(int i = 0; i < 256; ++i) {
                keyTapped[i] = false;
                keyReleased[i] = false;
            }
            
            // Reset mouse states
            for(int i = 0; i < 5; ++i) {
                mouseButtonsTapped[i] = false;
                mouseButtonsReleased[i] = false;
            }
            
            // Reset wheel and delta
            mouseDelta = ws::Vec2i(0, 0);
            mouseWheelDelta = 0;
            
            anyKeyPressed = false;
            anyMouseButtonPressed = false;
        }
        
        // Keyboard functions
        bool getKey(int keyCode, bool tapping = false)
        {
            if (keyCode < 0 || keyCode >= 256) return false;
            return tapping ? keyTapped[keyCode] : keyDown[keyCode];
        }
        
        bool getKey(const std::string& keyName, bool tapping = false)
        {
            auto it = keyNames.find(keyName);
            if (it != keyNames.end()) {
                return getKey(it->second, tapping);
            }
            return false;
        }
        
        bool getKeyReleased(int keyCode)
        {
            if (keyCode < 0 || keyCode >= 256) return false;
            return keyReleased[keyCode];
        }
        
        bool isAnyKeyPressed() { return anyKeyPressed; }
        
        // Mouse functions
        ws::Vec2i getMousePos() { return mousePos; }
        ws::Vec2i getMouseDelta() { return mouseDelta; }
        
        bool getMouseButton(int button, bool tapping = false)
        {
            if (button < 0 || button >= 5) return false;
            return tapping ? mouseButtonsTapped[button] : mouseButtons[button];
        }
        
        bool getMouseButtonReleased(int button)
        {
            if (button < 0 || button >= 5) return false;
            return mouseButtonsReleased[button];
        }
        
        int getMouseWheelDelta() { return mouseWheelDelta; }
        bool isAnyMouseButtonPressed() { return anyMouseButtonPressed; }
        
        // Button constants for easier use
        enum MouseButton {
            LEFT = 0,
            RIGHT = 1,
            MIDDLE = 2,
            X1 = 3,
            X2 = 4
        };
        
        // Main update function
        void update(MSG &e)
        {
            // Store previous mouse position for delta calculation
            mousePrevPos = mousePos;
            
            // Handle keyboard events
            if (e.message == WM_KEYDOWN || e.message == WM_SYSKEYDOWN)
            {
                int keyCode = e.wParam;
                if (keyCode >= 0 && keyCode < 256)
                {
                    if (!keyDown[keyCode])  // First time pressed
                    {
                        keyTapped[keyCode] = true;
                    }
                    keyDown[keyCode] = true;
                    anyKeyPressed = true;
                }
            }
            else if (e.message == WM_KEYUP || e.message == WM_SYSKEYUP)
            {
                int keyCode = e.wParam;
                if (keyCode >= 0 && keyCode < 256)
                {
                    keyDown[keyCode] = false;
                    keyReleased[keyCode] = true;
                }
            }
            
            // Handle mouse movement
            if (e.message == WM_MOUSEMOVE)
            {
                mousePos.x = GET_X_LPARAM(e.lParam);
                mousePos.y = GET_Y_LPARAM(e.lParam);
                
                // Calculate delta movement
                mouseDelta.x = mousePos.x - mousePrevPos.x;
                mouseDelta.y = mousePos.y - mousePrevPos.y;
                
            }
            
            // Handle mouse button down
            if (e.message == WM_LBUTTONDOWN)
            {
                if (!mouseButtons[LEFT])
                {
                    mouseButtonsTapped[LEFT] = true;
                }
                mouseButtons[LEFT] = true;
                anyMouseButtonPressed = true;
            }
            else if (e.message == WM_RBUTTONDOWN)
            {
                if (!mouseButtons[RIGHT])
                {
                    mouseButtonsTapped[RIGHT] = true;
                }
                mouseButtons[RIGHT] = true;
                anyMouseButtonPressed = true;
            }
            else if (e.message == WM_MBUTTONDOWN)
            {
                if (!mouseButtons[MIDDLE])
                {
                    mouseButtonsTapped[MIDDLE] = true;
                }
                mouseButtons[MIDDLE] = true;
                anyMouseButtonPressed = true;
            }
            else if (e.message == WM_XBUTTONDOWN)
            {
                int button = GET_XBUTTON_WPARAM(e.wParam);
                int buttonIndex = (button == XBUTTON1) ? X1 : X2;
                
                if (!mouseButtons[buttonIndex])
                {
                    mouseButtonsTapped[buttonIndex] = true;
                }
                mouseButtons[buttonIndex] = true;
                anyMouseButtonPressed = true;
            }
            
            // Handle mouse button up
            else if (e.message == WM_LBUTTONUP)
            {
                mouseButtons[LEFT] = false;
                mouseButtonsReleased[LEFT] = true;
            }
            else if (e.message == WM_RBUTTONUP)
            {
                mouseButtons[RIGHT] = false;
                mouseButtonsReleased[RIGHT] = true;
            }
            else if (e.message == WM_MBUTTONUP)
            {
                mouseButtons[MIDDLE] = false;
                mouseButtonsReleased[MIDDLE] = true;
            }
            else if (e.message == WM_XBUTTONUP)
            {
                int button = GET_XBUTTON_WPARAM(e.wParam);
                int buttonIndex = (button == XBUTTON1) ? X1 : X2;
                
                mouseButtons[buttonIndex] = false;
                mouseButtonsReleased[buttonIndex] = true;
            }
            
            // Handle mouse wheel
            else if (e.message == WM_MOUSEWHEEL)
            {
                mouseWheelDelta = GET_WHEEL_DELTA_WPARAM(e.wParam) / WHEEL_DELTA;
            }
        }
    }
}


int main()
{
    ws::Window window(960, 540, "Input System Demo");
    
    while(window.isOpen())
    {
        ws::Input::beginFrame();
        
        MSG m;
        while(window.pollEvent(m))
        {
            ws::Input::update(m);
        }
        
        // Example usage:
        
        // Keyboard input
        if(ws::Input::getKey(VK_RETURN, true))  // Tapped Enter
        {
            std::cout << "Enter pressed!\n";
        }
        
        if(ws::Input::getKey("A"))  // Holding A key
        {
            std::cout << "Holding A key\n";
        }
        
        // Mouse input
        if(ws::Input::getMouseButton(ws::Input::LEFT, true))  // Left click tapped
        {
            std::cout << "Left clicked at: (" 
                      << ws::Input::getMousePos().x << ", " 
                      << ws::Input::getMousePos().y << ")\n";
        }
        
        if(ws::Input::getMouseButton(ws::Input::RIGHT))  // Right mouse held
        {
            // Do something while right button is held
        }
        
        // Mouse movement
        ws::Vec2i delta = ws::Input::getMouseDelta();
        if(delta.x != 0 || delta.y != 0)
        {
            std::cout << "Mouse moved by: (" << delta.x << ", " << delta.y << ")\n";
        }
        
        // Mouse wheel
        int wheel = ws::Input::getMouseWheelDelta();
        if(wheel != 0)
        {
            std::cout << "Mouse wheel: " << wheel << "\n";
        }
    }
    
    return 0;
}