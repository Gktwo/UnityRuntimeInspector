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

        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Refresh Unity Explorer"))
            {
                if (m_unityExplorer && m_unityExplorerInitialized)
                {
                    // Force refresh by reinitializing
                    shutdownUnityExplorer();
                    initializeUnityExplorer();
                }
            }

            ImGui::Separator();

            if (ImGui::BeginMenu("Style"))
            {
                if (ImGui::MenuItem("Dark"))
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

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                ImGui::OpenPopup("About Unity Runtime Inspector");
            }
            ImGui::EndMenu();
        }

        // Status indicator
        ImGui::Separator();
        if (m_unityExplorer && m_unityExplorerInitialized && m_showUnityExplorer)
        {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Unity Explorer: Active");
        }
        else if (m_showUnityExplorer)
        {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Unity Explorer: Initializing...");
        }
        else
        {
            ImGui::TextDisabled("Unity Explorer: Inactive");
        }

        // Controls hint
        ImGui::Separator();
        ImGui::TextDisabled("INSERT: Toggle GUI | F1: Unity Explorer");

        ImGui::EndMainMenuBar();
    }
}

void GUI::renderExampleWindow()
{
    static bool p_open = true;

    if (!ImGui::Begin("Example Window", &p_open, ImGuiWindowFlags_None))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Unity Runtime Inspector");
    ImGui::Separator();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::Spacing();

    // Unity Explorer section
    ImGui::Text("Unity Tools:");
    if (ImGui::Button("Open Unity Explorer"))
    {
        showUnityExplorer();
    }
    ImGui::SameLine();
    if (ImGui::Button("Refresh Scene"))
    {
        if (m_unityExplorer && m_unityExplorerInitialized)
        {
            shutdownUnityExplorer();
            initializeUnityExplorer();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();

    // Unity backend status
    ImGui::Text("Unity Backend Status:");

    auto unityModule = GetModuleHandleA("GameAssembly.dll");
    if (!unityModule)
    {
        unityModule = GetModuleHandleA("UnityPlayer.dll");
    }

    if (unityModule)
    {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "✓ Unity backend detected");

        auto gameAssembly = GetModuleHandleA("GameAssembly.dll");
        if (gameAssembly)
        {
            ImGui::Text("Backend: IL2CPP (GameAssembly.dll)");
        }
        else
        {
            ImGui::Text("Backend: Mono (UnityPlayer.dll)");
        }
    }
    else
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "✗ Unity backend not found");
        ImGui::TextWrapped("Make sure this is injected into a Unity game.");
    }

    // Unity Explorer status
    if (m_unityExplorerInitialized)
    {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "✓ Unity Explorer ready");
    }
    else
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "○ Unity Explorer not initialized");
    }

    ImGui::Spacing();
    ImGui::Separator();

    // Original example content
    // static float f = 0.0f;
    // static int counter = 0;
    //
    // ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    //
    // if (ImGui::Button("Button"))
    // {
    //     counter++;
    // }
    // ImGui::SameLine();
    // ImGui::Text("counter = %d", counter);
    //
    // static bool show_another_window = false;
    // ImGui::Checkbox("Another Window", &show_another_window);
    //
    // if (show_another_window)
    // {
    //     ImGui::Begin("Another Window", &show_another_window);
    //     ImGui::Text("Hello from another window!");
    //     if (ImGui::Button("Close Me"))
    //     {
    //         show_another_window = false;
    //     }
    //     ImGui::End();
    // }

    // Test Unity API button
    ImGui::Spacing();
    if (ImGui::Button("Test Unity API"))
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
        ImGui::Text("Unity Runtime Inspector");
        ImGui::Separator();

        ImGui::Text("A runtime inspection tool for Unity games");
        ImGui::Text("Built with Dear ImGui and UnityResolve");

        ImGui::Spacing();
        ImGui::Text("Features:");
        ImGui::BulletText("Scene hierarchy explorer");
        ImGui::BulletText("GameObject and component inspector");
        ImGui::BulletText("Real-time value monitoring");
        ImGui::BulletText("Cross-platform Unity support (IL2CPP/Mono)");

        ImGui::Spacing();
        ImGui::Text("Controls:");
        ImGui::BulletText("INSERT - Toggle GUI visibility");
        ImGui::BulletText("F1 - Toggle Unity Explorer");
        ImGui::BulletText("F5 - Refresh scene (in Unity Explorer)");

        ImGui::Spacing();

        if (ImGui::Button("Close"))
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

    // Rounding
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 3.0f;

    // Spacing
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(8, 4);
    style.ItemInnerSpacing = ImVec2(4, 4);
    style.IndentSpacing = 20.0f;

    // Borders
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;

    // Colors
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
}
