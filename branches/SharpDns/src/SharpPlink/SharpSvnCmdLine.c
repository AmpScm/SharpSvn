
#include "SharpPlink.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#define cmdline_run_saved putty_cmdline_run_saved
#include "PuttySrc/cmdline.c"
#undef cmdline_run_saved

#pragma warning(pop)

extern Config* sPlinkCurrentConfig = NULL;


void cmdline_run_saved(Config *cfg)
{
	putty_cmdline_run_saved(cfg);

	sPlinkCurrentConfig = cfg;
}


