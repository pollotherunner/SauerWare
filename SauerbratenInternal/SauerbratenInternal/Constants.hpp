#pragma once
#include <vector>
#include <Windows.h>
#include <Dependencies/OpenGLDefines.hpp>
#include <ImGui/imgui.h>

namespace Core {

	namespace Offsets {

		namespace SDL {
			constexpr int SDL_GL_GetProcAddress = 0x226c0;
			constexpr int SDL_WarpMouseInWindow = 0x21380;
			constexpr int SDL_SetRelativeMouseMode = 0x21390;
			constexpr int SDL_ShowCursor = 0x21420;
		}

		namespace Game {
			constexpr int EntityListOffset = 0x346c90;
			constexpr int EntityCount = 0x346C9C;
			constexpr int InterpBones = 0x570c0;

			constexpr int LocalPlayer = 0x2A3528;

			constexpr int ViewMatrix = 0x32D040;

			constexpr int ScreenH = 0x2A3534;
			constexpr int ScreenW = 0x2A3538;

			constexpr int Camera = 0x2a2560;
			constexpr int CamPos = 0x0;
			constexpr int CamYaw = 0x3c;
			constexpr int CamPitch = 0x40;
			constexpr int CamIgnoreWalls = 0x44;
			constexpr int CamVelocity = 0x48;

			constexpr int ServerIp = 0x312980;

			constexpr int Thirdperson = 0x32cfa8;

			constexpr int Fov = 0x32bf34;

			constexpr int PlayerFlag = 0x77;
			constexpr int PlayerTeam = 0x378;
			constexpr int PlayerPos = 0x0;
			constexpr int PlayerName = 0x274;
			constexpr int PlayerAmmo = 0x1AC;
			constexpr int PlayerHealth = 0x178;
			constexpr int PlayerMaxHealth = 0x17C;
			constexpr int PlayerState = 0x77;
			constexpr int PlayerClientNum = 0x1d0;
			constexpr int PlayerPrivilege = 0x1d4;
		}

	}

	namespace Patterns {
		std::vector<uint8_t> InputGrabPattern = { 0x40, 0x53, 0x48, 0x83, 0xEC, 0x00, 0x0F, 0xB6, 0xDA, 0x84, 0xC9 };
		std::vector<uint8_t> InterpBonesPattern = { 0x48, 0x8b, 0xc4, 0x4c, 0x89, 0x48, 0x00, 0xf3, 0x0f, 0x11, 0x50 };
		std::vector<uint8_t> ShootPattern = { 0x40, 0x55, 0x53, 0x41, 0x56 };
		std::vector<uint8_t> RenderModelPattern = { 0x48, 0x8B, 0xC4, 0x4C, 0x89, 0x48, 0x00, 0x44, 0x89, 0x40, 0x00, 0x55 };
		std::vector<uint8_t> RayCubePattern = { 0x48, 0x8B, 0xC4, 0x44, 0x89, 0x48, 0x00, 0x48, 0x89, 0x50 };
		std::vector<uint8_t> SkeletonRenderPattern = { 0x48, 0x8B, 0xC4, 0x48, 0x89, 0x50, 0x00 , 0x57 };
	
	}

	namespace LocalPlayer {
		Vec3 Pos;
		std::string Team;
	}

	namespace Config {




		namespace Menu {
			bool ShowMenu = true;
		}
		namespace Aimbot {

			bool IgnoreDead = true;
			bool IgnoreTeam = true;

			bool EnableAimbot = false;
			bool EnableAimbotVisibleCheck = false;
			float AimbotFov = 20;
			float AimbotSmooth = 1;
			bool EnableShowFov = false;
			int AimbotKey = 0x00; 
			ImColor AimbotFovColor = ImColor(255, 255, 255, 255);
		}
		namespace SilentAim {
			bool EnableSilent = false;
			bool EnableSilentVisibleCheck = false;
			float SilentFov = 20;
			bool EnableShowSilentFov = false;
			int SilentKey = 0x00;
			ImColor SilentFovColor = ImColor(255, 0, 0, 255);
		}
		namespace ESP {
			bool IgnoreDead = true;
			bool IgnoreTeam = true;
			bool EnableESP = true;
			bool EnableBox = true;
			bool EnableLine = true;
			bool EnableHealthBar = true;
			bool EnableDistance = true;
			bool EnableName = true;
			bool EnableChams = false;
			int	 CurrentChams = 0;
			ImColor ChamsColor = ImColor(255, 0, 0, 255);
			ImColor ColorBoxVisible = ImColor(0, 255, 0, 255);
			ImColor ColorBoxInvisible = ImColor(255, 0, 0, 255);
			ImColor ColorLineVisible = ImColor(0, 255, 0, 255);
			ImColor ColorLineInvisible = ImColor(255, 0, 0, 255);
			ImColor ColorHealthBar = ImColor(0, 255, 0, 255);
			ImColor ColorText = ImColor(255, 255, 255, 255);
		}

		namespace Misc {
			bool Thirdperson = false;
			int CameraFov = 100;
		}
	
	}

	namespace ProcessInformations {

		HMODULE SDL2_Module = nullptr;
		HMODULE GameModule = nullptr;
		HWND Hwnd = nullptr;
		HANDLE GameHandle = nullptr;
		WNDPROC hGameWindowProc;
	}

	namespace ImportedFuncs {

		// ------------------------------
		// SDL Functions
		// ------------------------------
		typedef void* (*SDL_GL_GetProcAddress_t)(const char* name);
		typedef void (*SDL_WarpMouseInWindow_t)(SDL_Window* window, int x, int y);
		typedef int  (*SDL_SetRelativeMouseMode_t)(bool enabled);
		typedef int  (*SDL_ShowCursor_t)(int toggle);

		SDL_GL_GetProcAddress_t       SDL_GL_GetProc = nullptr;
		SDL_WarpMouseInWindow_t       SDL_WarpMouseInWindowAddress = nullptr;
		SDL_SetRelativeMouseMode_t    SDL_SetRelativeMouseModeAddress = nullptr;
		SDL_ShowCursor_t              SDL_ShowCursor = nullptr;

		SDL_WarpMouseInWindow_t       SDL_WarpMouseInWindowOriginal = nullptr;
		SDL_SetRelativeMouseMode_t    SDL_SetRelativeMouseModeOriginal = nullptr;


		// ------------------------------
		// OpenGL Functions
		// ------------------------------
		typedef void(__stdcall* glDrawRangeElements_t)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices);
		typedef void(__stdcall* glEnable_t)(GLenum cap);
		typedef void(__stdcall* glDisable_t)(GLenum cap);
		typedef void(__stdcall* glBlendFunc_t)(GLenum sfactor, GLenum dfactor);
		typedef BOOL(__stdcall* wglSwapBuffers_t)(HDC hdc);
		
		glDrawRangeElements_t     glDrawRangeElementsAddress = nullptr;
		glEnable_t                glEnable = nullptr;
		glDisable_t               glDisable = nullptr;
		glBlendFunc_t             glBlendFunc = nullptr;
		wglSwapBuffers_t          wglSwapBuffersAddress = nullptr;

		glDrawRangeElements_t     glDrawRangeElementsOriginal = nullptr;
		wglSwapBuffers_t          wglSwapBuffersOriginal = nullptr;


		// ------------------------------
		// Game/Internal Functions
		// ------------------------------
		typedef __int64(__fastcall* InputGrab_t)(bool on, bool delay);
		typedef __int64(__fastcall* Shoot_t)(void* entity, Vec3* target);
		typedef void(__fastcall* SkeletonRender_t)(void* skel,const void* as,float pitch,const Vec3* axis,const Vec3* forward, void* d,void* p);
		typedef float(__fastcall* RayCube_t)(const Vec3* a1, const Vec3* a2, float a3, int a4, int size, void* t);
		typedef void(__fastcall* RenderModel_t)(void* p_light,const char* a2,int a3,const Vec3* a4,float yaw,float pitch,int flags,void* d,void* a,unsigned int basetime,unsigned int basetime2,float trans);
		//typedef void(__fastcall* InterpBones_t)(void* skel,const void* as,float pitch,const void* axis,const void* forward,int numanimparts,const unsigned __int8* partmask, void* sc);

		//InterpBones_t InterpBonesAddress = nullptr;
		InputGrab_t InputGrabAddress = nullptr;
		Shoot_t ShootAddress = nullptr;
		RayCube_t RayCube = nullptr;
		RenderModel_t RenderModelAddress = nullptr;
		SkeletonRender_t SkeletonRenderAddress = nullptr;

		SkeletonRender_t SkeletonRenderOriginal = nullptr;
		RenderModel_t RenderModelOriginal = nullptr;
		//InterpBones_t InterpBonesOriginal = nullptr;
		InputGrab_t InputGrabOriginal = nullptr;
		Shoot_t ShootOriginal = nullptr;

	}
}