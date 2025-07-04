#include "pch.h"
#include "core/renderer.h"
#include <Windows.h>
#include <iostream>
#include <thread>

void Test()
{
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

	std::cout << "[ImGui] Starting initialization..." << std::endl;

	// Give the application time to initialize
	Sleep(1000);

	// Initialize renderer
	Renderer& renderer = Renderer::getInstance();
	if (renderer.initialize())
	{
		std::cout << "[ImGui] Renderer initialized successfully!" << std::endl;
		std::cout << "[ImGui] Using backend: " << renderer.getBackend()->getName() << std::endl;
		std::cout << "[ImGui] Press INSERT to toggle GUI visibility" << std::endl;
	}
	else
	{
		std::cout << "[ImGui] Failed to initialize renderer!" << std::endl;
	}
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD   ul_reason_for_call,
                      LPVOID  lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		std::thread(Test).detach();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
