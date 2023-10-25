#include "../../Includes.h"

namespace direct_x_11_base
{
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
	const static std::uintptr_t g_offset_IDXGISwapChain_Present = g_dxgi_base_address + 0x15E0; //sig "48 89 5C 24 10 48 89 74 24 20 55 57 41 56"
	const static std::uintptr_t g_offset_IDXGISwapChain_ResizeBuffers = g_dxgi_base_address + 0x22F40; //sig "48 8B C4 55 41 54 41 55 41 56 41 57 48 8D 68 B1 48 81 EC C0"

	std::string DXGI_IMAGE_BASE_CONSOLE_TEXT = "dxgi.dll Imagebase: ";
	std::string IDXGI_SWAPCHAIN_PRESENT_CONSOLE_TEXT = "IDXGISwapChain::Present: ";
	std::string IDXGI_SWAPCHAIN_RESIZEBUFFERS_CONSOLE_TEXT = "IDXGISwapChain::ResizeBuffers: ";
	std::string DEVICE_CONSOLE_TEXT = "Device: ";
	std::string CONTEXT_CONSOLE_TEXT = "Context: ";
	std::string FAILED_TO_GET_BACK_BUFFER_CONSOLE_TEXT = "Failed to get back_buffer";
	std::string FAILED_TO_GET_RENDER_TARGET_CONSOLE_TEXT= "Failed to get RenderTarget";

	//Variable for wnd proc hook
	WNDPROC o_wnd_proc_handler = nullptr;

	//Variable to store the device pointer of the ID3D11Device of the swap chain
	ID3D11Device* ptr_device = nullptr;

	//Variable to store the context pointer of the ID3D11DeviceContext of the swap chain
	ID3D11DeviceContext* ptr_context = nullptr;

	// HWND (Window Handle): A handle to the application's window, initialized to nullptr.
	HWND window = nullptr;

	// Represents a DirectX 11 depth stencil state with depth testing turned off.
	// It's initially set to nullptr.
	ID3D11DepthStencilState* depth_stencil_state_false = nullptr;

	// Represents the original/default DirectX 11 depth stencil state, which typically has depth testing enabled.
	// It's initially set to nullptr.
	ID3D11DepthStencilState* depth_stencil_state_orig = nullptr;

	// Represents a custom DirectX 11 rasterizer state with depth bias settings to false.
	// Used for certain rendering effects.
	ID3D11RasterizerState* depth_bias_state_false;

	// Represents a custom DirectX 11 rasterizer state with depth bias settings to true.
	// Used for certain rendering effects.
	ID3D11RasterizerState* depth_bias_state_true;

	// Represents the original/default DirectX 11 rasterizer state, which does not apply depth bias.
	// Used for certain rendering effects.
	ID3D11RasterizerState* depth_bias_state_orig;

	// Represents a view that allows rendering to a target, like a back buffer in DirectX.
	// It's initially set to nullptr.
	ID3D11RenderTargetView* render_target_view = nullptr;

	// Represents a DirectX 11 viewport configuration that defines the area of the render target to draw into.
	D3D11_VIEWPORT viewport;

	// Variables to store the center coordinates of the screen
	float screen_center_x;
	float screen_center_y;

	// Number of viewports being used
	unsigned int num_of_view_ports = 1;

	// An int64_t variable used to store HRESULT error codes.
	// HRESULT is a standard error code in Windows programming.
	int64_t h_result_code;

	// A boolean flag that indicates whether ImGui has been initialized. It's initially set to false.
	bool is_imgui_initialized = false;

	// This is a macro that calculates a depth bias value for DirectX 32-bit floating-point format.
	// It's used to convert depth bias from a float to a format compatible with DXGI.
	#define DEPTH_BIAS_D32_FLOAT(d) (d/(1/pow(2,23)))
	
	// int64_t __fastcall IDXGISwapChain_Present(IDXGISwapChain* this, unsigned int SyncInterval, unsigned int Flags)
	using tIDXGISwapChain_Present = int64_t(__fastcall*)(IDXGISwapChain* this_ptr, unsigned int SyncInterval,unsigned int Flags);
	tIDXGISwapChain_Present o_IDXGISwapChain_Present = reinterpret_cast<tIDXGISwapChain_Present>(g_offset_IDXGISwapChain_Present);

	// int64_t __fastcall IDXGISwapChain::ResizeBuffers(IDXGISwapChain* this, unsigned int BufferCount, unsigned int Width, unsigned int Height, DXGI_FORMAT NewFormat, unsigned int SwapChainFlags)
	using tIDXGISwapChain_ResizeBuffers = int64_t(__fastcall*)(IDXGISwapChain* this_ptr, unsigned int BufferCount,unsigned int Width, unsigned int Height,DXGI_FORMAT NewFormat, unsigned int SwapChainFlags);
	tIDXGISwapChain_ResizeBuffers o_IDXGISwapChain_ResizeBuffers = reinterpret_cast<tIDXGISwapChain_ResizeBuffers>(g_offset_IDXGISwapChain_ResizeBuffers);

	// Universal Wnd Process hook, should stop most game of draging your mouse to the middle of the screen or transmit keystrokes to the gamme
	LRESULT CALLBACK hooks::hk_wnd_proc(const HWND h_wnd, const UINT u_msg, const WPARAM w_param, const LPARAM l_param)
	{
		// Get a reference to the ImGui input/output (IO) structure
		ImGuiIO& io = ImGui::GetIO();

		// Declare a POINT structure to store mouse cursor position
		POINT m_pos;

		// Get the current cursor position in screen coordinates
		GetCursorPos(&m_pos);

		// Convert the screen coordinates to client coordinates of the game window 
		ScreenToClient(window, &m_pos);

		// Update the ImGui mouse cursor position based on the converted client coordinates
		ImGui::GetIO().MousePos.x = m_pos.x;
		ImGui::GetIO().MousePos.y = m_pos.y;		

		// Check if ImGui wants to handle the Windows message (UI input)
		if (ImGui_ImplWin32_WndProcHandler(h_wnd, u_msg, w_param, l_param))
		{
			// ImGui handles the message, so return true to prevent further processing
			return true;
		}

		// Check if the menu is being shown and ignore certain messages
		if (show_menu && u_msg != 0x8043 && u_msg != 0x8044)
		{
			// Menu is being shown, and the message is not one of the specified types, so return true to block it
			return true;
		}

		// If none of the above conditions apply, call the original window procedure to handle the message
		return CallWindowProc(o_wnd_proc_handler, h_wnd, u_msg, w_param, l_param);
	}

	int64_t __fastcall hooks::hk_idxgi_swap_chain_resize_buffers(IDXGISwapChain* this_ptr, const unsigned int buffer_count, const unsigned int width, const unsigned int height,const DXGI_FORMAT new_format, const unsigned int swap_chain_flags)
	{
		// Invalidate ImGui's device objects to prepare for resizing
		ImGui_ImplDX11_InvalidateDeviceObjects();

		// Check if a render target view exists and release it
		if (nullptr != render_target_view)
		{
			render_target_view->Release();
			render_target_view = nullptr;
		}

		// Call the original IDXGISwapChain::ResizeBuffers function and store its return value
		const int64_t r_result = o_IDXGISwapChain_ResizeBuffers(this_ptr, buffer_count, width, height, new_format, swap_chain_flags);

		// Recreate ImGui's device objects after resizing
		ImGui_ImplDX11_CreateDeviceObjects();

		// Return the result of the original IDXGISwapChain::ResizeBuffers
		return r_result;

	}

	int64_t __fastcall hooks::hk_idxgi_swap_chain_present(IDXGISwapChain* this_ptr, const unsigned int sync_interval, const unsigned int flags)
	{
		// Check if ImGui is initialized
		if (!is_imgui_initialized)
		{
			is_imgui_initialized = true;  // Only initialize ImGui once

			// Get the device associated with the swap chain
			if (SUCCEEDED(this_ptr->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&ptr_device))))
			{
				// Gets the device pointer of the swap chain for later usage
				this_ptr->GetDevice(__uuidof(ptr_device), reinterpret_cast<void**>(&ptr_device));
				std::cout << DEVICE_CONSOLE_TEXT << std::format("{:#x}", reinterpret_cast<std::uintptr_t>(ptr_device)) << std::endl;

				//Get pointer of te context of the swap chain
				ptr_device->GetImmediateContext(&ptr_context);
				std::cout << CONTEXT_CONSOLE_TEXT << std::format("{:#x}", reinterpret_cast<std::uintptr_t>(ptr_context)) << std::endl;
			}

			// Create an ImGui context
			DXGI_SWAP_CHAIN_DESC swap_chain_desc;
			this_ptr->GetDesc(&swap_chain_desc);

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			(void)io;
			ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;

			// Enable keyboard controls for ImGui
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

			// Get the window associated with the swap chain
			window = swap_chain_desc.OutputWindow;

			// Set a custom window procedure handler for ImGui
			o_wnd_proc_handler = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hk_wnd_proc)));

			// Initialize ImGui for Win and DirectX 11
			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX11_Init(ptr_device, ptr_context);
			ImGui::GetIO().ImeWindowHandle = window;

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
			ptr_device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state_false);

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
			ptr_device->CreateRasterizerState(&rasterizer_desc, &depth_bias_state_false);

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

			ptr_device->CreateRasterizerState(&normal_rasterizer_desc, &depth_bias_state_true);
		}

		if (render_target_view == nullptr)
		{
			// Get viewport information
			ptr_context->RSGetViewports(&num_of_view_ports, &viewport);
			screen_center_x = viewport.Width / 2.0f;
			screen_center_y = viewport.Height / 2.0f;

			// Gets the back buffer for rendering
			ID3D11Texture2D* back_buffer = nullptr;
			h_result_code = this_ptr->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));
			if (FAILED(h_result_code))
			{
				printf("%s\n",FAILED_TO_GET_BACK_BUFFER_CONSOLE_TEXT.c_str());
				return h_result_code;
			}

			// Creates a render target view for the back buffer
			h_result_code = ptr_device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
			back_buffer->Release();
			if (FAILED(h_result_code))
			{
				printf("%s\n", FAILED_TO_GET_RENDER_TARGET_CONSOLE_TEXT.c_str());
				return h_result_code;
			}
		}
		else 
		{ 
			// Set the render target view before drawing
			ptr_context->OMSetRenderTargets(1, &render_target_view, nullptr);
		}

		// Prepare for a new frame with ImGui
		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX11_NewFrame();
		ImGui::NewFrame();

		// Toggle menu visibility with the INSERT key
		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			show_menu = !show_menu;
		}

		if (show_menu)
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
			direct_x_11_base::hooks::unhook();
		}

		// Finish the ImGui frame and render ImGui
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Call the original IDXGISwapChain::Present function and return its result
		return o_IDXGISwapChain_Present(this_ptr, sync_interval, flags);
	}

	// Sets needed hooks here
	void hooks::init_hooks()
	{
		std::cout << DXGI_IMAGE_BASE_CONSOLE_TEXT << std::format("{:#x}", g_dxgi_base_address) << std::endl;
		std::cout << IDXGI_SWAPCHAIN_PRESENT_CONSOLE_TEXT << std::format("{:#x}", g_offset_IDXGISwapChain_Present) << std::endl;
		std::cout << IDXGI_SWAPCHAIN_RESIZEBUFFERS_CONSOLE_TEXT << std::format("{:#x}", g_offset_IDXGISwapChain_ResizeBuffers) << std::endl;

		ATTACH_HOOK(o_IDXGISwapChain_Present, direct_x_11_base::hooks::hk_idxgi_swap_chain_present);
		ATTACH_HOOK(o_IDXGISwapChain_ResizeBuffers, direct_x_11_base::hooks::hk_idxgi_swap_chain_resize_buffers);
	}

	// Removes created hooks
	void hooks::unhook()
	{
		DETACH_HOOK(o_IDXGISwapChain_Present, direct_x_11_base::hooks::hk_idxgi_swap_chain_present);
		DETACH_HOOK(o_IDXGISwapChain_ResizeBuffers, direct_x_11_base::hooks::hk_idxgi_swap_chain_resize_buffers);
		DETACH_HOOK(o_wnd_proc_handler, hk_wnd_proc);
	}
}
