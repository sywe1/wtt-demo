#include <data_loader.hpp>

#include <triangle_mesh_scene.hpp>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>

#include <QDebug>
#include <QFile>

#include <sstream>

DataLoader::DataLoader()
{
}

void DataLoader::onOpenGLContextReady()
{
  qDebug() << "OpenGL context ready";
  scene_ptr_ = QSharedPointer<TriangleMeshScene>(new TriangleMeshScene());
  scene_ptr_->init();
  loadFromPolyhedron();
}

void DataLoader::loadFromPolyhedron()
{
  using K = CGAL::Simple_cartesian<double>;
  using Mesh = CGAL::Polyhedron_3<K>;
  using Halfedge = typename Mesh::Halfedge_handle;
  using Vertex = typename Mesh::Vertex_handle;
  using Facet = typename Mesh::Facet_handle;

  Mesh mesh;

  QFile mesh_file(":/mesh/dragon_1000.off");

  if (mesh_file.open(QFile::ReadOnly))
  {
    std::stringstream mesh_stream;
    mesh_stream << QString(mesh_file.readAll()).toStdString();
    mesh_stream >> mesh;
  }
  else
  {
    qCritical() << "Unable to open mesh file";
    return;
  }

  if (mesh.size_of_vertices() == 0)
  {
    qCritical() << "No vertices data";
    return;
  }

  if (!mesh.is_pure_triangle())
  {
    qCritical() << "Only support triangle mesh";
    return;
  }

  std::vector<GLfloat> vpos;
  std::vector<GLfloat> vnorms;
  std::vector<GLfloat> vbcs;

  for (Facet f = mesh.facets_begin(); f != mesh.facets_end(); ++f)
  {
    Halfedge hc = f->facet_begin();
    Vertex v0 = hc->vertex();
    Vertex v1 = hc->next()->vertex();
    Vertex v2 = hc->next()->next()->vertex();

    vpos.push_back(static_cast<GLfloat>(v0->point().x()));
    vpos.push_back(static_cast<GLfloat>(v0->point().y()));
    vpos.push_back(static_cast<GLfloat>(v0->point().z()));

    vpos.push_back(static_cast<GLfloat>(v1->point().x()));
    vpos.push_back(static_cast<GLfloat>(v1->point().y()));
    vpos.push_back(static_cast<GLfloat>(v1->point().z()));

    vpos.push_back(static_cast<GLfloat>(v2->point().x()));
    vpos.push_back(static_cast<GLfloat>(v2->point().y()));
    vpos.push_back(static_cast<GLfloat>(v2->point().z()));

    vbcs.push_back(1.0);
    vbcs.push_back(0.0);
    vbcs.push_back(0.0);

    vbcs.push_back(0.0);
    vbcs.push_back(1.0);
    vbcs.push_back(0.0);

    vbcs.push_back(0.0);
    vbcs.push_back(0.0);
    vbcs.push_back(1.0);

    QVector3D v0p(v0->point().x(), v0->point().y(), v0->point().z());
    QVector3D v1p(v1->point().x(), v1->point().y(), v1->point().z());
    QVector3D v2p(v2->point().x(), v2->point().y(), v2->point().z());

    QVector3D normal(QVector3D::normal(v1p - v0p, v2p - v0p));
    normal.normalize();

    vnorms.push_back(static_cast<GLfloat>(normal.x()));
    vnorms.push_back(static_cast<GLfloat>(normal.y()));
    vnorms.push_back(static_cast<GLfloat>(normal.z()));

    vnorms.push_back(static_cast<GLfloat>(normal.x()));
    vnorms.push_back(static_cast<GLfloat>(normal.y()));
    vnorms.push_back(static_cast<GLfloat>(normal.z()));

    vnorms.push_back(static_cast<GLfloat>(normal.x()));
    vnorms.push_back(static_cast<GLfloat>(normal.y()));
    vnorms.push_back(static_cast<GLfloat>(normal.z()));
  }
  

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

  scene_ptr_->setPrimitiveSize(mesh.size_of_facets() * 3);

  emit dataReady(scene_ptr_);
}