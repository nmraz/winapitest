#include "resource_cache.h"

#include "base/assert.h"

namespace gfx {

void resource_cache::remove(const resource_key* key) {
  // Don't be tempted to lock `key->lock_` later as an optimization - deadlock
  // could result due to different locking order compared with `find_or_create`.
  std::scoped_lock hold(entry_lock_, key->lock_);
  do_purge_invalid();

  auto it = entries_.find(key);
  if (it != entries_.end()) {
    do_remove(it);
  }
}


void resource_cache::clear() {
  std::scoped_lock hold(entry_lock_);
  do_purge_invalid();
  
  for (auto it = entries_.begin(); it != entries_.end();) {
    std::scoped_lock hold_key(it->first->lock_);
    it = do_remove(it);
  }
}

void resource_cache::purge_invalid() {
  std::scoped_lock hold(entry_lock_);
  do_purge_invalid();
}


// PRIVATE

void resource_cache::add(const resource_key* key, std::unique_ptr<cached_resource> res) {
  std::scoped_lock hold(entry_lock_);
  ASSERT(entries_.find(key) == entries_.end()) << "Adding key twice";

  entries_.emplace(key, std::move(res));
  key->add_owning_cache(this);
}

resource_cache::entry_iter resource_cache::do_remove(entry_iter it) {
  it->first->remove_owning_cache(this);
  return entries_.erase(it);
}


cached_resource* resource_cache::find(const resource_key* key) {
  std::scoped_lock hold(entry_lock_);
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