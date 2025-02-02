#include "factories.h"

#include "base/win/last_error.h"
#include <mutex>

namespace gfx::impl {

const base::win::com_ptr<ID2D1Factory1>& get_d2d_factory() {
  static base::win::com_ptr<ID2D1Factory1> factory;
  static std::once_flag guard;

  std::call_once(guard, [&] {
    D2D1_FACTORY_OPTIONS opts = { D2D1_DEBUG_LEVEL_NONE };

#ifndef NDEBUG
    opts.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

    base::win::throw_if_failed(::D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, opts, factory.addr()),
      "Failed to create Direct2D factory");
  });

  return factory;
}

const base::win::com_ptr<IDWriteFactory>& get_dwrite_factory() {
  static base::win::com_ptr<IDWriteFactory> factory;
  static std::once_flag guard;

  std::call_once(guard, [&] {
    base::win::throw_if_failed(::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), factory.unknown_addr()),
      "Failed to create DirectWrite factory");
  });

  return factory;
}

}  // namespace gfx::impl