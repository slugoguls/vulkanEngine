
# Add source to this project's executable.
add_executable (engine 
  "source/main.cpp"
  "include/vk_types.h"
  "source/vk_initializers.cpp"
  "include/vk_initializers.h"
  "include/vk_images.h"
  "source/vk_images.cpp" 
  "include/vk_descriptors.h"
  "source/vk_descriptors.cpp"
  "include/vk_pipelines.h"
  "source/vk_pipelines.cpp"
  "include/vk_engine.h"
  "source/vk_engine.cpp"
  "include/vk_loader.h"
  "source/vk_loader.cpp"
  "source/camera.cpp"
  "include/camera.h"
)

set_property(TARGET engine PROPERTY CXX_STANDARD 20)
target_compile_definitions(engine PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_include_directories(engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(engine PUBLIC vma glm Vulkan::Vulkan fmt::fmt stb_image SDL2::SDL2 vkbootstrap imgui fastgltf::fastgltf)

target_precompile_headers(engine PUBLIC <optional> <vector> <memory> <string> <vector> <unordered_map> <glm/mat4x4.hpp>  <glm/vec4.hpp> <vulkan/vulkan.h>)

if(WIN32)
  add_custom_command(TARGET engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:engine> $<TARGET_FILE_DIR:engine>
    COMMAND_EXPAND_LISTS
    )
endif()