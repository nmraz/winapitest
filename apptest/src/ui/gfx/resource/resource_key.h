#pragma once

#include "base/non_copyable.h"
#include "ui/gfx/resource/cached_resource.h"
#include <mutex>
#include <vector>

namespace gfx {

class resource_cache;

class resource_key : public base::non_copy_movable {
public:
  ~resource_key();

  resource_version version() const { return ver_; }
  void invalidate() {
    ver_ = static_cast<resource_version>(static_cast<unsigned int>(ver_) + 1);
  }

private:
  friend class resource_cache;

  void add_owning_cache(resource_cache* cache);
  void remove_owning_cache(resource_cache* cache);

  resource_version ver_{ 0 };

  std::vector<resource_cache*> owning_caches_;
  std::mutex owning_cache_lock_;  // protects owning_caches_
};

}  // namespace gfx