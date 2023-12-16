#pragma once
#include "raylib.h"

namespace core
{
    enum class GameScreen
    {
        SCREEN_LOGO = 0,
        SCREEN_TITLE,
        SCREEN_GAMEPLAY
    };

    enum class GameState
    {
        None,
        Playing,
        Paused,
    };

    static constexpr int screenWidth = 1280;
    static constexpr int screenHeight = 720;
    static constexpr int scaleGameScreen = 4;
    static constexpr int gameScreenWidth = 240 * scaleGameScreen;
    static constexpr int gameScreenHeight = 135 * scaleGameScreen;
    constexpr int FIXED_FRAME_RATE = 60;

    class Core
    {
    public:
        static Core* getInstance();
        static void cleanup();
        void Init();
        double GetCurrentTime() const;
        float GetDeltaTime() const;

        void Update();
        void DrawOverlay();
        bool AcceptPressed();

        bool IsPaused() { return gameState == GameState::Paused; }

        static void CenterWindow();

    private:
        Core() = default;
        ~Core();

        inline static Core* instance = nullptr;
        GameState gameState = GameState::Paused;
        RenderTexture2D target;
        double time = 0;

    };
    
}