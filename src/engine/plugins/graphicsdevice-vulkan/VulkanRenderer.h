
#pragma once

#include <xe/Predef.h>
#include <xe/math/Vector.h>

#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#if defined(_MSC_VER)
#pragma warning(pop, 0)
#endif

#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "HostPlatform.h"

struct Vertex {
    XE::Vector2 pos;
    XE::Vector3 color;

    static vk::VertexInputBindingDescription createBindingDescription() {
        vk::VertexInputBindingDescription desc;

        // all the vertex data is packed in just one array.
        desc.binding = 0;

        // size, in bytes, of each vertex
        desc.stride = sizeof(Vertex);

        // no instancing
        desc.inputRate = vk::VertexInputRate::eVertex;

        return desc;
    }
};

struct QueryFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }

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

struct SwapchainDetail {
    vk::SurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<vk::SurfaceFormatKHR> surfaceFormats;
    std::vector<vk::PresentModeKHR> surfacePresentModes;
};

class HostPlatform;
class VulkanRenderer {
public:
    explicit VulkanRenderer(HostPlatform &platform) : mPlatform(platform) {}

    void initialize() {
        std::vector<const char *> extensions = mPlatform.enumerateRequiredInstanceExtensions();

        mInstance = createInstance(extensions, mPlatform.enumerateValidationLayers());
        assert(mInstance);

        mDebugMessenger = createDebugMessenger(mInstance);

        mSurface = mPlatform.createSurface(mInstance);
        assert(mSurface);

        mPhysicalDevice = pickPhysicalDevice(mInstance.enumeratePhysicalDevices());
        mFamilies = identifyQueueFamilies(mPhysicalDevice, mSurface);
        assert(mFamilies.isComplete());

        mDevice = createDevice(mPhysicalDevice, mFamilies);
        assert(mDevice);

        mDevice.getQueue(mFamilies.graphicsFamily.value(), 0, &mGraphicsQueue);
        assert(mGraphicsQueue);

        mDevice.getQueue(mFamilies.presentFamily.value(), 0, &mPresentationQueue);
        assert(mPresentationQueue);

        /*Swapchain initialization section*/
        const SwapchainDetail swapchainDetail = querySwapchainDetail(mPhysicalDevice, mSurface);
        const uint32_t imageCount = chooseImageCount(swapchainDetail.surfaceCapabilities);

        const std::optional<vk::SurfaceFormatKHR> surfaceFormat = pickSwapchainSurfaceFormat(swapchainDetail.surfaceFormats);
        assert(surfaceFormat);

        mSwapchainFormat = surfaceFormat.value();
        mSwapchainExtent = mPlatform.pickSwapExtent(swapchainDetail.surfaceCapabilities);

        mSwapchain = createSwapchain(mDevice, mSurface, mSwapchainFormat, mSwapchainExtent, pickPresentMode(swapchainDetail.surfacePresentModes), imageCount,
                                     swapchainDetail.surfaceCapabilities.currentTransform, mFamilies);
        assert(mSwapchain);

        mSwapchainImages = mDevice.getSwapchainImagesKHR(mSwapchain);
        mSwapchainImageViews = createSwapchainImageViews(mDevice, mSwapchainFormat, mSwapchainImages);

        mPipelineLayout = createPipelineLayout(mDevice);
        mRenderPass = createRenderPass(mDevice, mSwapchainFormat.format);
        mGraphicsPipeline = createGraphicsPipeline(mDevice, mSwapchainExtent, mPipelineLayout, mRenderPass);

        for (const vk::ImageView &imageView : mSwapchainImageViews) {
            mSwapchainFramebuffers.push_back(createFramebuffer(mDevice, imageView, mRenderPass, mSwapchainExtent));
        }

        mCommandPool = createCommandPool(mDevice, mFamilies.graphicsFamily.value());
        mCommandBuffer = allocateCommandBuffer(mDevice, mCommandPool);

        mImageAvailableSemaphore = mDevice.createSemaphore({});
        mRenderFinishedSemaphore = mDevice.createSemaphore({});
        mInFlightFence = createFence(mDevice);
    }

    void terminate() {}

    void renderLoop() {
        while (!mPlatform.closeWasRequested()) {
            mPlatform.pollEvents();
            drawFrame();
        }
    }

private:
    HostPlatform &mPlatform;
    vk::Instance mInstance;
    vk::DebugUtilsMessengerEXT mDebugMessenger;
    vk::PhysicalDevice mPhysicalDevice;
    vk::SurfaceKHR mSurface;
    vk::Device mDevice;
    QueryFamilyIndices mFamilies;
    vk::Queue mGraphicsQueue;
    vk::Queue mPresentationQueue;
    vk::SwapchainKHR mSwapchain;
    std::vector<vk::Image> mSwapchainImages;
    std::vector<vk::ImageView> mSwapchainImageViews;
    vk::Extent2D mSwapchainExtent;
    vk::SurfaceFormatKHR mSwapchainFormat;
    vk::RenderPass mRenderPass;
    vk::PipelineLayout mPipelineLayout;
    vk::Pipeline mGraphicsPipeline;
    std::vector<vk::Framebuffer> mSwapchainFramebuffers;

    vk::CommandPool mCommandPool;
    vk::CommandBuffer mCommandBuffer;

    //! signals when an image is available from the swapchain
    vk::Semaphore mImageAvailableSemaphore;

    //! signals when an image is ready for presentation (rendering process finished)
    vk::Semaphore mRenderFinishedSemaphore;

    //! pending
    vk::Fence mInFlightFence;

private:
    vk::ApplicationInfo createAppInfo() const {
        // Identifica a la aplicacion / motor en uso, para que pueda ser (potencialmente)
        // considerado por el implementador de Hardware (nVidia, Intel, etc), para aplicar
        // ciertas optimizaciones a nivel del Driver.
        vk::ApplicationInfo appInfo;
        appInfo.setPApplicationName("VulkanTestApp");
        appInfo.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));
        appInfo.setPEngineName("XE");
        appInfo.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));

        // La version de la API de vulkan a usar.
        // Se deberia detectar previamente la maxima version de vulkan soportada por el driver.
        appInfo.apiVersion = VK_API_VERSION_1_0;

        return appInfo;
    }

    std::vector<const char *> getRequiredExtensions() const {
        uint32_t extensionCount;
        const char **extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

        std::vector<const char *> result = {extensions, extensions + extensionCount};

        result.push_back("VK_KHR_get_physical_device_properties2");

        return result;
    }

    vk::DebugUtilsMessengerCreateInfoEXT createDebugMessengerInfo() const {
        vk::DebugUtilsMessengerCreateInfoEXT msgInfo;

        msgInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                   vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo);

        msgInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                               vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

        msgInfo.pfnUserCallback = debugCallback;
        msgInfo.pUserData = nullptr;

        return msgInfo;
    }

    vk::DebugUtilsMessengerEXT createDebugMessenger(vk::Instance instance) const {
        const auto debugInfo = static_cast<VkDebugUtilsMessengerCreateInfoEXT>(createDebugMessengerInfo());

        VkDebugUtilsMessengerEXT messenger;
        const VkResult result = CreateDebugUtilsMessengerEXT(static_cast<VkInstance>(instance), &debugInfo, nullptr, &messenger);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to setup the debug messenger");
        }

        return vk::DebugUtilsMessengerEXT{messenger};
    }

    vk::Instance createInstance(const std::vector<const char *> &extensions, const std::vector<const char *> &validationLayers) const {
        const vk::ApplicationInfo appInfo = createAppInfo();

        // crear la instancia con una seria de extensiones / capas de validacion requeridas
        vk::InstanceCreateInfo info;
        info.setPApplicationInfo(&appInfo);
        // establecer que requerimos layers referentes a la validacion del uso de la API
        info.setPEnabledLayerNames(validationLayers);
        // establecer las extensiones requeridas por GLFW
        info.setPEnabledExtensionNames(extensions);

        return vk::createInstance(info);
    }

    vk::PhysicalDevice pickPhysicalDevice(const std::vector<vk::PhysicalDevice> &devices) {
        for (const auto &device : devices) {
            showPhysicalDeviceInformation(device);
        }

        return devices[0];
    }

    void showPhysicalDeviceInformation(const vk::PhysicalDevice &device) {
        const auto properties = device.getProperties();

        std::cout << "\"" << properties.deviceName << "\"" << std::endl;
        std::cout << "  deviceID: "
                  << "\"" << properties.deviceID << "\"" << std::endl;
        std::cout << "  deviceName: "
                  << "\"" << properties.deviceName << "\"" << std::endl;
        std::cout << "  vendorID: "
                  << "\"" << properties.vendorID << "\"" << std::endl;
        std::cout << "  apiVersion: "
                  << "\"" << properties.apiVersion << "\"" << std::endl;
        std::cout << "  deviceType: "
                  << "\"" << properties.deviceType << "\"" << std::endl;
        std::cout << "  driverVersion: "
                  << "\"" << properties.driverVersion << "\"" << std::endl;
    }

    std::vector<vk::DeviceQueueCreateInfo> mapQueueCreateInfo(const QueryFamilyIndices &indices) const {
        const float priority = 1.0f;

        std::vector<vk::DeviceQueueCreateInfo> infos;

        for (const uint32_t familyIndex : indices.uniques()) {
            vk::DeviceQueueCreateInfo info;

            info.queueFamilyIndex = familyIndex;
            info.setPQueuePriorities(&priority);
            info.setQueueCount(1);

            infos.push_back(info);
        }

        return infos;
    }

    vk::Device createDevice(const vk::PhysicalDevice &physicalDevice, const QueryFamilyIndices &indices) const {
        const std::vector<vk::DeviceQueueCreateInfo> queueInfos = mapQueueCreateInfo(indices);

        vk::DeviceCreateInfo info;

        // set the required queues (graphics and presentation, for now).
        info.setPQueueCreateInfos(queueInfos.data());
        info.setQueueCreateInfoCount(static_cast<uint32_t>(queueInfos.size()));

        // set the required device extensions
        const auto deviceExtensions = mPlatform.enumerateRequiredDeviceExtensions();

        if (deviceExtensions.size() > 0) {
            info.setPEnabledExtensionNames(deviceExtensions);
            info.setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensions.size()));
        }

        // set the validation layers
        auto validationLayers = mPlatform.enumerateValidationLayers();

        if (validationLayers.size() > 0) {
            info.setPEnabledLayerNames(validationLayers);
            info.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
        }

        return physicalDevice.createDevice(info);
    }

    QueryFamilyIndices identifyQueueFamilies(const vk::PhysicalDevice &physicalDevice, const vk::SurfaceKHR &surface) const {
        QueryFamilyIndices indices;

        const auto queueProperties = physicalDevice.getQueueFamilyProperties();

        for (uint32_t i = 0; i < static_cast<uint32_t>(queueProperties.size()); i++) {
            const auto &props = queueProperties[i];

            // grab the family index for graphics
            if (props.queueFlags & vk::QueueFlagBits::eGraphics) {
                indices.graphicsFamily = i;
            }

            // check if the current queue has presentation capabilities
            vk::Bool32 presentationSuport = VK_FALSE;
            const vk::Result result = physicalDevice.getSurfaceSupportKHR(i, surface, &presentationSuport);

            if (result == vk::Result::eSuccess && presentationSuport == VK_TRUE) {
                indices.presentFamily = i;
            }
        }

        return indices;
    }

    vk::SwapchainKHR createSwapchain(const vk::Device &device, const vk::SurfaceKHR &surface, const vk::SurfaceFormatKHR &swapchainFormat, const vk::Extent2D &swapchainExtent,
                                     const vk::PresentModeKHR presentMode, const uint32_t imageCount, const vk::SurfaceTransformFlagBitsKHR preTransform,
                                     const QueryFamilyIndices &indices) const {
        vk::SwapchainCreateInfoKHR info = {};
        info.surface = surface;
        info.minImageCount = imageCount;
        info.imageFormat = swapchainFormat.format;
        info.imageColorSpace = swapchainFormat.colorSpace;
        info.imageExtent = swapchainExtent;
        info.imageArrayLayers = 1;
        info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

        if (indices.graphicsFamily != indices.presentFamily) {
            const uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

            info.imageSharingMode = vk::SharingMode::eConcurrent;
            info.queueFamilyIndexCount = 2;
            info.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            info.imageSharingMode = vk::SharingMode::eExclusive;
            info.queueFamilyIndexCount = 0;
            info.pQueueFamilyIndices = nullptr;
        }

        info.preTransform = preTransform;
        info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        info.presentMode = presentMode;
        info.oldSwapchain = nullptr;

        return device.createSwapchainKHR(info);
    }

    SwapchainDetail querySwapchainDetail(const vk::PhysicalDevice &physicalDevice, const vk::SurfaceKHR &surface) const {
        SwapchainDetail detail;

        detail.surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
        detail.surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);
        detail.surfacePresentModes = physicalDevice.getSurfacePresentModesKHR(surface);

        return detail;
    }

    //! picks an required surface format
    std::optional<vk::SurfaceFormatKHR> pickSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &surfaceFormats) const {
        for (const vk::SurfaceFormatKHR &surfaceFormat : surfaceFormats) {
            if (surfaceFormat.format == vk::Format::eB8G8R8A8Srgb && surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                return surfaceFormat;
            }
        }

        return {};
    }

    vk::PresentModeKHR pickPresentMode(const std::vector<vk::PresentModeKHR> &presentModes) const {
        const auto it = std::find(presentModes.begin(), presentModes.end(), vk::PresentModeKHR::eMailbox);

        if (it != presentModes.end()) {
            return vk::PresentModeKHR::eMailbox;
        }

        return vk::PresentModeKHR::eFifo;
    }

    uint32_t chooseImageCount(const vk::SurfaceCapabilitiesKHR &caps) const {
        const uint32_t imageCount = caps.minImageCount + 1;

        if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount) {
            return caps.maxImageCount;
        }

        return imageCount;
    }

    std::vector<vk::ImageView> createSwapchainImageViews(const vk::Device &device, const vk::SurfaceFormatKHR &swapchainFormat,
                                                         const std::vector<vk::Image> &swapchainImages) const {
        std::vector<vk::ImageView> imageViews;

        imageViews.reserve(swapchainImages.size());

        for (const vk::Image &image : swapchainImages) {
            vk::ImageViewCreateInfo info = {};

            info.image = image;
            info.viewType = vk::ImageViewType::e2D;
            info.format = swapchainFormat.format;
            info.components.r = vk::ComponentSwizzle::eIdentity;
            info.components.g = vk::ComponentSwizzle::eIdentity;
            info.components.b = vk::ComponentSwizzle::eIdentity;
            info.components.a = vk::ComponentSwizzle::eIdentity;
            info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            info.subresourceRange.baseMipLevel = 0;
            info.subresourceRange.levelCount = 1;
            info.subresourceRange.baseArrayLayer = 0;
            info.subresourceRange.layerCount = 1;

            imageViews.push_back(device.createImageView(info));
        }

        return imageViews;
    }

    std::vector<char> loadBinaryFile(const std::string &filename) const {
        const std::string path = XE_MEDIA_PATH + filename;
        return readFile(path);
    }

    vk::Pipeline createGraphicsPipeline(vk::Device &device, const vk::Extent2D &swapchainExtent, const vk::PipelineLayout &pipelineLayout, const vk::RenderPass &renderPass) const {
        vk::ShaderModule vertModule = createShaderModule(device, loadBinaryFile("/shaders/triangle/vert.spv"));
        vk::ShaderModule fragModule = createShaderModule(device, loadBinaryFile("/shaders/triangle/frag.spv"));

        vk::PipelineShaderStageCreateInfo vertexShaderStageInfo;
        vertexShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
        vertexShaderStageInfo.module = vertModule;
        vertexShaderStageInfo.pName = "main";

        vk::PipelineShaderStageCreateInfo fragmentShaderStageInfo;
        fragmentShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
        fragmentShaderStageInfo.module = fragModule;
        fragmentShaderStageInfo.pName = "main";

        // configured shader stages
        const std::vector<vk::PipelineShaderStageCreateInfo> shaderStages{vertexShaderStageInfo, fragmentShaderStageInfo};

        // vertex binding description
        vk::VertexInputBindingDescription vertexBindingDesc = Vertex::createBindingDescription();

        // vertex attribute description
        std::array<vk::VertexInputAttributeDescription, 2> vertexAttributeDescs;

        // vertex position
        vertexAttributeDescs[0].binding = 0;
        vertexAttributeDescs[0].location = 0;
        vertexAttributeDescs[0].format = vk::Format::eR32G32Sfloat;
        vertexAttributeDescs[0].offset = offsetof(Vertex, pos);

        // vertex color
        vertexAttributeDescs[1].binding = 0;
        vertexAttributeDescs[1].location = 1;
        vertexAttributeDescs[1].format = vk::Format::eR32G32B32Sfloat;
        vertexAttributeDescs[1].offset = offsetof(Vertex, color);

        // vertex layout
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDesc;

        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescs.size());
        vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescs.data();

        // primitive used to draw
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        // viewport specification
        // region of the framebuffer
        vk::Viewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchainExtent.width);
        viewport.height = static_cast<float>(swapchainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vk::Rect2D scissor;
        scissor.offset = vk::Offset2D{0, 0};
        scissor.extent = swapchainExtent;

        vk::PipelineViewportStateCreateInfo viewportStateInfo;
        viewportStateInfo.viewportCount = 1;
        viewportStateInfo.pViewports = &viewport;
        viewportStateInfo.scissorCount = 1;
        viewportStateInfo.pScissors = &scissor;

        // rasterization state
        // equivalent to RenderState in Direct3D 7-ish
        vk::PipelineRasterizationStateCreateInfo rasterizationStateInfo;
        rasterizationStateInfo.depthClampEnable = VK_FALSE;
        rasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateInfo.polygonMode = vk::PolygonMode::eFill;
        rasterizationStateInfo.lineWidth = 1.0f;
        rasterizationStateInfo.cullMode = vk::CullModeFlagBits::eBack;
        rasterizationStateInfo.frontFace = vk::FrontFace::eClockwise;
        rasterizationStateInfo.depthBiasEnable = VK_FALSE;

        // multisampling
        vk::PipelineMultisampleStateCreateInfo multisampleStateInfo;
        multisampleStateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;

        // fragment color blending parameters
        // describes how to combine the fragment's final color with its previous color
        vk::PipelineColorBlendAttachmentState colorBlendAttachment;
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_FALSE;

        vk::PipelineColorBlendStateCreateInfo colorBlendStateInfo;
        colorBlendStateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateInfo.attachmentCount = 1;
        colorBlendStateInfo.pAttachments = &colorBlendAttachment;

        // dynamic state specification
        //        const std::vector<vk::DynamicState> dynamicStates = {
        //            vk::DynamicState::eViewport, vk::DynamicState::eLineWidth
        //        };
        //
        //        vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
        //        dynamicStateInfo.dynamicStateCount = dynamicStates.size();
        //        dynamicStateInfo.pDynamicStates = dynamicStates.data();

        vk::GraphicsPipelineCreateInfo pipelineInfo;

        // reference all the description states, created earlier in this method

        // shader stages used
        pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
        pipelineInfo.pStages = shaderStages.data();

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
        pipelineInfo.pViewportState = &viewportStateInfo;
        pipelineInfo.pRasterizationState = &rasterizationStateInfo;
        pipelineInfo.pMultisampleState = &multisampleStateInfo;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &colorBlendStateInfo;
        pipelineInfo.pDynamicState = nullptr;

        pipelineInfo.layout = pipelineLayout;

        // the render pass that this pipeline will use
        pipelineInfo.renderPass = renderPass;

        // the index of the sub pass within the sub pass that will be used
        pipelineInfo.subpass = 0;

        auto pipelineResult = device.createGraphicsPipeline({}, pipelineInfo);

        if (pipelineResult.result != vk::Result::eSuccess) {
            throw std::runtime_error("Can't create a GraphicsPipeline with those parameters.");
        }

        return pipelineResult.value;
    }

    vk::ShaderModule createShaderModule(const vk::Device &device, const std::vector<char> &shaderCode) const {
        vk::ShaderModuleCreateInfo info;

        info.codeSize = shaderCode.size();
        info.pCode = reinterpret_cast<const uint32_t *>(shaderCode.data());

        return device.createShaderModule(info);
    }

    vk::PipelineLayout createPipelineLayout(const vk::Device &device) const {
        // pipeline layout
        // used for passing values to the uniforms found in the current shader program
        vk::PipelineLayoutCreateInfo info;
        info.setLayoutCount = 0;

        return device.createPipelineLayout(info);
    }

    vk::RenderPass createRenderPass(const vk::Device &device, const vk::Format &swapchainFormat) const {
        // Color Buffer Attachment
        // its represented from by one of the images in the Swapchain
        vk::AttachmentDescription colorAttachment;

        // format should match the format of the swapchain images
        colorAttachment.format = swapchainFormat;

        // no multisampling.
        colorAttachment.samples = vk::SampleCountFlagBits::e1;

        // what to do before rendering
        // in this case, clear the color buffer
        // apply for color and depth data
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;

        // what to do after rendering
        // in this case, rendered content will be stored in memory
        // apply for color and depth data
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;

        // what to do before and after rendering, this time, for the stencil buffer.
        // dont-care for now.
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

        // what change to make with respect to the layout of the pixels in memory
        // describes wich layout the image will have before the render pass begins
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;

        // specified wich layout the image will have after the render pass finish.
        // present with the swapchain.
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        // subpass specification

        // Attachment reference
        // specifies references to use within a subpass
        vk::AttachmentReference colorAttachmentRef;

        // attachment index from the attachment description array
        colorAttachmentRef.attachment = 0;

        // we will use the attachment as a Color Buffer
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        // describes a render subpass
        vk::SubpassDescription subpass;

        // this is a graphics subpass
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        // subpass dependency
        // controls how the transitions of the image layouts are made.
        vk::SubpassDependency dependency;

        // refers to te implicit subpass dependency
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;

        // refers to our only subpass
        // dstSubpass must always be greater than srcSubpass to prevent cycles in the dependency graph
        dependency.dstSubpass = 0;

        // specify that we need to wait for the swapchain from the image, before we can make use of.
        // this is implicit (in this case), as this is the result of the color attachment output stage itself.
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

        // the color attachment output stage must wait on this
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

        // render pass creation
        // use the attachment and the sub pass specification
        // subpasses uses internally the attachments specified here, via the attachment reference, specified earlier.
        vk::RenderPassCreateInfo renderPassInfo;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        return device.createRenderPass(renderPassInfo);
    }

    vk::Framebuffer createFramebuffer(const vk::Device &device, const vk::ImageView &imageView, const vk::RenderPass &renderPass, const vk::Extent2D &swapchainExtent) const {
        vk::FramebufferCreateInfo info;

        // render pass to be compatible with
        info.renderPass = renderPass;

        // image views to bind with
        info.attachmentCount = 1;
        info.pAttachments = &imageView;

        // size of the framebuffer
        info.width = swapchainExtent.width;
        info.height = swapchainExtent.height;

        // our image views from the swapchain are single-layered, so we put 1 one
        info.layers = 1;

        return device.createFramebuffer(info);
    }

    vk::CommandPool createCommandPool(const vk::Device &device, const uint32_t queueFamily) const {
        // a commandPool is an Object that records different types of commands (like rendering and memory transfer),
        // to be executed by the GPU.

        vk::CommandPoolCreateInfo info;

        // specifies we will be recretating the command pool in every render frame
        info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

        // put this command pool in the specified Queue Family
        info.queueFamilyIndex = queueFamily;

        return device.createCommandPool(info);
    }

    vk::CommandBuffer allocateCommandBuffer(const vk::Device &device, const vk::CommandPool &commandPool) const {
        vk::CommandBufferAllocateInfo info;

        info.commandPool = commandPool;
        // a primary buffer is a buffer that can be submitted to a queue for execution directly, but can't be called from another buffers
        // a secondary buffer is a buffer that can't be submitted to a queue for execution, but can be called from primary buffers
        info.level = vk::CommandBufferLevel::ePrimary;

        // the number of buffers to allocate
        info.commandBufferCount = 1;

        vk::CommandBuffer commandBuffer;

        const vk::Result result = device.allocateCommandBuffers(&info, &commandBuffer);

        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("Can't allocate the CommandBuffer with the supplied parameters.");
        }

        return commandBuffer;
    }

    vk::RenderPassBeginInfo createRenderPassBeginInfo(const vk::Framebuffer &framebuffer, const vk::ClearValue &clearValue, const vk::RenderPass &renderPass,
                                                      const vk::Extent2D &swapchainExtent) const {

        // render pass
        vk::RenderPassBeginInfo info;

        info.renderPass = renderPass;
        info.framebuffer = framebuffer;
        info.renderArea.offset = vk::Offset2D{0, 0};
        info.renderArea.extent = swapchainExtent;
        info.clearValueCount = 1;
        info.pClearValues = &clearValue;

        return info;
    }

    void recordCommandBuffer(const vk::CommandBuffer &commandBuffer, const vk::Framebuffer &framebuffer, const vk::RenderPass &renderPass, const vk::Extent2D &swapchainExtent,
                             const vk::Pipeline &graphicsPipeline) const {

        vk::ClearValue clearColor;
        clearColor.color.setFloat32({0.2f, 0.2f, 0.2f, 1.0f});

        vk::CommandBufferBeginInfo beginInfo;

        // implicitlt, resets the command buffer
        commandBuffer.begin(beginInfo);
        commandBuffer.beginRenderPass(createRenderPassBeginInfo(framebuffer, clearColor, renderPass, swapchainExtent), vk::SubpassContents::eInline);
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
        commandBuffer.draw(3, 1, 0, 0);
        commandBuffer.endRenderPass();
        commandBuffer.end();
    }

    vk::Fence createFence(const vk::Device &device) const {
        vk::FenceCreateInfo info;

        info.flags = vk::FenceCreateFlagBits::eSignaled;

        return device.createFence(info);
    }

    void initializeVertexBuffer() {
        
    }

    void drawFrame() const {
        // first, wait for the previous frame to render
        const auto waitResult = mDevice.waitForFences(1, &mInFlightFence, VK_TRUE, UINT64_MAX);

        if (waitResult != vk::Result::eSuccess) {
            vk::detail::throwResultException(waitResult, "Fence Wait operation failed.");
        }

        // reset the frame-rendering fence
        const auto resetResult = mDevice.resetFences(1, &mInFlightFence);

        if (resetResult != vk::Result::eSuccess) {
            vk::detail::throwResultException(resetResult, "Fence Reset operation failed.");
        }

        // image index, in the mSwapchainImage member array
        uint32_t imageIndex;

        // acquire an image from the swapchain
        const auto acquireResult = mDevice.acquireNextImageKHR(mSwapchain, UINT64_MAX, mImageAvailableSemaphore, nullptr, &imageIndex);
        if (acquireResult != vk::Result::eSuccess) {
            vk::detail::throwResultException(acquireResult, "Acquire image from the swapchain operation failed.");
        }

        assert(imageIndex < mSwapchainFramebuffers.size());

        // record the necesary commands render the frame
        mCommandBuffer.reset();
        recordCommandBuffer(mCommandBuffer, mSwapchainFramebuffers[imageIndex], mRenderPass, mSwapchainExtent, mGraphicsPipeline);

        // submit the command buffer to the graphics pipeline queue
        const vk::Semaphore waitSemaphores[] = {mImageAvailableSemaphore};
        const vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        const vk::Semaphore signalSemaphores[] = {mRenderFinishedSemaphore};

        vk::SubmitInfo submitInfo;

        // configure the semaphores to use for waiting the execution in specific
        // pipeline stages (color attachment output, in this case), begins.
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        // the commands to be submitted
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &mCommandBuffer;

        // the semaphores to signal when the command finish execution
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        const auto resultSubmit = mGraphicsQueue.submit(1, &submitInfo, mInFlightFence);
        if (resultSubmit != vk::Result::eSuccess) {
            vk::detail::throwResultException(resetResult, "Graphics queue submit operation failed.");
        }

        // presentation info
        vk::PresentInfoKHR presentInfo;

        // wich semaphores wait on, before presentation can occur
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        // specify wich swapchain use to present images to.
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &mSwapchain;

        // the index of the image, for each swapchain used.
        presentInfo.pImageIndices = &imageIndex;

        // results to use
        presentInfo.pResults = nullptr;

        const vk::Result presentResult = mPresentationQueue.presentKHR(presentInfo);
        if (presentResult != vk::Result::eSuccess) {
            vk::detail::throwResultException(resetResult, "Graphics queue submit operation failed.");
        }
    }
};
