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
		static int64_t __fastcall hk_idxgi_swap_chain_present(IDXGISwapChain* this_ptr, unsigned int sync_interval, unsigned int flags);
		static int64_t __fastcall hk_idxgi_swap_chain_resize_buffers(IDXGISwapChain* this_ptr, unsigned int buffer_count, unsigned int width, unsigned int height, DXGI_FORMAT new_format, unsigned int swap_chain_flags);
		static LRESULT CALLBACK hk_wnd_proc(const HWND h_wnd, const UINT u_msg, const WPARAM w_param, const LPARAM l_param);
		static int64_t __fastcall hk_ID3D11DeviceContext_DrawIndexed(ID3D11DeviceContext* pContext, unsigned int IndexCount, unsigned int StartIndexLocation, int BaseVertexLocation);
		static int64_t __fastcall hk_ID3D11DeviceContext_DrawIndexedInstanced(ID3D11DeviceContext* pContext, unsigned int IndexCountPerInstance, unsigned int InstanceCount, unsigned int StartIndexLocation, int  BaseVertexLocation, unsigned int StartInstanceLocation);
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
		std::string ID3D11DEVICECONTEXT_DRAWINDEXEDINSTANCED_CONSOLE_TEXT = "[+] ID3D11DeviceContext::DrawIndexedInstanced: ";
		std::string FAILED_TO_GET_BACK_BUFFER_CONSOLE_TEXT = "[-] Failed to get back_buffer";
		std::string FAILED_TO_GET_RENDER_TARGET_CONSOLE_TEXT = "[-] Failed to get RenderTarget";

		//Variable for wnd proc hook
		WNDPROC o_wnd_proc_handler = nullptr;

		// A boolean flag that indicates whether ImGui has been initialized. It's initially set to false.
		bool g_is_imgui_initialized = false;

		// A boolean flag that indicated whether ImGui Menu is getting shown or not. It's initially set to false
		bool show_menu = false;

		// Variables to store the center coordinates of the screen
		float g_screen_center_x = 0;
		float g_screen_center_y = 0;

		//Variable to store the device pointer of the ID3D11Device of the swap chain
		ID3D11Device* g_ptr_device = nullptr;

		//Variable to store the context pointer of the ID3D11DeviceContext of the swap chain
		ID3D11DeviceContext* g_ptr_context = nullptr;

		// HWND (Window Handle): A handle to the application's window, initialized to nullptr.
		HWND g_window = nullptr;

		// Represents a DirectX 11 depth stencil state with depth testing turned off.
		// It's initially set to nullptr.
		ID3D11DepthStencilState* g_depth_stencil_state_false = nullptr;

		// Represents the original/default DirectX 11 depth stencil state, which typically has depth testing enabled.
		// It's initially set to nullptr.
		ID3D11DepthStencilState* g_depth_stencil_state_orig = nullptr;

		// Represents a custom DirectX 11 rasterizer state with depth bias settings to false.
		// Used for certain rendering effects.
		ID3D11RasterizerState* g_depth_bias_state_false;

		// Represents a custom DirectX 11 rasterizer state with depth bias settings to true.
		// Used for certain rendering effects.
		ID3D11RasterizerState* g_depth_bias_state_true;

		// Represents the original/default DirectX 11 rasterizer state, which does not apply depth bias.
		// Used for certain rendering effects.
		ID3D11RasterizerState* g_depth_bias_state_orig;

		// Represents a view that allows rendering to a target, like a back buffer in DirectX.
		// It's initially set to nullptr.
		ID3D11RenderTargetView* g_render_target_view = nullptr;

		// Represents a DirectX 11 viewport configuration that defines the area of the render target to draw into.
		D3D11_VIEWPORT g_viewport;

		// An int64_t variable used to store HRESULT error codes.
		// HRESULT is a standard error code in Windows programming.
		int64_t g_h_result_code;


	} extern _hooks;
}