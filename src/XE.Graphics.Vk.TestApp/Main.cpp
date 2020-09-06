
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
#include <fstream>

struct QueryFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }

    std::set<uint32_t> uniques() const {
        return {
            graphicsFamily.value(), 
            presentFamily.value(), 
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

static std::vector<char> readFile(const std::string& filename) {
    std::ifstream fs{filename.c_str(), std::ios::ate | std::ios::binary};

    if (! fs.is_open()) {
        throw std::runtime_error("failed to open file " + filename);
    }

    const auto fileSize = static_cast<size_t>(fs.tellg());

    std::vector<char> buffer;
    buffer.resize(fileSize);

    fs.seekg(0);
    fs.read(buffer.data(), fileSize);
    fs.close();

    return buffer;
}

static const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

static const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


namespace XE {
    const uint32_t SCREEN_WIDTH = 1024;
    const uint32_t SCREEN_HEIGHT = 768;

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
            createSwapChain();
            createGraphicsPipeline();
        }

        void loop() {
            while (!glfwWindowShouldClose(mWindow)) {
                glfwPollEvents();
            }
        }

        void terminate() {
            for (VkImageView imageView : mSwapchainImageViews) {
                vkDestroyImageView(mDevice, imageView, nullptr);
            }

            vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);

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
            vkGetDeviceQueue(mDevice, indices.presentFamily.value(), 0, &mPresentationQueue);
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
                    indices.presentFamily = i;
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


        std::optional<VkSurfaceFormatKHR> chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceformats) const {
            for (auto surfaceFormat : surfaceformats) {
                if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    return surfaceFormat;
                }
            }

            return {};
        }

        VkPresentModeKHR chooseSwapPresentMode(const std::set<VkPresentModeKHR>& presentModes) const {
            if (const auto it = presentModes.find(VK_PRESENT_MODE_MAILBOX_KHR); it != presentModes.end()) {
                return VK_PRESENT_MODE_MAILBOX_KHR;
            }

            return VK_PRESENT_MODE_FIFO_KHR;
        }

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& caps) const {
            if (caps.currentExtent.width != UINT32_MAX) {
                return caps.currentExtent;
            }

            return {
                std::max(caps.minImageExtent.width, std::min(caps.maxImageExtent.width, SCREEN_WIDTH )), 
                std::max(caps.minImageExtent.height, std::min(caps.maxImageExtent.height, SCREEN_HEIGHT))
            };
        }
        
        uint32_t chooseImageCount(const VkSurfaceCapabilitiesKHR& caps) const {
            const uint32_t imageCount = caps.minImageCount + 1;

            if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount) {
                return caps.maxImageCount;
            }

            return imageCount;
        }


        void createSwapChain() {
            const SwapChainSupportDetails swapChainSupport = querySwapChainSupport(mPhysicaldevice);
            const std::optional<VkSurfaceFormatKHR> surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);

            if (!surfaceFormat) {
                throw std::runtime_error("No suitable swapchain surface format found");
            }

            const VkPresentModeKHR presentMode = chooseSwapPresentMode({swapChainSupport.presentModes.begin(), swapChainSupport.presentModes.end()});
            const VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
            const uint32_t imageCount = chooseImageCount(swapChainSupport.capabilities);

            VkSwapchainCreateInfoKHR info = {};
            info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            info.surface = mSurface;
            info.minImageCount = imageCount;
            info.imageFormat = surfaceFormat->format;
            info.imageColorSpace = surfaceFormat->colorSpace;
            info.imageExtent = extent;
            info.imageArrayLayers = 1;
            info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            const QueryFamilyIndices indices = findQueueFamilies(mPhysicaldevice);
            const uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

            if (indices.graphicsFamily != indices.presentFamily) {
                info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                info.queueFamilyIndexCount = 2;
                info.pQueueFamilyIndices = queueFamilyIndices;
            } else {
                info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                info.queueFamilyIndexCount = 0;
                info.pQueueFamilyIndices = nullptr;
            }

            info.preTransform = swapChainSupport.capabilities.currentTransform;
            info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            info.presentMode = presentMode;
            info.clipped = VK_TRUE;
            info.oldSwapchain = VK_NULL_HANDLE;

            if (vkCreateSwapchainKHR(mDevice, &info, nullptr, &mSwapchain) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create swapchain!");
            }

            mSwapchainImageFormat = surfaceFormat->format;
            mSwapchainExtent = extent;

            retrieveSwapchainImages();
            createImageViews();
        }

        void retrieveSwapchainImages() {
            uint32_t imageCount;
            vkGetSwapchainImagesKHR(mDevice, mSwapchain, &imageCount, nullptr);

            mSwapchainImages.resize(static_cast<size_t>(imageCount));
            vkGetSwapchainImagesKHR(mDevice, mSwapchain, &imageCount, mSwapchainImages.data());
        }

        void createImageViews() {
            mSwapchainImageViews.reserve(mSwapchainImages.size());

            std::transform(mSwapchainImages.begin(), mSwapchainImages.end(), std::back_inserter(mSwapchainImageViews), [this](VkImage image) {
                VkImageViewCreateInfo info {};

                info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                info.image = image;
                info.viewType = VK_IMAGE_VIEW_TYPE_2D;
                info.format = mSwapchainImageFormat;
                info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
                info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                info.subresourceRange.baseMipLevel = 0;
                info.subresourceRange.levelCount = 1;
                info.subresourceRange.baseArrayLayer = 0;
                info.subresourceRange.layerCount = 1;

                VkImageView imageView;

                if (vkCreateImageView(mDevice, &info, nullptr, &imageView) != VK_SUCCESS) {
                    throw std::runtime_error("Failed to create image views from the swapchain images");
                }

                return imageView;
            });
        }

        void createGraphicsPipeline() {
            const auto vertexShaderCode = readFile("media/shaders/triangle/vert.spv");
            const auto fragmentShaderCode = readFile("media/shaders/triangle/frag.spv");

            const VkShaderModule vertexShaderModule = createShaderModule(vertexShaderCode);
            const VkShaderModule fragmentShaderModule = createShaderModule(fragmentShaderCode);

            VkPipelineShaderStageCreateInfo vsStageInfo = {};
            vsStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vsStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vsStageInfo.module = vertexShaderModule;
            vsStageInfo.pName = "main";

            VkPipelineShaderStageCreateInfo fsStageInfo = {};
            fsStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fsStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            fsStageInfo.module = fragmentShaderModule;
            fsStageInfo.pName = "main";

            VkPipelineShaderStageCreateInfo shaderStages[] = {
                vsStageInfo, fsStageInfo
            };

            VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
            vertexInputInfo.pVertexAttributeDescriptions = nullptr;
            vertexInputInfo.vertexBindingDescriptionCount = 0;
            vertexInputInfo.pVertexBindingDescriptions = nullptr;

            VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            VkViewport viewport {};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = mSwapchainExtent.width;
            viewport.height = mSwapchainExtent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            VkRect2D scissor {};
            scissor.offset = {0, 0};
            scissor.extent = mSwapchainExtent;

            VkPipelineViewportStateCreateInfo viewportInfo {};
            viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportInfo.viewportCount = 1;
            viewportInfo.pViewports = &viewport;
            viewportInfo.scissorCount = 1;
            viewportInfo.pScissors = &scissor;

            VkPipelineRasterizationStateCreateInfo rasterizationInfo {};
            rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizationInfo.depthClampEnable = VK_FALSE;
            rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
            rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizationInfo.lineWidth = 1.0f;
            rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
            rasterizationInfo.depthBiasEnable = VK_FALSE;
            
            vkDestroyShaderModule(mDevice, vertexShaderModule, nullptr);
            vkDestroyShaderModule(mDevice, fragmentShaderModule, nullptr);
        }

        VkShaderModule createShaderModule(const std::vector<char> &code) const {
            VkShaderModuleCreateInfo info {};

            info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            info.pCode = (const uint32_t*) code.data();
            info.codeSize = code.size();
            
            VkShaderModule shaderModule;
            if (vkCreateShaderModule(mDevice, &info, nullptr, &shaderModule) != VK_SUCCESS) {
                throw std::runtime_error("Cannot create shader module from code");
            }

            return shaderModule;
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
        VkSwapchainKHR mSwapchain;
        std::vector<VkImage> mSwapchainImages;
        VkFormat mSwapchainImageFormat;
        VkExtent2D mSwapchainExtent;

        std::vector<VkImageView> mSwapchainImageViews;
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
