#include "fssimplewindow.h"
#include "Manager.h"

int VisualLayer::nextId = 0;

int main()
{
	FsOpenWindow(16, 16, 945, 700, 1, "Project");
	
	Manager Manager;

	
	while (Manager.manage()) {
			FsSwapBuffers();
			FsSleep(10);
		}




}