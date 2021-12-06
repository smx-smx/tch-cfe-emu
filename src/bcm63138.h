/*
 * Copyright (C) 2021 Stefano Moioli <smxdev4@gmail.com>
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#pragma once
#include "boot1.h"
#include "uart.h"
#include "soc.h"
#include "63138_map_part.h"
#include "jtagotp.h"
#include "armcfg.h"
#include "procmon.h"

class bcm63138 {
public:
	bcm63138(uc_engine *uc);
	int start();
	void dump_regs();
	void setTraceEnabled(bool value);
	uint32_t readReg(int regid);
	void writeReg(int regId, uint32_t value);
private:
	bool traceEnabled;
	void on_mem_unmapped(
		uc_engine *uc, uc_mem_type type,
		uint64_t address, int size, int64_t value, void *user_data
	);
	void mmapSoc();
	uc_hook traceHook;
	uc_hook unmappedHook;
	uc_engine *uc;
	
	soc soc;
	jtagotp jtagOtp;
	uart<UART0_PHYS_BASE> uart0;
	uart<UART1_PHYS_BASE> uart1;
	armcfg armcfg;
	procmon procmon;
};
