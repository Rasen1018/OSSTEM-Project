#include "panoramaform.h"
#include "ui_panoramaform.h"

#include <QLabel>
#include <QLineEdit>
#include <QFile>
#include <QFileDialog>
#include <QBuffer>
#include <QPainter>
#include <QMessageBox>

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n

typedef quint8 ubyte8;

PanoramaForm::PanoramaForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanoramaForm)
{
    ui->setupUi(this);

    dentalImageView = new DentalImageView;
    dentalImageView->setFixedSize(1020, 655);

    /* 필터 버튼 시그널 추가 */
    filterWidget = new FilterButtonForm;

    connect(filterWidget, SIGNAL(panoLowPassCutOff(int)),
            this, SLOT(sendFourierSignal(int)));
    connect(filterWidget, SIGNAL(panoHighPassCutOff(int)),
            this, SLOT(send2FourierSignal(int)));
    connect(filterWidget, SIGNAL(sendPanoMedian(int)),
            this, SLOT(sendMedianSignal(int)));
    /***************************************************/

    //dentalImageView->setStyleSheet("border: 1px solid rgb(184,191,200);");

    ui->verticalLayout_7->insertWidget(2, dentalImageView);

    connect(ui->exitButton, SIGNAL(clicked()), qApp, SLOT(closeAllWindows()) ); //종료 버튼

    /* Send PanoramaImg to View*/
    connect(this, SIGNAL(sendPanoView(QPixmap)),
            dentalImageView, SLOT(receiveLoadImg(QPixmap)));
    /* Reset PanoImage SIGNAL/SLOT */
    connect(this, SIGNAL(sendResetPano(QPixmap&)),
            dentalImageView, SLOT(receiveResetPano(QPixmap&)));
    /* SaveSignal SIGNAL/SLOT */
    connect(this, SIGNAL(savePanoSignal()),
            dentalImageView, SLOT(receiveSavePano()));
    /* panoimg save 하기위한 SIGNAL/SLOT */
    connect(dentalImageView, SIGNAL(sendSave(QImage&)),
            this, SLOT(panoImageSave(QImage&)));

}

PanoramaForm::~PanoramaForm()
{
    delete ui;
}

/********************************************************************************************/
/********************************************************************************************/
void PanoramaForm::on_brightSlider_valueChanged(int brightValue)
{
    QPixmap pixmap;

    if(defaultPixmap.isNull())  return;
    else
    {
        contrastValue = ui->contrastSlider->value();
        sbValue = ui->sbSlider->value();
        deNoiseValue = ui->deNoiseSlider->value();
        emit sendPanoValue(brightValue , contrastValue, sbValue, deNoiseValue);
    }
    ui->brightLineEdit->setText( QString::number(ui->brightSlider->value()) );
}



void PanoramaForm::on_brightMinusButton_clicked()
{
    if(defaultPixmap.isNull())  return;
    else {
        brightValue = ui->brightSlider->value();
        brightValue -= 10;
        if(brightValue < -100) return;
        ui->brightSlider->setValue(brightValue);
        ui->brightLineEdit->setText( QString::number(brightValue) );
    }
}

void PanoramaForm::on_brightPlusButton_clicked()
{
    if(defaultPixmap.isNull())  return;
    else {
        brightValue = ui->brightSlider->value();
        brightValue += 10;
        if(brightValue > 100) return;
        ui->brightSlider->setValue(brightValue);
        ui->brightLineEdit->setText( QString::number(brightValue) );

    }
}



/********************************************************************************************/
void PanoramaForm::on_contrastSlider_valueChanged(int contrastValue)
{   if(defaultPixmap.isNull())  return;

    QPixmap pixmap;
    QImage image = defaultImg;

    brightValue = ui->brightSlider->value();
    sbValue = ui->sbSlider->value();
    deNoiseValue = ui->deNoiseSlider->value();

    emit sendPanoValue(brightValue , contrastValue, sbValue, deNoiseValue);


    ui->contrastLineEdit->setText( QString::number(ui->contrastSlider->value()) );

}
void PanoramaForm::on_contrastMinusButton_clicked()
{
    if(defaultPixmap.isNull())  return;
    else {
        contrastValue = ui->contrastSlider->value();
        contrastValue -= 10;
        if(contrastValue < -100) return;
        ui->contrastSlider->setValue(contrastValue);
        ui->contrastLineEdit->setText( QString::number(contrastValue) );

    }
}

void PanoramaForm::on_contrastPlusButton_clicked()
{
    if(defaultPixmap.isNull())  return;
    else {
        contrastValue = ui->contrastSlider->value();
        contrastValue += 10;
        if(contrastValue > 100) return;
        ui->contrastSlider->setValue(contrastValue);
        ui->contrastLineEdit->setText( QString::number(contrastValue) );

    }
}

/********************************************************************************************/
void PanoramaForm::on_sharpenButton_clicked()
{
    if(defaultPixmap.isNull())  return;

    sbValue = ui->sbSlider->value();
    sbValue--;

    if(sbValue < -6) return;
    ui->sbSlider->setValue(sbValue);
    ui->sbLineEdit->setText( QString::number(sbValue) );
}

void PanoramaForm::on_blurButton_clicked()
{
    if(defaultPixmap.isNull())  return;

    sbValue = ui->sbSlider->value();
    sbValue++;

    if(sbValue > 6) return;
    ui->sbSlider->setValue(sbValue);
    ui->sbLineEdit->setText( QString::number(sbValue) );
}


void PanoramaForm::on_sbSlider_valueChanged(int sbValue)
{
    if(defaultPixmap.isNull())  return;

    brightValue = ui->brightSlider->value();
    contrastValue = ui->contrastSlider->value();
    deNoiseValue = ui->deNoiseSlider->value();

    emit sendPanoValue(brightValue , contrastValue, sbValue, deNoiseValue);

    ui->sbLineEdit->setText( QString::number(ui->sbSlider->value()) );
}
void PanoramaForm::on_deNoisePlusButton_clicked()
{
    if(defaultPixmap.isNull())  return;

    deNoiseValue = ui->deNoiseSlider->value();
    deNoiseValue++;

    if(deNoiseValue > 10) return;
    ui->deNoiseSlider->setValue(deNoiseValue);
    ui->deNoiseLineEdit->setText( QString::number(deNoiseValue) );
}

void PanoramaForm::on_deNoiseMinusButton_clicked()
{
    if(defaultPixmap.isNull())  return;

    deNoiseValue = ui->deNoiseSlider->value();
    deNoiseValue--;

    if(deNoiseValue < 0) return;
    ui->deNoiseSlider->setValue(deNoiseValue);
    ui->deNoiseLineEdit->setText( QString::number(deNoiseValue) );

}
void PanoramaForm::on_deNoiseSlider_valueChanged(int deNoiseValue)
{
    if(defaultPixmap.isNull())  return;
    brightValue = ui->brightSlider->value();
    contrastValue = ui->contrastSlider->value();
    sbValue = ui->sbSlider->value();

    emit sendPanoValue(brightValue, contrastValue, sbValue, deNoiseValue);

    ui->deNoiseLineEdit->setText( QString::number(ui->deNoiseSlider->value()) );
    \
}


/********************************************************************************************/
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
    if(defaultPixmap.isNull())  return;

    emit sendPanoPreset(preset);

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
}

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

    if(defaultPixmap.isNull())  return;

    emit sendPanoPreset(preset);

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
}

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
    if(defaultPixmap.isNull())  return;

    emit sendPanoPreset(preset);

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
}

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
    if(defaultPixmap.isNull())  return;

    emit sendPanoPreset(preset);

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
}

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
    if(defaultPixmap.isNull())  return;

    emit sendPanoPreset(preset);

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
}
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
    if(defaultPixmap.isNull())  return;

    emit sendPanoPreset(preset);

    /* Image Send 후 value 초기화 */
    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
}


/********************************************************************************************/
void PanoramaForm::on_resetButton_clicked()
{
    if(defaultImg.isNull())
        return;

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

    QPixmap pixmap;
    pixmap = pixmap.fromImage(defaultImg.convertToFormat(QImage::Format_Grayscale8));

    emit sendResetPano(pixmap);
    emit sendSetReset();
}


void PanoramaForm::on_imageSaveButton_clicked()
{
    emit savePanoSignal();
}

void PanoramaForm::on_filePushButton_clicked()
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

    for(int i = 0; i <= 100; i ++)
        ui->panoProgressBar->setValue(i);
}

void PanoramaForm::text(QPixmap &pixmap)
{
    ui->panoImgLabel->setPixmap(QPixmap());
    ui->panoImgLabel->setPixmap(pixmap.scaled(1105, 600));
}

void PanoramaForm::receieveImg(QPixmap& pixmap)
{
    prevPixmap = pixmap;
    emit sendPanoView(pixmap);
}

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

void PanoramaForm::on_hePushButton_clicked()
{
    if(defaultPixmap.isNull()) return;

    /* preset Img가 있으면 preset, 없으면 원본 Img */
    if(prevPixmap.isNull())  emit sendPanoPrev(defaultPixmap);
    else emit sendPanoPrev(prevPixmap);

    ui->brightSlider->setValue(0);
    ui->contrastSlider->setValue(0);
    ui->sbSlider->setValue(0);
    ui->deNoiseSlider->setValue(0);
}


void PanoramaForm::on_filterPushButton_clicked()
{
    if (filterWidget->getTitle() == "Cephalo")
        filterWidget->exit();

    filterWidget->setTitle("Panorama");
    filterWidget->show();
}

/******************** 시그널/ 슬롯 추가 **********************/
void PanoramaForm::sendFourierSignal(int cutoff) {

    emit sendCutOffValue(cutoff);
}

void PanoramaForm::send2FourierSignal(int cutoff) {
    emit send2CutOffValue(cutoff);
}

void PanoramaForm::sendMedianSignal(int value) {

    emit sendMedianValue(value);
}
