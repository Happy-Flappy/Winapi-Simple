#include "winsimple.h"




int main()
{
	
	ws::Window window(960,540,"");

	ws::Button buttonSmaller;
	buttonSmaller.init(window);
	buttonSmaller.setText("Smaller");
	buttonSmaller.setSize(100,100);
	buttonSmaller.setFillColor(RGB(255,0,0));
	
	ws::Button buttonLarger;
	buttonLarger.init(window);
	buttonLarger.setText("Larger");
	buttonLarger.setSize(100,100);
	buttonLarger.setPosition(960-100,0);
	
	
	ws::Slider slider;
	slider.init(window);
	slider.setRange(0,255);
	slider.setPosition(0,150);
	slider.setSlidePosition(50);
	slider.setVertical();
	
	
	
	ws::Slider slider2;
	slider2.init(window);
	slider2.setRange(0,255);
	slider2.setSize(960,20);
	slider2.setHorizontal();
	slider2.setPosition(0,540 - 20);
	slider2.setSlidePosition(255);




	ws::TextBox textbox;
	textbox.setPosition(960 - 200, 540 - 100 - 20);
	textbox.setSize(200,100);
	textbox.init(window);




	ws::Label label;
	label.setPosition(textbox.getPosition().x - textbox.getSize().x, textbox.getPosition().y);
	label.setSize(textbox.getSize());
	label.init(window);
	label.setText("Retype on right. Right into a fabel.\n Pressing down Enter. Will set the new label. 8P");

	
	
	ws::Radial radial;
	radial.setRadius(100);
	radial.setPosition(300,300);
	radial.setFillColor({255,255,160,0});
	radial.setBorderColor({255,200,130,0});
	
	
	


	
	float radius = 100;



	while(window.isOpen())
	{

		bool keyShift = (ws::Global::getKey(VK_SHIFT));			
		
		
		MSG msg;
		

		while(window.pollEvent(msg))
		{

			buttonSmaller.update(&msg); //Updates size and settings
			buttonLarger.update(&msg);
			slider.update(&msg);
			slider2.update(&msg);
			textbox.update(&msg);
			label.update(&msg);

			
			if(buttonSmaller.isPressed(msg))
			{
				radius --;
				radial.setRadius(radius);
			}
			
			if(buttonLarger.isPressed(msg))
			{
				radius ++;
				radial.setRadius(radius);
			}
			
			
			if(slider.getScroll(msg))
			{
				std::cerr << slider.getSlidePosition() << "\n";
				radial.setFillColor(Gdiplus::Color(slider.getSlidePosition(),slider2.getSlidePosition(),160,0));
			}
			
			if(slider2.getScroll(msg))
			{
				std::cerr << "FillColor: "<<slider2.getSlidePosition() << "\n";
				radial.setFillColor(Gdiplus::Color(slider.getSlidePosition(),slider2.getSlidePosition(),160,0));
			}
			
			if(msg.message == WM_KEYDOWN)
			{
				if(msg.wParam == VK_RETURN)
				{
					if(!keyShift)
					{
						label.setText(textbox.getText());
						std::cerr << "textbox message:"<< textbox.getText() << "\n";
						textbox.setText("");
					}
				}
			}
			
		}
		
		
		
		
		window.clear();
		window.draw(radial);
		window.display();
	}
	
	return 0;
}