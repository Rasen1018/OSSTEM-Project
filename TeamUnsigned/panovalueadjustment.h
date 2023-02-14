#ifndef PANOVALUEADJUSTMENT_H
#define PANOVALUEADJUSTMENT_H

#include <QPixmap>
#include <QObject>

class QPixmap;
class PanoValueAdjustment : public QObject
{
    Q_OBJECT
public:
    explicit PanoValueAdjustment(QObject *parent = nullptr);

private:
    QPixmap pixmap;
    QImage defaultImg, image, prevImg;
    QImage currentImg, calcImg;

    unsigned char* inimg, *mask, *outimg;
    unsigned char *gammaImg, *sharpenImg, *copyImg;
    unsigned char *fftImg, *medianFilterImg;

    int width = 0, height = 0, imageSize = 0;
    int dentalViewWidth = 1000;
    int dentalViewHeight = 600;
    double avg = 0;

    void set3x3MaskValue();

    void highBoost(unsigned char*, int);
    void gaussian(unsigned char*, float);
    void ADFilter(unsigned char* ,int);

private slots:
    void insertion(ushort a[], int n);
    void receiveFile(QPixmap&);  //defaultImg receive 수정 해야댈 듯
    void changePanoValue(int, int, int, int, int);   //밝기, 대조, unsharp, deNoise, gamma
    void receivePrev(QPixmap&); //prevImg receive
    void receiveSetPresetImg(QPixmap&);
    void setResetImg();
    void median(int value);
    void lowPassFFT(int cutoff);
    void highPassFFT(int cutoff);

signals:
    void panoImgSend(QPixmap&);
    void exitFilterSignal();
};

#endif // PANOVALUEADJUSTMENT_H
