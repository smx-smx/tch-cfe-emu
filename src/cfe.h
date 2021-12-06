/*
 * Copyright (C) 2021 Stefano Moioli <smxdev4@gmail.com>
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#pragma once
#include "bcm63138.h"

#define BOOT1_BASE 0x80710000
#define BOOT3_BASE 0x00f00000

#include <fstream>

class cfe {
public:
	cfe(uc_engine *uc, bcm63138& emu, const char *nandPath);
	void loadBtrmParams(const char *paramsPath);
	void loadBoot1(const char *boot1Path);
	void loadBoot3(const char *boot3Path);
	void setMarketId(uint16_t marketId);
	void loadEckKey(const char *eckKeyPath);
	void setEckKey(uint8_t *eck);
	int start();
	virtual ~cfe();
private:
	uint8_t eck[16];
	std::ifstream nandFile;
	void rip2_drv_read(uint32_t rpSize, uint16_t itemId, uint32_t rpBuf);
	void rip2_crypto_process(uint32_t rpData, uint32_t rpLength, uint32_t cryptoAttr, uint16_t itemId);
	void nandReadPage(uint32_t startAddr, uint32_t buffer, uint32_t length);
	void patchBoot3(uint32_t baseAddr);
	void patchReturn(uint32_t codeAddr, uint16_t retVal);
	uc_engine *uc;
	bcm63138 emu;
};