in vec4 TexCoords;
out vec4 color;

uniform samplerCube skybox;

void main()
{

    color = texture(skybox, vec3(TexCoords));
    //color=vec4(1.0);
}
