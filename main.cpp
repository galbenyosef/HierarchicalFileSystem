
#include "myFolder.h"
#include "myFile.h"
#include "myTerminal.h"

int main(){

	string home = "V";
	myTerminal virtual_system(home);
	virtual_system.start();
    return 0;

}
