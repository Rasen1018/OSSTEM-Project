#ifndef PANORAMAFORM_H
#define PANORAMAFORM_H

#include "dentalimageview.h"

#include <QWidget>
#include <QObject>

class QFile;
class DentalImageView;

namespace Ui {
class PanoramaForm;
}

class PanoramaForm : public QWidget
{
    Q_OBJECT

public:
    explicit PanoramaForm(QWidget *parent = nullptr);
    ~PanoramaForm();
protected:

private slots:
    void on_filePushButton_clicked();

    void on_brightSlider_valueChanged(int value);
    void on_brightMinusButton_clicked();
    void on_brightPlusButton_clicked();

    void on_contrastSlider_valueChanged(int value);
    void on_contrastMinusButton_clicked();
    void on_contrastPlusButton_clicked();

    void on_sbSlider_valueChanged(int value);
    void on_sharpenButton_clicked();
    void on_blurButton_clicked();

    void on_preset_Button1_clicked();
    void on_preset_Button2_clicked();
    void on_preset_Button3_clicked();
    void on_preset_Button4_clicked();
    void on_preset_Button5_clicked();
    void on_preset_Button6_clicked();

    void on_resetButton_clicked();

    void text(QPixmap&);
    void receieveImg(QPixmap&);

    void on_imageSaveButton_clicked();

    void panoImageSave(QImage&);

    void on_hePushButton_clicked();

    void on_deNoisePlusButton_clicked();
    void on_deNoiseMinusButton_clicked();
    void on_deNoiseSlider_valueChanged(int value);

private:
    Ui::PanoramaForm *ui;
    QFile* file;
    QImage defaultImg;
    QPixmap defaultPixmap, prevPixmap;
    DentalImageView* dentalImageView;

    int imageWidth;
    int imageHeight;
    int panoImgLabelWidth = 360;
    int panoImgLabelHeight = 260;
    int brightValue;
    int contrastValue;
    int sbValue;
    int deNoiseValue;

signals:
    void sendPanoView(QPixmap);
    void sendPanoAdj(QPixmap&);

    void sendPanoValue(int, int, int, int);  //밝기, 대조, 필터 값

    void sendResetPano(QPixmap&);
    void savePanoSignal();

    void sendPanoPrev(QPixmap&);
    void sendPanoPreset(int);
    void sendSetReset();
};

#endif // PANORAMAFORM_H
