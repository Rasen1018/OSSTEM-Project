#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "panoramaform.h"
#include "cephaloform.h"
#include "panovalueadjustment.h"
#include "cephvalueadjustment.h"
#include "panopreset.h"
#include "cephpreset.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Unsigned Viewer");

    connect(this, SIGNAL(destroyed(QObject*)),
            this, SLOT(deleteLater()));

    /* Panorama, Cephalo Form의 객체 선언 */
    panoramaForm = new PanoramaForm(this);
    connect(panoramaForm, SIGNAL(destroyed()),
            panoramaForm, SLOT(deleteLater()));
    cephaloForm = new CephaloForm(this);
    connect(cephaloForm, SIGNAL(destroyed()),
            cephaloForm, SLOT(deleteLater()));

    /* Panorama, Cephalo 연산 클래스의 객체 선언*/
    panoValueAdjustment = new PanoValueAdjustment(this);
    connect(panoValueAdjustment, SIGNAL(destroyed()),
            panoValueAdjustment, SLOT(deleteLater()));
    cephValueAdjustment = new CephValueAdjustment(this);
    connect(cephValueAdjustment, SIGNAL(destroyed()),
            cephValueAdjustment, SLOT(deleteLater()));

    /* Panorama, Cephalo 프리셋 연산을 수행하는 객체 선언*/
    panoPreset = new PanoPreset(this);
    connect(panoPreset, SIGNAL(destroyed()),
            panoPreset, SLOT(deleteLater()));
    cephPreset = new CephPreset(this);
    connect(cephPreset, SIGNAL(destroyed()),
            cephPreset, SLOT(deleteLater()));


    /* ui 설정 */
    ui->stackedWidget->insertWidget(0, panoramaForm);
    ui->stackedWidget->insertWidget(1, cephaloForm);

    /* DB Load SIGNAL/SLOT*/
    connect(this, SIGNAL(loadPanoDB(QString)),
            panoramaForm, SLOT(loadDB_Data(QString)));
    connect(this, SIGNAL(loadCephDB(QString)),
            cephaloForm, SLOT(loadDB_Data(QString)));

    /* panorama SIGNAL/SLOT */
    /* Load 시, 이미지 경로 SIGNAL/SLOT */
    connect(panoramaForm, SIGNAL(sendPanoAdj(QPixmap&)),
            panoValueAdjustment, SLOT(receiveFile(QPixmap&)));
    /* 밝기, 대조, unsharp, deNoise, 감마 조정 SIGNAL/SLOT */
    connect(panoramaForm, SIGNAL(sendPanoValue(int, int, int, int, int)),
            panoValueAdjustment, SLOT(changePanoValue(int, int, int, int, int)));
    /* 연산 결과 pixmap 반환 SIGNAL/SLOT */
    connect(panoValueAdjustment, SIGNAL(panoImgSend(QPixmap&)),
            panoramaForm, SLOT(receieveImg(QPixmap&)));
    /* 히스토 연산을 위한 SIGNAL/SLOT */
    connect(panoramaForm, SIGNAL(sendPanoPrev(QPixmap&)),
            panoValueAdjustment, SLOT(receivePrev(QPixmap&)));

    /* panorama Preset 설정 SIGNAL/SLOT */
    connect(panoramaForm, SIGNAL(sendPanoAdj(QPixmap&)),
            panoPreset, SLOT(receiveFile(QPixmap&)));
    connect(panoramaForm, SIGNAL(sendPanoPreset(int)),
            panoPreset, SLOT(receievePreset(int)));
    connect(panoPreset, SIGNAL(panoPresetSend(QPixmap&)),
            panoramaForm, SLOT(receieveImg(QPixmap&)));

    /* preset 연산을 위한 이미지 설정 변경 SIGNAL/SLOT  */
    connect(panoPreset, SIGNAL(panoPresetAdj(QPixmap&)),
            panoValueAdjustment, SLOT(receiveSetPresetImg(QPixmap&)));
    /* preset_Reset  SIGNAL/SLOT*/
    connect(panoramaForm, SIGNAL(sendSetReset()),
            panoValueAdjustment, SLOT(setResetImg()));


    /* cephalo SIGNAL/SLOT */
    /* Load 시, 이미지 경로 SIGNAL/SLOT */
    connect(cephaloForm, SIGNAL(sendCephAdj(QPixmap&)),
            cephValueAdjustment, SLOT(receiveFile(QPixmap&)));
    /* 밝기, 대조, 블러 DeNoise Value SIGNAL/SLOT */
    connect(cephaloForm, SIGNAL(sendCephValue(int, int, int, int, int)),
            cephValueAdjustment, SLOT(changeCephValue(int, int, int, int, int)));
    /* 연산 결과 pixmap 반환 SIGNAL/SLOT */
    connect(cephValueAdjustment, SIGNAL(cephImgSend(QPixmap&)),
            cephaloForm, SLOT(receieveImg(QPixmap&)));
    /* 히스토 연산을 위한 SIGNAL/SLOT */
    connect(cephaloForm, SIGNAL(sendCephPrev(QPixmap&)),
            cephValueAdjustment, SLOT(receivePrev(QPixmap&)));

    /* cephalo Preset 설정 SIGNAL/SLOT */
    connect(cephaloForm, SIGNAL(sendCephAdj(QPixmap&)),
            cephPreset, SLOT(receiveFile(QPixmap&)));
    connect(cephaloForm, SIGNAL(sendCephPreset(int)),
            cephPreset, SLOT(receievePreset(int)));
    connect(cephPreset, SIGNAL(cephPresetSend(QPixmap&)),
            cephaloForm, SLOT(receieveImg(QPixmap&)));

    /* preset 연산을 위한 이미지 설정 변경 SIGNAL/SLOT  */
    connect(cephPreset, SIGNAL(cephPresetAdj(QPixmap&)),
            cephValueAdjustment, SLOT(receiveSetPresetImg(QPixmap&)));
    /* preset_ Reset  SIGNAL/SLOT*/
    connect(cephaloForm, SIGNAL(sendSetReset()),
            cephValueAdjustment, SLOT(setResetImg()));

    /* panorama 필터 연산을 위한 SIGNAL / SLOT */
    connect(panoramaForm, SIGNAL(sendCutOffValue(int)),
            panoValueAdjustment, SLOT(lowPassFFT(int)));
    connect(panoramaForm, SIGNAL(send2CutOffValue(int)),
            panoValueAdjustment, SLOT(highPassFFT(int)));
    connect(panoramaForm, SIGNAL(sendMedianValue(int)),
            panoValueAdjustment, SLOT(median(int)));

    /* cephalo 필터 연산을 위한 SIGNAL / SLOT */
    connect(cephaloForm, SIGNAL(sendCutOffValue(int)),
            cephValueAdjustment, SLOT(lowPassFFT(int)));
    connect(cephaloForm, SIGNAL(send2CutoffValue(int)),
            cephValueAdjustment, SLOT(highPassFFT(int)));
    connect(cephaloForm, SIGNAL(sendMedianValue(int)),
            cephValueAdjustment, SLOT(median(int)));

    /* 필터 연산 후 slider 초기화 */
    connect(panoValueAdjustment, SIGNAL(exitFilterSignal()),
            panoramaForm, SLOT(resetFilCalcValue()));
    connect(cephValueAdjustment, SIGNAL(exitFilterSignal()),
            cephaloForm, SLOT(resetFilCalcValue()));

    /* MainWindow 종료 */
    connect(panoramaForm, SIGNAL(exitPanoSignal()),
            this, SLOT(close()));
    connect(cephaloForm, SIGNAL(exitCephSignal()),
            this, SLOT(close()));

}

MainWindow::~MainWindow()
{
    qDebug()<<"Success Close MainWindow !!!";
    delete panoramaForm;
    delete cephaloForm;
    delete panoValueAdjustment;
    delete cephValueAdjustment;
    delete panoPreset;
    delete cephPreset;
    delete ui;
}
/* 메인 Form 종료 시, 종료 시그널 발생 */
void MainWindow::closeEvent(QCloseEvent *event){
    Q_UNUSED(event);

    emit closeMainWindow();
}
/* panorama 툴버튼 클릭 시, panoramaForm load 및 ui 설정 변경 */
void MainWindow::on_panoToolButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->cephToolButton->setStyleSheet("");
    ui->panoToolButton->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);");
}

/* cephalo 툴버튼 클릭 시, panoramaForm load 및 ui 설정 변경 */
void MainWindow::on_cephToolButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->panoToolButton->setStyleSheet("");
    ui->cephToolButton->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);");
}
/* DB에서 load 시 type 에 맞는 Form load.*/
void MainWindow::setReceiveMainWindow(QString type, QString cephPath, QString panoPath){
    if(type == "Pano"){
        ui->stackedWidget->setCurrentIndex(0);
        ui->cephToolButton->setStyleSheet("");
        ui->panoToolButton->setStyleSheet("background-color: rgb(35, 190, 212);"
                                          "color: rgb(255, 255, 255);");
    }
    else if(type == "Ceph"){
        ui->stackedWidget->setCurrentIndex(1);
        ui->panoToolButton->setStyleSheet("");
        ui->cephToolButton->setStyleSheet("background-color: rgb(35, 190, 212);"
                                          "color: rgb(255, 255, 255);");
    }


    emit loadPanoDB(panoPath);
    emit loadCephDB(cephPath);
}
