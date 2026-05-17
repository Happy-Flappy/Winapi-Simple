# Winsimple V1.1 API Documentation


![Graphics](../images/Screenshot 2026-05-16 201350.png)

```
#include "graphics.h"
```

## Namespace: `ws`

## Linking
-lgdi32 
-luser32 
-lmsimg32 
-lkernel32 


### Structures

#### `Vector2f`
2D floating point vector.
- `float x` - X coordinate
- `float y` - Y coordinate

#### `View`
Manages world/viewport coordinate transformations.

**Public Methods:**
- `setRect(RECT viewRect)` - Set world rectangle
- `setSize(POINT size)` - Set world size
- `setPos(POINT pos)` - Set world position
- `setPortRect(RECT portRect)` - Set viewport rectangle
- `setPortSize(POINT size)` - Set viewport size
- `setPortPos(POINT pos)` - Set viewport position
- `getRect()` → `RECT` - Get world rectangle
- `getSize()` → `POINT` - Get world size
- `getPos()` → `POINT` - Get world position
- `getPortRect()` → `RECT` - Get viewport rectangle
- `getPortSize()` → `POINT` - Get viewport size
- `getPortPos()` → `POINT` - Get viewport position
- `zoom(float factor)` - Zoom viewport (factor > 1 = zoom in)
- `move(POINT delta)` - Move world position
- `movePort(POINT delta)` - Move viewport position
- `toWorld(POINT pos)` → `POINT` - Convert window coords to world coords
- `toWindow(POINT pos)` → `POINT` - Convert world coords to window coords

### Classes

#### `Texture`
Bitmap texture container.

**Public Variables:**
- `HBITMAP bitmap` - Native bitmap handle
- `int width` - Texture width
- `int height` - Texture height
- `COLORREF transparencyColor` - Color to treat as transparent (default: CLR_INVALID)

**Public Methods:**
- `Texture()` - Default constructor
- `Texture(std::string path)` - Constructor with file loading
- `~Texture()` - Destructor
- `load(std::string path)` → `bool` - Load bitmap from file
- `isValid()` → `bool` - Check if texture is loaded
- `setTransparentMask(COLORREF color)` - Set transparent color mask

#### `Drawable` (Abstract Base Class)
Base class for all drawable objects.

**Public Variables:**
- `int x, y, z` - Position coordinates
- `int width, height` - Dimensions
- `Vector2f scale` - Scale factors (default: {1,1})

**Public Methods:**
- `virtual void draw(HDC hdc) = 0` - Pure virtual draw method
- `virtual bool contains(POINT pos) = 0` - Pure virtual point containment check
- `virtual ~Drawable()` - Virtual destructor
- `setScale(Vector2f s)` - Set scaling factors
- `setOrigin(POINT pos)` - Set drawing origin point
- `getScaledWidth()` → `int` - Get width after scaling
- `getScaledHeight()` → `int` - Get height after scaling
- `getScaledOrigin()` → `POINT` - Get origin after scaling

#### `Sprite` (Inherits from `Drawable`)
Drawable sprite with texture support.

**Public Methods:**
- `Sprite()` - Constructor
- `bool contains(POINT pos)` override - Check if point is within sprite bounds
- `void draw(HDC hdc)` override - Draw sprite to device context
- `setTexture(Texture &texture)` - Set texture reference
- `setTextureRect(RECT r)` - Set texture sub-rectangle
- `getTextureRect()` → `RECT` - Get current texture rectangle
- `getTexture()` → `Texture&` - Get texture reference
- `getTexture() const` → `const Texture*` - Get const texture pointer
- `hasTexture()` → `bool` - Check if sprite has texture

#### `Shape` (Inherits from `Drawable`)
Simple colored rectangle shape.

**Public Variables:**
- `COLORREF color` - Fill color (default: RGB(125,255,255))

**Public Methods:**
- `Shape()` - Constructor (default size: 10x10)
- `bool contains(POINT pos)` override - Check if point is within shape bounds
- `void draw(HDC hdc)` override - Draw filled rectangle

#### `Window`
Main application window.

**Public Variables:**
- `HWND hwnd` - Native window handle
- `int x, y, width, height` - Window position and size
- `View view` - View management object
- `HBITMAP stretchBufferBitmap` - Intermediate buffer bitmap
- `HDC stretchBufferDC` - Intermediate buffer device context

**Public Methods:**
- `Window(int width, int height, std::string title)` - Constructor
- `~Window()` - Destructor
- `setView(View &v)` - Replace current view
- `setFullscreen(bool fullscreen)` - Toggle fullscreen mode
- `toggleFullscreen()` - Toggle fullscreen state
- `getFullscreen()` → `bool` - Get fullscreen state
- `isOpen()` → `bool` - Check if window is open (processes messages)
- `pollEvent(MSG &message)` → `bool` - Poll next window event
- `clear(COLORREF color = RGB(255,255,255))` - Clear back buffer with color
- `draw(Drawable &draw)` - Draw any Drawable object to back buffer
- `display()` - Display back buffer to screen

### Utility Functions

- `WIDE(std::string str)` → `std::wstring` - Convert UTF-8 string to wide string
- `TO_LPCSTR(std::string &str)` → `LPCSTR` - Convert std::string to LPCSTR