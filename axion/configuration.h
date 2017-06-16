#ifndef CONFIGURATION_H
#define CONFIGURATION_H


#include <vector>
#include <QObject>


namespace axion
{


enum class UiScale { Small, Medium, Large };


struct UiSize
{
  int text_size;
  int icon_size;
  int avatar_size;
  int image_size;
};


struct Settings
{
  UiSize ui_size;
};


class Configuration : public QObject
{
  Q_OBJECT

  Q_PROPERTY(int uiScale READ ui_scale NOTIFY ui_scale_changed)
  Q_PROPERTY(int iconSize READ icon_size NOTIFY ui_scale_changed)
  Q_PROPERTY(int textSize READ text_size NOTIFY ui_scale_changed)
  Q_PROPERTY(int avatarSize READ avatar_size NOTIFY ui_scale_changed)
  Q_PROPERTY(int imageSize READ image_size NOTIFY ui_scale_changed)

public:
  Configuration();

  int ui_scale() const { return static_cast<int>(ui_scale_); }
  int text_size() const { return settings_.ui_size.text_size; }
  int icon_size() const { return settings_.ui_size.icon_size; }
  int avatar_size() const { return settings_.ui_size.avatar_size; }
  int image_size() const { return settings_.ui_size.image_size; }

public slots:
  void set_ui_scale(int index);

signals:
  void ui_scale_changed();

private:
  const std::vector<UiSize> ui_sizes_;
  UiScale ui_scale_ = UiScale::Large;
  Settings settings_;
};


}  // namepsace axion


#endif  // CONFIGURATION_H
