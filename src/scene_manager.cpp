#include "scene_manager.h"

#include <string>

#include "core.h"
#include "mazegen.h"
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

SceneManager::SceneManager()
{
    std::string dir = GetWorkingDirectory();
    background = LoadTexture((dir + "/sprites/background.png"s).c_str());
    starfield = LoadShader(nullptr, TextFormat((dir + "/shaders/starfield%i.fs"s).c_str(), GLSL_VERSION));
    secondsLoc = GetShaderLocation(starfield, "seconds");
}

Rectangle& SceneManager::ScreenInWorld()
{
    return screenInWorld;
}

bool SceneManager::IsLevelClear()
{
    return levelClear;
}

void SceneManager::NextLevel()
{
    Reset();
    Load();
}

void SceneManager::Load()
{
    player.Init();
    Pool::createAsteroids();
    Pool::createEnemies();
}

void SceneManager::Update()
{
    seconds += GetFrameTime();
    SetShaderValue(starfield, secondsLoc, &seconds, SHADER_UNIFORM_FLOAT);
    Pool::UpdateMissle();
    Pool::UpdateEnemies();
    Pool::UpdateExplosions();
    player.Update();
    Pool::UpdateAsteroids();
    activeEnemiesCount = 0;
    for (auto& enemy : Pool::enemies)
    {
        if (enemy.isAlive)
        {
            activeEnemiesCount++;
        }
    }
    levelClear = activeEnemiesCount == 0;
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
        screenInWorld = Rectangle{ screenOriginInWorld.x, screenOriginInWorld.y, screenEdgeInWorld.x - screenOriginInWorld.x,screenEdgeInWorld.y - screenOriginInWorld.y };

        float bgScale = 0.05f;
        Rectangle sourceRect = Rectangle{ screenInWorld.x * bgScale, screenInWorld.y * bgScale, screenInWorld.width * bgScale, screenInWorld.height * bgScale };

        BeginShaderMode(starfield);
            DrawTexturePro(background, sourceRect, screenInWorld, Vector2Zero(), 0, WHITE);
        EndShaderMode();
        Pool::DrawMissle();
        Pool::DrawAsteroids();
        Pool::DrawEnemies();
        Pool::DrawExplosions();
        player.Draw();
    EndMode2D();
}

void SceneManager::Reset()
{
    Pool::enemies.clear();
    Pool::misslePool.clear();
    Pool::explosions.clear();
    Pool::asteroids.clear();
}

Player& SceneManager::GetPlayer()
{
    return player;
}

const int SceneManager::GetTileSize()
{
    return TILE_SIZE;
}
