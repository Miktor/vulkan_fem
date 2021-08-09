#pragma once

#include "model.h"
#include "vulcan.h"

namespace vulkan_fem {

template <uint32_t DIM = 3>
class VulkanModel {
 private:
  std::shared_ptr<Model<DIM>> model_;

 public:
  VulkanModel(std::shared_ptr<Model<DIM>> model) : model_(model) {}

  [[nodiscard]] std::vector<Vertex> GetVertices() const { return ToVertices(model_->GetVertices()); }
  [[nodiscard]] std::vector<uint16_t> GetIndices() const { return ToIndices(model_->GetIndices()); }

 private:
  static std::vector<Vertex> ToVertices(const std::vector<vulkan_fem::Vertex3> &data) {
    std::vector<Vertex> result;
    result.reserve(data.size());

    for (const auto &input : data) {
      Vertex vertex{};
      vertex.pos_ = {input[0], input[1]};
      result.push_back(vertex);
    }

    return result;
  }

  std::vector<uint16_t> ToIndices(const std::vector<uint16_t> &data) const {
    if (model_->GetElementType()->GetElementCount() == 4)  // rect
    {
      std::vector<uint16_t> result;
      result.reserve(data.size() / 4 * 6);

      for (int i = 0; i < data.size(); ++i) {
        if (i % 4 == 3) {
          result.push_back(data[i - 1]);
          result.push_back(data[i]);
          result.push_back(data[i - 3]);
        } else {
          result.push_back(data[i]);
        }
      }
      return result;
    }
    return data;
  }
};

}  // namespace vulkan_fem