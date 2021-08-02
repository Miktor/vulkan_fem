#pragma once

#include "fem.h"

#define USE_MATH_DEFINES
#include "model.h"
#include <cmath>

namespace vulkan_fem {

class ModelFactory {
 public:
  static constexpr Precision kTwoPi = static_cast<Precision>(2.) * static_cast<Precision>(M_PI);

  static std::shared_ptr<Model<2>> CreateRectangle() {
    std::vector<Vertex3> vertices = {
        {-0.5, -0.5, .0},
        {0.5, -0.5, .0},
        {0.5, 0.5, .0},
        {-0.5, 0.5, .0},
    };
    std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

    std::vector<Constraint> constraints = {{0, Constraint::kUxy}, {1, Constraint::kUxy}};
    std::vector<Load<2>> loads = {{3, {1, -1.0}}};

    return std::make_shared<Model<2>>(std::make_shared<TriangleElement>(), vertices, indices, constraints, loads, 0.2e4,
                                      0.3);  // 200GPa, 0.3 Young, Poisson's for steel
  }

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
  static void GenerateCylinder(Precision r, Precision h, std::vector<Vertex3> &vertices, std::vector<uint16_t> &indices) {
    static const uint32_t kSides = 5;
    static const uint32_t kRings = 6;

    // const uint32_t faces = SIDES * RINGS;
    const uint32_t n_verts = (kSides + 1) * kRings + 2 * (kSides + 1);

    vertices.resize(n_verts);
    indices.resize(6 * kSides * (kRings - 1) + 2 * 3 * kSides);

    // The side of the cylinder
    Precision d_theta = kTwoPi / static_cast<Precision>(kSides);
    Precision dy = h / static_cast<Precision>(kRings - 1);
    Precision half_h = h / Precision(2);

    // Iterate over heights (rings)
    uint16_t index = 0;

    for (uint32_t ring = 0; ring < kRings; ++ring) {
      Precision y = -half_h + static_cast<Precision>(ring) * dy;

      // Iterate over slices (segments in a ring)
      for (uint32_t slice = 0; slice < kSides + 1; ++slice) {
        Precision theta = static_cast<Precision>(slice) * d_theta;
        Precision cos_theta = std::cos(theta);
        Precision sin_theta = std::sin(theta);

        vertices[index] = {r * cos_theta, y, r * sin_theta};

        ++index;
      }
    }

    uint16_t el_index = 0;
    for (uint32_t i = 0; i < kRings - 1; ++i) {
      const uint16_t ring_start_index = i * (kSides + 1);
      const uint32_t next_ring_start_index = (i + 1) * (kSides + 1);

      for (uint32_t j = 0; j < kSides; ++j) {
        // Split the quad into two triangles
        indices[el_index] = ring_start_index + j;
        indices[el_index + 1] = ring_start_index + j + 1;
        indices[el_index + 2] = next_ring_start_index + j;
        indices[el_index + 3] = next_ring_start_index + j;
        indices[el_index + 4] = ring_start_index + j + 1;
        indices[el_index + 5] = next_ring_start_index + j + 1;

        el_index += 6;
      }
    }

    GenerateEndCapVertexData(kSides, r, half_h, index, el_index, vertices, indices);
    GenerateEndCapVertexData(kSides, r, -half_h, index, el_index, vertices, indices);
  }

  static void GenerateEndCapVertexData(const uint32_t sides, Precision r, Precision y, uint16_t &index, uint16_t &el_index,
                                       std::vector<Vertex3> &vertices, std::vector<uint16_t> &indices) {
    // Make a note of the vertex index for the center of the end cap
    const uint16_t end_cap_start_index = index;

    vertices[index++] = {.0, y, .0};

    Precision d_theta{kTwoPi / static_cast<Precision>(sides)};
    for (uint32_t slice = 0; slice < sides; ++slice) {
      Precision theta = static_cast<Precision>(slice) * d_theta;
      Precision cos_theta = std::cos(theta);
      Precision sin_theta = std::sin(theta);

      vertices[index] = {r * cos_theta, y, r * sin_theta};

      ++index;
    }

    for (uint32_t i = 0; i < sides; ++i) {
      // Use center point and i+1, and i+2 vertices to create triangles
      if (i != sides - 1) {
        indices[el_index] = end_cap_start_index;
        indices[el_index + 1] = end_cap_start_index + i + 1;
        indices[el_index + 2] = end_cap_start_index + i + 2;
      } else {
        indices[el_index] = end_cap_start_index;
        indices[el_index + 1] = end_cap_start_index + i + 1;
        indices[el_index + 2] = end_cap_start_index + 1;
      }

      el_index += 3;
    }
  }
};

}  // namespace vulkan_fem
