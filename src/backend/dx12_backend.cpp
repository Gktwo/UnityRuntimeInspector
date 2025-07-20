#include "pch.h"
#include "dx12_backend.h"

#include "NotoSans.hpp"
#include "HYWenHei.hpp"

#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "ui/gui.h"
#include "utils/dx_utils.h"

DX12Backend* DX12Backend::s_instance = nullptr;
DX12Backend::Present_t DX12Backend::m_originalPresent = nullptr;
DX12Backend::ResizeBuffers_t DX12Backend::m_originalResizeBuffers = nullptr;
WNDPROC DX12Backend::m_originalWndProc = nullptr;

// DescriptorHeapAllocator implementation
void DescriptorHeapAllocator::Create(ID3D12Device* device, ID3D12DescriptorHeap* heap)
{
    IM_ASSERT(Heap == nullptr && FreeIndices.empty());
    Heap = heap;
    D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
    HeapType = desc.Type;
    HeapStartCpu = Heap->GetCPUDescriptorHandleForHeapStart();
    HeapStartGpu = Heap->GetGPUDescriptorHandleForHeapStart();
    HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(HeapType);
    FreeIndices.reserve((int)desc.NumDescriptors);
    for (int n = desc.NumDescriptors; n > 0; n--)
        FreeIndices.push_back(n - 1);
}

void DescriptorHeapAllocator::Destroy()
{
    Heap = nullptr;
    FreeIndices.clear();
}

void DescriptorHeapAllocator::Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
{
    IM_ASSERT(FreeIndices.Size > 0);
    int idx = FreeIndices.back();
    FreeIndices.pop_back();
    out_cpu_desc_handle->ptr = HeapStartCpu.ptr + (idx * HeapHandleIncrement);
    out_gpu_desc_handle->ptr = HeapStartGpu.ptr + (idx * HeapHandleIncrement);
}

void DescriptorHeapAllocator::Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
{
    int cpu_idx = (int)((out_cpu_desc_handle.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
    int gpu_idx = (int)((out_gpu_desc_handle.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);
    IM_ASSERT(cpu_idx == gpu_idx);
    FreeIndices.push_back(cpu_idx);
}

DX12Backend::DX12Backend()
    : m_initialized(false)
    , m_imguiInitialized(false)
    , m_window(nullptr)
    , m_device(nullptr)
    , m_rtvDescHeap(nullptr)
    , m_srvDescHeap(nullptr)
    , m_commandQueue(nullptr)
    , m_commandList(nullptr)
    , m_fence(nullptr)
    , m_fenceEvent(nullptr)
    , m_fenceLastSignaledValue(0)
    , m_swapChain(nullptr)
    , m_swapChainOccluded(false)
    , m_swapChainWaitableObject(nullptr)
    , m_frameIndex(0)
{
    assert(s_instance == nullptr);
    s_instance = this;

    // Initialize arrays
    for (int i = 0; i < NUM_BACK_BUFFERS; i++)
    {
        m_renderTargetResource[i] = nullptr;
        m_renderTargetDescriptor[i] = {};
    }

    for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
    {
        m_frameContext[i].CommandAllocator = nullptr;
        m_frameContext[i].FenceValue = 0;
    }
}

DX12Backend::~DX12Backend()
{
    shutdown();
    s_instance = nullptr;
}

bool DX12Backend::initialize()
{
    if (m_initialized) return true;

    if (!HookManager::getInstance().initialize()) return false;

    if (!setupHooks()) return false;

    m_initialized = true;
    return true;
}

void DX12Backend::shutdown()
{
    if (!m_initialized) return;

    shutdownImGui();
    cleanupD3D12Resources();

    if (m_window && m_originalWndProc)
    {
        SetWindowLongPtr(m_window, GWLP_WNDPROC, (LONG_PTR)m_originalWndProc);
        m_originalWndProc = nullptr;
    }

    m_initialized = false;
}

LRESULT CALLBACK DX12Backend::hookedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) return true;

    // Handle hotkeys
    if (uMsg == WM_KEYDOWN)
    {
        GUI& gui = GUI::getInstance();
        
        switch (wParam)
        {
            case VK_INSERT:
                gui.setVisible(!gui.isVisible());
            return true;
            
            case VK_F1:
                if (gui.isVisible())
                    gui.toggleUnityExplorer();
            return true;
            
            default:
                break;
        }
    }

    if (s_instance->onInput(uMsg, wParam, lParam)) return true;

    // Handle specific messages
    switch (uMsg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED) s_instance->onResize(LOWORD(lParam), HIWORD(lParam));
        break;
    }

    // Call original window procedure
    return CallWindowProc(m_originalWndProc, hWnd, uMsg, wParam, lParam);
}

bool DX12Backend::setupHooks()
{
    HWND tempWindow = Utils::DXUtils::createTempWindow();
    if (!tempWindow) return false;

    ID3D12Device* tempDevice = nullptr;
    ID3D12CommandQueue* tempCommandQueue = nullptr;
    IDXGISwapChain3* tempSwapChain = nullptr;

    if (!Utils::DXUtils::createTempD3D12Device(tempWindow, &tempDevice, &tempCommandQueue, &tempSwapChain))
    {
        Utils::DXUtils::destroyTempWindow(tempWindow);
        return false;
    }

    // Get vtable function addresses
    void* presentAddr = getVTableFunction(tempSwapChain, 8);
    void* resizeBuffersAddr = getVTableFunction(tempSwapChain, 13);

    // Clean up
    tempSwapChain->Release();
    tempCommandQueue->Release();
    tempDevice->Release();
    Utils::DXUtils::destroyTempWindow(tempWindow);

    // Create hooks
    auto& hookManager = HookManager::getInstance();

    bool success = true;
    success &= hookManager.createHook(presentAddr, hookedPresent, &m_originalPresent);
    success &= hookManager.createHook(resizeBuffersAddr, hookedResizeBuffers, &m_originalResizeBuffers);

    return success;
}

void DX12Backend::setupWindowHook()
{
    if (m_window && !m_originalWndProc) 
        m_originalWndProc = (WNDPROC)SetWindowLongPtrW(m_window, GWLP_WNDPROC, (LONG_PTR)hookedWndProc);
}

void* DX12Backend::getVTableFunction(void* instance, int index)
{
    if (!instance) return nullptr;

    return (*static_cast<void***>(instance))[index];
}

HRESULT WINAPI DX12Backend::hookedPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags)
{
    static bool initialized = false;

    if (s_instance && !initialized)
    {
        // Cast to IDXGISwapChain3 for DX12
        if (SUCCEEDED(swapChain->QueryInterface(IID_PPV_ARGS(&s_instance->m_swapChain))))
        {
            if (s_instance->getDeviceAndContext())
            {
                DXGI_SWAP_CHAIN_DESC desc;
                swapChain->GetDesc(&desc);
                s_instance->m_window = desc.OutputWindow;

                // Hook window procedure after we have the real window
                s_instance->setupWindowHook();

                // Initialize ImGui
                s_instance->initializeImGui();
                initialized = true;
            }
        }
    }

    if (s_instance && s_instance->m_imguiInitialized)
    {
        s_instance->beginFrame();
        s_instance->renderImGui();
        s_instance->endFrame();
    }

    return m_originalPresent(swapChain, syncInterval, flags);
}

HRESULT WINAPI DX12Backend::hookedResizeBuffers(IDXGISwapChain* swapChain, UINT bufferCount,
                                                UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags)
{
    if (s_instance && s_instance->m_imguiInitialized) 
    {
        s_instance->waitForLastSubmittedFrame();
        s_instance->cleanupRenderTarget();
    }

    HRESULT result = m_originalResizeBuffers(swapChain, bufferCount, width, height, newFormat, swapChainFlags);

    if (s_instance && SUCCEEDED(result) && s_instance->m_imguiInitialized) 
        s_instance->createRenderTarget();

    return result;
}

bool DX12Backend::getDeviceAndContext()
{
    if (m_device) return true;

    if (!m_swapChain) return false;

    HRESULT hr = m_swapChain->GetDevice(IID_PPV_ARGS(&m_device));
    if (FAILED(hr)) return false;

    return createD3D12Resources();
}

bool DX12Backend::createD3D12Resources()
{
    // Create RTV descriptor heap
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = NUM_BACK_BUFFERS;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;
        if (FAILED(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_rtvDescHeap))))
            return false;

        SIZE_T rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        {
            m_renderTargetDescriptor[i] = rtvHandle;
            rtvHandle.ptr += rtvDescriptorSize;
        }
    }

    // Create SRV descriptor heap
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = SRV_HEAP_SIZE;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (FAILED(m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_srvDescHeap))))
            return false;
        m_srvDescHeapAlloc.Create(m_device, m_srvDescHeap);
    }

    // Create command queue
    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 1;
        if (FAILED(m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue))))
            return false;
    }

    // Create command allocators
    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_frameContext[i].CommandAllocator))))
            return false;

    // Create command list
    if (FAILED(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_frameContext[0].CommandAllocator, nullptr, IID_PPV_ARGS(&m_commandList))) ||
        FAILED(m_commandList->Close()))
        return false;

    // Create fence
    if (FAILED(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence))))
        return false;

    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr)
        return false;

    // Get waitable object from swap chain
    m_swapChainWaitableObject = m_swapChain->GetFrameLatencyWaitableObject();

    return createRenderTarget();
}

void DX12Backend::cleanupD3D12Resources()
{
    cleanupRenderTarget();

    if (m_swapChainWaitableObject != nullptr) 
    {
        CloseHandle(m_swapChainWaitableObject);
        m_swapChainWaitableObject = nullptr;
    }

    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (m_frameContext[i].CommandAllocator) 
        {
            m_frameContext[i].CommandAllocator->Release();
            m_frameContext[i].CommandAllocator = nullptr;
        }

    if (m_commandQueue) { m_commandQueue->Release(); m_commandQueue = nullptr; }
    if (m_commandList) { m_commandList->Release(); m_commandList = nullptr; }
    if (m_rtvDescHeap) { m_rtvDescHeap->Release(); m_rtvDescHeap = nullptr; }
    if (m_srvDescHeap) { m_srvDescHeap->Release(); m_srvDescHeap = nullptr; }
    if (m_fence) { m_fence->Release(); m_fence = nullptr; }
    if (m_fenceEvent) { CloseHandle(m_fenceEvent); m_fenceEvent = nullptr; }
    if (m_swapChain) { m_swapChain->Release(); m_swapChain = nullptr; }
    if (m_device) { m_device->Release(); m_device = nullptr; }

    m_srvDescHeapAlloc.Destroy();
}

bool DX12Backend::createRenderTarget()
{
    if (!m_swapChain || !m_device) return false;

    for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
    {
        ID3D12Resource* pBackBuffer = nullptr;
        HRESULT hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
        if (FAILED(hr) || !pBackBuffer) return false;

        m_device->CreateRenderTargetView(pBackBuffer, nullptr, m_renderTargetDescriptor[i]);
        m_renderTargetResource[i] = pBackBuffer;
    }

    return true;
}

void DX12Backend::cleanupRenderTarget()
{
    waitForLastSubmittedFrame();

    for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        if (m_renderTargetResource[i]) 
        {
            m_renderTargetResource[i]->Release();
            m_renderTargetResource[i] = nullptr;
        }
}

bool DX12Backend::initializeImGui()
{
    if (m_imguiInitialized || !m_device || !m_commandQueue || !m_window) return false;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;
    
    io.FontDefault = io.Fonts->AddFontFromMemoryCompressedTTF(
        HYWenHei_compressed_data, 
        *HYWenHei_compressed_data,
        15.0f,
        &fontConfig,
        io.Fonts->GetGlyphRangesChineseFull()
    );

    // Setup style
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(m_window);

    // Setup DX12 backend
    ImGui_ImplDX12_InitInfo init_info = {};
    init_info.Device = m_device;
    init_info.CommandQueue = m_commandQueue;
    init_info.NumFramesInFlight = NUM_FRAMES_IN_FLIGHT;
    init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
    init_info.SrvDescriptorHeap = m_srvDescHeap;
    init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) 
    { 
        return s_instance->m_srvDescHeapAlloc.Alloc(out_cpu_handle, out_gpu_handle); 
    };
    init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) 
    { 
        return s_instance->m_srvDescHeapAlloc.Free(cpu_handle, gpu_handle); 
    };

    if (!ImGui_ImplDX12_Init(&init_info))
        return false;

    m_imguiInitialized = true;
    return true;
}

void DX12Backend::shutdownImGui()
{
    if (!m_imguiInitialized) return;

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    m_imguiInitialized = false;
}

void DX12Backend::beginFrame()
{
    if (!m_imguiInitialized) return;

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void DX12Backend::endFrame()
{
    if (!m_imguiInitialized) return;

    ImGui::EndFrame();
    ImGui::Render();

    FrameContext* frameCtx = waitForNextFrameResources();
    UINT backBufferIdx = m_swapChain->GetCurrentBackBufferIndex();
    frameCtx->CommandAllocator->Reset();

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = m_renderTargetResource[backBufferIdx];
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    m_commandList->Reset(frameCtx->CommandAllocator, nullptr);
    m_commandList->ResourceBarrier(1, &barrier);

    // Clear render target and set up for rendering
    const float clear_color_with_alpha[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    m_commandList->ClearRenderTargetView(m_renderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, nullptr);
    m_commandList->OMSetRenderTargets(1, &m_renderTargetDescriptor[backBufferIdx], FALSE, nullptr);
    m_commandList->SetDescriptorHeaps(1, &m_srvDescHeap);

    // Render ImGui
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_commandList);

    // Transition back to present
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    m_commandList->ResourceBarrier(1, &barrier);
    m_commandList->Close();

    // Execute command list
    m_commandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&m_commandList);

    // Signal fence
    UINT64 fenceValue = m_fenceLastSignaledValue + 1;
    m_commandQueue->Signal(m_fence, fenceValue);
    m_fenceLastSignaledValue = fenceValue;
    frameCtx->FenceValue = fenceValue;
}

void DX12Backend::renderImGui()
{
    if (m_imguiInitialized)
    {
        GUI::getInstance().render();
    }
}

void DX12Backend::onResize(int width, int height)
{
    // DX12 resize is handled in the ResizeBuffers hook
}

int DX12Backend::onInput(UINT msg, WPARAM wParam, LPARAM lParam)
{
    ImGuiIO& io = ImGui::GetIO();

    auto isKeyboardMessage = [](UINT msg) -> bool
    {
        switch (msg)
        {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
            return true;
        default:
            return false;
        }
    };

    auto isInputMessage = [](UINT msg) -> bool
    {
        switch (msg)
        {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:

        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEWHEEL:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
            return true;

        default:
            return false;
        }
    };

    if (io.WantCaptureKeyboard && isKeyboardMessage(msg))
    {
        ImGui_ImplWin32_WndProcHandler(m_window, msg, wParam, lParam);
        return 1;
    }

    if (io.WantCaptureMouse && isInputMessage(msg))
    {
        ImGui_ImplWin32_WndProcHandler(m_window, msg, wParam, lParam);
        return 1;
    }

    return 0;
}

FrameContext* DX12Backend::waitForNextFrameResources()
{
    UINT nextFrameIndex = m_frameIndex + 1;
    m_frameIndex = nextFrameIndex;

    HANDLE waitableObjects[] = { m_swapChainWaitableObject, nullptr };
    DWORD numWaitableObjects = 1;

    FrameContext* frameCtx = &m_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue != 0) // means no fence was signaled
    {
        frameCtx->FenceValue = 0;
        m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent);
        waitableObjects[1] = m_fenceEvent;
        numWaitableObjects = 2;
    }

    WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

    return frameCtx;
}

void DX12Backend::waitForLastSubmittedFrame()
{
    FrameContext* frameCtx = &m_frameContext[m_frameIndex % NUM_FRAMES_IN_FLIGHT];

    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue == 0)
        return; // No fence was signaled

    frameCtx->FenceValue = 0;
    if (m_fence->GetCompletedValue() >= fenceValue)
        return;

    m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent);
    WaitForSingleObject(m_fenceEvent, INFINITE);
}
