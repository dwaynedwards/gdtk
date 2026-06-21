//
// Created by Dwayne Edwards on 2026-06-20.
//


#include "gdtk/graphics/renderer.h"

#include <volk.h>

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>

#include "gdtk/platform/app.h"
#include "gdtk/platform/errors.h"
#include "gdtk/platform/log.h"

static const U32 VULKAN_VERSION = {VK_API_VERSION_1_4};
static const char* VALIDATION_LAYERS[] = {
    "VK_LAYER_KHRONOS_validation",
};

struct GDTK_Renderer
{
  SDL_Window* handle;

  VkInstance instance;
  VkSurfaceKHR surface;

  VkPhysicalDeviceProperties physical_device_props;
  VkPhysicalDevice physical_device;
  VkDevice device;

  U32 graphics_queue_idx;
  U32 present_queue_idx;
  VkQueue graphics_queue;
  VkQueue present_queue;
};

static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
               const VkDebugUtilsMessengerCallbackDataEXT* data, void* user_data)
{
  if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
  {
    LOG_ERROR("Vulkan: %s\n", data->pMessage);
  }
  else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
  {
    LOG_WARN("Vulkan: %s\n", data->pMessage);
  }
  return VK_FALSE;
}

static bool
create_instance(GDTK_Renderer* renderer, const GDTK_Config* cfg)
{
  LOG_INFO("Creating VkInstance");

  if (!SDL_CHECK(SDL_Vulkan_LoadLibrary(NULL)))
  {
    return false;
  }

  if (!VK_CHECK(volkInitialize()))
  {
    return false;
  }

  const VkApplicationInfo app_info = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .apiVersion = VULKAN_VERSION,
      .pApplicationName = cfg->title,
      .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
      .pEngineName = "GDTK Engine",
      .engineVersion = VK_MAKE_VERSION(0, 1, 0),
  };

  const VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
      .pfnUserCallback = debug_callback,
  };

  U32 extensions_count = {0};
  char const* const* instance_extensions = {SDL_Vulkan_GetInstanceExtensions(&extensions_count)};
  if (!SDL_CHECK(instance_extensions))
  {
    return false;
  }

  const U32 extra_count = {4};
  const U32 requested_extensions_count = {extensions_count + extra_count};
  const char** requested_instance_extensions = SDL_malloc(sizeof(char*) * requested_extensions_count);
  requested_instance_extensions[0] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
  requested_instance_extensions[1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
  requested_instance_extensions[2] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
  requested_instance_extensions[3] = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;
  SDL_memcpy(&requested_instance_extensions[extra_count], instance_extensions, sizeof(char*) * extensions_count);

  const VkInstanceCreateInfo instance_create_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = &debug_create_info,
      .pApplicationInfo = &app_info,
      .enabledLayerCount = 1,
      .ppEnabledLayerNames = VALIDATION_LAYERS,
      .enabledExtensionCount = requested_extensions_count,
      .ppEnabledExtensionNames = requested_instance_extensions,
      .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
  };

  const bool is_valid = VK_CHECK(vkCreateInstance(&instance_create_info, NULL, &renderer->instance));
  SDL_free(requested_instance_extensions);

  if (!is_valid)
  {
    return false;
  }

  volkLoadInstance(renderer->instance);

  LOG_INFO("VkInstance created");

  return true;
}

static bool
create_surface(GDTK_Renderer* renderer, const GDTK_Config* cfg)
{
  LOG_INFO("Creating VkSurfaceKHR");

  const SDL_WindowFlags flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  renderer->handle = SDL_CreateWindow(cfg->title, cfg->dimensions.w, cfg->dimensions.h, flags);
  if (!SDL_CHECK(renderer->handle))
  {
    return false;
  }

  if (!SDL_CHECK(SDL_Vulkan_CreateSurface(renderer->handle, renderer->instance, NULL, &renderer->surface)))
  {
    return false;
  }

  LOG_INFO("VkSurfaceKHR created");

  return true;
}

static bool
find_queue_family(GDTK_Renderer* renderer, VkPhysicalDevice physical_device)
{
  U32 queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);
  VkQueueFamilyProperties* queue_family_props = SDL_malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_props);

  VkBool32 has_graphics_support = VK_FALSE;
  VkBool32 has_present_support = VK_FALSE;
  for (U32 i = 0; i < queue_family_count; ++i)
  {
    const VkQueueFamilyProperties props = queue_family_props[i];
    if (props.queueCount > 0 && props.queueFlags & (VK_QUEUE_GRAPHICS_BIT))
    {
      has_graphics_support = VK_TRUE;
      renderer->graphics_queue_idx = i;
    }
    vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, renderer->surface, &has_present_support);
    if (!has_present_support)
    {
      renderer->present_queue_idx = i;
    }
    if (has_graphics_support && has_present_support)
    {
      break;
    }
  }
  SDL_free(queue_family_props);

  return has_graphics_support && has_present_support;
}

static bool
choose_physical_device(GDTK_Renderer* renderer)
{
  LOG_INFO("Choosing VkPhysicalDevice");

  U32 gpus_count = 0;
  if (!VK_CHECK(vkEnumeratePhysicalDevices(renderer->instance, &gpus_count, NULL)))
  {
    return false;
  }

  VkPhysicalDevice* gpus = SDL_malloc(sizeof(VkPhysicalDevice) * gpus_count);
  if (!VK_CHECK(vkEnumeratePhysicalDevices(renderer->instance, &gpus_count, gpus)))
  {
    return false;
  }

  VkPhysicalDevice discrete_gpu = NULL;
  VkPhysicalDevice integrated_gpu = NULL;

  for (U32 i = 0; i < gpus_count; ++i)
  {
    VkPhysicalDevice gpu = gpus[i];
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(gpu, &props);
    const char* gpu_type = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "Discrete" : "Integrated";

    LOG_INFO("[%d] - GPU : %s - %s", i, props.deviceName, gpu_type);

    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
      if (find_queue_family(renderer, gpu))
      {
        discrete_gpu = gpu;
        break;
      }
      continue;
    }

    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
    {
      if (find_queue_family(renderer, gpu))
      {
        integrated_gpu = gpu;
      }
    }
  }

  SDL_free(gpus);

  if (discrete_gpu != NULL)
  {
    renderer->physical_device = discrete_gpu;
  }
  else if (integrated_gpu != NULL)
  {
    renderer->physical_device = integrated_gpu;
  }
  else
  {
    return false;
  }

  vkGetPhysicalDeviceProperties(renderer->physical_device, &renderer->physical_device_props);

  const char* gpu_type =
      renderer->physical_device_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "Discrete" : "Integrated";
  LOG_INFO("GPU chosen: %s - %s", renderer->physical_device_props.deviceName, gpu_type);

  if (!SDL_CHECK(SDL_Vulkan_GetPresentationSupport(renderer->instance, renderer->physical_device,
                                                   renderer->graphics_queue_idx)))
  {
    return false;
  }

  LOG_INFO("Chose VkPhysicalDevice");

  return true;
}

static bool
create_logical_device(GDTK_Renderer* renderer)
{
  LOG_INFO("Creating VkDevice");

  VkPhysicalDeviceVulkan14Features supported_features14 = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
      .pNext = NULL,
  };

  VkPhysicalDeviceVulkan13Features supported_features13 = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
      .pNext = &supported_features14,
  };

  VkPhysicalDeviceVulkan12Features supported_features12 = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
      .pNext = &supported_features13,
  };

  VkPhysicalDeviceFeatures2 supported_features = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
      .pNext = &supported_features12,
  };

  vkGetPhysicalDeviceFeatures2(renderer->physical_device, &supported_features);

  const F32 queue_priorities = {1.0f};

  VkDeviceQueueCreateInfo queue_create_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = renderer->graphics_queue_idx,
      .queueCount = 1,
      .pQueuePriorities = &queue_priorities,
  };

  const U32 device_extensions_count = 2;
  const char* device_extensions[2] = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      "VK_KHR_portability_subset",
  };

  VkPhysicalDeviceVulkan14Features enable_vk14_features = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
      .pNext = NULL,
  };

  VkPhysicalDeviceVulkan13Features enable_vk13_features = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
      .pNext = &enable_vk14_features,
      .synchronization2 = VK_TRUE,
      .dynamicRendering = VK_TRUE,
  };

  VkPhysicalDeviceVulkan12Features enable_vk12_features = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
      .pNext = &enable_vk13_features,
      .descriptorIndexing = VK_TRUE,
      .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
      .descriptorBindingVariableDescriptorCount = VK_TRUE,
      .runtimeDescriptorArray = VK_TRUE,
      .bufferDeviceAddress = VK_TRUE,
  };

  VkPhysicalDeviceFeatures2 enable_vk10_features = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
      .pNext = &enable_vk12_features,
      .features =
          {
              .samplerAnisotropy = VK_TRUE,
          },
  };

  VkDeviceCreateInfo device_create_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = &enable_vk10_features,
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &queue_create_info,
      .enabledExtensionCount = device_extensions_count,
      .ppEnabledExtensionNames = (const char* const*)device_extensions,
      .pEnabledFeatures = NULL,
  };

  if (!VK_CHECK(vkCreateDevice(renderer->physical_device, &device_create_info, NULL, &renderer->device)))
  {
    return false;
  }

  vkGetDeviceQueue(renderer->device, renderer->graphics_queue_idx, 0, &renderer->graphics_queue);
  vkGetDeviceQueue(renderer->device, renderer->present_queue_idx, 0, &renderer->present_queue);

  LOG_INFO("VkDevice created");

  return true;
}


GDTK_Renderer*
renderer_create(const GDTK_Config* cfg)
{
  GDTK_Renderer* renderer = SDL_malloc(sizeof(GDTK_Renderer));
  if (!renderer)
  {
    LOG_ERROR("Failed to allocate GDTK_Renderer memory");
    renderer_destroy(renderer);
    return NULL;
  }
  SDL_memset(renderer, 0, sizeof(GDTK_Renderer));

  if (!create_instance(renderer, cfg))
  {
    LOG_ERROR("Failed to create VkInstance");
    renderer_destroy(renderer);
    return NULL;
  }

  if (!create_surface(renderer, cfg))
  {
    LOG_ERROR("Failed to create VkSurfaceKHR");
    renderer_destroy(renderer);
    return NULL;
  }

  if (!choose_physical_device(renderer))
  {
    LOG_ERROR("Failed to choose VkPhysicalDevice");
    renderer_destroy(renderer);
    return NULL;
  }

  if (!create_logical_device(renderer))
  {
    LOG_ERROR("Failed to create VkDevice");
    renderer_destroy(renderer);
    return NULL;
  }

  return renderer;
}

void
renderer_destroy(GDTK_Renderer* renderer)
{
  if (!renderer)
  {
    return;
  }

  LOG_INFO("Destroying GDTK_Renderer");

  if (renderer->device)
  {
    vkDestroyDevice(renderer->device, NULL);
    renderer->device = NULL;
  }

  if (renderer->surface)
  {
    vkDestroySurfaceKHR(renderer->instance, renderer->surface, NULL);
    renderer->surface = NULL;
  }

  if (renderer->instance)
  {
    vkDestroyInstance(renderer->instance, NULL);
    renderer->instance = NULL;
  }

  if (renderer->handle)
  {
    SDL_DestroyWindow(renderer->handle);
    renderer->handle = NULL;
  }

  SDL_free(renderer);
  renderer = NULL;

  LOG_INFO("GDTK_Renderer destroyed");
}
