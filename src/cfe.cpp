#include "cfe.h"

#include <iostream>
#include <fstream>
#include <cstring>

#include "helpers.h"
#include "rip_ids.h"

cfe::cfe(uc_engine *uc, bcm63138& emu, const char *nandPath):
	uc(uc),
	emu(emu),
	nandFile(nandPath, std::ios::binary)
{
}

cfe::~cfe() {
	nandFile.close();
}

void cfe::loadBtrmParams(const char *paramsPath){
	std::ifstream binFile(paramsPath, std::ios::binary);
	
	std::vector<char> bytes(
         (std::istreambuf_iterator<char>(binFile)),
         (std::istreambuf_iterator<char>()));
	
	binFile.close();
	if(uc_mem_write(uc, 0x8073F000, bytes.data(), bytes.size())){
		fprintf(stderr, "uc_mem_write() failed\n");
		std::terminate();
	}
}

void cfe::loadBoot1(const char *boot1Path){
	std::ifstream binFile(boot1Path, std::ios::binary);
	
	std::vector<char> bytes(
         (std::istreambuf_iterator<char>(binFile)),
         (std::istreambuf_iterator<char>()));

	binFile.close();
	if(uc_mem_write(uc, BOOT1_BASE, bytes.data(), bytes.size())){
		fprintf(stderr, "uc_mem_write() failed\n");
		std::terminate();
	}
}

#define NAND_PAGESZ 512

void cfe::nandReadPage(uint32_t startAddr, uint32_t buffer, uint32_t length){
	printf("nandRead %08X (%d) -> %08X\n", startAddr, length, buffer);
	nandFile.seekg(startAddr, std::ios::beg);
	
	char locBuf[length];	
	nandFile.read(locBuf, length);
	int read = nandFile.gcount();	
	uc_mem_write(uc, buffer, locBuf, read);
}

void cfe::setEckKey(uint8_t *pEck){
	std::memcpy(this->eck, pEck, sizeof(this->eck));
}

void cfe::rip2_drv_read(uint32_t pSize, uint16_t itemId, uint32_t pBuf){
	printf(">>> rip2_drv_read: 0x%x\n", itemId);
}

void cfe::rip2_crypto_process(uint32_t rpData, uint32_t rpLength, uint32_t cryptoAttr, uint16_t itemId){
	if(cryptoAttr & 0x4000000){
		// mcv
		printf(">>> requires mcv: 0x%x\n", itemId);
		
		// remove MCV signing requirement
		cryptoAttr = cryptoAttr & ~0x4000000;
		this->emu.writeReg(UC_ARM_REG_R2, cryptoAttr);
		
		switch(itemId){
			case RIP_ID_EIK:
				printf(">>> setting EIK key length\n");
				// set eik key length
				uint32_t length = 256;
				uc_mem_write(uc, rpLength, &length, sizeof(length));
				break;
		}
	}
	
	if(cryptoAttr & 0x2000000){
		// bek
		printf(">>> requires bek: 0x%x\n", itemId);
		
		switch(itemId){
			// set eck key
			case RIP_ID_ECK:
				uint32_t length = 16;
				uc_mem_write(uc, rpLength, &length, sizeof(length));
				
				printf(">>> overwriting ECK key\n");
				for(int i=0; i<16; i++){
					printf("%02X ", this->eck[i]);
				}
				puts("");
				
				uc_mem_write(uc, rpData, this->eck, sizeof(this->eck));
				uint32_t lr = this->emu.readReg(UC_ARM_REG_LR);
				this->emu.writeReg(UC_ARM_REG_R0, 1);
				this->emu.writeReg(UC_ARM_REG_PC, lr);
				break;
		}
	}
}

void cfe::patchReturn(uint32_t codePtr, uint16_t retVal){
	// R0 = 0
	uint32_t MOV_R0_CONST = ((0xe3a0) << 16) | retVal;
	uint32_t EOR_R0_R0 = 0xe0200000;
	// return
	uint32_t BX_LR = 0xe12fff1e;
	
	uc_write<uint32_t>(uc, codePtr + 0, MOV_R0_CONST);
	uc_write<uint32_t>(uc, codePtr + 4, BX_LR);
}

void cfe::patchBoot3(uint32_t baseAddr){
	uint32_t INSN_NOP = 0x0;	
	uint32_t BX_ENABLEMMU = 0x00F00134; //0x00F00134;
	
	uc_write<uint32_t>(uc, BX_ENABLEMMU, INSN_NOP);
	
	// nandflash_wait_status
	this->patchReturn(0x00F1CEF8, 1);
	// nandflash_read_page
	this->patchReturn(0x00F1D5A4, 1);
	// nandflash_block_erase
	this->patchReturn(0x00F1DC48, 1);
	// initTbbt
	this->patchReturn(0xF24F40, 1);
	
	// BpGetEthernetMacInfoArrayPtr stub
	this->patchReturn(0xF4458C, 0);
	
	// sha256 final (i think)
	//this->patchReturn(0xF41A60, 0);
	
	// install nandflash_read_page hook
	{
		uc_cb_hookcode_t cb = [](
			uc_engine *uc, uint64_t address,
			uint32_t size, void *user_data
		){
			cfe *c = reinterpret_cast<cfe *>(user_data);
			bcm63138& emu = c->emu;

			uint32_t startAddr = emu.readReg(UC_ARM_REG_R1);
			uint32_t buffer = emu.readReg(UC_ARM_REG_R2);
			uint32_t length = emu.readReg(UC_ARM_REG_R3);
			c->nandReadPage(startAddr, buffer, length);
		};
		uc_hook uh_nand_read_page;
		uc_hook_add(uc, &uh_nand_read_page, UC_HOOK_CODE,
			reinterpret_cast<void *>(cb),
			reinterpret_cast<void *>(this),
			HOOK_INSN(0x0F1D5A4)
		);
	}
	
#if 0
	// install rip2_drv_read hook
	{
		uc_cb_hookcode_t cb = [](
			uc_engine *uc, uint64_t address,
			uint32_t size, void *user_data
		){
			cfe *c = reinterpret_cast<cfe *>(user_data);
			bcm63138& emu = c->emu;

			uint32_t pSize = emu.readReg(UC_ARM_REG_R0);
			uint32_t itemId = emu.readReg(UC_ARM_REG_R1);
			uint32_t pBuf = emu.readReg(UC_ARM_REG_R2);
			c->rip2_drv_read(pSize, itemId, pBuf);
		};
		uc_hook uh_rip2_drv_read;
		uc_hook_add(uc, &uh_rip2_drv_read, UC_HOOK_CODE,
			reinterpret_cast<void *>(cb),
			reinterpret_cast<void *>(this),
			HOOK_INSN(0x00F01A40)
		);
	}
#endif
	
	// install rip2_crypto_process hook
	{
		uc_cb_hookcode_t cb = [](
			uc_engine *uc, uint64_t address,
			uint32_t size, void *user_data
		){
			cfe *c = reinterpret_cast<cfe *>(user_data);
			bcm63138& emu = c->emu;
			
			uint32_t rpData = emu.readReg(UC_ARM_REG_R0);
			uint32_t rpLength = emu.readReg(UC_ARM_REG_R1);
			uint32_t cryptoAttr = emu.readReg(UC_ARM_REG_R2);
			uint32_t ripId = emu.readReg(UC_ARM_REG_R3);
			c->rip2_crypto_process(rpData, rpLength, cryptoAttr, ripId);
		};
		uc_hook uh_rip2_crypto_process;
		uc_hook_add(uc, &uh_rip2_crypto_process, UC_HOOK_CODE,
			reinterpret_cast<void *>(cb),
			reinterpret_cast<void *>(this),
			HOOK_INSN(0x00F02594)
		);
	}
	
	// patch rip2_drv_read
	/*{
		uc_cb_hookcode_t cb = [](
			uc_engine *uc, uint64_t address,
			uint32_t size, void *user_data
		){
			cfe *c = reinterpret_cast<cfe *>(user_data);
			bcm63138& emu = c->emu;
		};
		uc_hook uh_rip2_drv_read
	}*/
	
}

void cfe::loadEckKey(const char *keyPath){
	std::ifstream binFile(keyPath, std::ios::binary);
	
	std::vector<char> bytes(
         (std::istreambuf_iterator<char>(binFile)),
         (std::istreambuf_iterator<char>()));

	this->setEckKey((uint8_t *)bytes.data());

	binFile.close();
}

void cfe::loadBoot3(const char *boot3Path){
	std::ifstream binFile(boot3Path, std::ios::binary);
	
	std::vector<char> bytes(
		(std::istreambuf_iterator<char>(binFile)),
		(std::istreambuf_iterator<char>()));
	
	printf("=> writing boot3 (0x%08X bytes)\n", bytes.size());
	if(uc_mem_write(uc, BOOT3_BASE, bytes.data(), bytes.size())){
		fprintf(stderr, "uc_mem_write() failed\n");
		std::terminate();
	}
	patchBoot3(BOOT3_BASE);
	binFile.close();
}

void cfe::setMarketId(uint16_t marketId){
	uc_write<uint16_t>(uc, NANDFLASH_PHYS_BASE + 0x22, __builtin_bswap16(marketId));
}

int cfe::start(){
	return emu.start();
}

