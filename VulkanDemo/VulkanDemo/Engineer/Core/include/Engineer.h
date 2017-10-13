#ifndef FD_ENGINEER
#define FD_ENGINEER

#include "vulkan.h"
#include "OperatingSystem.h"

namespace FD {


//#define TRY_CALL( func,ret ) if( func() != true) { DEBUG_ERROR("#func call failed!"); return ret; }
#define TRY_CALL( func,ret,...) if( func(__VA_ARGS__) != true) { DEBUG_ERROR("%s call failed!",#func); return ret; }

	class Engineer : public FD::OS::EngineerBase
	{
	public:
		Engineer();
		virtual ~Engineer();
#pragma region Override

		bool OnWindowSizeChanged() override;
		bool Draw() override;
		
#pragma endregion
	public:
		bool PrepareVulkan();

	private:
		virtual bool CreateInstance();


	};

} // FD
#endif