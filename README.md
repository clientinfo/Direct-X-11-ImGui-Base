# DirectX 11 Hooking and ImGui Implementation Base by clientinfo

This project provides a demonstration of how to hook DirectX 11 and render ImGui in a games and application.
It allows you to render IGui elements into the DirectX rendering pipeline of the application.
The project is well-commented to guide you through the code.

## License

This project is provided under the MIT License. Feel free to use and modify it as needed, but please credit the original authors.

## IMPORTANT
Stuff will only work on Windows 10 and only on x64 Processes

## Project Structure

- `Includes.h`: Contains necessary include statements and defines.
- `hooks.h`: Defines hooking functions for DirectX 11 functions.
- `hooks.cpp`: Implements the DirectX 11 hooking functions.
- `main.cpp`: Contains the application's main entry point and ImGui setup.
- `README.md`: This document.
- Other files and directories: Contain needed libaries and headers

## Getting Started

Before you use this project, make sure you understand how it works:

1. **Hooking DirectX Functions:** This project hooks into functions like `IDXGISwapChain::Present`, `IDXGISwapChain::ResizeBuffers`, `ID3D11DeviceContext_DrawIndexed` and `ID3D11DeviceContext_DrawIndexedInstanced` in DirectX 11, `IDXGISwapChain::Present` for rendering and `IDXGISwapChain::ResizeBuffers` to fix the crash when resizing the application/game. `ID3D11DeviceContext_DrawIndexed` to be able to filter through stripes and logcount for example. `ID3D11DeviceContext_DrawIndexedInstanced` serverse as a alternative in case `ID3D11DeviceContext_DrawIndexed` fails.

2. **ImGui Integration:** The project integrates ImGui to create, update, and render the UI. ImGui provides an easy way to design and render UI elements.

3. **Usage:** This project demonstrates how to initialize hokks and how to unhook the DirectX functions, as well as how to handle the UI elements of ImGui.

## Setting Up the Project

1. Open the project in your development environment.

2. Compile the code, ensuring that the necessary headers and libraries are correctly set up.

3. Run a game or application which uses direct x 11.

4. Use your favorite injector and it should display an ImGui-based overlay inside of the application.
   
## Modifying the Project

You can modify the project to meet your specific needs, such as adding custom UI elements, implementing additional features, or integrating it with your game or application.

## Important Notes

- This code is meant for educational purposes and as a starting point for DirectX 11 hooking and ImGui integration.

- DirectX hooking may be subject to changes with Windows updates, so be prepared to update offsets manually (Instructions are in the source "hooks.cpp").

- The project provides a basic demonstration, and it may need adjustments to work with specific games or applications.

## Contributions

Contributions are welcome! If you find issues or have improvements to suggest, please create a pull request or open an issue.

## Acknowledgments

This project was created by clientinfo.

Feel free to reach out with any questions or suggestions.

Happy coding!
