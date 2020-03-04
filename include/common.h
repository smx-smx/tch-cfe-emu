/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   common.h
 * Author: sm
 *
 * Created on 17 novembre 2019, 12.48
 */

#ifndef COMMON_H
#define COMMON_H

#define DEFINE_MEM_HANDLER_IMPL(name, uc, type, address, size, value, user_data) \
	name(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data)

#define DEFINE_MEM_HANDLER(name, uc, type, address, size, value, user_data) \
	bool DEFINE_MEM_HANDLER_IMPL(name, uc, type, address, size, value, user_data)

#endif /* COMMON_H */

