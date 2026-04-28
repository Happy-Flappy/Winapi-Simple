#include "winsimple.hpp"
#include "winsimple-controls.hpp"
#include <iostream>

int main()
{
    ws::Window window(800, 600, "Multi‑Page Controls Demo");
    if (!window.isOpen()) return -1;

    // Tab control – set size and position first
    ws::Tabs tabs;
    tabs.setPosition(10, 10);
    tabs.setSize(780, 550);
    
    // ========== PAGE 1 ==========
    // Use a Label as the page container (it has a background)
    ws::Label page1;
    page1.setText("");                     // no text
    page1.setPosition(0, 0);               // will be resized by tabs
    page1.setSize(100, 100);
    // Give it a light gray background so we can see the page area
    // (Note: ws::Label doesn't have setBackgroundColor; we'll use a workaround)
    // Instead, we'll add a large rectangle or rely on child controls.
    // For visibility, add a colored label that fills the page later.
    
    ws::TextBox textBox;
    textBox.setPosition(20, 20);
    textBox.setSize(300, 25);
    textBox.setText("Type here...");
    page1.addChild(textBox);
    
    ws::Button button1;
    button1.setPosition(340, 20);
    button1.setSize(100, 25);
    button1.setText("Show Text");
    page1.addChild(button1);
    
    ws::Label label1;
    label1.setPosition(20, 60);
    label1.setSize(300, 25);
    label1.setText("Label: ");
    page1.addChild(label1);
    
    // Add a full‑size colored rectangle behind everything to verify page position
    ws::Label debugBg;
    debugBg.setText("");
    debugBg.setPosition(0, 0);
    debugBg.setSize(780, 550);
    debugBg.addStyle(SS_WHITERECT);       // white rectangle background
    page1.addChild(debugBg);
    // Move debugBg to the back (send to bottom of Z-order)
    // (Not strictly necessary, but helps see other controls)
    
    // Initialize the page before adding to tabs
    page1.init(window);
    
    // ========== PAGE 2 ==========
    ws::Label page2;
    page2.init(window);
    
    ws::ComboBox combo;
    combo.setPosition(20, 20);
    combo.setSize(200, 100);
    combo.addItems({"Option 1", "Option 2", "Option 3"});
    combo.setSelectedIndex(0);
    page2.addChild(combo);
    
    ws::ListBox listBox;
    listBox.setPosition(20, 60);
    listBox.setSize(200, 150);
    listBox.addItems({"Item A", "Item B", "Item C"});
    page2.addChild(listBox);
    
    ws::Label label2;
    label2.setPosition(240, 20);
    label2.setSize(200, 25);
    label2.setText("Selection: ");
    page2.addChild(label2);
    
    // ========== PAGE 3 ==========
    ws::Label page3;
    page3.init(window);
    
    ws::Slider slider;
    slider.setPosition(20, 20);
    slider.setSize(300, 30);
    slider.setRange(0, 100);
    slider.setSlidePosition(50);
    page3.addChild(slider);
    
    ws::Label sliderLabel;
    sliderLabel.setPosition(20, 60);
    sliderLabel.setSize(200, 25);
    sliderLabel.setText("Slider value: 50");
    page3.addChild(sliderLabel);
    
    ws::Button resetButton;
    resetButton.setPosition(20, 100);
    resetButton.setSize(120, 25);
    resetButton.setText("Reset Slider");
    page3.addChild(resetButton);
    
    // Add pages to tabs (they are already initialized)
    tabs.addPage("Page 1", page1);
    tabs.addPage("Page 2", page2);
    tabs.addPage("Page 3", page3);
    
    // Now add the tabs to the window – this calls tabs.init() and processes pending pages
    window.addChild(tabs);
    
    // Optional: force a redraw of the tab control
    InvalidateRect(tabs.hwnd, NULL, TRUE);
    UpdateWindow(tabs.hwnd);
    
    // Set a nicer font for all controls
    ws::Font defaultFont;
    defaultFont.loadFromSystem("Segoe UI");
    ws::Text fontSettings;
    fontSettings.setCharacterSize(14);
    
    textBox.setFont(defaultFont, fontSettings);
    button1.setFont(defaultFont, fontSettings);
    label1.setFont(defaultFont, fontSettings);
    combo.setFont(defaultFont, fontSettings);
    listBox.setFont(defaultFont, fontSettings);
    label2.setFont(defaultFont, fontSettings);
    slider.setFont(defaultFont, fontSettings);
    sliderLabel.setFont(defaultFont, fontSettings);
    resetButton.setFont(defaultFont, fontSettings);
    
    // Main message loop
    MSG msg;
    while (window.isOpen())
    {
        while (window.pollEvent(msg))
        {
            if (msg.message == WM_COMMAND)
            {
                if (button1.isPressed(msg))
                {
                    std::string userText = textBox.getText();
                    label1.setText("Label: " + userText);
                }
                if (combo.selectionChanged(msg))
                {
                    label2.setText("Selection: " + combo.getSelectedText());
                }
                if (listBox.selectionChanged(msg))
                {
                    label2.setText("Selection: " + listBox.getSelectedText());
                }
                if (resetButton.isPressed(msg))
                {
                    slider.setSlidePosition(50);
                    sliderLabel.setText("Slider value: 50");
                }
            }
            if (msg.message == WM_HSCROLL || msg.message == WM_VSCROLL)
            {
                if (slider.getScroll(msg))
                {
                    int val = slider.getSlidePosition();
                    sliderLabel.setText("Slider value: " + std::to_string(val));
                }
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // No custom drawing needed – controls draw themselves
    }
    return 0;
}