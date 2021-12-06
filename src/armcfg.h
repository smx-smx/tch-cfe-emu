/*
 * Copyright (C) 2021 Stefano Moioli <smxdev4@gmail.com>
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
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

