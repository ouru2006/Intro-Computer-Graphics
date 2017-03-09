#include "renderer.h"

#include "scene.h"

#include "imgui.h"
#include "stb_image.h"
#include "preamble.glsl"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL.h>
GLuint loadCubemap(){
    int width,height, comp;

    std::string faces[6];
    faces[0] = "assets/ely_snow/iceflow_ft.tga";
    faces[1] = "assets/ely_snow/iceflow_bk.tga";
    faces[2] = "assets/ely_snow/iceflow_up.tga";
    faces[3] = "assets/ely_snow/iceflow_dn.tga";
    faces[4] = "assets/ely_snow/iceflow_lf.tga";
    faces[5] = "assets/ely_snow/iceflow_rt.tga";
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for(GLuint i = 0; i < 6; i++){
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* image = stbi_load(faces[i].c_str(), &width, &height, &comp, 4);
        stbi_set_flip_vertically_on_load(0);
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return textureID;
}
void Renderer::Init(Scene* scene)
{
    mScene = scene;

    // feel free to increase the GLSL version if your computer supports it
    mShaders.SetVersion("410");
    mShaders.SetPreambleFile("preamble.glsl");

    //depth buffer
    mDepthVisSP = mShaders.AddProgramFromExts({ "depthvis.vert", "depthvis.frag" });
    glGenVertexArrays(1, &mNullVAO);
    glBindVertexArray(mNullVAO);
    glBindVertexArray(0);
    kShadowMapResolution = 512;
    //init skybox
    // Setup skybox VAO
    GLfloat skyboxVertices[] = {
        // Positions
        -50.0f,  50.0f, -50.0f,
        -50.0f, -10.0f, -50.0f,
         50.0f, -10.0f, -50.0f,
         50.0f, -10.0f, -50.0f,
         50.0f,  50.0f, -50.0f,
        -50.0f,  50.0f, -50.0f,

        -50.0f, -10.0f,  50.0f,
        -50.0f, -10.0f, -50.0f,
        -50.0f,  50.0f, -50.0f,
        -50.0f,  50.0f, -50.0f,
        -50.0f,  50.0f,  50.0f,
        -50.0f, -10.0f,  50.0f,

         50.0f, -10.0f, -50.0f,
         50.0f, -10.0f,  50.0f,
         50.0f,  50.0f,  50.0f,
         50.0f,  50.0f,  50.0f,
         50.0f,  50.0f, -50.0f,
         50.0f, -10.0f, -50.0f,

        -50.0f, -10.0f,  50.0f,
        -50.0f,  50.0f,  50.0f,
         50.0f,  50.0f,  50.0f,
         50.0f,  50.0f,  50.0f,
         50.0f, -10.0f,  50.0f,
        -50.0f, -10.0f,  50.0f,

        -50.0f,  50.0f, -50.0f,
         50.0f,  50.0f, -50.0f,
         50.0f,  50.0f,  50.0f,
         50.0f,  50.0f,  50.0f,
        -50.0f,  50.0f,  50.0f,
        -50.0f,  50.0f, -50.0f,

        -50.0f, -10.0f, -50.0f,
        -50.0f, -10.0f,  50.0f,
         50.0f, -10.0f, -50.0f,
         50.0f, -10.0f, -50.0f,
        -50.0f, -10.0f,  50.0f,
         50.0f, -10.0f,  50.0f
    };

       glGenVertexArrays(1, &skyboxVAO);
       glGenBuffers(1, &skyboxVBO);
       glBindVertexArray(skyboxVAO);
       glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
       glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
       glEnableVertexAttribArray(0);
       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
       glBindVertexArray(0);
       cubemapTexture = loadCubemap();

    // Init Shadow FBO
    {

        glDeleteTextures(1, &mShadowDepthTO);
        glGenTextures(1, &mShadowDepthTO);
        glBindTexture(GL_TEXTURE_2D, mShadowDepthTO);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, kShadowMapResolution, kShadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

        const float kShadowBorderDepth[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, kShadowBorderDepth);
        glBindTexture(GL_TEXTURE_2D, 0);

        glDeleteFramebuffers(1, &mShadowFBO);
        glGenFramebuffers(1, &mShadowFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, mShadowFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mShadowDepthTO, 0);
        GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
            fprintf(stderr, "glCheckFramebufferStatus: %x\n", fboStatus);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    mShadowSP = mShaders.AddProgramFromExts({ "shadow.vert", "shadow.frag" });
    mSkyboxSP = mShaders.AddProgramFromExts({ "skybox.vert", "skybox.frag" });
    mSceneSP = mShaders.AddProgramFromExts({ "scene.vert", "scene.frag" });
}

void Renderer::Resize(int width, int height)
{
    mBackbufferWidth = width;
    mBackbufferHeight = height;



    // Init Backbuffer FBO
    {
        glDeleteTextures(1, &mBackbufferColorTO);
        glGenTextures(1, &mBackbufferColorTO);
        glBindTexture(GL_TEXTURE_2D, mBackbufferColorTO);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, mBackbufferWidth, mBackbufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glDeleteTextures(1, &mBackbufferDepthTO);
        glGenTextures(1, &mBackbufferDepthTO);
        glBindTexture(GL_TEXTURE_2D, mBackbufferDepthTO);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mBackbufferWidth, mBackbufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glDeleteFramebuffers(1, &mBackbufferFBO);
        glGenFramebuffers(1, &mBackbufferFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, mBackbufferFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBackbufferColorTO, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mBackbufferDepthTO, 0);
        GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
            fprintf(stderr, "glCheckFramebufferStatus: %x\n", fboStatus);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}


void Renderer::Render()
{
    mShaders.UpdatePrograms();

    if (ImGui::Begin("Renderer Options", 0, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Checkbox("Show shadow map", &mShowDepthVis);
        ImGui::SliderFloat("Slope Scale Bias", &mShadowSlopeScaleBias, 0.0f, 10.0f);
        ImGui::SliderFloat("Depth Bias", &mShadowDepthBias, 0.0f, 1000.0f);
    }
    ImGui::End();

    const Light& mainLight = mScene->MainLight;
    glm::vec3 lightPos = mainLight.Position;
    glm::vec3 lightUp = mainLight.Up;
    glm::mat4 lightWorldView = glm::lookAt(lightPos, lightPos + mainLight.Direction, lightUp);
    glm::mat4 lightViewProjection = glm::perspective(mainLight.FovY, 1.0f, 0.01f, 100.0f);
    glm::mat4 lightWorldProjection = lightViewProjection * lightWorldView;

    glm::mat4 lightOffsetMatrix = glm::mat4(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f);
   // glm::mat4 lightMatrix = lightOffsetMatrix * lightWorldProjection;

    const Camera& mainCamera = mScene->MainCamera;

    glm::vec3 eye = mainCamera.Eye;
    glm::vec3 up = mainCamera.Up;

    glm::mat4 worldView = glm::lookAt(eye, eye + mainCamera.Look, up);
    glm::mat4 viewProjection = glm::perspective(mainCamera.FovY, (float)mBackbufferWidth / mBackbufferHeight, 0.01f, 100.0f);
    glm::mat4 worldProjection = viewProjection * worldView;

    //shadow
    if (*mShadowSP){
        glUseProgram(*mShadowSP);

        GLint SCENE_MODELVIEWPROJECTION_UNIFORM_LOCATION = glGetUniformLocation(*mShadowSP, "ModelViewProjection");

        glBindFramebuffer(GL_FRAMEBUFFER, mShadowFBO);
        glClearDepth(1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, kShadowMapResolution, kShadowMapResolution);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(mShadowSlopeScaleBias, mShadowDepthBias);

        for (uint32_t instanceID : mScene->Instances)
        {
            // TODO: Draw every object in the scene,
            // the same way as the scene rendering pass, but you don't need any material properties.
            // One major different is your viewprojection matrix is now based on the light,
            // rather than the camera.

            const Instance* instance = &mScene->Instances[instanceID];
            const Transform* transform = &mScene->Transforms[instance->TransformID];

            glm::mat4 modelWorld;
            for (const Transform* curr_transform = transform;true;curr_transform = &mScene->Transforms[curr_transform->ParentID]) {
                modelWorld = translate(-curr_transform->RotationOrigin) * modelWorld;
                modelWorld = mat4_cast(curr_transform->Rotation) * modelWorld;
                modelWorld = translate(curr_transform->RotationOrigin) * modelWorld;
                modelWorld = scale(curr_transform->Scale) * modelWorld;
                modelWorld = translate(curr_transform->Translation) * modelWorld;
                if (curr_transform->ParentID == -1) {
                    break;
                }
            }


            glm::mat4 modelViewProjection = lightWorldProjection * modelWorld;

            glProgramUniformMatrix4fv(*mShadowSP, SCENE_MODELVIEWPROJECTION_UNIFORM_LOCATION, 1, GL_FALSE, value_ptr(modelViewProjection));
            const Mesh* mesh = &mScene->Meshes[instance->MeshID];


            glBindVertexArray(mesh->MeshVAO);
            for (size_t meshDrawIdx = 0; meshDrawIdx < mesh->DrawCommands.size(); meshDrawIdx++)
            {
                const GLDrawElementsIndirectCommand* drawCmd = &mesh->DrawCommands[meshDrawIdx];


                glDrawElementsBaseVertex(GL_TRIANGLES, drawCmd->count, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * drawCmd->firstIndex), drawCmd->baseVertex);
            }
            glBindVertexArray(0);
        }
        glPolygonOffset(0.0f, 0.0f);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(0);

    }

    // Clear last frame
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mBackbufferFBO);

        glClearColor(100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // render scene
    if (*mSceneSP)
    {
        glUseProgram(*mSceneSP);

        // GL 4.1 = no shader-specified uniform locations. :( Darn you OSX!!!
        GLint SCENE_MODELWORLD_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "ModelWorld");
        GLint SCENE_NORMAL_MODELWORLD_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "Normal_ModelWorld");
        GLint SCENE_MODELVIEWPROJECTION_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "ModelViewProjection");
        GLint SCENE_CAMERAPOS_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "CameraPos");
        GLint SCENE_HAS_DIFFUSE_MAP_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "HasDiffuseMap");
        GLint SCENE_HAS_SPECULAR_MAP_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "HasSpecularMap");
        GLint SCENE_HAS_BUMP_MAP_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "HasBumpMap");
        GLint SCENE_HAS_ALPHA_MASK_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "HasAlphaMask");
        GLint SCENE_AMBIENT_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "Ambient");
        GLint SCENE_DIFFUSE_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "Diffuse");
        GLint SCENE_SPECULAR_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "Specular");
        GLint SCENE_SHININESS_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "Shininess");
        GLint SCENE_DIFFUSE_MAP_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "DiffuseMap");
        GLint SCENE_SPECULAR_MAP_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "SpecularMap");
        GLint SCENE_BUMP_MAP_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "BumpMap");
        GLint SCENE_ALPHA_MASK_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "AlphaMask");
        GLint SCENE_LIGHT_MATRIX_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "lightMatrix");




        glProgramUniform3fv(*mSceneSP, SCENE_CAMERAPOS_UNIFORM_LOCATION, 1, value_ptr(eye));


        glBindFramebuffer(GL_FRAMEBUFFER, mBackbufferFBO);
        glViewport(0, 0, mBackbufferWidth, mBackbufferHeight);
        glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_DEPTH_TEST);

        glActiveTexture(GL_TEXTURE0 + SCENE_SHADOW_MAP_TEXTURE_BINDING);
        GLint SCENE_SHADOW_MAP_UNIFORM_LOCATION = glGetUniformLocation(*mSceneSP, "ShadowMap");
        glUniform1i(SCENE_SHADOW_MAP_UNIFORM_LOCATION, SCENE_SHADOW_MAP_TEXTURE_BINDING);
        glBindTexture(GL_TEXTURE_2D, mShadowDepthTO);

        for (uint32_t instanceID : mScene->Instances)
        {
            const Instance* instance = &mScene->Instances[instanceID];
            const Mesh* mesh = &mScene->Meshes[instance->MeshID];
            const Transform* transform = &mScene->Transforms[instance->TransformID];
            glm::mat4 modelWorld;
           for (const Transform* curr_transform = transform;true;curr_transform = &mScene->Transforms[curr_transform->ParentID]) {
                modelWorld = translate(-curr_transform->RotationOrigin) * modelWorld;
                modelWorld = mat4_cast(curr_transform->Rotation) * modelWorld;
                modelWorld = translate(curr_transform->RotationOrigin) * modelWorld;
                modelWorld = scale(curr_transform->Scale) * modelWorld;
                modelWorld = translate(curr_transform->Translation) * modelWorld;
                if (curr_transform->ParentID == -1) {
                    break;
                }
            }

            glm::mat4 lightMatrix = lightOffsetMatrix * lightWorldProjection;
            lightMatrix = lightMatrix*modelWorld;
            glProgramUniformMatrix4fv(*mSceneSP, SCENE_LIGHT_MATRIX_UNIFORM_LOCATION, 1, GL_FALSE, value_ptr(lightMatrix));

            glm::mat3 normal_ModelWorld;

            normal_ModelWorld = mat3_cast(transform->Rotation) * normal_ModelWorld;
            normal_ModelWorld = glm::mat3(scale(1.0f / transform->Scale)) * normal_ModelWorld;

            glm::mat4 modelViewProjection = worldProjection * modelWorld;

            glProgramUniformMatrix4fv(*mSceneSP, SCENE_MODELWORLD_UNIFORM_LOCATION, 1, GL_FALSE, value_ptr(modelWorld));
            glProgramUniformMatrix3fv(*mSceneSP, SCENE_NORMAL_MODELWORLD_UNIFORM_LOCATION, 1, GL_FALSE, value_ptr(normal_ModelWorld));
            glProgramUniformMatrix4fv(*mSceneSP, SCENE_MODELVIEWPROJECTION_UNIFORM_LOCATION, 1, GL_FALSE, value_ptr(modelViewProjection));

            glBindVertexArray(mesh->MeshVAO);
            for (size_t meshDrawIdx = 0; meshDrawIdx < mesh->DrawCommands.size(); meshDrawIdx++)
            {
                const GLDrawElementsIndirectCommand* drawCmd = &mesh->DrawCommands[meshDrawIdx];
                const Material* material = &mScene->Materials[mesh->MaterialIDs[meshDrawIdx]];
                //diffuse
                glActiveTexture(GL_TEXTURE0 + SCENE_DIFFUSE_MAP_TEXTURE_BINDING);
                glProgramUniform1i(*mSceneSP, SCENE_DIFFUSE_MAP_UNIFORM_LOCATION, SCENE_DIFFUSE_MAP_TEXTURE_BINDING);
                if ((int)material->DiffuseMapID == -1)
                {
                    glBindTexture(GL_TEXTURE_2D, 0);
                    glProgramUniform1i(*mSceneSP, SCENE_HAS_DIFFUSE_MAP_UNIFORM_LOCATION, 0);
                }
                else
                {
                    const DiffuseMap* diffuseMap = &mScene->DiffuseMaps[material->DiffuseMapID];
                    glBindTexture(GL_TEXTURE_2D, diffuseMap->DiffuseMapTO);
                    glProgramUniform1i(*mSceneSP, SCENE_HAS_DIFFUSE_MAP_UNIFORM_LOCATION, 1);
                }
                //specular
                glActiveTexture(GL_TEXTURE0 + SCENE_SPECULAR_MAP_TEXTURE_BINDING);
                glProgramUniform1i(*mSceneSP, SCENE_SPECULAR_MAP_UNIFORM_LOCATION, SCENE_SPECULAR_MAP_TEXTURE_BINDING);
                if ((int)material->SpecularMapID == -1)
                {
                    glBindTexture(GL_TEXTURE_2D, 0);
                    glProgramUniform1i(*mSceneSP, SCENE_HAS_SPECULAR_MAP_UNIFORM_LOCATION, 0);
                }
                else
                {
                    const SpecularMap* specularMap = &mScene->SpecularMaps[material->SpecularMapID];
                    glBindTexture(GL_TEXTURE_2D, specularMap->SpecularMapTO);
                    glProgramUniform1i(*mSceneSP, SCENE_HAS_SPECULAR_MAP_UNIFORM_LOCATION, 1);
                }
                //bump
                glActiveTexture(GL_TEXTURE0 + SCENE_BUMP_MAP_TEXTURE_BINDING);
                glProgramUniform1i(*mSceneSP, SCENE_BUMP_MAP_UNIFORM_LOCATION, SCENE_BUMP_MAP_TEXTURE_BINDING);
                if ((int)material->BumpMapID == -1)
                {
                    glBindTexture(GL_TEXTURE_2D, 0);
                    glProgramUniform1i(*mSceneSP, SCENE_HAS_BUMP_MAP_UNIFORM_LOCATION, 0);
                }
                else
                {
                    const BumpMap* bumpMap = &mScene->BumpMaps[material->BumpMapID];
                    glBindTexture(GL_TEXTURE_2D, bumpMap->BumpMapTO);
                    glProgramUniform1i(*mSceneSP, SCENE_HAS_BUMP_MAP_UNIFORM_LOCATION, 1);
                }
                //alpha mask
                glActiveTexture(GL_TEXTURE0 + SCENE_ALPHA_MASK_TEXTURE_BINDING);
                glProgramUniform1i(*mSceneSP, SCENE_ALPHA_MASK_UNIFORM_LOCATION, SCENE_ALPHA_MASK_TEXTURE_BINDING);
                if ((int)material->AlphaMaskID == -1)
                {
                    glBindTexture(GL_TEXTURE_2D, 0);
                    glProgramUniform1i(*mSceneSP, SCENE_HAS_ALPHA_MASK_UNIFORM_LOCATION, 0);
                }
                else
                {
                    const AlphaMask* alphaMask = &mScene->AlphaMasks[material->AlphaMaskID];
                    glBindTexture(GL_TEXTURE_2D, alphaMask->AlphaMaskTO);
                    glProgramUniform1i(*mSceneSP, SCENE_HAS_ALPHA_MASK_UNIFORM_LOCATION, 1);
                }

                glProgramUniform3fv(*mSceneSP, SCENE_AMBIENT_UNIFORM_LOCATION, 1, material->Ambient);
                glProgramUniform3fv(*mSceneSP, SCENE_DIFFUSE_UNIFORM_LOCATION, 1, material->Diffuse);
                glProgramUniform3fv(*mSceneSP, SCENE_SPECULAR_UNIFORM_LOCATION, 1, material->Specular);
                glProgramUniform1f(*mSceneSP, SCENE_SHININESS_UNIFORM_LOCATION, material->Shininess);

                glDrawElementsBaseVertex(GL_TRIANGLES, drawCmd->count, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * drawCmd->firstIndex), drawCmd->baseVertex);
            }
            glBindVertexArray(0);
        }
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_FRAMEBUFFER_SRGB);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glUseProgram(0);
    }

    //render skybox
    if (*mSkyboxSP){
        glDepthMask(GL_FALSE);// Remember to turn depth writing off
        glUseProgram(*mSkyboxSP);
        glBindFramebuffer(GL_FRAMEBUFFER, mBackbufferFBO);
        glViewport(0, 0, mBackbufferWidth, mBackbufferHeight);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        // Draw skybox first
        glProgramUniformMatrix4fv(*mSkyboxSP,glGetUniformLocation(*mSkyboxSP, "view"), 1, GL_FALSE, glm::value_ptr(worldView));
        glProgramUniformMatrix4fv(*mSkyboxSP,glGetUniformLocation(*mSkyboxSP, "projection"), 1, GL_FALSE, glm::value_ptr(viewProjection));
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(*mSkyboxSP, "skybox"), 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glDepthFunc(GL_LESS);
        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(0);
        glDepthMask(GL_TRUE);

    }
    if (mShowDepthVis && *mDepthVisSP)
    {
        glUseProgram(*mDepthVisSP);
        GLint DEPTHVIS_TRANSFORM2D_UNIFORM_LOCATION = glGetUniformLocation(*mDepthVisSP,"Transform2D");
        GLint DEPTHVIS_ORTHO_PROJECTION_UNIFORM_LOCATION = glGetUniformLocation(*mDepthVisSP,"OrthoProjection");
        GLint DEPTHVIS_DEPTH_MAP_UNIFORM_LOCATION = glGetUniformLocation(*mDepthVisSP,"DepthMap");
        glBindFramebuffer(GL_FRAMEBUFFER, mBackbufferFBO);
        glViewport(0, 0, mBackbufferWidth, mBackbufferHeight);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        float depthBufferVisSize = 256.0f;
        glm::mat4 transform2D =
        glm::translate(glm::vec3((float)(mBackbufferWidth - depthBufferVisSize),(float)(mBackbufferHeight - depthBufferVisSize), 0.0f)) *
        glm::scale(glm::vec3(depthBufferVisSize, depthBufferVisSize, 0.0f));
        glProgramUniformMatrix4fv(*mDepthVisSP, DEPTHVIS_TRANSFORM2D_UNIFORM_LOCATION, 1, GL_FALSE, value_ptr(transform2D));

        glm::mat4 ortho = glm::ortho(0.0f, (float)mBackbufferWidth, 0.0f,(float)mBackbufferHeight);
        glUniformMatrix4fv(DEPTHVIS_ORTHO_PROJECTION_UNIFORM_LOCATION, 1, GL_FALSE, value_ptr(ortho));

        glActiveTexture(GL_TEXTURE0 + DEPTHVIS_DEPTH_MAP_TEXTURE_BINDING);
        glUniform1i(DEPTHVIS_DEPTH_MAP_UNIFORM_LOCATION, DEPTHVIS_DEPTH_MAP_TEXTURE_BINDING);
        glBindTexture(GL_TEXTURE_2D, mShadowDepthTO);

        // need to disable depth comparison before sampling with non-shadow sampler
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glBindVertexArray(mNullVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);

        // re-enable depth comparison
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glDisable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(0);
    }

    // Render ImGui
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mBackbufferFBO);
        ImGui::Render();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // copy to window
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mBackbufferFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(
            0, 0, mBackbufferWidth, mBackbufferHeight,
            0, 0, mBackbufferWidth, mBackbufferHeight,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void* Renderer::operator new(size_t sz)
{
    // zero out the memory initially, for convenience.
    void* mem = ::operator new(sz);
    memset(mem, 0, sz);
    return mem;
}

