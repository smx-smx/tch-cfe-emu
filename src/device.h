/*
 * Copyright (C) 2021 Stefano Moioli <smxdev4@gmail.com>
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#define DECL_READ_MEMBER(type, klass, name) \
	type name(uint64_t address, size_t size)
	
#define DECL_WRITE_MEMBER(type, klass, name) \
	type name(uint64_t address, size_t size, type value)

#define REGN(name) name ## Reg
#define REGN_READ(name) name ## _read
#define REGN_WRITE(name) name ## _write

#define DECL_MEMBER(type, klass, name, read, write) \
	DECL_READ_MEMBER(type, klass, REGN_READ(name)) read \
	DECL_WRITE_MEMBER(type, klass, REGN_WRITE(name)) write

#define MAKE_REGMEM(type, addr, klass, name) \
	regMem<type, klass, addr, &REGN_READ(name), &REGN_WRITE(name)> REGN(name)

#define REGMEM_INIT(name, uc) REGN(name) (uc, this)


class device {
private:
	const char *deviceName;
public:
	device(const char *deviceName) : deviceName(deviceName){
		
	}
	uint8_t dev_read8(uint64_t address, size_t size){
		printf(">>> %s read8 0x%08X [%d]\n", deviceName, address, size);
		return 0x0;
	}
	uint8_t dev_write8(uint64_t address, size_t size, uint8_t value){
		printf(">>> %s write8 0x%08X => 0x%08X [%d]\n", deviceName, value, address, size);
		return value;
	}
	uint16_t dev_read16(uint64_t address, size_t size){
		printf(">>> %s read16 0x%08X [%d]\n", deviceName, address, size);
		return 0x0;
	}
	uint16_t dev_write16(uint64_t address, size_t size, uint16_t value){
		printf(">>> %s write16 0x%08X => 0x%08X [%d]\n", deviceName, value, address, size);
		return value;
	}
	uint32_t dev_read32(uint64_t address, size_t size){
		printf(">>> %s read32 0x%08X [%d]\n", deviceName, address, size);
		return 0x0;
	}
	uint32_t dev_write32(uint64_t address, size_t size, uint32_t value){
		printf(">>> %s write32 0x%08X => 0x%08X [%d]\n", deviceName, value, address, size);
		return value;
	}
};