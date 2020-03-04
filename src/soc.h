#pragma once
#include <unicorn/unicorn.h>

#include "memmap.h"

#define DRAM_SIZE (512 * 1024 * 1024)

class soc {
public:
	soc(uc_engine *uc);
	virtual ~soc();
private:
	memmap dram;
	memmap dsllmem;
	memmap regsLow;
	memmap regsHigh;
	uc_engine *uc;
};

