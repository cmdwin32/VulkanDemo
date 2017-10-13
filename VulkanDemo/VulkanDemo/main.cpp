#include "Engineer.h"
#include <cstdlib>

bool Run()
{

	FD::OS::Window window;
	FD::Engineer engineer;

	TRY_CALL(window.Create, -1, "Vulkan demo");
	TRY_CALL(engineer.PrepareVulkan, -1);
	TRY_CALL(window.RenderingLoop, -1, engineer);

	return 0;
}

int main(int argc, char ** argv)
{
	int res = Run();
	std::system("Pause");

	return res;
}