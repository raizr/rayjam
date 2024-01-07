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

inline size_t AddEffect(const char* name, float volume = 1.0f);

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
        UIControl = LoadTexture((dir + "/sprites/stick.png").c_str());
        UIPause = LoadTexture((dir + "/sprites/pause.png").c_str());
        UITutorial = LoadTexture((dir + "/sprites/tutorial.png").c_str());
        InitAudioDevice();
        for (int i = 0; i < 3; i++)
        {
            explosion[i] = AddEffect((dir + TextFormat("/sound/explosion_00%d.wav", i + 1)).c_str(), 0.1f);
        }
        for (int i = 0; i < 3; i++)
        {
            shoot[i] = AddEffect((dir + TextFormat("/sound/shoot%d.wav", i + 1)).c_str(), 0.1f);
        }
        for (int i = 0; i < 3; i++)
        {
            powerups[i] = AddEffect((dir + TextFormat("/sound/powerup%d.wav", i + 1)).c_str(), 0.1f);
        }
        music = LoadMusicStream((dir + std::string("/sound/track.ogg")).c_str());
        music.looping = true;
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
    inline static Texture UIControl;
    inline static Texture UIPause;
    inline static Texture UITutorial;
    inline static std::vector<Sound> effects;
    inline static std::array<size_t, 3> explosion;
    inline static std::array<size_t, 3> shoot;
    inline static std::array<size_t, 3> powerups;
    inline static Music music;
};

inline size_t AddEffect(const char* name, float volume)
{
    auto sound = LoadSound(name);
    SetSoundVolume(sound, volume);
    Resources::effects.emplace_back(sound);
    return Resources::effects.size() - 1;
}

inline void PlaySoundEffect(size_t effect)
{
    if (effect <= Resources::effects.size())
        PlaySound(Resources::effects[effect]);
}