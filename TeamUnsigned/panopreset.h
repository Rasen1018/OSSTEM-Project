#ifndef PANOPRESET_H
#define PANOPRESET_H

#include <QObject>
#include <QImage>
#include <QPixmap>

/*
 * panorama 영상의 프리셋 연산을 사용하는
 * PanoPreset 클래스
 */
class PanoPreset : public QObject
{
    Q_OBJECT
public:
    explicit PanoPreset(QObject *parent = nullptr);

private:
    QImage defaultImg, image;       //원본, 연산한 영상의 QImage 변수

    /* 1~6번 까지의 프리셋 영상의 QImage 변수 */
    QImage presetImg1, presetImg2, presetImg3
          , presetImg4, presetImg5, presetImg6;

    QPixmap pixmap;                 //영상의 QPixmap 변수
    unsigned char * inimg, *mask, *outimg;  //입력, mask값, 출력을 위한 메모리
    unsigned char *copyImg, *copyImg2;      //연산을 위한 메모리
    unsigned char * fftImg;                 //푸리에 변환을 위한 메모리

    int width, height, imageSize;   //이미지 가로, 세로, 넓이
    int dentalViewWidth = 1000;     //View의 가로
    int dentalViewHeight = 600;     //View의 세로

    double avg = 0; //대조값을 위한 영상의 평균 값

    /* 1~6번까지의 프리셋 설정 */
    void setPreset_1();
    void setPreset_2();
    void setPreset_3();
    void setPreset_4();
    void setPreset_5();
    void setPreset_6();

    void set3x3MaskValue();         //평균값 필터를 이용한 영상의 mask 값

    unsigned char* highBoost(unsigned char*, int);          //하이부스트 필터
    unsigned char* ADFilter(unsigned char* ,int);           //비등방성 확산 필터
    unsigned char* lowPassFFT(unsigned char*, int cutoff);  //저역통과 필터

signals:
    void panoPresetSend(QPixmap&);  //panoramaForm으로 프리셋 영상 전송
    void panoPresetAdj(QPixmap&);   //panorama 연산 클래스로 프리셋 영상 전송

private slots:
    void receiveFile(QPixmap&);     //영상 load 시, 영상 프리셋 연산 슬롯
    void receievePreset(int);       //panoramaForm에서 입력받은 번호에 따른 영상 반환 슬롯
};

#endif // PANOPRESET_H
