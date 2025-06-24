#pragma once
#include <Includes.hpp>

namespace Core {
	namespace ConfigManager {
		
		std::string GetConfigPath() {
			
			char TempPath[MAX_PATH];
			DWORD Result = GetTempPathA(MAX_PATH, TempPath);
					if (Result == 0) {
				std::cout << "[-] Error: Unable to get temp path" << '\n';
				
				char* UserProfile = nullptr;
				size_t Len = 0;
				if (_dupenv_s(&UserProfile, &Len, "USERPROFILE") == 0 && UserProfile) {					
                    std::string DirectoryPath = std::string(UserProfile) + "\\SauerWareConfig";
					std::cout << "[+] Using user folder: " << DirectoryPath << '\n';

					BOOL CreateResult = CreateDirectoryA(DirectoryPath.c_str(), NULL);
					DWORD Error = GetLastError();					if (CreateResult) {
						std::cout << "[+] Directory created successfully" << '\n';
					}
					else if (Error == ERROR_ALREADY_EXISTS) {
						std::cout << "[+] Directory already exists" << '\n';
					}
					else {
						std::cout << "[-] Error creating directory. Code: " << Error << '\n';
					}

					free(UserProfile); 
					return DirectoryPath;
				}
				return "";
			}
					std::string DirectoryPath = std::string(TempPath) + "SauerWareConfig";
			
			
			
			BOOL CreateResult = CreateDirectoryA(DirectoryPath.c_str(), NULL);
			DWORD Error = GetLastError();
					if (CreateResult) {
				std::cout << "[+] Directory created successfully" << '\n';
			} else if (Error == ERROR_ALREADY_EXISTS) {

			} else {
				std::cout << "[-] Error creating directory. Code: " << Error << '\n';
			}
			return DirectoryPath;
		}
		void CreateConfig(const std::string& ConfigName) {
			try {
				std::string ConfigPath = GetConfigPath();
				if (ConfigPath.empty()) {
					std::cout << "[-] Empty configuration path" << '\n';
					return;
				}
				
				std::string FullPath = ConfigPath + "\\" + ConfigName + ".cfg";
				
				std::cout << "[+] Trying to save config in: " << FullPath << '\n';
				
				std::string TestFile = ConfigPath + "\\test_write.tmp";
				std::ofstream TestStream(TestFile);
				if (TestStream.is_open()) {
					TestStream << "test";
					TestStream.close();
					DeleteFileA(TestFile.c_str());
					std::cout << "[+] Directory writing test: yes" << '\n';
				} else {
					std::cout << "[-] Directory writing test: no" << '\n';
				}
				
				std::ofstream ConfigFile(FullPath, std::ios::out | std::ios::trunc);
				
				if (!ConfigFile.is_open()) {
					std::cout << "[-] Error: Could not open file for writing" << '\n';
					
					
					HANDLE HFile = CreateFileA(FullPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (HFile == INVALID_HANDLE_VALUE) {
						DWORD Error = GetLastError();
						std::cout << "[-] Erro CreateFile: " << Error << '\n';
						return;
					}
					CloseHandle(HFile);
					
					
					ConfigFile.open(FullPath, std::ios::out | std::ios::trunc);
					if (!ConfigFile.is_open()) {
						std::cout << "[-] Error: Still unable to open after CreateFile" << '\n';
						return;}
				}
				
				std::cout << "[+] File opened successfully, saving settings..." << '\n';
						
				ConfigFile << "[Aimbot]" << std::endl;
				ConfigFile << "IgnoreDead=" << (Core::Config::Aimbot::IgnoreDead ? "1" : "0") << std::endl;
				ConfigFile << "IgnoreTeam=" << (Core::Config::Aimbot::IgnoreTeam ? "1" : "0") << std::endl;
				ConfigFile << "EnableAimbot=" << (Core::Config::Aimbot::EnableAimbot ? "1" : "0") << std::endl;
				ConfigFile << "EnableAimbotVisibleCheck=" << (Core::Config::Aimbot::EnableAimbotVisibleCheck ? "1" : "0") << std::endl;
				ConfigFile << "AimbotFov=" << Core::Config::Aimbot::AimbotFov << std::endl;
				ConfigFile << "AimbotSmooth=" << Core::Config::Aimbot::AimbotSmooth << std::endl;
				ConfigFile << "EnableShowFov=" << (Core::Config::Aimbot::EnableShowFov ? "1" : "0") << std::endl;
				ConfigFile << "AimbotKey=" << Core::Config::Aimbot::AimbotKey << std::endl;
				ConfigFile << "AimbotFovColor=" << Core::Config::Aimbot::AimbotFovColor.Value.x << "," 
				                                << Core::Config::Aimbot::AimbotFovColor.Value.y << "," 
												<< Core::Config::Aimbot::AimbotFovColor.Value.z << "," 
												<< Core::Config::Aimbot::AimbotFovColor.Value.w << std::endl;
				
				
				ConfigFile << std::endl << "[SilentAim]" << std::endl;
				ConfigFile << "EnableSilent=" << (Core::Config::SilentAim::EnableSilent ? "1" : "0") << std::endl;
				ConfigFile << "EnableSilentVisibleCheck=" << (Core::Config::SilentAim::EnableSilentVisibleCheck ? "1" : "0") << std::endl;
				ConfigFile << "SilentFov=" << Core::Config::SilentAim::SilentFov << std::endl;
				ConfigFile << "EnableShowSilentFov=" << (Core::Config::SilentAim::EnableShowSilentFov ? "1" : "0") << std::endl;
				ConfigFile << "SilentKey=" << Core::Config::SilentAim::SilentKey << std::endl;
				ConfigFile << "SilentFovColor=" << Core::Config::SilentAim::SilentFovColor.Value.x << "," 
				                               << Core::Config::SilentAim::SilentFovColor.Value.y << "," 
											   << Core::Config::SilentAim::SilentFovColor.Value.z << "," 
											   << Core::Config::SilentAim::SilentFovColor.Value.w << std::endl;						
				ConfigFile << std::endl << "[ESP]" << std::endl;
				ConfigFile << "IgnoreDead=" << (Core::Config::ESP::IgnoreDead ? "1" : "0") << std::endl;
				ConfigFile << "IgnoreTeam=" << (Core::Config::ESP::IgnoreTeam ? "1" : "0") << std::endl;
				ConfigFile << "EnableESP=" << (Core::Config::ESP::EnableESP ? "1" : "0") << std::endl;
				ConfigFile << "EnableBox=" << (Core::Config::ESP::EnableBox ? "1" : "0") << std::endl;
				ConfigFile << "EnableLine=" << (Core::Config::ESP::EnableLine ? "1" : "0") << std::endl;
				ConfigFile << "EnableHealthBar=" << (Core::Config::ESP::EnableHealthBar ? "1" : "0") << std::endl;
				ConfigFile << "EnableDistance=" << (Core::Config::ESP::EnableDistance ? "1" : "0") << std::endl;
				ConfigFile << "EnableName=" << (Core::Config::ESP::EnableName ? "1" : "0") << std::endl;
				ConfigFile << "EnableChams=" << (Core::Config::ESP::EnableChams ? "1" : "0") << std::endl;
				ConfigFile << "CurrentChams=" << Core::Config::ESP::CurrentChams << std::endl;
				ConfigFile << "ChamsColor=" << Core::Config::ESP::ChamsColor.Value.x << "," 
				                          << Core::Config::ESP::ChamsColor.Value.y << "," 
										  << Core::Config::ESP::ChamsColor.Value.z << "," 
										  << Core::Config::ESP::ChamsColor.Value.w << std::endl;
				ConfigFile << "ColorBoxVisible=" << Core::Config::ESP::ColorBoxVisible.Value.x << "," 
				                               << Core::Config::ESP::ColorBoxVisible.Value.y << "," 
											   << Core::Config::ESP::ColorBoxVisible.Value.z << "," 
											   << Core::Config::ESP::ColorBoxVisible.Value.w << std::endl;
				ConfigFile << "ColorBoxInvisible=" << Core::Config::ESP::ColorBoxInvisible.Value.x << "," 
				                                 << Core::Config::ESP::ColorBoxInvisible.Value.y << "," 
												 << Core::Config::ESP::ColorBoxInvisible.Value.z << "," 
												 << Core::Config::ESP::ColorBoxInvisible.Value.w << std::endl;
				ConfigFile << "ColorLineVisible=" << Core::Config::ESP::ColorLineVisible.Value.x << "," 
				                                << Core::Config::ESP::ColorLineVisible.Value.y << "," 
												<< Core::Config::ESP::ColorLineVisible.Value.z << "," 
												<< Core::Config::ESP::ColorLineVisible.Value.w << std::endl;
				ConfigFile << "ColorLineInvisible=" << Core::Config::ESP::ColorLineInvisible.Value.x << "," 
				                                  << Core::Config::ESP::ColorLineInvisible.Value.y << "," 
												  << Core::Config::ESP::ColorLineInvisible.Value.z << "," 
												  << Core::Config::ESP::ColorLineInvisible.Value.w << std::endl;
				ConfigFile << "ColorHealthBar=" << Core::Config::ESP::ColorHealthBar.Value.x << "," 
				                              << Core::Config::ESP::ColorHealthBar.Value.y << "," 
											  << Core::Config::ESP::ColorHealthBar.Value.z << "," 
											  << Core::Config::ESP::ColorHealthBar.Value.w << std::endl;
				ConfigFile << "ColorText=" << Core::Config::ESP::ColorText.Value.x << "," 
				                         << Core::Config::ESP::ColorText.Value.y << "," 
										 << Core::Config::ESP::ColorText.Value.z << "," 
										 << Core::Config::ESP::ColorText.Value.w << std::endl;
						
				ConfigFile << std::endl << "[Misc]" << std::endl;
				ConfigFile << "Thirdperson=" << (Core::Config::Misc::Thirdperson ? "1" : "0") << std::endl;
				ConfigFile << "CameraFov=" << Core::Config::Misc::CameraFov << std::endl;
				
				
				if (ConfigFile.fail()) {
					std::cout << "[-] Error writing to file" << '\n';
				}
				
				ConfigFile.flush();
				ConfigFile.close();
				
				
				DWORD Attributes = GetFileAttributesA(FullPath.c_str());
				if (Attributes != INVALID_FILE_ATTRIBUTES) {
					std::cout << "[+] Configuration saved successfully! File verified." << '\n';
				} else {
					std::cout << "[-] Error: File not created even after apparent success" << '\n';
				}
			}
			catch (const std::exception& e) {				std::cout << "[-] Error saving configuration: " << e.what() << '\n';
			}
		}
				bool LoadConfig(const std::string& ConfigName) {
			std::string ConfigPath = GetConfigPath() + "\\" + ConfigName + ".cfg";
			std::ifstream ConfigFile(ConfigPath);
			
			if (!ConfigFile.is_open()) {
				return false;
			}
			
			std::string Line;
			std::string CurrentSection;
			
			while (std::getline(ConfigFile, Line)) {
				
				if (Line.empty())
					continue;
				
				
				if (Line[0] == '[' && Line.back() == ']') {
					CurrentSection = Line.substr(1, Line.size() - 2);
					continue;
				}
				
				
				size_t EqualsPos = Line.find('=');
				if (EqualsPos != std::string::npos) {
					std::string Key = Line.substr(0, EqualsPos);
					std::string Value = Line.substr(EqualsPos + 1);
					
					
					if (CurrentSection == "Aimbot") {
						if (Key == "IgnoreDead") Core::Config::Aimbot::IgnoreDead = (Value == "1");
						else if (Key == "IgnoreTeam") Core::Config::Aimbot::IgnoreTeam = (Value == "1");
						else if (Key == "EnableAimbot") Core::Config::Aimbot::EnableAimbot = (Value == "1");
						else if (Key == "EnableAimbotVisibleCheck") Core::Config::Aimbot::EnableAimbotVisibleCheck = (Value == "1");
						else if (Key == "AimbotFov") Core::Config::Aimbot::AimbotFov = std::stof(Value);
						else if (Key == "AimbotSmooth") Core::Config::Aimbot::AimbotSmooth = std::stof(Value);
						else if (Key == "EnableShowFov") Core::Config::Aimbot::EnableShowFov = (Value == "1");
						else if (Key == "AimbotKey") Core::Config::Aimbot::AimbotKey = std::stoi(Value);
						else if (Key == "AimbotFovColor") {
							
							std::string R, G, B, A;
							size_t Pos1 = Value.find(',');
							R = Value.substr(0, Pos1);
							size_t Pos2 = Value.find(',', Pos1 + 1);
							G = Value.substr(Pos1 + 1, Pos2 - Pos1 - 1);
							size_t Pos3 = Value.find(',', Pos2 + 1);
							B = Value.substr(Pos2 + 1, Pos3 - Pos2 - 1);
							A = Value.substr(Pos3 + 1);
							Core::Config::Aimbot::AimbotFovColor = ImColor(std::stof(R), std::stof(G), std::stof(B), std::stof(A));
						}
					}
					else if (CurrentSection == "SilentAim") {
						if (Key == "EnableSilent") Core::Config::SilentAim::EnableSilent = (Value == "1");
						else if (Key == "EnableSilentVisibleCheck") Core::Config::SilentAim::EnableSilentVisibleCheck = (Value == "1");
						else if (Key == "SilentFov") Core::Config::SilentAim::SilentFov = std::stof(Value);
						else if (Key == "EnableShowSilentFov") Core::Config::SilentAim::EnableShowSilentFov = (Value == "1");
						else if (Key == "SilentKey") Core::Config::SilentAim::SilentKey = std::stoi(Value);
						else if (Key == "SilentFovColor") {
							
							std::string R, G, B, A;
							size_t Pos1 = Value.find(',');
							R = Value.substr(0, Pos1);
							size_t Pos2 = Value.find(',', Pos1 + 1);
							G = Value.substr(Pos1 + 1, Pos2 - Pos1 - 1);
							size_t Pos3 = Value.find(',', Pos2 + 1);
							B = Value.substr(Pos2 + 1, Pos3 - Pos2 - 1);
							A = Value.substr(Pos3 + 1);
							Core::Config::SilentAim::SilentFovColor = ImColor(std::stof(R), std::stof(G), std::stof(B), std::stof(A));						}
					}
					else if (CurrentSection == "ESP") {
						if (Key == "IgnoreDead") Core::Config::ESP::IgnoreDead = (Value == "1");
						else if (Key == "IgnoreTeam") Core::Config::ESP::IgnoreTeam = (Value == "1");
						else if (Key == "EnableESP") Core::Config::ESP::EnableESP = (Value == "1");
						else if (Key == "EnableBox") Core::Config::ESP::EnableBox = (Value == "1");
						else if (Key == "EnableLine") Core::Config::ESP::EnableLine = (Value == "1");
						else if (Key == "EnableHealthBar") Core::Config::ESP::EnableHealthBar = (Value == "1");
						else if (Key == "EnableDistance") Core::Config::ESP::EnableDistance = (Value == "1");
						else if (Key == "EnableName") Core::Config::ESP::EnableName = (Value == "1");
						else if (Key == "EnableChams") Core::Config::ESP::EnableChams = (Value == "1");
						else if (Key == "CurrentChams") Core::Config::ESP::CurrentChams = std::stoi(Value);
						
						else if (Key == "ChamsColor" || Key == "ColorBoxVisible" || Key == "ColorBoxInvisible" || 
						         Key == "ColorLineVisible" || Key == "ColorLineInvisible" || Key == "ColorHealthBar" || Key == "ColorText") {
							
							std::string R, G, B, A;
							size_t Pos1 = Value.find(',');
							R = Value.substr(0, Pos1);
							size_t Pos2 = Value.find(',', Pos1 + 1);
							G = Value.substr(Pos1 + 1, Pos2 - Pos1 - 1);
							size_t Pos3 = Value.find(',', Pos2 + 1);
							B = Value.substr(Pos2 + 1, Pos3 - Pos2 - 1);
							A = Value.substr(Pos3 + 1);
							ImColor Color = ImColor(std::stof(R), std::stof(G), std::stof(B), std::stof(A));
							
							if (Key == "ChamsColor") Core::Config::ESP::ChamsColor = Color;
							else if (Key == "ColorBoxVisible") Core::Config::ESP::ColorBoxVisible = Color;
							else if (Key == "ColorBoxInvisible") Core::Config::ESP::ColorBoxInvisible = Color;
							else if (Key == "ColorLineVisible") Core::Config::ESP::ColorLineVisible = Color;
							else if (Key == "ColorLineInvisible") Core::Config::ESP::ColorLineInvisible = Color;
							else if (Key == "ColorHealthBar") Core::Config::ESP::ColorHealthBar = Color;
							else if (Key == "ColorText") Core::Config::ESP::ColorText = Color;
						}
					}
					else if (CurrentSection == "Misc") {
						if (Key == "Thirdperson") {
							Core::Config::Misc::Thirdperson = (Value == "1");
							Funcs::Write<BYTE>((uintptr_t)Core::ProcessInformations::GameModule + Core::Offsets::Game::Thirdperson, 
							                   Core::Config::Misc::Thirdperson ? 1 : 0);
						}
						else if (Key == "CameraFov") {
							Core::Config::Misc::CameraFov = std::stoi(Value);
							Funcs::Write<int>((uintptr_t)Core::ProcessInformations::GameModule + Core::Offsets::Game::Fov, 
							                  Core::Config::Misc::CameraFov);
						}
					}
				}
			}
					ConfigFile.close();
			return true;
		}
		
		void SaveConfig(const std::string& ConfigName) {
			try {
				std::string ConfigPath = GetConfigPath();
				if (ConfigPath.empty()) {
					std::cout << "[-] Empty configuration path" << '\n';
					return;
				}
				
				std::string FullPath = ConfigPath + "\\" + ConfigName + ".cfg";
				
				std::cout << "[+] Trying to save config in: " << FullPath << '\n';
				
				std::ofstream ConfigFile(FullPath, std::ios::out | std::ios::trunc);
				
				if (!ConfigFile.is_open()) {
					std::cout << "[-] Error: Could not open file for writing" << '\n';
					
					HANDLE HFile = CreateFileA(FullPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (HFile == INVALID_HANDLE_VALUE) {
						DWORD Error = GetLastError();
						std::cout << "[-] Error CreateFile: " << Error << '\n';
						return;
					}
					CloseHandle(HFile);
					
					ConfigFile.open(FullPath, std::ios::out | std::ios::trunc);
					if (!ConfigFile.is_open()) {
						std::cout << "[-] Error: Still unable to open after CreateFile" << '\n';
						return;
					}
				}
				
				std::cout << "[+] File opened successfully, saving settings..." << '\n';
						
				ConfigFile << "[Aimbot]" << std::endl;
				ConfigFile << "IgnoreDead=" << (Core::Config::Aimbot::IgnoreDead ? "1" : "0") << std::endl;
				ConfigFile << "IgnoreTeam=" << (Core::Config::Aimbot::IgnoreTeam ? "1" : "0") << std::endl;
				ConfigFile << "EnableAimbot=" << (Core::Config::Aimbot::EnableAimbot ? "1" : "0") << std::endl;
				ConfigFile << "EnableAimbotVisibleCheck=" << (Core::Config::Aimbot::EnableAimbotVisibleCheck ? "1" : "0") << std::endl;
				ConfigFile << "AimbotFov=" << Core::Config::Aimbot::AimbotFov << std::endl;
				ConfigFile << "AimbotSmooth=" << Core::Config::Aimbot::AimbotSmooth << std::endl;
				ConfigFile << "EnableShowFov=" << (Core::Config::Aimbot::EnableShowFov ? "1" : "0") << std::endl;
				ConfigFile << "AimbotKey=" << Core::Config::Aimbot::AimbotKey << std::endl;
				ConfigFile << "AimbotFovColor=" << Core::Config::Aimbot::AimbotFovColor.Value.x << "," 
				                                << Core::Config::Aimbot::AimbotFovColor.Value.y << "," 
												<< Core::Config::Aimbot::AimbotFovColor.Value.z << "," 
												<< Core::Config::Aimbot::AimbotFovColor.Value.w << std::endl;
				
				ConfigFile << std::endl << "[SilentAim]" << std::endl;
				ConfigFile << "EnableSilent=" << (Core::Config::SilentAim::EnableSilent ? "1" : "0") << std::endl;
				ConfigFile << "EnableSilentVisibleCheck=" << (Core::Config::SilentAim::EnableSilentVisibleCheck ? "1" : "0") << std::endl;
				ConfigFile << "SilentFov=" << Core::Config::SilentAim::SilentFov << std::endl;
				ConfigFile << "EnableShowSilentFov=" << (Core::Config::SilentAim::EnableShowSilentFov ? "1" : "0") << std::endl;
				ConfigFile << "SilentKey=" << Core::Config::SilentAim::SilentKey << std::endl;
				ConfigFile << "SilentFovColor=" << Core::Config::SilentAim::SilentFovColor.Value.x << "," 
				                               << Core::Config::SilentAim::SilentFovColor.Value.y << "," 
											   << Core::Config::SilentAim::SilentFovColor.Value.z << "," 
											   << Core::Config::SilentAim::SilentFovColor.Value.w << std::endl;
											   
				ConfigFile << std::endl << "[ESP]" << std::endl;
				ConfigFile << "IgnoreDead=" << (Core::Config::ESP::IgnoreDead ? "1" : "0") << std::endl;
				ConfigFile << "IgnoreTeam=" << (Core::Config::ESP::IgnoreTeam ? "1" : "0") << std::endl;
				ConfigFile << "EnableESP=" << (Core::Config::ESP::EnableESP ? "1" : "0") << std::endl;
				ConfigFile << "EnableBox=" << (Core::Config::ESP::EnableBox ? "1" : "0") << std::endl;
				ConfigFile << "EnableLine=" << (Core::Config::ESP::EnableLine ? "1" : "0") << std::endl;
				ConfigFile << "EnableHealthBar=" << (Core::Config::ESP::EnableHealthBar ? "1" : "0") << std::endl;
				ConfigFile << "EnableDistance=" << (Core::Config::ESP::EnableDistance ? "1" : "0") << std::endl;
				ConfigFile << "EnableName=" << (Core::Config::ESP::EnableName ? "1" : "0") << std::endl;
				ConfigFile << "EnableChams=" << (Core::Config::ESP::EnableChams ? "1" : "0") << std::endl;
				ConfigFile << "CurrentChams=" << Core::Config::ESP::CurrentChams << std::endl;
				ConfigFile << "ChamsColor=" << Core::Config::ESP::ChamsColor.Value.x << "," 
				                          << Core::Config::ESP::ChamsColor.Value.y << "," 
										  << Core::Config::ESP::ChamsColor.Value.z << "," 
										  << Core::Config::ESP::ChamsColor.Value.w << std::endl;
				ConfigFile << "ColorBoxVisible=" << Core::Config::ESP::ColorBoxVisible.Value.x << "," 
				                               << Core::Config::ESP::ColorBoxVisible.Value.y << "," 
											   << Core::Config::ESP::ColorBoxVisible.Value.z << "," 
											   << Core::Config::ESP::ColorBoxVisible.Value.w << std::endl;
				ConfigFile << "ColorBoxInvisible=" << Core::Config::ESP::ColorBoxInvisible.Value.x << "," 
				                                 << Core::Config::ESP::ColorBoxInvisible.Value.y << "," 
												 << Core::Config::ESP::ColorBoxInvisible.Value.z << "," 
												 << Core::Config::ESP::ColorBoxInvisible.Value.w << std::endl;
				ConfigFile << "ColorLineVisible=" << Core::Config::ESP::ColorLineVisible.Value.x << "," 
				                                << Core::Config::ESP::ColorLineVisible.Value.y << "," 
												<< Core::Config::ESP::ColorLineVisible.Value.z << "," 
												<< Core::Config::ESP::ColorLineVisible.Value.w << std::endl;
				ConfigFile << "ColorLineInvisible=" << Core::Config::ESP::ColorLineInvisible.Value.x << "," 
				                                  << Core::Config::ESP::ColorLineInvisible.Value.y << "," 
												  << Core::Config::ESP::ColorLineInvisible.Value.z << "," 
												  << Core::Config::ESP::ColorLineInvisible.Value.w << std::endl;
				ConfigFile << "ColorHealthBar=" << Core::Config::ESP::ColorHealthBar.Value.x << "," 
				                              << Core::Config::ESP::ColorHealthBar.Value.y << "," 
											  << Core::Config::ESP::ColorHealthBar.Value.z << "," 
											  << Core::Config::ESP::ColorHealthBar.Value.w << std::endl;
				ConfigFile << "ColorText=" << Core::Config::ESP::ColorText.Value.x << "," 
				                         << Core::Config::ESP::ColorText.Value.y << "," 
										 << Core::Config::ESP::ColorText.Value.z << "," 
										 << Core::Config::ESP::ColorText.Value.w << std::endl;
						
				ConfigFile << std::endl << "[Misc]" << std::endl;
				ConfigFile << "Thirdperson=" << (Core::Config::Misc::Thirdperson ? "1" : "0") << std::endl;
				ConfigFile << "CameraFov=" << Core::Config::Misc::CameraFov << std::endl;
				
				if (ConfigFile.fail()) {
					std::cout << "[-] Error writing to file" << '\n';
				}
				
				ConfigFile.flush();
				ConfigFile.close();
				
				DWORD Attributes = GetFileAttributesA(FullPath.c_str());
				if (Attributes != INVALID_FILE_ATTRIBUTES) {
					std::cout << "[+] Configuration saved successfully! File verified." << '\n';
				} else {
					std::cout << "[-] Error: File not created even after apparent success" << '\n';
				}
			}
			catch (const std::exception& e) {
				std::cout << "[-] Error saving configuration: " << e.what() << '\n';
			}
		}
		
		std::vector<std::string> ListConfigFiles() {
			std::vector<std::string> ConfigFiles;
			std::string ConfigPath = GetConfigPath();
			std::string SearchPattern = ConfigPath + "\\*.cfg";
			
			WIN32_FIND_DATAA FindData;
			HANDLE HFind = FindFirstFileA(SearchPattern.c_str(), &FindData);
			
			if (HFind != INVALID_HANDLE_VALUE) {
				do {
					if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
						std::string Filename = FindData.cFileName;
						
						if (Filename.length() > 4 && Filename.substr(Filename.length() - 4) == ".cfg") {
							ConfigFiles.push_back(Filename.substr(0, Filename.length() - 4));
						}
					}
				} while (FindNextFileA(HFind, &FindData));
				FindClose(HFind);
			}
			
			return ConfigFiles;
		}
	}
}
