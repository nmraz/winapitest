#pragma once

namespace base::win {

class scoped_co_init {
public:
	scoped_co_init(bool mta = false);
	~scoped_co_init();
};

}  // namepsace base::win