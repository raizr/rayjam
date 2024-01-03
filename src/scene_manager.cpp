#include "scene_manager.h"

#include <string>

#include "core.h"
#include "pool.h"
#include "raymath.h"

using namespace scene;
using namespace std::string_literals;

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

SceneManager* SceneManager::getInstance()
{
    if (!instance)
    {
        instance = new SceneManager();
    }
    return instance;
}

void SceneManager::cleanup()
{
    delete instance;
}

void SceneManager::Load()
{
    std::string dir = GetWorkingDirectory();
    printf("dir %s", dir.c_str());
    background = LoadTexture((dir +"/sprites/background.png"s).c_str());
    player.Init();
    starfield = LoadShader(nullptr, TextFormat((dir + "/shaders/starfield%i.fs"s).c_str() , GLSL_VERSION));
    secondsLoc = GetShaderLocation(starfield, "seconds");
    Pool::createEnemies(10000);
}

void SceneManager::Update()
{
    seconds += GetFrameTime();

    SetShaderValue(starfield, secondsLoc, &seconds, SHADER_UNIFORM_FLOAT);
    Pool::UpdateMissle();
    Pool::UpdateEnemies();
    Pool::UpdateExplosions();
    player.Update();
}

void SceneManager::Draw()
{
    Vector2 center = Vector2Scale({ (float)core::gameScreenWidth , (float)core::gameScreenHeight }, 0.5f);
    worldCamera.offset = center;
    worldCamera.target = player.GetPosition();
    worldCamera.zoom = 0.5f;
    float screenSize[2] = { (float)core::gameScreenWidth, (float)core::gameScreenHeight };

    SetShaderValue(starfield, GetShaderLocation(starfield, "position"), &player.GetPosition(), SHADER_UNIFORM_VEC2);
    SetShaderValue(starfield, GetShaderLocation(starfield, "size"), &screenSize, SHADER_UNIFORM_VEC2);

    BeginMode2D(worldCamera);
        Rectangle screen = { 0, 0,  center.x * 2,  center.y * 2 };
        Vector2 screenOriginInWorld = GetScreenToWorld2D(Vector2Zero(), worldCamera);
        Vector2 screenEdgeInWorld = GetScreenToWorld2D(Vector2{ screen.width, screen.height }, worldCamera);
        Rectangle screenInWorld = Rectangle{ screenOriginInWorld.x, screenOriginInWorld.y, screenEdgeInWorld.x - screenOriginInWorld.x,screenEdgeInWorld.y - screenOriginInWorld.y };

        float bgScale = 0.05f;
        Rectangle sourceRect = Rectangle{ screenInWorld.x * bgScale, screenInWorld.y * bgScale, screenInWorld.width * bgScale, screenInWorld.height * bgScale };

        BeginShaderMode(starfield);
            DrawTexturePro(background, sourceRect, screenInWorld, Vector2Zero(), 0, WHITE);
        EndShaderMode();
        Pool::DrawMissle();
        Pool::DrawEnemies();
        Pool::DrawExplosions();
        player.Draw();
    EndMode2D();
    DrawText(TextFormat("pos: %f %f %f", player.GetPosition().x, player.GetPosition().y, player.axisThrust), 10, 10, 20, WHITE);
    DrawText(TextFormat("vector: %f %f", player.shipVector.x, player.shipVector.y), 10, 20, 0, WHITE);
    int i = 15;
    for (auto& enemy : Pool::enemies)
    {
        if (enemy.isFound)
        {
            i += 20;
            DrawText(TextFormat("radius: %f", enemy.orientation), 10, i, 20, GREEN);
        }
    }
}

Player& SceneManager::GetPlayer()
{
    return player;
}
