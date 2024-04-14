
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

enum class HostPlatformFlagBits { None = 0x00, Debug = 0x01 };

class HostPlatform {
public:
    explicit HostPlatform(const std::string &title, const uint32_t screenWidth, const uint32_t screenHeight, const HostPlatformFlagBits flags)
        : mScreenWidth(screenWidth), mScreenHeight(screenHeight), mFlags(flags) {
        assert(screenWidth > 0);
        assert(screenHeight > 0);

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        mWindow = glfwCreateWindow(mScreenWidth, mScreenHeight, title.c_str(), nullptr, nullptr);
    }

    ~HostPlatform() {
        destroyWindow();
        glfwTerminate();
    }

    bool closeWasRequested() const { return glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(mWindow) == GLFW_TRUE; }

    uint32_t getScreenWidth() const { return mScreenWidth; }

    uint32_t getScreenHeight() const { return mScreenHeight; }

    std::vector<const char *> enumerateRequiredInstanceExtensions() const {
        uint32_t extensionCount = 0;
        const char **extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

        std::vector<const char *> result = {extensions, extensions + extensionCount};

        result.push_back("VK_KHR_get_physical_device_properties2");

        if (mFlags == HostPlatformFlagBits::Debug) {
            result.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return result;
    }

    std::vector<const char *> enumerateRequiredDeviceExtensions() const {
        return {
            // this extensions provides all the objects required for swapchains
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,

            // required for platforms where Vulkan isn't supported directly by the OS
#if defined(XE_OS_MACOS) || defined(XE_OS_IOS)
                "VK_KHR_portability_subset"
#endif
        };
    }

    std::vector<const char *> enumerateValidationLayers() const {
        if (mFlags == HostPlatformFlagBits::Debug) {
            return {"VK_LAYER_KHRONOS_validation"};
        }

        return {};
    }

    void pollEvents() { glfwPollEvents(); }

    vk::Extent2D pickSwapExtent(const vk::SurfaceCapabilitiesKHR &surfaceCaps) const {
        assert(mWindow);

        if (surfaceCaps.currentExtent.width != UINT32_MAX) {
            return surfaceCaps.currentExtent;
        }

        int width, height;
        glfwGetFramebufferSize(mWindow, &width, &height);

        const vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        return {
            std::clamp(actualExtent.width, surfaceCaps.minImageExtent.width, surfaceCaps.maxImageExtent.width),
            std::clamp(actualExtent.height, surfaceCaps.minImageExtent.height, surfaceCaps.maxImageExtent.height),
        };
    }

    vk::SurfaceKHR createSurface(vk::Instance &instance) const {
        VkSurfaceKHR rawsurf;

        if (glfwCreateWindowSurface(instance, mWindow, nullptr, &rawsurf) != VK_SUCCESS) {
            return {};
        }

        return rawsurf;
    }

private:
    void destroyWindow() {
        if (mWindow) {
            glfwDestroyWindow(mWindow);
            mWindow = nullptr;
        }
    }

private:
    GLFWwindow *mWindow = nullptr;
    uint32_t mScreenWidth = 1024;
    uint32_t mScreenHeight = 768;
    HostPlatformFlagBits mFlags;
};
