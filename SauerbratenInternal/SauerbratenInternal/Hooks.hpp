#pragma once
#include <Includes.hpp>
#include <Dependencies/MinHook/include/MinHook.h>
#include <Gui.hpp>
#include <Features/AimAssist.hpp>

namespace Core {
	namespace Hooks {
      
		void __stdcall hkglDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices) 
		{
			if (Config::ESP::EnableChams) {
				if (mode == 4 && type == 0x1403u && (count == 0x1830 || count == 0x7da || count == 0x1bf6 || count == 0x2118 || count == 0x203a))
				{
					if (Config::ESP::CurrentChams == 0) {
						Core::ImportedFuncs::glDisable(GL_DEPTH_TEST);
						Core::ImportedFuncs::glEnable(GL_BLEND);

						Core::ImportedFuncs::glBlendFunc(GL_ZERO, GL_ZERO);
						Core::ImportedFuncs::glDrawRangeElementsOriginal(mode, start, end, count, type, indices);

						Core::ImportedFuncs::glEnable(GL_DEPTH_TEST);
					}
					else if (Config::ESP::CurrentChams == 1 || Config::ESP::CurrentChams == 2) {
						Core::ImportedFuncs::glDisable(GL_DEPTH_TEST);
						Core::ImportedFuncs::glDrawRangeElementsOriginal(mode, start, end, count, type, indices);
						Core::ImportedFuncs::glEnable(GL_DEPTH_TEST);
					}
					

					
				}
			}

			
			return Core::ImportedFuncs::glDrawRangeElementsOriginal(mode, start, end, count, type, indices);
		}

		BOOL __stdcall hkwglSwapBuffers(HDC hdc)
		{
			Core::ProcessInformations::Hwnd = WindowFromDC(hdc);
			Core::Gui::RenderGui();
			return Core::ImportedFuncs::wglSwapBuffersOriginal(hdc);
		}

		void hkSDL_WarpMouseInWindow(SDL_Window* window, int x, int y) {
			if (Core::Config::Menu::ShowMenu == true) {
				return;
			}

			return Core::ImportedFuncs::SDL_WarpMouseInWindowOriginal(window, x, y);
		}

		int hkSDL_SetRelativeMouseMode(bool enabled) {
			if (Core::Config::Menu::ShowMenu == true) {
				enabled = false;
			}

			return Core::ImportedFuncs::SDL_SetRelativeMouseModeOriginal(enabled);
		}

		void hkRenderModel(void* p_light, const char* a2, int a3, const Vec3* a4, float yaw, float pitch, int flags, void* d, void* a, unsigned int basetime, unsigned int basetime2, float trans) {

			if (Config::ESP::EnableChams && Config::ESP::CurrentChams == 2) {
				if (strcmp("captaincannon", a2) == 0 || strcmp("mrfixit", a2) == 0 || strcmp("snoutx10k", a2) == 0 || strcmp("inky", a2) == 0 || strcmp("ogro", a2) == 0 || strcmp("ogro2", a2) == 0) {
					if (d) {
						flags = 0;
						Funcs::Write<Vec3>((uintptr_t)d + 0x80, Vec3(Config::ESP::ChamsColor.Value.x, Config::ESP::ChamsColor.Value.y, Config::ESP::ChamsColor.Value.z));
					}

				}
			}

			return Core::ImportedFuncs::RenderModelOriginal(p_light, a2, a3, a4, yaw, pitch, flags, d, a, basetime, basetime2, trans);
		}

		void hkRenderSkeleton(void* skel, const void* as, float pitch, const Vec3* axis, const Vec3* forward, void* d, void* p) {
			auto Meshes = Funcs::Read<uintptr_t>((uintptr_t)skel + 0x20);
			void* MeshesBuf = Funcs::Read<void*>(Meshes + 0x0);
			auto MeshesLen = Funcs::Read<int>(Meshes + 0xc);

			if (MeshesLen > 2) {
				constexpr size_t StructSize = 0x20; 
				uintptr_t PtrToThird = (uintptr_t)MeshesBuf + 2 * StructSize;


				constexpr size_t GroupOffset = 0x8;

				uint64_t GroupValue = Funcs::Read<uint64_t>(PtrToThird + GroupOffset);
				uint32_t High = (uint32_t)(GroupValue >> 32);

				std::cout << std::hex << High << '\n'; //FUNÇÃO PRA RETORNAR O COUNT DOS MODELOS
			}

			return Core::ImportedFuncs::SkeletonRenderOriginal(skel, as, pitch, axis, forward, d, p); 
		}



		/*void __fastcall hkinterpbones(
			void* skel,
			const void* as,
			float pitch,
			const void* axis,
			const void* forward,
			int numanimparts,
			const unsigned __int8* partmask,
			void* sc)
		{
			Core::ImportedFuncs::InterpBonesOriginal(skel, as, pitch, axis, forward, numanimparts, partmask, sc);
			simple_dualquat* bdata = Funcs::Read<simple_dualquat*>((uintptr_t)sc + 0xA8);
			if (!bdata)
				return;
		
			int count = Funcs::Read<int>((uintptr_t)skel + 0x28);

			

			for (int i = 0; i < count; i++)
			{
				simple_dualquat& dq = bdata[i];
				float x, y, z;
				dq.gettranslation(x, y, z);

			

			}
		}*/


		void Setup() {


			auto status = MH_Initialize();

			if (status != MH_OK)
				std::cout << "[!] Error -> Minhook Initialize -> Status: " << status << '\n';


			status = MH_CreateHook(Core::ImportedFuncs::glDrawRangeElementsAddress, &hkglDrawRangeElements, (void**)&Core::ImportedFuncs::glDrawRangeElementsOriginal);
			if (status != MH_OK)
				std::cout << "[!] Error -> Minhook Create Hook -> glDrawRangeElementsAddress -> Status: " << status << '\n';
			
			status = MH_EnableHook(Core::ImportedFuncs::glDrawRangeElementsAddress);
			if (status != MH_OK)
				std::cout << "[!] Error -> Minhook Enable Hook -> glDrawRangeElementsAddress -> Status: " << status << '\n';


			status = MH_CreateHook(Core::ImportedFuncs::wglSwapBuffersAddress, &hkwglSwapBuffers, (void**)&Core::ImportedFuncs::wglSwapBuffersOriginal);
			if (status != MH_OK)
				std::cout << "[!] Error -> MinHook Create Hook -> wglSwapBuffers -> Status: " << status << '\n';

			status = MH_EnableHook(Core::ImportedFuncs::wglSwapBuffersAddress);
			if (status != MH_OK)
				std::cout << "[!] Error -> MinHook Enable Hook -> wglSwapBuffers -> Status: " << status << '\n';

				


			status = MH_CreateHook(Core::ImportedFuncs::SDL_WarpMouseInWindowAddress, &hkSDL_WarpMouseInWindow, (void**)&Core::ImportedFuncs::SDL_WarpMouseInWindowOriginal);
			if (status != MH_OK)
				std::cout << "[!] Error -> MinHook Create Hook -> SDL_WarpMouseInWindow -> Status: " << status << '\n';

			status = MH_EnableHook(Core::ImportedFuncs::SDL_WarpMouseInWindowAddress);
			if (status != MH_OK)
				std::cout << "[!] Error -> MinHook Enable Hook -> SDL_WarpMouseInWindow -> Status: " << status << '\n';



			status = MH_CreateHook(Core::ImportedFuncs::SDL_SetRelativeMouseModeAddress, &hkSDL_SetRelativeMouseMode, (void**)&Core::ImportedFuncs::SDL_SetRelativeMouseModeOriginal);
			if (status != MH_OK)
				std::cout << "[!] Error -> MinHook Create Hook -> SDL_SetRelativeMouseMode -> Status: " << status << '\n';

			status = MH_EnableHook(Core::ImportedFuncs::SDL_SetRelativeMouseModeAddress);
			if (status != MH_OK)
				std::cout << "[!] Error -> MinHook Enable Hook -> SDL_SetRelativeMouseMode -> Status: " << status << '\n';


			/*status = MH_CreateHook(Core::ImportedFuncs::InterpBonesAddress, &hkinterpbones, (void**)&Core::ImportedFuncs::InterpBonesOriginal);
			if (status != MH_OK)
				std::cout << "[!] Error -> MinHook Create Hook -> InterpBones -> Status: " << status << '\n';

			status = MH_EnableHook(Core::ImportedFuncs::InterpBonesAddress);
			if (status != MH_OK)
				std::cout << "[!] Error -> MinHook Enable Hook -> InterpBones -> Status: " << status << '\n';*/


			status = MH_CreateHook(Core::ImportedFuncs::ShootAddress, &Features::AimAssist::RunSilent, (void**)&Core::ImportedFuncs::ShootOriginal);
			if (status != MH_OK)
				std::cout << "[!] Error -> MinHook Create Hook -> Shoot -> Status: " << status << '\n';

			status = MH_EnableHook(Core::ImportedFuncs::ShootAddress);
			if (status != MH_OK)
				std::cout << "[!] Error -> MinHook Enable Hook -> Shoot -> Status: " << status << '\n';


			status = MH_CreateHook(Core::ImportedFuncs::RenderModelAddress, &hkRenderModel, (void**)&Core::ImportedFuncs::RenderModelOriginal);
			if (status != MH_OK)
				std::cout << "[!] Error -> MinHook Create Hook -> RenderModel -> Status: " << status << '\n';

			status = MH_EnableHook(Core::ImportedFuncs::RenderModelAddress);
			if (status != MH_OK)
				std::cout << "[!] Error -> MinHook Enable Hook -> RenderModel -> Status: " << status << '\n';


			//status = MH_CreateHook(Core::ImportedFuncs::SkeletonRenderAddress, &hkRenderSkeleton, (void**)&Core::ImportedFuncs::SkeletonRenderOriginal);
			//if (status != MH_OK)
			//	std::cout << "[!] Error -> MinHook Create Hook -> SkeletonRender -> Status: " << status << '\n';

			//status = MH_EnableHook(Core::ImportedFuncs::SkeletonRenderAddress);
			//if (status != MH_OK)
			//	std::cout << "[!] Error -> MinHook Enable Hook -> SkeletonRender -> Status: " << status << '\n';




			std::cout << "[+] Done Hooks\n";

			

		}

	}
}