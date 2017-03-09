layout (location = SCENE_POSITION_ATTRIB_LOCATION)
in vec4 position;
out vec4 TexCoords;

uniform mat4 projection;
uniform mat4 view;


void main()
{
    gl_Position =   projection * view * position;
    gl_Position.z = gl_Position.w;
    TexCoords = position;
}
