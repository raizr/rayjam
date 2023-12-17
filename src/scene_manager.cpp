#include "scene_manager.h"

#include <string>

#include "core.h"
#include "raymath.h"

using namespace scene;

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
    backgroundAnim = LoadImageAnim((dir + std::string("/sprites/background.gif")).c_str(), &animFrames);
    background = LoadTextureFromImage(backgroundAnim);
    player.Init();
}

void SceneManager::Update()
{
    player.Update();
}

void SceneManager::Draw()
{
    Vector2 center = Vector2Scale({ (float)core::gameScreenWidth , (float)core::gameScreenHeight }, 0.5f);
    worldCamera.offset = center;
    worldCamera.target = player.GetPosition();
    worldCamera.zoom = 0.3f;
    BeginMode2D(worldCamera);
    // compute the size of the background and shift it based on our movement
    Rectangle screen = { 0,0,  center.x * 2,  center.y * 2 };
    Vector2 screenOriginInWorld = GetScreenToWorld2D(Vector2Zero(), worldCamera);
    Vector2 screenEdgeInWorld = GetScreenToWorld2D(Vector2{ screen.width, screen.height }, worldCamera);
    Rectangle screenInWorld = Rectangle{ screenOriginInWorld.x, screenOriginInWorld.y, screenEdgeInWorld.x - screenOriginInWorld.x,screenEdgeInWorld.y - screenOriginInWorld.y };

    float bgScale = 0.5f;
    Rectangle sourceRect = Rectangle{ screenInWorld.x * bgScale, screenInWorld.y * bgScale, screenInWorld.width * bgScale, screenInWorld.height * bgScale };
    frameCounter++;
    if (frameCounter >= frameDelay)
    {
        // Move to next frame
        // NOTE: If final frame is reached we return to first frame
        currentAnimFrame++;
        if (currentAnimFrame >= animFrames) currentAnimFrame = 0;

        // Get memory offset position for next frame data in image.data
        nextFrameDataOffset = backgroundAnim.width * backgroundAnim.height * 4 * currentAnimFrame;

        // Update GPU texture data with next frame image data
        // WARNING: Data size (frame size) and pixel format must match already created texture
        UpdateTexture(background, (unsigned char*)backgroundAnim.data + nextFrameDataOffset);
        frameCounter = 0;
    }
    DrawTexturePro(background, sourceRect, screenInWorld, Vector2Zero(), 0, WHITE);
    // draw the world and pass in the viewport window in world space for culling
    //world.Draw(screenInWorld);
    player.Draw();
    EndMode2D();
    DrawText(TextFormat("orientation: %f", player.orientation), 10, 80, 20, RED);
    DrawText(TextFormat("velocity %f", player.speed), 10, 100, 20, GREEN);
}
