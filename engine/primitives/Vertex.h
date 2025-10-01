struct CVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    CVertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex)
        : position(pos), normal(norm), uv(tex) {
    }
};
