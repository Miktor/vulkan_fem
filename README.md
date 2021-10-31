# vulkan_fem

Finite Element Method implementation in c++ with Vulkan fronted.

## Controls

* __1__ load model with triangular elements
* __2__ load model with rectangular elements
* __space__ simulate models with top nodes fixed and load force to BR corner.

## Dependencies

* [Vulkan](https://www.lunarg.com/vulkan-sdk/)
* [GLM](https://github.com/g-truc/glm)
* [glfw3](https://www.glfw.org/)
* [Eigen3](https://eigen.tuxfamily.org/index.php?title=Main_Page)
* [spdlog](https://github.com/gabime/spdlog)

## Build

```
mkdir build
cd build
cmake ../
make
```

To run, go to root directory and execute ```./build/vulkan_fem```
