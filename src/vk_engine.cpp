//> includes
#include "vk_engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vk_initializers.h>
#include <vk_types.h>

#include "VkBootstrap.h" // for VkBootstrap, simplifying the init code

#include <chrono>
#include <thread>

constexpr bool bUseValidationLayers{ false };
VulkanEngine* loadedEngine = nullptr;

VulkanEngine& VulkanEngine::Get() { return *loadedEngine; }
void VulkanEngine::init()
{
    // only one engine initialization is allowed with the application.
    assert(loadedEngine == nullptr);
    loadedEngine = this;

    // We initialize SDL and create a window with it.
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

    _window = SDL_CreateWindow(
        "Vulkan Engine",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        _windowExtent.width,
        _windowExtent.height,
        window_flags);

	// Vulkan Initialization
	init_vulkan();
	init_swapchain();
	init_commands();
	init_sync_structures();

    // everything went fine
    _isInitialized = true;

}

void VulkanEngine::cleanup()
{
    if (_isInitialized) {

        SDL_DestroyWindow(_window);
    }

    // clear engine pointer
    loadedEngine = nullptr;
}

void VulkanEngine::draw()
{
    // nothing yet
}

void VulkanEngine::run()
{
    SDL_Event e;
    bool bQuit = false;

    // main loop
    while (!bQuit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // close the window when user alt-f4s or clicks the X button
            if (e.type == SDL_QUIT)
                bQuit = true;

            if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                    stop_rendering = true;
                }
                if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
                    stop_rendering = false;
                }
            }
        }

        // do not draw if we are minimized
        if (stop_rendering) {
            // throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        draw();
    }
}






// Vulkan Initialization functions

void VulkanEngine::init_vulkan()
{
	vkb::InstanceBuilder builder; // from the bootstarap library and abstracts away some of the init code

	auto inst_ret = builder.set_app_name("Vulkan Engine") //name of the application
		.request_validation_layers(bUseValidationLayers) //Enable validation layers if needed
		.use_default_debug_messenger() //Enable debug messenger if needed
		.require_api_version(1, 3, 0) //Require at least Vulkan 1.3
		.build(); // build the instance

	vkb::Instance vkb_inst = inst_ret.value(); //get the instance or crash if it failed

	_instance = vkb_inst.instance; //get the VkInstance handle
	_debug_messenger = vkb_inst.debug_messenger; //get the debug messenger handle

    SDL_Vulkan_CreateSurface(_window, _instance, &_surface); // create the window surface

    //vulkan 1.3 features
    VkPhysicalDeviceVulkan13Features features13{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    features13.synchronization2 = VK_TRUE; // we want to use the synchronization2 feature
    features13.dynamicRendering = VK_TRUE; // we want to use dynamic rendering

    //vulkan 1.2 features
    VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
    features12.descriptorIndexing = VK_TRUE; // we want to use descriptor indexing
    features12.bufferDeviceAddress = VK_TRUE; // we want to use buffer device address

    //use bootstrap to select a GPU
    //We want a gpu that can write to the SDL surface and supports vulkan 1.3 with the correct features
    vkb::PhysicalDeviceSelector selector{ vkb_inst };
    vkb::PhysicalDevice physicalDevice = selector
        .set_minimum_version(1, 3)
        .set_required_features_13(features13)
        .set_required_features_12(features12)
        .set_surface(_surface)
        .select()
        .value();

    //create the final vulkan device
	vkb::DeviceBuilder deviceBuilder{ physicalDevice }; //create the device builder using the selected physical device

	vkb::Device vkbDevice = deviceBuilder.build().value(); //create the device

	//get the vulkan device handles
	_device = vkbDevice.device;
	_physicalDevice = physicalDevice.physical_device;

}
void VulkanEngine::init_swapchain()
{
    //nothing yet
}
void VulkanEngine::init_commands()
{
    //nothing yet
}
void VulkanEngine::init_sync_structures()
{
    //nothing yet
}