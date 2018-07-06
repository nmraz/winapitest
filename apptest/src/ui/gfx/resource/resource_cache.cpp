#include "resource_cache.h"

namespace gfx {
namespace {

bool is_valid(const resource_key* key, const impl::cached_resource_impl& cached) {
  return key->version() == cached.ver;
}

}  // namespace

void resource_cache::add(const resource_key* key, std::unique_ptr<cached_resource> res) {
  std::scoped_lock hold(lock_);
  do_add(key, std::move(res));
}

void resource_cache::remove(const resource_key* key) {
  std::scoped_lock hold(lock_);

  auto it = entries_.find(key);
  if (it != entries_.end()) {
    do_remove(it);
  }
}


void resource_cache::clear() {
  std::scoped_lock hold(lock_);
  for (auto it = entries_.begin(); it != entries_.end(); it = do_remove(it)) {}
}

void resource_cache::purge_invalid() {
  std::scoped_lock hold(lock_);

  auto it = entries_.begin();
  while (it != entries_.end()) {
    const auto& [key, cached] = *it;

    if (!is_valid(key, cached)) {
      it = do_remove(it);
    } else {
      it++;
    }
  }
}


// PRIVATE

void resource_cache::do_add(const resource_key* key, std::unique_ptr<cached_resource> res) {
  entries_[key] = { std::move(res), key->version() };
  key->add_owning_cache(this);
}

resource_cache::entry_iter resource_cache::do_remove(entry_iter it) {
  it->first->remove_owning_cache(this);
  return entries_.erase(it);
}

cached_resource* resource_cache::do_find(const resource_key* key) {
  auto it = entries_.find(key);
  if (it == entries_.end()) {
    return nullptr;
  }

  const impl::cached_resource_impl& cached = it->second;
  if (!is_valid(key, cached)) {
    do_remove(it);
    return nullptr;
  }
  return cached.res.get();
}

}  // namespace gfx