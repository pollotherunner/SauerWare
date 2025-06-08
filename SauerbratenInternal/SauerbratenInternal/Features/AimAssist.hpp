#pragma once
#include <Includes.hpp>
#include <Entity.hpp>

bool GetBestTarget(Core::EntityNamespace::EntityData& OutEntity, float ArgFov, bool IsVisible) {
    std::scoped_lock lock(Core::EntityNamespace::gMutex);

    if (Core::EntityNamespace::EntityMap.empty()) return false;

    ImVec2 ScreenCenter = { ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2 };

    float BestFov = ArgFov;
    bool Found = false;

    for (const auto& Entity : Core::EntityNamespace::EntityMap)
    {
        Vec3 EntityPos = Entity.second.Position;

        if (Entity.second.Flag == 1 && Core::Config::Aimbot::IgnoreDead)continue;
        if ((Core::LocalPlayer::Team == Entity.second.Team) && Core::Config::Aimbot::IgnoreTeam) continue;

        if (IsVisible && !Entity.second.IsVisible) continue;

        Vec2 EntityToScreen;

        if (!Core::Funcs::WorldToScreen(EntityPos, EntityToScreen)) continue;

        float Fov = std::hypot(EntityToScreen.x - ScreenCenter.x, EntityToScreen.y - ScreenCenter.y);

        if (Fov < BestFov) {
            BestFov = Fov;
            OutEntity = Entity.second;
            Found = true;
        }
    }

    return Found;
}

float NormalizeAngle(float angle) {
    while (angle > 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}



namespace Features {
    namespace AimAssist {
        void RunAimbot() {
            while (true) {
                if (Core::Config::Aimbot::EnableAimbot && (GetAsyncKeyState(Core::Config::Aimbot::AimbotKey) & 0x8000)){

                    uintptr_t PlayerCamera = Core::Funcs::Read<uintptr_t>((uintptr_t)Core::ProcessInformations::GameModule + Core::Offsets::Game::Camera);

                    Core::EntityNamespace::EntityData Entity;
                    if (GetBestTarget(Entity, Core::Config::Aimbot::AimbotFov,Core::Config::Aimbot::EnableAimbotVisibleCheck)) {
                        Vec3 LocalPlayerPos = Core::LocalPlayer::Pos;
                        Vec3 EntityPos = Entity.Position;

                        Vec3 Delta = EntityPos - LocalPlayerPos;

                        float Yaw = -atan2(Delta.x, Delta.y);
                        float Pitch = atan2(Delta.z, sqrt(Delta.x * Delta.x + Delta.y * Delta.y));

                        float YawDeg = Yaw * 57.2957795f;
                        float PitchDeg = Pitch * 57.2957795f;

                        float CurrentYaw = Core::Funcs::Read<float>(PlayerCamera + Core::Offsets::Game::CamYaw);
                        float CurrentPitch = Core::Funcs::Read<float>(PlayerCamera + Core::Offsets::Game::CamPitch);

                        float YawDelta = NormalizeAngle(YawDeg - CurrentYaw);
                        float PitchDelta = NormalizeAngle(PitchDeg - CurrentPitch);

                        float SmoothYaw = CurrentYaw + YawDelta / Core::Config::Aimbot::AimbotSmooth;
                        float SmoothPitch = CurrentPitch + PitchDelta / Core::Config::Aimbot::AimbotSmooth;


                        Core::Funcs::Write<float>(PlayerCamera + Core::Offsets::Game::CamYaw, SmoothYaw);
                        Core::Funcs::Write<float>(PlayerCamera + Core::Offsets::Game::CamPitch, SmoothPitch);
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
           
            
        }

        void __fastcall RunSilent(void* entity, Vec3* target) {
            if (Core::Config::SilentAim::EnableSilent && (GetAsyncKeyState(Core::Config::SilentAim::SilentKey) & 0x8000)) {
                Core::EntityNamespace::EntityData Entity;
                if (GetBestTarget(Entity, Core::Config::SilentAim::SilentFov, Core::Config::SilentAim::EnableSilentVisibleCheck)) {
                    *target = Entity.Position;
                }
            }
            Core::ImportedFuncs::ShootOriginal(entity, target);
        }
    }
}
