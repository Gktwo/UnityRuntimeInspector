#include "pch.h"
#include "main.h"

namespace Main
{
	void run()
	{
		Utils::attachConsole();

		LOG_DEBUG("[ImGui] Starting initialization...");
	
		Sleep(1000);

		// Initialize renderer
		Renderer& renderer = Renderer::getInstance();
		if (renderer.initialize())
		{
			LOG_DEBUG("[ImGui] Renderer initialized successfully!");
			LOG_DEBUG("[ImGui] Using backend: %s", renderer.getBackend()->getName());
			LOG_DEBUG("[ImGui] Press INSERT to toggle GUI visibility");
		}
		else
		{
			LOG_DEBUG("[ImGui] Failed to initialize renderer!");
		}
	}
}
