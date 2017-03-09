layout(location = SCENE_POSITION_ATTRIB_LOCATION)
in vec4 Position;

layout(location = SCENE_TEXCOORD_ATTRIB_LOCATION)
in vec2 TexCoord;

layout(location = SCENE_NORMAL_ATTRIB_LOCATION)
in vec3 Normal;

uniform mat4 ModelWorld;
uniform mat4 ModelViewProjection;
uniform mat3 Normal_ModelWorld;
out vec2 fragment_texcoord;
out vec3 normalInterp;
out vec3 vertPos;

uniform mat4 lightMatrix;
out vec4 shadowMapCoord;

void main()
{
    // TODO: Set to MVP * P
    gl_Position = ModelViewProjection*Position;
    vec4 vertPos4 = ModelWorld * Position;
    vertPos = vec3(vertPos4) / vertPos4.w;
    normalInterp = Normal_ModelWorld*Normal;
    // TODO: Pass vertex attributes to fragment shader
    fragment_texcoord = TexCoord;
    shadowMapCoord = lightMatrix * Position;
}
