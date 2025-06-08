#pragma once
#include <Windows.h>
#include <vector>
#include <Psapi.h>

#define M_PI 3.14159265358979323846

namespace Core {
	namespace Funcs {
		uintptr_t FindSignature(const std::vector<uint8_t>& Signature, HMODULE hModule)
		{
			const size_t signatureSize = Signature.size();

			if (signatureSize == 0)
				return 0;

			if (!hModule)
				return 0;

			MODULEINFO modInfo;
			if (!GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(modInfo)))
				return 0;

			uintptr_t base = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
			uintptr_t size = static_cast<uintptr_t>(modInfo.SizeOfImage);

			for (uintptr_t address = base; address <= base + size - signatureSize; ++address)
			{
				bool found = true;

				for (size_t i = 0; i < signatureSize; ++i)
				{
					uint8_t byteInMem = *reinterpret_cast<uint8_t*>(address + i);
					if (Signature[i] != 0x00 && byteInMem != Signature[i])
					{
						found = false;
						break;
					}
				}

				if (found)
					return address;
			}

			return 0;
		}

		std::string ReadString(uintptr_t addr, size_t maxLen = 64)
		{
			if (!addr) return "";

			const char* cstr = reinterpret_cast<const char*>(addr);

			size_t len = strnlen(cstr, maxLen);
			return std::string(cstr, len);
		}

		std::string intToIp(uint32_t num) {
			return std::to_string((num >> 24) & 0xFF) + "." +
				std::to_string((num >> 16) & 0xFF) + "." +
				std::to_string((num >> 8) & 0xFF) + "." +
				std::to_string(num & 0xFF);
		}

		template<typename T>
		T Read(uintptr_t addr) {
			if (!addr || addr > 0x7FFFFFFFFFFF) return T();
			return *(T*)addr;
		}


		template<typename T>
		void Write(uintptr_t addr, T value) {
			*(T*)addr = value;		
		}

		template<typename T>
		T Lerp(T a, T b, float t) {
			return a + (b - a) * t;
		}


		inline bool ReadRaw(uintptr_t address, void* buffer, size_t size)
		{
			if (!address || !buffer || size == 0)
				return false;

			memcpy(buffer, reinterpret_cast<void*>(address), size);
			return true;
		}


		bool WorldToScreen(Vec3 position, Vec2& screen)
		{
			float vm[16];
			if (!ReadRaw((uintptr_t)ProcessInformations::GameModule + Offsets::Game::ViewMatrix, vm, sizeof(vm)))
				return false;

			Vec4 mvp;
			mvp.x = position.x * vm[0] + position.y * vm[4] + position.z * vm[8] + vm[12];
			mvp.y = position.x * vm[1] + position.y * vm[5] + position.z * vm[9] + vm[13];
			mvp.z = position.x * vm[2] + position.y * vm[6] + position.z * vm[10] + vm[14];
			mvp.w = position.x * vm[3] + position.y * vm[7] + position.z * vm[11] + vm[15];

			if (mvp.w < 0.1f) 
				return false;


			Vec2 ndc;
			ndc.x = mvp.x / mvp.w;
			ndc.y = mvp.y / mvp.w;

			int width = Read<int>((uintptr_t)ProcessInformations::GameModule + Offsets::Game::ScreenW);
			int height = Read<int>((uintptr_t)ProcessInformations::GameModule + Offsets::Game::ScreenH);

			screen.x = (width / 2 * ndc.x) + (ndc.x + width / 2);
			screen.y = -(height / 2 * ndc.y) + (ndc.y + height / 2);
			return true;
		}

		
		enum { RAY_BB = 1, RAY_POLY = 3, RAY_ALPHAPOLY = 7, RAY_ENTS = 9, RAY_CLIPMAT = 16, RAY_SKIPFIRST = 32, RAY_EDITMAT = 64, RAY_SHADOW = 128, RAY_PASS = 256, RAY_SKIPSKY = 512, RAY_SKYTEX = 1024 };

		bool IsVisible(const Vec3& Origin, const Vec3& Final) {
			Vec3 Dir = Final - Origin;
			float Distance = Dir.length();
			if (Distance == 0.0f) return true;

			Dir = Dir / Distance;

			float HitDist = ImportedFuncs::RayCube(&Origin, &Dir, Distance, RAY_CLIPMAT | RAY_SKIPFIRST, 8, nullptr);

			return HitDist >= Distance;
		}

	}
}