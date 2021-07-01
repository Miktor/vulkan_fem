
#include "fem_application.h"
#include <cstdlib>
#include <exception>
#include <iostream>

// struct Vertex {
//   glm::vec3 pos;
//   static VkVertexInputBindingDescription getBindingDescription() {
//     VkVertexInputBindingDescription binding_description = {};
//     binding_description.binding = 0;
//     binding_description.stride = sizeof(Vertex);
//     binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//     return binding_description;
//   }

//   static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions() {
//     std::array<VkVertexInputAttributeDescription, 1> attribute_descriptions = {};
//     attribute_descriptions[0].binding = 0;
//     attribute_descriptions[0].location = 0;
//     attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
//     attribute_descriptions[0].offset = offsetof(Vertex, pos);
//     return attribute_descriptions;
//   }
// };
// struct UniformBufferObject {
//   glm::mat4 model;
//   glm::mat4 view;
//   glm::mat4 proj;
// };

int main(const int /*argc*/, const char ** /*argv[]*/) {
  FEMApplication app;

  try {
    app.Run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
