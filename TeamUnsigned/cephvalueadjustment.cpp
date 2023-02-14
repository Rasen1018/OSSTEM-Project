#include "cephvalueadjustment.h"
#include "fourierprocessing.h"
#include "qdebug.h"

#include <QImage>

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n
#define PI 3.1416926535f

/* 정렬을 위한 전역함수 */
void insertion(ushort a[], int n)
{
    int i, j;
    for (i = 1; i < n; i++) {
        int tmp = a[i];
        for (j = i; j > 0 && a[j - 1] > tmp; j--)
            a[j] = a[j - 1];
        a[j] = tmp;
    }
}

CephValueAdjustment::CephValueAdjustment(QObject *parent)
    : QObject{parent}
{
}

/* unsharp mask filter 적용을 위한 (원본 - 평균값) 이미지 출력 함수 */
void CephValueAdjustment::set3x3MaskValue()
{
    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(mask, 0, sizeof(unsigned char) * imageSize);

    int arr[9] = {0};

    int x = 0, y = -1, cnt = 0;
    for(int i = 0; i < imageSize; i ++) {
        x = i % width;
        if(i % width == 0) y++;

        /* padding image 메모리 없이 지역 배열로 처리 */
        if(x==0){
            //LeftUpVertex
            if(y==0) {
                arr[0] = arr[1] = arr[3] = arr[4] = blenImg[x+(y*width) ];
                arr[2] = arr[5] = blenImg[x+1 + (y*width) ];
                arr[6] = arr[7] = blenImg[x+ ((y+1)*width) ];
                arr[8] = blenImg[x+1+((y+1)*width) ];
            }
            //LeftDownVertex
            else if(y==height-1) {
                arr[0] = arr[1] =blenImg[x+((y-1)*width) ];
                arr[2] = blenImg[x+1 + ((y-1)*width) ];
                arr[3] = arr[6] = arr[7] = arr[4] = blenImg[x+(y*width)  ];
                arr[8] = arr[5] = blenImg[x+1 + (y*width)  ];
            }
            else {
                arr[0] = arr[1] = blenImg[x+( (y-1)*width)  ];
                arr[2] = blenImg[x+1+( (y-1)*width)  ];
                arr[3] = arr[4] = blenImg[x+(y*width) ];
                arr[5] = blenImg[x+1+(y*width) ];
                arr[6] = arr[7] = blenImg[x+ ( (y+1)*width)  ];
                arr[8] = blenImg[x+1+( (y+1)*width)  ];
            }

            //padding 경계를 인접 픽셀 값으로 대입
            cnt=0;
            float sum = 0.0;
            for(int i = 0; i < 9; i++) {
                    sum += (1/9.0)*arr[cnt++];
            }

            /* outimg = 블러된 이미지
             * blenImg = 원본 이미지
             * mask = 원본 이미지 - 블러된 이미지 */
            *(outimg + i) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(blenImg + i) - *(outimg + i));
        }

        else if( x==(width*1 -1) ){
            //RightUpVertex
            if(y==0){
                arr[0] = arr[3] = blenImg[x-1 + (y*width)  ];
                arr[1] = arr[2] = arr[5] = arr[4] = blenImg[x + (y*width)  ];
                arr[6] = blenImg[x-1 + ((y-1)*width)  ];
                arr[7] = arr[8] = blenImg[x+((y+1)*width) ];
            }
            //RightDownVertex
            else if(y==height-1){
                arr[0] = blenImg[x-1 + ((y-1)*width)  ];
                arr[1] = arr[2] = blenImg[x-1 +((y-1)*width)  ];
                arr[3] = arr[6] = blenImg[x-1+(y*width) ];
                arr[4] = arr[5] = arr[7] = arr[8] = blenImg[x+(y*width) ];
            }
            else{
                arr[0] = blenImg[x-1 + ((y-1)*width)  ];
                arr[2] = arr[1] = blenImg[x + ((y-1)*width)  ];
                arr[3] = blenImg[x-1 + (y*width)  ];
                arr[5] = arr[4] = blenImg[x+(y*width)  ];
                arr[6] = blenImg[x-1 + ((y+1)*width)  ];
                arr[8] = arr[7] = blenImg[x+((y+1)*width)  ];
            }

            //padding 경계를 인접 픽셀 값으로 대입
            cnt=0;
            float sum = 0.0;
            for(int i = 0; i < 9; i++) {
                    sum += (1/9.0)*arr[cnt++];
            }
            /* outimg = 블러된 이미지
             * blenImg = 원본 이미지
             * mask = 원본 이미지 - 블러된 이미지 */
            *(outimg + i ) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(blenImg + i) - *(outimg + i) );
        }
        else if(y==0){
            if( x!=1 && x!=width-1 ){
                arr[0] = arr[3] = blenImg[x-1+(y*width)  ];
                arr[1] = arr[4] = blenImg[x+(y*width) ];
                arr[2] = arr[5] = blenImg[x+1+(y*width)  ];
                arr[6] = blenImg[x-1+((y+1)*width)  ];
                arr[7] = blenImg[x+((y+1)*width)  ];
                arr[8] = blenImg[x+1 + ((y+1)*width)  ];

                //mask 출력
                cnt=0;
                float sum = 0.0;
                for(int i = 0; i < 9; i++) {
                        sum += (1/9.0)*arr[cnt++];
                }
                *(outimg + i ) = LIMIT_UBYTE(sum);
                *(mask + i) = LIMIT_UBYTE( *(blenImg + i) - *(outimg + i) );
            }
        }
        else if( y ==(height -1) ){
            if( x!=1 && x!=width-1 ){
                arr[0] = blenImg[x-1+((y-1)*width) ];
                arr[1] = blenImg[x+((y-1)*width) ];
                arr[2] = blenImg[x+1+((y-1)*width) ];
                arr[3] = arr[6] = blenImg[x-1+(y*width) ];
                arr[4] = arr[7] = blenImg[x+(y*width) ];
                arr[5] = arr[8] = blenImg[x+1+(y*width) ];

                //mask 출력
                cnt=0;
                float sum = 0.0;
                for(int i = 0; i < 9; i++) {
                        sum += (1/9.0)*arr[cnt++];
                }
                *(outimg + i ) = LIMIT_UBYTE(sum);
                *(mask + i) =LIMIT_UBYTE( *(blenImg + i) - *(outimg + i) );
            }
        }
        else{
            //padding 부분 제외한 안쪽 영상 mask 출력
            float sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += (1/9.0)*blenImg[((x+i*1)+(y+j)*width) ];
                }
            }
            *(outimg + i) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(blenImg + i) - *(outimg + i) );
        }
    }
}

/* 하이부스트 필터 구현 함수*/
void CephValueAdjustment::highBoost(unsigned char* in, int sbValue)
{ //unsharp mask = 원본이미지 + mask 값
    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    int sharpen = sbValue * 2.5;

    for (int i = 0; i < imageSize; i += 1) {
        *(outimg + i) = LIMIT_UBYTE ( *(in + i) + sharpen * *(mask + i) );    //highBoost = 원본이미지 + k * mask 값
    }
    prevImg = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
}

/* 가우시안 블러 필터 구현 함수 */
/* 2차원 가우시안 함수 값을 이용하여 마스크를 생성하고, 입력 영상과 마스크 연산을 수행
 * x 방향과 y 방향으로의 1차원 마스크 연산을 각각 수행함으로써 결과 영상을 생성 */
void CephValueAdjustment::gaussian(unsigned char* in, float sigma)
{
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    //연산을 위한 임시 이미지 배열
    float* tempImg;
    tempImg = (float*)malloc(sizeof(float) * width * height);

    int x;
    int dim = static_cast<int>(2 * 4 * sigma + 1.0);    //마스크 크기

    if (dim < 3) dim = 3;       //최소 마스크 크기 계산(3x3)
    if (dim % 2 == 0) dim++;    //마스크 크기는 홀수만 받게
    int dimHalf = dim / 2;

    //1차원 가우시안 마스크 생성
    float* mask = new float[dim];

    for (int i = 0; i < dim; i++) {
        //x 값의 범위는 -4 * sigma부터 +4 * sigma까지
        x = i - dimHalf;
        //평균이 0이고, 표준 편차가 sigma인 1차원 가우시안 분포의 함수 식 표현
        mask[i] = exp(-(x*x) / (2 * sigma * sigma)) / (sqrt(2 * PI) * sigma);
    }

    float sum1, sum2;

    //세로 방향 마스크 연산
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {

            sum1 = sum2 = 0.f;
            for (int k = 0; k < dim; k++) {

                x = k - dimHalf + j;
                if (x>= 0 && x <height) {
                    sum1 += mask[k];
                    sum2 += (mask[k] * in[x + i*height]);
                }
            }
            tempImg[j+ i*height] = sum2 / sum1;
        }
    }

    //가로 방향 마스크 연산
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            sum1 = sum2 = 0.f;

            for (int k = 0; k < dim; k++) {

                x = k - dimHalf + i;
                if ( x>= 0 && x < width) {
                    sum1 += mask[k];
                    sum2 += (mask[k] * tempImg[j*width + x]);
                }
            }
            outimg[i + j * width] = sum2 / sum1;
        }
    }
    prevImg = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();

    free(tempImg);
    delete[] mask;
}

/* 비등방성 확산 필터 구현 함수 */
/* 상하 좌우 대칭의 형태를 갖는 필터 마스크를 사용하는 필터를 등방성 필터
 * isotropic filter라고 부른다. 앞에서 살펴보았던 평균 값 필터도 등방성의 성질*/
void CephValueAdjustment::ADFilter(unsigned char * inimg, int iter)
{
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    float lambda = 0.25;    //픽셀 값의 변화량을 결정짓는 상수
    float k = 4;            //실험 상수

    auto copy = (inimg);    //연산이 반복된 이미지를 복사해서 사용

    int i;
    float gradn= 0.0, grads= 0.0, grade=0.0, gradw=0.0;     //네 방향 1차 미분 값
    float gcn=0.0, gcs=0.0, gce=0.0, gcw=0.0;               //네 방향 전달 계수
    float pow2k = k * k;

    /* iter 횟수만큼 비등방성 확산 알고리즘 수행 */
    for (i = 0; i < iter; i++)
    {
        int x = 0, y = -1;
        for (int i = 0; i < imageSize; i += 1) {
            x = i % width;
            if(i % width == 0) y++;

            /* 네 방향에 대한 1차 미분 값 */
            gradn = copy[(y - 1) * width + x] - copy[y * width + x];
            grads = copy[(y + 1) * width + x] - copy[y * width + x];
            grade = copy[y * width + (x-1)] - copy[y * width + x];
            gradw = copy[y * width + (x+1)] - copy[y * width + x];

            /* 전달 계수 구하기 */
            gcn = gradn / (1.0f + gradn * gradn / pow2k);
            gcs = grads / (1.0f + grads * grads / pow2k);
            gce = grade / (1.0f + grade * grade / pow2k);
            gcw = gradw / (1.0f + gradw * gradw / pow2k);

            //비등방성 확산 수식 = 현재 위치의 픽셀 값 + lambda * 네 방향 전달 계수
            outimg[y * width + x] = copy[y * width + x] + lambda * (gcn + gcs + gce + gcw);
        }
        if (i < iter - 1)
            std::memcpy((unsigned char*)copy, outimg, sizeof(unsigned char) * width * height);
    }
    //iter만큼 반복 후 이미지 저장
    prevImg = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
}

/* raw 파일 불러오기 함수 */
void CephValueAdjustment::receiveFile(QPixmap& roadPixmap)
{
    pixmap = roadPixmap;

    //defaultImg 저장.
    defaultImg = pixmap.scaled(cephViewWidth, cephViewHeight).toImage();

    //연산을 위한 QImage 저장
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);

    //QImage를 배열로 복사
    inimg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    inimg = image.bits();

    width = image.width();
    height = image.height();
    imageSize = width * height;

    /* 이미지 연산을 중첩으로 하기 위해 후처리별 이미지 생성 */
    outimg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    mask = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);

    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(mask, 0, sizeof(unsigned char) * imageSize);

    sharpenImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    memset(sharpenImg, 0, sizeof(unsigned char) * imageSize);

    copyImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);

    gammaImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    memset(gammaImg, 0, sizeof(unsigned char) * imageSize);

    fftImg = (unsigned char*)malloc(sizeof(unsigned char) * cephViewWidth * cephViewHeight);
    memset(fftImg, 0, sizeof(unsigned char) * cephViewWidth * cephViewHeight);

    medianFilterImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    memset(medianFilterImg, 0, sizeof(unsigned char) * imageSize);

    blenImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    memset(blenImg, 0, sizeof(unsigned char) * imageSize);

    /* 연조직과 골조직의 분리를 위해 감마 조절 후 blending 한 이미지 생성*/
    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / 0.6 )) * 255 + 0.f   );
        *(sharpenImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / 1.4 )) * 255 + 0.f   );
        *(blenImg + i) = *(copyImg + i)*0.5 + *(sharpenImg + i)*0.5;
    }

    set3x3MaskValue();  //영상의 Mask 값은 blending 한 이미지로 구현.

    for(int i = 0; i < imageSize; i ++){ //영상의 평균 value를 저장하기 위함
        avg += inimg[i];
    }

    avg = avg/imageSize;
}

/* 히스토그램 평활화 이미지 전달 함수 */
void CephValueAdjustment::receivePrev(QPixmap& pixmap)
{
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    //현재 뷰어에 보이는 QPixmap -> QImage 변환
    QImage image;
    image = pixmap.scaled(cephViewWidth, cephViewHeight).toImage();

    image = image.convertToFormat(QImage::Format_Grayscale8).copy();

    //QImage -> 배열로 복사
    unsigned char *histoInimg;
    histoInimg = image.bits();

    width = image.width();
    height = image.height();
    imageSize = width * height;

    int histo[256], sum_of_h[256];
    int value,k;
    float constant;

    /* 히스토그램 배열 초기화 */
    for(int i = 0; i < 256; i ++) {
        histo[i] =0;
        sum_of_h[i] = 0;
    }

    /* 히스토그램 분포 저장 */
    for (int i = 0; i < imageSize; i++) {
        value = histoInimg[i];
        histo[value] += 1;
        outimg[i] = value;
    }

    /* 히스토그램 누적 합 계산 */
    for (int i = 0, sum = 0; i < 256; i++){
        sum += histo[i];
        sum_of_h[i] = sum;
    }

    /* 히스토그램 평활화를 위한 정규화 계수
     * constant = ( max gray value x 이미지 사이즈) */
    constant = (float)(256) / (float)(height * width);

    /* 히스토그램 평활화 */
    for (int i = 0; i < imageSize; i++) {
        k = outimg[i];
        outimg[i] = LIMIT_UBYTE( sum_of_h[k] * constant );

        inimg[i] = outimg[i];   //평활화 후 이미지 연산은 평활화 한 이미지로 진행
    }

    //label에 pixmap 띄워주기
    image = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
    pixmap = pixmap.fromImage(image);
    emit cephImgSend(pixmap);
}

/* preset img 받고 전송 */
void CephValueAdjustment::receiveSetPresetImg(QPixmap& prePixmap)
{
    memset(inimg, 0, sizeof(unsigned char) * imageSize);

    QImage presetImg;
    presetImg = prePixmap.scaled(cephViewWidth, cephViewHeight).toImage();
    currentImg = presetImg.convertToFormat(QImage::Format_Grayscale8).copy();

    inimg = currentImg.bits();
}

/* 이미지 처음 상태로 되돌리는 함수 */
void CephValueAdjustment::setResetImg()
{
    memset(inimg, 0, sizeof(unsigned char) * imageSize);

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8).copy();
    inimg = image.bits();

    calcImg = QImage();
}

/* 이미지 밝기, 대조, 선예도, 감마, 노이즈 제거 연산 함수 */
void CephValueAdjustment::changeCephValue(int brightValue, int contrastValue,int sbValue,
                                          int deNoiseValue, int gammaValue )
{
    QImage image;

    float gamma;
    float contrast;
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    /* 밝기값만 조정되는 case */
    if(contrastValue == 0 && sbValue == 0 && deNoiseValue == 0 && gammaValue ==0) {
        int value =  brightValue / 2.5;
        for(int i = 0; i < imageSize; i ++){
            *(outimg + i) = LIMIT_UBYTE( *(inimg + i) + value );
        }
    }
    /* 대비값만 조정되는 case */
    if(brightValue == 0 && sbValue == 0 && deNoiseValue == 0 && gammaValue ==0) {
        if (contrastValue > 0) {
            contrast = (100.0+contrastValue/2)/100.0;
        }
        else if(contrastValue == 0) {
            contrast = 1;
        }
        else {
            contrastValue *= 0.5;
            contrast = (100.0+contrastValue/2)/100.0;
        }
        for(int i = 0; i < imageSize; i ++){
            *(outimg + i) = LIMIT_UBYTE( avg + (*(inimg+i)-avg) *contrast );
        }
    }
    /* 선예도값만 조정되는 case */
    if(brightValue == 0 && contrastValue == 0 && deNoiseValue == 0 && gammaValue ==0) {
        if(sbValue < 0)
            gaussian(inimg, (float)sbValue*(-0.5));
        else if(sbValue > 0)
            highBoost(inimg, sbValue);
        else  prevImg = defaultImg;

        image = prevImg;
    }

    /* 노이즈 제거만 조정되는 case */
    if(brightValue == 0 && contrastValue == 0 && sbValue == 0 && gammaValue ==0) {
        int adfValue = 2 * deNoiseValue;

        ADFilter(inimg, adfValue);

        image = prevImg;
    }
    if(brightValue == 0 && contrastValue == 0 && sbValue == 0 && deNoiseValue ==0) {
        if(gammaValue ==0){
            for(int i = 0; i < imageSize; i ++){
                *(outimg + i) = *(inimg + i);
            }
        }
        else {   //gammaValue가 0이 아닌 경우
            gamma = 1.0 + gammaValue*0.02;

            for(int i = 0; i < imageSize; i ++){
                *(outimg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / gamma )) * 255 + 0.f );
            }
        }

    }
    /* 두 개 이상의 값이 조정되는 case */
    else{
        int value =  brightValue / 2.5;
        if(gammaValue ==0){
            for(int i = 0; i < imageSize; i ++) {
                *(gammaImg + i) = *(inimg + i);
            }
        }
        else{   //gammaValue가 0이 아닌 경우
            gamma = 1.0 + gammaValue*0.02;

            for(int i = 0; i < imageSize; i ++){
                *(gammaImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / gamma )) * 255 + 0.f   );
            }
        }
        if(deNoiseValue == 0){  //노이즈 제거가 되지 않을 경우
            if(sbValue != 0){   //선예도가 조정된 경우
                switch(sbValue) {
                case -6:
                    gaussian(gammaImg, 3.0);
                    break;
                case -5:
                    gaussian(gammaImg, 2.5);
                    break;
                case -4:
                    gaussian(gammaImg, 2.0);
                    break;
                case -3:
                    gaussian(gammaImg, 1.5);
                    break;
                case -2:
                    gaussian(gammaImg, 1.0);
                    break;
                case -1:
                    gaussian(gammaImg, 0.5);
                    break;
                default:
                    highBoost(gammaImg, sbValue);
                    break;
                }

                if(sbValue < 0){
                    gaussian(gammaImg, (float)sbValue*(-0.5));
                }
                else if(sbValue > 0){
                    highBoost(gammaImg, sbValue);
                }

                image = prevImg;
                sharpenImg = image.bits();  //선예도 연산 후 밝기, 대조 연산.
                if (contrastValue > 0) {
                    contrast = (100.0+contrastValue/2)/100.0;
                }
                else if(contrastValue == 0) {
                    contrast = 1;
                }
                else {
                    contrastValue *= 0.5;
                    contrast = (100.0+contrastValue/2)/100.0;
                }
                for(int i = 0; i < imageSize; i ++){
                    *(outimg + i) = LIMIT_UBYTE( (avg + (*(sharpenImg+i)-avg) * contrast)  + value );
                }
            }
            else if(sbValue == 0){ //선예도가 조정되지 않은 경우
                if (contrastValue > 0) {
                    contrast = (100.0+contrastValue/2)/100.0;
                }
                else if(contrastValue == 0) {
                    contrast = 1;
                }
                else {
                    contrastValue *= 0.5;
                    contrast = (100.0+contrastValue/2)/100.0;
                }
                for(int i = 0; i < imageSize; i ++){
                    *(outimg + i) = LIMIT_UBYTE( avg + (*(gammaImg+i)- avg) *contrast  + value );
                }
            }
        }
        else { //노이즈 정도가 조정 된 경우

            int adfValue = 2 * deNoiseValue;

            if(sbValue != 0){   //선예도가 조정된 경우
                switch(sbValue) {
                case -6:
                    gaussian(gammaImg, 3.0);
                    break;
                case -5:
                    gaussian(gammaImg, 2.5);
                    break;
                case -4:
                    gaussian(gammaImg, 2.0);
                    break;
                case -3:
                    gaussian(gammaImg, 1.5);
                    break;
                case -2:
                    gaussian(gammaImg, 1.0);
                    break;
                case -1:
                    gaussian(gammaImg, 0.5);
                    break;
                default:
                    highBoost(gammaImg, sbValue);
                    break;
                }
                if(sbValue <0){
                    gaussian(gammaImg, (float)sbValue*(-0.5));
                }
                else if(sbValue >0){
                    highBoost(gammaImg, sbValue);
                }
                image = prevImg;
                sharpenImg = image.bits();  //선예도 연산 후 밝기, 대조 연산.
                if (contrastValue > 0) {
                    contrast = (100.0+contrastValue/2)/100.0;
                }
                else if(contrastValue == 0) {
                    contrast = 1;
                }
                else {
                    contrastValue *= 0.5;
                    contrast = (100.0+contrastValue/2)/100.0;
                }
                for(int i = 0; i < imageSize; i ++){
                    *(copyImg + i) = LIMIT_UBYTE( (avg + (*(sharpenImg+i)-avg) * contrast)  + value );
                }

                ADFilter(copyImg, adfValue);
                image = prevImg;
            }
            else if(sbValue == 0){ //선예도가 조정되지 않은 경우
                if (contrastValue > 0) {
                    contrast = (100.0+contrastValue/2)/100.0;
                }
                else if(contrastValue == 0) {
                    contrast = 1;
                }
                else {
                    contrastValue *= 0.5;
                    contrast = (100.0+contrastValue/2)/100.0;
                }
                for(int i = 0; i < imageSize; i ++){
                    *(copyImg + i) = LIMIT_UBYTE( avg + (*(gammaImg+i)-avg) *contrast  + value );
                }
                ADFilter(copyImg, adfValue);
                image = prevImg;
            }

        }

    }

    image = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
    pixmap = pixmap.fromImage(image);
    emit cephImgSend(pixmap);

    calcImg = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();      //연산 결과 이미지 저장
}

/* 중간값 필터 구현 함수 */
void CephValueAdjustment::median(int value)
{
    Q_UNUSED(value);

    memset(medianFilterImg, 0, sizeof(unsigned char) * imageSize);
    if(calcImg.isNull() != 1) inimg = calcImg.bits();

    int imageSize = width * height;
    int rowSize = width;
    int x = 0, y = -1;
    int cnt = 0;

    ushort arr[9] = { 0, };

    for (int i = 0; i < imageSize; i++) {
        x = i % width;
        if (i % width == 0) y++;

        if (x == 0) {
            //좌측 상단 Vertex
            if (y == 0) {
                arr[0] = arr[1] = arr[3] = arr[4] = inimg[x + (y * rowSize)];
                arr[2] = arr[5] = inimg[x + 1 + (y * rowSize)];
                arr[6] = arr[7] = inimg[x + ((y + 1) * rowSize)];
                arr[8] = inimg[x + 1 + ((y + 1) * rowSize)];
            }
            //좌측 하단 Vertex
            else if (y == height - 1) {
                arr[0] = arr[1] = inimg[x + ((y - 1) * rowSize)];
                arr[2] = inimg[x + 1 + ((y - 1) * rowSize)];
                arr[3] = arr[6] = arr[7] = arr[4] = inimg[x + ((y * rowSize))];
                arr[8] = arr[5] = inimg[x + 1 + (y * rowSize)];
            }
            else {
                arr[0] = arr[1] = inimg[x + ((y - 1) * rowSize)];
                arr[2] = inimg[x + 1 + ((y - 1) * rowSize)];
                arr[3] = arr[4] = inimg[x + (y * rowSize)];
                arr[5] = inimg[x + 1 + (y * rowSize)];
                arr[6] = arr[7] = inimg[x + ((y + 1) * rowSize)];
                arr[8] = inimg[x + 1 + ((y + 1) * rowSize)];
            }

            insertion(arr, 9);
            medianFilterImg[(x + y * rowSize)] = arr[4];
        }
        else if (x == (rowSize - 1)) {
            //우측 상단 Vertex
            if (y == 0) {
                arr[0] = arr[3] = inimg[x - 1 + (y * rowSize)];
                arr[1] = arr[2] = arr[5] = arr[4] = inimg[x + (y * rowSize)];
                arr[6] = inimg[x - 1 + ((y - 1) * rowSize)];
                arr[7] = arr[8] = inimg[x + ((y + 1) * rowSize)];
            }
            //우측 하단 Vertex
            else if (y == height - 1) {
                arr[0] = inimg[x - 1 + ((y - 1) * rowSize)];
                arr[1] = arr[2] = inimg[x - 1 + ((y - 1) * rowSize)];
                arr[3] = arr[6] = inimg[x - 1 + (y * rowSize)];
                arr[4] = arr[5] = arr[7] = arr[8] = inimg[x + (y * rowSize)];
            }
            else {
                arr[0] = inimg[x - 1 + ((y - 1) * rowSize)];
                arr[2] = arr[1] = inimg[x + ((y - 1) * rowSize)];
                arr[3] = inimg[x - 1 + (y * rowSize)];
                arr[5] = arr[4] = inimg[x + (y * rowSize)];
                arr[6] = inimg[x - 1 + ((y + 1) * rowSize)];
                arr[8] = arr[7] = inimg[x + ((y + 1) * rowSize)];
            }

            insertion(arr, 9);
            medianFilterImg[(x + y * rowSize)] = arr[4];
        }
        else if (y == 0) {
            if (x != 1 && x != rowSize - 1) {
                arr[0] = arr[3] = inimg[x - 1 + (y * rowSize)];
                arr[1] = arr[4] = inimg[x + (y * rowSize)];
                arr[2] = arr[5] = inimg[x + 1 + (y * rowSize)];
                arr[6] = inimg[x - 1 + ((y + 1) * rowSize)];
                arr[7] = inimg[x + ((y + 1) * rowSize)];
                arr[8] = inimg[x + 1 + ((y + 1) * rowSize)];
            }

            insertion(arr, 9);
            medianFilterImg[(x + y * rowSize)] = arr[4];
        }
        else if (y == (height - 1)) {
            if (x != 1 && x != rowSize - 1) {
                arr[0] = inimg[x - 1 + ((y - 1) * rowSize)];
                arr[1] = inimg[x + ((y - 1) * rowSize)];
                arr[2] = inimg[x + 1 + ((y - 1) * rowSize)];
                arr[3] = arr[6] = inimg[x - 1 + (y * rowSize)];
                arr[4] = arr[7] = inimg[x + (y * rowSize)];
                arr[5] = arr[8] = inimg[x + 1 + (y * rowSize)];
            }

            insertion(arr, 9);
            medianFilterImg[(x + y * rowSize)] = arr[4];
        }
        else {
            cnt = 0;
            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    arr[cnt++] = inimg[((x + i) + (y + j) * width)];
                }
            }
            insertion(arr, 9);
            medianFilterImg[(x + y * rowSize)] = arr[4];
        }
    }
    QPixmap medianPixmap;

    currentImg = QImage(medianFilterImg, width, height, QImage::Format_Grayscale8).copy();
    medianPixmap = pixmap.fromImage(currentImg);

    inimg = currentImg.bits();
    emit cephImgSend(medianPixmap);
    emit exitFilterSignal();
}

/* 주파수 내 저역 가우시안 필터링 적용 함수 */
void CephValueAdjustment::lowPassFFT(int cutoff)
{
    memset(fftImg, 0, sizeof(unsigned char) * cephViewWidth*cephViewHeight);

    /* 이미 밝기, 대조, 선예도, 감마, 노이즈 제거 연산이 되어있을 경우
     * 연산된 이미지 사용 */
    if(calcImg.isNull() != 1) inimg = calcImg.bits();

    /* 푸리에 변환을 위한 객체 생성 */
    FourierProcessing fourier(cephViewWidth, cephViewHeight, inimg);

    //가우시안 필터링 적용
    fourier.lowPassGaussian(fftImg, cutoff);
    currentImg = QImage(fftImg, cephViewWidth, cephViewHeight, QImage::Format_Grayscale8).copy();

    //필터링 적용한 이미지 전달
    QPixmap fourierPixmap;
    fourierPixmap = pixmap.fromImage(currentImg);
    emit cephImgSend(fourierPixmap);

    inimg = currentImg.bits();

    //푸리 변환을 위한 생성했던 객체 내 메모리 제거
    fourier.deleteMemory();
    emit exitFilterSignal();
}

/* 주파수 내 고역 가우시안 필터링 적용 함수 */
void CephValueAdjustment::highPassFFT(int cutoff)
{
    memset(fftImg, 0, sizeof(unsigned char) * cephViewWidth*cephViewHeight);

    /* 이미 밝기, 대조, 선예도, 감마, 노이즈 제거 연산이 되어있을 경우
     * 연산된 이미지 사용 */
    if(calcImg.isNull() != 1) inimg = calcImg.bits();

    /* 푸리에 변환을 위한 객체 생성 */
    FourierProcessing fourier(cephViewWidth, cephViewHeight, inimg);

    //가우시안 필터링 적용
    fourier.highFrequencyPass(fftImg, cutoff);
    currentImg = QImage(fftImg, cephViewWidth, cephViewHeight, QImage::Format_Grayscale8).copy();

    //필터링 적용한 이미지 전달
    QPixmap fourierPixmap;
    fourierPixmap = pixmap.fromImage(currentImg);
    emit cephImgSend(fourierPixmap);

    inimg = currentImg.bits();

    //푸리 변환을 위한 생성했던 객체 내 메모리 제거
    fourier.deleteMemory();
    emit exitFilterSignal();
}


