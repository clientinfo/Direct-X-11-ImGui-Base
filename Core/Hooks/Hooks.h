#pragma once

namespace direct_x_11_base
{
	class hooks
	{
	public:
		//Macro to simplify Function Hooking/Unhooking
#define ATTACH_HOOK(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourAttach((LPVOID*)&(original), (LPVOID)(hook)), DetourTransactionCommit())
#define DETACH_HOOK(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourDetach((LPVOID*)&(original), (LPVOID)(hook)), DetourTransactionCommit())

		// This is a macro that calculates a depth bias value for DirectX 32-bit floating-point format.
		// It's used to convert depth bias from a float to a format compatible with DXGI.
#define DEPTH_BIAS_D32_FLOAT(d) (d/(1/pow(2,23)))

		static void init_hooks();
		static void unhook();

	protected:
		static int64_t __fastcall hk_idxgi_swap_chain_present(IDXGISwapChain* arg_p_swapchain,
		                                                      unsigned int arg_sync_interval, unsigned int arg_flags);
		static int64_t __fastcall hk_idxgi_swap_chain_resize_buffers(IDXGISwapChain* arg_p_swapchain,
		                                                             unsigned int arg_buffer_count,
		                                                             unsigned int arg_width, unsigned int arg_height,
		                                                             DXGI_FORMAT arg_new_format,
		                                                             unsigned int arg_swap_chain_flags);
		static LRESULT CALLBACK hk_wnd_proc(const HWND arg_hwnd, const UINT arg_umsg, const WPARAM arg_wparam,
		                                    const LPARAM arg_lparam);
		static int64_t __fastcall hk_ID3D11DeviceContext_DrawIndexed(ID3D11DeviceContext* arg_pContext,
		                                                             unsigned int arg_IndexCount,
		                                                             unsigned int arg_StartIndexLocation,
		                                                             int arg_BaseVertexLocation);
		static int64_t __fastcall hk_ID3D11DeviceContext_DrawIndexedInstanced(
			ID3D11DeviceContext* arg_pContext, unsigned int arg_IndexCountPerInstance, unsigned int arg_InstanceCount,
			unsigned int arg_StartIndexLocation, int arg_BaseVertexLocation, unsigned int arg_StartInstanceLocation);

	private:
		// Number of viewports being used
		inline static unsigned int g_num_of_view_ports = 1;

		//String variables for console text
		std::string DXGI_IMAGE_BASE_CONSOLE_TEXT = "[+] dxgi.dll Imagebase: ";
		std::string IDXGI_SWAPCHAIN_PRESENT_CONSOLE_TEXT = "[+] IDXGISwapChain::Present: ";
		std::string IDXGI_SWAPCHAIN_RESIZEBUFFERS_CONSOLE_TEXT = "[+] IDXGISwapChain::ResizeBuffers: ";
		std::string DEVICE_CONSOLE_TEXT = "[+] Device: ";
		std::string CONTEXT_CONSOLE_TEXT = "[+] Context: ";
		std::string ID3D11DEVICECONTEXT_DRAWINDEXED_CONSOLE_TEXT = "[+] ID3D11DeviceContext::DrawIndexed: ";
		std::string ID3D11DEVICECONTEXT_DRAWINDEXEDINSTANCED_CONSOLE_TEXT =
			"[+] ID3D11DeviceContext::DrawIndexedInstanced: ";
		std::string FAILED_TO_GET_BACK_BUFFER_CONSOLE_TEXT = "[-] Failed to get back_buffer";
		std::string FAILED_TO_GET_RENDER_TARGET_CONSOLE_TEXT = "[-] Failed to get RenderTarget";

		//Variable for wnd proc hook
		WNDPROC o_wnd_proc_handler = nullptr;

		// A boolean flag that indicates whether ImGui has been initialized. It's initially set to false.
		bool m_is_imgui_initialized = false;

		// A boolean flag that indicated whether ImGui Menu is getting shown or not. It's initially set to false
		bool m_is_menu_shown = false;

		// Variables to store the center coordinates of the screen
		float m_screen_center_x = 0;
		float m_screen_center_y = 0;

		//Variable to store the device pointer of the ID3D11Device of the swap chain
		ID3D11Device* m_ptr_device = nullptr;

		//Variable to store the context pointer of the ID3D11DeviceContext of the swap chain
		ID3D11DeviceContext* m_ptr_context = nullptr;

		// HWND (Window Handle): A handle to the application's window, initialized to nullptr.
		HWND m_ptr_window = nullptr;

		// Represents a DirectX 11 depth stencil state with depth testing turned off.
		// It's initially set to nullptr.
		ID3D11DepthStencilState* m_depth_stencil_state_false = nullptr;

		// Represents the original/default DirectX 11 depth stencil state, which typically has depth testing enabled.
		// It's initially set to nullptr.
		ID3D11DepthStencilState* m_depth_stencil_state_orig = nullptr;

		// Represents a custom DirectX 11 rasterizer state with depth bias settings to false.
		// Used for certain rendering effects.
		ID3D11RasterizerState* m_depth_bias_state_false = nullptr;

		// Represents a custom DirectX 11 rasterizer state with depth bias settings to true.
		// Used for certain rendering effects.
		ID3D11RasterizerState* m_depth_bias_state_true = nullptr;

		// Represents the original/default DirectX 11 rasterizer state, which does not apply depth bias.
		// Used for certain rendering effects.
		ID3D11RasterizerState* m_depth_bias_state_orig = nullptr;

		// Represents a view that allows rendering to a target, like a back buffer in DirectX.
		// It's initially set to nullptr.
		ID3D11RenderTargetView* m_render_target_view = nullptr;

		// Represents a DirectX 11 viewport configuration that defines the area of the render target to draw into.
		D3D11_VIEWPORT m_viewport = {};

		// An int64_t variable used to store HRESULT error codes.
		// HRESULT is a standard error code in Windows programming.
		int64_t m_h_result_code = 0;
	} extern g_hooks;
}
