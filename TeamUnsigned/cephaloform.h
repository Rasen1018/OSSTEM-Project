#ifndef CEPHALOFORM_H
#define CEPHALOFORM_H

#include "cephimageview.h"

#include <QWidget>

class QFile;
class CephImageView;

namespace Ui {
class CephaloForm;
}

class CephaloForm : public QWidget
{
    Q_OBJECT

public:
    explicit CephaloForm(QWidget *parent = nullptr);
    ~CephaloForm();

private:
    Ui::CephaloForm *ui;
    QFile* file;
    QImage defaultImg;
    QPixmap defaultPixmap, prevPixmap;
    CephImageView *cephImageView;
    int imageWidth;
    int imageHeight;
    int panoImgLabelWidth = 360;
    int panoImgLabelHeight = 287;

    int brightValue;
    int contrastValue;
    int sbValue;
    int deNoiseValue;

private slots:
    void on_brightSlider_valueChanged(int value);
    void on_brightMinusButton_clicked();
    void on_brightPlusButton_clicked();

    void on_contrastSlider_valueChanged(int value);
    void on_contrastMinusButton_clicked();
    void on_contrastPlusButton_clicked();

    void on_sbSlider_valueChanged(int value);
    void on_sharpenButton_clicked();
    void on_blurButton_clicked();

    void on_ceph_Preset_Button1_clicked();
    void on_ceph_Preset_Button2_clicked();
    void on_ceph_Preset_Button3_clicked();
    void on_ceph_Preset_Button4_clicked();
    void on_ceph_Preset_Button5_clicked();
    void on_ceph_Preset_Button6_clicked();

    void on_resetButton_clicked();
    void on_imageSaveButton_clicked();
    void on_filePushButton_clicked();

    void text(QPixmap&);
    void receieveImg(QPixmap&);
    void cephImageSave(QImage&);

    void on_deNoisePlusButton_clicked();

    void on_deNoiseMinusButton_clicked();

    void on_deNoiseSlider_valueChanged(int value);

    void on_hePushButton_clicked();

signals:

    void sendCephView(QPixmap);
    void sendCephAdj(QPixmap&);

    void sendCephValue(int, int, int, int);  //밝기, 대조, 필터 값

    void sendResetCeph(QPixmap&);
    void saveCephSignal();

    void sendCephPrev(QPixmap&);
    void sendCephPreset(int);
    void sendSetReset();
};

#endif // CEPHALOFORM_H
