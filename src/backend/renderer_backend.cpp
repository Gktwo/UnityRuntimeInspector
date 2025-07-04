#include <pch.h>
#include "renderer_backend.h"

#include "dx11_backend.h"
#include "utils/dx_utils.h"

std::unique_ptr<IRendererBackend> createBackend()
{
	RenderAPI api = DXUtils::getRenderAPI();
    
	switch (api)
	{
	case RenderAPI::DirectX9:
		// TODO: Implement DirectX9 backend
		return nullptr;
	case RenderAPI::DirectX11:
		return std::make_unique<DX11Backend>();
	case RenderAPI::DirectX12:
		// TODO: Implement DirectX12 backend
		return nullptr;
	default:
		return nullptr;
	}
}
