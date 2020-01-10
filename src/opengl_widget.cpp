#include <opengl_widget.hpp>
#include <triangle_mesh_scene.hpp>

#include <QDebug>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QScreen>
#include <QApplication>

#include <cmath>
#include <cassert>

OpenGLWidget::OpenGLWidget(QWidget* parent):
QOpenGLWidget(parent),
camera_(QVector3D(0.0, 0.0, 5.0),
        QVector3D(0.0, 0.0, 0.0),
        QVector3D(0.0, 1.0, 0.0)),
scale_(1.0),
view_(camera_.getViewMatrix()),
data_ready_(false),
show_edge_(false),
toggle_edge_button_(new QPushButton(this)),
debug(DebugLogger("[OpenGL Widget]")),
critical(FatalLogger("[OpenGL Widget]"))
{
  model_.setToIdentity();
  projection_.setToIdentity();
  this->setFocusPolicy(Qt::StrongFocus);
  this->setMouseTracking(false);

  QVBoxLayout* layout = new QVBoxLayout(toggle_edge_button_);
  QLabel* toggle_edge_icon = new QLabel(toggle_edge_button_);
  toggle_edge_icon->setPixmap(QPixmap(":/images/topology.png"));
  toggle_edge_icon->setScaledContents(true);
  layout->addWidget(toggle_edge_icon);
  toggle_edge_button_->setLayout(layout);
  toggle_edge_button_->show();
  toggle_edge_button_->raise();
  toggle_edge_button_->setStyleSheet("QPushButton {border: none; background-color: none;} QLabel{border: none; background-color: none;}");
  connect(toggle_edge_button_, &QPushButton::pressed, this, &OpenGLWidget::onToggleEdge);
}

OpenGLWidget::~OpenGLWidget()
{
  if (scene_ptr_) {
    delete scene_ptr_;
  }
}

void OpenGLWidget::resizeEvent(QResizeEvent *e) {
  qreal scale = qApp->primaryScreen()->logicalDotsPerInch() / 96.0;
  toggle_edge_button_->setGeometry(0, 0.5 * this->height(), 64 * scale, 64 * scale);
  QOpenGLWidget::resizeEvent(e);
}

void OpenGLWidget::mousePressEvent(QMouseEvent* e)
{
  mouse_last_pos_ = e->pos();
  QOpenGLWidget::mousePressEvent(e);
}

void OpenGLWidget::wheelEvent(QWheelEvent* e) {

  bool positive = e->angleDelta().y() > 0;
  if (e->inverted()) {
    positive = !positive;
  }
  if (data_ready_) {
    if (positive) {
      model_.scale(1.05);
    } else {
      model_.scale(0.95);
    }
  }
  QOpenGLWidget::wheelEvent(e);
  this->update();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  QPointF mouse_mov = e->pos() - mouse_last_pos_;
  qreal window_radius = std::sqrt(this->width() * this->width() + 
                                  this->height() * this->height());
  if (e->modifiers() == Qt::ShiftModifier)
  {
    camera_.shift(QVector2D(mouse_mov.x() / window_radius, mouse_mov.y() / window_radius));
  }
  else
  {
    camera_.drag(QVector2D(mouse_mov.x() / window_radius, mouse_mov.y() / window_radius));
  }

  view_ = camera_.getViewMatrix();

  mouse_last_pos_ = e->pos();

  this->update();

  QOpenGLWidget::mouseMoveEvent(e);
}

void OpenGLWidget::keyPressEvent(QKeyEvent* e)
{
  switch (e->key())
  {
    case Qt::Key_Up:
    {

      break;
    }
    case Qt::Key_Down:
    {

      break;
    }
    case Qt::Key_Left:
    {

      break;
    }
    case Qt::Key_Right:
    {

      break;
    }
  }

  QOpenGLWidget::keyPressEvent(e);
}


void OpenGLWidget::initializeGL()
{
  qDebug() << "[OpenGL Widget] initializeGL()";
  initializeOpenGLFunctions();
  QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
  f->glEnable(GL_CULL_FACE);
  f->glEnable(GL_DEPTH_TEST);
  f->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  scene_ptr_ = new TriangleMeshScene();
  scene_ptr_->init();
}

void OpenGLWidget::resizeGL(int w, int h)
{
  qDebug() << "[OpenGL Widget] resizeGL(" << w << ", " << h << ")";
  if (this->context())
  {
    glViewport(0, 0,  w, h);
  }
  projection_.setToIdentity();
  projection_.perspective(45, float(w) / float(h), 1.0, 1000.0);
  this->update();
}

void OpenGLWidget::paintGL()
{
  QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
  f->glClearColor(0.7, 0.7, 0.7, 0.7);
  f->glClear(GL_COLOR_BUFFER_BIT);
  f->glClear(GL_DEPTH_BUFFER_BIT);

  if (data_ready_ && scene_ptr_) {
    scene_ptr_->renderEdge(show_edge_);
    scene_ptr_->setModelMat(model_);
    scene_ptr_->setViewMat(view_);
    scene_ptr_->setProjMat(projection_);
    scene_ptr_->render();
  }
}

void OpenGLWidget::onToggleEdge() {
  show_edge_ = !show_edge_;
  this->update();
}
void OpenGLWidget::onDataReady(const QVector<float> &vpos, const QVector<float> &vnorms, const QVector<float> &vbcs)
{
  data_ready_ = !vpos.empty();
  static_assert(sizeof(GLfloat) == sizeof(float));
  scene_ptr_->allocateVboData(sizeof(GLfloat) * vpos.size(),
                              TriangleMeshScene::VBO::POSITION);
  scene_ptr_->updateVboData(0,
                            vpos.data(),
                            sizeof(GLfloat) * vpos.size(),
                            TriangleMeshScene::VBO::POSITION);

  scene_ptr_->allocateVboData(sizeof(GLfloat) * vnorms.size(),
                              TriangleMeshScene::VBO::NORMAL);
  scene_ptr_->updateVboData(0,
                            vnorms.data(),
                            sizeof(GLfloat) * vnorms.size(),
                            TriangleMeshScene::VBO::NORMAL);

  scene_ptr_->allocateVboData(sizeof(GLfloat) * vbcs.size(),
                              TriangleMeshScene::VBO::BARYCENTRIC);
  scene_ptr_->updateVboData(0,
                            vbcs.data(),
                            sizeof(GLfloat) * vbcs.size(),
                            TriangleMeshScene::VBO::BARYCENTRIC);

  scene_ptr_->setPrimitiveSize(vpos.size() / 3);

  emit meshLoaded();
  this->update();
}
