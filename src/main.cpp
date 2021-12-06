/*
 * Copyright (C) 2021 Stefano Moioli <smxdev4@gmail.com>
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unicorn/unicorn.h>
#include <unistd.h>

#define BOOT3_DATA_START 0x00F527DC

#include "63138_cpu.h"
#include "63138_intr.h"
#include "63138_map_part.h"
#include "bcm63138.h"
#include "cfe.h"


static const uint32_t NOP = 0x00000000;
static const uint32_t BX_LR = 0x1eff2fe1;

static void dump_regs(uc_engine *uc){
	uint32_t r[16];
	uc_reg_read(uc, UC_ARM_REG_R0, &r[0]);
	uc_reg_read(uc, UC_ARM_REG_R1, &r[1]);
	uc_reg_read(uc, UC_ARM_REG_R2, &r[2]);
	uc_reg_read(uc, UC_ARM_REG_R3, &r[3]);
	uc_reg_read(uc, UC_ARM_REG_R4, &r[4]);
	uc_reg_read(uc, UC_ARM_REG_R5, &r[5]);
	uc_reg_read(uc, UC_ARM_REG_R6, &r[6]);
	uc_reg_read(uc, UC_ARM_REG_R7, &r[7]);
	uc_reg_read(uc, UC_ARM_REG_R8, &r[8]);
	uc_reg_read(uc, UC_ARM_REG_R9, &r[9]);
	uc_reg_read(uc, UC_ARM_REG_R10, &r[10]);
	uc_reg_read(uc, UC_ARM_REG_R11, &r[11]);
	uc_reg_read(uc, UC_ARM_REG_R12, &r[12]);
	uc_reg_read(uc, UC_ARM_REG_R13, &r[13]);
	uc_reg_read(uc, UC_ARM_REG_R14, &r[14]);
	uc_reg_read(uc, UC_ARM_REG_R15, &r[15]);
	
	uint32_t cpsr;
	uc_reg_read(uc, UC_ARM_REG_CPSR, &cpsr);
	
	uint32_t sp, ip, pc, lr;
	uc_reg_read(uc, UC_ARM_REG_SP, &sp);
	uc_reg_read(uc, UC_ARM_REG_IP, &ip);
	uc_reg_read(uc, UC_ARM_REG_PC, &pc);
	uc_reg_read(uc, UC_ARM_REG_LR, &lr);
	
	printf(
		"r0: 0x%08X\n"
		"r1: 0x%08X\n"
		"r2: 0x%08X\n"
		"r3: 0x%08X\n"
		"r4: 0x%08X\n"
		"r5: 0x%08X\n"
		"r6: 0x%08X\n"
		"r7: 0x%08X\n"
		"r8: 0x%08X\n"
		"r9: 0x%08X\n"
		"r10: 0x%08X\n"
		"r11: 0x%08X\n"
		"r12: 0x%08X\n" //also ip
		"r13: 0x%08X\n"
		"r14: 0x%08X\n"
		"r15: 0x%08X\n"
		"cpsr: 0x%08X\n"
		"sp: 0x%08X\n"
		"lr: 0x%08X\n"
		"pc: 0x%08X\n"
		"last_insn: 0x%08X\n",
		r[0],r[1],r[2],r[3],r[4],r[5],r[6],r[7],r[8],r[9],
		r[10],r[11],r[12],r[13],r[14],r[15],
		cpsr,sp,lr,pc,
		pc - 16
	);
}

static void on_mem_unmapped(
	uc_engine *uc, uc_mem_type type,
    uint64_t address, int size, int64_t value, void *user_data
){
	switch(type){
		case UC_MEM_READ_UNMAPPED:
			fprintf(stderr, "UNMAPPED read [%d] %p\n", size, value);
			dump_regs(uc);
			break;
		case UC_MEM_WRITE_UNMAPPED:
			fprintf(stderr, "UNMAPPED write [%d] %p => %p\n", size, value, address);
			dump_regs(uc);
			break;
		case UC_MEM_FETCH_UNMAPPED:
			fprintf(stderr, "UNMAPPED fetch [%d] %p\n", size, address);
			dump_regs(uc);
			break;
	}
}

static void hook_block(uc_engine *uc, uint64_t address, uint32_t size, void *user_data)
{
    printf(">>> basic block 0x%llx, size = 0x%08X\n", address, size);
}

static void my_xprinthook(uc_engine *uc, uint64_t address, uint32_t size, void *user_data){
	uint32_t r0;
	uc_reg_read(uc, UC_ARM_REG_R3, &r0);
	
	char str[255];
	printf("Reading string @0x%08X\n", r0);
	uc_mem_read(uc, r0, &str, sizeof(str));
	
	puts(str);
}

static void on_console_write(uc_engine *uc, uint64_t address, uint32_t size, void *user_data){
	puts("print");
	uint32_t r1, r2;
	uc_reg_read(uc, UC_ARM_REG_R1, &r1);
	uc_reg_read(uc, UC_ARM_REG_R2, &r2);
	
	//r1 = string pointer
	//r2 = string length
	
	puts("reached");
	
	char str[r2];
	uc_mem_read(uc, r1, &str, r2);	
	puts(str);
}

static void on_nand_read_buf(uc_engine *uc, uint64_t address, uint32_t size, void *user_data){
	uint32_t r0, r1, r2, r3;
	uc_reg_read(uc, UC_ARM_REG_R0, &r0);
	uc_reg_read(uc, UC_ARM_REG_R1, &r1);
	uc_reg_read(uc, UC_ARM_REG_R2, &r2);
	uc_reg_read(uc, UC_ARM_REG_R3, &r3);
	
	printf("nand_read blk:%d, offset:%d\n", r0, r1);
}

#define mem_write_all(uc, address, bytes, size) \
	uc_mem_write(uc, address, bytes, size); \
	uc_mem_write(uc, address - BOOT3_BASE, bytes, size)

#define hook_all(uc, type, callback, user_data, begin, end) { \
	uc_hook uh1, uh2; \
	uc_hook_add(uc, &uh1, type, callback, user_data, begin, end); \
	uc_hook_add(uc, &uh2, type, callback, user_data, begin - BOOT3_BASE, end - BOOT3_BASE)

static void trace(uc_engine *uc, uint64_t address){
	printf("code 0x%08X\n", address);
	
	/*if(address == 0x00F2B3AC){
		uint32_t r12;
		uc_reg_read(uc, UC_ARM_REG_R3, &r12);
		printf("R12: 0x%08X\n", r12);
		sleep(9000);
	}*/
}

int do_emu(
	const char *boot1Path,
	const char *boot3Path,
	const char *nandPath,
	const char *eckKeyPath,
	bool traceEnabled
){
	uc_arch arch = UC_ARCH_ARM;
	uc_mode mode = (uc_mode)(UC_MODE_ARM | UC_MODE_LITTLE_ENDIAN);
	
	uc_engine *uc;
	uc_err err = uc_open(arch, mode, &uc);
	if(err != UC_ERR_OK){
		fprintf(stderr, "uc_open() failed with %u\n", err);
		return EXIT_FAILURE;
	}
		
	bcm63138 soc(uc);
	soc.setTraceEnabled(traceEnabled);
	
	cfe cfeEmu(uc, soc, nandPath);
	
	cfeEmu.loadBtrmParams("0x8073F000.bin");
	
	puts("=> Loading boot1...");
	cfeEmu.loadBoot1(boot1Path);
	
	puts("=> Loading boot3...");
	cfeEmu.loadBoot3(boot3Path);
	
	cfeEmu.setMarketId(0xFFFC);

	puts("=> Loading ECK key...");
	cfeEmu.loadEckKey(eckKeyPath);
	
	puts("=> Starting...");
	cfeEmu.start();
	
	
#if 0	
	boot1_mmap(uc, boot1Path);
	soc_mmap(uc);
	
	{ // hook accesses to unmapped memory
		uc_hook uh;
		uc_hook_add(uc, &uh, UC_HOOK_MEM_UNMAPPED, on_mem_unmapped, NULL, 0, 0xFFFFFFFF);
	}
	
	{ // tracing all basic blocks with customized callback
		uc_hook block;
		//uc_hook_add(uc, &block, UC_HOOK_BLOCK, hook_block, NULL, 1, 0);
	}

	err = uc_emu_start(uc, BOOT1_BASE, 0xFFFFFFFF, 0, 0);
	if(err){
		fprintf(stderr, "uc_emu_start() failed with %u (%s)\n", err, uc_strerror(err));
		//return EXIT_FAILURE;
	}
	
	puts("Emulation Finished");
	dump_regs(uc);
#endif
	return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
	if(argc < 5){
		fprintf(stderr, "Usage: %s [boot1.bin][boot3.bin][nand.bin][eckKey.bin] [[-d]]\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	bool traceEnabled = false;
	if(argc > 5){
		traceEnabled = strcmp(argv[5], "-d") == 0;
	}
	
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
		
	do_emu(argv[1], argv[2], argv[3], argv[4], traceEnabled);
	return EXIT_SUCCESS;
}

