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