#version 410core

#define MAX_LIGHTS 8

struct MaterialChannel {
    vec4 color;
    sampler2D textureMap;
    float textureMapEnable;
};

struct Material {
    MaterialChannel ambient;
    MaterialChannel diffuse;
    MaterialChannel specular;
    MaterialChannel emissive;
};

struct Light {
    vec3 direction;
    vec4 ambient;
    vec4 diffuse;
};

struct Lighting {
    vec4 globalAmbient;
    Light lights[MAX_LIGHTS];
    int lightCount;
};

uniform int uEnableLighting = 0;
uniform Material uMaterial;
uniform Lighting uLighting;

in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 finalColor;

vec4 evaluateMaterialChannel(MaterialChannel channel, vec2 texCoord) {
    if (channel.textureMapEnable == 1.0) {
        return channel.color * texture(channel.textureMap, texCoord);
    }

    return channel.color;
}

vec4 computeLighting() {
    // compute ambient component
    vec4 ambient = uLighting.globalAmbient;

    vec4 lightAmbient = vec4(0.0);
    for (int i = 0; i < uLighting.lightCount; i++) {
        lightAmbient +=  uLighting.lights[i].ambient;
    }
    ambient += lightAmbient * evaluateMaterialChannel(uMaterial.ambient, fragTexCoord);

    // compute diffuse component
    vec4 materialDiffuse = evaluateMaterialChannel(uMaterial.diffuse, fragTexCoord);

    vec4 lightingDiffuse = vec4(0.0, 0.0, 0.0, 0.0);

    for (int i = 0; i < uLighting.lightCount; i++) {
        float d = max(dot(uLighting.lights[i].direction, fragNormal), 0.0);
        lightingDiffuse += uLighting.lights[i].diffuse * d;
    }

    vec4 diffuse = lightingDiffuse * materialDiffuse;

    // compute specular component
    vec4 specular = vec4(0.0, 0.0, 0.0, 0.0);

    // compute emissive component
    vec4 emissive = evaluateMaterialChannel(uMaterial.emissive, fragTexCoord);

    return ambient + diffuse + specular + emissive;
}


void main() {
    if (uEnableLighting == 0) {
        vec4 color = vec4(0.0);
        color += evaluateMaterialChannel(uMaterial.ambient, fragTexCoord);
        color += evaluateMaterialChannel(uMaterial.diffuse, fragTexCoord);

        finalColor = color;
    }
    else {
        // this block of code generates a segmentation fault in MacOS while performing linking
        finalColor = computeLighting();
    }
}
