//
// Created by Dwayne Edwards on 2026-06-21.
//

#include "gdtk/graphics/vulkan/instance.h"

#include "gdtk/platform/app.h"

static const u32 VULKAN_VERSION = {VK_API_VERSION_1_4};
static const char* VALIDATION_LAYERS[] = {
    "VK_LAYER_KHRONOS_validation",
};

static VKAPI_ATTR VkBool32 VKAPI_CALL
internal_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
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

b8
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
      .pfnUserCallback = internal_debug_callback,
  };

  u32 available_extensions_count = {0};
  char const* const* instance_extensions = {SDL_Vulkan_GetInstanceExtensions(&available_extensions_count)};
  if (!SDL_CHECK(instance_extensions))
  {
    return false;
  }

#if GDTK_PLATFORM_APPLE
  const u32 extended_extensions_count = {3};
#else
  const u32 extended_extensions_count = {2};
#endif
  const u32 requested_extensions_count = {available_extensions_count + extended_extensions_count};
  const char** requested_instance_extensions = SDL_malloc(sizeof(char*) * requested_extensions_count);
  requested_instance_extensions[0] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
  requested_instance_extensions[1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
#if GDTK_PLATFORM_APPLE
  requested_instance_extensions[2] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
#endif
  SDL_memcpy(&requested_instance_extensions[extended_extensions_count], instance_extensions,
             sizeof(char*) * available_extensions_count);

  const VkInstanceCreateInfo instance_create_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = &debug_create_info,
      .pApplicationInfo = &app_info,
      .enabledLayerCount = 1,
      .ppEnabledLayerNames = VALIDATION_LAYERS,
      .enabledExtensionCount = requested_extensions_count,
      .ppEnabledExtensionNames = requested_instance_extensions,
#if GDTK_PLATFORM_APPLE
      .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#endif
  };

  const b8 is_valid = VK_CHECK(vkCreateInstance(&instance_create_info, NULL, &renderer->instance));
  SDL_free(requested_instance_extensions);

  if (!is_valid)
  {
    return false;
  }

  volkLoadInstance(renderer->instance);

  LOG_INFO("VkInstance created");

  return true;
}

void
destroy_instance(GDTK_Renderer* renderer)
{
  if (!renderer->instance)
  {
    return;
  }

  vkDestroyInstance(renderer->instance, NULL);
  renderer->instance = NULL;
}
