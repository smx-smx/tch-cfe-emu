#pragma once
#include <unicorn/unicorn.h>



class boot1 {
public:
	boot1(uc_engine *uc, const char *boot1Path);
private:
	uc_engine *uc;

};