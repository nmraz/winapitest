#pragma once

#include "base/non_copyable.h"
#include "ui/gfx/resource/cached_resource.h"
#include <mutex>
#include <vector>

namespace gfx {

class resource_key : public base::non_copy_movable {
public:
  struct holder {
    virtual void key_destroyed(resource_key* res) = 0;
  };

  resource_version version() const { return ver_; }
  void invalidate() { ver_++; }

  void add_holder(holder* h);
  void remove_holder(holder* h);

private:
  resource_version ver_ = 0;

  std::vector<holder*> holders_;
  std::mutex holder_lock_;  // protects holders_
};

}  // namespace gfx