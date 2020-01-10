#include "wtt_manager.hpp"

#include <wtlib/loop_wavelet_transform.hpp>
#include <wtlib/butterfly_wavelet_transform.hpp>

#include <QVector3D>

#include <QDebug>
#include <QFile>
#include <sstream>
WTTManager::WTTManager(): debug(DebugLogger("[WTTManager]")),
                          critical(FatalLogger("[WTTManager]"))
{

}

void WTTManager::onLoadMesh(QString filename) {
  debug() << "on loadMesh request";
  QFile mesh_file(filename);
  mesh_origin_.clear();
  mesh_for_wt_.clear();
  coefs_.clear();
  if (mesh_file.open(QFile::ReadOnly)) {
    std::stringstream mesh_stream;
    mesh_stream << QString(mesh_file.readAll()).toStdString();
    mesh_stream >> mesh_origin_;
  } else {
    critical() << "Unable to open mesh file " << mesh_file.fileName();
    emit meshLoaded("Fail to open " + mesh_file.fileName());
    prepareBuffer(mesh_origin_);
    return;
  }

  if (mesh_origin_.size_of_vertices() == 0) {
    critical() << "No vertices data.";
    emit meshLoaded("No geometry data found");
    prepareBuffer(mesh_origin_);
    return;
  }

  if (!mesh_origin_.is_pure_triangle()) {
    critical() << "The mesh is not pure triangle.";
    emit meshLoaded("Input mesh is not pure triangle.");
    prepareBuffer(mesh_origin_);
    return;
  }

  mesh_origin_.normalize_border();
  mesh_for_wt_ = mesh_origin_;
  emit meshLoaded("");
  prepareBuffer(mesh_origin_);
}

void WTTManager::prepareBuffer(const Mesh& mesh) {
  debug() << "Prepare buffers for rendering";
  vpos_.clear();
  vbcs_.clear();
  vnorms_.clear();
  vpos_.reserve(mesh.size_of_facets() * 9);
  vbcs_.reserve(mesh.size_of_facets() * 9);
  vnorms_.reserve(mesh.size_of_facets() * 9);
  for (Facet f = mesh.facets_begin(); f != mesh.facets_end(); ++f)
  {
    Halfedge hc = f->facet_begin();
    Vertex v0 = hc->vertex();
    Vertex v1 = hc->next()->vertex();
    Vertex v2 = hc->next()->next()->vertex();

    vpos_.push_back(static_cast<float>(v0->point().x()));
    vpos_.push_back(static_cast<float>(v0->point().y()));
    vpos_.push_back(static_cast<float>(v0->point().z()));

    vpos_.push_back(static_cast<float>(v1->point().x()));
    vpos_.push_back(static_cast<float>(v1->point().y()));
    vpos_.push_back(static_cast<float>(v1->point().z()));

    vpos_.push_back(static_cast<float>(v2->point().x()));
    vpos_.push_back(static_cast<float>(v2->point().y()));
    vpos_.push_back(static_cast<float>(v2->point().z()));

    vbcs_.push_back(1.0);
    vbcs_.push_back(0.0);
    vbcs_.push_back(0.0);

    vbcs_.push_back(0.0);
    vbcs_.push_back(1.0);
    vbcs_.push_back(0.0);

    vbcs_.push_back(0.0);
    vbcs_.push_back(0.0);
    vbcs_.push_back(1.0);

    QVector3D v0p(v0->point().x(), v0->point().y(), v0->point().z());
    QVector3D v1p(v1->point().x(), v1->point().y(), v1->point().z());
    QVector3D v2p(v2->point().x(), v2->point().y(), v2->point().z());

    QVector3D normal(QVector3D::normal(v1p - v0p, v2p - v0p));
    normal.normalize();

    vnorms_.push_back(static_cast<float>(normal.x()));
    vnorms_.push_back(static_cast<float>(normal.y()));
    vnorms_.push_back(static_cast<float>(normal.z()));

    vnorms_.push_back(static_cast<float>(normal.x()));
    vnorms_.push_back(static_cast<float>(normal.y()));
    vnorms_.push_back(static_cast<float>(normal.z()));

    vnorms_.push_back(static_cast<float>(normal.x()));
    vnorms_.push_back(static_cast<float>(normal.y()));
    vnorms_.push_back(static_cast<float>(normal.z()));
  }
  emit dataReady(vpos_, vnorms_, vbcs_);
}

void WTTManager::onDoFWT(int type, int level) {
  MeshOps meshops;
  bool res = false;
  coefs_.clear();
  if (type == WTType::LOOP) {
    debug() << "Performing " << level << " levels Loop FWT";
    res = wtlib::loop_analyze(mesh_for_wt_, meshops, coefs_, level);
  } else {
    debug() << "Performing " << level << " levels Butterfly FWT";
    if (!mesh_for_wt_.is_closed()) {
      emit fwtDone(false, level, "Butterfly WT is not supported on meshes with boundaries.");
      return;
    }
    res = wtlib::butterfly_analyze(mesh_for_wt_, meshops, coefs_, level);
  }
  if (!res) {
    emit fwtDone(false, level, "The mesh does not have " + QString::number(level) + " levels subdivision connectivity.");
    return;
  }
  emit fwtDone(true, level, "");
  prepareBuffer(mesh_for_wt_);
}

void WTTManager::onDoIWT(int type, int level) {
  using Modifier = wtlib::ptq_impl::PTQ_subdivision_modifier<Mesh, MeshOps>;
  MeshOps meshops;
  bool padding = false;
  if (coefs_.size() < level) {
    padding = true;
    coefs_.resize(level);
  }
  for (int i = 0; i < coefs_.size(); ++i) {
    int expect_size = Modifier::get_mesh_size(mesh_for_wt_, MeshOps{}, i + 1) - Modifier::get_mesh_size(mesh_for_wt_, MeshOps{}, i);
    if (coefs_[i].size() != expect_size) {
      padding = true;
      coefs_[i].resize(expect_size, Vector3 {0.0, 0.0, 0.0});
    }
  }

  if (type == WTType::BUTTERFLY) {
    debug() << "Performing " << level << " Butterfly IWT";
    if (!mesh_for_wt_.is_closed()) {
      emit iwtDone(false, level, "Butterfly WT is not supported on meshes with boundaries.");
      return;
    }
    wtlib::butterfly_synthesize(mesh_for_wt_, meshops, coefs_, level);
  } else {
    debug() << "Performing " << level << " Loop IWT";
    wtlib::loop_synthesize(mesh_for_wt_, meshops, coefs_, level);
  }

  QString msg;
  if (padding) {
    msg = QString("Zero wavelet coefficients padded.");
  } 

  emit iwtDone(true, level,  msg);
  prepareBuffer(mesh_for_wt_);
}

void WTTManager::onCompress(double perc) {
  debug() << "Performing compressing with compression rate " << perc << "%";
  int size = 0;
  for (const auto& v : coefs_) {
    size += v.size();
  }
  std::vector<std::pair<int, int>> idxmap;
  idxmap.reserve(size);
  for (int b = 0; b < coefs_.size(); ++b) {
    for (int i = 0; i < coefs_[b].size(); ++i) {
      idxmap.emplace_back(b, i);
    }
  }

  auto compare = [this](const auto& l, const auto& r) {
    const Vector3& lv = coefs_[l.first][l.second];
    const Vector3& rv = coefs_[r.first][r.second];
    return lv.squared_length() > rv.squared_length();
  };

  int desired_length = int(idxmap.size() * perc / 100.0);
  
  if (desired_length < size) {
    for (int i = desired_length; i < idxmap.size(); ++i) {
      std::pair<int, int> idx = idxmap[i];
      coefs_[idx.first][idx.second] = Vector3{0.0, 0.0, 0.0};
    }
  }
  QString msg = "Set " + QString::number(size > desired_length ? size - desired_length : 0) + " out of " + QString::number(size) + " wavelet coefficients to 0";

  emit compressDone(msg);
}

void WTTManager::onDenoise(int level) {
  debug() << "Performing " << level << " levels denosing";
  for (int l = 0; l < coefs_.size(); ++l) {
    if (l + 1 <= level) {
      continue;
    }
    std::vector<Vector3>& band_coefs = coefs_[l];
    for (Vector3& v : band_coefs) {
      v = Vector3{0.0, 0.0, 0.0};
    }
  }
  emit denoiseDone("Set wavelet coefficients in level " + QString::number(level) + " and above to 0");
}