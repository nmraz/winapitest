#include "resource_key.h"

#include "ui/gfx/resource/resource_cache.h"
#include <algorithm>
#include <type_traits>

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

void resource_key::invalidate() {
  using resource_version_und_t = std::underlying_type_t<resource_version>;

  ver_ = static_cast<resource_version>(static_cast<resource_version_und_t>(ver_) + 1);
}


// PRIVATE

void resource_key::add_owning_cache(resource_cache* cache) const {
  std::lock_guard hold(owning_cache_lock_);
  owning_caches_.push_back(cache);
}

void resource_key::remove_owning_cache(resource_cache* cache) const {
  std::lock_guard hold(owning_cache_lock_);
  owning_caches_.erase(
    std::remove(owning_caches_.begin(), owning_caches_.end(), cache),
    owning_caches_.end()
  );
}

}  // namespace gfx