#include"util.h"
#include<sstream>
namespace sylar {
	unsigned long long GetThreadId() {
		std::ostringstream oss;
		oss << std::this_thread::get_id();
		std::string stid = oss.str();
		unsigned long long tid = std::stoull(stid);
		return tid;
	}
	uint32_t GetFiberId() {
		return 0;
	}
}