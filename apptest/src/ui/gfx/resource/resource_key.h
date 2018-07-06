#pragma once

#include "base/non_copyable.h"
#include "ui/gfx/resource/cached_resource.h"
#include <mutex>
#include <vector>

namespace gfx {

class resource_cache;

enum class resource_version : unsigned int {};


class resource_key : public base::non_copy_movable {
public:
  ~resource_key();

  void invalidate();

private:
  friend class resource_cache;

  void add_owning_cache(resource_cache* cache) const;
  void remove_owning_cache(resource_cache* cache) const;

  resource_version ver_{ 0 };

  mutable std::vector<resource_cache*> owning_caches_;
  mutable std::mutex owning_cache_lock_;  // protects owning_caches_
};

}  // namespace gfx