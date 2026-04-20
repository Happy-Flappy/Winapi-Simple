#include "winsimple.hpp"
#include "winsimple-controls.hpp"


int main()
{
	ws::Window window(320,200,"");
	
    ws::ComboBox combo;
    combo.setPosition(50, 50);
    combo.setSize(200,200);
    combo.addItem("Apple");
    combo.addItem("Banana");
    combo.addItem("Cherry");
    combo.addItem("Date");
    combo.setSelectedIndex(0); 
	
	combo.setDropdownStyle(true);
	
	window.addChild(combo);
	


	ws::ListBox list;
    list.addItem("Apple");
    list.addItem("Banana");
    list.addItem("Cherry");
    list.addItem("Date");	
	
	
	window.addChild(list);
	
	
	while(window.isOpen())
	{
        window.clear();

        MSG msg;
        while(window.pollEvent(msg))
        {
            if(combo.selectionChanged(msg))
            {
                std::string selected = combo.getSelectedText();
                std::string msgText = "You selected: " + selected;
                MessageBoxA(window.hwnd, msgText.c_str(), "Selection Changed", MB_OK | MB_ICONINFORMATION);
            }
        }


		window.display();
	}
	
}