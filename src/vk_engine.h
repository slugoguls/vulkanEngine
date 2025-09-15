// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vk_descriptors.h>

//constants
constexpr unsigned int FRAME_OVERLAP = 2;


// Simple deletion queue for vulkan resources
struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void flush() {
		// reverse iterate the deletion queue to execute all the functions
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
			(*it)(); //call functors
		}

		deletors.clear();
	}
};

// Per-frame data
struct FrameData {

	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	//synchronization structures
	VkSemaphore _swapchainSemaphore, _renderSemaphore;
	VkFence _renderFence;

	//Deletion queue
	DeletionQueue _deletionQueue;
};

//Push Constants struct
struct ComputePushConstants {
	glm::vec4 data1;
	glm::vec4 data2;
	glm::vec4 data3;
	glm::vec4 data4;
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

	//Deletion queue
	DeletionQueue _mainDeletionQueue;

	//VMA allocator
	VmaAllocator _allocator;

	//draw resources
	AllocatedImage _drawImage;
	VkExtent2D _drawExtent;

	//DescriptorAlloc
	DescriptorAllocator globalDescriptorAllocator;

	VkDescriptorSet _drawImageDescriptors;
	VkDescriptorSetLayout _drawImageDescriptorLayout;

	//Pipeline Builder
	VkPipeline _gradientPipeline;
	VkPipelineLayout _gradientPipelineLayout;

	// immediate submit structures
	VkFence _immFence;
	VkCommandBuffer _immCommandBuffer;
	VkCommandPool _immCommandPool;
	

private:

	//VulkanInitialization
	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();

	//Swapchain commands
	void create_swapchain(uint32_t width, uint32_t height);
	void destroy_swapchain();

	//draw function
	void draw_background(VkCommandBuffer cmd);
	void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);

	//descriptor func
	void init_descriptors();

	//Pipeline Funcs
	void init_pipelines();
	void init_background_pipelines();

	//Immediate submit
	void init_imgui();
	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

};

