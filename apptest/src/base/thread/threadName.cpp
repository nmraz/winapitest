#include "threadName.h"

#include <utility>
#include <Windows.h>

namespace base {
namespace {

thread_local std::string gCurrentName = "thr" + std::to_string(::GetCurrentThreadId());


// Adapted from https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx

constexpr DWORD threadNameException = 0x406D1388;

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO {
	DWORD dwType; // Must be 0x1000.  
	LPCSTR szName; // Pointer to name (in user addr space).  
	DWORD dwThreadID; // Thread ID (-1=caller thread).  
	DWORD dwFlags; // Reserved for future use, must be zero.  
} THREADNAME_INFO;
#pragma pack(pop)  

void setDebuggerThreadName(const char* name) {
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = name;
	info.dwThreadID = -1;
	info.dwFlags = 0;

#pragma warning(push)
#pragma warning(disable: 6320 6322) // constant EXCEPTION_EXECUTE_HANDLER, empty __except block
	__try {
		::RaiseException(threadNameException, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR*>(&info));
	} __except(EXCEPTION_EXECUTE_HANDLER) {}
#pragma warning(pop)
}

}  // namespace

void setCurrentThreadName(std::string name) {
	setDebuggerThreadName(name.c_str());
	gCurrentName = std::move(name);
}

const std::string& getCurrentThreadName() {
	return gCurrentName;
}

}  // namespace base