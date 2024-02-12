#include "costmap/costmap_2d.h"
#include "costmap/costmap_2d_settings.h"

#include <cstdint>
#include <cstring>
#include <mutex>

namespace map {
namespace costmap_2d {

Costmap2D::Costmap2D(const Costmap2DSettings &settings)
    : settings_(settings), costmap_(nullptr), access_(nullptr) {
  access_ = new std::mutex();
  init_maps(settings_.cells_size_x_, settings_.cells_size_y_);
  reset_maps();
}

Costmap2D::Costmap2D(const Costmap2D &map) : costmap_(nullptr) {
  access_ = new std::mutex();
  *this = map;
}

Costmap2D &Costmap2D::operator=(const Costmap2D &map) {
  // check for self-assignment
  if (this == &map) {
    return *this;
  }

  // clean up old data
  delete_maps();

  settings_ = map.settings_;

  init_maps(settings_.cells_size_x_, settings_.cells_size_y_);

  // copy the costmap
  std::uint32_t size_x = settings_.cells_size_x_;
  std::uint32_t size_y = settings_.cells_size_y_;
  {
    std::lock_guard<std::mutex> lock(*access_);
    memcpy(costmap_.get(), map.costmap_.get(),
           size_x * size_y * sizeof(unsigned char));
  }

  return *this;
}

Costmap2D::Costmap2D() : costmap_(nullptr) {
  settings_ = Costmap2DSettings::default_settings();
  access_ = new std::mutex();
}

Costmap2D::~Costmap2D() {
  delete_maps();
  if (access_) {
    delete access_;
    access_ = nullptr;
  }
}

unsigned char Costmap2D::get_cost(std::uint32_t mx, std::uint32_t my) {
  return costmap_.get()[get_index(mx, my)];
}

void Costmap2D::set_cost(std::uint32_t mx, std::uint32_t my,
                         unsigned char cost) {
  costmap_.get()[get_index(mx, my)] = cost;
}

void Costmap2D::init_maps(std::uint32_t size_x, std::uint32_t size_y) {
  std::lock_guard<std::mutex> lock(*access_);
  if (costmap_) {
    costmap_.reset();
  }
  settings_.cells_size_x_ = size_x;
  settings_.cells_size_y_ = size_y;
  costmap_ = std::make_shared<unsigned char[]>(size_x * size_y);
}

void Costmap2D::reset_maps() {
  std::lock_guard<std::mutex> lock(*access_);
  unsigned char default_value = settings_.default_value_;
  std::uint32_t size_x = settings_.cells_size_x_;
  std::uint32_t size_y = settings_.cells_size_y_;
  memset(costmap_.get(), default_value,
         size_x * size_y * sizeof(unsigned char));
}

void Costmap2D::delete_maps() {
  std::lock_guard<std::mutex> lock(*access_);
  if (costmap_) {
    costmap_.reset();
  }
}

}  // namespace costmap_2d
}  // namespace map