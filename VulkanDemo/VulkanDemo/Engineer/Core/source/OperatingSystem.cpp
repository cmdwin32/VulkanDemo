#include <thread>
#include <chrono>
#include "OperatingSystem.h"
#include "VulkanFunctions.h"
#include <cstdlib>
#include <vector>
namespace FD {
	namespace OS {


		Window::Window()
			:_parameters()
		{

		}


		WindowParameters Window::GetParameters() const
		{
			return _parameters;
		}

#if defined(VK_USE_PLATFORM_WIN32_KHR)
#pragma region WIN32
#define INSTANCE_NAME "API without Secrets: Introduction to Vulkan"

		LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_SIZE:
			case WM_EXITSIZEMOVE:
				PostMessage(hWnd, WM_USER + 1, wParam, lParam);
				break;
			case WM_KEYDOWN:
			case WM_CLOSE:
				PostMessage(hWnd, WM_USER + 2, wParam, lParam);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			return 0;
		}

		Window::~Window()
		{
			if (_parameters.Handle)
			{
				DestroyWindow(_parameters.Handle);
			}
			if (_parameters.Instance)
			{
				UnregisterClass(INSTANCE_NAME, _parameters.Instance);
			}
		}

		bool Window::Create(const char * title)
		{
			_parameters.Instance = GetModuleHandle(nullptr);

			WNDCLASSEX wc;

			wc.cbSize = sizeof(WNDCLASSEX);

			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = WndProc;
			wc.cbClsExtra = NULL;
			wc.cbWndExtra = NULL;
			wc.hInstance = _parameters.Instance;
			wc.hIcon = NULL;
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wc.lpszMenuName = NULL;
			wc.lpszClassName = INSTANCE_NAME;
			wc.hIconSm = NULL;

			if (!RegisterClassEx(&wc))
			{
				DEBUG_ERROR("RegisterClassEx FAILED");
				return false;
			}

			_parameters.Handle = CreateWindow(
				INSTANCE_NAME,
				title,
				WS_OVERLAPPEDWINDOW,
				20, 20, 500, 500,
				nullptr,
				nullptr,
				_parameters.Instance,
				nullptr
			);

			if (_parameters.Handle == nullptr)
			{
				DEBUG_ERROR("Create Window Failed");
				return false;
			}
			return true;

		}

		bool Window::RenderingLoop(EngineerBase & engineer) const
		{
			ShowWindow(_parameters.Handle, SW_SHOWNORMAL);
			UpdateWindow(_parameters.Handle);
			bool res = true;
			bool loop = true;
			bool resize = false;
			MSG message;

			while (loop)
			{
				if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
				{
					switch (message.message)
					{
						// resize
					case WM_USER + 1:
						resize = true;
						break;
						// close
					case WM_USER + 2:
						loop = false;
						break;
					}

					TranslateMessage(&message);
					DispatchMessage(&message);
				}
				else
				{
					if (resize)
					{
						resize = false;
						if (engineer.OnWindowSizeChanged() != true)
						{
							res = false;
							break;
						}
					}
					if (engineer.ReadyToDraw())
					{
						if (engineer.Draw() != true)
						{
							res = false;
							break;
						}
					}
					else
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
					}
				}
			}

			return res;
		}


#pragma endregion

#elif defined(VK_USE_PLATFORM_XCB_KHR)


#elif defined(VK_USE_PLATFORM_XLIB_KHR)

#endif


		bool EngineerBase::LoadVulkanLibrary()
		{
		#if defined(VK_USE_PLATFORM_WIN32_KHR)
			_vulkanLibary = LoadLibrary("vulkan-1.dll");
		#elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR)
			VulkanLibary = dlopen("libvulkan.so.1", RTLD_NOW);
		#endif

			if (_vulkanLibary == nullptr)
			{
				std::cout << "Could not load Vulkan library!" << std::endl;
				return false;
			}

			return true;

		}

		bool EngineerBase::LoadExportedEntryPoints() {
		#if defined(VK_USE_PLATFORM_WIN32_KHR)
		#define LoadProcAddress GetProcAddress
		#elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR)
		#define LoadProcAddress dlsym
		#endif


#define VK_EXPORTED_FUNCTION( fun )                                                   \
			if( !(fun = (PFN_##fun)LoadProcAddress( _vulkanLibary, #fun )) ) {                \
			  std::cout << "Could not load exported function: " << #fun << "!" << std::endl;  \
			  return false;                                                                   \
			}

#include "ListOfFunctions.inl"

			return true;
		}

		bool EngineerBase::LoadGlobalLevelEntryPoints()
		{
#define VK_GLOBAL_LEVEL_FUNCTION( fun ) \
			if ( !(fun = (PFN_##fun)vkGetInstanceProcAddr(nullptr, #fun)) ) \
			{ \
				std::cout << "Could not load global level function:" << #fun << "!" << std::endl; \
				return false; \
			}\

#include "ListOfFunctions.inl"
			return true;
		}

		bool EngineerBase::LoadInstanceLevelEntryPoints()
		{
#define VK_INSTANCE_LEVEL_FUNCTION( fun ) \
			if( !(fun = (PFN_##fun)vkGetInstanceProcAddr( _vulkan.Instance, #fun )) )\
			{ \
				DEBUG_ERROR("Could not load isntacne level function:%s!",#fun); \
				return false; \
			} 

#include "ListOfFunctions.inl"
			return true;
		}



		bool EngineerBase::CreateDevice()
		{
			uint32_t num_devices = 0;
			if ( ( vkEnumeratePhysicalDevices( _vulkan.Instance, &num_devices, nullptr ) != VK_SUCCESS )
				|| ( num_devices == 0 )
				)
			{
				DEBUG_ERROR("Error occurred during physical devices enumeration!");
				return false;
			}

			std::vector<VkPhysicalDevice> physical_devices( num_devices );
			if (vkEnumeratePhysicalDevices(_vulkan.Instance, &num_devices, &physical_devices[0] ) != VK_SUCCESS)
			{
				DEBUG_ERROR("Error occured during physical devices enumeration path 2 !");
				return false;
			}

			VkPhysicalDevice selected_physical_device = VK_NULL_HANDLE;
			uint32_t selected_queue_family_index = UINT32_MAX;
			for (uint32_t i = 0; i < num_devices; i++)
			{
				if (CheckPhysicalDeviceProperties( physical_devices[i], selected_queue_family_index ))
				{
					selected_physical_device = physical_devices[i];
					break;
				}
			}

			if (selected_physical_device == VK_NULL_HANDLE)
			{
				DEBUG_ERROR("Could not select physical device based on the chosen properties!");
				return false;
			}

			std::vector<float> queue_priorities = { 1.f };

			VkDeviceQueueCreateInfo queue_create_info = 
			{
				VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
				nullptr,
				0,
				selected_queue_family_index,
				static_cast<uint32_t>(queue_priorities.size()),
				&queue_priorities[0]
			};

			VkDeviceCreateInfo device_create_info = 
			{
				VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
				nullptr,
				0,
				1,
				&queue_create_info,
				0,
				nullptr,
				0,
				nullptr,
				nullptr
			};

			if ( vkCreateDevice( selected_physical_device, &device_create_info, nullptr, &_vulkan.Device ) != VK_SUCCESS )
			{
				DEBUG_ERROR("Could not create Culkan device!");
				return false;
			}

			_vulkan.QueueFamilyIndex = selected_queue_family_index;
			return true;

		}

		bool EngineerBase::LoadDeviceLevelEntryPoints()
		{
#define VK_DEVICE_LEVEL_FUNCTION( fun ) \
			if( !(fun = (PFN_##fun)vkGetDeviceProcAddr(_vulkan.Device, #fun) ) )\
			{\
				DEBUG_ERROR("Could not load device level function: %s",#fun);\
				return false;\
			}

#include "ListOfFunctions.inl"
			return true;
		}


		bool EngineerBase::CheckPhysicalDeviceProperties( VkPhysicalDevice physical_device, uint32_t &queue_family_index )
		{
			VkPhysicalDeviceProperties	device_properties;
			VkPhysicalDeviceFeatures	device_features;

			vkGetPhysicalDeviceProperties	( physical_device, &device_properties );
			vkGetPhysicalDeviceFeatures		( physical_device, &device_features );

			uint32_t major_version = VK_VERSION_MAJOR( device_properties.apiVersion );
			uint32_t minor_version = VK_VERSION_MINOR( device_properties.apiVersion );
			uint32_t patch_version = VK_VERSION_PATCH( device_properties.apiVersion );

			DEBUG_LOG("major_version:[%d]", major_version);
			DEBUG_LOG("minor_version:[%d]", minor_version);
			DEBUG_LOG("patch_version:[%d]", patch_version);

			DEBUG_LOG("device_properties.limits.maxImageDimension2D:[%d]", device_properties.limits.maxImageDimension2D);

			if ( (major_version < 1)
				|| device_properties.limits.maxImageDimension2D < 4096
				)
			{
				std::cout << "Physical device " << physical_device << " doesn;t support required parameters !" << std::endl;
				return false;
			}

			uint32_t queue_families_count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, nullptr);
			DEBUG_LOG("queue_families_count:[%d]", queue_families_count);
			if ( queue_families_count == 0 )
			{
				std::cout << "Physical device " << physical_device << " doesn't have any queue families!" << std::endl;
				return false;
			}

			std::vector<VkQueueFamilyProperties> queue_family_properties( queue_families_count );
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, &queue_family_properties[0]);
			for (uint32_t i = 0; i < queue_families_count; i++)
			{
				DEBUG_LOG("queueCount[%d]", queue_family_properties[i].queueCount);
				DEBUG_LOG("queue_family_properties[i].queueFlags[%d]", queue_family_properties[i].queueFlags);
				if ( (queue_family_properties[i].queueCount > 0)
					&& (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
					)
				{
					queue_family_index = i;
					DEBUG_LOG("Selected device: %s",device_properties.deviceName);
					return true;
				}
			}

			std::cout << "Could not find queue family with required properties on physical device:" << physical_device << "!" << std::endl;
			return false;
		}

		bool EngineerBase::GetDeviceQueue()
		{
			vkGetDeviceQueue( _vulkan.Device, _vulkan.QueueFamilyIndex, 0, &_vulkan.Queue );
			return true;
		}

		EngineerBase::EngineerBase()
		:_vulkanLibary()
		, _vulkan()
		, canRender(false)
		{
		
		}

		EngineerBase::~EngineerBase()
		{
			if (_vulkan.Device != VK_NULL_HANDLE)
			{
				vkDeviceWaitIdle(_vulkan.Device);
				vkDestroyDevice( _vulkan.Device, nullptr );
			}

			if (_vulkan.Instance != VK_NULL_HANDLE)
			{
				vkDestroyInstance(_vulkan.Instance, nullptr);
			}

			if (_vulkanLibary) {
#if defined(VK_USE_PLATFORM_WIN32_KHR)
				FreeLibrary(_vulkanLibary);
#elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR)
				dlclose(_vulkanLibary);
#endif
			}
		}

	} // OS

} // FD