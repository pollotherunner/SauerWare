#pragma once
#include <Includes.hpp>
#include <thread>
#include <chrono>

namespace Core {
	namespace EntityNamespace {

		inline std::mutex gMutex;

		struct EntityData {
			uintptr_t Addr;
			std::string Name;
			Vec3 Position;
			float Health;
			float MaxHealth;
			bool IsVisible;
			BYTE Flag;
			std::string Team;
			int ClientNum;
		};

		
		inline std::unordered_map<uintptr_t, EntityData> EntityMap;


		Vec3 GetPos(uintptr_t Addr) {
			return Funcs::Read<Vec3>(Addr + Offsets::Game::PlayerPos);
		}
		
		float GetHealth(uintptr_t Addr) {
			return Funcs::Read<float>(Addr + Offsets::Game::PlayerHealth);
		}

		float GetMaxHealth(uintptr_t Addr) {
			return Funcs::Read<float>(Addr + Offsets::Game::PlayerMaxHealth);
		}

		std::string GetName(uintptr_t Addr) {
			return Funcs::ReadString(Addr + Offsets::Game::PlayerName);
		}

		BYTE GetFlag(uintptr_t Addr) {
			return Funcs::Read<BYTE>(Addr + Offsets::Game::PlayerFlag);
		}

		std::string GetTeam(uintptr_t Addr) {
			return Funcs::ReadString(Addr + Offsets::Game::PlayerTeam);
		}

		int GetClientNum(uintptr_t Addr) {
			return Funcs::Read<int>(Addr + Offsets::Game::PlayerClientNum);
		}
		void UpdateEntityList() {
			while (true) {
					uintptr_t EntityListPtr = Funcs::Read<uintptr_t>((uintptr_t)ProcessInformations::GameModule + Offsets::Game::EntityListOffset);
					int EntityCount = Funcs::Read<int>((uintptr_t)ProcessInformations::GameModule + Offsets::Game::EntityCount);

					std::unordered_map<uintptr_t, EntityData> NewEntityMap;

					for (int i = 1; i < EntityCount; i++) {
						uintptr_t EntityAddr = Funcs::Read<uintptr_t>(EntityListPtr + i * 0x8);

						if (!EntityAddr) continue;

						EntityData Data;
						Data.Addr = EntityAddr;
						Data.Name = GetName(EntityAddr);
						Data.Health = GetHealth(EntityAddr);
						Data.MaxHealth = GetMaxHealth(EntityAddr);
						Data.Position = GetPos(EntityAddr);
						Data.Flag = GetFlag(EntityAddr);
						Data.Team = GetTeam(EntityAddr);
						Data.ClientNum = GetClientNum(EntityAddr);
						Data.IsVisible = Funcs::IsVisible(LocalPlayer::Pos, Data.Position);

						NewEntityMap[EntityAddr] = Data;
					}

					{
						std::scoped_lock lock(gMutex);
						EntityMap = std::move(NewEntityMap);
					}
			
				
				std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
			}
		}


		
		void UpdateLocalPlayer() {
			while (true) {
					uintptr_t LocalPlayerAddr = Funcs::Read<uintptr_t>((uintptr_t)ProcessInformations::GameModule + Offsets::Game::LocalPlayer);
					if (LocalPlayerAddr) {
						Core::LocalPlayer::Pos = Funcs::Read<Vec3>(LocalPlayerAddr + Offsets::Game::PlayerPos);
						Core::LocalPlayer::Team = Funcs::ReadString(LocalPlayerAddr + Offsets::Game::PlayerTeam);
					}
			
				
				
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}


		

	}
}
