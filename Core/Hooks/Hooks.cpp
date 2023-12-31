#include "../../Includes.h"

namespace direct_x_11_base
{
	hooks g_hooks;

	//dxgi.dll Function offsets
	//-----------------------------
	//To get those:
	//1. Open IDA
	//2. Put the "dxgi.dll" from "C:\Windows\System32" in IDA
	//3. IDA will complain that you need to store the idb somewhere else, DO IT!
	//4. IDA is going to prompt you to load extra DLLs, just click on cancel on those prompts
	//5. IDA should now ask, if you wanna load the pdb of the DLL as an addition, DO IT!
	//6. After the loading has finished, look for a function named "CDXGISwapChain::Present"
	//7. Take the Offset of the Function, in this example it is "0x1800015E0"
	//8. Now look for the imagebase in IDA by scrolling up till the top
	//9. You should find it in the first few entrys of the .text section, in this case the imagebase is 0x180000000
	//10. Now calculate the relative virtual address (also called "RVA") this goes followed: 0x1800015E0 - 0x180000000 = 0x15E0
	//11. So we going to do adjust out code to be "g_dxgi_base_address + 0x15E0"
	//12. Do the same with the function "CDXGISwapChain::ResizeBuffers"
	//13. And you are done finding them and updating them in the source

	//Note: Those Offsets might break after Windows Updates so be prepare to update those manually or find a way to automaticly update them, since signatures also break easily
	const static std::uintptr_t g_dxgi_base_address = reinterpret_cast<std::uintptr_t>(GetModuleHandleA("dxgi.dll"));
	const static std::uintptr_t g_offset_IDXGISwapChain_Present = g_dxgi_base_address + 0x15E0;	//sig "48 89 5C 24 10 48 89 74 24 20 55 57 41 56"
	const static std::uintptr_t g_offset_IDXGISwapChain_ResizeBuffers = g_dxgi_base_address + 0x22F40;//sig "48 8B C4 55 41 54 41 55 41 56 41 57 48 8D 68 B1 48 81 EC C0"

	// int64_t __fastcall IDXGISwapChain_Present(IDXGISwapChain* this, unsigned int SyncInterval, unsigned int Flags)
	using tIDXGISwapChain_Present = int64_t(__fastcall*)(IDXGISwapChain* this_ptr, unsigned int SyncInterval,
	                                                     unsigned int Flags);
	tIDXGISwapChain_Present o_IDXGISwapChain_Present = reinterpret_cast<tIDXGISwapChain_Present>(
		g_offset_IDXGISwapChain_Present);

	// int64_t __fastcall IDXGISwapChain::ResizeBuffers(IDXGISwapChain* this, unsigned int BufferCount, unsigned int Width, unsigned int Height, DXGI_FORMAT NewFormat, unsigned int SwapChainFlags)
	using tIDXGISwapChain_ResizeBuffers = int64_t(__fastcall*)(IDXGISwapChain* this_ptr, unsigned int BufferCount,
	                                                           unsigned int Width, unsigned int Height,
	                                                           DXGI_FORMAT NewFormat, unsigned int SwapChainFlags);
	tIDXGISwapChain_ResizeBuffers o_IDXGISwapChain_ResizeBuffers = reinterpret_cast<tIDXGISwapChain_ResizeBuffers>(
		g_offset_IDXGISwapChain_ResizeBuffers);

	//int64_t __fastcall ID3D11DeviceContext::DrawIndexed(ID3D11DeviceContext* pContext, unsigned int IndexCount,unsigned int StartIndexLocation,int  BaseVertexLocation)
	using tID3D11DeviceContext_DrawIndexed = int64_t(__fastcall*)(ID3D11DeviceContext* pContext,
	                                                              unsigned int IndexCount,
	                                                              unsigned int StartIndexLocation,
	                                                              int BaseVertexLocation);
	tID3D11DeviceContext_DrawIndexed oID3D11DeviceContext_DrawIndexed = nullptr;

	//int64_t __fastcall* ID3D11DeviceContext::DrawIndexedInstanced(ID3D11DeviceContext* pContext, unsigned int IndexCountPerInstance,unsigned int InstanceCount,unsigned int StartIndexLocation,int  BaseVertexLocation,unsigned int StartInstanceLocation)
	using tID3D11DeviceContext_DrawIndexedInstanced = int64_t(__fastcall*)(
		ID3D11DeviceContext* pContext, unsigned int IndexCountPerInstance, unsigned int InstanceCount,
		unsigned int StartIndexLocation, int BaseVertexLocation, unsigned int StartInstanceLocation);
	tID3D11DeviceContext_DrawIndexedInstanced oID3D11DeviceContext_DrawIndexedInstanced = nullptr;

	// Universal Wnd Process hook, should stop most game of draging your mouse to the middle of the screen or transmit keystrokes to the gamme
	LRESULT CALLBACK hooks::hk_wnd_proc(const HWND arg_hwnd, const unsigned int arg_umsg, const WPARAM arg_wparam,
	                                    const LPARAM arg_lparam)
	{
		// Declare a POINT structure to store mouse cursor position
		POINT l_pos;

		// Get the current cursor position in screen coordinates
		GetCursorPos(&l_pos);

		// Convert the screen coordinates to client coordinates of the game window 
		ScreenToClient(g_hooks.m_ptr_window, &l_pos);

		// Update the ImGui mouse cursor position based on the converted client coordinates
		ImGui::GetIO().MousePos.x = l_pos.x;
		ImGui::GetIO().MousePos.y = l_pos.y;

		// Check if ImGui wants to handle the Windows message (UI input)
		if (ImGui_ImplWin32_WndProcHandler(arg_hwnd, arg_umsg, arg_wparam, arg_lparam))
		{
			// ImGui handles the message, so return true to prevent further processing
			return true;
		}

		// Check if the menu is being shown and ignore certain messages
		if (g_hooks.m_is_menu_shown && arg_umsg != 0x8043 && arg_umsg != 0x8044)
		{
			// Menu is being shown, and the message is not one of the specified types, so return true to block it
			return true;
		}

		// If none of the above conditions apply, call the original window procedure to handle the message
		return CallWindowProc(g_hooks.o_wnd_proc_handler, arg_hwnd, arg_umsg, arg_wparam, arg_lparam);
	}

	int64_t __fastcall hooks::hk_ID3D11DeviceContext_DrawIndexedInstanced(
		ID3D11DeviceContext* arg_pContext, unsigned int arg_IndexCountPerInstance, unsigned int arg_InstanceCount,
		unsigned int arg_StartIndexLocation, int arg_BaseVertexLocation, unsigned int arg_StartInstanceLocation)
	{
		//can be used for video games cheat to do direct X chams incase DrawIndexed doesnt work

		return oID3D11DeviceContext_DrawIndexedInstanced(arg_pContext, arg_IndexCountPerInstance, arg_InstanceCount,
		                                                 arg_StartIndexLocation, arg_BaseVertexLocation,
		                                                 arg_StartInstanceLocation);
	}

	int64_t __fastcall hooks::hk_ID3D11DeviceContext_DrawIndexed(ID3D11DeviceContext* arg_pContext,
	                                                             unsigned int arg_IndexCount,
	                                                             unsigned int arg_StartIndexLocation,
	                                                             int arg_BaseVertexLocation)
	{
		//can be used to applied textures or shaders or other property changes to object in the application

		return oID3D11DeviceContext_DrawIndexed(arg_pContext, arg_IndexCount, arg_StartIndexLocation,
		                                        arg_BaseVertexLocation);
	}

	int64_t __fastcall hooks::hk_idxgi_swap_chain_resize_buffers(IDXGISwapChain* arg_p_swapchain,
	                                                             const unsigned int arg_buffer_count,
	                                                             const unsigned int arg_width,
	                                                             const unsigned int arg_height,
	                                                             const DXGI_FORMAT arg_new_format,
	                                                             const unsigned int arg_swap_chain_flags)
	{
		// Invalidate ImGui's device objects to prepare for resizing
		ImGui_ImplDX11_InvalidateDeviceObjects();

		// Check if a render target view exists and release it
		if (nullptr != g_hooks.m_render_target_view)
		{
			g_hooks.m_render_target_view->Release();
			g_hooks.m_render_target_view = nullptr;
		}

		// Call the original IDXGISwapChain::ResizeBuffers function and store its return value
		const int64_t r_result = o_IDXGISwapChain_ResizeBuffers(arg_p_swapchain, arg_buffer_count, arg_width,
		                                                        arg_height, arg_new_format,
		                                                        arg_swap_chain_flags);

		// Recreate ImGui's device objects after resizing
		ImGui_ImplDX11_CreateDeviceObjects();

		// Return the result of the original IDXGISwapChain::ResizeBuffers
		return r_result;
	}

	int64_t __fastcall hooks::hk_idxgi_swap_chain_present(IDXGISwapChain* arg_p_swapchain,
	                                                      const unsigned int arg_sync_interval,
	                                                      const unsigned int arg_flags)
	{
		// Check if ImGui is initialized
		if (!g_hooks.m_is_imgui_initialized)
		{
			g_hooks.m_is_imgui_initialized = true; // Only initialize ImGui once

			// Get the device associated with the swap chain
			if (SUCCEEDED(
				arg_p_swapchain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&g_hooks.m_ptr_device))))
			{
				// Gets the device pointer of the swap chain for later usage
				arg_p_swapchain->GetDevice(__uuidof(g_hooks.m_ptr_device),
				                           reinterpret_cast<void**>(&g_hooks.m_ptr_device));
				std::cout << g_hooks.DEVICE_CONSOLE_TEXT << std::format(
						"{:#x}", reinterpret_cast<std::uintptr_t>(g_hooks.m_ptr_device))
					<< std::endl;

				//Get pointer of te context of the swap chain
				g_hooks.m_ptr_device->GetImmediateContext(&g_hooks.m_ptr_context);
				std::cout << g_hooks.CONTEXT_CONSOLE_TEXT << std::format(
					"{:#x}", reinterpret_cast<std::uintptr_t>(g_hooks.m_ptr_context)) << std::endl;

				//This part gives us additional Funktion addresses which we going to hook
				auto p_device_context_v_table = reinterpret_cast<std::uintptr_t*>(g_hooks.m_ptr_context);
				p_device_context_v_table = reinterpret_cast<std::uintptr_t*>(p_device_context_v_table[0]);

				oID3D11DeviceContext_DrawIndexed = reinterpret_cast<tID3D11DeviceContext_DrawIndexed>(
					p_device_context_v_table[12]);
				oID3D11DeviceContext_DrawIndexedInstanced = reinterpret_cast<tID3D11DeviceContext_DrawIndexedInstanced>(
					p_device_context_v_table[20]);

				std::cout << g_hooks.ID3D11DEVICECONTEXT_DRAWINDEXED_CONSOLE_TEXT << std::format(
					"{:#x}", reinterpret_cast<std::uintptr_t>(oID3D11DeviceContext_DrawIndexed)) << std::endl;
				std::cout << g_hooks.ID3D11DEVICECONTEXT_DRAWINDEXEDINSTANCED_CONSOLE_TEXT << std::format(
					"{:#x}", reinterpret_cast<std::uintptr_t>(oID3D11DeviceContext_DrawIndexedInstanced)) << std::endl;

				ATTACH_HOOK(oID3D11DeviceContext_DrawIndexed,
				            direct_x_11_base::hooks::hk_ID3D11DeviceContext_DrawIndexed);
				ATTACH_HOOK(oID3D11DeviceContext_DrawIndexedInstanced,
				            direct_x_11_base::hooks::hk_ID3D11DeviceContext_DrawIndexedInstanced);
			}

			// Create an ImGui context
			DXGI_SWAP_CHAIN_DESC swap_chain_desc;
			arg_p_swapchain->GetDesc(&swap_chain_desc);

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			(void)io;
			ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;

			// Enable keyboard controls for ImGui
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

			// Get the window associated with the swap chain
			g_hooks.m_ptr_window = swap_chain_desc.OutputWindow;

			// Set a custom window procedure handler for ImGui
			g_hooks.o_wnd_proc_handler = reinterpret_cast<WNDPROC>(SetWindowLongPtr(
				g_hooks.m_ptr_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hk_wnd_proc)));

			// Initialize ImGui for Win and DirectX 11
			ImGui_ImplWin32_Init(g_hooks.m_ptr_window);
			ImGui_ImplDX11_Init(g_hooks.m_ptr_device, g_hooks.m_ptr_context);
			ImGui::GetIO().ImeWindowHandle = g_hooks.m_ptr_window;

			// Creates depth stencil state for graphics rendering
			D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
			depth_stencil_desc.DepthEnable = TRUE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			depth_stencil_desc.StencilEnable = FALSE;
			depth_stencil_desc.StencilReadMask = 0xFF;
			depth_stencil_desc.StencilWriteMask = 0xFF;

			// Stencil operations if pixel is front-facing
			depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			// Stencil operations if pixel is back-facing
			depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			g_hooks.m_ptr_device->CreateDepthStencilState(&depth_stencil_desc, &g_hooks.m_depth_stencil_state_false);

			// Creates depth stencil state for graphics rendering
			D3D11_RASTERIZER_DESC rasterizer_desc;
			ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
			rasterizer_desc.FillMode = D3D11_FILL_SOLID;
			rasterizer_desc.CullMode = D3D11_CULL_NONE;
			rasterizer_desc.FrontCounterClockwise = false;
			constexpr float bias = 1000.0f;
			float bias_float = -bias;
			bias_float /= 10000.0f;
			rasterizer_desc.DepthBias = DEPTH_BIAS_D32_FLOAT(*reinterpret_cast<DWORD*>(&bias_float));
			rasterizer_desc.SlopeScaledDepthBias = 0.0f;
			rasterizer_desc.DepthBiasClamp = 0.0f;
			rasterizer_desc.DepthClipEnable = true;
			rasterizer_desc.ScissorEnable = false;
			rasterizer_desc.MultisampleEnable = false;
			rasterizer_desc.AntialiasedLineEnable = false;
			g_hooks.m_ptr_device->CreateRasterizerState(&rasterizer_desc, &g_hooks.m_depth_bias_state_false);

			// Creates normal rasterizer state for graphics rendering
			D3D11_RASTERIZER_DESC normal_rasterizer_desc;
			ZeroMemory(&normal_rasterizer_desc, sizeof(normal_rasterizer_desc));
			normal_rasterizer_desc.FillMode = D3D11_FILL_SOLID;
			normal_rasterizer_desc.CullMode = D3D11_CULL_NONE;
			normal_rasterizer_desc.FrontCounterClockwise = false;
			normal_rasterizer_desc.DepthBias = 0.0f;
			normal_rasterizer_desc.SlopeScaledDepthBias = 0.0f;
			normal_rasterizer_desc.DepthBiasClamp = 0.0f;
			normal_rasterizer_desc.DepthClipEnable = true;
			normal_rasterizer_desc.ScissorEnable = false;
			normal_rasterizer_desc.MultisampleEnable = false;
			normal_rasterizer_desc.AntialiasedLineEnable = false;

			g_hooks.m_ptr_device->CreateRasterizerState(&normal_rasterizer_desc, &g_hooks.m_depth_bias_state_true);
		}

		if (g_hooks.m_render_target_view == nullptr)
		{
			// Get viewport information
			g_hooks.m_ptr_context->RSGetViewports(&hooks::g_num_of_view_ports, &g_hooks.m_viewport);
			g_hooks.m_screen_center_x = g_hooks.m_viewport.Width / 2.0f;
			g_hooks.m_screen_center_y = g_hooks.m_viewport.Height / 2.0f;

			// Gets the back buffer for rendering
			ID3D11Texture2D* back_buffer = nullptr;
			g_hooks.m_h_result_code = arg_p_swapchain->
				GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));
			if (FAILED(g_hooks.m_h_result_code))
			{
				printf("%s\n", g_hooks.FAILED_TO_GET_BACK_BUFFER_CONSOLE_TEXT.c_str());
				return g_hooks.m_h_result_code;
			}

			// Creates a render target view for the back buffer
			g_hooks.m_h_result_code = g_hooks.m_ptr_device->CreateRenderTargetView(
				back_buffer, nullptr, &g_hooks.m_render_target_view);
			back_buffer->Release();
			if (FAILED(g_hooks.m_h_result_code))
			{
				printf("%s\n", g_hooks.FAILED_TO_GET_RENDER_TARGET_CONSOLE_TEXT.c_str());
				return g_hooks.m_h_result_code;
			}
		}
		else
		{
			// Set the render target view before drawing
			g_hooks.m_ptr_context->OMSetRenderTargets(1, &g_hooks.m_render_target_view, nullptr);
		}

		// Prepare for a new frame with ImGui
		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();

		// Toggle menu visibility with the INSERT key
		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			g_hooks.m_is_menu_shown = !g_hooks.m_is_menu_shown;
		}

		if (g_hooks.m_is_menu_shown)
		{
			ImGui::GetIO().MouseDrawCursor = true;

			// DRAW YOUR MENU STUFF HERE, WE USE THE DEMO WINDOW AS A EXAMPLE HERE
			bool test = true;
			ImGui::ShowDemoWindow(&test);
		}
		else
		{
			// Stops to the Mouse Cursor to be drawn
			ImGui::GetIO().MouseDrawCursor = false;
		}

		// Handle the F1 key press to unhook Direct X Functions and wnd process
		if (GetAsyncKeyState(VK_F1) & 1)
		{
			unhook();
		}

		// Finish the ImGui frame and render ImGui
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Call the original IDXGISwapChain::Present function and return its result
		return o_IDXGISwapChain_Present(arg_p_swapchain, arg_sync_interval, arg_flags);
	}

	// Sets needed hooks here
	void hooks::init_hooks()
	{
		std::cout << g_hooks.DXGI_IMAGE_BASE_CONSOLE_TEXT << std::format("{:#x}", g_dxgi_base_address) << std::endl;
		std::cout << g_hooks.IDXGI_SWAPCHAIN_PRESENT_CONSOLE_TEXT << std::format(
			"{:#x}", g_offset_IDXGISwapChain_Present) << std::endl;
		std::cout << g_hooks.IDXGI_SWAPCHAIN_RESIZEBUFFERS_CONSOLE_TEXT << std::format(
			"{:#x}", g_offset_IDXGISwapChain_ResizeBuffers) << std::endl;

		ATTACH_HOOK(o_IDXGISwapChain_Present, direct_x_11_base::hooks::hk_idxgi_swap_chain_present);
		ATTACH_HOOK(o_IDXGISwapChain_ResizeBuffers, direct_x_11_base::hooks::hk_idxgi_swap_chain_resize_buffers);
	}

	// Removes created hooks
	void hooks::unhook()
	{
		DETACH_HOOK(o_IDXGISwapChain_Present, direct_x_11_base::hooks::hk_idxgi_swap_chain_present);
		DETACH_HOOK(o_IDXGISwapChain_ResizeBuffers, direct_x_11_base::hooks::hk_idxgi_swap_chain_resize_buffers);
		DETACH_HOOK(g_hooks.o_wnd_proc_handler, hk_wnd_proc);
	}
}
