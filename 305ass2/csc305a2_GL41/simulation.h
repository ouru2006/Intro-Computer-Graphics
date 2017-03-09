#pragma once

#include <cstddef>

#include <SDL.h>

struct SDL_Window;
union SDL_Event;
class Scene;

class Simulation
{
    Scene* mScene;

    int mDeltaMouseX;
    int mDeltaMouseY;

    uint32_t mSpinningTransformID;

public:
    void Init(Scene* scene);
    void HandleEvent(const SDL_Event& ev);
    void Update(float deltaTime);

    void* operator new(size_t sz);
};
