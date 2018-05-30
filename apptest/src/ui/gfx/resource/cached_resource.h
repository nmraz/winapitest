#pragma once

namespace gfx {

enum class resource_version : unsigned int {};

class cached_resource {
public:
  cached_resource(resource_version ver) : ver_(ver) {}
  resource_version version() const { return ver_; }

  virtual ~cached_resource() = 0 {}

private:
  const resource_version ver_;
};

}  // namespace gfx