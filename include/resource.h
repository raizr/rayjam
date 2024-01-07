#pragma once
#include "raylib-aseprite.h"
#include "raylib.h"
#include <string>
#include <array>


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
        //using std::string_literals::operator""s;
        std::string dir = GetWorkingDirectory();
        ship = LoadTexture((dir + "/sprites/ship-player.png").c_str());
        shieldTexture = LoadTexture((dir + "/sprites/shield-player.png").c_str());
        missle = LoadTexture((dir + "/sprites/missle.png").c_str());
        thrust = LoadAseprite((dir + "/sprites/thrust.aseprite").c_str());
        animExplosionSprite = {
            LoadAseprite((dir + "/sprites/explosion.aseprite").c_str()),
            LoadAseprite((dir + "/sprites/explosion-a.aseprite").c_str())
        };
        asteroid = LoadTexture((dir + "/sprites/asteroid.png").c_str());
        for (int i = 0; i < 5; i++)
        {
            shipEnemy[i] = LoadTexture((dir + TextFormat("/sprites/ship-enemy%d.png", i + 1)).c_str());
        }
        for (int i = 0; i < 3; i++)
        {
            powerUpSprites[i] = LoadTexture((dir + TextFormat("/sprites/power-up%d.png", i + 1)).c_str());
        }
        UIShield = LoadTexture((dir + "/sprites/shield-ui.png").c_str());
        UILife = LoadTexture((dir + "/sprites/life-ui.png").c_str());
    }
    inline static Texture ship;
    inline static std::array<Texture,5> shipEnemy;
    inline static Aseprite thrust;
    inline static Texture missle;
    inline static Texture shieldTexture;
    inline static std::array<Aseprite, 2> animExplosionSprite;
    inline static std::array<Texture, 3> powerUpSprites;
    inline static Texture asteroid;
    inline static Texture UIShield;
    inline static Texture UILife;
};