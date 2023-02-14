#ifndef CEPHPRESET_H
#define CEPHPRESET_H

#include <QImage>
#include <QPixmap>

class CephPreset : public QObject
{
    Q_OBJECT
public:
    explicit CephPreset(QObject *parent = nullptr);

private slots:
    void receievePreset(int);
    void receiveFile(QPixmap&);

signals:
    void cephPresetSend(QPixmap&);
    void cephPresetAdj(QPixmap&);

private:
    QImage defaultImg, image;
    QImage presetImg1,presetImg2,presetImg3,presetImg4,presetImg5,presetImg6;

    QPixmap pixmap;
    unsigned char * inimg, *mask, *outimg ;
    unsigned char *copyImg, *copyImg2;
    unsigned char *fftImg, *blendImg;

    int width, height, imageSize;
    int cephViewWidth = 800;
    int cephViewHeight = 600;
    double avg = 0;

    void setPreset_1();
    void setPreset_2();
    void setPreset_3();
    void setPreset_4();
    void setPreset_5();
    void setPreset_6();

    void set3x3MaskValue();

    unsigned char* highBoost(unsigned char*, int);
    unsigned char* ADFilter(unsigned char* ,int);

    unsigned char* lowPassFFT(unsigned char*, int cutoff);

};

#endif // CEPHPRESET_H
