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
    //unsharp mask filter 적용을 위한 (원본 - 평균값) 이미지
    void set3x3MaskValue();

    //mask를 활용한 영상 후처리 필터
    void highBoost(unsigned char* , int);
    void gaussian(unsigned char* , float);
    void ADFilter(unsigned char* ,int);

    //연산 결과 반환을 위한 QImage, QPixmap 선언
    QPixmap pixmap;
    QImage defaultImg, image, prevImg;
    QImage currentImg, calcImg;

    /* QImage -> 픽셀 단위 데이터로 변환하여
    연산 처리 수행을 위한 unsigned char 선언 */
    unsigned char* inimg;
    unsigned char  *mask, *outimg, *gammaImg, *sharpenImg, *copyImg;
    unsigned char *fftImg, *medianFilterImg, *blenImg;

    int width = 0, height = 0, imageSize = 0;

    //scaled image width, height
    int cephViewWidth = 800;
    int cephViewHeight = 600;
    double avg = 0;

signals:
    void cephImgSend(QPixmap&);     //연산된 이미지를 viewer widget으로 전달
    void exitFilterSignal();        //slider 초기화를 위한 시그널

private slots:
   void receiveFile(QPixmap&);
   //slider value : 밝기, 대조, unsharpen, deNoise
   void receivePrev(QPixmap&);      //prevImg receive
   void receiveSetPresetImg(QPixmap&);
   void setResetImg();
   void changeCephValue(int, int, int, int, int);
   void median(int value);
   void lowPassFFT(int cutoff);
   void highPassFFT(int cutoff);


};

#endif // CEPHVALUEADJUSTMENT_H
