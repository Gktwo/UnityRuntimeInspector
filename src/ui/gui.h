#pragma once

class GUI
{
public:
	static GUI& getInstance();
    
	// Main render function called by backends
	void render();
    
	// Show/hide the GUI
	void setVisible(bool visible) { m_visible = visible; }
	bool isVisible() const { return m_visible; }
    
	// Demo functions
	void showDemoWindow();
	void showExampleWindow();
    
private:
	GUI() = default;
	~GUI() = default;
    
	GUI(const GUI&) = delete;
	GUI& operator=(const GUI&) = delete;
    
	bool m_visible = true;
	bool m_showDemo = false;
	bool m_showExample = true;
    
	void renderMainMenuBar();
	void renderExampleWindow();
};
