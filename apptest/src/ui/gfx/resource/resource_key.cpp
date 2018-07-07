#include "resource_key.h"

#include "ui/gfx/resource/resource_cache.h"
#include <algorithm>
#include <type_traits>

namespace gfx {

resource_key::~resource_key() {
  invalidate();
}

void resource_key::invalidate() {
  std::vector<resource_cache*> tmp_owning_caches;
  {
    std::scoped_lock hold(owning_cache_lock_);
    tmp_owning_caches.swap(owning_caches_);
  }

  for (resource_cache* cache : tmp_owning_caches) {
    cache->invalidate(this);
  }
}


// PRIVATE

void resource_key::add_owning_cache(resource_cache* cache) const {
  std::scoped_lock hold(owning_cache_lock_);
  owning_caches_.push_back(cache);
}

void resource_key::remove_owning_cache(resource_cache* cache) const {
  std::scoped_lock hold(owning_cache_lock_);
  owning_caches_.erase(
    std::remove(owning_caches_.begin(), owning_caches_.end(), cache),
    owning_caches_.end()
  );
}

}  // namespace gfx