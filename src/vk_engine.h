// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vk_descriptors.h>

#include <vk_loader.h>// for loading meshes
#include <camera.h> // for the camera class

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

	// Descriptor Allocator
	DescriptorAllocatorGrowable _frameDescriptors;
};

//Scene data struct
struct GPUSceneData {
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 viewproj;
	glm::vec4 ambientColor;
	glm::vec4 sunlightDirection; // w for sun power
	glm::vec4 sunlightColor;
};

//Push Constants struct
struct ComputePushConstants {
	glm::vec4 data1;
	glm::vec4 data2;
	glm::vec4 data3;
	glm::vec4 data4;
};

//Compute Effect struct
struct ComputeEffect {
	const char* name;

	VkPipeline pipeline;
	VkPipelineLayout layout;

	ComputePushConstants data;
};


//Material system
struct GLTFMetallic_Roughness {
	MaterialPipeline opaquePipeline;
	MaterialPipeline transparentPipeline;

	VkDescriptorSetLayout materialLayout;

	struct MaterialConstants {
		glm::vec4 colorFactors;
		glm::vec4 metal_rough_factors;
		//padding, we need it anyway for uniform buffers
		glm::vec4 extra[14];
	};

	struct MaterialResources {
		AllocatedImage colorImage;
		VkSampler colorSampler;
		AllocatedImage metalRoughImage;
		VkSampler metalRoughSampler;
		VkBuffer dataBuffer;
		uint32_t dataBufferOffset;
	};

	DescriptorWriter writer;

	void build_pipelines(VulkanEngine* engine);
	void clear_resources(VkDevice device);

	MaterialInstance write_material(VkDevice device, MaterialPass pass, const MaterialResources& resources, DescriptorAllocatorGrowable& descriptorAllocator);
};


//Scene graph system
//draw Mesh node
struct MeshNode : public Node {

	std::shared_ptr<MeshAsset> mesh;

	virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
};

struct RenderObject {
	uint32_t indexCount;
	uint32_t firstIndex;
	VkBuffer indexBuffer;

	MaterialInstance* material;

	glm::mat4 transform;
	VkDeviceAddress vertexBufferAddress;
};

struct DrawContext {
	std::vector<RenderObject> OpaqueSurfaces;
};



class VulkanEngine {
public:

	bool _isInitialized{ false };
	int _frameNumber {0};
	bool stop_rendering{ false };
	VkExtent2D _windowExtent{ 1920 , 1080 };

	struct SDL_Window* _window{ nullptr };

	bool resize_requested{ false }; //resize request flag

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
	AllocatedImage _depthImage;
	VkExtent2D _drawExtent; //size of the draw image
	float renderScale = 1.f; //scale of the render resolution compared to the window resolution

	//DescriptorAlloc
	DescriptorAllocatorGrowable globalDescriptorAllocator;

	VkDescriptorSet _drawImageDescriptors;
	VkDescriptorSetLayout _drawImageDescriptorLayout;

	//Pipeline Builder
	VkPipeline _gradientPipeline;
	VkPipelineLayout _gradientPipelineLayout;

	// immediate submit structures
	VkFence _immFence;
	VkCommandBuffer _immCommandBuffer;
	VkCommandPool _immCommandPool;

	//compute effects
	std::vector<ComputeEffect> backgroundEffects;
	int currentBackgroundEffect{ 0 };

	/*triangle pipeline
	VkPipelineLayout _trianglePipelineLayout;
	VkPipeline _trianglePipeline;
	*/

	//mesh pipeline
	VkPipelineLayout _meshPipelineLayout;
	VkPipeline _meshPipeline;
	GPUMeshBuffers rectangle;
	GPUMeshBuffers uploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices); //uploads a mesh to the gpu and returns a struct containing the buffers

	//test meshes
	std::vector<std::shared_ptr<MeshAsset>> testMeshes;

	//scene data
	GPUSceneData sceneData;
	VkDescriptorSetLayout _gpuSceneDataDescriptorLayout;

	//base textures/sampler
	AllocatedImage _whiteImage;
	AllocatedImage _blackImage;
	AllocatedImage _greyImage;
	AllocatedImage _errorCheckerboardImage;

	VkSampler _defaultSamplerLinear;
	VkSampler _defaultSamplerNearest;

	VkDescriptorSetLayout _singleImageDescriptorLayout; // layout for a single image sampler

	//default material
	MaterialInstance defaultData;
	GLTFMetallic_Roughness metalRoughMaterial;

	//Scene graph
	DrawContext mainDrawContext;
	std::unordered_map<std::string, std::shared_ptr<Node>> loadedNodes;

	//camera
	Camera mainCamera;


private:

	//VulkanInitialization
	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();

	//Swapchain commands
	void create_swapchain(uint32_t width, uint32_t height);
	void destroy_swapchain();
	void resize_swapchain();

	//draw function
	void draw_background(VkCommandBuffer cmd);
	void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);
	void draw_geometry(VkCommandBuffer cmd);

	//descriptor func
	void init_descriptors();

	//Pipeline Funcs
	void init_pipelines();
	void init_background_pipelines();
	//void init_triangle_pipeline();
	void init_mesh_pipeline();

	//buffer functions
	AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
	void destroy_buffer(const AllocatedBuffer& buffer);

	//helper functions
	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function); //Immediate submit
	void init_imgui(); //ImGui init
	void init_default_data(); //default resources init

	//textures
	AllocatedImage create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
	AllocatedImage create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
	void destroy_image(const AllocatedImage& img);

	//scene functions
	void update_scene();


};

