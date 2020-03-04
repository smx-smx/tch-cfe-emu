#include <exception>

#include "soc.h"

#define _CFE_
#include "63138_map_part.h"
#include "63138_common.h"

soc::soc(uc_engine *uc) :
	uc(uc),
	dram(uc, 0x00000000, DRAM_SIZE, UC_PROT_ALL),
	dsllmem(uc, DSLLMEM_PHYS_BASE, 0xF900000, UC_PROT_ALL),
	regsLow(uc, REG_BASE, DSLLMEM_PHYS_BASE - REG_BASE),
	regsHigh(uc, SPIFLASH_PHYS_BASE, 0x300000)
{
}

soc::~soc() {
}

