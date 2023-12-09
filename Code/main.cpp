#include "fssimplewindow.h"
#include "Manager.h"
#include "model.h"
#include "Trainer.h"

int VisualLayer::nextId = 0;


int main()
{
	FsOpenWindow(16, 16, 945, 750, 1, "Project");
	
	Manager Manager;
	ArialFont arial;

	
	while (Manager.manage()) {
			
			FsSwapBuffers();
			FsSleep(10);
		}




}