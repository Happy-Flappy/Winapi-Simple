#include "winsimple.h"









int main()
{
	
	ws::Window window(960,540,"");
	
	
	
	ws::Menu menu;    
    menu.setWindow(window);
    
	
	ws::Dropdown fileMenu(1000, "File");
    
    fileMenu.addItem(1001,MF_STRING, "Save");
    fileMenu.addItem(1002,MF_STRING, "Open");
    fileMenu.addItem(0   ,MF_SEPARATOR, "");
    fileMenu.addItem(1004,MF_STRING, "Exit");

  
    menu.addDropdown(fileMenu);
    
	//WINAPI method
    
//    // 2. Create menu bar with ONE dropdown
//    HMENU bar = CreateMenu();
//    HMENU hFileMenu = CreatePopupMenu();
//    
//    // 3. Add 3 items to the File dropdown
//    AppendMenuA(hFileMenu, MF_STRING, 1001, "New");
//    AppendMenuA(hFileMenu, MF_STRING, 1002, "Open");
//    AppendMenuA(hFileMenu, MF_SEPARATOR, 0, NULL);  // Optional separator
//    AppendMenuA(hFileMenu, MF_STRING, 1003, "Exit");
//    
//    // 4. Add the dropdown to the menu bar
//    AppendMenuA(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, "File");
//    
//    // 5. Attach menu bar to window
//    SetMenu(window.hwnd, hMenuBar);	
//	
	
	
	
	
	
	
	ws::ComboBox box;
	box.init(window);
	box.setPosition(50,100);
	box.addItem("Item 1");
	box.addItem("Item 2");
	box.addItem("Item 3");
	box.addItem("Item 4");
	box.addItem("Item 5");
	
	
	
	
	
	
	
	
	ws::Texture backTex;
	backTex.loadFromFile("title.png");
	
	ws::Sprite back;
	back.setTexture(backTex);
	back.setScale(0.25,0.25);
	
	
	
	ws::Font font;
	font.loadFromSystem("Comic Sans MS");
	
	ws::Text text;
	text.setFont(font);
	
	std::string str = "";
	text.setString(str);

	text.setBorderColor(Gdiplus::Color(255,100,200,100));
	text.setBorderWidth(2);
	text.setStyle(Gdiplus::FontStyleBoldItalic);
	text.setCharacterSize(30);
	

	text.setPosition(30,400);	
	
	
	
	ws::FileWindow fileWindow;
	fileWindow.setTitle("My Open/Save Dialog");
	fileWindow.setFileName("DefaultOpenFile.txt");
	
	ws::GIF gif;
	
	
	ws::ClickMenu clickMenu;
	clickMenu.init(window);
	clickMenu.addItem("Save");
	clickMenu.addItem("Open");
	clickMenu.addItem("Open Folder");
	clickMenu.addItem("Add to windows File Clipboard");
	
	
	ws::FolderWindow folderWin;
	folderWin.setTitle("Folder Window");
	std::string folderName = "";
	
	
	
	bool load = false;
	
	while(window.isOpen())
	{
		
		if(ws::Global::getKey(VK_ESCAPE))
			window.close();
		
		MSG m;
		while(window.pollEvent(m))
		{
			if(m.message == WM_RBUTTONDOWN)
			{
				int x = GET_X_LPARAM(m.lParam);
				int y = GET_Y_LPARAM(m.lParam);
				
				
				clickMenu.show(ws::Vec2i(x,y));
				int c = clickMenu.getCommand();
				
				if(c == 1)
				{
					//Save
					fileWindow.save(&window);
					if(fileWindow.getFileName() != "")
						window.backBuffer.saveToFile(fileWindow.getFileName());				
				}
				if(c == 2)
				{
					load = true;
				}
				
				if(c == 3)
				{
					folderWin.open(&window);
					
					std::string t = folderWin.getFolderName();
					folderName = t;
				}
					
				if(c == 4)
				{
					fileWindow.open(&window);
					std::string str = fileWindow.getFileName();
					ws::clipboard.copyFile(str);
				}		
						
		
			}
			
			
			switch(menu.getEvent(m))
			{
				case 1001:
				
					//Save
					fileWindow.save(&window);
					if(fileWindow.getFileName() != "")
						window.backBuffer.saveToFile(fileWindow.getFileName());	
					break;
					
				case 1002:
				
					load = true;
					break;
				
				case 1004:
					window.close();
					break;
			}
			
		}
		
		
		
		
		if(load)
		{
			//Load
			fileWindow.open(&window);
			std::string str = fileWindow.getFileName();
			
			
			if(str != "")
			{
			
				std::filesystem::path file(str);
				if(file.extension() == ".gif")
				{
					gif.loadFromFile(str);
					gif.play();
					gif.setLoop();
					back.setTexture(gif.getTexture());
				}
				else
				{
					gif.stop();
					backTex.loadFromFile(str);
					back.setTexture(backTex);
				}
			}
			load = false;
		}
		
		
		
		
		
		
		if(gif.getStatus() == "playing")
			gif.update();
		
		back.setScale(1,1);	
		while(back.getVisualWidth() > 960 || back.getVisualHeight() > 540)	
		{
			back.setScale(back.getScale().x - 0.001,back.getScale().y - 0.001);
		}	
		
		text.setString("		Welcome to the ScreenShot Demo!\nRight click to see a menu to choose from!\nSelected Folder:" + folderName);
			
		window.clear();
		window.draw(back);
		window.draw(text);
		window.display();
	}
	return 0;
}