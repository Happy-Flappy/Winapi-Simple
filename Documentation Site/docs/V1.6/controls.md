## Controls (winsimple_controls.hpp)

### Class ControlsInit (internal)
Initialises common controls and COM for controls. Automatically created as a global static.

### Base Class Child
All controls inherit from `Child`. Wraps a Win32 child window.

* `Child(const std::wstring& className = L"Button")` – constructor; sets the window class name.
* `virtual ~Child()` – destroys the control window and frees the font.
* `virtual bool init(ws::Window& parent)` – creates the child window inside the given `ws::Window`; returns success.
* `virtual bool init(HWND phwnd)` – creates the child window with a given parent HWND.
* `void registerDestructorCallback(DestructorCallback callback)` – registers a callback to be invoked when the child is destroyed.
* `void clearDestructorCallbacks()` – removes all registered destructor callbacks.
* `void addChild(ws::Child& child)` – embeds another child control as a child of this control.
* `void removeChild(ws::Child &child)` – removes a direct child control.
* `bool hasChild(ws::Child &child)` – returns true if the given child is directly owned by this control.
* `void setClass(const std::wstring& className = L"Button")` – changes the Win32 class name; recreates the window if already created.
* `std::string getClass()` – returns the current Win32 class name.
* `void setVisible(bool visible)` – shows or hides the control.
* `void setPosition(int xPos, int yPos)` – moves the control.
* `void setPosition(ws::Vec2i pos)` – moves the control.
* `ws::Vec2i getPosition() const` – returns the control’s position.
* `virtual void setSize(int w, int h)` – resizes the control.
* `virtual void setSize(ws::Vec2i size)` – resizes the control.
* `ws::Vec2i getSize() const` – returns the control’s size.
* `HFONT getFontHandle()` – returns the custom font handle, if any.
* `void addStyle(DWORD addedStyle)` – adds a window style.
* `void removeStyle(DWORD removedStyle)` – removes a window style.
* `bool hasStyle(DWORD checkStyle) const` – checks if a style is set.
* `void addExStyle(DWORD addedStyle)` – adds an extended window style.
* `void removeExStyle(DWORD removedStyle)` – removes an extended window style.
* `bool hasExStyle(DWORD checkStyle) const` – checks if an extended style is set.
* `void setText(const std::string& newText)` – sets the control’s text.
* `std::string getText() const` – returns the control’s text.
* `void setFont(ws::Font& font, ws::Text& textSettings)` – applies a `ws::Font` with size and style from a `ws::Text` object.
* `bool contains(ws::Vec2i point) const` – checks if a point (relative to parent) is inside the control.
* `virtual bool handleCommand(MSG &msg)` – override to handle `WM_COMMAND`; returns `true` if handled.
* `virtual bool handleNotify(NMHDR* pnmh)` – override to handle `WM_NOTIFY`; returns `true` if handled.

### Class Tabs : public Child
A tab control (`SysTabControl32`) that hosts a set of child controls as pages.

* `Tabs()` – constructor; sets tab styles.
* `virtual bool init(ws::Window& parent) override` – initialises the tab control and processes pending pages.
* `void addPage(const std::string& title, ws::Child& page)` – adds a tab page with the given title and child control.
* `void removePage(ws::Child &child)` – removes a page child.
* `bool hasPage(ws::Child &child)` – returns `true` if the child is a registered page.
* `void setSelected(int index)` – selects the page at the given index.
* `int getSelected() const` – returns the index of the currently selected tab.
* `virtual void setSize(int w, int h) override` – resizes the tab control and repositions page contents.
* `virtual bool handleNotify(NMHDR* pnmh) override` – handles `TCN_SELCHANGE` to switch visible pages.

### Class ComboBox : public Child
A drop‑down combo box.

* `ComboBox()` – constructor; sets `CBS_DROPDOWN` and `WS_VSCROLL` styles.
* `virtual bool init(ws::Window &parent) override` – initialises the combo box and adds any pending items.
* `void addItem(const std::string& item)` – adds a string item.
* `void addItems(const std::vector<std::string>& items)` – adds multiple items.
* `void removeItem(int index)` – removes the item at the given index.
* `void clear()` – removes all items.
* `int getSelectedIndex()` – returns the index of the currently selected item.
* `void setSelectedIndex(int index)` – selects the item at the given index.
* `std::string getSelectedText()` – returns the text of the currently selected item.
* `int getItemCount()` – returns the total number of items.
* `std::string getItemText(int index)` – returns the text of the item at the given index.
* `bool selectionChanged(MSG &msg)` – returns `true` if `msg` indicates a selection change in this combo box.
* `void setDropdownStyle(bool allowEdit = true)` – switches between editable (`CBS_DROPDOWN`) and read‑only (`CBS_DROPDOWNLIST`) styles.
* `std::string getEditText()` – returns the text currently typed in the edit field (editable style only).
* `void setEditText(const std::string& text)` – sets the text in the edit field.

### Class Button : public Child
A standard push button.

* `Button()` – constructor; sets class to `"Button"`.
* `virtual bool init(ws::Window &parent) override` – initialises the button and sets a default font if needed.
* `bool isPressed(MSG &msg)` – returns `true` if `msg` indicates a `BN_CLICKED` for this button.

### Class Slider : public Child
A trackbar control (slider).

* `Slider()` – constructor; sets horizontal style and default range 0‑100.
* `virtual bool init(ws::Window &parent) override` – initialises the slider and restores pending range/position.
* `bool getScroll(MSG &msg)` – returns `true` if `msg` indicates the slider was moved; updates the stored position.
* `void setHorizontal()` – sets the slider to horizontal orientation.
* `void setVertical()` – sets the slider to vertical orientation.
* `void setRange(int minimum = 0, int maximum = 100)` – sets the minimum and maximum values.
* `void setSlidePosition(int pos = 0)` – sets the current slider position.
* `int getSlidePosition()` – returns the current position.

### Class TextBox : public Child
A multiline edit control.

* `TextBox()` – constructor; sets `ES_MULTILINE` and associated styles.
* `virtual bool init(ws::Window &parent) override` – initialises the text box, sets a default font and character limit.
* `void setCharacterLimit(int max_chars = 0)` – limits the maximum number of characters (0 = unlimited).
* `bool getFocus()` – returns `true` if the text box currently has keyboard focus.

### Class Label : public Child
A static text label.

* `Label()` – constructor; sets `SS_NOTIFY` and `SS_LEFT` styles.
* `virtual bool init(ws::Window &parent) override` – initialises the label and sets a default font.

### Class ColorDialog
Opens the standard Windows colour picker dialog.

* `void init(ws::Window &newParent)` – sets the parent window.
* `ws::Window* getParent()` – returns the parent window pointer.
* `void addFlag(DWORD newFlag)` – adds a `CHOOSECOLOR` flag.
* `void removeFlag(DWORD removeFlag)` – removes a flag.
* `DWORD getFlags()` – returns the current flags.
* `void setInitColor(ws::Hue hue)` – sets the colour initially shown.
* `ws::Hue getInitColor()` – returns the initial colour.
* `ws::Hue getResult()` – returns the colour chosen by the user.
* `bool open()` – opens the dialog; returns `true` if a colour was selected.

### Class Dropdown
Represents a popup menu item that can hold sub‑items or sub‑menus.

* `Dropdown(int newID, std::string newName)` – creates a popup menu (unless `newID == 0` for leaf items).
* `void addItem(int id, DWORD type, std::string itemName)` – adds a menu item with a given ID and type (e.g. `MF_STRING`).
* `void addSubmenu(Dropdown &submenu)` – attaches another `Dropdown` as a sub‑menu.
* `void addItem(Dropdown drop)` – adds another `Dropdown` as a string menu item (using its name and ID).
* `HMENU getHandle()` – returns the underlying `HMENU`.
* `std::string getName()` – returns the dropdown name.
* `int getID()` – returns the menu ID.

### Class Menu
A menu bar that can be attached to a `ws::Window`.

* `Menu()` – creates an empty menu bar.
* `void addDropdown(ws::Dropdown &drop)` – adds a dropdown menu to the bar.
* `void setWindow(ws::Window &window)` – attaches the menu bar to a window.
* `int getEvent(MSG &m)` – extracts the command ID from a `WM_COMMAND` message; returns -1 if not applicable.

### Class ClickMenu
A right‑click context menu.

* `void addFlag(DWORD newFlag)` – adds a track‑popup‑menu flag.
* `void removeFlag(DWORD removeFlag)` – removes a flag.
* `DWORD getFlags()` – returns the current flags.
* `int getResult()` – returns and resets the last selected command ID.
* `std::vector<std::string> getList()` – returns the current list of menu item strings.
* `void setList(std::vector<std::string> newList)` – sets the menu items.
* `void addItem(std::string item)` – adds one menu item.
* `void removeItem(std::string item)` – removes a menu item by string.
* `void init(ws::Window &newParent)` – sets the parent window.
* `ws::Window* getParent()` – returns the parent window.
* `bool open(ws::Vec2i mouse)` – opens the context menu at the given client coordinates; returns `true` if an item was selected.

### Class FileWindow
Wraps `GetOpenFileNameW` and `GetSaveFileNameW` for file selection dialogs.

* `FileWindow()` – constructor.
* `void setInitResult(std::string file)` – sets the initial filename.
* `std::string getResult()` – returns the selected filename.
* `void setTitle(std::string name)` – sets the dialog title.
* `std::string getTitle()` – returns the dialog title.
* `void addFlag(DWORD newFlag)` – adds an `OPENFILENAME` flag.
* `void removeFlag(DWORD removeFlag)` – removes a flag.
* `DWORD getFlags()` – returns the current flags.
* `bool open(ws::Window *parent = nullptr)` – opens the file **open** dialog; returns `true` if a file was chosen.
* `bool save(ws::Window *parent = nullptr)` – opens the file **save** dialog; returns `true` if a path was chosen.

### Class FolderWindow
Wraps `SHBrowseForFolderW` for folder selection.

* `FolderWindow()` – constructor.
* `void setTitle(std::string name)` – sets the dialog title.
* `std::string getTitle()` – returns the title.
* `void addFlag(DWORD flag)` – adds a `BROWSEINFO` flag.
* `void setFlags(DWORD allFlags)` – replaces all flags.
* `void removeFlag(DWORD flag)` – removes a flag.
* `DWORD getFlags()` – returns the current flags.
* `std::string getResult()` – returns the selected folder path.
* `bool open(ws::Window *parent = nullptr)` – opens the folder selection dialog; returns `true` if a folder was chosen.
* `void setInitResult(std::string folder)` – sets the initial folder displayed.

### Class ExploreWindow
Uses the modern `IFileDialog` COM interface for file/folder dialogs.

* `ExploreWindow()` – creates an `IFileOpenDialog` instance.
* `void addStyle(DWORD style)` – adds a `FOS_*` option.
* `void removeStyle(DWORD style)` – removes an option.
* `void setTitle(std::string title)` – sets the dialog title.
* `void setInitResult(std::string folder)` – sets the initial folder or result.
* `std::string getResult()` – returns the selected path.
* `bool open(ws::Window *parent = nullptr)` – shows the dialog; returns `true` if a selection was made.

### Class ListBox : public Child
A standard list box.

* `ListBox()` – constructor; sets `LBS_STANDARD` and notification styles.
* `virtual bool init(ws::Window &parent) override` – initialises the list box and adds pending items.
* `void addItem(const std::string& item)` – adds a string item.
* `void addItems(const std::vector<std::string>& items)` – adds multiple items.
* `void removeItem(int index)` – removes the item at the given index.
* `void clear()` – removes all items.
* `int getSelectedIndex()` – returns the index of the currently selected item.
* `std::string getSelectedText()` – returns the text of the selected item.
* `std::string getItemText(int index)` – returns the text of the item at the given index.
* `void setSelectedIndex(int index)` – selects an item by index.
* `bool selectionChanged(MSG &msg)` – returns `true` if `msg` indicates `LBN_SELCHANGE` for this list box.
* `int getItemCount()` – returns the total number of items.