#pragma once

#include "base/assert.h"
#include "ui/gfx/geom/point.h"
#include "ui/gfx/pixel_format.h"

namespace gfx {

int compute_pitch(int total_size, int height, pixel_format fmt);
int pixel_offset(int x, int y, int pitch, pixel_format fmt);


template<typename Bmp>
color pixel_at(const Bmp& bmp, const pointi& pt) {
  ASSERT(pt.x() < bmp.pixel_size().width() && pt.y() < bmp.pixel_size().height()) << "Point out of range";
  
  int offset = pixel_offset(pt.x(), pt.y(), bmp.pitch(), bmp.info().format());
  return read_pixel(bmp.pixels().data() + offset, bmp.info().format(), bmp.info().alpha());
}

template<typename Bmp>
void set_pixel_at(Bmp& bmp, const pointi& pt, const color& col) {
  ASSERT(pt.x() < bmp.pixel_size().width() && pt.y() < bmp.pixel_size().height()) << "Point out of range";

  int offset = pixel_offset(pt.x(), pt.y(), bmp.pitch(), bmp.info().format());
  write_pixel(bmp.pixels().data() + offset, col, bmp.info().format(), bmp.info().alpha());
}

}  // namespace gfx