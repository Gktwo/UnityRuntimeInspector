#include "pch.h"
#include "gui.h"
#include "language.h"

#include "inspector/unity_explorer.h"

GUI::GUI()
{
    setupImGuiStyle();
    
    // Initialize language system
    if (!Language::getInstance().initialize())
    {
        LOG_WARNING("[GUI] Failed to initialize language system, using fallback text");
    }
    else
    {
        LOG_INFO("[GUI] Language system initialized successfully");
        LOG_INFO("[GUI] Current language: %s", Language::getInstance().getLanguageName(Language::getInstance().getCurrentLanguage()));
    }
}

GUI::~GUI()
{
    shutdownUnityExplorer();
}

GUI& GUI::getInstance()
{
    static GUI instance;
    return instance;
}

void GUI::render()
{
    if (!m_visible)
    {
        return;
    }

    // Initialize Unity Explorer on first render if needed
    if (!m_unityExplorerInitialized && m_showUnityExplorer)
    {
        initializeUnityExplorer();
    }

    // Render main menu bar
    renderMainMenuBar();

    // Render example window if enabled
    if (m_showExample)
    {
        renderExampleWindow();
    }

    // Render Unity Explorer if enabled and initialized
    if (m_showUnityExplorer && m_unityExplorer && m_unityExplorerInitialized)
    {
        m_unityExplorer->update();
    }

    // Render about modal
    renderAboutModal();
}

void GUI::showExampleWindow()
{
    m_showExample = true;
}

void GUI::showUnityExplorer()
{
    m_showUnityExplorer = true;
    if (!m_unityExplorerInitialized)
    {
        initializeUnityExplorer();
    }
}

void GUI::renderMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        // File menu
        if (ImGui::BeginMenu(MENU_FILE))
        {
            if (ImGui::MenuItem(LANG("Exit"), "Alt+F4"))
            {
                // TODO: Implement exit functionality
            }
            ImGui::EndMenu();
        }

        // Windows menu
        if (ImGui::BeginMenu(MENU_WINDOWS))
        {
            ImGui::MenuItem(LANG("Example Window"), nullptr, &m_showExample);
            ImGui::Separator();

            if (ImGui::MenuItem(LANG("Unity Explorer"), "F1", &m_showUnityExplorer))
            {
                if (m_showUnityExplorer && !m_unityExplorerInitialized)
                {
                    initializeUnityExplorer();
                }
            }

            ImGui::EndMenu();
        }

        // Tools menu
        if (ImGui::BeginMenu(MENU_TOOLS))
        {
            if (ImGui::MenuItem(LANG("Refresh Unity Explorer"), "F5"))
            {
                if (m_unityExplorer && m_unityExplorerInitialized)
                {
                    // Force refresh by reinitializing
                    shutdownUnityExplorer();
                    initializeUnityExplorer();
                }
            }

            ImGui::Separator();

            //if (ImGui::MenuItem(LANG("Test Language System")))
            //{
            //    testLanguageSystem();
            //}

            ImGui::Separator();

            if (ImGui::BeginMenu(LANG("Theme")))
            {
                if (ImGui::MenuItem(LANG("Dark (Default)")))
                {
                    ImGui::StyleColorsDark();
                    setupImGuiStyle();
                    LOG_INFO("[GUI] Switched to Dark theme");
                }
                if (ImGui::MenuItem(LANG("Light")))
                {
                    ImGui::StyleColorsLight();
                    // Don't call setupImGuiStyle() for light theme to preserve ImGui's light colors
                    LOG_INFO("[GUI] Switched to Light theme");
                }
                if (ImGui::MenuItem(LANG("Classic")))
                {
                    ImGui::StyleColorsClassic();
                    // Don't call setupImGuiStyle() for classic theme to preserve ImGui's classic colors
                    LOG_INFO("[GUI] Switched to Classic theme");
                }
                
                // Language submenu
                ImGui::Separator();
                 if (ImGui::BeginMenu(LANG("Language")))
                {
                    // Get current language for comparison
                    LanguageType currentLang = Language::getInstance().getCurrentLanguage();
                    
                    // Create radio button style language selection
                    if (ImGui::MenuItem(LANG("English"), nullptr, currentLang == LanguageType::English))
                    {
                        if (currentLang != LanguageType::English)
                        {
                            LOG_INFO("[GUI] Switching to English");
                            SWITCH_LANG(LanguageType::English);
                        }
                    }
                    if (ImGui::MenuItem(LANG("Chinese"), nullptr, currentLang == LanguageType::Chinese))
                    {
                        if (currentLang != LanguageType::Chinese)
                        {
                            LOG_INFO("[GUI] Switching to Chinese");
                            SWITCH_LANG(LanguageType::Chinese);
                        }
                    }
                    
                    ImGui::EndMenu();
                }
                
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        // Help menu
        if (ImGui::BeginMenu(MENU_HELP))
        {
            if (ImGui::MenuItem(LANG("About Unity Runtime Inspector")))
            {
                ImGui::OpenPopup(LANG("About Unity Runtime Inspector"));
            }
            ImGui::Separator();
            if (ImGui::MenuItem(LANG("Controls Reference")))
            {
                ImGui::OpenPopup(LANG("Controls Reference"));
            }
            ImGui::EndMenu();
        }

        // Status section with better visual indicators
        ImGui::Separator();
        
        // Unity Explorer status with colored indicators
        ImGui::Text("%s:", LANG("Unity Explorer"));
        ImGui::SameLine();
        
        if (m_unityExplorer && m_unityExplorerInitialized && m_showUnityExplorer)
        {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), STATUS_ACTIVE);
        }
        else if (m_showUnityExplorer)
        {
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), STATUS_INITIALIZING);
        }
        else
        {
            ImGui::TextDisabled(STATUS_INACTIVE);
        }

        // Unity backend status
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();
        
        auto unityModule = GetModuleHandleA("GameAssembly.dll");
        if (!unityModule)
        {
            unityModule = GetModuleHandleA("UnityPlayer.dll");
        }

        ImGui::Text("%s:", LANG("Unity"));
        ImGui::SameLine();
        
        if (unityModule)
        {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), STATUS_CONNECTED);
        }
        else
        {
            ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), STATUS_DISCONNECTED);
        }

        // Performance info
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();
        
        float fps = ImGui::GetIO().Framerate;
        ImGui::Text("%s: %.1f", LANG("FPS"), fps);
        
        // Color code FPS
        if (fps >= 60.0f)
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), LANG("●"));
        }
        else if (fps >= 30.0f)
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), LANG("●"));
        }
        else
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), LANG("●"));
        }

        // Controls hint
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();
        ImGui::TextDisabled("%s: %s | F1: %s | F5: %s", 
                           LANG("INSERT"), LANG("Toggle GUI"), LANG("Unity Explorer"), LANG("Refresh"));

        ImGui::EndMainMenuBar();
    }
}

void GUI::renderExampleWindow()
{
    static bool p_open = true;

    if (!ImGui::Begin(LANG("Unity Runtime Inspector - Dashboard"), &p_open, ImGuiWindowFlags_None))
    {
        ImGui::End();
        return;
    }

    // Header section
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Use default font for header
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), LANG("Unity Runtime Inspector"));
    ImGui::PopFont();
    ImGui::TextDisabled(LANG("Real-time Unity game inspection and debugging tool"));
    ImGui::Separator();

    // Performance section
    ImGui::Text(LBL_PERFORMANCE);
    ImGui::SameLine();
    helpMarker(LANG("Real-time performance metrics"));
    
    float fps = ImGui::GetIO().Framerate;
    float frameTime = 1000.0f / fps;
    
    // Performance metrics in a styled box
    ImGui::BeginChild("Performance", ImVec2(0, 80), true);
    ImGui::Text("%s: %.1f FPS", LANG("Frame Rate"), fps);
    ImGui::Text("%s: %.2f ms", LANG("Frame Time"), frameTime);
    
    // Performance indicator
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 200);
    if (fps >= 60.0f)
    {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), LANG("Excellent"));
    }
    else if (fps >= 30.0f)
    {
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), LANG("Good"));
    }
    else
    {
        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), LANG("Poor"));
    }
    ImGui::EndChild();

    ImGui::Spacing();

    // Unity Backend Status section
    ImGui::Text(LANG("Unity Backend Status"));
    ImGui::SameLine();
    helpMarker(LANG("Connection status to Unity runtime"));
    
    ImGui::BeginChild("BackendStatus", ImVec2(0, 100), true);
    
    auto unityModule = GetModuleHandleA("GameAssembly.dll");
    if (!unityModule)
    {
        unityModule = GetModuleHandleA("UnityPlayer.dll");
    }

    if (unityModule)
    {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), LANG("Unity backend detected"));
        
        auto gameAssembly = GetModuleHandleA("GameAssembly.dll");
        if (gameAssembly)
        {
            ImGui::Text("%s: IL2CPP (GameAssembly.dll)", LANG("Backend Type"));
            ImGui::Text("%s: %s", LANG("Status"), LANG("Connected and ready"));
        }
        else
        {
            ImGui::Text("%s: Mono (UnityPlayer.dll)", LANG("Backend Type"));
            ImGui::Text("%s: %s", LANG("Status"), LANG("Connected and ready"));
        }
        
        ImGui::Text("%s: 0x%p", LANG("Module Address"), unityModule);
    }
    else
    {
        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), LANG("Unity backend not found"));
        ImGui::TextWrapped(LANG("Make sure this is injected into a Unity game."));
        ImGui::TextWrapped(LANG("Supported Unity versions: 2019.4+ (IL2CPP/Mono)"));
    }
    ImGui::EndChild();

    ImGui::Spacing();

    // Unity Explorer Status section
    ImGui::Text(LANG("Unity Explorer Status"));
    ImGui::SameLine();
    helpMarker(LANG("Scene hierarchy and object inspector status"));
    
    ImGui::BeginChild("ExplorerStatus", ImVec2(0, 80), true);
    
    if (m_unityExplorerInitialized)
    {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), LANG("Unity Explorer ready"));
        ImGui::Text("%s: %s", LANG("Status"), LANG("Initialized and functional"));
        ImGui::Text(LANG("Features: Scene hierarchy, Object inspector, Component analysis"));
    }
    else
    {
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), LANG("Unity Explorer not initialized"));
        ImGui::Text("%s: %s", LANG("Status"), LANG("Waiting for initialization"));
        ImGui::Text(LANG("Click 'Open Unity Explorer' to start"));
    }
    ImGui::EndChild();

    ImGui::Spacing();

    // Quick Actions section
    ImGui::Text(LANG("Quick Actions"));
    ImGui::SameLine();
    helpMarker(LANG("Common actions and shortcuts"));
    
    ImGui::BeginChild("QuickActions", ImVec2(0, 120), true);
    
    // Action buttons in a grid layout
    if (ImGui::Button(LANG("Open Unity Explorer"), ImVec2(150, 30)))
    {
        showUnityExplorer();
    }
    ImGui::SameLine();
    if (ImGui::Button(LANG("Refresh Scene"), ImVec2(150, 30)))
    {
        if (m_unityExplorer && m_unityExplorerInitialized)
        {
            shutdownUnityExplorer();
            initializeUnityExplorer();
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button(LANG("Test Unity API"), ImVec2(150, 30)))
    {
        try
        {
            auto coreModule = UnityResolve::Get("UnityEngine.CoreModule.dll");
            if (coreModule)
            {
                LOG_INFO("[GUI] Unity API test: SUCCESS - Found UnityEngine.CoreModule.dll");
            }
            else
            {
                LOG_ERROR("[GUI] Unity API test: FAILED - Could not find UnityEngine.CoreModule.dll");
            }
        }
        catch (...)
        {
            LOG_ERROR("[GUI] Unity API test: EXCEPTION - Error accessing Unity API");
        }
    }
    
    ImGui::Spacing();
    
    // Shortcuts info
    ImGui::Text(LANG("Keyboard Shortcuts:"));
    ImGui::BulletText(LANG("INSERT - Toggle GUI visibility"));
    ImGui::BulletText(LANG("F1 - Toggle Unity Explorer"));
    ImGui::BulletText(LANG("F5 - Refresh scene (in Unity Explorer)"));
    ImGui::BulletText(LANG("Alt+F4 - Exit application"));
    
    ImGui::EndChild();

    ImGui::Spacing();

    // System Information section
    ImGui::Text(LANG("System Information"));
    ImGui::SameLine();
    helpMarker(LANG("Runtime environment details"));
    
    ImGui::BeginChild("SystemInfo", ImVec2(0, 80), true);
    
    // Get system info
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    
    ImGui::Text("%s: %s", LANG("Architecture"), (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) ? "x64" : "x86");
    ImGui::Text("%s: %d", LANG("Processors"), sysInfo.dwNumberOfProcessors);
    ImGui::Text("%s: %d KB", LANG("Page Size"), sysInfo.dwPageSize / 1024);
    
    // Memory info
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo))
    {
        ImGui::Text("%s: %.1f GB", LANG("Total RAM"), (float)memInfo.ullTotalPhys / (1024.0f * 1024.0f * 1024.0f));
    }
    
    ImGui::EndChild();

    ImGui::End();

    if (!p_open)
    {
        m_showExample = false;
    }
}

void GUI::renderAboutModal()
{
    if (ImGui::BeginPopupModal(LANG("About Unity Runtime Inspector"), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // Header
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), LANG("Unity Runtime Inspector"));
        ImGui::PopFont();
        ImGui::TextDisabled(LBL_VERSION);
        ImGui::Separator();

        ImGui::TextWrapped(LANG("A powerful runtime inspection and debugging tool for Unity games."));
        ImGui::TextWrapped(LANG("Built with Dear ImGui and UnityResolve for seamless integration with Unity's runtime environment."));

        ImGui::Spacing();
        ImGui::Text(LANG("Key Features:"));
        ImGui::BulletText(LANG("Real-time scene hierarchy explorer"));
        ImGui::BulletText(LANG("GameObject and component inspector"));
        ImGui::BulletText(LANG("Live value monitoring and modification"));
        ImGui::BulletText(LANG("Cross-platform Unity support (IL2CPP/Mono)"));
        ImGui::BulletText(LANG("Performance monitoring and analysis"));
        ImGui::BulletText(LANG("Memory inspection and debugging"));

        ImGui::Spacing();
        ImGui::Text(LANG("Supported Unity Versions:"));
        ImGui::BulletText(LANG("Unity 2019.4 LTS and later"));
        ImGui::BulletText(LANG("IL2CPP and Mono scripting backends"));
        ImGui::BulletText(LANG("Windows x86 and x64 platforms"));

        ImGui::Spacing();
        ImGui::Text(LANG("Technologies:"));
        ImGui::BulletText(LANG("Dear ImGui - Immediate mode GUI"));
        ImGui::BulletText(LANG("UnityResolve - Unity runtime access"));
        ImGui::BulletText(LANG("MinHook - API hooking library"));

        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button(BTN_CLOSE, ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // Controls Reference Modal
    if (ImGui::BeginPopupModal(LANG("Controls Reference"), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), LANG("Controls Reference"));
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::Text(LANG("Global Controls:"));
        ImGui::BulletText(LANG("INSERT - Toggle GUI visibility"));
        ImGui::BulletText(LANG("Alt+F4 - Exit application"));

        ImGui::Spacing();
        ImGui::Text(LANG("Unity Explorer Controls:"));
        ImGui::BulletText(LANG("F1 - Toggle Unity Explorer window"));
        ImGui::BulletText(LANG("F5 - Refresh scene hierarchy"));
        ImGui::BulletText(LANG("Mouse Click - Select objects in hierarchy"));
        ImGui::BulletText(LANG("Double Click - Expand/collapse tree nodes"));
        ImGui::BulletText(LANG("Right Click - Context menu for objects"));

        ImGui::Spacing();
        ImGui::Text(LANG("Object Inspector Controls:"));
        ImGui::BulletText(LANG("Click headers - Expand/collapse components"));
        ImGui::BulletText(LANG("Hover over (?) - Show help tooltips"));
        ImGui::BulletText(LANG("Search box - Filter objects by name"));

        ImGui::Spacing();
        ImGui::Text(LANG("Navigation:"));
        ImGui::BulletText(LANG("Mouse wheel - Scroll through lists"));
        ImGui::BulletText(LANG("Ctrl+Mouse wheel - Zoom in/out"));
        ImGui::BulletText(LANG("Tab - Navigate between input fields"));

        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button(BTN_CLOSE, ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

bool GUI::initializeUnityExplorer()
{
    if (m_unityExplorerInitialized)
    {
        LOG_WARNING("[GUI] Unity Explorer already initialized");
        return true;
    }

    LOG_INFO("[GUI] Initializing Unity Explorer...");

    try
    {
        m_unityExplorer = std::make_unique<UnityExplorer>();

        if (m_unityExplorer->initialize())
        {
            m_unityExplorerInitialized = true;
            LOG_INFO("[GUI] Unity Explorer initialized successfully!");
            return true;
        }
        LOG_ERROR("[GUI] Failed to initialize Unity Explorer");
        m_unityExplorer.reset();
        return false;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("[GUI] Exception initializing Unity Explorer: %s", e.what());
        m_unityExplorer.reset();
        return false;
    }
    catch (...)
    {
        LOG_ERROR("[GUI] Unknown exception initializing Unity Explorer");
        m_unityExplorer.reset();
        return false;
    }
}

void GUI::shutdownUnityExplorer()
{
    if (m_unityExplorer)
    {
        LOG_INFO("[GUI] Shutting down Unity Explorer...");
        m_unityExplorer->shutdown();
        m_unityExplorer.reset();
    }
    m_unityExplorerInitialized = false;
}

void GUI::setupImGuiStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();

    // Modern rounded corners
    style.WindowRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 6.0f;
    style.ChildRounding = 6.0f;
    style.PopupRounding = 6.0f;

    // Improved spacing and padding
    style.WindowPadding = ImVec2(12, 12);
    style.FramePadding = ImVec2(10, 6);
    style.ItemSpacing = ImVec2(10, 6);
    style.ItemInnerSpacing = ImVec2(6, 6);
    style.IndentSpacing = 24.0f;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 8.0f;

    // Borders
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;

    
    ImVec4* colors = style.Colors;
    
    // Core colors
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    
    // Window colors
    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.11f, 0.95f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.06f, 0.07f, 0.09f, 0.95f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.09f, 0.11f, 0.95f);
    
    // Border colors
    colors[ImGuiCol_Border] = ImVec4(0.20f, 0.22f, 0.25f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    
    // Frame colors
    colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.14f, 0.16f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.15f, 0.17f, 0.19f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.20f, 0.22f, 1.00f);
    
    // Title bar colors
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.14f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
    
    // Menu bar colors
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
    
    // Scrollbar colors
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.07f, 0.09f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.27f, 0.30f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.30f, 0.32f, 0.35f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.35f, 0.37f, 0.40f, 1.00f);
    
    // Button colors with blue accent
    colors[ImGuiCol_Button] = ImVec4(0.15f, 0.17f, 0.19f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.27f, 0.30f, 1.00f);
    
    // Header colors (for collapsible sections)
    colors[ImGuiCol_Header] = ImVec4(0.15f, 0.17f, 0.19f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.27f, 0.30f, 1.00f);
    
    // Tab colors
    colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.14f, 0.16f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.15f, 0.17f, 0.19f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.18f, 0.20f, 0.22f, 1.00f);
    
    // Separator colors
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.22f, 0.25f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.25f, 0.27f, 0.30f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.30f, 0.32f, 0.35f, 1.00f);
    
    // Resize grip colors
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.25f, 0.27f, 0.30f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.30f, 0.32f, 0.35f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.35f, 0.37f, 0.40f, 0.95f);
    
    // Plot colors
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    
    // Nav colors
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    
    // Modal overlay
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void GUI::helpMarker(const char* desc)
{
    ImGui::TextDisabled(LANG("(?)"));
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void GUI::testLanguageSystem()
{
    LOG_INFO("[GUI] === Language System Test ===");
    
    // Initialize (should be instant now)
    Language::getInstance().initialize();
    LOG_INFO("[GUI] %s", LANG("Language system initialized successfully"));
    
    // Test current language
    LanguageType currentLang = Language::getInstance().getCurrentLanguage();
    const char* langName = Language::getInstance().getLanguageName(currentLang);
    LOG_INFO("[GUI] %s: %s", LANG("Current language"), langName);
    
    // Test key translations
    const char* testKeys[] = {
        "Unity Runtime Inspector",
        "Performance", 
        "File", "Windows", "Tools", "Help",
        "Open Unity Explorer",
        "Close",
        "English", "Chinese",
        "Non-existent Key"  // Should return key itself
    };
    
    LOG_INFO("[GUI] Testing %s translations:", LANG("Chinese"));
    for (const char* key : testKeys)
    {
        const char* translation = LANG(key);
        LOG_INFO("[GUI]   '%s' -> '%s'", key, translation);
    }
    
    // Test language switching
    LOG_INFO("[GUI] Switching to %s...", LANG("English"));
    SWITCH_LANG(LanguageType::English);
    
    for (const char* key : testKeys)
    {
        const char* translation = LANG(key);
        LOG_INFO("[GUI]   '%s' -> '%s'", key, translation);
    }
    
    // Switch back
    LOG_INFO("[GUI] Switching back to %s...", LANG("Chinese"));
    SWITCH_LANG(LanguageType::Chinese);
    
    // Test available languages
    const auto& availableLangs = Language::getInstance().getAvailableLanguages();
    LOG_INFO("[GUI] %s: %s", LANG("Available languages"), availableLangs.size() > 0 ? "EN, zh-CN" : "None");
    
    LOG_INFO("[GUI] === Language System Test Complete ===");
}
