#include "cephaloform.h"
#include "ui_cephaloform.h"

#include <QLabel>
#include <QLineEdit>
#include <QFile>
#include <QFileDialog>
#include <QBuffer>
#include <QPainter>
#include <QIntValidator>
#include <QMessageBox>

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n

CephaloForm::CephaloForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CephaloForm)
{
    ui->setupUi(this);

    cephImageView = new CephImageView;
    cephImageView->setFixedSize(1020, 655);

    ui->verticalLayout_7->insertWidget(2, cephImageView);

    /* cephaloForm 의 이미지를 View 로 전송하는 SIGNAL/SLOT */
    connect(this, SIGNAL(sendCephView(QPixmap)),
            cephImageView, SLOT(receiveLoadImg(QPixmap)));
    /* reset 신호, 원본 이미지를 View로 전송하는 SIGNAL/SLOT */
    connect(this, SIGNAL(sendResetCeph(QPixmap&)),
            cephImageView, SLOT(receiveResetCeph(QPixmap&)));
    /* 저장 버튼에 대한 SIGNAL/SLOT */
    connect(this, SIGNAL(saveCephSignal()),
            cephImageView, SLOT(receiveSaveCeph()));
    /* View 에서 후처리한 영상을 저장하기 위해 받아오는 SIGNAL/SLOT */
    connect(cephImageView, SIGNAL(sendSave(QImage&)),
            this, SLOT(cephImageSave(QImage&)));

}

CephaloForm::~CephaloForm()
{
    delete ui;
}
/* type이 cephalo 인 DB load하는 슬롯
 * @param DB의 cephalo raw파일 경로
 */
void CephaloForm::loadDB_Data(QString cephPath){
    if(cephPath == "")  return;

    QPixmap pixmap;

    QString extension = cephPath.split("/").last().split(".").last();

    if( extension == "raw"){
        file = new QFile(cephPath);

        file->open(QFile::ReadOnly);

        QByteArray byteArray;
        byteArray = file->readAll();

        unsigned char* data = new unsigned char[ byteArray.size() ];
        memcpy( data, byteArray.data(), byteArray.size() );

        QImage image; //declare variables on header file
        QImage *temp = new QImage(data, 3000, 2400,QImage::Format_Grayscale16);

        image = *temp;

        pixmap = QPixmap::fromImage(image,Qt::AutoColor);

    }
    else if( extension != "raw"){
        pixmap.load(cephPath);
    }

    emit sendCephAdj(pixmap);

    QStringList nameStr = cephPath.split("/").last().split(".");
    QString fileName = nameStr.first();
    ui->pathLineEdit->setText(fileName);

    if(!pixmap.isNull()) {
        emit sendCephView(pixmap);
        ui->cephImgLabel->setPixmap(pixmap.scaled(panoImgLabelWidth, panoImgLabelHeight));
        defaultImg = pixmap.toImage();
        defaultPixmap =  defaultPixmap.fromImage(defaultImg.convertToFormat(QImage::Format_Grayscale8));
        ui->progressbarLabel->setText("Success Load Panorama Image !!!");
    }
    file->close();
    delete file;

    /* ui 설정 */
    ui->ceph_Preset_Button1->setStyleSheet("");
    ui->ceph_Preset_Button2->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("");
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);

    /* prograssBar 설정 */
    for(int i = 0; i <= 100; i ++)
        ui->ceph_ProgressBar->setValue(i);
}
/* Load버튼 클릭 시, 이미지를 load하는 슬롯 */
void CephaloForm::on_filePushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open file",
                                                    "C:\\Users\\KOSA\\OneDrive\\바탕 화면\\PostData");

    QPixmap pixmap;

    if(filename.length()) {          // 파일이 존재한다면
        file = new QFile(filename);

        QString extension = file->fileName().split("/").last().split(".").last();
        if( extension == "raw"){
            file->open(QFile::ReadOnly);

            QByteArray byteArray;
            byteArray = file->readAll();

            unsigned char* data = new unsigned char[ byteArray.size() ];
            memcpy( data, byteArray.data(), byteArray.size() );

            QImage image; //declare variables on header file
            QImage *temp = new QImage(data, 3000, 2400,QImage::Format_Grayscale16);
            image = *temp;

            pixmap = QPixmap::fromImage(image,Qt::AutoColor);
        }
        else if( extension != "raw"){
            pixmap.load(file->fileName());
        }

        emit sendCephAdj(pixmap);

        QStringList nameStr = file->fileName().split("/").last().split(".");
        QString fileName = nameStr.first();
        ui->pathLineEdit->setText(fileName);

        if(!pixmap.isNull()) {
            emit sendCephView(pixmap);
            ui->cephImgLabel->setPixmap(pixmap.scaled(panoImgLabelWidth, panoImgLabelHeight));
            defaultImg = pixmap.toImage();
            defaultPixmap =  defaultPixmap.fromImage(defaultImg.convertToFormat(QImage::Format_Grayscale8));
            ui->progressbarLabel->setText("Success Load Cephalo Image !!!");
        }
        file->close();
        delete file;
    }
    else {
        QMessageBox::warning(this, "Error", "Can't Load this file", QMessageBox::Ok); ;
        return;
    }
    /* ui 설정 */
    ui->ceph_Preset_Button1->setStyleSheet("");
    ui->ceph_Preset_Button2->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("");
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);

    /* prograssBar 설정 */
    for(int i = 0; i <=100; i++)
        ui->ceph_ProgressBar->setValue(i);
}
/* cephaloForm ui의 밝기 값을 처리하는 슬롯
 * @param cephaloForm의 slider 밝기 값
 */
void CephaloForm::on_brightSlider_valueChanged(int brightValue)
{
    QPixmap pixmap;

    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    contrastValue = ui->contrastSlider->value();
    sbValue = ui->sbSlider->value();
    deNoiseValue = ui->deNoiseSlider->value();
    gammaValue = ui->gammaSlider->value();
    ui->brightLineEdit->setText( QString::number(ui->brightSlider->value()) );

    /* cephalo 연산 클래스로 연산값 전달하는 SIGNAL */
    emit sendCephValue(brightValue , contrastValue, sbValue, deNoiseValue,gammaValue);
}
/* 밝기값 감소 슬롯 */
void CephaloForm::on_brightMinusButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리
    brightValue = ui->brightSlider->value();
    brightValue -= 10;

    if(brightValue < -100) return;      //정해진 값보다 작으면 예외 처리
    ui->brightSlider->setValue(brightValue);
    ui->brightLineEdit->setText( QString::number(brightValue) );
}
/* 밝기값 증가 슬롯 */
void CephaloForm::on_brightPlusButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    brightValue = ui->brightSlider->value();
    brightValue += 10;

    if(brightValue > 100) return;       //정해진 값보다 크면 예외 처리
    ui->brightSlider->setValue(brightValue);
    ui->brightLineEdit->setText( QString::number(brightValue) );
}
/* cephaloForm ui의 대조 값을 처리하는 슬롯
 * @param cephaloForm의 slider 대조 값
 */
void CephaloForm::on_contrastSlider_valueChanged(int contrastValue)
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    brightValue = ui->brightSlider->value();
    sbValue = ui->sbSlider->value();
    deNoiseValue = ui->deNoiseSlider->value();
    gammaValue = ui->gammaSlider->value();
    ui->contrastLineEdit->setText( QString::number(ui->contrastSlider->value()) );

    /* cephalo 연산 클래스로 연산값 전달하는 SIGNAL */
    emit sendCephValue(brightValue , contrastValue, sbValue, deNoiseValue, gammaValue);
}
/* 대조값 감소 슬롯 */
void CephaloForm::on_contrastMinusButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    contrastValue = ui->contrastSlider->value();
    contrastValue -= 10;

    if(contrastValue < -100) return;    //정해진 값보다 작으면 예외 처리
    ui->contrastSlider->setValue(contrastValue);
    ui->contrastLineEdit->setText( QString::number(contrastValue) );
}
/* 대조값 증가 슬롯 */
void CephaloForm::on_contrastPlusButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    contrastValue = ui->contrastSlider->value();
    contrastValue += 10;
    if(contrastValue > 100) return; //정해진 값보다 크면 예외 처리
    ui->contrastSlider->setValue(contrastValue);
    ui->contrastLineEdit->setText( QString::number(contrastValue) );
}
/* cephaloForm ui의 선예도 값을 처리하는 슬롯
 * @param cephaloForm의 slider 선예도 값
 */
void CephaloForm::on_sbSlider_valueChanged(int sbValue)
{
    if(defaultPixmap.isNull())  return;  //이미지가 load되지 않은 경우 예외 처리

    brightValue = ui->brightSlider->value();
    contrastValue = ui->contrastSlider->value();
    deNoiseValue = ui->deNoiseSlider->value();
    gammaValue = ui->gammaSlider->value();
    ui->sbLineEdit->setText( QString::number(ui->sbSlider->value()) );

    /* cephalo 연산 클래스로 연산값 전달하는 SIGNAL */
    emit sendCephValue(brightValue , contrastValue, sbValue, deNoiseValue, gammaValue);
}
/* 선예도 감소 슬롯 */
void CephaloForm::on_blurButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    sbValue = ui->sbSlider->value();
    sbValue--;

    if(sbValue < -6) return;            //정해진 값보다 작으면 예외 처리
    ui->sbSlider->setValue(sbValue);
    ui->sbLineEdit->setText( QString::number(sbValue) );
}
/* 선예도 증가 슬롯*/
void CephaloForm::on_sharpenButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    sbValue = ui->sbSlider->value();
    sbValue++;

    if(sbValue > 6) return;             //정해진 값보다 크면 예외 처리
    ui->sbSlider->setValue(sbValue);
    ui->sbLineEdit->setText( QString::number(sbValue) );
}
/* cephaloForm ui의 감마 값을 처리하는 슬롯
 * @param cephaloForm의 slider 감마 값
 */
void CephaloForm::on_gammaSlider_valueChanged(int gammaValue)
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리
    brightValue = ui->brightSlider->value();
    contrastValue = ui->contrastSlider->value();
    sbValue = ui->sbSlider->value();
    deNoiseValue = ui->deNoiseSlider->value();
    ui->gammaLineEdit->setText( QString::number(ui->gammaSlider->value()) );

    /* cephalo 연산 클래스로 연산값 전달하는 SIGNAL */
    emit sendCephValue(brightValue, contrastValue, sbValue, deNoiseValue, gammaValue);
}
/* 감마 값 감소 슬롯*/
void CephaloForm::on_gammaMinusButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    gammaValue = ui->gammaSlider->value();
    gammaValue--;

    if(gammaValue < -50) return;        //정해진 값보다 작으면 예외 처리
    ui->gammaSlider->setValue(gammaValue);
    ui->gammaLineEdit->setText( QString::number(gammaValue) );
}
/* 감마 값 증가 슬롯*/
void CephaloForm::on_gammaPlusButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리
    gammaValue = ui->gammaSlider->value();
    gammaValue++;

    if(gammaValue > 50) return;         //정해진 값보다 크면 예외 처리
    ui->gammaSlider->setValue(gammaValue);
    ui->gammaLineEdit->setText( QString::number(gammaValue) );
}
/* cephaloForm ui의 deNoise를 처리하는 슬롯
 * @param cephaloForm의 slider deNoise 값
 */
void CephaloForm::on_deNoiseSlider_valueChanged(int deNoiseValue)
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    brightValue = ui->brightSlider->value();
    contrastValue = ui->contrastSlider->value();
    sbValue = ui->sbSlider->value();
    gammaValue = ui->gammaSlider->value();
    ui->deNoiseLineEdit->setText( QString::number(ui->deNoiseSlider->value()) );

    /* cephalo 연산 클래스로 연산값 전달하는 SIGNAL */
    emit sendCephValue(brightValue, contrastValue, sbValue, deNoiseValue,gammaValue);
}
/* deNoise 값 감소 슬롯 */
void CephaloForm::on_deNoiseMinusButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리
    deNoiseValue = ui->deNoiseSlider->value();
    deNoiseValue--;

    if(deNoiseValue < 0) return;        //정해진 값보다 작으면 예외 처리
    ui->deNoiseSlider->setValue(deNoiseValue);
    ui->deNoiseLineEdit->setText( QString::number(deNoiseValue) );
}
/* deNoise 값 증가 슬롯 */
void CephaloForm::on_deNoisePlusButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리
    deNoiseValue = ui->deNoiseSlider->value();
    deNoiseValue++;

    if(deNoiseValue > 10) return;       //정해진 값보다 크면 예외 처리
    ui->deNoiseSlider->setValue(deNoiseValue);
    ui->deNoiseLineEdit->setText( QString::number(deNoiseValue) );
}

/* cephaloForm의 1번 프리셋을 처리하는 슬롯 */
void CephaloForm::on_ceph_Preset_Button1_clicked()
{
    int preset = 1;
    /* preset button ui 초기화 */
    ui->ceph_Preset_Button2->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("");
    ui->ceph_Preset_Button1->setStyleSheet("background-color: rgb(35, 190, 212);"
                                           "color: rgb(255, 255, 255);"
                                           "border: 2px solid rgb(184,191,200);");
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    emit sendCephPreset(preset);        //프리셋 연산 클래스로 번호 전송

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);
}
/* cephaloForm의 2번 프리셋을 처리하는 슬롯 */
void CephaloForm::on_ceph_Preset_Button2_clicked()
{
    int preset = 2;
    /* preset button ui 초기화 */
    ui->ceph_Preset_Button1->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("");
    ui->ceph_Preset_Button2->setStyleSheet("background-color: rgb(35, 190, 212);"
                                           "color: rgb(255, 255, 255);"
                                           "border: 2px solid rgb(184,191,200);");
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    emit sendCephPreset(preset);        //프리셋 연산 클래스로 번호 전송

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);

}
/* cephaloForm의 3번 프리셋을 처리하는 슬롯 */
void CephaloForm::on_ceph_Preset_Button3_clicked()
{
    int preset = 3;
    /* preset button ui 초기화 */
    ui->ceph_Preset_Button1->setStyleSheet("");
    ui->ceph_Preset_Button2->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("background-color: rgb(35, 190, 212);"
                                           "color: rgb(255, 255, 255);"
                                           "border: 2px solid rgb(184,191,200);");
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    emit sendCephPreset(preset);        //프리셋 연산 클래스로 번호 전송

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);
}
/* cephaloForm의 4번 프리셋을 처리하는 슬롯 */
void CephaloForm::on_ceph_Preset_Button4_clicked()
{
    int preset = 4;
    /* preset button ui 초기화 */
    ui->ceph_Preset_Button1->setStyleSheet("");
    ui->ceph_Preset_Button2->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("background-color: rgb(35, 190, 212);"
                                           "color: rgb(255, 255, 255);"
                                           "border: 2px solid rgb(184,191,200);");
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    emit sendCephPreset(preset);        //프리셋 연산 클래스로 번호 전송

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);
}
/* cephaloForm의 5번 프리셋을 처리하는 슬롯*/
void CephaloForm::on_ceph_Preset_Button5_clicked()
{
    int preset = 5;
    /* preset button ui 초기화 */
    ui->ceph_Preset_Button1->setStyleSheet("");
    ui->ceph_Preset_Button2->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("background-color: rgb(35, 190, 212);"
                                           "color: rgb(255, 255, 255);"
                                           "border: 2px solid rgb(184,191,200);");
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    emit sendCephPreset(preset);        //프리셋 연산 클래스로 번호 전송

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);
}
/* cephaloForm의 6번 프리셋을 처리하는 슬롯*/
void CephaloForm::on_ceph_Preset_Button6_clicked()
{
    int preset = 6;
    /* preset button ui 초기화 */
    ui->ceph_Preset_Button1->setStyleSheet("");
    ui->ceph_Preset_Button2->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("background-color: rgb(35, 190, 212);"
                                           "color: rgb(255, 255, 255);"
                                           "border: 2px solid rgb(184,191,200);");
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    emit sendCephPreset(preset);        //프리셋 연산 클래스로 번호 전송

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);
}
/* cephaloForm reset 슬롯 */
void CephaloForm::on_resetButton_clicked()
{
    if(defaultImg.isNull()) return; //이미지가 load되지 않은 경우 예외 처리

    ui->ceph_Preset_Button1->setStyleSheet("");
    ui->ceph_Preset_Button2->setStyleSheet("");
    ui->ceph_Preset_Button3->setStyleSheet("");
    ui->ceph_Preset_Button4->setStyleSheet("");
    ui->ceph_Preset_Button5->setStyleSheet("");
    ui->ceph_Preset_Button6->setStyleSheet("");

    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);

    prevPixmap = QPixmap();     //프리셋 이미지 초기화

    QPixmap pixmap;
    pixmap = pixmap.fromImage(defaultImg.convertToFormat(QImage::Format_Grayscale8));

    emit sendResetCeph(pixmap);  //reset 신호와 원본 이미지를 View로 전송, 시그널
    emit sendSetReset();         //cephalo 연산 클래스로 리셋 시그널 전송
}
/* cephalo 연산클래스에서의 영상 연산 결과를 반환하는 슬롯
 * @param cephalo영상의 연산 결과
 */
void CephaloForm::receieveImg(QPixmap& pixmap)
{
    prevPixmap = pixmap;
    emit sendCephView(pixmap);  //View로 연산한 영상 전송, 시그널
}
/* 저장버튼 클릭 시, View로 시그널을 전송하는 슬롯 */
void CephaloForm::on_imageSaveButton_clicked()
{
    emit saveCephSignal();//저장버튼 클릭 시그널
}
/* View에서 후처리한 cephalo 영상을 저장하는 슬롯
 * @param View에서 후처리한 cephalo Image
 */
void CephaloForm::cephImageSave(QImage& saveimg)
{
    if(defaultImg.isNull()) {
        QMessageBox::warning(this, "Error", "There are no image files to save", QMessageBox::Ok);
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, "Select a file to save", ".",
                                                    "Image File(*.raw )");
    QFile * file = new QFile(filename);
    file->open(QIODevice::WriteOnly | QIODevice::Text);
    QFileInfo fileInfo(filename);

    QByteArray byteArray = filename.toLocal8Bit();
    const char *saveFileName = byteArray.data();

    if(fileInfo.isWritable()){
        FILE* fp;

        fp = fopen(saveFileName, "wb");
        QImage image = saveimg.convertToFormat(QImage::Format_Grayscale16);
        ushort* saveFile = reinterpret_cast<ushort*>(image.bits());

        fwrite(saveFile, sizeof(unsigned short) * saveimg.width() * saveimg.height(), 1, fp);

        fclose(fp);
    }
    else {
        QMessageBox::warning(this, "Error", "Can't Save this file", QMessageBox::Ok);
    }

    file->close();
    delete file;
}
/* 평활화 버튼 클릭 시, 처리 슬롯 */
void CephaloForm::on_hePushButton_clicked()
{
    if(defaultPixmap.isNull()) return;  //이미지가 load되지 않은 경우 예외 처리

    /* 프리셋 이미지가 있으면 preset img, 없으면 원본 Img를
     * 연산 클래스로 전송하는 시그널 */
    if(prevPixmap.isNull())  emit sendCephPrev(defaultPixmap);
    else emit sendCephPrev(prevPixmap);

    /* ui 설정 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);
}
/* 종료 버튼 클릭 시, 처리 슬롯*/
void CephaloForm::on_exitButton_clicked()
{
    emit exitCephSignal();  //main Form으로 종료 시그널 전송
}
/* 필터 버튼 클릭 시, 처리 슬롯*/
void CephaloForm::on_filterPushButton_clicked()
{
    /* filterButtonForm의 객체 생성 */
    filterWidget = new FilterButtonForm;

    /* filterButtonForm에서 cephaloForm으로 파라미터 반환 SIGNAL/SLOT  */
    connect(filterWidget, SIGNAL(cephLowPassCutOff(int)),
            this, SLOT(sendFourierSignal(int)));
    connect(filterWidget, SIGNAL(cephHighPassCutOff(int)),
            this, SLOT(send2FourierSignal(int)));
    connect(filterWidget, SIGNAL(sendCephMedian(int)),
            this, SLOT(sendMedianSignal(int)));

    if (filterWidget->getTitle() == "Panorama")
        filterWidget->exit();

    filterWidget->setTitle("Cephalo");
    filterWidget->cephReadSettings();
    filterWidget->show();
}
/* low-pass filter 연산을 위해 cephalo 연산 클래스로 시그널 전송하는 슬롯
 * @parma 주파수 대역
 */
void CephaloForm::sendFourierSignal(int cutoff)
{
    emit sendCutOffValue(cutoff);
}
/* high-pass filter 연산을 위해 cephalo 연산 클래스로 시그널 전송하는 슬롯
 * @parma 주파수 대역
 */
void CephaloForm::send2FourierSignal(int cutoff)
{
    emit send2CutoffValue(cutoff);
}
/* Median filter 연산을 위해 cephalo 연산 클래스로 시그널 전송하는 슬롯
 * @parma 체크박스의 파라미터 값
 */
void CephaloForm::sendMedianSignal(int value)
{
    emit sendMedianValue(value);
}
/* 필터 연산 후, 초기화 슬롯 */
void CephaloForm::resetFilCalcValue(){
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);
}

