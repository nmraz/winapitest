#include "scoped_co_init.h"

#include "base/win/last_error.h"
#include <objbase.h>

namespace base::win {

scoped_co_init::scoped_co_init(bool mta) {
	COINIT init_flags = mta ? COINIT_MULTITHREADED : COINIT_APARTMENTTHREADED;
	throw_if_failed(::CoInitializeEx(nullptr, init_flags));
}

scoped_co_init::~scoped_co_init() {
	::CoUninitialize();
}

}  // namepsace base::win