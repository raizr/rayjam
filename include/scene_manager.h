#pragma once
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


    private:
        SceneManager() = default;
        ~SceneManager() = default;

        inline static SceneManager* instance = nullptr;
        Texture background = { };
        Image backgroundAnim = { };
        int animFrames = 0;
        unsigned int nextFrameDataOffset = 0;
        int currentAnimFrame = 0;
        int frameDelay = 16;
        int frameCounter = 0;
        Camera2D worldCamera = { };
        Player player;
    };

}