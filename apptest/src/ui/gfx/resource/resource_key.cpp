#include "resource_key.h"

#include <algorithm>

namespace gfx {

resource_key::~resource_key() {
  // ignore changes to holders_ from now on
  std::vector<holder*> tmp_holders;
  {
    std::lock_guard hold(holder_lock_);
    tmp_holders.swap(holders_);
  }

  for (holder* h : tmp_holders) {
    h->key_destroyed(this);
  }
}

void resource_key::add_holder(holder* h) {
  std::lock_guard hold(holder_lock_);
  holders_.push_back(h);
}

void resource_key::remove_holder(holder* h) {
  std::lock_guard hold(holder_lock_);
  holders_.erase(
    std::remove(holders_.begin(), holders_.end(), h),
    holders_.end()
  );
}

}  // namespace gfx