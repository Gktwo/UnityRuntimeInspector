#include "pch.h"
#include "gui.h"

#include "inspector/unity_explorer.h"

GUI::GUI()
{
    setupImGuiStyle();
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
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                // TODO: Implement exit functionality
            }
            ImGui::EndMenu();
        }

        // Windows menu
        if (ImGui::BeginMenu("Windows"))
        {
            ImGui::MenuItem("Example Window", nullptr, &m_showExample);
            ImGui::Separator();

            if (ImGui::MenuItem("Unity Explorer", "F1", &m_showUnityExplorer))
            {
                if (m_showUnityExplorer && !m_unityExplorerInitialized)
                {
                    initializeUnityExplorer();
                }
            }

            ImGui::EndMenu();
        }

        // Tools menu
        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Refresh Unity Explorer", "F5"))
            {
                if (m_unityExplorer && m_unityExplorerInitialized)
                {
                    // Force refresh by reinitializing
                    shutdownUnityExplorer();
                    initializeUnityExplorer();
                }
            }

            ImGui::Separator();

            if (ImGui::BeginMenu("Theme"))
            {
                if (ImGui::MenuItem("Dark (Default)"))
                {
                    ImGui::StyleColorsDark();
                    setupImGuiStyle();
                }
                if (ImGui::MenuItem("Light"))
                {
                    ImGui::StyleColorsLight();
                    setupImGuiStyle();
                }
                if (ImGui::MenuItem("Classic"))
                {
                    ImGui::StyleColorsClassic();
                    setupImGuiStyle();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        // Help menu
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About Unity Runtime Inspector"))
            {
                ImGui::OpenPopup("About Unity Runtime Inspector");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Controls Reference"))
            {
                ImGui::OpenPopup("Controls Reference");
            }
            ImGui::EndMenu();
        }

        // Status section with better visual indicators
        ImGui::Separator();
        
        // Unity Explorer status with colored indicators
        ImGui::Text("Unity Explorer:");
        ImGui::SameLine();
        
        if (m_unityExplorer && m_unityExplorerInitialized && m_showUnityExplorer)
        {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "● Active");
        }
        else if (m_showUnityExplorer)
        {
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "● Initializing...");
        }
        else
        {
            ImGui::TextDisabled("● Inactive");
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

        ImGui::Text("Unity:");
        ImGui::SameLine();
        
        if (unityModule)
        {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "● Connected");
        }
        else
        {
            ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "● Disconnected");
        }

        // Performance info
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();
        
        float fps = ImGui::GetIO().Framerate;
        ImGui::Text("FPS: %.1f", fps);
        
        // Color code FPS
        if (fps >= 60.0f)
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "●");
        }
        else if (fps >= 30.0f)
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "●");
        }
        else
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "●");
        }

        // Controls hint
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();
        ImGui::TextDisabled("INSERT: Toggle GUI | F1: Unity Explorer | F5: Refresh");

        ImGui::EndMainMenuBar();
    }
}

void GUI::renderExampleWindow()
{
    static bool p_open = true;

    if (!ImGui::Begin("Unity Runtime Inspector - Dashboard", &p_open, ImGuiWindowFlags_None))
    {
        ImGui::End();
        return;
    }

    // Header section
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Use default font for header
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Unity Runtime Inspector");
    ImGui::PopFont();
    ImGui::TextDisabled("Real-time Unity game inspection and debugging tool");
    ImGui::Separator();

    // Performance section
    ImGui::Text("Performance");
    ImGui::SameLine();
    helpMarker("Real-time performance metrics");
    
    float fps = ImGui::GetIO().Framerate;
    float frameTime = 1000.0f / fps;
    
    // Performance metrics in a styled box
    ImGui::BeginChild("Performance", ImVec2(0, 80), true);
    ImGui::Text("Frame Rate: %.1f FPS", fps);
    ImGui::Text("Frame Time: %.2f ms", frameTime);
    
    // Performance indicator
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 200);
    if (fps >= 60.0f)
    {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "● Excellent");
    }
    else if (fps >= 30.0f)
    {
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "● Good");
    }
    else
    {
        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "● Poor");
    }
    ImGui::EndChild();

    ImGui::Spacing();

    // Unity Backend Status section
    ImGui::Text("Unity Backend Status");
    ImGui::SameLine();
    helpMarker("Connection status to Unity runtime");
    
    ImGui::BeginChild("BackendStatus", ImVec2(0, 100), true);
    
    auto unityModule = GetModuleHandleA("GameAssembly.dll");
    if (!unityModule)
    {
        unityModule = GetModuleHandleA("UnityPlayer.dll");
    }

    if (unityModule)
    {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "✓ Unity backend detected");
        
        auto gameAssembly = GetModuleHandleA("GameAssembly.dll");
        if (gameAssembly)
        {
            ImGui::Text("Backend Type: IL2CPP (GameAssembly.dll)");
            ImGui::Text("Status: Connected and ready");
        }
        else
        {
            ImGui::Text("Backend Type: Mono (UnityPlayer.dll)");
            ImGui::Text("Status: Connected and ready");
        }
        
        ImGui::Text("Module Address: 0x%p", unityModule);
    }
    else
    {
        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "✗ Unity backend not found");
        ImGui::TextWrapped("Make sure this is injected into a Unity game.");
        ImGui::TextWrapped("Supported Unity versions: 2019.4+ (IL2CPP/Mono)");
    }
    ImGui::EndChild();

    ImGui::Spacing();

    // Unity Explorer Status section
    ImGui::Text("Unity Explorer Status");
    ImGui::SameLine();
    helpMarker("Scene hierarchy and object inspector status");
    
    ImGui::BeginChild("ExplorerStatus", ImVec2(0, 80), true);
    
    if (m_unityExplorerInitialized)
    {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "✓ Unity Explorer ready");
        ImGui::Text("Status: Initialized and functional");
        ImGui::Text("Features: Scene hierarchy, Object inspector, Component analysis");
    }
    else
    {
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "○ Unity Explorer not initialized");
        ImGui::Text("Status: Waiting for initialization");
        ImGui::Text("Click 'Open Unity Explorer' to start");
    }
    ImGui::EndChild();

    ImGui::Spacing();

    // Quick Actions section
    ImGui::Text("Quick Actions");
    ImGui::SameLine();
    helpMarker("Common actions and shortcuts");
    
    ImGui::BeginChild("QuickActions", ImVec2(0, 120), true);
    
    // Action buttons in a grid layout
    if (ImGui::Button("Open Unity Explorer", ImVec2(150, 30)))
    {
        showUnityExplorer();
    }
    ImGui::SameLine();
    if (ImGui::Button("Refresh Scene", ImVec2(150, 30)))
    {
        if (m_unityExplorer && m_unityExplorerInitialized)
        {
            shutdownUnityExplorer();
            initializeUnityExplorer();
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Test Unity API", ImVec2(150, 30)))
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
    ImGui::Text("Keyboard Shortcuts:");
    ImGui::BulletText("INSERT - Toggle GUI visibility");
    ImGui::BulletText("F1 - Toggle Unity Explorer");
    ImGui::BulletText("F5 - Refresh scene (in Unity Explorer)");
    ImGui::BulletText("Alt+F4 - Exit application");
    
    ImGui::EndChild();

    ImGui::Spacing();

    // System Information section
    ImGui::Text("System Information");
    ImGui::SameLine();
    helpMarker("Runtime environment details");
    
    ImGui::BeginChild("SystemInfo", ImVec2(0, 80), true);
    
    // Get system info
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    
    ImGui::Text("Architecture: %s", (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) ? "x64" : "x86");
    ImGui::Text("Processors: %d", sysInfo.dwNumberOfProcessors);
    ImGui::Text("Page Size: %d KB", sysInfo.dwPageSize / 1024);
    
    // Memory info
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo))
    {
        ImGui::Text("Total RAM: %.1f GB", (float)memInfo.ullTotalPhys / (1024.0f * 1024.0f * 1024.0f));
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
    if (ImGui::BeginPopupModal("About Unity Runtime Inspector", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // Header
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Unity Runtime Inspector");
        ImGui::PopFont();
        ImGui::TextDisabled("Version 1.0.0");
        ImGui::Separator();

        ImGui::TextWrapped("A powerful runtime inspection and debugging tool for Unity games. "
                          "Built with Dear ImGui and UnityResolve for seamless integration with Unity's runtime environment.");

        ImGui::Spacing();
        ImGui::Text("Key Features:");
        ImGui::BulletText("Real-time scene hierarchy explorer");
        ImGui::BulletText("GameObject and component inspector");
        ImGui::BulletText("Live value monitoring and modification");
        ImGui::BulletText("Cross-platform Unity support (IL2CPP/Mono)");
        ImGui::BulletText("Performance monitoring and analysis");
        ImGui::BulletText("Memory inspection and debugging");

        ImGui::Spacing();
        ImGui::Text("Supported Unity Versions:");
        ImGui::BulletText("Unity 2019.4 LTS and later");
        ImGui::BulletText("IL2CPP and Mono scripting backends");
        ImGui::BulletText("Windows x86 and x64 platforms");

        ImGui::Spacing();
        ImGui::Text("Technologies:");
        ImGui::BulletText("Dear ImGui - Immediate mode GUI");
        ImGui::BulletText("UnityResolve - Unity runtime access");
        ImGui::BulletText("MinHook - API hooking library");

        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button("Close", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // Controls Reference Modal
    if (ImGui::BeginPopupModal("Controls Reference", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Controls Reference");
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::Text("Global Controls:");
        ImGui::BulletText("INSERT - Toggle GUI visibility");
        ImGui::BulletText("Alt+F4 - Exit application");

        ImGui::Spacing();
        ImGui::Text("Unity Explorer Controls:");
        ImGui::BulletText("F1 - Toggle Unity Explorer window");
        ImGui::BulletText("F5 - Refresh scene hierarchy");
        ImGui::BulletText("Mouse Click - Select objects in hierarchy");
        ImGui::BulletText("Double Click - Expand/collapse tree nodes");
        ImGui::BulletText("Right Click - Context menu for objects");

        ImGui::Spacing();
        ImGui::Text("Object Inspector Controls:");
        ImGui::BulletText("Click headers - Expand/collapse components");
        ImGui::BulletText("Hover over (?) - Show help tooltips");
        ImGui::BulletText("Search box - Filter objects by name");

        ImGui::Spacing();
        ImGui::Text("Navigation:");
        ImGui::BulletText("Mouse wheel - Scroll through lists");
        ImGui::BulletText("Ctrl+Mouse wheel - Zoom in/out");
        ImGui::BulletText("Tab - Navigate between input fields");

        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button("Close", ImVec2(120, 0)))
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
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
