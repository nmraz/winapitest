#pragma once

#include "ui/gfx/resource/cached_resource.h"
#include "ui/gfx/resource/resource_key.h"
#include <memory>
#include <mutex>
#include <unordered_map>

namespace gfx {

class resource_cache : private resource_key::holder {
public:
  ~resource_cache() { clear(); }

  void add(resource_key* key, std::unique_ptr<cached_resource> res);
  void remove(resource_key* key);

  void clear();
  void purge_invalid();

  template<typename Res>
  Res* find(resource_key* key);

private:
  using entry_map = std::unordered_map<resource_key*, std::unique_ptr<cached_resource>>;
  using entry_iter = entry_map::iterator;

  void do_add(resource_key* key, std::unique_ptr<cached_resource> res);
  entry_iter do_remove(entry_iter it);
  cached_resource* do_find(resource_key* key);

  void key_destroyed(resource_key* key) override { remove(key); }

  entry_map entries_;
  std::mutex lock_;
};


template<typename Res>
Res* resource_cache::find(resource_key* key) {
  std::lock_guard hold(lock_);
  return static_cast<Res*>(do_find(key));
}

}  // namespace gfx