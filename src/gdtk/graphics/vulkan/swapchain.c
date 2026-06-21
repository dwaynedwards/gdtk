//
// Created by Dwayne Edwards on 2026-06-21.
//

#include "gdtk/graphics/vulkan/swapchain.h"

static b8
internal_create_swapchain(GDTK_Renderer* renderer)
{
  LOG_INFO("Creating VkSwapchainKHR");

  VkSurfaceCapabilitiesKHR surface_caps;
  if (!VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(renderer->physical_device, renderer->surface, &surface_caps)))
  {
    return false;
  }

  u32 surface_format_count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(renderer->physical_device, renderer->surface, &surface_format_count, NULL);

  VkSurfaceFormatKHR* surface_formats = SDL_malloc(sizeof(VkSurfaceFormatKHR) * surface_format_count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(renderer->physical_device, renderer->surface, &surface_format_count,
                                       surface_formats);
  VkSurfaceFormatKHR chosen_surface_formats = surface_formats[0];

  for (u32 i = 0; i < surface_format_count; ++i)
  {
    if (surface_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
        surface_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    {
      chosen_surface_formats = surface_formats[i];
    }
  }
  SDL_free(surface_formats);
  renderer->swapchain_format = chosen_surface_formats.format;


  if (surface_caps.currentExtent.width != UINT32_MAX)
  {
    renderer->swapchain_extent = surface_caps.currentExtent;
  }

  if (surface_caps.currentExtent.width == 0xFFFFFFFF)
  {
    s32 window_w, window_h;
    if (!SDL_CHECK(SDL_GetWindowSize(renderer->window, &window_w, &window_h)))
    {
      return false;
    }

    VkExtent2D extent = {
        (u32)window_w,
        (u32)window_h,
    };
    if (extent.width < surface_caps.minImageExtent.width)
    {
      extent.width = surface_caps.minImageExtent.width;
    }
    if (extent.height < surface_caps.minImageExtent.height)
    {
      extent.height = surface_caps.minImageExtent.height;
    }
    if (extent.width > surface_caps.maxImageExtent.width)
    {
      extent.width = surface_caps.maxImageExtent.width;
    }
    if (extent.height > surface_caps.maxImageExtent.height)
    {
      extent.height = surface_caps.maxImageExtent.height;
    }
    renderer->swapchain_extent = extent;
  }

  u32 min_image_count = surface_caps.minImageCount + 1;
  if (surface_caps.maxImageCount > 0 && min_image_count > surface_caps.maxImageCount)
  {
    min_image_count = surface_caps.maxImageCount;
  }

  VkSwapchainCreateInfoKHR swapchain_create_info = {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = renderer->surface,
      .minImageCount = min_image_count,
      .imageFormat = renderer->swapchain_format,
      .imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
      .imageExtent = renderer->swapchain_extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = VK_PRESENT_MODE_FIFO_KHR,
      .clipped = VK_TRUE,
  };

  const u32 queue_family_indices[] = {
      renderer->graphics_queue_idx,
      renderer->present_queue_idx,
  };

  if (renderer->graphics_queue_idx == renderer->present_queue_idx)
  {
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }
  else
  {
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchain_create_info.queueFamilyIndexCount = 2;
    swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
  }

  if (!VK_CHECK(vkCreateSwapchainKHR(renderer->device, &swapchain_create_info, NULL, &renderer->swapchain)))
  {
    return false;
  }

  LOG_INFO("VkSwapchainKHR created");

  return true;
}

static b8
internal_create_swapchain_images_and_views(GDTK_Renderer* renderer)
{
  LOG_INFO("Create swapchain VkImages & VkImageViews");

  if (!VK_CHECK(vkGetSwapchainImagesKHR(renderer->device, renderer->swapchain, &renderer->swapchain_image_count, NULL)))
  {
    return false;
  }

  renderer->swapchain_images = SDL_malloc(sizeof(VkImage) * renderer->swapchain_image_count);
  if (!VK_CHECK(vkGetSwapchainImagesKHR(renderer->device, renderer->swapchain, &renderer->swapchain_image_count,
                                        renderer->swapchain_images)))
  {
    return false;
  }

  renderer->swapchain_image_views = SDL_malloc(sizeof(VkImageView) * renderer->swapchain_image_count);

  for (size_t i = 0; i < renderer->swapchain_image_count; ++i)
  {
    VkImageViewCreateInfo image_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = renderer->swapchain_images[i],
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = renderer->swapchain_format,
        .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .levelCount = 1,
                .layerCount = 1,
            },
    };
    if (!VK_CHECK(
            vkCreateImageView(renderer->device, &image_view_create_info, NULL, &renderer->swapchain_image_views[i])))
    {
      return false;
    }
  }

  LOG_INFO("Swapchain VkImages & VkImageViews created");

  return true;
}

b8
create_swapchain(GDTK_Renderer* renderer)
{
  if (!internal_create_swapchain(renderer))
  {
    LOG_ERROR("failed to create VkSwapchainKHR");
    return false;
  }

  if (!internal_create_swapchain_images_and_views(renderer))
  {
    LOG_ERROR("failed to create swapchain VkImages & VkImageViews");
    return false;
  }

  return true;
}

void
destroy_swapchain(GDTK_Renderer* renderer)
{
  if (renderer->swapchain_image_views)
  {
    for (u32 i = 0; i < renderer->swapchain_image_count; ++i)
    {
      vkDestroyImageView(renderer->device, renderer->swapchain_image_views[i], NULL);
    }
    SDL_free(renderer->swapchain_image_views);
    renderer->swapchain_image_views = NULL;
  }

  if (renderer->swapchain_images)
  {
    SDL_free(renderer->swapchain_images);
    renderer->swapchain_images = NULL;
  }

  if (renderer->swapchain)
  {
    vkDestroySwapchainKHR(renderer->device, renderer->swapchain, NULL);
    renderer->swapchain = NULL;
  }
}
