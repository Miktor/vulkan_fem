#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

#include "model.h"
#include "model_factory.h"
#include "solver.h"
#include "vulcan.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

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

// // class VulkanFem : public vkb::VulkanApplication {
// //  public:
// //   VulkanFem() {}

// //   // static void cursorPosCallback(GLFWwindow* window, double x, double y)
// {
// //   //   if (window == nullptr) return;

// //   //   auto app =
// //   reinterpret_cast<VulkanFem*>(glfwGetWindowUserPointer(window));
// //   //   app->OnMouseMove(x, y);
// //   // }

// //   // static void focusCallback(GLFWwindow* window, int focus) {
// //   //   if (window == nullptr) return;

// //   //   auto app =
// //   reinterpret_cast<VulkanFem*>(glfwGetWindowUserPointer(window));
// //   //   app->OnFocusChange(focus == GLFW_TRUE);
// //   // }

// //   // static void scrollCallback(GLFWwindow*, double, double) {}

// //   // static void keyCallback(GLFWwindow* window, int key, int, int action,
// //   int) {
// //   //   if (action != GLFW_PRESS) return;

// //   //   auto app =
// //   reinterpret_cast<VulkanFem*>(glfwGetWindowUserPointer(window));

// //   //   switch (key) {
// //   //     case GLFW_KEY_R:
// //   //       break;
// //   //     case GLFW_KEY_SPACE:
// //   //       app->solver_->Solve(*app->model_);
// //   //       break;
// //   //   }
// //   // }

// //  protected:
// //    void InitAdditionalResources()  {
// //     solver_ = std::make_shared<vulkan_fem::Solver<2>>();
// //     model_ = vulkan_fem::ModelFactory::CreateRectangle();
// //   }

// //  private:
// //   std::shared_ptr<vulkan_fem::Solver<2>> solver_;
// //   std::shared_ptr<vulkan_fem::Model<2>> model_;
// // };

// // VulkanExample* vulkanExample;
// // static void handleEvent(const DFBWindowEvent* event) {
// //   if (vulkanExample != NULL) {
// //     vulkanExample->handleEvent(event);
// //   }
// // }

int main(const int /*argc*/, const char ** /*argv[]*/) {
  HelloTriangleApplication app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
