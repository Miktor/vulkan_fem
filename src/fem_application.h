#pragma once

#include "model_factory.h"
#include "solver.h"
#include "vulcan.h"
#include "vulkan_model.h"
#include <cstddef>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>

class FEMApplication final : public Application {
 private:
  VkBuffer vertex_buffer_{VK_NULL_HANDLE};
  VkDeviceMemory vertex_buffer_memory_{VK_NULL_HANDLE};
  VkBuffer index_buffer_{VK_NULL_HANDLE};
  VkDeviceMemory index_buffer_memory_{VK_NULL_HANDLE};

  std::vector<uint16_t> indices_;

  std::shared_ptr<vulkan_fem::Solver<2>> solver_;
  std::shared_ptr<vulkan_fem::Model<2>> model_;
  std::shared_ptr<vulkan_fem::VulkanModel<2>> render_model_;

  bool needs_update_ = false;

 protected:
  void PreInit() final;

  bool ProcessInput(GLFWindow *window, int key, int scancode, int action, int mods) final;

  static std::vector<Vertex> ToVertices(const std::vector<vulkan_fem::Vertex3> &data);

  void CrateBuffers() final;

  void UpdateBuffer(VkBuffer &buffer, const void *data, VkDeviceSize data_size);

  void PreDrawFrame(uint32_t /*image_index*/) final;

  void DrawRenderPass(VkCommandBuffer command_buffers) final;

  void Cleanup() final;
};