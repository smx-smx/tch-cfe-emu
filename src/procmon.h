/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   procmon.h
 * Author: sm
 *
 * Created on 27 novembre 2019, 22.53
 */

#ifndef PROCMON_H
#define PROCMON_H

#include "helpers.h"
#include "device.h"
#include "63138_map_part.h"

class procmon : device {
public:
	procmon(uc_engine *uc)
		: device("procmon"),
		  PMSSBMasterControl(uc, this){}
	virtual ~procmon(){}
private:
	DECL_MEMBER(uint32_t, procmon, PMSSBMasterControl, {
		return 0;
	}, {
		return value;
	});
	
	regMem<uint32_t, procmon, PROC_MON_BASE + 0x60, &PMSSBMasterControl_read, &PMSSBMasterControl_write> PMSSBMasterControl;
};

#endif /* PROCMON_H */

