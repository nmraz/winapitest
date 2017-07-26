#include "scoped_co_init.h"

#include <objbase.h>
#include <stdexcept>

namespace base::win {

scoped_co_init::scoped_co_init(bool mta) {
	COINIT init_flags = mta ? COINIT_MULTITHREADED : COINIT_APARTMENTTHREADED;
	HRESULT ret = ::CoInitializeEx(nullptr, init_flags);
	if (!SUCCEEDED(ret)) {
		throw std::runtime_error("Failed to initialize COM");
	}
}

scoped_co_init::~scoped_co_init() {
	::CoUninitialize();
}

}  // namepsace base::win