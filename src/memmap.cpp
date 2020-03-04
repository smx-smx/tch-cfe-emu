#include <unicorn/unicorn.h>
#include <exception>

#include "memmap.h"

memmap::memmap(uc_engine *uc, uint64_t address, size_t size, uint32_t perms) {
	uc_err err = uc_mem_map(uc, address, size, perms);
	if(err != UC_ERR_OK){
		fprintf(stderr, "uc_mem_map(0x%08X, 0x%x) failed with %u (%s)\n", address, size, err, uc_strerror(err));
		std::terminate();
	}
}

memmap::memmap(uc_engine *uc, uint64_t address, size_t size) :
	memmap(uc, address, size, UC_PROT_READ | UC_PROT_WRITE)
{
}

memmap::~memmap() {
	uc_mem_unmap(uc, address, size);
}

