#include "configuration.h"


axion::Configuration::Configuration()
    : ui_sizes_{{12, 16, 32, 224}, {16, 24, 48, 288}, {20, 32, 64, 352}}
{
  settings_.ui_size = ui_sizes_[static_cast<std::size_t>(ui_scale_)];
}


void axion::Configuration::set_ui_scale(int index)
{
  if (static_cast<std::size_t>(index) < ui_sizes_.size()) {
    ui_scale_ = static_cast<UiScale>(index);
    settings_.ui_size = ui_sizes_[index];
    emit ui_scale_changed();
  }
}
