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

    panoramaForm = new PanoramaForm(this);
    connect(panoramaForm, SIGNAL(destroyed()),
            panoramaForm, SLOT(deleteLater()));
    cephaloForm = new CephaloForm(this);
    connect(cephaloForm, SIGNAL(destroyed()),
            cephaloForm, SLOT(deleteLater()));

    panoValueAdjustment = new PanoValueAdjustment(this);
    connect(panoValueAdjustment, SIGNAL(destroyed()),
            panoValueAdjustment, SLOT(deleteLater()));
    cephValueAdjustment = new CephValueAdjustment(this);
    connect(cephValueAdjustment, SIGNAL(destroyed()),
            cephValueAdjustment, SLOT(deleteLater()));

    panoPreset = new PanoPreset(this);
    connect(panoPreset, SIGNAL(destroyed()),
            panoPreset, SLOT(deleteLater()));
    cephPreset = new CephPreset(this);
    connect(cephPreset, SIGNAL(destroyed()),
            cephPreset, SLOT(deleteLater()));


    /* ui 설정 */
    ui->stackedWidget->insertWidget(0, panoramaForm);
    ui->stackedWidget->insertWidget(1, cephaloForm);
    ui->stackedWidget->setCurrentIndex(0);

    /* panorama SIGNAL/SLOT */
    /* Load 시, 이미지 경로 SIGNAL/SLOT */
    connect(panoramaForm, SIGNAL(sendPanoAdj(QPixmap&)),
            panoValueAdjustment, SLOT(receiveFile(QPixmap&)));
    /* 밝기, 대조, 블러 DeNoise Value SIGNAL/SLOT */
    connect(panoramaForm, SIGNAL(sendPanoValue(int, int, int, int)),
            panoValueAdjustment, SLOT(changePanoValue(int, int, int, int)));
    /* 연산 결과 pixmap 반환 SIGNAL/SLOT */
    connect(panoValueAdjustment, SIGNAL(panoImgSend(QPixmap&)),
            panoramaForm, SLOT(receieveImg(QPixmap&)));
    /* 히스토 연산을 위한 SIGNAL/SLOT */
    connect(panoramaForm, SIGNAL(sendPanoPrev(QPixmap&)),
            panoValueAdjustment, SLOT(receivePrev(QPixmap&)));

    /* Preset 설정 SIGNAL/SLOT */
    connect(panoramaForm, SIGNAL(sendPanoAdj(QPixmap&)),
            panoPreset, SLOT(receiveFile(QPixmap&)));
    connect(panoramaForm, SIGNAL(sendPanoPreset(int)),
            panoPreset, SLOT(receievePreset(int)));
    connect(panoPreset, SIGNAL(panoPresetSend(QPixmap&)),
            panoramaForm, SLOT(receieveImg(QPixmap&)));

    /* preset 연산을 위한 이미지 설정 변경 SIGNAL/SLOT  */
    connect(panoPreset, SIGNAL(panoPresetAdj(QPixmap&)),
            panoValueAdjustment, SLOT(receiveSetPresetImg(QPixmap&)));
    /* preset_ Reset  SIGNAL/SLOT*/
    connect(panoramaForm, SIGNAL(sendSetReset()),
            panoValueAdjustment, SLOT(setResetImg()));


    /* cephalo SIGNAL/SLOT */
    /* Load 시, 이미지 경로 SIGNAL/SLOT */
    connect(cephaloForm, SIGNAL(sendCephAdj(QPixmap&)),
            cephValueAdjustment, SLOT(receiveFile(QPixmap&)));
    /* 밝기, 대조, 블러 DeNoise Value SIGNAL/SLOT */
    connect(cephaloForm, SIGNAL(sendCephValue(int, int, int, int)),
            cephValueAdjustment, SLOT(changeCephValue(int, int, int, int)));
    /* 연산 결과 pixmap 반환 SIGNAL/SLOT */
    connect(cephValueAdjustment, SIGNAL(cephImgSend(QPixmap&)),
            cephaloForm, SLOT(receieveImg(QPixmap&)));
    /* 히스토 연산을 위한 SIGNAL/SLOT */
    connect(cephaloForm, SIGNAL(sendCephPrev(QPixmap&)),
            cephValueAdjustment, SLOT(receivePrev(QPixmap&)));

    /* Preset 설정 SIGNAL/SLOT */
    connect(cephaloForm, SIGNAL(sendCephAdj(QPixmap&)),
            cephPreset, SLOT(receiveFile(QPixmap&)));
    connect(cephaloForm, SIGNAL(sendCephPreset(int)),
            cephPreset, SLOT(receievePreset(int)));
    connect(cephPreset, SIGNAL(panoPresetSend(QPixmap&)),
            cephaloForm, SLOT(receieveImg(QPixmap&)));

}

MainWindow::~MainWindow()
{
    delete panoramaForm;
    delete cephaloForm;
    delete panoValueAdjustment;
    delete cephValueAdjustment;
    delete ui;
}

void MainWindow::on_panoToolButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->cephToolButton->setStyleSheet("");
    ui->panoToolButton->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);");
}

void MainWindow::on_cephToolButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->panoToolButton->setStyleSheet("");
    ui->cephToolButton->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);");

}

