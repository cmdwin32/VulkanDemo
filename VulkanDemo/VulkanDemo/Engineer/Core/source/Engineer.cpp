#include "Engineer.h"
#include "VulkanFunctions.h"
namespace FD {

	Engineer::Engineer()
	{

	}

	Engineer::~Engineer()
	{

	}

	bool Engineer::OnWindowSizeChanged() 
	{
		return true;
	}

	bool Engineer::Draw() 
	{
		return true;
	}


	bool Engineer::PrepareVulkan()
	{
		TRY_CALL(LoadVulkanLibrary,false);
		TRY_CALL(LoadExportedEntryPoints, false);
		TRY_CALL(LoadGlobalLevelEntryPoints, false);
		TRY_CALL(CreateInstance, false);
		TRY_CALL(LoadInstanceLevelEntryPoints, false);
		TRY_CALL(CreateDevice, false);
		TRY_CALL(LoadDeviceLevelEntryPoints, false);
		TRY_CALL(GetDeviceQueue, false);
		return true;
	}

	bool Engineer::CreateInstance()
	{
		VkApplicationInfo app_info =
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			"API without Secrets: Introduction to Vulkan",
			VK_MAKE_VERSION(1, 0, 0), // app version
			"Vulkan Engineer Demo",
			VK_MAKE_VERSION(1, 0, 0), // engine version
			VK_MAKE_VERSION(1, 0, 0) // api version
		};

		VkInstanceCreateInfo instance_create_info =
		{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			nullptr,
			0,
			&app_info,
			0,
			nullptr,
			0,
			nullptr,D
		};

		if (vkCreateInstance( &instance_create_info, nullptr, & _vulkan.Instance ) != VK_SUCCESS )
		{
			DEBUG_ERROR("Could not create Vulkan instance!");
			return false;
		}
		return true;

	}


} // FD
