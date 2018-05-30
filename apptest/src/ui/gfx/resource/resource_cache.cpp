#include "resource_cache.h"

namespace gfx {
namespace {

bool is_valid(resource_key* key, cached_resource* res) {
  return key->version() == res->version();
}

}  // namespace

void resource_cache::add(resource_key* key, std::unique_ptr<cached_resource> res) {
  std::lock_guard hold(lock_);
  do_add(key, std::move(res));
}

void resource_cache::remove(resource_key* key) {
  std::lock_guard hold(lock_);

  auto it = entries_.find(key);
  if (it != entries_.end()) {
    do_remove(it);
  }
}


void resource_cache::clear() {
  std::lock_guard hold(lock_);
  for (auto it = entries_.begin(); it != entries_.end(); it = do_remove(it)) {}
}

void resource_cache::purge_invalid() {
  std::lock_guard hold(lock_);

  auto it = entries_.begin();
  while (it != entries_.end()) {
    const auto& [key, res] = *it;

    if (!is_valid(key, res.get())) {
      it = do_remove(it);
    } else {
      it++;
    }
  }
}


// PRIVATE

void resource_cache::do_add(resource_key* key, std::unique_ptr<cached_resource> res) {
  entries_.insert_or_assign(key, std::move(res));
  key->add_holder(this);
}

resource_cache::entry_iter resource_cache::do_remove(entry_iter it) {
  it->first->remove_holder(this);
  return entries_.erase(it);
}

cached_resource* resource_cache::do_find(resource_key* key) {
  auto it = entries_.find(key);
  if (it == entries_.end()) {
    return nullptr;
  }

  cached_resource* res = it->second.get();
  if (!is_valid(key, res)) {
    do_remove(it);
    return nullptr;
  }
  return res;
}

}  // namespace gfx