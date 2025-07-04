#include "pch.h"
#include "gui.h"

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

	// Render main menu bar
	renderMainMenuBar();

	// Render demo window if enabled
	if (m_showDemo)
	{
		// ImGui::ShowDemoWindow(&m_showDemo);
	}

	// Render example window if enabled
	if (m_showExample)
	{
		renderExampleWindow();
	}
}

void GUI::showDemoWindow()
{
	m_showDemo = true;
}

void GUI::showExampleWindow()
{
	m_showExample = true;
}

void GUI::renderMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("Demo Window", nullptr, &m_showDemo);
			ImGui::MenuItem("Example Window", nullptr, &m_showExample);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
			{
				// Show about dialog
			}
			ImGui::EndMenu();
		}

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

	ImGui::Text("Hello from ImGui!");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
	            1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	static float f = 0.0f;
	static int   counter = 0;

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

	if (ImGui::Button("Button"))
	{
		counter++;
	}
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	static bool show_another_window = false;
	ImGui::Checkbox("Another Window", &show_another_window);

	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
		{
			show_another_window = false;
		}
		ImGui::End();
	}

	ImGui::End();

	if (!p_open)
	{
		m_showExample = false;
	}
}
