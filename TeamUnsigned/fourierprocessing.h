#ifndef FOURIERPROCESSING_H
#define FOURIERPROCESSING_H

#include <QObject>

struct Complex {
    double re;
    double im;
};

class FourierProcessing : public QObject
{
    Q_OBJECT
public:
    explicit FourierProcessing(int width, int height, const uchar*);

    void deleteMemory();
    /* 푸리에 변환 */
    void OnFFT2d(unsigned char* outimg);
    /* 푸리에 역변환 */
    void OnIFFT2d(unsigned char* outimg);
    // 주파수 공간에서의 필터링 함수
    void lowPassGaussian(unsigned char* outimg, int value);
    void highFrequencyPass(unsigned char* outimg, double value);

signals:


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

    bool direction = true;
    int pow2width;
    int pow2height;
    int scaledWidth;
    int scaledHeight;

    Complex** m_FFT;
    Complex** m_IFFT;
    double** m_tempImage;
    unsigned char* m_InputImage;
    unsigned char* m_OutputImage;
};

// 전역 함수 선언
void FFT1d(double* re, double* im, int N, int dir);
bool IsPowerOf2(int n);
//void DFT1d(double* re, double* im, int N, int dir);

#endif // FOURIERPROCESSING_H
