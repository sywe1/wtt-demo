#ifndef WTT_DEMO_INCLUDE_WTT_MANAGER_HPP
#define WTT_DEMO_INCLUDE_WTT_MANAGER_HPP

#include "custom_mesh_types.hpp"
#include "logger.hpp"

#include <QVector>
#include <QObject>
#include <QThread>
#include <QOpenGLFunctions>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
class SceneObject;

class WTTManager: public QThread {
  Q_OBJECT
public:
  enum WTType {
    LOOP = 0,
    BUTTERFLY = 1
  };
  using Halfedge = typename Mesh::Halfedge_const_handle;
  using Vertex = typename Mesh::Vertex_const_handle;
  using Facet = typename Mesh::Facet_const_handle;
  using Vector3 = typename Mesh::Traits::Vector_3;
  using Vertex_handle = typename Mesh::Vertex_handle;
  using Vertex_const_handle = typename Mesh::Vertex_const_handle;
  explicit WTTManager();


public slots:
  void onLoadMesh(QString filename);

  void onDoFWT(int type, int level);

  void onDoIWT(int type, int level);

  void onCompress(double perc);
  void onDenoise(int level);
  void prepareBuffer(const Mesh& mesh);


signals:
  void dataReady(const QVector<float>& pos,
                 const QVector<float>& normal,
                 const QVector<float>& bc);

  void meshLoaded(QString err);
  void checkSCDone(bool, int);
  void fwtDone(bool, int, QString err);
  void iwtDone(bool, int, QString msg);
  void compressDone(QString msg);
  void denoiseDone(QString msg);

protected:
  Mesh mesh_origin_;
  Mesh mesh_for_wt_;

  QVector<float> vpos_;
  QVector<float> vnorms_;
  QVector<float> vbcs_;
  std::vector<std::vector<Vector3>> coefs_;

  DebugLogger debug;
  FatalLogger critical;
};

#endif