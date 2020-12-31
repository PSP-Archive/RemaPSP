#include "conf.h"

PSP_MODULE_INFO("WEBNAB", 0, 1, 1);


int main()
{
	struct ctrlSetup setup = readConfig("defaults.txt");
	return 0;
}
