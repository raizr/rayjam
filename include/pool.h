#pragma once
#include <vector>
#include "missle.h"

struct Pool
{
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

    static void UpdateMissle()
    {
        for (auto& missle : misslePool)
        {
			missle.Update();
        }
    }

    static void DrawMissle()
	{
		for (auto& missle : misslePool)
		{
			missle.Draw();
		}
	}

    inline static std::vector<Missle> misslePool;
};
