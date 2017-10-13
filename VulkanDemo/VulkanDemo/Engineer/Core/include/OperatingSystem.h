#ifndef FD_OS
#define FD_OS

#define VK_USE_PLATFORM_WIN32_KHR true
#if defined(VK_USE_PLATFORM_WIN32_KHR)
#include <Windows.h>

#elif defined(VK_USE_PLATFORM_XCB_KHR)
#include <xcb/xcb.h>
#include <dlfcn.h>
#include <cstdlib>

#elif defined(VK_USE_PLATFORM_XLIB_KHR)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <dlfcn.h>
#include <cstdlib>

#endif

#include <cstring>
#include <iostream>
#include "loger.h"
#include "vulkan.h"

namespace FD {
	namespace OS {

		// ************************************************************ //
		// LibraryHandle                                                //
		//                                                              //
		// Dynamic Library OS dependent type                            //
		// ************************************************************ //
		// 
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		typedef HMODULE LibraryHandle;

#elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR)
		typedef void* LibraryHandle;

#endif


		struct  VK_Parameters
		{
			VkInstance	Instance;
			VkDevice	Device;
			uint32_t	QueueFamilyIndex;
			VkQueue		Queue;
			VK_Parameters()
				:Instance(VK_NULL_HANDLE)
				, Device(VK_NULL_HANDLE)
				, QueueFamilyIndex(0)
				, Queue(VK_NULL_HANDLE)
			{

			}
		};

		class EngineerBase
		{
		public:
			virtual bool OnWindowSizeChanged() = NULL;
			virtual bool Draw() = NULL;
			virtual bool ReadyToDraw() const final {
				return canRender;
			}
			virtual bool EngineerBase::LoadVulkanLibrary();
			virtual bool EngineerBase::LoadExportedEntryPoints();
			virtual bool LoadGlobalLevelEntryPoints();
			virtual bool CreateInstance() = NULL;
			virtual bool LoadInstanceLevelEntryPoints();
			virtual bool CreateDevice();
			virtual bool CheckPhysicalDeviceProperties(VkPhysicalDevice physical_decice, uint32_t &queue_family_index);
			virtual bool LoadDeviceLevelEntryPoints();
			virtual bool GetDeviceQueue();
			EngineerBase();
			virtual ~EngineerBase();
			

		protected:
			bool canRender;
			OS::LibraryHandle			_vulkanLibary;
			VK_Parameters				_vulkan;


		};

		struct WindowParameters
		{
#if defined(VK_USE_PLATFORM_WIN32_KHR)
			HINSTANCE           Instance;
			HWND                Handle;

			WindowParameters() :
				Instance(),
				Handle() {
			}
#elif defined(VK_USE_PLATFORM_XCB_KHR)
			xcb_connection_t   *Connection;
			xcb_window_t        Handle;

			WindowParameters() :
				Connection(),
				Handle() {
			}

#elif defined(VK_USE_PLATFORM_XLIB_KHR)
			Display            *DisplayPtr;
			Window              Handle;

			WindowParameters() :
				DisplayPtr(),
				Handle() {
			}
#endif
		};

		class Window
		{
		public:
			Window();
			~Window();

		public:
			bool Create(const char * title);
			bool RenderingLoop(EngineerBase & engineer) const;
			WindowParameters GetParameters() const;

		private:
			WindowParameters _parameters;
		};
	}
}

#endif // !FD_OS




