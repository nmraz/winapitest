#pragma once

#include "ui/gfx/resource/cached_resource.h"
#include "ui/gfx/resource/resource_key.h"
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace gfx {

class resource_cache {
public:
  ~resource_cache() { clear(); }

  template<typename F>
  auto find_or_create(const resource_key& key, F&& factory);
  void remove(const resource_key& key);

  void clear();
  void purge_invalid();

private:
  friend class resource_key;

  using key_list = std::vector<const resource_key*>;
  using entry_map = std::unordered_map<const resource_key*, std::unique_ptr<cached_resource>>;
  using entry_iter = entry_map::iterator;

  void add(const resource_key& key, std::unique_ptr<cached_resource> res);
  cached_resource* find(const resource_key& key);
  
  entry_iter do_remove(entry_iter it);
  void do_purge_invalid();

  void invalidate(const resource_key* key);

  entry_map entries_;
  std::mutex entry_lock_;  // protects entries_

  key_list invalid_keys_;
  std::mutex invalid_key_lock_;  // protects invalid_keys_
};


template<typename F>
auto resource_cache::find_or_create(const resource_key& key, F&& factory) {
  using res_type = typename std::invoke_result_t<F>::element_type;
  
  // Use double-checked locking - safe here as exclusive locks always exclude shared ones.

  {
    std::shared_lock hold_key(key.lock_);
    if (cached_resource* res = find(key)) {
      return static_cast<res_type*>(res);
    }
  }

  {
    std::scoped_lock hold_key(key.lock_);

    if (cached_resource* res = find(key)) {
      return static_cast<res_type*>(res);
    }

    auto res_holder = std::forward<F>(factory)();
    auto* res = res_holder.get();

    add(key, std::move(res_holder));
    return res;
  }
}

}  // namespace gfx