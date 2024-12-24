
#include <apostate/ScenarioParser.h>

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <optional>


namespace apostate {
    namespace ir {
        struct Shader {
            std::optional<std::string> type;
            std::optional<std::string> src;
        };

        struct ShaderProgram {
            std::vector<Shader> shaders;
        };

        struct Color {
            std::optional<float> red;  
            std::optional<float> green;
            std::optional<float> blue;
            std::optional<float> alpha;
        };

        struct MaterialComponent {
            std::optional<Color> color;
        };

        struct Material {
            std::optional<std::string> name;
            std::optional<MaterialComponent> ambient;
            std::optional<MaterialComponent> diffuse;
            std::optional<MaterialComponent> specular;
            std::optional<MaterialComponent> emissive;
            std::optional<float> shininess;
        };

        struct LightingModel {
            std::optional<ShaderProgram> shaderProgram;
            std::vector<Material> materials;
        };

        /*
{
  "lighting_model": {
    "shader_program": {
      "shaders": [
        { "type": "vertex", "src": "gouraud.vert" },
        { "type": "fragment", "src": "gouraud.frag" }
      ]
    },
    "materials": [
      {
        "name": "basic",
        "ambient": [0.2, 0.2, 0.2, 1.0],
        "diffuse": [0.8, 0.8, 0.8, 1.0],
        "specular": [0.0, 0.0, 0.0, 1.0],
        "shininess": 0.0,
        "emissive": [0.0, 0.0, 0.0, 1.0]
      }
    ]
  },
  "models": [
    {
      "type": "box",
      "name": "box01",
      "center": [0.0, 0.0, 0.0],
      "size": [10.0, 1.0, 10.0],
      "cell_size": [0.5, 0.5, 0.5],
      "material": "basic"
    }
  ],
  "scene": {
    "lighting": {
      "lights": [
        {
          "type": "directional",
          "direction": [0.5, 1.0, 0.25],
          "ambient": [0.2, 0.2, 0.2, 1.0],
          "diffuse": [0.8, 0.8, 0.8, 0.8]
        }, {
          "type": "directional",
          "direction": [-0.5, -0.1, 0.25],
          "ambient": [0.0, 0.0, 0.0, 1.0],
          "diffuse": [0.8, 0.8, 0.8, 0.8]
        }, {
          "type": "directional",
          "direction": [0.0, 0.0, -1.0],
          "ambient": [0.2, 0.2, 0.2, 1.0],
          "diffuse": [0.8, 0.8, 0.8, 0.8]
        }
      ]
    },
    "camera": {
      "position": [0.0, 0.25, 10.0],
      "direction": [0.0, 0.0, 0.0],
      "up": [0.0, 1.0, 0.0]
    },
    "node": {
      "transformation": "identity",
      "children": [
        {

        }
      ]
    }
  }
}
        */
    }
}
