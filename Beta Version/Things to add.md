
# Things to add...


## To ws::Texture

```

#include <shlwapi.h>
#include <atlbase.h>



#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "shlwapi.lib")



void loadFromMemory(const BYTE* buffer,size_t bufferSize)
{
	//create an Istream object from the memory buffer.
	
	CComPtr<IStream> stream;
	
	// SHCreateMemStream creates a stream backed by a global memory handle
	stream.Attach(SHCreateMemStream(static_cast<const BYTE*>(buffer), static_cast<UINT>(bufferSize)));
	
	
	
}

```


```

void setPixel(int index,Gdiplus::Color color)
{
	int x = index % img->width;
        int y = index / img->width;
	setPixel(x,y,color);
}
```

```
ws::Vec2i getSize()
{
	return ws::Vec2i(width,height); 
}
```





## to ws::Window

- Close() = a call to a destructor.


- setVisible(bool vis)

- getVisible()


- Ability to construct window without definition.

- create() function.



#### If the clear color is less than opaque, set the window mode to layered and use alpha.

#### Likewise, if window is not transparent at all, undo that setting. 


#### OR


#### Have window always be layered. 

#### AND

#### provide legacy transparency support via alphaBlend.







## To all things

#### Alternative parameters for ws::Vec2i as x,y

#### Replace raw variables with get and set functions.(for consistency)





