/*
 * Copyright (C) 2021 Stefano Moioli <smxdev4@gmail.com>
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#pragma once
#include <stdio.h>

#include "common.h"
#include "helpers.h"

#include "63138_map_part.h"
#include "memmap.h"

#include "device.h"

#define TXFIFOTHOLD     0x0008

#define UART_DATA 0x14
#define UART_STS 0x10

// for getche()
#include <conio.h>

template<uint32_t baseAddr>
class uart : device {
public:
	uart(uc_engine *uc) :
		device("uart"),
		uartReg4(uc, this),
		uartData(uc, this),
		uartSts(uc, this),
		uartFifoCtl(uc, this)
	{
	}
	virtual ~uart(){
	}
private:	
	uint8_t uart_data_read(uint64_t address, size_t size){
		int ch = getche();
		return (uint8_t)ch;
	}
	
	uint8_t uart_data_write(uint64_t address, size_t size, uint8_t value){
		putchar(value);
		return value;
	}
	
	uint16_t uart_sts_read(uint64_t address, size_t size){	
		return RXFIFONE | TXFIFOEMT;
	}
	
	uint16_t uart_sts_write(uint64_t address, size_t size, uint16_t value){
		return value;
	}
	
	DECL_MEMBER(uint32_t, uart, fifoctl, {
		return fifoctl_val;
	}, {
		fifoctl_val = value;
		return value;
	});
	
	uint32_t fifoctl_val;
		
	regMem<uint32_t, uart, baseAddr + 0x0, &fifoctl_read, &fifoctl_write> uartFifoCtl;
	regMem<uint32_t, device, baseAddr + 0x4, &dev_read32, &dev_write32> uartReg4;
	regMem<uint8_t, uart, baseAddr + UART_DATA, &uart_data_read, &uart_data_write> uartData;
	regMem<uint16_t, uart, baseAddr + UART_STS, &uart_sts_read, &uart_sts_write> uartSts;
};
