#pragma once

namespace direct_x_11_base
{
	
	class hooks
	{

	public:

		#define ATTACH_HOOK(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourAttach((LPVOID*)&(original), (LPVOID)(hook)), DetourTransactionCommit())
		#define DETACH_HOOK(original, hook) (DetourTransactionBegin(), DetourUpdateThread(GetCurrentThread()), DetourDetach((LPVOID*)&(original), (LPVOID)(hook)), DetourTransactionCommit())

		static void init_hooks();

		inline static bool show_menu = false;

		static void unhook();

	protected:
		static int64_t __fastcall hk_idxgi_swap_chain_present(IDXGISwapChain* this_ptr, unsigned int sync_interval, unsigned int flags);
		static int64_t __fastcall hk_idxgi_swap_chain_resize_buffers(IDXGISwapChain* this_ptr, unsigned int buffer_count, unsigned int width, unsigned int height, DXGI_FORMAT new_format, unsigned int swap_chain_flags);
		static LRESULT CALLBACK hk_wnd_proc(const HWND h_wnd, const UINT u_msg, const WPARAM w_param, const LPARAM l_param);
		

	} extern _hooks;
}