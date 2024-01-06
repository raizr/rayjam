#pragma once
#include <vector>

#include "asteroid.h"
#include "enemy.h"
#include "explosion.h"
#include "missle.h"
#include "raymath.h"
#include "resource.h"
#include "scene_manager.h"

struct Pool
{
    static void createEnemies(int enemiesSize)
    {
        auto& bounds = scene::SceneManager::getInstance()->GetMazeBounds();
        auto tileSize = scene::SceneManager::getInstance()->GetTileSize();
        /*for (auto it = bounds.begin(); it != bounds.end(); ++it)
        {
            auto& bound = *it;
            for (auto i = 0; i < 4; i++)
            {
                auto enemy = Enemy();
                enemy.Init();
                enemy.position = {
                    GetRandomValueF(bound.x + tileSize/2.f, bound.width + tileSize / 2.f),
                    GetRandomValueF(bound.y + tileSize / 2.f, bound.height + tileSize / 2.f)
                };
                enemies.emplace_back(std::move(enemy));
            }
        }*/
    }

    static void createAsteroids()
    {
        for (auto& bound : scene::SceneManager::getInstance()->GetMazeBounds())
        {
            for (int i = 0; i < 100; i++)
            {
                Asteroid asteroid;
                asteroid.position = {
                    GetRandomValueF(bound.x, bound.width),
                    GetRandomValueF(bound.y, bound.height)
                };
                asteroid.scale = GetRandomValueF(0.5f, 1.5f);
                asteroid.radius *= asteroid.scale;
                if (bound.x != bound.width)
                {
                    asteroid.velocity.x = GetRandomValueF(-20.0f, 20.f);
                    asteroid.boundary = { bound.x, bound.width };
                }
                if (bound.y != bound.height)
                {
                    asteroid.velocity.y = GetRandomValueF(-20.0f, 20.f);
                    asteroid.boundary = { bound.y, bound.height };
                }
                asteroids.push_back(asteroid);
            }
        }
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
            scene::SceneManager::getInstance()->CheckMazeBound(enemy);
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

    inline static std::vector<Missle> misslePool;
    inline static std::vector<Enemy> enemies;
    inline static std::vector<Explosion> explosions;
    inline static std::vector<Asteroid> asteroids;
};
