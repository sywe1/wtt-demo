#include "message_box.hpp"
#include "action_panel.hpp"
#include "opengl_widget.hpp"
#include "integer_setter.hpp"
#include "input_prop.hpp"
#include "mainwindow.hpp"
#include "wtt_manager.hpp"

#include <QDebug>
#include <QEvent>
#include <QScreen>
#include <QMovie>
#include <QLabel>
#include <QFileDialog>
#include <QGraphicsOpacityEffect>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent):QMainWindow(parent),
                                        ui_ptr_(new Ui::MainWindow),
                                        opengl_widget_ptr_(new OpenGLWidget(this)),
                                        action_panel_ptr_(new ActionPanel(this)),
                                        proc_diag_ptr_(new MessageBox(this)),
                                        msg_prop_ptr_(new MessageBox(this)),
                                        wt_type_setter_ptr_(new MessageBox(this)),
                                        fwt_level_setter_ptr_(new IntegerSetter(this)),
                                        iwt_level_setter_ptr_(new IntegerSetter(this)),
                                        denoise_level_setter_ptr_(new IntegerSetter(this)),
                                        compress_rate_setter_ptr_(new InputProp(this)),
                                        wtt_manager_(new WTTManager()),
                                        debug(DebugLogger("[MainWindow]")),
                                        critical(FatalLogger("[MainWindow]"))
{
  qRegisterMetaType<QVector<float>>();
  ui_ptr_->setupUi(this);
  ui_ptr_->main_layout->addWidget(opengl_widget_ptr_);
  action_panel_ptr_->raise();
  action_panel_ptr_->show();
  initWidgets();
  initializeGeometry();
  setupConnections();

  wtt_manager_->moveToThread(wtt_manager_);
  wtt_manager_->start();
}

MainWindow::~MainWindow()
{
  wtt_manager_->exit(0);
  while (wtt_manager_->isRunning());
  delete wtt_manager_;
  delete ui_ptr_;
}

void MainWindow::initWidgets() {
  proc_diag_ptr_->resetStyleSheet(":/qss/proc.qss");
  QLabel* proc_lable = proc_diag_ptr_->getDescription();
  QMovie* proc_animation = new QMovie(":/images/loading.gif", QByteArray(), this);
  connect(proc_animation, &QMovie::error,[this](QImageReader::ImageReaderError err){
    if (err == 1) {
      qCritical() << "\033[31;1m[ERROR][Processing Diag] Animation file not found\033[0m";
    } else {
      qCritical() << "\033[31;1m[ERROR][Processing Diag] animation file loading error " << err << "\033[0m";
    }
  });
  proc_lable->setMovie(proc_animation);
  proc_lable->setScaledContents(true);
  proc_animation->start();
  QGraphicsOpacityEffect* e = new QGraphicsOpacityEffect(proc_diag_ptr_);
  e->setOpacity(0.85);
  proc_diag_ptr_->setGraphicsEffect(e);


  msg_prop_ptr_->addAcceptButton("Confirm");
  msg_prop_ptr_->getDescription()->setWordWrap(true);
  msg_prop_ptr_->resetStyleSheet(":/qss/message_box.qss");

  wt_type_setter_ptr_->addAcceptButton("Butterfly");
  wt_type_setter_ptr_->addRejectButton("Loop");
  wt_type_setter_ptr_->resetStyleSheet(":/qss/wt_type_setter.qss");

  wt_type_setter_ptr_->getDescription()->setText("Set wavelet transform type to");
  fwt_level_setter_ptr_->setDescription("Set FWT level to");
  iwt_level_setter_ptr_->setDescription("Set IWT level to");
  denoise_level_setter_ptr_->setDescription("Set denoise level to");
  
  wt_type_setter_ptr_->setBackgroundOpacity(125);
  fwt_level_setter_ptr_->setBackgroundOpacity(125);
  iwt_level_setter_ptr_->setBackgroundOpacity(125);
  denoise_level_setter_ptr_->setBackgroundOpacity(125);
  compress_rate_setter_ptr_->setBackgroundOpacity(125);
}

void MainWindow::initializeGeometry()
{
  debug() << "initialize geometry";
  qreal scale = qApp->primaryScreen()->logicalDotsPerInch() / 96.0;
  QSize availabel_size = qApp->primaryScreen()->availableSize();

  this->setMinimumSize(1024 * scale,
                       768 * scale);

  this->setGeometry(0.45 * (availabel_size.width() - 1024 * scale),
                    0.45 * (availabel_size.height() - 768 * scale),
                    1024 * scale,
                    768 * scale);
  proc_diag_ptr_->setGeometry(0, 0, this->width(), this->height());
  proc_diag_ptr_->getDescription()->setMinimumSize(200, 200);

  msg_prop_ptr_->setGeometry(0, 0, this->width(), this->height());
  msg_prop_ptr_->getDescription()->setMinimumSize(400, 70);
  msg_prop_ptr_->setBodySize(QSize(600, 350));

  wt_type_setter_ptr_->setGeometry(0, 0, this->width(), this->height());
  wt_type_setter_ptr_->getDescription()->setMinimumSize(400, 70);
  wt_type_setter_ptr_->setBodySize(QSize(600, 250));

  fwt_level_setter_ptr_->setGeometry(0, 0, this->width(), this->height());
  fwt_level_setter_ptr_->setBodySize(QSize(600, 350));
  iwt_level_setter_ptr_->setGeometry(0, 0, this->width(), this->height());
  iwt_level_setter_ptr_->setBodySize(QSize(600, 350));
}

void MainWindow::resizeEvent(QResizeEvent* e)
{

  QSize floating_widget_size = action_panel_ptr_->minimumSizeHint();

  action_panel_ptr_->setGeometry(0,
                                this->height() - floating_widget_size.height() - 20,
                                this->width(),
                                floating_widget_size.height());
  proc_diag_ptr_->setGeometry(0, 0, this->width(), this->height());
  msg_prop_ptr_->setGeometry(0, 0, this->width(), this->height());
  wt_type_setter_ptr_->setGeometry(0, 0, this->width(), this->height());
  fwt_level_setter_ptr_->setGeometry(0, 0, this->width(), this->height());
  iwt_level_setter_ptr_->setGeometry(0, 0, this->width(), this->height());
  denoise_level_setter_ptr_->setGeometry(0, 0, this->width(), this->height());
  compress_rate_setter_ptr_->setGeometry(0, 0, this->width(), this->height());
  QMainWindow::resizeEvent(e); 
}

void MainWindow::setupConnections() {
  debug() << "setup connections";
  connect(action_panel_ptr_, &ActionPanel::userAction, this, &MainWindow::onUserAction);

  connect(wt_type_setter_ptr_, &MessageBox::finished, this, &MainWindow::onWTTypeSet);
  connect(fwt_level_setter_ptr_, &IntegerSetter::finished, this, &MainWindow::onFWTLevelSet);
  connect(iwt_level_setter_ptr_, &IntegerSetter::finished, this, &MainWindow::onIWTLevelSet);
  connect(denoise_level_setter_ptr_, &IntegerSetter::finished, this, &MainWindow::onDenoiseLevelSet);
  connect(compress_rate_setter_ptr_, &InputProp::finished, this, &MainWindow::onCompressRateSet);

  connect(opengl_widget_ptr_, &OpenGLWidget::meshLoaded, this, &MainWindow::onOpenGLLoadMesh);
  connect(wtt_manager_, &WTTManager::dataReady, opengl_widget_ptr_, &OpenGLWidget::onDataReady);

  connect(this, &MainWindow::loadMesh, wtt_manager_, &WTTManager::onLoadMesh);
  connect(wtt_manager_, &WTTManager::meshLoaded, this, &MainWindow::onMeshLoaded);
  connect(this, &MainWindow::doFWT, wtt_manager_, &WTTManager::onDoFWT);
  connect(this, &MainWindow::doIWT, wtt_manager_, &WTTManager::onDoIWT);
  connect(wtt_manager_, &WTTManager::fwtDone, this, &MainWindow::onFWTDone);
  connect(wtt_manager_, &WTTManager::iwtDone, this, &MainWindow::onIWTDone);
  connect(wtt_manager_, &WTTManager::compressDone, this, &MainWindow::onCompressDone);
  connect(wtt_manager_, &WTTManager::denoiseDone, this, &MainWindow::onDenoiseDone);
  connect(this, &MainWindow::doCompress, wtt_manager_, &WTTManager::onCompress);
  connect(this, &MainWindow::doDenoise, wtt_manager_, &WTTManager::onDenoise);
}

void MainWindow::onUserAction(int action)
{
  proc_diag_ptr_->open();
  QString fileName;
  switch (action) {
    case ActionPanel::OPENMESH:
      debug() << "User action: open mesh";
      fileName = QFileDialog::getOpenFileName(this, "Open Mesh", "/home/sywe1/Dropbox/MEng/meshes", "OFF Files (*.off)");
      debug() << "User open file: " << fileName;
      if (fileName.isEmpty()) {
        proc_diag_ptr_->done();
      } else {
        emit loadMesh(fileName);
      }
      break;
    case ActionPanel::RESETMESH:
      debug() << "User action: reset";
      proc_diag_ptr_->done(1);
      break;
    case ActionPanel::SETTYPE:
      debug() << "User action: set wt type";
      wt_type_setter_ptr_->exec();
      break;
    case ActionPanel::FWT:
      debug() << "User action: do fwt";
      fwt_level_setter_ptr_->exec();
      break;
    case ActionPanel::IWT:
      debug() << "User action: do iwt";
      iwt_level_setter_ptr_->exec();
      break;
    case ActionPanel::DENOISE:
      debug() << "User action: denoise";
      denoise_level_setter_ptr_->exec();
      break;
    case ActionPanel::COMPRESS:
      debug() << "User action: compress";
      compress_rate_setter_ptr_->exec();
      break;
    default:
      critical() << "Unknown user action";
      break;
  }
}

void MainWindow::onOpenGLLoadMesh() {
  debug() << "Receive signal: Mesh rendered by OpenGL";
  proc_diag_ptr_->done(1);
}

void MainWindow::onMeshLoaded(QString err) {
  if (err.isEmpty()) {
    action_panel_ptr_->onOpenMeshDone(true);
  } else {
    action_panel_ptr_->onOpenMeshDone(false);
    msg_prop_ptr_->getDescription()->setText(err);
    msg_prop_ptr_->exec();
  }
  denoise_level_setter_ptr_->setValue(0);
  denoise_level_setter_ptr_->setMax(0);
}

void MainWindow::onFWTLevelSet(int code) {
  if (code == IntegerSetter::Accepted) {
    int level = fwt_level_setter_ptr_->getValue();
    debug() << "Receive request: " << level << " levels FWT transform";
    emit doFWT(wt_type_, level);
  } else {
    proc_diag_ptr_->done(0);
  }
}

void MainWindow::onIWTLevelSet(int code) {
  if (code == IntegerSetter::Accepted) {
    int level = iwt_level_setter_ptr_->getValue();
    debug() << "Receive request: " << level << " levels IWT transform";
    emit doIWT(wt_type_, level);
  } else {
    proc_diag_ptr_->done(0);
  }
}

void MainWindow::onCompressRateSet(int code) {
  if (code == InputProp::Accepted) {
    double perc = compress_rate_setter_ptr_->getValue();
    debug() << "Send signal: compression" << perc << "%";
    emit doCompress(perc);
  } else {
    proc_diag_ptr_->done(0);
  }
}

void MainWindow::onDenoiseLevelSet(int code) {
  if (code == IntegerSetter::Accepted) {
    int level = denoise_level_setter_ptr_->getValue();
    debug() << "Send signal: denoising (level =" << level <<")";
    emit doDenoise(level);
  } else {
    proc_diag_ptr_->done(0);
  }
}


void MainWindow::onWTTypeSet(int type) {
  debug() << "Receive signal: WT type set to" << type;
  proc_diag_ptr_->done(1);
  action_panel_ptr_->onTypeSelected(type);
  wt_type_ = type;
}

void MainWindow::onFWTDone(bool succ, int level, QString err) {
  debug() << "Receive signal:" << level << "levels FWT done";
  proc_diag_ptr_->done(1);
  if (succ) {
    action_panel_ptr_->onFWTDone(true);
    denoise_level_setter_ptr_->setMax(level);
    denoise_level_setter_ptr_->setValue(0);
    iwt_level_setter_ptr_->setValue(level);
  } else {
    msg_prop_ptr_->getDescription()->setText(err);
    msg_prop_ptr_->exec();
    critical() << "Receive FWT error: " << err;
    action_panel_ptr_->onFWTDone(false);
  }
}

void MainWindow::onIWTDone(bool succ, int level, QString err) {
  debug() << "Receive signal:" << level << "levels IWT done";
  proc_diag_ptr_->done(1);
  denoise_level_setter_ptr_->setMax(0);
  denoise_level_setter_ptr_->setValue(0);
  if (succ) {
    action_panel_ptr_->onIWTDone(true);
    if (!err.isEmpty()) {
      msg_prop_ptr_->getDescription()->setText(err);
      msg_prop_ptr_->exec();
    }
  } else {
    action_panel_ptr_->onIWTDone(false);
  }
}

void MainWindow::onCompressDone(QString msg) {
  proc_diag_ptr_->done(1);
  msg_prop_ptr_->getDescription()->setText(msg);
  msg_prop_ptr_->exec();
}

void MainWindow::onDenoiseDone(QString msg) {
  proc_diag_ptr_->done(1);
  msg_prop_ptr_->getDescription()->setText(msg);
  msg_prop_ptr_->exec();
}