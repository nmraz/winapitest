#include "resource_cache.h"

namespace gfx {
namespace {



}  // namespace

void resource_cache::add(const resource_key* key, std::unique_ptr<cached_resource> res) {
  std::scoped_lock hold(entry_lock_);
  do_add(key, std::move(res));
}

void resource_cache::remove(const resource_key* key) {
  std::scoped_lock hold(entry_lock_);

  auto it = entries_.find(key);
  if (it != entries_.end()) {
    do_remove(it);
  }
}


void resource_cache::clear() {
  std::scoped_lock hold(entry_lock_);
  for (auto it = entries_.begin(); it != entries_.end(); it = do_remove(it)) {}
}

void resource_cache::purge_invalid() {
  std::scoped_lock hold(entry_lock_);

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

bool resource_cache::is_valid(const resource_key* key, const impl::cached_resource_impl& cached) {
  return key->ver_ == cached.ver;
}


void resource_cache::do_add(const resource_key* key, std::unique_ptr<cached_resource> res) {
  entries_[key] = { std::move(res), key->ver_ };
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