#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include "../Vertex.h"

class Cube {
public:
    Cube(VkDevice device, VkPhysicalDevice physicalDevice, float size = 1.0f);
    ~Cube();

    void draw(VkCommandBuffer cmd, VkPipelineLayout pipelineLayout, const glm::mat4& modelMatrix);

private:
    VkDevice device;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexMemory;
    uint32_t indexCount;

    void createBuffers(VkPhysicalDevice physicalDevice,
        const std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices);

    void destroyBuffers();
};
