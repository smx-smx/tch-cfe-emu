/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   memc.h
 * Author: sm
 *
 * Created on 25 novembre 2019, 23.40
 */

#ifndef MEMC_H
#define MEMC_H

#include "device.h"


class memc : device {
public:
	memc();
	virtual ~memc();
private:
	//regMem<uint8_t, memc, MEMC_PHYS_BASE + MEMC_CO
};

#endif /* MEMC_H */

