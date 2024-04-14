
#include "VulkanRenderer.h"


const std::vector<Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

static std::ostream &operator<<(std::ostream &os, const vk::PhysicalDeviceType deviceType) {
    switch (deviceType) {
    case vk::PhysicalDeviceType::eCpu:
        os << "vk::PhysicalDeviceType::eCpu";
        break;

    case vk::PhysicalDeviceType::eDiscreteGpu:
        os << "vk::PhysicalDeviceType::eDiscreteGpu";
        break;

    case vk::PhysicalDeviceType::eIntegratedGpu:
        os << "vk::PhysicalDeviceType::eIntegratedGpu";
        break;

    case vk::PhysicalDeviceType::eVirtualGpu:
        os << "vk::PhysicalDeviceType::eVirtualGpu";
        break;

    case vk::PhysicalDeviceType::eOther:
        os << "vk::PhysicalDeviceType::eOther";
        break;
    default:
        os << "<Unknown>:" << (int)deviceType;
    }

    return os;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT /*severity*/, VkDebugUtilsMessageTypeFlagsEXT /*type*/,
                                                    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void * /*pUserData*/) {

    std::cout << pCallbackData->pMessage << std::endl << std::endl;

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                                      VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

std::vector<char> readFile(const std::string &filename) {
    std::ifstream fs{filename.c_str(), std::ios::ate | std::ios::binary};

    if (!fs.is_open()) {
        std::cerr << "failed to open file " << filename << std::endl;
        throw std::exception();
    }

    const auto fileSize = fs.tellg();
    assert(fileSize >= 0);

    std::vector<char> buffer;
    buffer.resize(static_cast<size_t>(fileSize));

    fs.seekg(0);
    fs.read(buffer.data(), fileSize);
    fs.close();

    return buffer;
}

