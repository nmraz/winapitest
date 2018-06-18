#include "bitmap.h"

#include "ui/gfx/util.h"

namespace gfx {

sizef bitmap::size() const {
  return {
    px_to_dip(pixel_size().width(), info().dpix()),
    px_to_dip(pixel_size().height(), info().dpiy())
  };
}

int bitmap::pitch() const {
  return compute_pitch(static_cast<int>(pixels().size()), pixel_size().height(), info().format());
}

}  // namespace gfx