#ifndef WTT_DEMO_INCLUDE_TRIANGLE_MESH_SCENE_HPP
#define WTT_DEMO_INCLUDE_TRIANGLE_MESH_SCENE_HPP

#include "logger.hpp"
#include <scene_object.hpp>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>

class TriangleMeshScene: public SceneObject
{
  Q_OBJECT
public:
  enum VBO
  {
    POSITION = 0,
    NORMAL = 1,
    BARYCENTRIC = 2
  };
public:
  explicit TriangleMeshScene(QObject* parent = 0);
  virtual ~TriangleMeshScene();

  virtual void render() override;
  virtual void init() override;

  virtual void setModelMat(const QMatrix4x4& model) override;
  virtual void setViewMat(const QMatrix4x4& view) override;
  virtual void setProjMat(const QMatrix4x4& projection) override;

  virtual QMatrix4x4 modelMat() const;

  virtual void allocateVboData(int count,
                               unsigned int vbo) override;
  virtual void updateVboData(int offset,
                             const void* data,
                             int count,
                             unsigned int vbo) override;

  virtual void allocatePos(int count);
  virtual void allocateNormal(int count);
  /**
   * \brief      Allocate data for barycentric vbo.
   *
   * \param[in]  data   Data starting point
   * \param[in]  count  Count in bytes.
   */
  virtual void allocateBC(int count);

  virtual void updatePos(int offset, const void* data, int count);
  virtual void updateNormal(int offset, const void* data, int count);
  virtual void updateBC(int offset, const void* data, int count);

  virtual void setPrimitiveSize(std::size_t size) override;
  virtual std::size_t primitiveSize() const override;

  virtual void loadShader();

  virtual void renderEdge(bool on) override;

  virtual void rotate(float angle, const QVector3D& vector) override;
  virtual void rotate(const QQuaternion& q) override;

protected:
  QOpenGLVertexArrayObject vao_;
  QOpenGLBuffer vpos_;
  QOpenGLBuffer vnormal_;
  QOpenGLBuffer vbarycentric_;

  QMatrix4x4 model_;
  QMatrix4x4 view_;
  QMatrix4x4 proj_;

  bool show_edge_;
  std::size_t tri_size_;
  DebugLogger debug;
  FatalLogger critical;
};

#endif  // define WTT_DEMO_INCLUDE_TRIANGLE_MESH_SCENE_HPP