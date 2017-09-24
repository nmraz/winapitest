#include "factories.h"

#include "base/win/last_error.h"

namespace gfx::impl {

const base::win::com_ptr<ID2D1Factory1>& get_d2d_factory() {
  static base::win::com_ptr<ID2D1Factory1> factory;
  if (!factory) {
    base::win::throw_if_failed(::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, factory.addr()),
      "Failed to create Direct2D factory");
  }
  return factory;
}

const base::win::com_ptr<IDWriteFactory>& get_dwrite_factory() {
  static base::win::com_ptr<IDWriteFactory> factory;
  if (!factory) {
    base::win::throw_if_failed(::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), factory.unknown_addr()),
      "Failed to create DirectWrite factory");
  }
  return factory;
}

}  // namespace gfx::impl