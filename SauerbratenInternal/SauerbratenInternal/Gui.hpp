#pragma once
#include <Dependencies/ImGui/imgui.h>
#include <Dependencies/ImGui/imgui_impl_opengl3.h>
#include <Dependencies/ImGui/imgui_impl_win32.h>
#include <Constants.hpp>
#include <Features/ESP.hpp>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK windowProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN && wParam == VK_INSERT) {
		Core::Config::Menu::ShowMenu = !Core::Config::Menu::ShowMenu;
		return 0;
	}

	if (Core::Config::Menu::ShowMenu && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
		return true;
	}

	return CallWindowProc(Core::ProcessInformations::hGameWindowProc, hWnd, uMsg, wParam, lParam);
}

bool Initialized = false;

namespace Core {

	namespace Gui {

		void InitGui() {
			ImGui::CreateContext();
			ImGui_ImplWin32_Init(Core::ProcessInformations::Hwnd);
			ImGui_ImplOpenGL3_Init();
			ImGui::SetNextWindowSize({ 400,300 });
			ImGuiStyle& Style = ImGui::GetStyle();
			Style.Colors[ImGuiCol_TitleBg] = ImColor(255,0, 0,255);
			Style.Colors[ImGuiCol_TitleBgActive] = ImColor(255,0,0,255);
			Style.Colors[ImGuiCol_FrameBg] = ImColor(50, 50, 50,255);
			Style.Colors[ImGuiCol_FrameBgHovered] = ImColor(100,100,100,255);
			Style.Colors[ImGuiCol_FrameBgActive] = ImColor(70, 70, 70,255);
			Style.Colors[ImGuiCol_Button] = ImColor(255,0,0,255);
			Style.Colors[ImGuiCol_ButtonHovered] = ImColor(255,0,0,255);
			Style.Colors[ImGuiCol_ButtonActive] = ImColor(255,0,0,255);
			Style.Colors[ImGuiCol_Border] = ImColor(50, 50, 50,255);
			Style.Colors[ImGuiCol_CheckMark] = ImColor(255, 255, 255,255);
			Style.Colors[ImGuiCol_SliderGrab] = ImColor(255, 0, 0,255);
			Style.Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 0, 0,255);
			Style.Colors[ImGuiCol_ResizeGrip] = ImColor(255, 0, 0,255);
			Style.Colors[ImGuiCol_ResizeGripHovered] = ImColor(255, 0, 0,255);
			Style.Colors[ImGuiCol_ResizeGripActive] = ImColor(255, 0, 0,255);
			Style.Colors[ImGuiCol_HeaderHovered] = ImColor(70, 70, 70,255);
			Style.Colors[ImGuiCol_HeaderActive] = ImColor(70, 70, 70,255);
			Style.Colors[ImGuiCol_Header] = ImColor(70, 70, 70,255);
				
			Core::ProcessInformations::hGameWindowProc = (WNDPROC)SetWindowLongPtr(Core::ProcessInformations::Hwnd, GWLP_WNDPROC, (LONG_PTR)windowProc_hook);
			Initialized = true;
		}

		static const char* KeyNames[] = {
		"None", "Mouse 1", "Mouse 2", "Mouse 3", "Mouse 4", "Mouse 5",
		"Alt", "Shift", "Ctrl",
		"Caps Lock", "E", "Q", "R", "X", "Z", "V", "F"
		};

		static const int KeyValues[] = {
			0x00, 0x01, 0x02, 0x04, 0x05, 0x06,
			0x12, 0x10, 0x11,
			0x14, 0x45, 0x51, 0x52,
			0x58, 0x5A, 0x56, 0x46
		};

		void RenderGui() {

			if (!Initialized) InitGui();


			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			if (Core::Config::Menu::ShowMenu) {

				Core::ImportedFuncs::SDL_ShowCursor(1);
				Core::ImportedFuncs::InputGrabAddress(false, false);
				ImGui::Begin("SauerWare", &Core::Config::Menu::ShowMenu, ImGuiWindowFlags_NoCollapse);

				static int CurrentTab = 0;

				if (ImGui::Button("AimAssist")) {
					CurrentTab = 0;
				}
				ImGui::SameLine();
				if (ImGui::Button("ESP"))
				{
					CurrentTab = 1;
				}
				ImGui::SameLine();
				if (ImGui::Button("Misc"))
				{
					CurrentTab = 2;
				}

				ImGui::Separator();

				if (CurrentTab == 0) {
					ImGui::Text("General");
					ImGui::Checkbox("Ignore Dead", &Core::Config::Aimbot::IgnoreDead);
					ImGui::Checkbox("Ignore Team", &Core::Config::Aimbot::IgnoreTeam);
					ImGui::Separator();
					ImGui::Text("Aimbot");
					ImGui::Checkbox("Enable Aimbot", &Core::Config::Aimbot::EnableAimbot);
					ImGui::Checkbox("Enable Aimbot Visible Check", &Core::Config::Aimbot::EnableAimbotVisibleCheck);
					ImGui::Checkbox("Enable Aimbot Show Fov", &Core::Config::Aimbot::EnableShowFov);
					ImGui::SliderFloat("Aimbot Fov", &Core::Config::Aimbot::AimbotFov, 0, 400);
					ImGui::SliderFloat("Aimbot Smooth", &Core::Config::Aimbot::AimbotSmooth, 1, 100);
					static int CurrentKeyAim = 0;
					if (ImGui::Combo("Aimbot KeyBind", &CurrentKeyAim, KeyNames, IM_ARRAYSIZE(KeyNames))) {
						Core::Config::Aimbot::AimbotKey = KeyValues[CurrentKeyAim];
					}
					ImGui::ColorEdit4("Aimbot Fov Color", (float*)&Core::Config::Aimbot::AimbotFovColor);
					ImGui::Separator();
					ImGui::Text("Silent");
					ImGui::Checkbox("Enable SilentAim", &Core::Config::SilentAim::EnableSilent);
					ImGui::Checkbox("Enable Silent Visible Check", &Core::Config::SilentAim::EnableSilentVisibleCheck);
					ImGui::Checkbox("Enable SilentAim Show Fov", &Core::Config::SilentAim::EnableShowSilentFov);
					ImGui::SliderFloat("SilentAim Fov", &Core::Config::SilentAim::SilentFov, 0, 400);
					static int CurrentKeySilentAim = 0;
					if (ImGui::Combo("SilentAim KeyBind", &CurrentKeySilentAim, KeyNames, IM_ARRAYSIZE(KeyNames))) {
						Core::Config::SilentAim::SilentKey = KeyValues[CurrentKeySilentAim];
					}
					ImGui::ColorEdit4("SilentAim Fov Color", (float*)&Core::Config::SilentAim::SilentFovColor);
				}
				else if (CurrentTab == 1)
				{
					ImGui::Text("General");
					ImGui::Checkbox("Ignore Dead", &Core::Config::ESP::IgnoreDead);
					ImGui::Checkbox("Ignore Team", &Core::Config::ESP::IgnoreTeam);
					ImGui::Separator();
					ImGui::Text("Chams");

					ImGui::Checkbox("Enable Chams", &Core::Config::ESP::EnableChams);
					ImGui::Combo("Chams Mode", &Config::ESP::CurrentChams, "Black\0Transparent\0Enlightened");
					if (Core::Config::ESP::CurrentChams == 2) {
						ImGui::ColorEdit3("Chams Color", (float*)&Core::Config::ESP::ChamsColor);
					}
					
					ImGui::Separator();
					ImGui::Text("Options");
					ImGui::Checkbox("Enable ESP", &Core::Config::ESP::EnableESP);
					ImGui::Checkbox("Enable Box", &Core::Config::ESP::EnableBox);
					ImGui::Checkbox("Enable Line", &Core::Config::ESP::EnableLine);
					ImGui::Checkbox("Enable HealthBar", &Core::Config::ESP::EnableHealthBar);
					ImGui::Checkbox("Enable Name", &Core::Config::ESP::EnableName);
					ImGui::Checkbox("Enable Distance", &Core::Config::ESP::EnableDistance);

					ImGui::ColorEdit4("Visible Box Color", (float*)&Core::Config::ESP::ColorBoxVisible);
					ImGui::ColorEdit4("Invisible Box Color", (float*)&Core::Config::ESP::ColorBoxInvisible);
					ImGui::ColorEdit4("Visible Line Color", (float*)&Core::Config::ESP::ColorLineVisible);
					ImGui::ColorEdit4("Invisible Line Color", (float*)&Core::Config::ESP::ColorLineInvisible);
					ImGui::ColorEdit4("HealthBar Color", (float*)&Core::Config::ESP::ColorHealthBar);
					ImGui::ColorEdit4("Text Color", (float*)&Core::Config::ESP::ColorText);
				}
				else if (CurrentTab == 2)
				{
					if (ImGui::Checkbox("Thirdperson", &Core::Config::Misc::Thirdperson)) {
						if (Core::Config::Misc::Thirdperson) {
							Funcs::Write<BYTE>((uintptr_t)Core::ProcessInformations::GameModule + Core::Offsets::Game::Thirdperson, 1);
						}
						else
						{
							Funcs::Write<BYTE>((uintptr_t)Core::ProcessInformations::GameModule + Core::Offsets::Game::Thirdperson, 0);
						}
					}

					if (ImGui::SliderInt("Camera Fov", &Core::Config::Misc::CameraFov, 1, 160)) {
						Funcs::Write<int>((uintptr_t)Core::ProcessInformations::GameModule + Core::Offsets::Game::Fov, Core::Config::Misc::CameraFov);
					}
				}




				ImGui::End();

			}
			else
			{
				Core::ImportedFuncs::SDL_ShowCursor(0);
				Core::ImportedFuncs::InputGrabAddress(true, false);

			}

			Features::ESP::Run();

			ImVec2 ScreenCenter = { ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2 };

			if (Core::Config::Aimbot::EnableShowFov) {
				ImGui::GetBackgroundDrawList()->AddCircle(ScreenCenter, Core::Config::Aimbot::AimbotFov, Core::Config::Aimbot::AimbotFovColor);
			}

			if (Core::Config::SilentAim::EnableShowSilentFov) {
				ImGui::GetBackgroundDrawList()->AddCircle(ScreenCenter, Core::Config::SilentAim::SilentFov, Core::Config::SilentAim::SilentFovColor);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());




		}
	}
}
