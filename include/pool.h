#pragma once
#include <vector>

#include "asteroid.h"
#include "enemy.h"
#include "explosion.h"
#include "missle.h"
#include "powerup.h"
#include "raymath.h"
#include "resource.h"
#include "scene_manager.h"
#include <core.h>

struct Pool
{
    static void createEnemies()
    {
        int size = 5 * core::Core::getInstance()->GetCurrentLevel();
        double j = 0;
        for (int i = 0; i < size; i++)
        {
            Enemy enemy;
            enemy.Init();

            enemy.position = {
                sinf(j) * GetRandomValue(500, 10000),
                cosf(j) * GetRandomValue(500, 10000)
            };
            enemies.push_back(enemy);
            j = j + (PI / 16);
        }
    }

    static void createAsteroids()
    {
        double j = 0;
        for (int i = 0; i < 500; i++)
        {
            Asteroid asteroid;
            asteroid.position = {
                sinf(j) * GetRandomValue(500, 10000),
                cosf(j) * GetRandomValue(500, 10000)
            };
            asteroid.scale = GetRandomValueF(0.5f, 1.5f);
            asteroid.radius *= asteroid.scale;
            asteroid.rotationalVelocity = GetRandomValueF(0.4f, 0.8f);
            asteroid.velocity = {
                GetRandomValueF(10.0f, 50.f),
                GetRandomValueF(10.0f, 50.f)
            };
            asteroids.push_back(asteroid);
            j = j + (PI / 16);
        }
    }

    static PowerUp* GetPowerUp()
    {
        PowerUp* slot = nullptr;
        for (auto& powerup : powerups)
        {
            if (!powerup.isAlive)
            {
                slot = &powerup;
                break;
            }
        }

        if (!slot)
        {
            powerups.emplace_back(PowerUp());
            slot = &powerups.back();
        }
        return slot;
    }

    static Missle* getMissle()
    {
        Missle* slot = nullptr;
        for (auto& missle : misslePool)
        {
            if (!missle.isAlive)
            {
                slot = &missle;
                break;
            }
        }

        if (!slot)
        {
            misslePool.emplace_back(Missle());
            slot = &misslePool.back();
        }
        return slot;
    }

    static Explosion* getExplosion()
    {
        Explosion* slot = nullptr;
        for (auto& explosion : explosions)
        {
            if (!explosion.isAlive)
            {
                slot = &explosion;
                break;
            }
        }

        if (!slot)
        {
            explosions.emplace_back(Explosion());
            slot = &explosions.back();
        }
        return slot;
    }

    static void UpdateMissle()
    {
        for (auto& missle : misslePool)
        {
			missle.Update();
            if (!missle.isAlive)
            {
                continue;
            }

            if (scene::SceneManager::getInstance()->GetPlayer().Collide(missle))
            {
                missle.isAlive = false;
                return;
            }

            for (auto& enemy : enemies)
            {
                if (!enemy.isAlive)
                {
                    continue;
                }
                if (enemy.Collide(missle))
                {
                    missle.isAlive = false;
                    break;
                }
            }

            for (auto& asteroid : asteroids)
            {
                if (!asteroid.isAlive)
                {
                    continue;
                }
                if (asteroid.Collide(missle))
                {
                    missle.isAlive = false;
                    break;
                }
            }
        }
    }

    static void UpdateEnemies()
    {
        for (auto& enemy : enemies)
        {
            enemy.Update();
        }
    }

    static void UpdateExplosions()
    {
        for (auto& explosion : explosions)
        {
            explosion.Update();
        }
    }

    static void UpdateAsteroids()
    {
        for (auto& asteroid : asteroids)
        {
            asteroid.Update();
        }
    }

    static void UpdatePowerup()
    {
        for (auto& powerup : powerups)
        {
            powerup.Update();
            auto& player = scene::SceneManager::getInstance()->GetPlayer();
            powerup.Collide(player);
        }
    }

    static void DrawMissle()
	{
		for (auto& missle : misslePool)
		{
			missle.Draw();
		}
	}

    static void DrawEnemies()
    {
        for (auto& enemy : enemies)
        {
            enemy.Draw();
        }
    }

    static void DrawExplosions()
    {
        for (auto& explosion : explosions)
        {
            explosion.Draw();
        }
    }

    static void DrawAsteroids()
    {
        auto& screenWorld = scene::SceneManager::getInstance()->ScreenInWorld();
        auto& player = scene::SceneManager::getInstance()->GetPlayer();
        for (auto& asteroid : asteroids)
        {
            float viewRadSq = powf(screenWorld.width * 0.65f, 2) + powf(screenWorld.height * 0.65f, 2);
            if (Vector2DistanceSqr(player.position, asteroid.position) < viewRadSq + (powf(asteroid.radius, 2)))
            {
                asteroid.Draw();
            }
        }
    }

    static void DrawPowerUp()
    {
        for (auto& powerup : powerups)
        {
            powerup.Draw();
        }
    }

    inline static std::vector<Missle> misslePool;
    inline static std::vector<Enemy> enemies;
    inline static std::vector<Explosion> explosions;
    inline static std::vector<Asteroid> asteroids;
    inline static std::vector<PowerUp> powerups;
};
