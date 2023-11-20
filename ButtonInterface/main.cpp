#include "fssimplewindow.h"
#include "MazeManager.h"

int main()
{
	FsOpenWindow(16, 16, 950, 800, 1, "Project");
	
	MazeManager Manager;

	
	while (Manager.manage()) {
			FsSwapBuffers();
			FsSleep(10);
		}




}