#ifndef FOURIERPROCESSING_H
#define FOURIERPROCESSING_H

#include <QObject>

/* 푸리에 변환에 의해 생성된 함수 F(u)는 복소수 공간에서 정의되기 때문에
 * 푸리에 변환을 구현할 때에는 함수 F(u)의 실수부와 허수부를 따로 고려하여 계산
 * 실수부와 허수부 계산을 위한 구조체 */
struct Complex {
    double re;      //실수부
    double im;      //허수부
};

class FourierProcessing : public QObject
{
    Q_OBJECT
public:
    // FourierProcessing(원본 너비, 원본 높이, 원본 이미지 배열)
    explicit FourierProcessing(int width, int height, const uchar*);

    /* 메모리 정리 함수 */
    void deleteMemory();
    /* 푸리에 변환 */
    void OnFFT2d(unsigned char* outimg);
    /* 푸리에 역변환 */
    void OnIFFT2d(unsigned char* outimg);

    // 저역 주파수만 가우시안 필터링
    void lowPassGaussian(unsigned char* outimg, int value);
    // 고역 주파수만 필터링
    void highFrequencyPass(unsigned char* outimg, double value);

private:
    /* direction에 따라 순방향/역방향 변환 결정 */
    void OnFFT1d(Complex *X, int N, int Log2N);
    /* 시프트 변환 함수 */
    void OnShuffle(Complex *X, int N, int Log2N);
    /* 버터플라이 알고리즘 */
    void OnButterfly(Complex *X, int N, int Log2N, int mode);
    /* 입력 데이터 순서를 바꾸기 위한 비트 반전 함수 */
    int OnReverseBitOrder(int index, int Log2N);
    /* 임시 영상 배열 생성 함수 */
    double** Image2DMem(int height, int width);

    bool direction = true;  // 푸리에 변환, 역변환을 결정하는 dir
    int pow2width;          // 2의 지수승의 width
    int pow2height;         // 2의 지수승의 height
    int scaledWidth;        // 원본 width
    int scaledHeight;       // 원본 height

    Complex** m_FFT;        // 푸리에 변환을 위한 구조체
    Complex** m_IFFT;       // 푸리에 역변환을 위한 구조체
    double** m_tempImage;   // 푸리에 변환 연산 저장을 위한 임시 배열
    unsigned char* m_InputImage;       // 2의 지수승으로 padding된 입력 이미지 배열
    unsigned char* m_OutputImage;      // 2의 지수승으로 padding된 출력 이미지 배열
};

#endif // FOURIERPROCESSING_H
