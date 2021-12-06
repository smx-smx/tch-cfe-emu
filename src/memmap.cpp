/*
 * Copyright (C) 2021 Stefano Moioli <smxdev4@gmail.com>
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

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

