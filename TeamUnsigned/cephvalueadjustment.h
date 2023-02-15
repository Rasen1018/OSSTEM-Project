#ifndef CEPHVALUEADJUSTMENT_H
#define CEPHVALUEADJUSTMENT_H

#include <QPixmap>
#include <QObject>

/*
 * cephalo 영상의 후처리 연산하는
 * CephValueAdjustment 클래스
 */
class CephValueAdjustment : public QObject
{
    Q_OBJECT
public:
    explicit CephValueAdjustment(QObject *parent = nullptr);

private:
    QPixmap pixmap;
    QImage defaultImg, image, prevImg;  //원본, 임시, 연산한 영상의 QImage 변수
    QImage currentImg, calcImg;         //현재 View에 보이는 영상, 연산중인 영상의 QImage 변수

    unsigned char *inimg, *mask, *outimg;              //입력, mask값, 출력을 위한 메모리
    unsigned char *gammaImg, *sharpenImg, *copyImg;    //감마, unsharpness, 연산을 위한 메모리
    unsigned char *fftImg, *medianFilterImg;           //FFT, median 연산을 위한 메모리
    unsigned char *blenImg;                            //영상 blending을 위한 메모리

    int width = 0, height = 0, imageSize = 0;       //이미지 가로, 세로, 넓이
    int cephViewWidth = 800;                        //cephalo View의 가로
    int cephViewHeight = 600;                       //cephalo View의 세로
    double avg = 0;                                 //대조값을 위한 영상의 평균 값

    void set3x3MaskValue();                         //평균값 필터를 이용한 영상의 mask 값 연산 함수

    void highBoost(unsigned char* , int);           //하이부스트 필터
    void gaussian(unsigned char* , float);          //가우시안 필터 함수
    void ADFilter(unsigned char* ,int);             //비등방성 확산 필터 함수

    void insertion(ushort a[], int n);              //정렬 함수

signals:
    void cephImgSend(QPixmap&);                     //후처리 연산 영상 결과를 panoramaForm으로 시그널 전송
    void exitFilterSignal();                        //필터 연산 후 panoramaForm으로 시그널 전송

private slots:
    void receiveFile(QPixmap&);                     //영상 load 시 연산 클래스 메모리 할당 및 설정
    void receiveSetPresetImg(QPixmap&);             //프리셋 버튼 클릭 시, 설정 슬롯
    void changeCephValue(int, int, int, int, int);  //cephaloForm의 slider 값에 따라 연산하는 슬롯
    void receivePrev(QPixmap&);                     //평탄화버튼 클릭 시, 평탄화 연산 슬롯
    void setResetImg();                             //리셋 버튼 클릭 시, 초기 설정 슬롯

    void median(int value);                         //median filter 연산 슬롯
    void lowPassFFT(int cutoff);                    //low-pass filter 연산 슬롯
    void highPassFFT(int cutoff);                   //high-pass filter 연산 슬롯

};

#endif // CEPHVALUEADJUSTMENT_H
