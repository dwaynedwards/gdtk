//
// Created by Dwayne Edwards on 2026-06-21.
//

#include "gdtk/graphics/vulkan/device.h"

static b8
internal_find_queue_family(GDTK_Renderer* renderer, VkPhysicalDevice physical_device)
{
  u32 queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);
  VkQueueFamilyProperties* queue_family_props = SDL_malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_props);

  VkBool32 has_graphics_support = VK_FALSE;
  VkBool32 has_present_support = VK_FALSE;
  for (u32 i = 0; i < queue_family_count; ++i)
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

static b8
internal_choose_physical_device(GDTK_Renderer* renderer)
{
  LOG_INFO("Choosing VkPhysicalDevice");

  u32 gpus_count = 0;
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

  for (u32 i = 0; i < gpus_count; ++i)
  {
    VkPhysicalDevice gpu = gpus[i];
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(gpu, &props);
    const char* gpu_type = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "Discrete" : "Integrated";

    LOG_INFO("Available GPU [%d] : %s - %s", i, props.deviceName, gpu_type);

    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
      if (internal_find_queue_family(renderer, gpu))
      {
        discrete_gpu = gpu;
        break;
      }
      continue;
    }

    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
    {
      if (internal_find_queue_family(renderer, gpu))
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

static b8
internal_create_logical_device(GDTK_Renderer* renderer)
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

  const f32 queue_priorities = {1.0f};

  VkDeviceQueueCreateInfo queue_create_info = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .queueFamilyIndex = renderer->graphics_queue_idx,
      .queueCount = 1,
      .pQueuePriorities = &queue_priorities,
  };

  const u32 device_extensions_count = 2;
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

b8
create_device(GDTK_Renderer* renderer)
{
  if (!internal_choose_physical_device(renderer))
  {
    LOG_ERROR("Failed to choose VkPhysicalDevice");
    return false;
  }

  if (!internal_create_logical_device(renderer))
  {
    LOG_ERROR("Failed to create VkDevice");
    return false;
  }

  return true;
}

void
destroy_device(GDTK_Renderer* renderer)
{
  if (renderer->device)
  {
    vkDestroyDevice(renderer->device, NULL);
    renderer->device = NULL;
  }
}
