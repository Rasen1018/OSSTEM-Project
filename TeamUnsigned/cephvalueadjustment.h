#ifndef CEPHVALUEADJUSTMENT_H
#define CEPHVALUEADJUSTMENT_H

#include <QPixmap>
#include <QObject>

class CephValueAdjustment : public QObject
{
    Q_OBJECT
public:
    explicit CephValueAdjustment(QObject *parent = nullptr);

private:
    QPixmap pixmap, sharpenPixmap;
    QImage defaultImg, image, prevImg;
    QImage currentImg;

    unsigned short *inimg;
    unsigned short *mask, *outimg, *sharpenImg, *copyImg;

    int width = 0, height = 0, imageSize = 0;

    int cephViewWidth = 800;
    int cephViewHeight = 600;
    double avg = 0;

    void set3x3MaskValue();

   void highBoost(int);
   void blur3x3(int);
   void blur5x5();

   void sharpen(int);// 세팔로 샤픈 임시 저장
   void gaussian(float);
   void ADFilter(unsigned short* ,int);

private slots:
   void receiveFile(QPixmap&);  //defaultImg receive 수정 해야댈 듯
   void changeCephValue(int, int, int, int);   //밝기, 대조, unsharp, deNoise
   void receivePrev(QPixmap&); //prevImg receive
   void receiveSetPresetImg(QPixmap&);
   void setResetImg();

signals:
    void cephImgSend(QPixmap&);
};

#endif // CEPHVALUEADJUSTMENT_H
