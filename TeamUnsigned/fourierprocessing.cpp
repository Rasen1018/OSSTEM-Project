#include "fourierprocessing.h"
#include "qdebug.h"

#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte8;
/*
 * *** fourier Logic *** *
 FourierProcessing(width, height, inimg);
 OnFFT2d();
 OnShuffle();
 OnFFT1d();
 lowPassGaussian(); ||  highFrequencyPass();
 OnFFT1d();
 OnShuffle();
 OnIFFT2d();
*/

FourierProcessing::FourierProcessing(int width, int height, const uchar* inimg)
{
    // 이미지 크기를 2의 지수승으로 padding
    pow2width = 1024;
    pow2height = 1024;
    // 원본 이미지 사이즈 저장
    scaledWidth = width, scaledHeight = height;

    // 2의 지수승 사이즈 입력 이미지
    m_InputImage = (ubyte8*)malloc(sizeof(ubyte8) * pow2width * pow2height);
    memset(m_InputImage, 0, sizeof(ubyte8) * pow2width * pow2height);

    // 2의 지수승 사이즈 출력 이미지
    m_OutputImage = (ubyte8*)malloc(sizeof(ubyte8) * pow2width * pow2height);
    memset(m_InputImage, 0, sizeof(ubyte8) * pow2width * pow2height);

    // 0으로 원본에서 나머지 픽셀 데이터 padding
    for(int y = 0; y < scaledHeight; y++) {
        for(int x = 0; x < scaledWidth; x++) {
            m_InputImage[x + y * pow2width] = inimg[x + y * scaledWidth];
        }
    }
}

/* 배열 메모리 정리 */
void FourierProcessing::deleteMemory() {

    free(*m_FFT);
    free(*m_IFFT);
    free(m_tempImage);
    free(m_InputImage);
    free(m_OutputImage);
}

/* 2차원 푸리에 변환 */
void FourierProcessing::OnFFT2d(ubyte8* outimg)
{
    Q_UNUSED(outimg);
    int x, y, row, col, Log2N, Num;
    Complex *Data;

    ubyte8 **temp;
    double Value, Absol;

    direction = true;

    Num = pow2width;
    Log2N = 0;

    /* log2N 계산 */
    while(Num >= 2)
    {
        Num >>= 1;  // 2보다 작아질 때까지 오른쪽으로 1칸씩 시프트 연산
        Log2N++;
    }

    // 임시 2차원 이미지 배열 생성 후 초기화
    m_tempImage = Image2DMem(pow2height, pow2width);

    Data = new Complex[pow2width];
    m_FFT = new Complex* [pow2height];
    temp = new ubyte8* [pow2height];

    /* 행 단위 푸리에 변환 */
    for(y = 0; y < pow2height; y++) {
        m_FFT[y] = new Complex [pow2width];
        temp[y] = new ubyte8[pow2width];
    }

    for(y = 0; y < pow2height; y++) {
        for (x = 0; x < pow2width; x++) {
            // 입력의 한 행을 복사, 실수 성분 값은 영상의 값
            Data[x].re = (double)m_InputImage[y * pow2width + x];
            Data[x].im = 0.0;
        }

        //1차원 푸리에 변환
        OnFFT1d(Data, pow2width, Log2N);

        //로그 변환 후 푸리에 스펙트럼을 실수부, 허수부에 저장
        for (x = 0; x < pow2width; x++) {
            m_FFT[y][x].re = Data[x].re;
            m_FFT[y][x].im = Data[x].im;
        }
    }

    Num = pow2height;
    Log2N = 0;

    /* log2N 계산 */
    while(Num >= 2) {
        Num >>= 1;
        Log2N++;
    }

    Data = new Complex[pow2height];

    /* 열단위 푸리에 변환 */
    for(x = 0; x<pow2width; x++) {
        for(y = 0; y<pow2height; y++) {
            Data[y].re = m_FFT[y][x].re;
            Data[y].im = m_FFT[y][x].im;
        }

        //1차원 푸리에 변환
        OnFFT1d(Data, pow2height, Log2N);

        //로그 변환후 푸리에 스펙트럼을 실수부, 허수부에 저장
        for (y = 0; y < pow2height; y++) {
            m_FFT[y][x].re = Data[y].re;
            m_FFT[y][x].im = Data[y].im;
        }
    }

    /* 복소수 값의 절댓값을 로그 변환하여 저장.
     * 푸리에 변환 결괏값을 0~255 사이의 값으로 변환 */
    for(y = 0; y < pow2height; y++) {
        for(x=0; x < pow2width; x++) {
            // 푸리에 변환 함수 절대값(푸리에 스펙트럼)
            Value = sqrt((m_FFT[y][x].re * m_FFT[y][x].re) +
                         (m_FFT[y][x].im * m_FFT[y][x].im));
            // 로그 변환
            Absol = 20 * log(Value);

            //0~255 값만 들어오도록 경계 처리
            if(Absol > 255.0)
                Absol = 255.0;
            if(Absol < 0.0)
                Absol = 0.0;

            //푸리에 스펙트럼을 영상으로 저장
            m_tempImage[y][x] = Absol;
        }
    }

    /* 셔플링 과정 :
     * 영상을 4등분하고 분할된 영상을 상하 대칭 및 좌우 대칭 */
    for(y =0; y < pow2height; y += pow2height / 2) {
        for(x =0; x < pow2width; x += pow2width / 2) {
            for (row = 0; row < pow2height / 2; row++) {
                for (col = 0; col < pow2width / 2; col++) {
                    temp[(pow2height/2-1) - row + y][(pow2width/2-1) - col + x] =
                            (ubyte8)m_tempImage[y+row][x+col];
                }
            }
        }
    }

    //푸리에 스펙트럼 영상 저장
    for(y=0; y < pow2height; y++) {
        for(x = 0; x < pow2width; x++) {
            m_OutputImage[y*pow2width + x] = temp[y][x];
        }
    }

    delete[] Data, **temp;
}


void FourierProcessing::OnIFFT2d(ubyte8* outimg)
{
    int x, y, Log2N, Num;
    Complex *Data;

    direction = false;

    Num = pow2width;
    Log2N = 0;

    while(Num >= 2)
    {
        Num >>= 1;
        Log2N++;
    }

    Data = new Complex[pow2height];
    m_IFFT = new Complex* [pow2height];

    for(y = 0; y < pow2height; y++) {
        m_IFFT[y] = new Complex [pow2width];// m_FFT[i] = new Complex[width];
    }

    for(y = 0; y < pow2height; y++) {
        for (x = 0; x < pow2width; x++) {
            // 입력의 한 행을 복사, 실수 성분 값은 영상의 값
            Data[x].re = m_FFT[y][x].re;
            Data[x].im = m_FFT[y][x].im;
        }

        OnFFT1d(Data, pow2width, Log2N);

        for (x = 0; x < pow2width; x++) {
            m_IFFT[y][x].re = Data[x].re;
            m_IFFT[y][x].im = Data[x].im;
        }
    }

    Num = pow2height;
    Log2N = 0;

    while(Num >= 2) {
        Num >>= 1;
        Log2N++;
    }

    Data = new Complex[pow2height];

    for(x = 0; x<pow2width; x++) {
        for(y = 0; y<pow2height; y++) {
            Data[y].re = m_IFFT[y][x].re;
            Data[y].im = m_IFFT[y][x].im;
        }

        OnFFT1d(Data, pow2height, Log2N);

        for (y = 0; y < pow2height; y++) {
            m_IFFT[y][x].re = Data[y].re;
            m_IFFT[y][x].im = Data[y].im;
        }
    }

    for(y=0; y < pow2height; y++) {
        for(x = 0; x < pow2width; x++) {
            m_OutputImage[y*pow2width + x] = (ubyte8)m_IFFT[y][x].re;
        }
    }

    for(y=0; y < pow2height; y++) {
        for(x = 0; x < pow2width; x++) {

            if (0 <= x && x < scaledWidth) {
                if (0 <= y && y < scaledHeight) {
                    outimg[x + y*scaledWidth] = m_OutputImage[x + y * pow2width];
                }
            }
        }
    }

    delete[] Data;
}

/* 1차원 푸리에 변환(data, 너비 or 높이, log2N) */
void FourierProcessing::OnFFT1d(Complex *X, int N, int Log2N)
{
    OnShuffle(X, N, Log2N);     // 입력 데이터 바꾸기
    if (direction) OnButterfly(X, N, Log2N, 1);      // 버터플라이 알고리즘 수행(순방향)
    else if (!direction) OnButterfly(X, N, Log2N, 2);       // 버터플라이 알고리즘 수행(역방향)

}

/* 입력 데이터 순서를 바꾸기 위한 함수(data, 너비 or 높이, log2N) */
void FourierProcessing::OnShuffle(Complex *X, int N, int Log2N)
{
    int i;
    Complex *temp;

    temp = new Complex[N];

    for (i = 0; i < N; i++) {
        temp[i].re = X[OnReverseBitOrder(i, Log2N)].re;
        temp[i].im = X[OnReverseBitOrder(i, Log2N)].im;
    }

    for (i = 0; i < N; i++) {
        X[i].re = temp[i].re;
        X[i].im = temp[i].im;
    }

    delete[] temp;

}

/* 버터플라이 알고리즘 구현 함수(data, 너비 or 높이, log2N, 방향) */
void FourierProcessing::OnButterfly(Complex *X, int N, int Log2N, int mode)
{
    int i, j , k, m = 0;
    int start;
    double Value;
    double PI = 3.14159265358979;

    Complex *Y, temp;

    Y = new Complex[N/2];

    for(i=0; i <Log2N; i++) {
        Value = pow(2, i+1);

        // 알고리즘 순방향
        if(mode == 1) {
            for(j=0; j < (int)(Value/2); j++) {
                Y[j].re = cos(j*2.0*PI / Value);
                Y[j].im = -sin(j*2.0*PI / Value);
            }
        }
        // 알고리즘 역방향
        if(mode == 2) {
            for(j=0; j < (int)(Value/2); j++) {
                Y[j].re = cos(j*2.0*PI / Value);
                Y[j].im = sin(j*2.0*PI / Value);
            }
        }

        start = 0;

        for(k = 0; k < N / (int)Value; k++) {
            for(j = start; j < start+(int)(Value/2); j++) {

                m = j + (int)(Value/2);
                temp.re = Y[j-start].re * X[m].re
                        - Y[j-start].im * X[m].im;
                temp.im = Y[j-start].im * X[m].re
                        + Y[j-start].re * X[m].im;

                X[m].re = X[j].re - temp.re;
                X[m].im = X[j].im - temp.im;

                X[j].re = X[j].re + temp.re;
                X[j].im = X[j].im + temp.im;
            }
            start = start + (int)Value;
        }
    }
    // 푸리에 역변환
    if (mode == 2) {
        for(i = 0; i < N; i++) {
            X[i].re = X[i].re / N;
            X[i].im = X[i].im / N;
        }
    }

    delete[] Y;
}

/* 비트 반전 구현 함수 */
int FourierProcessing::OnReverseBitOrder(int index, int Log2N)
{
    int i, X, Y;

    Y = 0;

    for(i=0; i < Log2N; i++) {
        X = (index & (1<<i)) >> i;
        Y = (Y << 1) | X;
    }

    return Y;
}

/* 임시 2차원 배열 초기화 함수 */
double** FourierProcessing::Image2DMem(int height, int width) {

    double** temp;
    int x, y;
    temp = new double* [height];

    for (y=0; y < height; y++) {
        temp[y] = new double[width];
    }
    for (y=0; y < height; y++) {
        for (x=0; x < width; x++) {
            temp[y][x] = 0.0;
        }
    } // 할당된 2차원 메모리를 초기화

    return temp;
}


void FourierProcessing::lowPassGaussian(ubyte8* outimg, int cutoff)
{

    int i, j, x = 0, y = 0;
    double dist2, hval;

    OnFFT2d(outimg);

    for (i = 0; i < pow2height; i++) {
        for (j = 0; j < pow2width; j++) {
            y = i;
            x = j;
            if (y > pow2height / 2)
                y = y - pow2height;
            if (x > pow2width / 2)
                x = x - pow2width;

            dist2 = static_cast<double>(x*x + y*y);

            hval = exp(-dist2 / (2 * cutoff * cutoff));

            m_FFT[i][j].re = m_FFT[i][j].re * hval;
            m_FFT[i][j].im = m_FFT[i][j].im * hval;
        }
    }
    OnIFFT2d(outimg);
}


void FourierProcessing::highFrequencyPass(ubyte8* outimg, double cutoff)
{
    int i, j, x = 0, y = 0;
    double temp, D, N;
    D = cutoff;
    N = 4.0;

    double dist2, hval;
    OnFFT2d(outimg);

    for (i=0; i < pow2height; i++) {
        for (j = 0; j < pow2width; j++) {
            y = i;
            x = j;
            if (y > pow2height / 2)
                y = y - pow2height;
            if (x > pow2width / 2)
                x = x - pow2width;

//            temp = 1.0 / (1.0 + pow(D / sqrt((double)(x*x + y*y)), 2*N));

//            m_FFT[i][j].re = m_FFT[i][j].re * temp;
//            m_FFT[i][j].im = m_FFT[i][j].im * temp;

            dist2 = static_cast<double>(x*x + y*y);

            hval = 1.0 - exp(-dist2 / (2 * cutoff * cutoff));

            m_FFT[i][j].re = m_FFT[i][j].re * hval;
            m_FFT[i][j].im = m_FFT[i][j].im * hval;
        }
    }    
    OnIFFT2d(outimg);
}






