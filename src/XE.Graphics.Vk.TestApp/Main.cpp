
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <iostream>
#include <functional>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <optional>

struct QueryFamilyIndices {
    std::optional<uint32_t> graphicsFamily;

    bool isComplete() const {
        return graphicsFamily.has_value();
    }
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) {

    std::cout << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

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
            setupInstance();

            if (enableValidationLayers) {
                setupDebugMessenger();
            }
        }

        void loop() {
            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
            }
        }

        void terminate() {
            if (enableValidationLayers) {
                DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
            }

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

            const auto extensions = getRequiredExtensions();
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();

            auto debugCreateInfo = makeDebugMessengerInfo();

            if (enableValidationLayers) {
                createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                createInfo.ppEnabledLayerNames = validationLayers.data();

                createInfo.pNext = &debugCreateInfo;
            } else {
                createInfo.enabledLayerCount = 0;
                createInfo.pNext = nullptr;
            }

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

        std::vector<const char*> getRequiredExtensions() const {
            uint32_t extensionCount;
            const char **extensions = ::glfwGetRequiredInstanceExtensions(&extensionCount);

            std::vector<const char*> result = {extensions, extensions + extensionCount};

            if (enableValidationLayers) {
                result.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }

            return result;
        }

        VkDebugUtilsMessengerCreateInfoEXT makeDebugMessengerInfo() const {
            VkDebugUtilsMessengerCreateInfoEXT msgInfo {};

            msgInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            msgInfo.messageSeverity 
                = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT 
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT 
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

            msgInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | 
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

            msgInfo.pfnUserCallback = debugCallback;
            msgInfo.pUserData = nullptr;

            return msgInfo;
        }

        void setupDebugMessenger() {
            VkDebugUtilsMessengerCreateInfoEXT msgInfo = makeDebugMessengerInfo();

            if (CreateDebugUtilsMessengerEXT(instance, &msgInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
                throw std::runtime_error("failed to setup the debug messenger");
            }
        }

        void pickPhysicalDevice() {
            auto devices = enumeratePhysicalDevices();

            if (devices.size() == 0) {
                throw std::runtime_error("There is no available GPUs");
            }

            for (const auto& device : devices) {
                if (isDeviceSuitable(device)) {
                    physicaldevice = device;
                }
            }
        }

        std::vector<VkPhysicalDevice> enumeratePhysicalDevices() const {
            uint32_t count;

            vkEnumeratePhysicalDevices(instance, &count, nullptr);

            std::vector<VkPhysicalDevice> physicalDevices;
            physicalDevices.reserve(count);
            vkEnumeratePhysicalDevices(instance, &count, physicalDevices.data());
        }

        //! Checks if the specified device is a Hardware accelerated GPU
        bool isDeviceSuitable(VkPhysicalDevice device) const {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);

            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(device, &features);

            return (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) && features.geometryShader;
        }

        QueryFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
            uint32_t count;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

            std::vector<VkQueueFamilyProperties> properties;
            properties.reserve(count);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &count, properties.data());

            QueryFamilyIndices indices;

            for (size_t i = 0; i < properties.size(); i++) {
                if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    indices.graphicsFamily = static_cast<int>(i);
                    break;
                }
            }

            return indices;
        }

    private:
        GLFWwindow *window = nullptr;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;

        VkPhysicalDevice physicaldevice = VK_NULL_HANDLE;
        VkDevice logicalDevice;
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
