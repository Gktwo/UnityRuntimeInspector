#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <filesystem>


#include <d3d9.h>
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi.h>

#include "UnityResolve.hpp"

// Include ImGui
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

// Project includes
#include "core/renderer.h"
#include "ui/gui.h"
#include "memory/hook_manager.h"
#include "utils/dx_utils.h"
#include "utils/helpers.h"