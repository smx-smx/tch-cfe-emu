/* 
 * File:   helpers.h
 * Author: sm
 *
 * Created on 17 novembre 2019, 12.54
 */

#ifndef HELPERS_H
#define HELPERS_H

#include <functional>
#include <unicorn/unicorn.h>
#include <c++/9.2.0/type_traits>

#include "common.h"

#define HOOK_INSN(x) x, x + 4

//https://stackoverflow.com/a/9779391/11782802
template <typename T, T> struct proxy;
template <typename T, typename R, typename ...Args, R (T::*mf)(Args...)>
struct proxy<R (T::*)(Args...), mf>
{	
    static R call(T & obj, Args &&... args)
    {
        return (obj.*mf)(std::forward<Args>(args)...);
    }
};

template<typename T>
uc_err uc_write(uc_engine *uc, uint64_t address, T data){
	using Tptr = typename std::add_pointer<T>::type;
	Tptr dataPtr = &data;
	return uc_mem_write(uc, address, dataPtr, sizeof(data));
}

template<typename T>
uc_err uc_write(uc_engine *uc, uint64_t address, T data, size_t dataSize){
	return uc_mem_write(uc, address, &data, dataSize);
}

template<typename T>
using regOpRead = std::function<T(uint64_t address, size_t size)>;

template<typename T>
using regOpWrite = std::function<T(uint64_t address, size_t size, T value)>;

template <
	typename T,
	typename Tclass,
	uint64_t regAddr,
	T (Tclass::*regRead)(uint64_t address, size_t size),
	T (Tclass::*regWrite)(uint64_t address, size_t size, T value)
>
class regMem {
	public:
	regMem(uc_engine *uc, Tclass *instance) : uc(uc)
	{
		uc_cb_hookmem_t cb = [](
			uc_engine *uc, uc_mem_type type,
			uint64_t address, int size,
			int64_t value, void *user_data
		){
			void **args = reinterpret_cast<void **>(user_data);
			regMem *r = reinterpret_cast<regMem *>(args[0]);
			Tclass *c = reinterpret_cast<Tclass *>(args[1]);
			
			T newValue;
			switch(type){
				case UC_MEM_READ:
					newValue = r->callRead(*c, address, size);
					uc_write<T>(uc, address, newValue, size);
					break;
				case UC_MEM_WRITE:
					newValue = r->callWrite(*c, address, size, value);
					if(newValue != value){
						uc_write<T>(uc, address, newValue, size);
					}
					break;
				default:
					throw "Unhandled memory access";
			}
		};
		
		this->args[0] = this;
		this->args[1] = instance;
		
		uc_err err = uc_hook_add(
			uc,
			&this->hook, UC_HOOK_MEM_VALID,
			reinterpret_cast<void *>(cb),
			args,
			regAddr, regAddr + sizeof(T)
		);
		if(err != UC_ERR_OK){
			fprintf(stderr, "uc_hook_add() failed with %u (%s)\n", err, uc_strerror(err));
			std::terminate();
		}
	}
	
	~regMem(){
		uc_hook_del(uc, this->hook);
	}
	
	private:
		void *args[2];
		
		static T callRead(Tclass & obj, uint64_t address, size_t size){
			return (obj.*regRead)(address, size);
		}
		
		static T callWrite(Tclass & obj, uint64_t address, size_t size, T value){
			return (obj.*regWrite)(address, size, value);
		}
		
		uc_engine *uc;
		uc_hook hook;
};

#endif /* HELPERS_H */

