#include "Includes.h"

// The DllMain function is called when the DLL is loaded or unloaded in a process
BOOL APIENTRY DllMain(const HMODULE h_module, const DWORD ul_reason_for_call, LPVOID lp_reserved)
{
    // DisableThreadLibraryCalls prevents unnecessary thread notifications, improving performance
    DisableThreadLibraryCalls(h_module);

    // Switch statement to handle different reasons for calling DllMain
    switch (ul_reason_for_call)
    {

    // When the DLL is loaded into a process
    case DLL_PROCESS_ATTACH:

        // Allocate a console window for debugging output
        AllocConsole();

        // Redirect standard output to the console window
        freopen("CONOUT$", "w", stdout);

        // Set the console window's title
        SetConsoleTitleA("Direct X 11 Hook Base by clientinfo, Release Date: 26.10.2023");

        // Initialize Needed Hooks
        direct_x_11_base::hooks::init_hooks();

        // Return TRUE to indicate successful DLL attachment
        return TRUE;

    // When the DLL is being unloaded from a process
    case DLL_PROCESS_DETACH:

        // Clean up and exit the DLL with a success code
        exit(EXIT_SUCCESS);

    default:
        // In case of any other reason, return FALSE
        return FALSE;
    }
}
