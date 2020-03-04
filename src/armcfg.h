/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   armcfg.h
 * Author: sm
 *
 * Created on 25 novembre 2019, 23.45
 */

#ifndef ARMCFG_H
#define ARMCFG_H

#include "63138_map_part.h"
#include "device.h"


class armcfg : device {
public:
	armcfg(uc_engine *uc) :
		device("armcfg"),
		pllarma(uc, this),
		policy_ctl(uc, this)
	{
	}
	virtual ~armcfg(){
	}
private:
	DECL_MEMBER(uint32_t, armcfg, pllarma, {
		return 0xFF;
	}, {
		return value;
	});
	
	DECL_MEMBER(uint32_t, armcfg, policy_ctl, {
		return 0;
	}, {
		return value;
	});
	
	regMem<uint32_t, armcfg, ARMCFG_PHYS_BASE + 0xC00, &pllarma_read, &pllarma_write> pllarma;
	regMem<uint32_t, armcfg, ARMCFG_PHYS_BASE + 0xC, &policy_ctl_read, &policy_ctl_write> policy_ctl;
};

#endif /* ARMCFG_H */

