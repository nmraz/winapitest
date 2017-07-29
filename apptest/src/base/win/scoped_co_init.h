#pragma once

#include "base/non_copyable.h"

namespace base::win {

class scoped_co_init : public non_copy_movable {
public:
	explicit scoped_co_init(bool mta = false);
	~scoped_co_init();
};

}  // namepsace base::win