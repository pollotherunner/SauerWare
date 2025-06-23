#pragma once
#include <Dependencies/ImGui/imgui.h>
#include <Dependencies/ImGui/imgui_impl_opengl3.h>
#include <Dependencies/ImGui/imgui_impl_win32.h>
#include <Constants.hpp>
#include <Features/ESP.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <Windows.h>


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

	namespace Gui {		// Path to settings folder
		std::string GetConfigPath() {
			// For injected DLLs, it is better to use a specific path
			char tempPath[MAX_PATH];
			DWORD result = GetTempPathA(MAX_PATH, tempPath);
			
			if (result == 0) {
				OutputDebugStringA("Error: Unable to get temp path\n");
				// Fallback to user folder
				char* userProfile = nullptr;
				size_t len = 0;
				if (_dupenv_s(&userProfile, &len, "USERPROFILE") == 0 && userProfile) {
					std::string directoryPath = std::string(userProfile) + "\\SauerWareConfig";
					OutputDebugStringA(("Using user folder: " + directoryPath + "\n").c_str());

					BOOL createResult = CreateDirectoryA(directoryPath.c_str(), NULL);
					DWORD error = GetLastError();

					if (createResult) {
						OutputDebugStringA("Directory created successfully\n");
					}
					else if (error == ERROR_ALREADY_EXISTS) {
						OutputDebugStringA("Directory already exists\n");
					}
					else {
						OutputDebugStringA(("Error creating directory. Code: " + std::to_string(error) + "\n").c_str());
					}

					free(userProfile); // Free the allocated memory
					return directoryPath;
				}
				return "";
			}
			
			std::string directoryPath = std::string(tempPath) + "SauerWareConfig";
			OutputDebugStringA(("Configuration directory (temp): " + directoryPath + "\n").c_str());
			
			// Creates the directory if it does not exist
			BOOL createResult = CreateDirectoryA(directoryPath.c_str(), NULL);
			DWORD error = GetLastError();
			
			if (createResult) {
				OutputDebugStringA("Directory created successfully\n");
			} else if (error == ERROR_ALREADY_EXISTS) {
				OutputDebugStringA("Directory already exists\n");
			} else {
				OutputDebugStringA(("Error creating directory. Code: " + std::to_string(error) + "\n").c_str());
			}
			
			return directoryPath;
		}		void SaveConfig(const std::string& configName) {
			try {
				std::string configPath = GetConfigPath();
				if (configPath.empty()) {
					OutputDebugStringA("Empty configuration path\n");
					return;
				}
				
				std::string fullPath = configPath + "\\" + configName + ".cfg";
				
				// Debug: Check if the path is correct
				OutputDebugStringA(("Trying to save config in: " + fullPath + "\n").c_str());
				
				// Directory write permission test
				std::string testFile = configPath + "\\test_write.tmp";
				std::ofstream testStream(testFile);
				if (testStream.is_open()) {
					testStream << "test";
					testStream.close();
					DeleteFileA(testFile.c_str());
					OutputDebugStringA("Directory writing test: yes\n");
				} else {
					OutputDebugStringA("Directory writing test: no\n");
				}
				
				std::ofstream configFile(fullPath, std::ios::out | std::ios::trunc);
				
				if (!configFile.is_open()) {
					OutputDebugStringA("Error: Could not open file for writing\n");
					
					// Try to create using Win32 functions
					HANDLE hFile = CreateFileA(fullPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile == INVALID_HANDLE_VALUE) {
						DWORD error = GetLastError();
						OutputDebugStringA(("Erro CreateFile: " + std::to_string(error) + "\n").c_str());
						return;
					}
					CloseHandle(hFile);
					
					// Try opening again
					configFile.open(fullPath, std::ios::out | std::ios::trunc);
					if (!configFile.is_open()) {
						OutputDebugStringA("Error: Still unable to open after CreateFile\n");
						return;
					}
				}
				
				OutputDebugStringA("File opened successfully, saving settings...\n");
				
				// Save Aimbot Settings
				configFile << "[Aimbot]" << std::endl;
				configFile << "IgnoreDead=" << (Core::Config::Aimbot::IgnoreDead ? "1" : "0") << std::endl;
				configFile << "IgnoreTeam=" << (Core::Config::Aimbot::IgnoreTeam ? "1" : "0") << std::endl;
				configFile << "EnableAimbot=" << (Core::Config::Aimbot::EnableAimbot ? "1" : "0") << std::endl;
				configFile << "EnableAimbotVisibleCheck=" << (Core::Config::Aimbot::EnableAimbotVisibleCheck ? "1" : "0") << std::endl;
				configFile << "AimbotFov=" << Core::Config::Aimbot::AimbotFov << std::endl;
				configFile << "AimbotSmooth=" << Core::Config::Aimbot::AimbotSmooth << std::endl;
				configFile << "EnableShowFov=" << (Core::Config::Aimbot::EnableShowFov ? "1" : "0") << std::endl;
				configFile << "AimbotKey=" << Core::Config::Aimbot::AimbotKey << std::endl;
				configFile << "AimbotFovColor=" << Core::Config::Aimbot::AimbotFovColor.Value.x << "," 
				                                << Core::Config::Aimbot::AimbotFovColor.Value.y << "," 
												<< Core::Config::Aimbot::AimbotFovColor.Value.z << "," 
												<< Core::Config::Aimbot::AimbotFovColor.Value.w << std::endl;
				
				// Save SilentAim settings
				configFile << std::endl << "[SilentAim]" << std::endl;
				configFile << "EnableSilent=" << (Core::Config::SilentAim::EnableSilent ? "1" : "0") << std::endl;
				configFile << "EnableSilentVisibleCheck=" << (Core::Config::SilentAim::EnableSilentVisibleCheck ? "1" : "0") << std::endl;
				configFile << "SilentFov=" << Core::Config::SilentAim::SilentFov << std::endl;
				configFile << "EnableShowSilentFov=" << (Core::Config::SilentAim::EnableShowSilentFov ? "1" : "0") << std::endl;
				configFile << "SilentKey=" << Core::Config::SilentAim::SilentKey << std::endl;
				configFile << "SilentFovColor=" << Core::Config::SilentAim::SilentFovColor.Value.x << "," 
				                               << Core::Config::SilentAim::SilentFovColor.Value.y << "," 
											   << Core::Config::SilentAim::SilentFovColor.Value.z << "," 
											   << Core::Config::SilentAim::SilentFovColor.Value.w << std::endl;
				
				// Save ESP Settings
				configFile << std::endl << "[ESP]" << std::endl;
				configFile << "IgnoreDead=" << (Core::Config::ESP::IgnoreDead ? "1" : "0") << std::endl;
				configFile << "IgnoreTeam=" << (Core::Config::ESP::IgnoreTeam ? "1" : "0") << std::endl;
				configFile << "EnableESP=" << (Core::Config::ESP::EnableESP ? "1" : "0") << std::endl;
				configFile << "EnableBox=" << (Core::Config::ESP::EnableBox ? "1" : "0") << std::endl;
				configFile << "EnableLine=" << (Core::Config::ESP::EnableLine ? "1" : "0") << std::endl;
				configFile << "EnableHealthBar=" << (Core::Config::ESP::EnableHealthBar ? "1" : "0") << std::endl;
				configFile << "EnableDistance=" << (Core::Config::ESP::EnableDistance ? "1" : "0") << std::endl;
				configFile << "EnableName=" << (Core::Config::ESP::EnableName ? "1" : "0") << std::endl;
				configFile << "EnableChams=" << (Core::Config::ESP::EnableChams ? "1" : "0") << std::endl;
				configFile << "CurrentChams=" << Core::Config::ESP::CurrentChams << std::endl;
				configFile << "ChamsColor=" << Core::Config::ESP::ChamsColor.Value.x << "," 
				                          << Core::Config::ESP::ChamsColor.Value.y << "," 
										  << Core::Config::ESP::ChamsColor.Value.z << "," 
										  << Core::Config::ESP::ChamsColor.Value.w << std::endl;
				configFile << "ColorBoxVisible=" << Core::Config::ESP::ColorBoxVisible.Value.x << "," 
				                               << Core::Config::ESP::ColorBoxVisible.Value.y << "," 
											   << Core::Config::ESP::ColorBoxVisible.Value.z << "," 
											   << Core::Config::ESP::ColorBoxVisible.Value.w << std::endl;
				configFile << "ColorBoxInvisible=" << Core::Config::ESP::ColorBoxInvisible.Value.x << "," 
				                                 << Core::Config::ESP::ColorBoxInvisible.Value.y << "," 
												 << Core::Config::ESP::ColorBoxInvisible.Value.z << "," 
												 << Core::Config::ESP::ColorBoxInvisible.Value.w << std::endl;
				configFile << "ColorLineVisible=" << Core::Config::ESP::ColorLineVisible.Value.x << "," 
				                                << Core::Config::ESP::ColorLineVisible.Value.y << "," 
												<< Core::Config::ESP::ColorLineVisible.Value.z << "," 
												<< Core::Config::ESP::ColorLineVisible.Value.w << std::endl;
				configFile << "ColorLineInvisible=" << Core::Config::ESP::ColorLineInvisible.Value.x << "," 
				                                  << Core::Config::ESP::ColorLineInvisible.Value.y << "," 
												  << Core::Config::ESP::ColorLineInvisible.Value.z << "," 
												  << Core::Config::ESP::ColorLineInvisible.Value.w << std::endl;
				configFile << "ColorHealthBar=" << Core::Config::ESP::ColorHealthBar.Value.x << "," 
				                              << Core::Config::ESP::ColorHealthBar.Value.y << "," 
											  << Core::Config::ESP::ColorHealthBar.Value.z << "," 
											  << Core::Config::ESP::ColorHealthBar.Value.w << std::endl;
				configFile << "ColorText=" << Core::Config::ESP::ColorText.Value.x << "," 
				                         << Core::Config::ESP::ColorText.Value.y << "," 
										 << Core::Config::ESP::ColorText.Value.z << "," 
										 << Core::Config::ESP::ColorText.Value.w << std::endl;
				
				// Save Misc Settings
				configFile << std::endl << "[Misc]" << std::endl;
				configFile << "Thirdperson=" << (Core::Config::Misc::Thirdperson ? "1" : "0") << std::endl;
				configFile << "CameraFov=" << Core::Config::Misc::CameraFov << std::endl;
				
				// Check if there was an error while writing
				if (configFile.fail()) {
					OutputDebugStringA("Error writing to file\n");
				}
				
				configFile.flush();
				configFile.close();
				
				// Check if the file was actually created
				DWORD attributes = GetFileAttributesA(fullPath.c_str());
				if (attributes != INVALID_FILE_ATTRIBUTES) {
					OutputDebugStringA("Configuration saved successfully! File verified.\n");
				} else {
					OutputDebugStringA("Error: File not created even after apparent success\n");
				}
			}
			catch (const std::exception& e) {
				std::string errorMsg = "Error saving configuration: " + std::string(e.what()) + "\n";
				OutputDebugStringA(errorMsg.c_str());
			}
		}
		
		bool LoadConfig(const std::string& configName) {
			std::string configPath = GetConfigPath() + "\\" + configName + ".cfg";
			std::ifstream configFile(configPath);
			
			if (!configFile.is_open()) {
				return false;
			}
			
			std::string line;
			std::string currentSection;
			
			while (std::getline(configFile, line)) {
				// Ignore empty lines
				if (line.empty())
					continue;
				
				// Detects sections [Section]
				if (line[0] == '[' && line.back() == ']') {
					currentSection = line.substr(1, line.size() - 2);
					continue;
				}
				
				// Processes keys=values
				size_t equalsPos = line.find('=');
				if (equalsPos != std::string::npos) {
					std::string key = line.substr(0, equalsPos);
					std::string value = line.substr(equalsPos + 1);
					
					// Processa por seção
					if (currentSection == "Aimbot") {
						if (key == "IgnoreDead") Core::Config::Aimbot::IgnoreDead = (value == "1");
						else if (key == "IgnoreTeam") Core::Config::Aimbot::IgnoreTeam = (value == "1");
						else if (key == "EnableAimbot") Core::Config::Aimbot::EnableAimbot = (value == "1");
						else if (key == "EnableAimbotVisibleCheck") Core::Config::Aimbot::EnableAimbotVisibleCheck = (value == "1");
						else if (key == "AimbotFov") Core::Config::Aimbot::AimbotFov = std::stof(value);
						else if (key == "AimbotSmooth") Core::Config::Aimbot::AimbotSmooth = std::stof(value);
						else if (key == "EnableShowFov") Core::Config::Aimbot::EnableShowFov = (value == "1");
						else if (key == "AimbotKey") Core::Config::Aimbot::AimbotKey = std::stoi(value);
						else if (key == "AimbotFovColor") {
							// Process color (format: r,g,b,a)
							std::string r, g, b, a;
							size_t pos1 = value.find(',');
							r = value.substr(0, pos1);
							size_t pos2 = value.find(',', pos1 + 1);
							g = value.substr(pos1 + 1, pos2 - pos1 - 1);
							size_t pos3 = value.find(',', pos2 + 1);
							b = value.substr(pos2 + 1, pos3 - pos2 - 1);
							a = value.substr(pos3 + 1);
							Core::Config::Aimbot::AimbotFovColor = ImColor(std::stof(r), std::stof(g), std::stof(b), std::stof(a));
						}
					}
					else if (currentSection == "SilentAim") {
						if (key == "EnableSilent") Core::Config::SilentAim::EnableSilent = (value == "1");
						else if (key == "EnableSilentVisibleCheck") Core::Config::SilentAim::EnableSilentVisibleCheck = (value == "1");
						else if (key == "SilentFov") Core::Config::SilentAim::SilentFov = std::stof(value);
						else if (key == "EnableShowSilentFov") Core::Config::SilentAim::EnableShowSilentFov = (value == "1");
						else if (key == "SilentKey") Core::Config::SilentAim::SilentKey = std::stoi(value);
						else if (key == "SilentFovColor") {
							// Process color (format: r,g,b,a)
							std::string r, g, b, a;
							size_t pos1 = value.find(',');
							r = value.substr(0, pos1);
							size_t pos2 = value.find(',', pos1 + 1);
							g = value.substr(pos1 + 1, pos2 - pos1 - 1);
							size_t pos3 = value.find(',', pos2 + 1);
							b = value.substr(pos2 + 1, pos3 - pos2 - 1);
							a = value.substr(pos3 + 1);
							Core::Config::SilentAim::SilentFovColor = ImColor(std::stof(r), std::stof(g), std::stof(b), std::stof(a));
						}
					}
					else if (currentSection == "ESP") {
						if (key == "IgnoreDead") Core::Config::ESP::IgnoreDead = (value == "1");
						else if (key == "IgnoreTeam") Core::Config::ESP::IgnoreTeam = (value == "1");
						else if (key == "EnableESP") Core::Config::ESP::EnableESP = (value == "1");
						else if (key == "EnableBox") Core::Config::ESP::EnableBox = (value == "1");
						else if (key == "EnableLine") Core::Config::ESP::EnableLine = (value == "1");
						else if (key == "EnableHealthBar") Core::Config::ESP::EnableHealthBar = (value == "1");
						else if (key == "EnableDistance") Core::Config::ESP::EnableDistance = (value == "1");
						else if (key == "EnableName") Core::Config::ESP::EnableName = (value == "1");
						else if (key == "EnableChams") Core::Config::ESP::EnableChams = (value == "1");
						else if (key == "CurrentChams") Core::Config::ESP::CurrentChams = std::stoi(value);
						// Colors
						else if (key == "ChamsColor" || key == "ColorBoxVisible" || key == "ColorBoxInvisible" || 
						         key == "ColorLineVisible" || key == "ColorLineInvisible" || key == "ColorHealthBar" || key == "ColorText") {
							// Process color (format: r,g,b,a)
							std::string r, g, b, a;
							size_t pos1 = value.find(',');
							r = value.substr(0, pos1);
							size_t pos2 = value.find(',', pos1 + 1);
							g = value.substr(pos1 + 1, pos2 - pos1 - 1);
							size_t pos3 = value.find(',', pos2 + 1);
							b = value.substr(pos2 + 1, pos3 - pos2 - 1);
							a = value.substr(pos3 + 1);
							ImColor color = ImColor(std::stof(r), std::stof(g), std::stof(b), std::stof(a));
							
							if (key == "ChamsColor") Core::Config::ESP::ChamsColor = color;
							else if (key == "ColorBoxVisible") Core::Config::ESP::ColorBoxVisible = color;
							else if (key == "ColorBoxInvisible") Core::Config::ESP::ColorBoxInvisible = color;
							else if (key == "ColorLineVisible") Core::Config::ESP::ColorLineVisible = color;
							else if (key == "ColorLineInvisible") Core::Config::ESP::ColorLineInvisible = color;
							else if (key == "ColorHealthBar") Core::Config::ESP::ColorHealthBar = color;
							else if (key == "ColorText") Core::Config::ESP::ColorText = color;
						}
					}
					else if (currentSection == "Misc") {
						if (key == "Thirdperson") {
							Core::Config::Misc::Thirdperson = (value == "1");
							Funcs::Write<BYTE>((uintptr_t)Core::ProcessInformations::GameModule + Core::Offsets::Game::Thirdperson, 
							                   Core::Config::Misc::Thirdperson ? 1 : 0);
						}
						else if (key == "CameraFov") {
							Core::Config::Misc::CameraFov = std::stoi(value);
							Funcs::Write<int>((uintptr_t)Core::ProcessInformations::GameModule + Core::Offsets::Game::Fov, 
							                  Core::Config::Misc::CameraFov);
						}
					}
				}
			}
			
			configFile.close();
			return true;
		}
				// List all available configuration files
		std::vector<std::string> ListConfigFiles() {
			std::vector<std::string> configFiles;
			std::string configPath = GetConfigPath();
			std::string searchPattern = configPath + "\\*.cfg";
			
			WIN32_FIND_DATAA findData;
			HANDLE hFind = FindFirstFileA(searchPattern.c_str(), &findData);
			
			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
						std::string filename = findData.cFileName;
						// Remove extension .cfg
						if (filename.length() > 4 && filename.substr(filename.length() - 4) == ".cfg") {
							configFiles.push_back(filename.substr(0, filename.length() - 4));
						}
					}
				} while (FindNextFileA(hFind, &findData));
				FindClose(hFind);
			}
			
			return configFiles;
		}

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
				}				else if (CurrentTab == 2)
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
							ImGui::Separator();
					ImGui::Text("Configuration");
					
					// Input for configuration name
					static char configNameBuffer[64] = "default";
					ImGui::InputText("Config Name", configNameBuffer, IM_ARRAYSIZE(configNameBuffer));
					
					// Status Message
					static char statusMessage[128] = "";
					static float statusTimer = 0.0f;
					
					if (statusTimer > 0.0f) {
						statusTimer -= ImGui::GetIO().DeltaTime;
						ImGui::TextColored(ImVec4(0, 1, 0, 1), statusMessage);  // Green for success
					}
					
					// Save and load buttons
					if (ImGui::Button("Save Config")) {
						SaveConfig(configNameBuffer);
						strcpy_s(statusMessage, "Configuration saved successfully!");
						statusTimer = 3.0f;  // Show message for 3 seconds
					}
					
					ImGui::SameLine();
					
					if (ImGui::Button("Load Config")) {
						if (LoadConfig(configNameBuffer)) {
							strcpy_s(statusMessage, "Configuration loaded successfully!");
							statusTimer = 3.0f;
						} else {
							strcpy_s(statusMessage, "Error: Configuration not found!");
							statusTimer = 3.0f;
						}
					}
					
					// List of available settings
					static int selectedConfig = -1;
					static std::vector<std::string> configFiles;
					static float updateTimer = 0.0f;
					
					// Updates the settings list every 2 seconds
					updateTimer += ImGui::GetIO().DeltaTime;
					if (updateTimer > 2.0f || configFiles.empty()) {
						configFiles = ListConfigFiles();
						updateTimer = 0.0f;
					}
					
					ImGui::Text("Available Configurations:");
					if (ImGui::BeginListBox("##ConfigFiles", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
						for (int i = 0; i < configFiles.size(); i++) {
							const bool isSelected = (selectedConfig == i);
							if (ImGui::Selectable(configFiles[i].c_str(), isSelected)) {
								selectedConfig = i;
								// Copy the name of the selected configuration to the buffer
								strcpy_s(configNameBuffer, configFiles[i].c_str());
							}
							
							if (isSelected) {
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndListBox();
					}
							// Button to delete configuration
					if (selectedConfig >= 0 && selectedConfig < configFiles.size()) {
						if (ImGui::Button("Delete Config")) {
							std::string configPath = GetConfigPath() + "\\" + configFiles[selectedConfig] + ".cfg";
							DeleteFileA(configPath.c_str());
							configFiles = ListConfigFiles();
							selectedConfig = -1;
						}
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
