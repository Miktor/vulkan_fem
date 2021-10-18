#pragma once

#include "model.h"
#include "vulcan.h"

namespace vulkan_fem {

template <uint32_t DIM = 3>
class VulkanModel {
 private:
  std::shared_ptr<Model<DIM>> model_;

 public:
  explicit VulkanModel(std::shared_ptr<Model<DIM>> model) : model_(model) {}

  [[nodiscard]] std::vector<Vertex> GetVertices() const { return ToVertices(model_->GetVertices()); }
  [[nodiscard]] std::vector<uint16_t> GetIndices() const { return ToIndices(model_->GetIndices()); }

 private:
  static std::vector<Vertex> ToVertices(const std::vector<vulkan_fem::Vertex3> &data) {
    std::vector<Vertex> result;
    result.reserve(data.size());
    
    for (const auto & i : data) {
      Vertex vertex{};
      vertex.pos_ = {i[0], i[1]};
      result.push_back(vertex);
    }

    return result;
  }

  [[nodiscard]] std::vector<uint16_t> ToIndices(const std::vector<uint16_t> &data) const {
    switch (model_->GetElementType()->GetElementCount()) {
      case 4: {
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
      case 6: {
        std::vector<uint16_t> result;
        result.reserve(data.size() / 2);

        for (int i = 0; i < data.size(); ++i) {
          if (i % 6 < 3) {
            result.push_back(data[i]);
          }
        }
        return result;
      }
      case 8: {
        std::vector<uint16_t> result;
        result.reserve(data.size() / 8 * 6);

        for (int i = 0; i < data.size(); ++i) {
          if (i % 8 == 3) {
            result.push_back(data[i - 1]);
            result.push_back(data[i]);
            result.push_back(data[i - 3]);
          } else if (i < 4) {
            result.push_back(data[i]);
          }
        }
        return result;
      }
    }
    return data;
  }
};

}  // namespace vulkan_fem