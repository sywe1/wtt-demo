#include "glview_control_panel.hpp"

#include <QPushButton>
#include <QLabel>
#include <QScreen>
#include <QFile>
#include <QApplication>
GLViewControlPanel::GLViewControlPanel(QWidget* parent):
ControlPanel(parent),
reset_cam_button_(new QPushButton(this)),
toggle_edge_button_(new QPushButton(this)),
smooth_shading_button_(new QPushButton(this)),
flat_shading_button_(new QPushButton(this))
{
  QFile qss_file(":/qss/glview_control_panel.qss");

  if (qss_file.open(QFile::ReadOnly))
  {
    this->setStyleSheet(QString(qss_file.readAll()));
  }
  else
  {
    qCritical() << "[GUI][ActionPanel] Error applying qss: " << qss_file.fileName();
  }
  initChildren();
  initSize();
  connect(reset_cam_button_, &QPushButton::clicked, std::bind(&GLViewControlPanel::action, this, GLViewActions::RESETCAM));
  connect(toggle_edge_button_, &QPushButton::clicked, std::bind(&GLViewControlPanel::action, this, GLViewActions::TOGGLEEDGE));
  connect(smooth_shading_button_, &QPushButton::clicked, std::bind(&GLViewControlPanel::action, this, GLViewActions::SMOOTHSHADING));
  connect(flat_shading_button_, &QPushButton::clicked, std::bind(&GLViewControlPanel::action, this, GLViewActions::FLATSHADING));
}

GLViewControlPanel::~GLViewControlPanel() {}

void GLViewControlPanel::initChildren() {
  QVBoxLayout* toggle_edge_layout = new QVBoxLayout(toggle_edge_button_);
  QLabel* toggle_edge_icon = new QLabel(toggle_edge_button_);
  toggle_edge_icon->setPixmap(QPixmap(":/images/topology.png"));
  toggle_edge_icon->setScaledContents(true);
  toggle_edge_layout->addWidget(toggle_edge_icon);
  toggle_edge_button_->setLayout(toggle_edge_layout);

  QVBoxLayout* reset_cam_layout = new QVBoxLayout(reset_cam_button_);
  QLabel* reset_cam_icon = new QLabel(reset_cam_button_);
  reset_cam_icon->setPixmap(QPixmap(":/images/reset_cam.png"));
  reset_cam_icon->setScaledContents(true);
  reset_cam_layout->addWidget(reset_cam_icon);
  reset_cam_button_->setLayout(reset_cam_layout);

  QVBoxLayout* flat_shading_layout = new QVBoxLayout(flat_shading_button_);
  QLabel* flat_shading_icon = new QLabel(flat_shading_button_);
  flat_shading_icon->setPixmap(QPixmap(":/images/flat_shading.png"));
  flat_shading_icon->setScaledContents(true);
  flat_shading_layout->addWidget(flat_shading_icon);
  flat_shading_button_->setLayout(flat_shading_layout);

  QVBoxLayout* smooth_shading_layout = new QVBoxLayout(smooth_shading_button_);
  QLabel* smooth_shading_icon = new QLabel(smooth_shading_button_);
  smooth_shading_icon->setPixmap(QPixmap(":/images/smooth_shading.png"));
  smooth_shading_icon->setScaledContents(true);
  smooth_shading_layout->addWidget(smooth_shading_icon);
  smooth_shading_button_->setLayout(smooth_shading_layout);

  this->addButton(reset_cam_button_);
  this->addButton(toggle_edge_button_);
  this->addButton(smooth_shading_button_);
  this->addButton(flat_shading_button_);
}

void GLViewControlPanel::initSize() {
  qreal scale = qApp->primaryScreen()->logicalDotsPerInch() / 96;
  reset_cam_button_->setFixedSize(QSize(64, 64) * scale);
  toggle_edge_button_->setFixedSize(QSize(64, 64) * scale);
  smooth_shading_button_->setFixedSize(QSize(64, 64) * scale);
  flat_shading_button_->setFixedSize(QSize(64, 64) * scale);
  this->setMinimumSize(QSize(64, 64 * 4) * scale);
}