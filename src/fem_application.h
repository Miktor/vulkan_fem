#pragma once

#include "model_factory.h"
#include "solver.h"
#include "vulcan.h"
#include <memory>

class FEMApplication final : public Application {
 protected:
  void PostVulkanInit() final {
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

 private:
  std::shared_ptr<vulkan_fem::Solver<2>> solver_;
  std::shared_ptr<vulkan_fem::Model<2>> model_;
};