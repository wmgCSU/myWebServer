#pragma once
#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__

#include<thread>

namespace sylar {
	unsigned long long GetThreadId();
	uint32_t GetFiberId();
}

#endif // !__SYLAR_UTIL_H__
