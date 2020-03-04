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
