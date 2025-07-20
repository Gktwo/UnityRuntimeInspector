#pragma once

// Forward declaration
class UnityExplorer;

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
    void showExampleWindow();

    // Unity Explorer functions
    void showUnityExplorer();
    void toggleUnityExplorer() { m_showUnityExplorer = !m_showUnityExplorer; }
    bool isUnityExplorerVisible() const { return m_showUnityExplorer; }

    // Language testing function
    void testLanguageSystem();

private:
    GUI();
    ~GUI();

    GUI(const GUI&) = delete;
    GUI& operator=(const GUI&) = delete;

    // Visibility flags
    bool m_visible = true;
    bool m_showExample = true;
    bool m_showUnityExplorer = false;
    bool m_showAboutModal = false;
    bool m_showControlsModal = false;

    // Unity Explorer
    std::unique_ptr<UnityExplorer> m_unityExplorer;
    bool m_unityExplorerInitialized = false;


    // Rendering methods
    void renderMainMenuBar();
    void renderExampleWindow();
    void renderAboutModal();

    // Unity Explorer management
    bool initializeUnityExplorer();
    void shutdownUnityExplorer();

    // Utility
    void setupImGuiStyle();
    void helpMarker(const char* desc);
};
