#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D texture_atlas;
uniform sampler2D shadow_depth_map;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(0.002 * (1.0 - dot(normal, lightDir)), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = (1.0 / textureSize(shadow_depth_map, 0)) * 0.75;
    
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadow_depth_map, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
        
    return shadow;
}

void main()
{
    vec4 texColor = texture(texture_atlas, fs_in.TexCoords);
    if (texColor.a < 0.1)
        discard;

    vec3 albedo = pow(texColor.rgb, vec3(2.2));

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec3 sunColor    = vec3(1.05, 0.98, 0.88);
    vec3 skyColor    = vec3(0.45, 0.62, 0.85);
    vec3 groundColor = vec3(0.28, 0.24, 0.20);

    float hemiFactor = normal.y * 0.5 + 0.5;
    vec3 ambient = mix(groundColor, skyColor, hemiFactor) * 0.55;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * sunColor;

    float rim = 1.0 - max(dot(viewDir, normal), 0.0);
    rim = smoothstep(0.6, 1.0, rim) * 0.15;
    vec3 rimLight = skyColor * rim * (0.3 + 0.7 * diff);

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);

    vec3 lighting = ambient + (1.0 - shadow) * diffuse + rimLight;
    vec3 result = albedo * lighting;

    float dist = length(viewPos - fs_in.FragPos);
    float fogStart = 48.0;
    float fogEnd   = 128.0;
    float fogFactor = clamp((dist - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    vec3 fogColor = mix(vec3(0.55, 0.72, 0.92), sunColor * 0.85, pow(max(dot(-viewDir, lightDir), 0.0), 4.0) * 0.4);
    result = mix(result, pow(fogColor, vec3(2.2)), fogFactor * fogFactor);

    result = result / (result + vec3(1.0));
    result = pow(result, vec3(1.0 / 2.2));

    FragColor = vec4(result, texColor.a);
}