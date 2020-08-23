
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <cassert>
#include <iostream>
#include <functional>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <optional>
#include <set>

struct QueryFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentationFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() && presentationFamily.has_value();
    }

    std::set<uint32_t> uniques() const {
        return {
            graphicsFamily.value(), 
            presentationFamily.value(), 
        };
    }
};


struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
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

static const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

static const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


namespace XE {
    const int SCREEN_WIDTH = 1024;
    const int SCREEN_HEIGHT = 768;

    const bool enableValidationLayers = true;

    class TriangleVulkanApplication {
    public:
        void run() {
            initWindow();
            initVulkan();
            loop();
            terminate();
        }

    private:
        void initWindow() {
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            mWindow = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Vulkan Window", nullptr, nullptr);
        }

        void initVulkan() {
            setupInstance();

            if (enableValidationLayers) {
                setupDebugMessenger();
            }

            createSurface();
            pickPhysicalDevice();
            createLogicalDevice();
        }

        void loop() {
            while (!glfwWindowShouldClose(mWindow)) {
                glfwPollEvents();
            }
        }

        void terminate() {
            vkDestroyDevice(mDevice, nullptr);

            if (enableValidationLayers) {
                DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
            }

            vkDestroySurfaceKHR(mInstance, mSurface, nullptr);

            vkDestroyInstance(mInstance, nullptr);
            
            if (mWindow) {
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

            if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS) {
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

            if (CreateDebugUtilsMessengerEXT(mInstance, &msgInfo, nullptr, &mDebugMessenger) != VK_SUCCESS) {
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
                    mPhysicaldevice = device;
                    return;
                }
            }

            throw std::runtime_error("No suitable GPU found.");
        }

        void createLogicalDevice() {
            const auto indices = findQueueFamilies(mPhysicaldevice);
            const float queuePriority = 1.0f;

            std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

            for (const uint32_t queueFamily : indices.uniques()) {
                VkDeviceQueueCreateInfo queueCreateInfo = {};
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = queueFamily;
                queueCreateInfo.pQueuePriorities = &queuePriority;
                queueCreateInfo.queueCount = 1;

                queueCreateInfos.push_back(queueCreateInfo);
            }
            
            VkPhysicalDeviceFeatures deviceFeatures = {};
            
            VkDeviceCreateInfo deviceInfo = {};
            deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceInfo.pQueueCreateInfos = queueCreateInfos.data();
            deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
            deviceInfo.pEnabledFeatures = &deviceFeatures;

            deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
            deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

            if (enableValidationLayers) {
                deviceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                deviceInfo.ppEnabledLayerNames = validationLayers.data();
            } else {
                deviceInfo.enabledLayerCount = 0;
            }

            if (vkCreateDevice(mPhysicaldevice, &deviceInfo, nullptr, &mDevice) != VK_SUCCESS) {
                throw std::runtime_error("failed to create logical device!");
            }

            vkGetDeviceQueue(mDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);
            vkGetDeviceQueue(mDevice, indices.presentationFamily.value(), 0, &mPresentationQueue);
        }

        std::vector<VkPhysicalDevice> enumeratePhysicalDevices() const {
            uint32_t count;

            vkEnumeratePhysicalDevices(mInstance, &count, nullptr);

            std::vector<VkPhysicalDevice> physicalDevices;
            physicalDevices.resize(count);
            vkEnumeratePhysicalDevices(mInstance, &count, physicalDevices.data());

            return physicalDevices;
        }

        //! Checks if the specified device is a Hardware accelerated GPU
        bool isDeviceSuitable(VkPhysicalDevice device) const {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);

            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(device, &features);

            // filter the device
            const bool isGpuWithGeometryShader = (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) && features.geometryShader;

            if (isGpuWithGeometryShader) {
                const bool extensionsSupported = checkDeviceExtensionSupport(device);

                bool swapChainAdequate = false;
                if (extensionsSupported) {
                    const auto details = querySwapChainSupport(device);
                    swapChainAdequate = !details.formats.empty() && !details.presentModes.empty();
                }

                const auto indices = findQueueFamilies(device);
                return indices.isComplete() && extensionsSupported && swapChainAdequate;
            }

            return false;
        }

        QueryFamilyIndices findQueueFamilies(VkPhysicalDevice device) const {
            assert(device);

            uint32_t count;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

            std::vector<VkQueueFamilyProperties> properties;
            properties.resize(count);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &count, properties.data());

            QueryFamilyIndices indices;

            for (uint32_t i = 0; i < static_cast<uint32_t>(properties.size()); i++) {
                // graphics support
                if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    indices.graphicsFamily = static_cast<int>(i);
                }

                // presentation support
                VkBool32 presentationSupport = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentationSupport);

                if (presentationSupport == VK_TRUE) {
                    indices.presentationFamily = i;
                }
            }

            return indices;
        }

        void createSurface() {
            if (glfwCreateWindowSurface(mInstance, mWindow, nullptr, &mSurface) != VK_SUCCESS) {
                throw std::runtime_error("failed to create window surface");
            }
        }


        bool checkDeviceExtensionSupport(VkPhysicalDevice device) const {
            uint32_t count;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);

            std::vector<VkExtensionProperties> extensions;
            extensions.resize(count);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &count, extensions.data());

            std::set<std::string> requiredExtensionNames(deviceExtensions.begin(), deviceExtensions.end());

            for (const auto& extension : extensions) {
                requiredExtensionNames.erase(extension.extensionName);
            }

            return requiredExtensionNames.empty();
        }

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const {
            SwapChainSupportDetails details;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &details.capabilities);

            // format count
            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);

            if (formatCount > 0) {
                details.formats.resize(formatCount);
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, details.formats.data());
            }

            // present modes
            uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);
            if (presentModeCount > 0) {
                details.presentModes.resize(presentModeCount);
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, details.presentModes.data());
            }

            return details;
        }

    private:
        GLFWwindow *mWindow = nullptr;
        VkInstance mInstance;
        VkDebugUtilsMessengerEXT mDebugMessenger;

        VkPhysicalDevice mPhysicaldevice = VK_NULL_HANDLE;
        VkDevice mDevice;
        VkQueue mGraphicsQueue;
        VkQueue mPresentationQueue;

        VkSurfaceKHR mSurface;
    };
}


int main() {
    XE::TriangleVulkanApplication app;

    try {
        app.run();
    } catch (const std::exception &exp) {
        std::cout << exp.what() << std::endl;
    }

    return 0;
}
