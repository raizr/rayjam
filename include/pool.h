#pragma once
#include <vector>

#include "enemy.h"
#include "explosion.h"
#include "missle.h"
#include "scene_manager.h"

struct Pool
{
    static void createEnemies(int enemiesSize)
    {
        for (auto i = 0; i < enemiesSize; i++)
        {
            auto enemy = Enemy();
            enemy.Init();
            enemies.emplace_back(std::move(enemy));
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

            scene::SceneManager::getInstance()->GetPlayer().Collide(missle);

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

    inline static std::vector<Missle> misslePool;
    inline static std::vector<Enemy> enemies;
    inline static std::vector<Explosion> explosions;
};
