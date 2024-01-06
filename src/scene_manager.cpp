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
    const int HEIGHT = 20;
    const int WIDTH = 20;

    mazegen::Config cfg;
    cfg.ROOM_BASE_NUMBER = 1;
    cfg.ROOM_SIZE_MIN = 5;
    cfg.ROOM_SIZE_MAX = 7;
    cfg.EXTRA_CONNECTION_CHANCE = 0.0;
    cfg.WIGGLE_CHANCE = 0.5;
    cfg.DEADEND_CHANCE = 0.5;
    cfg.RECONNECT_DEADENDS_CHANCE = 0.5;
    cfg.CONSTRAIN_HALL_ONLY = false;

    constraints = { {1, 1}, {WIDTH - 2, HEIGHT - 2} };
    
    maze.generate(WIDTH, HEIGHT, cfg, constraints);
    
    for (int y = 0; y < maze.maze_height(); y++) {
        for (int x = 0; x < maze.maze_width(); x++) {
            int C = maze.region_at(x, y);
            int L = maze.region_at(x - 1, y);
            int R = maze.region_at(x + 1, y);
            int T = maze.region_at(x, y - 1);
            int B = maze.region_at(x, y + 1);
            if (C != mazegen::NOTHING_ID)
            {
                if (T == mazegen::NOTHING_ID)
                {
                    auto rect = Rectangle{ (float)x * TILE_SIZE, (float)y * TILE_SIZE,
                                        (float)(x + 1) * TILE_SIZE, (float)y * TILE_SIZE };
                    mazeBounds.push_back(rect);
                }
                if (L == mazegen::NOTHING_ID)
                {
                    auto rect = Rectangle{ (float)x * TILE_SIZE, (float)y * TILE_SIZE,
                        (float)x * TILE_SIZE, (float)(y + 1) * TILE_SIZE };
                    mazeBounds.push_back(rect);
                }
                if (R == mazegen::NOTHING_ID)
                {
                    auto rect = Rectangle{ (float)(x + 1) * TILE_SIZE, (float)y * TILE_SIZE,
                        (float)(x + 1) * TILE_SIZE, (float)(y + 1) * TILE_SIZE };
                    mazeBounds.push_back(rect);
                }
                if (B == mazegen::NOTHING_ID)
                {
                    mazeBounds.push_back(Rectangle{ (float)x * TILE_SIZE, (float)(y + 1) * TILE_SIZE,
                        (float)(x + 1) * TILE_SIZE, (float)(y + 1) * TILE_SIZE });
                }
                if (T == mazegen::NOTHING_ID && B == mazegen::NOTHING_ID)
                {
                    auto rect = Rectangle{ (float)x * TILE_SIZE, (float)y * TILE_SIZE,
                                        (float)(x + 1) * TILE_SIZE, (float)y * TILE_SIZE };
                    for (auto i = 0; i < 4; i++)
                    {
                        auto enemy = Enemy();
                        enemy.Init();
                        enemy.position = {
                            GetRandomValueF(rect.x + TILE_SIZE / 2.f, rect.width + TILE_SIZE / 2.f),
                            GetRandomValueF(rect.y + TILE_SIZE / 2.f, rect.height + TILE_SIZE / 2.f)
                        };
                        Pool::enemies.push_back(enemy);
                    }
                }
            }
        }
    }
}

Rectangle& SceneManager::ScreenInWorld()
{
    return screenInWorld;
}

void SceneManager::Load()
{
    player.Init();
    player.position = {
        mazeBounds[0].x + TILE_SIZE / 2.f,
        mazeBounds[0].y + TILE_SIZE / 2.f
    };
    Pool::createEnemies(200);
    Pool::createAsteroids();
}

void SceneManager::Update()
{
    seconds += GetFrameTime();
    if (frame > 120)
    {
        frame = 0;
    }
    frame++;
    SetShaderValue(starfield, secondsLoc, &seconds, SHADER_UNIFORM_FLOAT);
    Pool::UpdateMissle();
    Pool::UpdateEnemies();
    Pool::UpdateExplosions();
    player.Update();
    if (frame % 2 == 0)
    {
        Pool::UpdateAsteroids();
    }
}

void SceneManager::DrawMaze()
{
    for (int y = 0; y < maze.maze_height(); y++) {
        for (int x = 0; x < maze.maze_width(); x++) {
            int C = maze.region_at(x, y);
            int L = maze.region_at(x - 1, y);
            int R = maze.region_at(x + 1, y);
            int T = maze.region_at(x, y - 1);
            int B = maze.region_at(x, y + 1);
            if (C != mazegen::NOTHING_ID)
            {
                if (T == mazegen::NOTHING_ID)
                {
                    DrawLine(x * TILE_SIZE, y * TILE_SIZE,
                        (x + 1) * TILE_SIZE, y * TILE_SIZE, BLUE);
                }
                if (L == mazegen::NOTHING_ID)
                {
                    DrawLine(x * TILE_SIZE, y * TILE_SIZE,
                        x * TILE_SIZE, (y + 1) * TILE_SIZE, GREEN);
                }
                if (R == mazegen::NOTHING_ID)
                {
                    DrawLine((x + 1) * TILE_SIZE, y * TILE_SIZE,
                        (x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE, WHITE);
                }
                if (B == mazegen::NOTHING_ID)
                {
                    DrawLine(x * TILE_SIZE, (y + 1) * TILE_SIZE,
                        (x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE, RED);
                }
            }
        }
    }
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
        //DrawMaze();
        Pool::DrawMissle();
        Pool::DrawAsteroids();
        Pool::DrawEnemies();
        Pool::DrawExplosions();
        player.Draw();
    EndMode2D();
    DrawText(TextFormat("pos: %f %f %f", player.GetPosition().x, player.GetPosition().y, player.axisThrust), 10, 10, 20, WHITE);
}

void SceneManager::Reset()
{
    Pool::enemies.clear();
    Pool::misslePool.clear();
    Pool::explosions.clear();
    Pool::asteroids.clear();
}

bool SceneManager::CheckMazeBound(Node& node)
{
    if (!node.isAlive)
    {
        return false;
    }
    bool collision = false;
    auto point = node.position;
    Vector2 point1;
    Vector2 point2;
    for (int i = 0; i < mazeBounds.size() - 1; i++)
    {
        point1 = { mazeBounds[i].x, mazeBounds[i].y };
        point2 = { mazeBounds[i].width, mazeBounds[i].height };

        collision = CheckCollisionPointLine(point, point1, point2, 50);
        if (collision)
            break;
    }
    
    if (collision)
    {
        Explosion::Create(point, 1);
    }

    return collision;
}



Player& SceneManager::GetPlayer()
{
    return player;
}

const mazegen::Generator& SceneManager::GetMaze()
{
    return maze;
}

const std::vector<Rectangle>& SceneManager::GetMazeBounds()
{
    return mazeBounds;
}

const int SceneManager::GetTileSize()
{
    return TILE_SIZE;
}
