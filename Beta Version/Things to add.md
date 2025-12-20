
# Things to add...


## To ws::Texture

```

#include <shlwapi.h>
#include <atlbase.h>



#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "shlwapi.lib")



bool loadFromMemory(const BYTE* buffer,size_t bufferSize)
{
	//create an Istream object from the memory buffer.
	
	CComPtr<IStream> stream;
	
	// SHCreateMemStream creates a stream backed by a global memory handle
	stream.Attach(SHCreateMemStream(static_cast<const BYTE*>(buffer), static_cast<UINT>(bufferSize)));
	
	
	bitmap = Gdiplus::Bitmap::FromStream(stream);

	if(bitmap->GetLastStatus() != Gdiplus::Ok)
	{
		delete bitmap;
		bitmap = nullptr;
		return false;
	}

	return true;
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

- Some kind of transparency method that will work. Chromakey most likely.

## To all things

#### Alternative parameters for ws::Vec2i as x,y

#### Replace raw variables with get and set functions.(for consistency)





