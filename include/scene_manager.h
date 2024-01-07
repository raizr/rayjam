#pragma once
#include "enemy.h"
#include "mazegen.h"
#include "raylib.h"
#include "player.h"


namespace scene
{
    class SceneManager
    {
    public:
        static SceneManager* getInstance();
        static void cleanup();
        void Load();
        void Update();
        void Draw();
        void Reset();
        Player& GetPlayer();
        const int GetTileSize();
        Rectangle& ScreenInWorld();
        bool IsLevelClear();
        void NextLevel();

    private:
        SceneManager();
        ~SceneManager() = default;

        inline static SceneManager* instance = nullptr;
        Rectangle screenInWorld;
        Texture background = { };
        Camera2D worldCamera = { };
        Player player;
        Shader starfield = { };
        float seconds = {};
        int secondsLoc = {};
        mazegen::PointSet constraints;
        int TILE_SIZE = 2000;
        int activeEnemiesCount = 0;
        bool levelClear = false;
    };

}