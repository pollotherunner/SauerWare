#pragma once
#include <Includes.hpp>
#include <Entity.hpp>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <Dependencies/ImGui/imgui.h>


namespace Features {
    namespace ESP {
        void Run() {

            std::scoped_lock lock(Core::EntityNamespace::gMutex);

            if (Core::EntityNamespace::EntityMap.empty()) return;

            auto DrawList = ImGui::GetBackgroundDrawList();
            if (!DrawList) return;

            ImVec2 ScreenCenter = { ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2 };

            int ScreenWidght = Core::Funcs::Read<int>((uintptr_t)Core::ProcessInformations::GameModule + Core::Offsets::Game::ScreenW);
            int ScreenHeigh = Core::Funcs::Read<int>((uintptr_t)Core::ProcessInformations::GameModule + Core::Offsets::Game::ScreenH);            for (const auto& Entity : Core::EntityNamespace::EntityMap)
            {
                if (Entity.second.Flag == 1 && Core::Config::ESP::IgnoreDead) continue;               
                if ((Core::LocalPlayer::Team == Entity.second.Team) && Core::Config::ESP::IgnoreTeam) continue;                

                if (Core::Config::ESP::EnableESP) {
                    
                    const char* NameCCP = Entity.second.Name.c_str();
                    Vec3 EnemyPos = Entity.second.Position;
                    Vec3 LocalPlayerPos = Core::LocalPlayer::Pos;
                    Vec3 Delta = EnemyPos - LocalPlayerPos;
                    float Distance = sqrtf(Delta.x * Delta.x + Delta.y * Delta.y + Delta.z * Delta.z);

                    Vec2 EnemyW2S;
                    bool EnemyIsVisibleOnCamera = Core::Funcs::WorldToScreen(EnemyPos, EnemyW2S);
                    if (EnemyIsVisibleOnCamera) {
                        
                        float screenFactor = (ScreenWidght + ScreenHeigh) / 10.0f;

                        float Height = 30.0f / (Distance / screenFactor);
                        float Width = 20.0f / (Distance / screenFactor);

                        ImVec2 BoxMin = { EnemyW2S.x - Width / 2.0f, EnemyW2S.y - 2 };
                        ImVec2 BoxMax = { EnemyW2S.x + Width / 2.0f, EnemyW2S.y + Height };



                        float CenterBox = ((BoxMax.x - BoxMin.x) / 2) + BoxMin.x;

                        if (Core::Config::ESP::EnableBox) {
                            DrawList->AddRect(BoxMin, BoxMax, Entity.second.IsVisible ? Core::Config::ESP::ColorBoxVisible : Core::Config::ESP::ColorBoxInvisible);
                        }
                        if (Core::Config::ESP::EnableLine) {
                            DrawList->AddLine({ ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y }, { CenterBox,BoxMax.y }, Entity.second.IsVisible ? Core::Config::ESP::ColorLineVisible : Core::Config::ESP::ColorLineInvisible);
                        }
                        if (Core::Config::ESP::EnableHealthBar) {

                            ImVec2 MinHealthBar = { BoxMin.x - 3, BoxMin.y };
                            ImVec2 MaxHealthBar = { BoxMin.x - 1,BoxMax.y };

                            float Height = MaxHealthBar.y - MinHealthBar.y;
                            float HealthPercentage = Entity.second.Health / Entity.second.MaxHealth;
                            float HealthHeigh = Height * HealthPercentage;

                            DrawList->AddRectFilled({ MinHealthBar.x - 1, MinHealthBar.y -1 }, { MaxHealthBar.x + 1, MaxHealthBar.y + 1 }, ImColor(50, 50, 50));
                            DrawList->AddRectFilled({ MinHealthBar.x, MinHealthBar.y}, MaxHealthBar,ImColor(20,20,20));
                            DrawList->AddRectFilled({ MinHealthBar.x, MaxHealthBar.y - HealthHeigh }, MaxHealthBar, Core::Config::ESP::ColorHealthBar);
                        }
                        if (Core::Config::ESP::EnableDistance) {
                            int DistanceInt = static_cast<int>(Distance);
                            std::string DistanceStr = std::to_string(DistanceInt);
                            const char* DistanceCChar = DistanceStr.c_str();
                            float DistAdjustY = Core::Config::ESP::EnableName ? ImGui::CalcTextSize(NameCCP).y - 2 : -2;
                            DrawList->AddText({ CenterBox - ImGui::CalcTextSize(DistanceCChar).x / 2, (BoxMin.y - ImGui::CalcTextSize(DistanceCChar).y) - DistAdjustY }, Core::Config::ESP::ColorText, DistanceCChar);

                        }
                        if (Core::Config::ESP::EnableName) {

                            DrawList->AddText({ CenterBox - ImGui::CalcTextSize(NameCCP).x / 2, (BoxMin.y - ImGui::CalcTextSize(NameCCP).y) - 2 }, Core::Config::ESP::ColorText, NameCCP);
                        }
                    }
                }
            }
        }
    }
}
