/*
 * Copyright (C) 2021 Stefano Moioli <smxdev4@gmail.com>
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#include <unicorn/unicorn.h>
#include "63138_map_part.h"

#include "bcm63138.h"
#include "cfe.h"

void bcm63138::dump_regs(){
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
		"pc: 0x%08X\n",
		r[0],r[1],r[2],r[3],r[4],r[5],r[6],r[7],r[8],r[9],
		r[10],r[11],r[12],r[13],r[14],r[15],
		cpsr,sp,lr,pc
	);
}

uint32_t bcm63138::readReg(int regId){
	uint32_t value;
	uc_reg_read(uc, regId, &value);
	return value;
}

void bcm63138::writeReg(int regId, uint32_t value){
	uc_reg_write(uc, regId, &value);
}

int bcm63138::start(){
	//uc_err err = uc_emu_start(this->uc, BOOT1_BASE, (uint64_t)-1, 0, 0);
	uc_err err = uc_emu_start(this->uc, BOOT3_BASE, (uint64_t)-1, 0, 0);
	if(err){
		fprintf(stderr, "uc_emu_start() failed with %u (%s)\n", err, uc_strerror(err));
		return EXIT_FAILURE;
	}
	puts("Emulation Finished");
	this->dump_regs();
	
	return EXIT_SUCCESS;
}

void bcm63138::on_mem_unmapped(
	uc_engine* uc, uc_mem_type type, uint64_t address,
	int size, int64_t value, void* user_data
){
	switch(type){
		case UC_MEM_READ_UNMAPPED:
			fprintf(stderr, "UNMAPPED read [%d] %p\n", size, value);
			this->dump_regs();
			break;
		case UC_MEM_WRITE_UNMAPPED:
			fprintf(stderr, "UNMAPPED write [%d] %p => %p\n", size, value, address);
			this->dump_regs();
			break;
		case UC_MEM_FETCH_UNMAPPED:
			fprintf(stderr, "UNMAPPED fetch [%d] %p\n", size, address);
			this->dump_regs();
			break;
	}
}

void bcm63138::setTraceEnabled(bool value){
	if(value == false)
		return;
	
	if(this->traceEnabled){
		uc_hook_del(uc, traceHook);
		return;
	}
	
	uc_cb_hookcode_t cb = [](
		uc_engine *uc, uint64_t address,
		uint32_t size, void *user_data
	){
		printf(">>> basic block 0x%llx, size = 0x%08X\n", address, size);
	};
	
	uc_hook_add(
		uc, &traceHook, UC_HOOK_BLOCK,
		reinterpret_cast<void *>(cb),
		nullptr, 1, 0
	);
}

bcm63138::bcm63138(
	uc_engine *uc
):	uc(uc),
	soc(uc),
	uart0(uc),
	uart1(uc),
	jtagOtp(uc),
	armcfg(uc),
	procmon(uc),
	traceEnabled(false)
{

	uc_cb_hookmem_t cb = [](
		uc_engine *uc, uc_mem_type type,
		uint64_t address, int size,
		int64_t value, void *user_data
	){
		bcm63138 *c = reinterpret_cast<bcm63138 *>(user_data);
		c->on_mem_unmapped(uc, type, address, size, value, user_data);
	};
	
	// hook accesses to unmapped memory
	uc_hook_add(uc, &this->unmappedHook, UC_HOOK_MEM_UNMAPPED,
		reinterpret_cast<void *>(cb),
		reinterpret_cast<void *>(this),
		0, 0xFFFFFFFF
	);
	
	/*
	uc_cb_hookmem_t cb2 = [](
		uc_engine *uc, uc_mem_type type,
		uint64_t address, int size,
		int64_t value, void *user_data
	){
		uint32_t r12, r3;
		uc_reg_read(uc, UC_ARM_REG_R3, &r3);
		uc_reg_read(uc, UC_ARM_REG_R12, &r12);
		
		uint32_t theTest;
		uc_mem_read(uc, 0x00F577A4, &theTest, sizeof(theTest));
		
		printf("%08X %08X\n", r3, r12);
		printf("rep 0x%08X\n", theTest);
	};
	uc_hook uh;
	uc_hook_add(uc, &uh, UC_HOOK_CODE,
		reinterpret_cast<void *>(cb2),
		reinterpret_cast<void *>(this),
		0x00F2B384, 0x00F2B3BC + 4
	);*/
}