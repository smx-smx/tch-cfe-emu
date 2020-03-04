#pragma once

#include <unicorn/unicorn.h>


struct memmap {
public:
	memmap(uc_engine *uc, uint64_t address, size_t size);
	memmap(uc_engine *uc, uint64_t address, size_t size, uint32_t perms);
	virtual ~memmap();
private:
	uc_engine *uc;
	uint64_t address;
	size_t size;
};