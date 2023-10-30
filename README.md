# DirectX 11 Hooking and ImGui Integration by clientinfo

This project provides a demonstration of how to hook DirectX 11 functions to integrate ImGui as a UI in games and applications.
It allows you to inject custom UI elements into the DirectX rendering pipeline.
The project is well-commented to guide you through the code.

## License

This project is provided under the MIT License. Feel free to use and modify it as needed, but please credit the original authors.

## Project Structure

- `Includes.h`: Contains necessary include statements and defines.
- `hooks.h`: Defines hooking functions for DirectX 11 functions.
- `hooks.cpp`: Implements the DirectX 11 hooking functions.
- `main.cpp`: Contains the application's main entry point and ImGui setup.
- `README.md`: This document.
- Other files and directories: Contain needed libaries and headers

## Getting Started

Before you use this project, make sure you understand how it works:

1. **Hooking DirectX Functions:** This project hooks into functions like `IDXGISwapChain::Present` and `IDXGISwapChain::ResizeBuffers` in DirectX 11, `IDXGISwapChain::Present` for rendering and `IDXGISwapChain::ResizeBuffers` to fix the crash when resizing the application/game.

2. **ImGui Integration:** The project integrates ImGui to create, update, and render the UI. ImGui provides an easy way to design and render UI elements.

3. **Usage:** This project demonstrates how to initialize and unhook the DirectX functions, as well as how to handle UI elements.

## Setting Up the Project

1. Open the project in your development environment.

2. Compile the code, ensuring that the necessary headers and libraries are correctly set up.

3. Run a game or application which uses direct x 11, and it should display an ImGui-based overlay inside of the application.

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
