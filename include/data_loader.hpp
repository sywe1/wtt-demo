#ifndef WTT_DEMO_INCLUDE_DATA_LOADER_HPP
#define WTT_DEMO_INCLUDE_DATA_LOADER_HPP

#include <QObject>
#include <QOpenGLBuffer>
#include <QSharedPointer>

class SceneObject;

class DataLoader: public QObject
{
  Q_OBJECT
public:
  explicit DataLoader();

  void loadFromPolyhedron();

public slots:
  void onOpenGLContextReady();

signals:
  void dataReady(QSharedPointer<SceneObject> scene_obj_ptr);

protected:
  QSharedPointer<SceneObject> scene_ptr_;

};
#endif  // define WTT_DEMO_INCLUDE_DATA_LOADER_HPP