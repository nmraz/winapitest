#pragma once

#include "ui/gfx/resource/cached_resource.h"
#include "ui/gfx/resource/resource_key.h"
#include <memory>
#include <mutex>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace gfx {

class resource_cache {
public:
  ~resource_cache() { clear(); }

  void add(const resource_key* key, std::unique_ptr<cached_resource> res);
  void remove(const resource_key* key);

  void clear();
  void purge_invalid();

  template<typename Res>
  Res* find(const resource_key* key);

  template<typename F>
  auto find_or_create(const resource_key* key, F&& factory);

private:
  friend class resource_key;

  using key_list = std::vector<const resource_key*>;
  using entry_map = std::unordered_map<const resource_key*, std::unique_ptr<cached_resource>>;
  using entry_iter = entry_map::iterator;

  void do_add(const resource_key* key, std::unique_ptr<cached_resource> res);
  entry_iter do_remove(entry_iter it);
  
  cached_resource* do_find(const resource_key* key);
  void do_purge_invalid();

  void invalidate(const resource_key* key);

  entry_map entries_;
  std::mutex entry_lock_;  // protects entries_

  key_list invalid_keys_;
  std::mutex invalid_key_lock_;  // protects invalid_keys_
};


template<typename Res>
Res* resource_cache::find(const resource_key* key) {
  std::scoped_lock hold(entry_lock_);
  do_purge_invalid();

  return static_cast<Res*>(do_find(key));
}

template<typename F>
auto resource_cache::find_or_create(const resource_key* key, F&& factory) {
  using res_type = typename std::invoke_result_t<F>::element_type;
  
  std::scoped_lock hold(entry_lock_);
  do_purge_invalid();

  if (cached_resource* res = do_find(key)) {
    return static_cast<res_type*>(res);
  }

  auto res_holder = std::forward<F>(factory)();
  res_type* res = res_holder.get();

  do_add(key, std::move(res_holder));
  return res;
}

}  // namespace gfx