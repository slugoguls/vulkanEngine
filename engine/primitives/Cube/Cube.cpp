#include "Cube.h"
#include <stdexcept>
#include <cstring>
#include "../VulkanHelpers.h"

// Helper: create a Vulkan buffer
void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
    VkDeviceSize size, VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer, VkDeviceMemory& bufferMemory);

// ========================
// Cube Implementation
// ========================
Cube::Cube(VkDevice device, VkPhysicalDevice physicalDevice, float size)
    : device(device) {

    float h = size / 2.0f;

    std::vector<CVertex> vertices = {
        {{-h, -h,  h}, {0, 0, 1}, {0.0f, 0.0f}},
        {{ h, -h,  h}, {0, 0, 1}, {1.0f, 0.0f}},
        {{ h,  h,  h}, {0, 0, 1}, {1.0f, 1.0f}},
        {{-h,  h,  h}, {0, 0, 1}, {0.0f, 1.0f}},
        {{-h, -h, -h}, {0, 0, -1}, {1.0f, 0.0f}},
        {{-h,  h, -h}, {0, 0, -1}, {1.0f, 1.0f}},
        {{ h,  h, -h}, {0, 0, -1}, {0.0f, 1.0f}},
        {{ h, -h, -h}, {0, 0, -1}, {0.0f, 0.0f}},
        // ... left, right, top, bottom like earlier
    };

    std::vector<uint32_t> indices = {
        0,1,2, 2,3,0,     // front
        4,5,6, 6,7,4,     // back
        8,9,10, 10,11,8,  // left
        12,13,14, 14,15,12, // right
        16,17,18, 18,19,16, // top
        20,21,22, 22,23,20  // bottom
    };

    indexCount = static_cast<uint32_t>(indices.size());

    createBuffers(physicalDevice, vertices, indices);
}

Cube::~Cube() {
    destroyBuffers();
}

void Cube::draw(VkCommandBuffer cmd, VkPipelineLayout pipelineLayout, const glm::mat4& modelMatrix) {
    VkBuffer vertexBuffers[] = { vertexBuffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    // push model matrix
    vkCmdPushConstants(cmd, pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT,
        0, sizeof(glm::mat4), &modelMatrix);

    vkCmdDrawIndexed(cmd, indexCount, 1, 0, 0, 0);
}

void Cube::createBuffers(VkPhysicalDevice physicalDevice,
    const std::vector<CVertex>& vertices,
    const std::vector<uint32_t>& indices) {
    VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();
    VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();

    createBuffer(device, physicalDevice, vertexBufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vertexBuffer, vertexMemory);

    createBuffer(device, physicalDevice, indexBufferSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        indexBuffer, indexMemory);

    // Upload data
    void* data;
    vkMapMemory(device, vertexMemory, 0, vertexBufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)vertexBufferSize);
    vkUnmapMemory(device, vertexMemory);

    vkMapMemory(device, indexMemory, 0, indexBufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)indexBufferSize);
    vkUnmapMemory(device, indexMemory);
}

void Cube::destroyBuffers() {
    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkFreeMemory(device, vertexMemory, nullptr);
    vkDestroyBuffer(device, indexBuffer, nullptr);
    vkFreeMemory(device, indexMemory, nullptr);
}
