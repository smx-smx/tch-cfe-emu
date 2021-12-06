/*
 * Copyright (C) 2021 Stefano Moioli <smxdev4@gmail.com>
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
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

