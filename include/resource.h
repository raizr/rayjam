#pragma once
#include "raylib-aseprite.h"
#include "raylib.h"
#include <array>

using namespace std::string_literals;

inline bool SearchAndSetResourceDir(const char* folderName)
{
    // check the working dir
    if (DirectoryExists(folderName))
    {
        ChangeDirectory(TextFormat("%s/%s", GetWorkingDirectory(), folderName));
        return true;
    }

    const char* appDir = GetApplicationDirectory();

    // check the applicationDir
    const char* dir = TextFormat("%s%s", appDir, folderName);
    if (DirectoryExists(dir))
    {
        ChangeDirectory(dir);
        return true;
    }

    // check one up from the app dir
    dir = TextFormat("%s../%s", appDir, folderName);
    if (DirectoryExists(dir))
    {
        ChangeDirectory(dir);
        return true;
    }

    // check two up from the app dir
    dir = TextFormat("%s../../%s", appDir, folderName);
    if (DirectoryExists(dir))
    {
        ChangeDirectory(dir);
        return true;
    }

    // check three up from the app dir
    dir = TextFormat("%s../../../%s", appDir, folderName);
    if (DirectoryExists(dir))
    {
        ChangeDirectory(dir);
        return true;
    }

    return false;
}


struct Resources
{
    static void LoadTextures()
    {
        std::string dir = GetWorkingDirectory();
        ship = LoadTexture((dir + "/sprites/ship2.png"s).c_str());
        thrust = LoadAseprite((dir + "/sprites/thrust.aseprite"s).c_str());
        animExplosionSprite = {
            LoadAseprite((dir + "/sprites/explosion.aseprite"s).c_str()),
            LoadAseprite((dir + "/sprites/explosion-a.aseprite"s).c_str())
        };
    }
    inline static Texture ship;
    inline static Aseprite thrust;
    inline static std::array<Aseprite, 2> animExplosionSprite;
};