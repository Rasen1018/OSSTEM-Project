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
    QPixmap pixmap, sharpenPixmap;
    QImage defaultImg, image, prevImg;
    QImage currentImg;

    //const uchar* inimg;
    unsigned char* inimg;
    unsigned char  *mask, *outimg, *sharpenImg, *copyImg;

    int width = 0, height = 0, imageSize = 0;
    int dentalViewWidth = 1000;
    int dentalViewHeight = 600;
    double avg = 0;

    void set3x3MaskValue();

    void highBoost(int);
    void blur3x3(int);
    void blur5x5();

    void sharpen(int);// 세팔로 샤픈 임시 저장
    void gaussian(float);
    void ADFilter(unsigned char* ,int);

private slots:
    void receiveFile(QPixmap&);  //defaultImg receive 수정 해야댈 듯
    void changePanoValue(int, int, int, int);   //밝기, 대조, unsharp, deNoise
    void receivePrev(QPixmap&); //prevImg receive
    void receiveSetPresetImg(QPixmap&);
    void setResetImg();

signals:
    void panoImgSend(QPixmap&);
};

#endif // PANOVALUEADJUSTMENT_H
