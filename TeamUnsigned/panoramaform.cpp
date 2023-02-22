#include "panoramaform.h"
#include "ui_panoramaform.h"

#include <QLabel>
#include <QLineEdit>
#include <QFile>
#include <QFileDialog>
#include <QBuffer>
#include <QPainter>
#include <QMessageBox>
#include <QScreen>

#include "filterbuttonform.h"

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n

PanoramaForm::PanoramaForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanoramaForm)
{
    ui->setupUi(this);

    dentalImageView = new DentalImageView;  //panoramaView 객체 생성

    dentalImageView->setFixedSize(1020, 655);

    ui->verticalLayout_7->insertWidget(2, dentalImageView);

    /* panoramaForm 의 이미지를 View 로 전송하는 SIGNAL/SLOT */
    connect(this, SIGNAL(sendPanoView(QPixmap)),
            dentalImageView, SLOT(receiveLoadImg(QPixmap)));
    /* reset 신호, 원본 이미지를 View로 전송하는 SIGNAL/SLOT */
    connect(this, SIGNAL(sendResetPano(QPixmap&)),
            dentalImageView, SLOT(receiveResetPano(QPixmap&)));
    /* 저장 버튼에 대한 SIGNAL/SLOT */
    connect(this, SIGNAL(savePanoSignal()),
            dentalImageView, SLOT(receiveSavePano()));
    /* View 에서 후처리한 영상을 저장하기 위해 받아오는 SIGNAL/SLOT */
    connect(dentalImageView, SIGNAL(sendSave(QImage&)),
            this, SLOT(panoImageSave(QImage&)));


}

PanoramaForm::~PanoramaForm()
{
    delete ui;
}

/* type이 panorama 인 DB load하는 슬롯
 * @param DB의 panorama raw파일 경로
 */
void PanoramaForm::loadDB_Data(QString panoPath){
    if(panoPath == "")return;

    QPixmap pixmap;
    QString extension = panoPath.split("/").last().split(".").last();

    if( extension == "raw"){
        file = new QFile(panoPath);

        file->open(QFile::ReadOnly);

        QByteArray byteArray;
        byteArray = file->readAll();

        unsigned char* data = new unsigned char[ byteArray.size() ];
        memcpy( data, byteArray.data(), byteArray.size() );

        QImage image; //declare variables on header file
        QImage *temp = new QImage(data, 3000, 1628,QImage::Format_Grayscale16);

        image = *temp;

        pixmap = QPixmap::fromImage(image,Qt::AutoColor);
    }
    else if( extension != "raw"){
        pixmap.load(panoPath);
    }

    emit sendPanoAdj(pixmap);

    QStringList nameStr = panoPath.split("/").last().split(".");
    QString fileName = nameStr.first();
    ui->pathLineEdit->setText(fileName);

    if(!pixmap.isNull()) {
        emit sendPanoView(pixmap);
        ui->panoImgLabel->setPixmap(pixmap.scaled(panoImgLabelWidth, panoImgLabelHeight));
        defaultImg = pixmap.toImage();
        defaultPixmap =  defaultPixmap.fromImage(defaultImg.convertToFormat(QImage::Format_Grayscale8));
        ui->progressbarLabel->setText("Success Load Panorama Image !!!");

    }
    file->close();
    delete file;

    /* ui 설정 */
    ui->preset_Button1->setStyleSheet("");
    ui->preset_Button2->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("");
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);

    /* prograssBar 설정 */
    for(int i = 0; i <= 100; i ++)
        ui->panoProgressBar->setValue(i);
}
/* Load버튼 클릭 시, 이미지를 load하는 슬롯 */
void PanoramaForm::on_filePushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open file",
                                                    "C:\\");
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
            QImage *temp = new QImage(data, 3000, 1628,QImage::Format_Grayscale16);
            image = *temp;

            pixmap = QPixmap::fromImage(image,Qt::AutoColor);
        }
        else if( extension != "raw"){
            pixmap.load(file->fileName());
        }

        emit sendPanoAdj(pixmap);

        QStringList nameStr = file->fileName().split("/").last().split(".");
        QString fileName = nameStr.first();
        ui->pathLineEdit->setText(fileName);

        if(!pixmap.isNull()) {
            emit sendPanoView(pixmap);
            ui->panoImgLabel->setPixmap(pixmap.scaled(panoImgLabelWidth, panoImgLabelHeight));
            defaultImg = pixmap.toImage();
            defaultPixmap =  defaultPixmap.fromImage(defaultImg.convertToFormat(QImage::Format_Grayscale8));
            ui->progressbarLabel->setText("Success Load Panorama Image !!!");
        }
        file->close();
        delete file;
    }
    else {
        QMessageBox::warning(this, "Error", "Can't Load this file", QMessageBox::Ok); ;
        return;
    }
    /* ui 설정 */
    ui->preset_Button1->setStyleSheet("");
    ui->preset_Button2->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("");
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);

    /* prograssBar 설정 */
    for(int i = 0; i <= 100; i ++)
        ui->panoProgressBar->setValue(i);

    prevPixmap = QPixmap();     //프리셋 이미지 초기화

    pixmap = pixmap.fromImage(defaultImg.convertToFormat(QImage::Format_Grayscale8));

    emit sendResetPano(pixmap); //reset 신호와 원본 이미지를 View로 전송, 시그널
    emit sendSetReset();        //panorama 연산 클래스로 리셋 시그널 전송
}
/* panoramaForm ui의 밝기 값을 처리하는 슬롯
 * @param panoramaForm의 slider 밝기 값
 */
void PanoramaForm::on_brightSlider_valueChanged(int brightValue)
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    contrastValue = ui->contrastSlider->value();
    sbValue = ui->sbSlider->value();
    deNoiseValue = ui->deNoiseSlider->value();
    gammaValue = ui->gammaSlider->value();
    ui->brightLineEdit->setText( QString::number(ui->brightSlider->value()) );

    /* panorama 연산 클래스로 연산값 전달하는 SIGNAL */
    emit sendPanoValue(brightValue , contrastValue, sbValue, deNoiseValue,gammaValue);
}
/* 밝기값 감소 슬롯 */
void PanoramaForm::on_brightMinusButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리
    brightValue = ui->brightSlider->value();
    brightValue -= 10;

    if(brightValue < -100) return;  //정해진 값보다 작으면 예외 처리
    ui->brightSlider->setValue(brightValue);
    ui->brightLineEdit->setText( QString::number(brightValue) );

}
/* 밝기값 증가 슬롯 */
void PanoramaForm::on_brightPlusButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리
    brightValue = ui->brightSlider->value();
    brightValue += 10;

    if(brightValue > 100) return;   //정해진 값보다 크면 예외 처리
    ui->brightSlider->setValue(brightValue);
    ui->brightLineEdit->setText( QString::number(brightValue) );
}
/* panoramaForm ui의 대조 값을 처리하는 슬롯
 * @param panoramaForm의 slider 대조 값
 */
void PanoramaForm::on_contrastSlider_valueChanged(int contrastValue)
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    brightValue = ui->brightSlider->value();
    sbValue = ui->sbSlider->value();
    deNoiseValue = ui->deNoiseSlider->value();
    gammaValue = ui->gammaSlider->value();
    ui->contrastLineEdit->setText( QString::number(ui->contrastSlider->value()) );

    /* panorama 연산 클래스로 연산값 전달하는 SIGNAL */
    emit sendPanoValue(brightValue , contrastValue, sbValue, deNoiseValue, gammaValue);
}
/* 대조값 감소 슬롯 */
void PanoramaForm::on_contrastMinusButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리
    contrastValue = ui->contrastSlider->value();
    contrastValue -= 10;

    if(contrastValue < -100) return;    //정해진 값보다 작으면 예외 처리
    ui->contrastSlider->setValue(contrastValue);
    ui->contrastLineEdit->setText( QString::number(contrastValue) );
}
/* 대조값 증가 슬롯 */
void PanoramaForm::on_contrastPlusButton_clicked()
{
    if(defaultPixmap.isNull())  return;  //이미지가 load되지 않은 경우 예외 처리
    contrastValue = ui->contrastSlider->value();
    contrastValue += 10;

    if(contrastValue > 100) return;      //정해진 값보다 크면 예외 처리
    ui->contrastSlider->setValue(contrastValue);
    ui->contrastLineEdit->setText( QString::number(contrastValue) );

}
/* panoramaForm ui의 선예도 값을 처리하는 슬롯
 * @param panoramaForm의 slider 선예도 값
 */
void PanoramaForm::on_sbSlider_valueChanged(int sbValue)
{
    if(defaultPixmap.isNull())  return;  //이미지가 load되지 않은 경우 예외 처리

    brightValue = ui->brightSlider->value();
    contrastValue = ui->contrastSlider->value();
    deNoiseValue = ui->deNoiseSlider->value();
    gammaValue = ui->gammaSlider->value();
    ui->sbLineEdit->setText( QString::number(ui->sbSlider->value()) );

    /* panorama 연산 클래스로 연산값 전달하는 SIGNAL */
    emit sendPanoValue(brightValue , contrastValue, sbValue, deNoiseValue,gammaValue);
}
/* 선예도 감소 슬롯 */
void PanoramaForm::on_blurButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    sbValue = ui->sbSlider->value();
    sbValue--;

    if(sbValue < -6) return;            //정해진 값보다 작으면 예외 처리
    ui->sbSlider->setValue(sbValue);
    ui->sbLineEdit->setText( QString::number(sbValue) );
}
/* 선예도 증가 슬롯*/
void PanoramaForm::on_sharpenButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    sbValue = ui->sbSlider->value();
    sbValue++;

    if(sbValue > 6) return;             //정해진 값보다 크면 예외 처리
    ui->sbSlider->setValue(sbValue);
    ui->sbLineEdit->setText( QString::number(sbValue) );
}
/* panoramaForm ui의 감마 값을 처리하는 슬롯
 * @param panoramaForm의 slider 감마 값
 */
void PanoramaForm::on_gammaSlider_valueChanged(int gammaValue)
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리
    brightValue = ui->brightSlider->value();
    contrastValue = ui->contrastSlider->value();
    sbValue = ui->sbSlider->value();
    deNoiseValue = ui->deNoiseSlider->value();
    ui->gammaLineEdit->setText( QString::number(ui->gammaSlider->value()) );

    /* panorama 연산 클래스로 연산값 전달하는 SIGNAL */
    emit sendPanoValue(brightValue, contrastValue, sbValue, deNoiseValue, gammaValue);
}
/* 감마 값 감소 슬롯*/
void PanoramaForm::on_gammaMinusButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리
    gammaValue = ui->gammaSlider->value();
    gammaValue--;

    if(gammaValue < -50) return;        //정해진 값보다 작으면 예외 처리
    ui->gammaSlider->setValue(gammaValue);
    ui->gammaLineEdit->setText( QString::number(gammaValue) );
}
/* 감마 값 증가 슬롯*/
void PanoramaForm::on_gammaPlusButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리
    gammaValue = ui->gammaSlider->value();
    gammaValue++;

    if(gammaValue > 50) return;         //정해진 값보다 크면 예외 처리
    ui->gammaSlider->setValue(gammaValue);
    ui->gammaLineEdit->setText( QString::number(gammaValue) );
}
/* panoramaForm ui의 deNoise를 처리하는 슬롯
 * @param panoramaForm의 slider deNoise 값
 */
void PanoramaForm::on_deNoiseSlider_valueChanged(int deNoiseValue)
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    brightValue = ui->brightSlider->value();
    contrastValue = ui->contrastSlider->value();
    sbValue = ui->sbSlider->value();
    gammaValue = ui->gammaSlider->value();
    ui->deNoiseLineEdit->setText( QString::number(ui->deNoiseSlider->value()) );

    /* panorama 연산 클래스로 연산값 전달하는 SIGNAL */
    emit sendPanoValue(brightValue, contrastValue, sbValue, deNoiseValue,gammaValue);
}
/* deNoise 값 감소 슬롯 */
void PanoramaForm::on_deNoiseMinusButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리
    deNoiseValue = ui->deNoiseSlider->value();
    deNoiseValue--;

    if(deNoiseValue < 0) return;        //정해진 값보다 작으면 예외 처리
    ui->deNoiseSlider->setValue(deNoiseValue);
    ui->deNoiseLineEdit->setText( QString::number(deNoiseValue) );
}
/* deNoise 값 증가 슬롯 */
void PanoramaForm::on_deNoisePlusButton_clicked()
{
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리
    deNoiseValue = ui->deNoiseSlider->value();
    deNoiseValue++;

    if(deNoiseValue > 10) return;       //정해진 값보다 크면 예외 처리
    ui->deNoiseSlider->setValue(deNoiseValue);
    ui->deNoiseLineEdit->setText( QString::number(deNoiseValue) );
}

/* panoramaForm의 1번 프리셋을 처리하는 슬롯 */
void PanoramaForm::on_preset_Button1_clicked()
{
    int preset = 1;

    /* preset button ui 초기화 */
    ui->preset_Button2->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("");
    ui->preset_Button1->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);"
                                      "border: 2px solid rgb(184,191,200);");
    if(defaultPixmap.isNull())  return;  //이미지가 load되지 않은 경우 예외 처리

    emit sendPanoPreset(preset);         //프리셋 연산 클래스로 번호 전송

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);
}
/* panoramaForm의 2번 프리셋을 처리하는 슬롯 */
void PanoramaForm::on_preset_Button2_clicked()
{
    int preset = 2;

    /* preset button ui 초기화 */
    ui->preset_Button1->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("");
    ui->preset_Button2->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);"
                                      "border: 2px solid rgb(184,191,200);");

    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    emit sendPanoPreset(preset);        //프리셋 연산 클래스로 번호 전송

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);

}
/* panoramaForm의 3번 프리셋을 처리하는 슬롯 */
void PanoramaForm::on_preset_Button3_clicked()
{
    int preset = 3;

    /* preset button ui 초기화 */
    ui->preset_Button1->setStyleSheet("");
    ui->preset_Button2->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);"
                                      "border: 2px solid rgb(184,191,200);");
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    emit sendPanoPreset(preset);        //프리셋 연산 클래스로 번호 전송

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);
}
/* panoramaForm의 4번 프리셋을 처리하는 슬롯 */
void PanoramaForm::on_preset_Button4_clicked()
{
    int preset = 4;
    /* preset button ui 초기화 */
    ui->preset_Button1->setStyleSheet("");
    ui->preset_Button2->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);"
                                      "border: 2px solid rgb(184,191,200);");
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    emit sendPanoPreset(preset);        //프리셋 연산 클래스로 번호 전송

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);
}
/* panoramaForm의 5번 프리셋을 처리하는 슬롯*/
void PanoramaForm::on_preset_Button5_clicked()
{
    int preset = 5;

    /* preset button ui 초기화 */
    ui->preset_Button1->setStyleSheet("");
    ui->preset_Button2->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);"
                                      "border: 2px solid rgb(184,191,200);");
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    emit sendPanoPreset(preset);        //프리셋 연산 클래스로 번호 전송

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);
}
/* panoramaForm의 6번 프리셋을 처리하는 슬롯*/
void PanoramaForm::on_preset_Button6_clicked()
{
    int preset = 6;
    /* preset button ui 초기화 */
    ui->preset_Button1->setStyleSheet("");
    ui->preset_Button2->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("background-color: rgb(35, 190, 212);"
                                      "color: rgb(255, 255, 255);"
                                      "border: 2px solid rgb(184,191,200);");
    if(defaultPixmap.isNull())  return; //이미지가 load되지 않은 경우 예외 처리

    emit sendPanoPreset(preset);        //프리셋 연산 클래스로 번호 전송

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);
}
/* panoramaForm reset 슬롯 */
void PanoramaForm::on_resetButton_clicked()
{
    if(defaultImg.isNull()) return; //이미지가 load되지 않은 경우 예외 처리

    /* ui 설정 */
    ui->preset_Button1->setStyleSheet("");
    ui->preset_Button2->setStyleSheet("");
    ui->preset_Button3->setStyleSheet("");
    ui->preset_Button4->setStyleSheet("");
    ui->preset_Button5->setStyleSheet("");
    ui->preset_Button6->setStyleSheet("");
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);

    prevPixmap = QPixmap();     //프리셋 이미지 초기화

    QPixmap pixmap;
    pixmap = pixmap.fromImage(defaultImg.convertToFormat(QImage::Format_Grayscale8));

    emit sendResetPano(pixmap); //reset 신호와 원본 이미지를 View로 전송, 시그널
    emit sendSetReset();        //panorama 연산 클래스로 리셋 시그널 전송
}
/* panorama 연산클래스에서의 영상 연산 결과를 반환하는 슬롯
 * @param panorama영상의 연산 결과
 */
void PanoramaForm::receieveImg(QPixmap& pixmap)
{
    prevPixmap = pixmap;
    emit sendPanoView(pixmap);  //View로 연산한 영상 전송, 시그널
}
/* 저장버튼 클릭 시, View로 시그널을 전송하는 슬롯 */
void PanoramaForm::on_imageSaveButton_clicked()
{
    emit savePanoSignal();  //저장버튼 클릭 시그널
}
/* View에서 후처리한 panorama 영상을 저장하는 슬롯
 * @param View에서 후처리한 panorama Image
 */
void PanoramaForm::panoImageSave(QImage& saveimg)
{
    if(defaultImg.isNull()) {
        QMessageBox::warning(this, "Error", "There are no image files to save", QMessageBox::Ok);
        return;
    }
    else {
        QString filename = QFileDialog::getSaveFileName(this, "Select a file to save", ".",
                                                        "Image File(*.raw)");
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
}
/* 평활화 버튼 클릭 시, 처리 슬롯 */
void PanoramaForm::on_hePushButton_clicked()
{
    if(defaultPixmap.isNull()) return;  //이미지가 load되지 않은 경우 예외 처리

    /* 프리셋 이미지가 있으면 preset img, 없으면 원본 Img를
     * 연산 클래스로 전송하는 시그널 */
    if(prevPixmap.isNull())  emit sendPanoPrev(defaultPixmap);
    else emit sendPanoPrev(prevPixmap);

    /* ui 설정 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);
}
/* 종료 버튼 클릭 시, 처리 슬롯*/
void PanoramaForm::on_exitButton_clicked(){
    emit exitPanoSignal();   //main Form으로 종료 시그널 전송
}
/* 필터 버튼 클릭 시, 처리 슬롯*/
void PanoramaForm::on_filterPushButton_clicked()
{
    /* filterButtonForm의 객체 생성 */
    filterWidget = new FilterButtonForm;

    /* filterButtonForm에서 panoramaForm으로 파라미터 반환 SIGNAL/SLOT  */
    connect(filterWidget, SIGNAL(panoLowPassCutOff(int)),
            this, SLOT(sendFourierSignal(int)));
    connect(filterWidget, SIGNAL(panoHighPassCutOff(int)),
            this, SLOT(send2FourierSignal(int)));
    connect(filterWidget, SIGNAL(sendPanoMedian(int)),
            this, SLOT(sendMedianSignal(int)));

    filterWidget->setTitle("Panorama");
    filterWidget->show();

}
/* low-pass filter 연산을 위해 cephalo 연산 클래스로 시그널 전송하는 슬롯
 * @parma 주파수 대역
 */
void PanoramaForm::sendFourierSignal(int cutoff)
{
    emit sendCutOffValue(cutoff);
}
/* high-pass filter 연산을 위해 cephalo 연산 클래스로 시그널 전송하는 슬롯
 * @parma 주파수 대역
 */
void PanoramaForm::send2FourierSignal(int cutoff)
{
    emit send2CutOffValue(cutoff);
}
/* Median filter 연산을 위해 panorama 연산 클래스로 시그널 전송하는 슬롯
 * @parma 체크박스의 파라미터 값
 */
void PanoramaForm::sendMedianSignal(int value)
{
    emit sendMedianValue(value);
}
/* 필터 연산 후, 초기화 슬롯 */
void PanoramaForm::resetFilCalcValue(){
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
    ui->gammaSlider->setValue(0);
}


