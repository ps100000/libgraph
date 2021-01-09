#include "context.h"
#include <iostream>  
void context::ctx_begin(){}

void context::ctx_end(){}

void context::use(std::function<void()> lambda){
	ctx_begin();
	lambda();
	ctx_end();
}

void context::use(std::vector<std::reference_wrapper<context>> ctxs, std::function<void()> lambda){
	for (std::vector<std::reference_wrapper<context>>::iterator ctx = ctxs.begin(); ctx != ctxs.end(); ctx++){
		(*ctx).get().ctx_begin();
	}
	lambda();
	for (std::vector<std::reference_wrapper<context>>::iterator ctx = ctxs.end() - 1; ctx != ctxs.begin(); ctx--){
		(*ctx).get().ctx_end();
	}
}

/**************************************************************/

generic_context::generic_context(
	std::function<void()> begin,
	std::function<void()> end): begin_func(begin), end_func(end){}

void generic_context::ctx_begin(){
	begin_func();
}

void generic_context::ctx_end(){
	end_func();
}
