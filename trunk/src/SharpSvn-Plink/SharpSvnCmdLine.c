
#include "SharpSvn-Plink.h"

#define cmdline_run_saved putty_cmdline_run_saved
#include "PuttySrc/cmdline.c"
#undef cmdline_run_saved

extern Config* sPlinkCurrentConfig = NULL;


void cmdline_run_saved(Config *cfg)
{
	putty_cmdline_run_saved(cfg);

	sPlinkCurrentConfig = cfg;
}


