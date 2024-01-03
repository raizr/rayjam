#pragma once
#include "enemy.h"
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
        Player& GetPlayer();

    private:
        SceneManager() = default;
        ~SceneManager() = default;

        inline static SceneManager* instance = nullptr;
        Texture background = { };
        Camera2D worldCamera = { };
        Player player;
        Shader starfield = {};
        float seconds = {};
        int secondsLoc = {};
    };

}