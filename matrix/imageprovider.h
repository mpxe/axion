#ifndef IMAGE_PROVIDER_H
#define IMAGE_PROVIDER_H


#include <QQuickImageProvider>
#include <QSize>
#include <QString>
#include <QHash>
#include <QPixmap>


namespace matrix
{


class ImageProvider : public QQuickImageProvider
{
public:
  ImageProvider();
  QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requested_size);
  void add_pixmap(const QString&& id, QPixmap&& pixmap) {
      images_.insert(std::move(id), std::move(pixmap)); }

private:
  QHash<QString, QPixmap> images_;
  QPixmap default_image_;
};


}  // namespace matrix


#endif  // IMAGE_PROVIDER_H
