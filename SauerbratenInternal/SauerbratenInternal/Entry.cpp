#include <Includes.hpp>
#include <Hooks.hpp>

#include <Entity.hpp> 
#include <Features/AimAssist.hpp>


void Start() {

    if (AllocConsole())
    {
        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
        freopen_s(&f, "CONOUT$", "w", stderr);
        freopen_s(&f, "CONIN$", "r", stdin);

        std::ios::sync_with_stdio(true);
        std::cout.clear();
        std::cerr.clear();
        std::cin.clear();

        SetConsoleTitleA("Internal");
    }

    Core::ProcessInformations::GameHandle = GetCurrentProcess();
    Core::ProcessInformations::GameModule = GetModuleHandleA(nullptr);
    Core::ProcessInformations::SDL2_Module = GetModuleHandleA("SDL2.dll");

    std::cout << "[+] GameModule -> " << std::hex << Core::ProcessInformations::GameModule << std::dec << '\n';
    std::cout << "[+] SDL2_Module -> " << std::hex << Core::ProcessInformations::SDL2_Module << std::dec << '\n';

    //SDL Funcs

    Core::ImportedFuncs::SDL_GL_GetProc = (Core::ImportedFuncs::SDL_GL_GetProcAddress_t)((int)Core::ProcessInformations::SDL2_Module + Core::Offsets::SDL::SDL_GL_GetProcAddress);
    std::cout << "[+] SDL_GL_GetProc -> " << std::hex << Core::ImportedFuncs::SDL_GL_GetProc << std::dec << '\n';

    Core::ImportedFuncs::SDL_SetRelativeMouseModeAddress = (Core::ImportedFuncs::SDL_SetRelativeMouseMode_t)((int)Core::ProcessInformations::SDL2_Module + Core::Offsets::SDL::SDL_SetRelativeMouseMode);
    std::cout << "[+] SDL_SetRelativeMouseMode -> " << std::hex << Core::ImportedFuncs::SDL_SetRelativeMouseModeAddress << std::dec << '\n';

    Core::ImportedFuncs::SDL_ShowCursor = (Core::ImportedFuncs::SDL_ShowCursor_t)((int)Core::ProcessInformations::SDL2_Module + Core::Offsets::SDL::SDL_ShowCursor);
    std::cout << "[+] SDL_ShowCursor -> " << std::hex << Core::ImportedFuncs::SDL_ShowCursor << std::dec << '\n';

    Core::ImportedFuncs::SDL_WarpMouseInWindowAddress = (Core::ImportedFuncs::SDL_WarpMouseInWindow_t)((int)Core::ProcessInformations::SDL2_Module + Core::Offsets::SDL::SDL_WarpMouseInWindow);
    std::cout << "[+] SDL_WarpMouseInWindow -> " << std::hex << Core::ImportedFuncs::SDL_WarpMouseInWindowAddress << std::dec << '\n';

    //opengl32.dll Funcs

    Core::ImportedFuncs::glEnable = (Core::ImportedFuncs::glEnable_t)Core::ImportedFuncs::SDL_GL_GetProc("glEnable");
    std::cout << "[+] glEnable -> " << std::hex << Core::ImportedFuncs::glEnable << std::dec << '\n';

    Core::ImportedFuncs::glDisable = (Core::ImportedFuncs::glDisable_t)Core::ImportedFuncs::SDL_GL_GetProc("glDisable");
    std::cout << "[+] glDisable -> " << std::hex << Core::ImportedFuncs::glDisable << std::dec << '\n';


    Core::ImportedFuncs::glBlendFunc = (Core::ImportedFuncs::glBlendFunc_t)Core::ImportedFuncs::SDL_GL_GetProc("glBlendFunc");
    std::cout << "[+] glBlendFunc -> " << std::hex << Core::ImportedFuncs::glBlendFunc << std::dec << '\n';


    Core::ImportedFuncs::wglSwapBuffersAddress = (Core::ImportedFuncs::wglSwapBuffers_t)Core::ImportedFuncs::SDL_GL_GetProc("wglSwapBuffers");
    std::cout << "[+] wglSwapBuffers -> " << std::hex << Core::ImportedFuncs::wglSwapBuffersAddress << std::dec << '\n';

    auto FunctionPointerLocation = (void**)((uintptr_t)Core::ProcessInformations::GameModule + 0x2A2570);
    Core::ImportedFuncs::glDrawRangeElementsAddress = (Core::ImportedFuncs::glDrawRangeElements_t)(*FunctionPointerLocation);
    std::cout << "[+] glDrawRangeElements -> " << std::hex << (void*)Core::ImportedFuncs::glDrawRangeElementsAddress << std::dec << '\n';

    //Game Funcs 

    Core::ImportedFuncs::InputGrabAddress = (Core::ImportedFuncs::InputGrab_t)Core::Funcs::FindSignature(Core::Patterns::InputGrabPattern, Core::ProcessInformations::GameModule);
    std::cout << "[+] InputGrab -> " << std::hex << Core::ImportedFuncs::InputGrabAddress << std::dec << '\n';

   /* Core::ImportedFuncs::InterpBonesAddress = (Core::ImportedFuncs::InterpBones_t)Core::Funcs::FindSignature(Core::Patterns::InterpBonesPattern, Core::ProcessInformations::GameModule);
    std::cout << "[+] InterpBones -> " << std::hex << Core::ImportedFuncs::InterpBonesAddress << std::dec << '\n';*/

    Core::ImportedFuncs::ShootAddress = (Core::ImportedFuncs::Shoot_t)Core::Funcs::FindSignature(Core::Patterns::ShootPattern, Core::ProcessInformations::GameModule);
    std::cout << "[+] Shoot -> " << std::hex << Core::ImportedFuncs::ShootAddress << std::dec << '\n';

    Core::ImportedFuncs::RayCube = (Core::ImportedFuncs::RayCube_t)Core::Funcs::FindSignature(Core::Patterns::RayCubePattern, Core::ProcessInformations::GameModule);
    std::cout << "[+] RayCube -> " << std::hex << Core::ImportedFuncs::RayCube << std::dec << '\n';

    Core::ImportedFuncs::RenderModelAddress = (Core::ImportedFuncs::RenderModel_t)Core::Funcs::FindSignature(Core::Patterns::RenderModelPattern, Core::ProcessInformations::GameModule);
    std::cout << "[+] RenderModel -> " << std::hex << Core::ImportedFuncs::RenderModelAddress << std::dec << '\n';

    Core::ImportedFuncs::SkeletonRenderAddress = (Core::ImportedFuncs::SkeletonRender_t)Core::Funcs::FindSignature(Core::Patterns::SkeletonRenderPattern, Core::ProcessInformations::GameModule);
    std::cout << "[+] SkeletonRender -> " << std::hex << Core::ImportedFuncs::SkeletonRenderAddress << std::dec << '\n';


    //Thread Start

    Core::Hooks::Setup();

    std::thread([] {Core::EntityNamespace::UpdateEntityList();}).detach();
    std::thread([] {Core::EntityNamespace::UpdateLocalPlayer();}).detach();
    std::thread([] {Features::AimAssist::RunAimbot();}).detach();

    std::cout << "[+] All Done!\n";
}


BOOL WINAPI DllMain( HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)  
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Start, NULL, NULL, NULL);
        break;

    case DLL_PROCESS_DETACH:

        if (lpvReserved != nullptr)
        {
            break; 
        }
        break;
    }
    return TRUE;  
}