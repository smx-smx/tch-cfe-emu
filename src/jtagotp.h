/*
 * Copyright (C) 2021 Stefano Moioli <smxdev4@gmail.com>
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#include "helpers.h"
#include "63138_map_part.h"
#include "device.h"

#include <map>

class jtagotp : device {
public:
	DECL_MEMBER(uint8_t, jtagotp, status, {
		return 0xF;
	}, {
		return value;
	})

	DECL_MEMBER(uint32_t, jtagotp, ctrl, {
		return ctrl;
	}, {
		this->ctrl = value;
		return value;
	})
	
	DECL_MEMBER(uint32_t, jtagotp, rowsel, {
		return row;
	}, {
		printf(">>> jtagotp: rowsel [%d]\n", value);
		this->row = value;
		return value;
	})
	
	DECL_MEMBER(uint32_t, jtagotp, data, {
		printf(">>> jtagotp: data read\n");
		return 0x0;
	}, {
		printf(">>> jtagotp: data write[0x%08X] to row [%d]\n", value, row);
		rowDataMap[row] = value;
		return value;
	})
	
	jtagotp(uc_engine *uc) : 
		device("jtagotp"),
		//ctrlReg(uc, this),
		REGMEM_INIT(ctrl, uc),
		dataReg(uc, this),
		rowselReg(uc, this),
		statusReg(uc, this){}
	virtual ~jtagotp(){}
private:
	uint32_t ctrl;
	uint32_t row;
	
	std::map<uint32_t, uint32_t> rowDataMap;
	
	MAKE_REGMEM(uint32_t, JTAG_OTP_PHYS_BASE + 0x4, jtagotp, ctrl);
	
	//regMem<uint32_t, jtagotp, JTAG_OTP_PHYS_BASE + 0x4, &jtagotp_ctrl1_read, &jtagotp_ctrl1_write> ctrl1Reg;
	//regMem<uint32_t, jtagotp, JTAG_OTP_PHYS_BASE + 0x4, &ctrl_read, &ctrl_write> ctrlReg;
	regMem<uint32_t, jtagotp, JTAG_OTP_PHYS_BASE + 0x8, &data_read, &data_write> dataReg;
	regMem<uint32_t, jtagotp, JTAG_OTP_PHYS_BASE + 0xC, &rowsel_read, &rowsel_write> rowselReg;
	regMem<uint8_t, jtagotp, JTAG_OTP_PHYS_BASE + 0x18, &status_read, &status_write> statusReg;
};
