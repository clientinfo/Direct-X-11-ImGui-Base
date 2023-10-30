#pragma once

namespace direct_x_11_base
{
	
	class hooks
	{

	public:

		//Macro to simplify Function Hooking/Unhooking
		#define ATTACH_HOOK(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourAttach((LPVOID*)&(original), (LPVOID)(hook)), DetourTransactionCommit())
		#define DETACH_HOOK(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourDetach((LPVOID*)&(original), (LPVOID)(hook)), DetourTransactionCommit())

		//String variables for console text
		inline static std::string DXGI_IMAGE_BASE_CONSOLE_TEXT = "dxgi.dll Imagebase: ";
		inline static std::string IDXGI_SWAPCHAIN_PRESENT_CONSOLE_TEXT = "IDXGISwapChain::Present: ";
		inline static std::string IDXGI_SWAPCHAIN_RESIZEBUFFERS_CONSOLE_TEXT = "IDXGISwapChain::ResizeBuffers: ";
		inline static std::string DEVICE_CONSOLE_TEXT = "Device: ";
		inline static std::string CONTEXT_CONSOLE_TEXT = "Context: ";
		inline static std::string FAILED_TO_GET_BACK_BUFFER_CONSOLE_TEXT = "Failed to get back_buffer";
		inline static std::string FAILED_TO_GET_RENDER_TARGET_CONSOLE_TEXT = "Failed to get RenderTarget";

		//Variable for wnd proc hook
		inline static WNDPROC o_wnd_proc_handler = nullptr;

		inline static bool show_menu = false;

		static void init_hooks();
		static void unhook();

	protected:
		static int64_t __fastcall hk_idxgi_swap_chain_present(IDXGISwapChain* this_ptr, unsigned int sync_interval, unsigned int flags);
		static int64_t __fastcall hk_idxgi_swap_chain_resize_buffers(IDXGISwapChain* this_ptr, unsigned int buffer_count, unsigned int width, unsigned int height, DXGI_FORMAT new_format, unsigned int swap_chain_flags);
		static LRESULT CALLBACK hk_wnd_proc(const HWND h_wnd, const UINT u_msg, const WPARAM w_param, const LPARAM l_param);
		

	} extern _hooks;
}