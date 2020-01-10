#ifndef WTT_DEMO_INCLUDE_OPENGL_WIDGET_HPP
#define WTT_DEMO_INCLUDE_OPENGL_WIDGET_HPP
#include "logger.hpp"
#include <arcball_camera.hpp>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QPointF>
#include <QOpenGLBuffer>

class QOpenGLShaderProgram;
class QMouseEvent;
class QKeyEvent;
class QPushButton;
class SceneObject;
class OpenGLWidget: public QOpenGLWidget, public QOpenGLFunctions
{
  Q_OBJECT
public:
  explicit OpenGLWidget(QWidget* parent = 0);
  virtual ~OpenGLWidget();

  virtual void paintGL() override;
  virtual void resizeGL(int w, int h) override;
  virtual void initializeGL() override;

signals:
  void meshLoaded();

public slots:
  void onDataReady(const QVector<float>& vpos,
                   const QVector<float>& vnorms,
                   const QVector<float>& vbcs);
  void onToggleEdge();

protected:
  virtual void resizeEvent(QResizeEvent* e) override;
  virtual void mousePressEvent(QMouseEvent* e) override;
  virtual void mouseMoveEvent(QMouseEvent* e) override;
  virtual void keyPressEvent(QKeyEvent* e) override;

  virtual void wheelEvent(QWheelEvent* e) override;

protected:

  ArcballCamera camera_;

  SceneObject* scene_ptr_;
  double scale_;
  QMatrix4x4 model_;
  QMatrix4x4 view_;
  QMatrix4x4 projection_;

  QPointF mouse_last_pos_;
  bool data_ready_;
  bool show_edge_;
  QPushButton* toggle_edge_button_;

  DebugLogger debug;
  FatalLogger critical;
};

#endif  // define WTT_DEMO_INCLUDE_OPENGL_WIDGET_HPP