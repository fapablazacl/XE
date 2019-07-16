
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <iostream>
#include <functional>
#include <string>
#include <stdexcept>

namespace XE {
    const int ScreenWidth = 1024;
    const int ScreenHeight = 768;

    const bool enableValidationLayers = true;

    class TriangleApplication {
    public:
        void run() {
            this->initWindow();
            this->initVulkan();
            this->loop();
            this->terminate();
        }

    private:
        void initWindow() {
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Vulkan Window", nullptr, nullptr);

        }

        void initVulkan() {
            this->setupInstance();
        }

        void loop() {
            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
            }
        }

        void terminate() {
            vkDestroyInstance(instance, nullptr);
            
            if (window) {
                glfwPollEvents();
            }

            glfwTerminate();
        }

        void setupInstance() {
            if (enableValidationLayers && !checkValidationLayers()) {
                throw std::runtime_error("Vulkan Validation Layers requested but Not Available!");
            }

            VkApplicationInfo info = {};

            info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            info.pApplicationName = "Hello Triangle";
            info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            info.pEngineName = "No Engine";
            info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            info.apiVersion = VK_API_VERSION_1_0;

            VkInstanceCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &info;

            uint32_t extensionCount = 0;
            const char **extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

            createInfo.enabledExtensionCount = extensionCount;
            createInfo.ppEnabledExtensionNames = extensions;
            createInfo.enabledLayerCount = 0;

            if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create Instance");
            }
        }

        bool checkValidationLayers() const {
            uint32_t layerCount;

            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
            std::vector<VkLayerProperties> layerProperties(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

            auto layerIt = std::find_if(layerProperties.begin(), layerProperties.end(), [](const VkLayerProperties &layer) {
                return std::string(layer.layerName) == "VK_LAYER_KHRONOS_validation";
            });

            return layerIt != layerProperties.end();
        }

    private:
        GLFWwindow *window = nullptr;
        VkInstance instance;
    };
}

int main() {
    XE::TriangleApplication app;

    try {
        app.run();
    } catch (const std::exception &exp) {
        std::cout << exp.what() << std::endl;
    }

    return 0;
}
