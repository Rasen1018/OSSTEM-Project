#include "fourierprocessing.h"
#include "qdebug.h"

#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte8;
/*
 **** fourier Logic ****
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
    //이미지 크기를 2의 지수승으로 padding
    pow2width = 1024;
    pow2height = 1024;
    //원본 이미지 사이즈 저장
    scaledWidth = width, scaledHeight = height;

    //2의 지수승 사이즈 입력 이미지
    m_InputImage = (ubyte8*)malloc(sizeof(ubyte8) * pow2width * pow2height);
    memset(m_InputImage, 0, sizeof(ubyte8) * pow2width * pow2height);

    //2의 지수승 사이즈 출력 이미지
    m_OutputImage = (ubyte8*)malloc(sizeof(ubyte8) * pow2width * pow2height);
    memset(m_InputImage, 0, sizeof(ubyte8) * pow2width * pow2height);

    //0으로 원본에서 나머지 픽셀 데이터 padding
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

    //임시 2차원 이미지 배열 생성 후 초기화
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
            //입력의 한 행을 복사, 실수 성분 값은 영상의 값
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
            //푸리에 변환 함수 절대값(푸리에 스펙트럼)
            Value = sqrt((m_FFT[y][x].re * m_FFT[y][x].re) +
                         (m_FFT[y][x].im * m_FFT[y][x].im));
            //로그 변환
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
            //입력의 한 행을 복사, 실수 성분 값은 영상의 값
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

/* 1차원 푸리에 변환(입력 데이터, 데이터 갯수, log2N) */
void FourierProcessing::OnFFT1d(Complex *X, int N, int Log2N)
{
    OnShuffle(X, N, Log2N);     // 입력 데이터 순서 바꾸기
    if (direction) OnButterfly(X, N, Log2N, 1);      // 고속 푸리에 변환 알고리즘 수행(순방향)
    else if (!direction) OnButterfly(X, N, Log2N, 2);       // 고속 푸리에 변환 알고리즘 수행(역방향)

}

/* 입력 데이터 순서를 바꾸기 위한 함수(입력 데이터, 데이터 갯수, log2N)
 * 고속 푸리에 변환 알고리즘 수행을 위해 원본 데이터를 짝수번째, 홀수번째 데이터로 정렬 */
void FourierProcessing::OnShuffle(Complex *X, int N, int Log2N)
{
    int i;
    Complex *temp;

    temp = new Complex[N];

    /* 입력 데이터를 짝수번째, 홀수번째 데이터로 정렬 */
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

/* 고속 푸리에 변환(FFT) 알고리즘 구현 함수(입력 데이터, 데이터 갯수, log2N, 방향) */
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

        // FFT 알고리즘 순방향
        if(mode == 1) {
            for(j=0; j < (int)(Value/2); j++) {
                Y[j].re = cos(j*2.0*PI / Value);
                Y[j].im = -sin(j*2.0*PI / Value);
            }
        }
        // FFT 알고리즘 역방향
        if(mode == 2) {
            for(j=0; j < (int)(Value/2); j++) {
                Y[j].re = cos(j*2.0*PI / Value);
                Y[j].im = sin(j*2.0*PI / Value);
            }
        }

        start = 0;

        // 버터플라이 알고리즘 수행
        for(k = 0; k < N / (int)Value; k++) {
            for(j = start; j < start+(int)(Value/2); j++) {

                //짝수 번째 데이터들과 홀수 번째 데이터들을 이용하여 푸리에 변환
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
    // FFT 알고리즘 역방향
    if (mode == 2) {
        for(i = 0; i < N; i++) {
            X[i].re = X[i].re / N;
            X[i].im = X[i].im / N;
        }
    }

    delete[] Y;
}

/* 비트 반전 구현 함수(데이터 인덱스, log2N) */
int FourierProcessing::OnReverseBitOrder(int index, int Log2N)
{
    int i, X, Y;

    Y = 0;

    //시프트 연산으로 비트 반전
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

/* 저역 통과 필터 : 영상의 주파수 성분 중에서 저주파에 해당하는 성분만 통과시키는 함수
 * 필터 중앙에서 1의 값을 가지면서 주변으로 갈수록 그 값이 점진적으로 작아진다
 * 차단 주파수 값이 커지면 더 많은 저주파 성분을 통과시키고, 작아지면 적은 양의 저주파 성분을 통과 */
void FourierProcessing::lowPassGaussian(ubyte8* outimg, int cutoff)
{                                    //(결과 저장할 이미지, 차단 주파수)

    int i, j, x = 0, y = 0;
    double dist2, lowPass;

    /* 푸리에 변환 */
    OnFFT2d(outimg);

    //스펙트럼 영상은 대칭이라 1/2 지점에 가장 강한 고주파 성분 표현
    for (i = 0; i < pow2height; i++) {
        for (j = 0; j < pow2width; j++) {
            y = i;
            x = j;
            if (y > pow2height / 2)
                y = y - pow2height;
            if (x > pow2width / 2)
                x = x - pow2width;

            /* 가우시안 저역 통과 필터 수식 */
            dist2 = static_cast<double>(x*x + y*y);
            lowPass = exp(-dist2 / (2 * cutoff * cutoff));

            //필터링 결과 저장
            m_FFT[i][j].re = m_FFT[i][j].re * lowPass;
            m_FFT[i][j].im = m_FFT[i][j].im * lowPass;
        }
    }
    /* 푸리에 역변환 */
    OnIFFT2d(outimg);
}

/* 고역 통과 필터 : 영상의 주파수 성분 중에서 고주파에 해당하는 성분만 통과시키는 함수
 * 차단 주파수가 증가함에 따라 저주파 성분이 더욱 적게 통과 */
void FourierProcessing::highFrequencyPass(ubyte8* outimg, double cutoff)
{
    int i, j, x = 0, y = 0;
    double dist2, highPass;

    /* 푸리에 변환 */
    OnFFT2d(outimg);

    //스펙트럼 영상은 대칭이라 1/2 지점에 가장 강한 고주파 성분 표현
    for (i=0; i < pow2height; i++) {
        for (j = 0; j < pow2width; j++) {
            y = i;
            x = j;
            if (y > pow2height / 2)
                y = y - pow2height;
            if (x > pow2width / 2)
                x = x - pow2width;

            /* 가우시안 고역 통과 필터 수식 */
            dist2 = static_cast<double>(x*x + y*y);
            highPass = 1.0 - exp(-dist2 / (2 * cutoff * cutoff));

            //필터링 결과 저장
            m_FFT[i][j].re = m_FFT[i][j].re * highPass;
            m_FFT[i][j].im = m_FFT[i][j].im * highPass;
        }
    }
    /* 푸리에 역변환 */
    OnIFFT2d(outimg);
}
