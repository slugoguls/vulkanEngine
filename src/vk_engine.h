// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>

//constants
constexpr unsigned int FRAME_OVERLAP = 2;


// Per-frame data
struct FrameData {

	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	//synchronization structures
	VkSemaphore _swapchainSemaphore, _renderSemaphore;
	VkFence _renderFence;
};



class VulkanEngine {
public:

	bool _isInitialized{ false };
	int _frameNumber {0};
	bool stop_rendering{ false };
	VkExtent2D _windowExtent{ 1700 , 900 };

	struct SDL_Window* _window{ nullptr };

	static VulkanEngine& Get();

	//initializes everything in the engine
	void init();

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();

	//run main loop
	void run();


	// Vulkan Initialization
	VkInstance _instance; // vulkan library handle
	VkDebugUtilsMessengerEXT _debug_messenger; // vulkan debug messenger handle
	VkSurfaceKHR _surface; // window surface handle
	VkPhysicalDevice _physicalDevice; // graphics card handle
	VkDevice _device; // logical device handle


	// Vulkan spawchain
	VkSwapchainKHR _swapchain; // swapchain handle
	VkFormat _swapchainImageFormat; // swapchain image format

	std::vector<VkImage> _swapchainImages; // swapchain images handles
	std::vector<VkImageView> _swapchainImageViews; // swapchain image views handles
	VkExtent2D _swapchainExtent; // swapchain image extent


	// Per-frame resources
	FrameData _frames[FRAME_OVERLAP];
	FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; }; // returns the current frame data


	// Vulkan queues
	VkQueue _graphicsQueue; // graphics queue handle
	uint32_t _graphicsQueueFamily; // graphics queue family index

private:

	//VulkanInitialization
	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();

	//Swapchain commands
	void create_swapchain(uint32_t width, uint32_t height);
	void destroy_swapchain();

};
