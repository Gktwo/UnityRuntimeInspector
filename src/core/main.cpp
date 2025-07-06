#include "pch.h"
#include "main.h"

void Main::run()
{
    Utils::attachConsole();

    LOG_INFO("[ImGui] Starting initialization...");

    Sleep(1000);
    if (!initializeUnity())
        LOG_ERROR("Unable to initialize Unity! Maybe assemblies are not found?");

    // Initialize renderer
    Renderer& renderer = Renderer::getInstance();
    if (renderer.initialize())
    {
        LOG_INFO("[ImGui] Renderer initialized successfully!");
        LOG_INFO("[ImGui] Using backend: %s", renderer.getBackend()->getName());
        LOG_INFO("[ImGui] Press INSERT to toggle GUI visibility");
    }
    else
    {
        LOG_INFO("[ImGui] Failed to initialize renderer!");
    }
}

UnityModuleBackendInfo Main::getUnityBackend()
{
    LOG_INFO("Finding Unity backend...");

    UnityModuleBackendInfo info;
    info.module = nullptr;
    info.mode = UnityResolve::Mode::Mono;

    // Check if Unity is loaded
    auto unityModule = GetModuleHandleA("UnityPlayer.dll");
    if (unityModule == nullptr)
    {
        LOG_ERROR("UnityPlayer.dll not found! Is this a Unity game?");
        return info;
    }

    auto assembly = GetModuleHandleA("GameAssembly.dll");
    if (assembly)
    {
        info.module = assembly;
        info.mode = UnityResolve::Mode::Il2Cpp;
        LOG_INFO("Found Il2Cpp backend!");
        return info;
    }
    LOG_WARNING("GameAssembly.dll not found, trying fallback to Mono...");
    assembly = GetModuleHandleA("mono-2.0-bdwgc.dll"); // Newer versions of Unity
    if (assembly)
    {
        info.module = assembly;
        info.mode = UnityResolve::Mode::Mono;
        LOG_INFO("Found Mono backend!");
        return info;
    }
    assembly = GetModuleHandleA("mono.dll");
    if (assembly)
    {
        info.module = assembly;
        info.mode = UnityResolve::Mode::Mono;
        LOG_INFO("Found Mono backend!");
        return info;
    }

    LOG_ERROR("Unable to find Unity backend! Neither GameAssembly.dll nor mono.dll found.");
    return info;
}

bool Main::initializeUnity()
{
    auto backend = getUnityBackend();
    if (backend.module == nullptr)
    {
        LOG_ERROR("Unity backend not found!");
        return false;
    }

    UnityResolve::Init(backend.module, backend.mode);

    return true;
}
