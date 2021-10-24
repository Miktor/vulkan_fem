#pragma once

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <optional>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_family_;
  std::optional<uint32_t> present_family_;

  bool IsComplete() { return graphics_family_.has_value() && present_family_.has_value(); }
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities_{};
  std::vector<VkSurfaceFormatKHR> formats_;
  std::vector<VkPresentModeKHR> present_modes_;
};

struct Vertex {
  glm::vec2 pos_;

  static VkVertexInputBindingDescription GetBindingDescription();
  static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
};

class Application {
 public:
  void Run();

  virtual bool ProcessInput(GLFWindow *window, int key, int scancode, int action, int mods);

 protected:
  GLFWindow *window_ = nullptr;

  VkInstance instance_ = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;
  VkSurfaceKHR surface_ = VK_NULL_HANDLE;

  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  VkDevice device_ = VK_NULL_HANDLE;

  VkQueue graphics_queue_ = VK_NULL_HANDLE;
  VkQueue present_queue_ = VK_NULL_HANDLE;

  VkSwapchainKHR swap_chain_ = VK_NULL_HANDLE;
  std::vector<VkImage> swap_chain_images_;
  VkFormat swap_chain_image_format_ = VkFormat::VK_FORMAT_UNDEFINED;
  VkExtent2D swap_chain_extent_{};
  std::vector<VkImageView> swap_chain_image_views_;
  std::vector<VkFramebuffer> swap_chain_framebuffers_;

  VkRenderPass render_pass_ = VK_NULL_HANDLE;
  VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
  VkPipeline graphics_pipeline_ = VK_NULL_HANDLE;

  VkCommandPool command_pool_ = VK_NULL_HANDLE;

  std::vector<VkCommandBuffer> command_buffers_;

  std::vector<VkSemaphore> image_available_semaphores_;
  std::vector<VkSemaphore> render_finished_semaphores_;
  std::vector<VkFence> in_flight_fences_;
  std::vector<VkFence> images_in_flight_;
  size_t current_frame_ = 0;

  virtual void PreInit() {}
  virtual void Cleanup();
  virtual void DrawRenderPass(VkCommandBuffer command_buffers) {}
  virtual void PreDrawFrame(uint32_t image_index) {}
  virtual void CrateBuffers() {}

  void CreateIndexBuffer(VkBuffer &index_buffer, VkDeviceMemory &index_buffer_memory, const std::vector<uint16_t> &indices);
  void CreateVertexBuffer(VkBuffer &vertex_buffer, VkDeviceMemory &vertex_buffer_memory, const std::vector<Vertex> &vertexes);

  void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &buffer_memory);
  void CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

  void CreateCommandBuffers();

 private:
  void InitWindow();
  void InitVulkan();
  void MainLoop();
  void CleanupSwapChain();
  void CreateInstance();
  static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &create_info);
  void SetupDebugMessenger();

  void CreateSurface();
  void PickPhysicalDevice();
  void CreateLogicalDevice();
  void CreateSwapChain();
  void CreateImageViews();
  void CreateRenderPass();
  static VkPipeline CreateGraphicsPipeline(VkDevice device, const VkExtent2D &swap_chain_extent, VkPipelineLayout pipeline_layout,
                                           VkRenderPass render_pass, VkPolygonMode mode);
  void CreateFramebuffers();
  void CreateCommandPool();

  void CreateSyncObjects();

  void DrawFrame();

  uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);
  static VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char> &code);
  static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats);
  static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available_present_modes);
  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
  SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
  bool IsDeviceSuitable(VkPhysicalDevice device);
  static bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
  static std::vector<const char *> GetRequiredExtensions();
  static bool CheckValidationLayerSupport();
  static std::vector<char> ReadFile(const std::string &filename);

  static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                      VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                      const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data, void *p_user_data);
};