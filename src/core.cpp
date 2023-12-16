#include "core.h"

#include <string>
#include <utils.h>

#include "raygui.h"
#include "raymath.h"
#include "resource.h"
#include "scene_manager.h"

using namespace core;

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

Core* Core::getInstance()
{
    if (!instance)
    {
        instance = new Core();
    }
    return instance;
}

void Core::cleanup()
{
    delete instance;
}

void Core::Init()
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Space Asterogue");
    SearchAndSetResourceDir("resources");
    std::string dir = GetWorkingDirectory();
    GuiLoadStyle((dir + "/style_cyber.rgs").c_str());
    HideCursor();
    SetExitKey(KEY_NULL);
    SetTargetFPS(FIXED_FRAME_RATE);
    scene::SceneManager::getInstance()->Load();
    target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
}


void Core::CenterWindow()
{
    int monitor = GetCurrentMonitor();

    int x = GetMonitorWidth(monitor) / 2 - screenWidth / 2;
    int y = GetMonitorHeight(monitor) / 2 - screenHeight / 2;

    SetWindowPosition(x, y);
}

Core::~Core()
{
    UnloadRenderTexture(target);
}

double Core::GetCurrentTime() const
{
    return time;
}

// to support pause, and make debugging easier, we track our own delta time
float Core::GetDeltaTime() const
{
    if (gameState == GameState::Paused)
    {
        return 0;
    }

#ifdef _DEBUG
    return 1.0f / FIXED_FRAME_RATE;		// if we are debugging, use a fixed frame rate so that we doin't get lag spikes when hitting breakpoints
#else
    return GetFrameTime();
#endif
}

void DrawCenteredText(const char* text, float textSize = 20, float yOffset = 0.5f, float xOffset = 0.5f)
{
    Vector2 size = MeasureTextEx(GetFontDefault(), text, textSize, textSize / 10);

    Vector2 pos = { GetScreenWidth() * xOffset - size.x / 2.0f, GetScreenHeight() * yOffset - size.y / 2.0f };
    DrawText(text, int(pos.x), int(pos.y), int(textSize), WHITE);
}

void Core::Update()
{
    if (gameState != GameState::Paused)
    {
        time += GetDeltaTime();
    }

    if (gameState == GameState::Paused)
    {
        if (AcceptPressed())
        {
            gameState = GameState::Playing;
        }
    }

    if (gameState == GameState::Playing)
    {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_TAB))
        {
            gameState = GameState::Paused;
        }
    }
    if (gameState == GameState::Playing)
    {
        scene::SceneManager::getInstance()->Update();
    }
    float scale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);
    BeginTextureMode(target);
        ClearBackground(BLACK);
        scene::SceneManager::getInstance()->Draw();
    EndTextureMode();
    BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(target.texture,
            Rectangle { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
            Rectangle{ (GetScreenWidth() - ((float)gameScreenWidth * scale)) * 0.5f,
                    (GetScreenHeight() - ((float)gameScreenHeight * scale)) * 0.5f,
                    (float)gameScreenWidth* scale, (float)gameScreenHeight* scale
        }, Vector2 { 0, 0 }, 0.0f, WHITE);
    if (gameState == GameState::Paused)
    {
        DrawOverlay();
    }
    if (IsCursorOnScreen())
    {
        GuiDrawIcon(50, GetMouseX(), GetMouseY(), 1, WHITE);
    }
    EndDrawing();
}

void Core::DrawOverlay()
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{ 0, 0, 0, 128 });
    DrawCenteredText("Space Asterogue", 60);
}

bool Core::AcceptPressed()
{
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return true;

    if (IsGamepadAvailable(0))
    {
        return IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    }

    return false;
}