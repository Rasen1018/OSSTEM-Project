#include "fourierprocessing.h"
#include "qdebug.h"

#define LIMIT_UBYTE(n) ((n)>UCHAR_MAX)?UCHAR_MAX:((n)<0)?0:(n)

typedef unsigned char ubyte8;
/*
 * *** fourier Logic *** *
 fourier.SetImage(img);
 fourier.FFT(1);
    if (dlg.m_nFilterShape == 0)
        fourier.LowPassIdeal(dlg.m_nCutoff);
    else
        fourier.LowPassGaussian(dlg.m_nCutoff);
 fourier.FFT(-1);
*/

FourierProcessing::FourierProcessing(int width, int height, const uchar* inimg)
{
    pow2width = 1024;
    pow2height = 1024;
    scaledWidth = width, scaledHeight = height;

    m_InputImage = (ubyte8*)malloc(sizeof(ubyte8) * pow2width * pow2height);
    memset(m_InputImage, 0, sizeof(ubyte8) * pow2width * pow2height);

    m_OutputImage = (ubyte8*)malloc(sizeof(ubyte8) * pow2width * pow2height);
    memset(m_InputImage, 0, sizeof(ubyte8) * pow2width * pow2height);

    for(int y = 0; y < scaledHeight; y++) {
        for(int x = 0; x < scaledWidth; x++) {
            m_InputImage[x + y * pow2width] = inimg[x + y * scaledWidth];
        }
    }
}

void FourierProcessing::deleteMemory() {

    free(*m_FFT);
    free(*m_IFFT);
    free(m_tempImage);
    free(m_InputImage);
    free(m_OutputImage);
}

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

    while(Num >= 2)
    {
        Num >>= 1;
        Log2N++;
    }

    m_tempImage = Image2DMem(pow2height, pow2width);
    Data = new Complex[pow2width];

    m_FFT = new Complex* [pow2height];
    // 주파수 영역 변환 영상을 저장하기 위한 배열
    temp = new ubyte8* [pow2height];

    for(y = 0; y < pow2height; y++) {
        m_FFT[y] = new Complex [pow2width];// m_FFT[i] = new Complex[width];
        temp[y] = new ubyte8[pow2width];
    }

    for(y = 0; y < pow2height; y++) {
        for (x = 0; x < pow2width; x++) {
            // 입력의 한 행을 복사, 실수 성분 값은 영상의 값
            Data[x].re = (double)m_InputImage[y * pow2width + x];
            Data[x].im = 0.0;
        }

        OnFFT1d(Data, pow2width, Log2N);

        for (x = 0; x < pow2width; x++) {
            m_FFT[y][x].re = Data[x].re;
            m_FFT[y][x].im = Data[x].im;
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
            Data[y].re = m_FFT[y][x].re;
            Data[y].im = m_FFT[y][x].im;
        }

        OnFFT1d(Data, pow2height, Log2N);

        for (y = 0; y < pow2height; y++) {
            m_FFT[y][x].re = Data[y].re;
            m_FFT[y][x].im = Data[y].im;
        }
    }

    for(y = 0; y < pow2height; y++) {
        for(x=0; x < pow2width; x++) {
            Value = sqrt((m_FFT[y][x].re * m_FFT[y][x].re) +
                         (m_FFT[y][x].im * m_FFT[y][x].im));
            Absol = 20 * log(Value);

            if(Absol > 255.0)
                Absol = 255.0;
            if(Absol < 0.0)
                Absol = 0.0;

            m_tempImage[y][x] = Absol;
        }
    }

    // 셔플링 과정
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
                    outimg[x + y*scaledWidth] = m_OutputImage[(pow2width-x) + (pow2height-y) * pow2width];
                }
            }
        }
    }

    delete[] Data;
}


void FourierProcessing::OnFFT1d(Complex *X, int N, int Log2N)
{
    OnShuffle(X, N, Log2N);
    if (direction) OnButterfly(X, N, Log2N, 1);
    else if (!direction) OnButterfly(X, N, Log2N, 2);

}


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

        if(mode == 1) {
            for(j=0; j < (int)(Value/2); j++) {
                Y[j].re = cos(j*2.0*PI / Value);
                Y[j].im = sin(j*2.0*PI / Value);
            }
        }

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
    if (mode == 2) {
        for(i = 0; i < N; i++) {
            X[i].re = X[i].re / N;
            X[i].im = X[i].im / N;
        }
    }

    delete[] Y;
}


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

    int i, j, x, y;
    double dist2, hval;

    OnFFT2d(outimg);

    for (i = 0; i < pow2height; i++)
    for (j = 0; j < pow2width; j++)
    {
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

    OnIFFT2d(outimg);
}


void FourierProcessing::highFrequencyPass(ubyte8* outimg, double value)
{
    int i, j, x, y = 0;
    double temp, D, N;
    D = value;
    N = 4.0;

    OnFFT2d(outimg);

    for (i=0; i < pow2height; i++) {
        for (j = 0; j < pow2width; j++) {
            y = i;
            x = j;
            if (y > pow2height / 2)
                y = y - pow2height;
            if (x > pow2width / 2)
                x = x - pow2width;

            temp = 1.0 / (1.0 + pow(D / sqrt((double)(x*x + y*y)), 2*N));

            m_FFT[i][j].re = m_FFT[i][j].re * temp;
            m_FFT[i][j].im = m_FFT[i][j].im * temp;
        }
    }    
    OnIFFT2d(outimg);
}






