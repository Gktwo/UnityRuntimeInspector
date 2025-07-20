#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <imgui.h>

#include "renderer_backend.h"

// Frame context for DX12
struct FrameContext
{
    ID3D12CommandAllocator* CommandAllocator;
    UINT64 FenceValue;
};

// Simple descriptor heap allocator for SRV descriptors
struct DescriptorHeapAllocator
{
    ID3D12DescriptorHeap* Heap = nullptr;
    D3D12_DESCRIPTOR_HEAP_TYPE HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
    D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu;
    D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu;
    UINT HeapHandleIncrement;
    ImVector<int> FreeIndices;

    void Create(ID3D12Device* device, ID3D12DescriptorHeap* heap);
    void Destroy();
    void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle);
    void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle);
};

class DX12Backend : public IRendererBackend
{
public:
    DX12Backend();
    ~DX12Backend() override;

    bool initialize() override;
    void shutdown() override;
    bool isInitialized() const override { return m_initialized; }
    const char* getName() const override { return "DirectX 12"; }
    void onResize(int width, int height) override;
    int onInput(UINT msg, WPARAM wParam, LPARAM lParam) override;

protected:
    void renderImGui() override;
    void beginFrame() override;
    void endFrame() override;
    bool initializeImGui() override;
    void shutdownImGui() override;

private:
    // Constants
    static const int NUM_FRAMES_IN_FLIGHT = 3;
    static const int NUM_BACK_BUFFERS = 3;
    static const int SRV_HEAP_SIZE = 64;

    // Hook functions
    static HRESULT WINAPI hookedPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags);
    static HRESULT WINAPI hookedResizeBuffers(IDXGISwapChain* swapChain, UINT bufferCount,
                                              UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);

    // Original function pointers
    typedef HRESULT(WINAPI* Present_t)(IDXGISwapChain*, UINT, UINT);
    typedef HRESULT(WINAPI* ResizeBuffers_t)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

    static Present_t m_originalPresent;
    static ResizeBuffers_t m_originalResizeBuffers;

    // Instance management
    static DX12Backend* s_instance;

    // State
    bool m_initialized;
    bool m_imguiInitialized;
    HWND m_window;

    // DX12 Resources
    ID3D12Device* m_device;
    ID3D12DescriptorHeap* m_rtvDescHeap;
    ID3D12DescriptorHeap* m_srvDescHeap;
    DescriptorHeapAllocator m_srvDescHeapAlloc;
    ID3D12CommandQueue* m_commandQueue;
    ID3D12GraphicsCommandList* m_commandList;
    ID3D12Fence* m_fence;
    HANDLE m_fenceEvent;
    UINT64 m_fenceLastSignaledValue;
    IDXGISwapChain3* m_swapChain;
    bool m_swapChainOccluded;
    HANDLE m_swapChainWaitableObject;
    ID3D12Resource* m_renderTargetResource[NUM_BACK_BUFFERS];
    D3D12_CPU_DESCRIPTOR_HANDLE m_renderTargetDescriptor[NUM_BACK_BUFFERS];

    // Frame contexts
    FrameContext m_frameContext[NUM_FRAMES_IN_FLIGHT];
    UINT m_frameIndex;

    // Window procedure hook
    static WNDPROC m_originalWndProc;
    static LRESULT CALLBACK hookedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Helper functions
    bool setupHooks();
    void setupWindowHook();
    void* getVTableFunction(void* instance, int index);
    bool createRenderTarget();
    void cleanupRenderTarget();
    bool getDeviceAndContext();
    bool createD3D12Resources();
    void cleanupD3D12Resources();
    FrameContext* waitForNextFrameResources();
    void waitForLastSubmittedFrame();
};
