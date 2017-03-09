#pragma once

#include "shaderset.h"

struct SDL_Window;
class Scene;

class Renderer
{
    Scene* mScene;

    // ShaderSet explanation:
    // https://nlguillemot.wordpress.com/2016/07/28/glsl-shader-live-reloading/
    ShaderSet mShaders;

    GLuint* mSceneSP;

    GLuint* mShadowSP;
    GLuint* mSkyboxSP;
    GLuint skyboxVAO, skyboxVBO;

    GLuint cubemapTexture;

    int mBackbufferWidth;
    int mBackbufferHeight;
    GLuint mBackbufferFBO;
    GLuint mBackbufferColorTO;
    GLuint mBackbufferDepthTO;

    GLuint mShadowFBO;
    GLuint mShadowDepthTO;
    int kShadowMapResolution;


    float mShadowSlopeScaleBias;
    float mShadowDepthBias;

    GLuint* mDepthVisSP;
    GLuint mNullVAO;
    bool mShowDepthVis;

public:
    void Init(Scene* scene);
    void Resize(int width, int height);
    void Render();

    void* operator new(size_t sz);
};
