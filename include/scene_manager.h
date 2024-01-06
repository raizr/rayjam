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
        void DrawMaze();
        void Draw();
        void Reset();
        bool CheckMazeBound(Node& node);
        Player& GetPlayer();
        const mazegen::Generator& GetMaze();
        const std::vector<Rectangle>& GetMazeBounds();
        const int GetTileSize();
        Rectangle& ScreenInWorld();
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
        int frame = 0;
        int secondsLoc = {};
        mazegen::Generator maze;
        mazegen::PointSet constraints;
        std::vector<Rectangle> mazeBounds;
        int TILE_SIZE = 2000;
    };

}