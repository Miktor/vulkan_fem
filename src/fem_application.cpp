#include "fem_application.h"

void FEMApplication::PreInit() {
  solver_ = std::make_shared<vulkan_fem::Solver<2>>();
  model_ = vulkan_fem::ModelFactory::CreateRectangle2();
  render_model_ = std::make_shared<vulkan_fem::VulkanModel<2>>(model_);
}

bool FEMApplication::ProcessInput(GLFWindow *window, int key, int scancode, int action, int mods) {
  if (!Application::ProcessInput(window, key, scancode, action, mods)) {
    return false;
  }

  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_SPACE:
        solver_->Solve(*model_);
        needs_update_ = true;
        return false;
      case GLFW_KEY_1:
        model_ = vulkan_fem::ModelFactory::CreateRectangle();
        render_model_ = std::make_shared<vulkan_fem::VulkanModel<2>>(model_);
        needs_update_ = true;
        return false;
      case GLFW_KEY_2:
        model_ = vulkan_fem::ModelFactory::CreateRectangle2();
        render_model_ = std::make_shared<vulkan_fem::VulkanModel<2>>(model_);
        needs_update_ = true;
        return false;
    }
  }

  return true;
}

void FEMApplication::CrateBuffers() {
  indices_ = render_model_->GetIndices();
  const auto vertices = render_model_->GetVertices();

  CreateIndexBuffer(index_buffer_, index_buffer_memory_, indices_);
  CreateVertexBuffer(vertex_buffer_, vertex_buffer_memory_, vertices);
}

void FEMApplication::PreDrawFrame(uint32_t /*image_index*/) {
  if (!needs_update_) {
    return;
  }

  SPDLOG_INFO("Updating");

  const auto vertexes = render_model_->GetVertices();
  const auto indices = render_model_->GetIndices();

  vkDestroyBuffer(device_, index_buffer_, nullptr);
  vkFreeMemory(device_, index_buffer_memory_, nullptr);
  vkDestroyBuffer(device_, vertex_buffer_, nullptr);
  vkFreeMemory(device_, vertex_buffer_memory_, nullptr);

  CreateVertexBuffer(vertex_buffer_, vertex_buffer_memory_, vertexes);
  CreateIndexBuffer(index_buffer_, index_buffer_memory_, indices);

  CreateCommandBuffers();

  needs_update_ = false;
}

void FEMApplication::DrawRenderPass(VkCommandBuffer command_buffers) {
  VkBuffer vertex_buffers[] = {vertex_buffer_};
  VkDeviceSize offsets[] = {0};

  vkCmdBindVertexBuffers(command_buffers, 0, 1, vertex_buffers, offsets);
  vkCmdBindIndexBuffer(command_buffers, index_buffer_, 0, VK_INDEX_TYPE_UINT16);

  vkCmdDrawIndexed(command_buffers, static_cast<uint32_t>(indices_.size()), 1, 0, 0, 0);
}

void FEMApplication::Cleanup() {
  vkDestroyBuffer(device_, index_buffer_, nullptr);
  vkFreeMemory(device_, index_buffer_memory_, nullptr);
  vkDestroyBuffer(device_, vertex_buffer_, nullptr);
  vkFreeMemory(device_, vertex_buffer_memory_, nullptr);

  Application::Cleanup();
}
