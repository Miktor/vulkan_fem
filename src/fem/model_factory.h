#ifndef model_factory_h__
#define model_factory_h__

#include "fem.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "model.h"

namespace vulkan_fem {

class ModelFactory {
 public:
  static const precision TWO_PI;

  static std::shared_ptr<Model<2>> CreateRectangle() {
    std::vector<Vertex3> vertices = {
        {.0, .0},
        {1.0, .0},
        {.0, 1.0},
        {1.0, 1.0},
    };
    std::vector<uint16_t> indices = {0, 1, 2, 1, 3, 2};

    std::vector<Constraint> constraints = {{0, Constraint::UXY},
                                           {1, Constraint::UY}};
    std::vector<Load<2>> loads = {{2, {0, 1.}}, {3, {0, 1.}}};

    return std::make_shared<Model<2>>(
        std::make_shared<TriangleElement>(), vertices, indices, constraints,
        loads, 0.2e4, 0.3);  // 200GPa, 0.3 Young, Poisson's for steel
  }

  // static std::shared_ptr<Model<3>> CreateCylinderModel(const precision r,
  // const precision h)
  //{
  //	std::vector<Vertex3> vertices;
  //	std::vector<uint16_t> indices;

  //	generateCylinder(r, h, vertices, indices);

  //	return std::shared_ptr<Model<3>>(new
  //Model<3>(std::make_shared<Element2dTo3d>(std::make_shared<TriangleElement>()),
  //									  vertices,
  //									  indices,
  //									  0.2e12, 0.3)); // 200GPa, 0.3 Young,
  //Poisson's for steel
  //}

 private:
  static void generateCylinder(const precision r, const precision h,
                               std::vector<Vertex3>& vertices,
                               std::vector<uint16_t>& indices) {
    static const uint32_t SIDES = 5;
    static const uint32_t RINGS = 6;

    const uint32_t faces = SIDES * RINGS;
    const uint32_t nVerts = (SIDES + 1) * RINGS + 2 * (SIDES + 1);

    vertices.resize(nVerts);
    indices.resize(6 * SIDES * (RINGS - 1) + 2 * 3 * SIDES);

    // The side of the cylinder
    const precision dTheta = TWO_PI / static_cast<precision>(SIDES);
    const precision dy = h / static_cast<precision>(RINGS - 1);
    const precision half_h = h / precision(2);

    // Iterate over heights (rings)
    uint16_t index = 0;

    for (uint32_t ring = 0; ring < RINGS; ++ring) {
      const precision y = -half_h + static_cast<precision>(ring) * dy;

      // Iterate over slices (segments in a ring)
      for (int slice = 0; slice < SIDES + 1; ++slice) {
        const precision theta = static_cast<precision>(slice) * dTheta;
        const precision cosTheta = std::cos(theta);
        const precision sinTheta = std::sin(theta);

        vertices[index] = {r * cosTheta, y, r * sinTheta};

        ++index;
      }
    }

    uint16_t elIndex = 0;
    for (uint32_t i = 0; i < RINGS - 1; ++i) {
      const uint16_t ringStartIndex = i * (SIDES + 1);
      const uint32_t nextRingStartIndex = (i + 1) * (SIDES + 1);

      for (uint32_t j = 0; j < SIDES; ++j) {
        // Split the quad into two triangles
        indices[elIndex] = ringStartIndex + j;
        indices[elIndex + 1] = ringStartIndex + j + 1;
        indices[elIndex + 2] = nextRingStartIndex + j;
        indices[elIndex + 3] = nextRingStartIndex + j;
        indices[elIndex + 4] = ringStartIndex + j + 1;
        indices[elIndex + 5] = nextRingStartIndex + j + 1;

        elIndex += 6;
      }
    }

    generateEndCapVertexData(SIDES, r, half_h, index, elIndex, vertices,
                             indices);
    generateEndCapVertexData(SIDES, r, -half_h, index, elIndex, vertices,
                             indices);
  }

  static void generateEndCapVertexData(const uint32_t sides, const precision r,
                                       const precision y, uint16_t& index,
                                       uint16_t& elIndex,
                                       std::vector<Vertex3>& vertices,
                                       std::vector<uint16_t>& indices) {
    // Make a note of the vertex index for the center of the end cap
    const uint16_t endCapStartIndex = index;

    vertices[index++] = {.0, y, .0};

    const precision dTheta{TWO_PI / static_cast<precision>(sides)};
    for (uint32_t slice = 0; slice < sides; ++slice) {
      const precision theta = static_cast<precision>(slice) * dTheta;
      const precision cosTheta = std::cos(theta);
      const precision sinTheta = std::sin(theta);

      vertices[index] = {r * cosTheta, y, r * sinTheta};

      ++index;
    }

    for (uint32_t i = 0; i < sides; ++i) {
      // Use center point and i+1, and i+2 vertices to create triangles
      if (i != sides - 1) {
        indices[elIndex] = endCapStartIndex;
        indices[elIndex + 1] = endCapStartIndex + i + 1;
        indices[elIndex + 2] = endCapStartIndex + i + 2;
      } else {
        indices[elIndex] = endCapStartIndex;
        indices[elIndex + 1] = endCapStartIndex + i + 1;
        indices[elIndex + 2] = endCapStartIndex + 1;
      }

      elIndex += 3;
    }
  }
};

const precision ModelFactory::TWO_PI =
    static_cast<precision>(2.) * static_cast<precision>(M_PI);
}  // namespace vulkan_fem

#endif  // model_factory_h__
