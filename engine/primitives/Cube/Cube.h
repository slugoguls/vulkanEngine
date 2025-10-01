// Cube.h
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Vertex.h"  

class Cube {
public:
    Cube(float size = 1.0f);

    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<uint32_t>& getIndices() const { return indices; }

private:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};
