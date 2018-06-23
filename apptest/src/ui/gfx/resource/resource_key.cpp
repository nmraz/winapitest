#include "resource_key.h"

#include "ui/gfx/resource/resource_cache.h"
#include <algorithm>

namespace gfx {

resource_key::~resource_key() {
  // ignore changes to owning_caches_ from now on
  std::vector<resource_cache*> tmp_owning_caches;
  {
    std::lock_guard hold(owning_cache_lock_);
    tmp_owning_caches.swap(owning_caches_);
  }

  for (resource_cache* cache : tmp_owning_caches) {
    cache->remove(this);
  }
}


// PRIVATE

void resource_key::add_owning_cache(resource_cache* h) {
  std::lock_guard hold(owning_cache_lock_);
  owning_caches_.push_back(h);
}

void resource_key::remove_owning_cache(resource_cache* h) {
  std::lock_guard hold(owning_cache_lock_);
  owning_caches_.erase(
    std::remove(owning_caches_.begin(), owning_caches_.end(), h),
    owning_caches_.end()
  );
}

}  // namespace gfx