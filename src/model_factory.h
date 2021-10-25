#pragma once

#include <vector>

#include "fem.h"
#include "model.h"

namespace vulkan_fem {

class ModelFactory {
 private:
  static auto Convert2dMesh(uint8_t elem_size, uint8_t order, std::vector<Vertex3> vertices, std::vector<uint16_t> indices);

 public:
  static constexpr Precision kTwoPi = static_cast<Precision>(2.) * static_cast<Precision>(M_PI);

  static std::shared_ptr<Model<2>> CreateRectangle();
  static std::shared_ptr<Model<2>> CreateRectangle2();

  // static std::shared_ptr<Model<3>> CreateCylinderModel(const precision r,
  // const precision h)
  //{
  //	std::vector<Vertex3> vertices;
  //	std::vector<uint16_t> indices;

  //	generateCylinder(r, h, vertices, indices);

  //	return std::shared_ptr<Model<3>>(new
  // Model<3>(std::make_shared<Element2dTo3d>(std::make_shared<TriangleElement>()),
  //									  vertices,
  //									  indices,
  //									  0.2e12,
  // 0.3));
  //// 200GPa, 0.3 Young, Poisson's for steel
  //}

 private:
  static void GenerateCylinder(Precision r, Precision h, std::vector<Vertex3> &vertices, std::vector<uint16_t> &indices);

  static void GenerateEndCapVertexData(const uint32_t sides, Precision r, Precision y, uint16_t &index, uint16_t &el_index,
                                       std::vector<Vertex3> &vertices, std::vector<uint16_t> &indices);
};

}  // namespace vulkan_fem
