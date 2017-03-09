#include "simulation.h"

#include "scene.h"

#include "imgui.h"

#define FLYTHROUGH_CAMERA_IMPLEMENTATION
#include "flythrough_camera.h"

#include <glm/gtc/type_ptr.hpp>

#include <SDL.h>

void Simulation::Init(Scene* scene)
{
    mScene = scene;

    std::vector<uint32_t> loadedMeshIDs;
/*
    loadedMeshIDs.clear();
    LoadMeshesFromFile(mScene, "assets/sponza/sponza.obj", &loadedMeshIDs);
    for (uint32_t loadedMeshID : loadedMeshIDs)
    {
       if (scene->Meshes[loadedMeshID].Name == "sponza_04")
        {continue; }
        uint32_t newInstanceID;
        AddMeshInstance(mScene, loadedMeshID, &newInstanceID);
        uint32_t newTransformID = scene->Instances[newInstanceID].TransformID;
        scene->Transforms[newTransformID].Scale = glm::vec3(1.0f / 50.0f);
        scene->Transforms[newTransformID].Translation += glm::vec3(0.0f, -10.0f, 0.0f);
    }
*/
    loadedMeshIDs.clear();
    LoadMeshesFromFile(mScene, "assets/cube/cube.obj", &loadedMeshIDs);
    for (uint32_t loadedMeshID : loadedMeshIDs)
    {

        uint32_t newInstanceID;
        AddMeshInstance(mScene, loadedMeshID, &newInstanceID);

        // scale up the cube
        uint32_t newTransformID = scene->Instances[newInstanceID].TransformID;
        scene->Transforms[newTransformID].Scale = glm::vec3(2.0f);


        scene->Transforms[newTransformID].Translation += glm::vec3(0.0f, -10.0f, 0.0f);
    }
    /*
    loadedMeshIDs.clear();
    LoadMeshesFromFile(mScene, "assets/skybox/cube.obj", &loadedMeshIDs);
    for (uint32_t loadedMeshID : loadedMeshIDs)
    {


        uint32_t newInstanceID;
        AddMeshInstance(mScene, loadedMeshID, &newInstanceID);

        // scale up the cube
        uint32_t newTransformID = scene->Instances[newInstanceID].TransformID;
        scene->Transforms[newTransformID].Scale = glm::vec3(70.0f);
        scene->Transforms[newTransformID].Translation=glm::vec3(0.0f, 34.5f, 0.0f);


    }
*/
    loadedMeshIDs.clear();
    LoadMeshesFromFile(mScene, "assets/teapot/teapot.obj", &loadedMeshIDs);

    for (uint32_t loadedMeshID : loadedMeshIDs)
    {


            uint32_t parentInstanceID;
            AddMeshInstance(mScene, loadedMeshID, &parentInstanceID);
            scene->Transforms[scene->Instances[parentInstanceID].TransformID].ParentID = -1;
            uint32_t newTransformID = scene->Instances[parentInstanceID].TransformID;
            //scene->Transforms[newTransformID].Scale = glm ::vec3(1.0f /2.0f);
            scene->Transforms[newTransformID].Translation = glm::vec3(0.0f, 2.0f, 0.0f);
            uint32_t childInstanceID;
            AddMeshInstance(mScene, loadedMeshID, &childInstanceID);
            scene->Transforms[scene->Instances[childInstanceID].TransformID].ParentID =
            scene->Instances[parentInstanceID].TransformID;

            uint32_t childTransformID = scene->Instances[childInstanceID].TransformID;

            scene->Transforms[childTransformID].Translation += glm::vec3(3.0f, -0.8f, -2.0f);
            scene->Transforms[childTransformID].RotationOrigin =-scene->Transforms[childTransformID].Translation;
            scene->Transforms[newTransformID].Rotation =glm ::angleAxis(glm ::radians(-60.0f), glm ::vec3(0.0f, 0.5f, 0.0f));


            uint32_t childInstanceID1;
            uint32_t invisTransformID = scene->Transforms.insert(Transform());
            scene->Transforms[invisTransformID].Scale = glm ::vec3(1.0f);
            scene->Transforms[invisTransformID].ParentID =scene->Instances[childInstanceID].TransformID;

            AddMeshInstance(mScene, loadedMeshID, &childInstanceID1);
            scene->Transforms[scene->Instances[childInstanceID1].TransformID].ParentID = invisTransformID;
            scene->Transforms[invisTransformID].Rotation =glm ::angleAxis(glm ::radians(30.0f), glm ::vec3(0.0f, 1.0f, 0.0f));
            scene->Transforms[scene->Instances[childInstanceID1].TransformID].Translation =glm ::vec3(3.0f, 0.0f, -4.0f);
            //scene->Transforms[invisTransformID].Translation += glm::vec3(0.0f, -10.0f, 0.0f);
            scene->Transforms[newTransformID].Translation += glm::vec3(0.0f, -10.0f, 0.0f);

            mSpinningTransformID=invisTransformID;



    }


    loadedMeshIDs.clear();
    LoadMeshesFromFile(mScene, "assets/floor/floor.obj", &loadedMeshIDs);
    for (uint32_t loadedMeshID : loadedMeshIDs)
    {
        uint32_t newInstanceID;
        AddMeshInstance(mScene, loadedMeshID, &newInstanceID);
        //scene->Transforms[transID].Translation += glm::vec3(0.0f, -10.0f, 0.0f);

        // scale up the cube
        uint32_t newTransformID = scene->Instances[newInstanceID].TransformID;
        scene->Transforms[newTransformID].Translation += glm::vec3(0.0f, -10.0f, 0.0f);
        //scene->Transforms[newTransformID].Scale = glm ::vec3(1.0f/2.0f);
    }

    Camera mainCamera;
    mainCamera.Eye = glm::vec3(17.80f,0.0f,5.0f);
    glm::vec3 target = glm::vec3(0.0f,-10.0f,0.0f);
    mainCamera.Look = normalize(target - mainCamera.Eye);
    mainCamera.Up = glm::vec3(0.0f, 1.0f, 0.0f);
    mainCamera.FovY = glm::radians(70.0f);
    mScene->MainCamera = mainCamera;

    Light mainLight;
    mainLight.Position = glm::vec3(8.8f,20.8f,1.8f);
    mainLight.Direction = normalize(target - mainLight.Position);
    mainLight.Up = glm::vec3(0.0f, 1.0f, 0.0f);
    mainLight.FovY = glm::radians(70.0f);
    mScene->MainLight = mainLight;
}

void Simulation::HandleEvent(const SDL_Event& ev)
{
    if (ev.type == SDL_MOUSEMOTION)
    {
        mDeltaMouseX += ev.motion.xrel;
        mDeltaMouseY += ev.motion.yrel;
    }

}

void Simulation::Update(float deltaTime)
{
    float angularVelocity = 30.0f; // rotating at 30 degrees per second
    mScene->Transforms[mSpinningTransformID].Rotation *=
        glm::angleAxis(
            glm::radians(angularVelocity * deltaTime),
            glm::vec3(0.0f, 1.0f, 0.0f));

    const Uint8* keyboard = SDL_GetKeyboardState(NULL);
    
    int mx, my;
    Uint32 mouse = SDL_GetMouseState(&mx, &my);

    if ((mouse & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0)
    {
        flythrough_camera_update(
            value_ptr(mScene->MainCamera.Eye),
            value_ptr(mScene->MainCamera.Look),
            value_ptr(mScene->MainCamera.Up),
            NULL,
            deltaTime,
            5.0f, // eye_speed
            0.1f, // degrees_per_cursor_move
            80.0f, // max_pitch_rotation_degrees
            mDeltaMouseX, mDeltaMouseY,
            keyboard[SDL_SCANCODE_W], keyboard[SDL_SCANCODE_A], keyboard[SDL_SCANCODE_S], keyboard[SDL_SCANCODE_D],
            keyboard[SDL_SCANCODE_SPACE], keyboard[SDL_SCANCODE_LCTRL],
            0);
    }

    mDeltaMouseX = 0;
    mDeltaMouseY = 0;

    Light& light = mScene->MainLight;
    Camera& camera = mScene->MainCamera;
    if (ImGui::Begin("Example GUI Window"))
    {
        ImGui::Text("Mouse Pos: (%d, %d)", mx, my);
        ImGui::SliderFloat3("Light position", value_ptr(light.Position), -40.0f, 40.0f);
        ImGui::SliderFloat3("Camera position", value_ptr(camera.Eye), -40.0f, 40.0f);
    }
    ImGui::End();
}

void* Simulation::operator new(size_t sz)
{
    // zero out the memory initially, for convenience.
    void* mem = ::operator new(sz);
    memset(mem, 0, sz);
    return mem;
}
