## Clipboard & Drag‑and‑Drop (winsimple_clipboard.hpp)

### Class ClipData
Container for data transferred via clipboard or drag‑and‑drop.

* `void setTexture(ws::Texture tex)` – stores an image.
* `void setText(std::string str)` – stores a text string.
* `void setFiles(std::vector<std::string> filesVec)` – stores a list of file paths.
* `ws::Texture getTexture() const` – returns the stored texture.
* `std::string getText() const` – returns the stored text.
* `const std::vector<std::string>& getFiles() const` – returns the stored file list.

### Helper Function
* `static bool CopyDIBToTexture(LPVOID pDIB, ws::Texture& tex)` – copies a DIB (device‑independent bitmap) from memory into a `ws::Texture`; returns success.

### Class Clipboard
Provides access to the system clipboard.

* `ClipData paste()` – retrieves all available data (text, image, files) from the clipboard.
* `bool copyText(const std::string& str)` – places a string on the clipboard; returns success.
* `bool copyFile(const std::string& filePath)` – places a single file path (as CF_HDROP) on the clipboard; returns success.
* `bool copyFiles(const std::vector<std::string>& filePaths)` – places multiple file paths on the clipboard; returns success.
* `bool copyTexture(ws::Texture &texture, ws::IntRect rect = {0,0,0,0})` – copies an image (optionally cropped) to the clipboard; returns success.
* `std::vector<std::string> pasteFiles()` – returns a list of file paths currently in the clipboard.
* `std::string pasteFile()` – returns the first file path from the clipboard, or an empty string.
* `bool hasFiles()` – checks whether the clipboard contains file data.
* `bool hasText()` – checks whether the clipboard contains text.
* `bool hasTexture()` – checks whether the clipboard contains an image.
* `bool clear()` – empties the clipboard; returns success.

A pre‑constructed global instance `ws::clipboard` is available.


### Example
```
//in loop
ws::ClipData data = ws::clipboard.paste();//retrieves any data that the user has pasted to this program.
ws::Texture copyTex = data.getTexture();
if(copyTex.isValid())
{
	ws::Sprite sprite(copyTex);
	window.draw(sprite);
}

```


### Class DropTarget
Implements `IDropTarget` for drag‑and‑drop support within a `ws::Window`.

* `void acceptType(std::string type, ws::DropEffect effect = ws::DropEffect::Copy)` – enables drop acceptance for `"files"`, `"text"`, or `"images"` with the specified effect.
* `void rejectType(std::string type)` – disables acceptance of a particular type.
* `void rejectAll()` – disables all drop types.
* `void onlyAcceptIf(std::string droptype, std::function<bool()> function)` – sets a conditional acceptance callback for a type; drop is allowed only when the function returns `true`.
* `bool pollDrop(ws::ClipData& outData, DropEffect& outEffect)` – checks if a drop event is queued; if so, fills `outData` and `outEffect` and returns `true`.
* `void setWindow(ws::Window &window)` – registers this drop target with a window.

### Enum class DropEffect
* `None`, `Copy`, `Move`, `Link` – matches standard `DROPEFFECT_*` values.


### Example
```
ws::DropTarget target;
target.setWindow(window);
target.acceptType("files");
target.rejectType("text");

//in window event loop
ws::ClipData data;
target.pollDrop(data,ws::DropEffect::Copy);

std::vector<std::string> files = data.getFiles();

for(auto& f : files)
	std::cout << f << std::endl;

```

