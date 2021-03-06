#pragma once

#include "model_factory.h"
#include "solver.h"
#include "vulcan.h"
#include <memory>

class FEMApplication final : public Application {
 private:
  VkBuffer vertex_buffer_;
  VkDeviceMemory vertex_buffer_memory_;
  VkBuffer index_buffer_;
  VkDeviceMemory index_buffer_memory_;

  std::vector<Vertex> vertices_;
  std::vector<uint16_t> indices_;

  std::shared_ptr<vulkan_fem::Solver<2>> solver_;
  std::shared_ptr<vulkan_fem::Model<2>> model_;

 protected:
  void PreInit() final {
    solver_ = std::make_shared<vulkan_fem::Solver<2>>();
    model_ = vulkan_fem::ModelFactory::CreateRectangle();
  }

  bool ProcessInput(GLFWwindow *window, int key, int scancode, int action, int mods) final {
    if (!Application::ProcessInput(window, key, scancode, action, mods)) {
      return false;
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
      solver_->Solve(*model_);
      return false;
    }

    return true;
  }

  static std::vector<Vertex> ToVertices(const std::vector<vulkan_fem::Vertex3> &data) {
    std::vector<Vertex> result;
    result.reserve(data.size());

    for (const auto &input : data) {
      Vertex vertex{};
      vertex.pos_ = {input.x_, input.y_};
      result.push_back(vertex);
    }

    return result;
  }

  void CrateBuffers() final {
    indices_ = model_->GetIndices();
    vertices_ = ToVertices(model_->GetVertices());

    CreateIndexBuffer(index_buffer_, index_buffer_memory_, indices_);
    CreateVertexBuffer(vertex_buffer_, vertex_buffer_memory_, vertices_);
  }

  void DrawRenderPass(VkCommandBuffer command_buffers) final {
    VkBuffer vertex_buffers[] = {vertex_buffer_};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(command_buffers, 0, 1, vertex_buffers, offsets);
    vkCmdBindIndexBuffer(command_buffers, index_buffer_, 0, VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(command_buffers, static_cast<uint32_t>(indices_.size()), 1, 0, 0, 0);
  }

  void Cleanup() final {
    vkDestroyBuffer(device_, index_buffer_, nullptr);
    vkFreeMemory(device_, index_buffer_memory_, nullptr);
    vkDestroyBuffer(device_, vertex_buffer_, nullptr);
    vkFreeMemory(device_, vertex_buffer_memory_, nullptr);

    Application::Cleanup();
  }
};