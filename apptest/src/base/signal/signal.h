#pragma once

#include "base/assert.h"
#include "base/auto_restore.h"
#include "base/finally.h"
#include "base/function.h"
#include "base/non_copyable.h"
#include "base/signal/slot_handle.h"
#include <algorithm>
#include <utility>
#include <vector>

namespace base {
namespace impl {

struct slot_rep_base {
  virtual void disconnect() = 0;
  virtual void block(bool block) = 0;
  virtual bool blocked() const = 0;
};

}  // namespace impl

template<typename... Args>
class signal : public non_copy_movable {
public:
  using slot_type = function<void(Args...)>;

  slot_handle connect(slot_type slot);
  void operator()(Args... args);

private:
  friend slot_handle;

  struct slot_rep : impl::slot_rep_base {
    slot_rep(signal* signal, slot_type slot)
      : signal_(signal)
      , slot_(std::move(slot)) {
    }

    void disconnect() override { signal_->disconnect(this); }
    void block(bool block) override { blocked_ = block; }
    bool blocked() const override { return blocked_; }

    void call(Args&... args) const { if (!blocked_) slot_(args...); }

    signal* signal_;
    slot_type slot_;
    bool blocked_ = false;
  };

  using slot_ptr = std::shared_ptr<slot_rep>;
  using slot_list = std::vector<slot_ptr>;

  void disconnect(slot_rep* rep);
  void tidy();  // removes elements marked for removal, if safe

  slot_list slots_;
  // number of nested emissions - when 0, it is safe to remove elements directly
  int emit_depth_ = 0;
};


template<typename... Args>
slot_handle signal<Args...>::connect(slot_type slot) {
  auto rep = std::make_shared<slot_rep>(this, std::move(slot));
  slots_.push_back(rep);
  return slot_handle(rep);
}


template<typename... Args>
void signal<Args...>::operator()(Args... args) {
  auto call_tidy = finally([this] { tidy(); });
  {
    auto_restore<int> restore(emit_depth_);
    ++emit_depth_;

    // iterators may be invalidated, use indices instead
    for (std::size_t i = 0; i < slots_.size(); ++i) {
      const auto& slot = slots_[i];
      if (slot) {
        slot->call(args...);
      }
    }
  }
}


// PRIVATE

template<typename... Args>
void signal<Args...>::disconnect(slot_rep* rep) {
  auto it = std::find_if(slots_.begin(), slots_.end(),
    [&](const auto& cur) {
      return cur.get() == rep;
    }
  );
  ASSERT(it != slots_.end()) << "Corrupt signal state";

  if (!emit_depth_) {
    slots_.erase(it);
  } else {
    *it = nullptr;
  }
}

template<typename... Args>
void signal<Args...>::tidy() {
  if (!emit_depth_) {
    slots_.erase(std::remove(slots_.begin(), slots_.end(), nullptr), slots_.end());
  }
}

}  // namepsace base