#include "resource_cache.h"

namespace gfx {

void resource_cache::add(const resource_key* key, std::unique_ptr<cached_resource> res) {
  // We don't need exclusive access here, we just want exclusion from `find_or_create`,
  // which locks in exclusive mode anyway.
  std::shared_lock hold_key(key->resource_lock_);
  std::scoped_lock hold_entries(entry_lock_);
  
  do_purge_invalid();
  do_add(key, std::move(res));
}

void resource_cache::remove(const resource_key* key) {
  std::scoped_lock hold(entry_lock_);
  do_purge_invalid();

  auto it = entries_.find(key);
  if (it != entries_.end()) {
    do_remove(it);
  }
}


void resource_cache::clear() {
  std::scoped_lock hold(entry_lock_);
  do_purge_invalid();
  
  for (auto it = entries_.begin(); it != entries_.end(); it = do_remove(it)) {}
}

void resource_cache::purge_invalid() {
  std::scoped_lock hold(entry_lock_);
  do_purge_invalid();
}


// PRIVATE

void resource_cache::do_add(const resource_key* key, std::unique_ptr<cached_resource> res) {
  entries_.emplace(key, std::move(res));
  key->add_owning_cache(this);
}

resource_cache::entry_iter resource_cache::do_remove(entry_iter it) {
  it->first->remove_owning_cache(this);
  return entries_.erase(it);
}


cached_resource* resource_cache::do_find(const resource_key* key) {
  do_purge_invalid();

  auto it = entries_.find(key);
  if (it == entries_.end()) {
    return nullptr;
  }
  return it->second.get();
}

void resource_cache::do_purge_invalid() {
  key_list tmp_invalid_keys;
  {
    std::scoped_lock hold(invalid_key_lock_);
    invalid_keys_.swap(tmp_invalid_keys);
  }

  for (const resource_key* key : tmp_invalid_keys) {
    entries_.erase(key);
  }
}


void resource_cache::invalidate(const resource_key* key) {
  std::scoped_lock hold(invalid_key_lock_);
  invalid_keys_.push_back(key);
}

}  // namespace gfx