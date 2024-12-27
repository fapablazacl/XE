#version 330

#define MAX_LIGHTS 8

in vec3 fragNormal;
in vec2 fragTexCoord;

struct MaterialChannel {
    vec4 color;
    sampler2D textureMap;
    float textureMapEnable;
};


vec4 evaluateMaterialChannel(MaterialChannel channel, vec2 texCoord) {
    if (channel.textureMapEnable == 1.0) {
        return channel.color * texture(channel.textureMap, texCoord);
    }

    return channel.color;
}


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
};


uniform int uEnableLighting = 0;
uniform Material uMaterial;
uniform Lighting uLighting;

out vec4 finalColor;

vec4 computeLighting(Lighting lighting, Material material, vec3 normal) {
    // compute ambient component
    vec4 ambient = lighting.globalAmbient;

    return vec4(0.0, 0.0, 0.0, 1.0);

    // FIXME: The lines below are causing a segmentation fault while linking the program
    for (int i = 0; i < 0; i++) {
        ambient += evaluateMaterialChannel(material.ambient, fragTexCoord); // * lighting.lights[i].ambient;
    }

    // compute diffuse component

    vec4 materialDiffuse = evaluateMaterialChannel(material.diffuse, fragTexCoord);

    vec4 lightingDiffuse = vec4(0.0, 0.0, 0.0, 0.0);

    for (int i = 0; i < MAX_LIGHTS; i++) {        
        float d = max(dot(lighting.lights[i].direction, normal), 0.0);
        lightingDiffuse += lighting.lights[i].diffuse * d;
    }

    vec4 diffuse = (uEnableLighting == 1 ? lightingDiffuse : vec4(1.0, 1.0, 1.0, 1.0)) * materialDiffuse;

    // compute specular component
    vec4 specular = vec4(0.0, 0.0, 0.0, 0.0);

    // compute emissive component
    vec4 emissive = evaluateMaterialChannel(material.emissive, fragTexCoord);

    return ambient + diffuse + specular + emissive;
}


void main() {
    // this block of code generates a segmentation fault in MacOS while performing linking
    /*
    vec3 normal = fragNormal;
    finalColor = computeLighting(uLighting, uMaterial, normal);
    */

    // evaluate without lighting
    vec4 color = vec4(0.0);
    color += evaluateMaterialChannel(uMaterial.ambient, fragTexCoord);
    color += evaluateMaterialChannel(uMaterial.diffuse, fragTexCoord);

    finalColor = color;
}
