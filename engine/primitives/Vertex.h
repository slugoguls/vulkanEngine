struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDesc{};
        bindingDesc.binding = 0;
        bindingDesc.stride = sizeof(Vertex);
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDesc;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attr{};
        attr[0].binding = 0; attr[0].location = 0; attr[0].format = VK_FORMAT_R32G32B32_SFLOAT; attr[0].offset = offsetof(Vertex, position);
        attr[1].binding = 0; attr[1].location = 1; attr[1].format = VK_FORMAT_R32G32B32_SFLOAT; attr[1].offset = offsetof(Vertex, normal);
        attr[2].binding = 0; attr[2].location = 2; attr[2].format = VK_FORMAT_R32G32_SFLOAT;    attr[2].offset = offsetof(Vertex, uv);
        return attr;
    }
};
