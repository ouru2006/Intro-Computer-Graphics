uniform vec3 CameraPos;

uniform vec3 Ambient;
uniform vec3 Diffuse;
uniform vec3 Specular;
uniform float Shininess;

uniform int HasDiffuseMap;

uniform int HasSpecularMap;
uniform int HasBumpMap;
uniform int HasAlphaMask;

uniform sampler2D DiffuseMap;

uniform sampler2D SpecularMap;
uniform sampler2D BumpMap;
uniform sampler2D AlphaMask;

in vec3 normalInterp;
in vec3 vertPos;
in vec2 fragment_texcoord;
out vec4 FragColor;

in vec4 shadowMapCoord;
uniform sampler2DShadow ShadowMap;

void main()
{
    // TODO: Replace with Phong shading

    vec3 normal = normalize(normalInterp);
    vec3 lightDir = normalize(CameraPos - vertPos);
    float lambertian = max(dot(lightDir, normal), 0.0);
    vec3 viewDir = normalize(-vertPos);
    // this is Blinn-Phong shading

    vec3 halfDir = normalize(lightDir + viewDir);
    float specAngle = max(dot(halfDir, normal), 0.0);
    float spec = pow(specAngle, Shininess);

    if (HasAlphaMask != 0){
        if (texture(AlphaMask, fragment_texcoord).r < 0.9){ discard; }
    }
    vec3 Diffusemap;
    if (HasDiffuseMap != 0)
        Diffusemap = texture(DiffuseMap, fragment_texcoord).rgb;
    else Diffusemap = Diffuse;
    vec3 Specularmap;
    if (HasSpecularMap != 0)
        Specularmap = texture(SpecularMap, fragment_texcoord).rgb;
    else Specularmap=Specular;
    vec3 colorLinear = 0.02*Ambient+lambertian*Diffusemap+spec*Specularmap;

    float visibility = textureProj(ShadowMap, shadowMapCoord);
    //FragColor =vec4(shadowMapCoord.xxx/shadowMapCoord.www, 1.0);
    FragColor = vec4( colorLinear - (1.0 - visibility) * Ambient*0.5, 1.0);
    //FragColor = vec4(visibility * colorLinear, 1.0);
   //FragColor = vec4(1.0);

}
