#include "imageprovider.h"


matrix::ImageProvider::ImageProvider() : QQuickImageProvider{QQuickImageProvider::Pixmap}
{
  if (!default_image_.load(":/img/res/img/default.png")) {
    default_image_ = QPixmap{256, 256};
    default_image_.fill(QColor{"#ff00ff"});
  }
}


QPixmap matrix::ImageProvider::requestPixmap(const QString& id, QSize* size,
    [[maybe_unused]] const QSize& requested_size)
{
  if (size)
    *size = QSize(32, 32);

  QPixmap p;
  if (images_.contains(id)) {
    return images_[id];
  }

  return default_image_;
}
