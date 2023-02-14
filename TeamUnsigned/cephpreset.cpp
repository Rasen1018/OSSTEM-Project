#include "cephpreset.h"

#include <QImage>
#include <QPixmap>
#include "fourierprocessing.h"

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n
#define PI 3.1416926535f

CephPreset::CephPreset(QObject *parent)
    : QObject{parent}
{
}
void CephPreset::receiveFile(QPixmap& roadPixmap){
    pixmap = roadPixmap;
    defaultImg = pixmap.scaled(cephViewWidth, cephViewHeight).toImage();

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);

    inimg = image.bits();

    width = image.width();
    height = image.height();
    imageSize = width * height;

    outimg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    mask = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    copyImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    copyImg2 = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    fftImg = (unsigned char*)malloc(sizeof(unsigned char) * cephViewWidth * cephViewHeight);

    memset(fftImg, 0, sizeof(unsigned char) * cephViewWidth * cephViewHeight);
    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(mask, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg2, 0, sizeof(unsigned char) * imageSize);

    blendImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    memset(blendImg, 0, sizeof(unsigned char) * imageSize);

    for(int i = 0; i < imageSize; i ++){ //영상의 평균 value를 저장하기 위함
        avg += inimg[i];
    }

    avg = avg/imageSize;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / 0.5 )) * 255 + 0.f   );
        *(copyImg2 + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / 1.6 )) * 255 + 0.f   );
        *(blendImg + i) = LIMIT_UBYTE ( *(copyImg + i)*0.5 + *(copyImg2 + i)*0.5 );
    }

    set3x3MaskValue();

    setPreset_1();
    setPreset_2();
    setPreset_3();
    setPreset_4();
    setPreset_5();
    setPreset_6();
}

void CephPreset::receievePreset(int preset){
    /* Preset Num에 따라 반환 */

    switch(preset) {
    case 1:
        pixmap = pixmap.fromImage(presetImg1);
        break;
    case 2:
        pixmap = pixmap.fromImage(presetImg2);
        break;
    case 3:
        pixmap = pixmap.fromImage(presetImg3);
        break;
    case 4:
        pixmap = pixmap.fromImage(presetImg4);
        break;
    case 5:
        pixmap = pixmap.fromImage(presetImg5);
        break;
    case 6:
        pixmap = pixmap.fromImage(presetImg6);
        break;
    }

    emit cephPresetSend(pixmap);
    emit cephPresetAdj(pixmap);

    pixmap = QPixmap();
}

void CephPreset::setPreset_1(){

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg2, 0, sizeof(unsigned char) * imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / 0.5 )) * 255 + 0.f   );
        *(copyImg2 + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / 2.0 )) * 255 + 0.f   );
        *(blendImg + i) = LIMIT_UBYTE ( *(copyImg + i)*0.5 + *(copyImg2 + i)*0.5 );
        *(copyImg + i) = LIMIT_UBYTE( *(blendImg + i) - 12);
    }
    memcpy(copyImg2, highBoost(copyImg, 2), sizeof(unsigned char) * imageSize);

    presetImg1 = QImage(copyImg2, width, height, QImage::Format_Grayscale8).copy();
}

void CephPreset::setPreset_2(){

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg2, 0, sizeof(unsigned char) * imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / 0.7 )) * 255 + 0.f   );
        *(copyImg2 + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / 2.5 )) * 255 + 0.f   );
        *(blendImg + i) = LIMIT_UBYTE ( *(copyImg + i)*0.5 + *(copyImg2 + i)*0.5 );
        *(copyImg + i) = LIMIT_UBYTE( *(blendImg + i) - 12);
    }
    memcpy(copyImg2, highBoost(copyImg, 2), sizeof(unsigned char) * imageSize);

    presetImg2 = QImage(copyImg2, width, height, QImage::Format_Grayscale8).copy();

}
void CephPreset::setPreset_3(){
    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg2, 0, sizeof(unsigned char) * imageSize);

    int contrastValue = -10;
    float contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(blendImg+i)-avg) * contrast) - 12 );
        *(copyImg2 + i) =  LIMIT_UBYTE( qPow(*(copyImg + i) / 255.f , abs(1.f / 1.1 )) * 255 + 0.f   );
    }

    memcpy(copyImg, lowPassFFT(copyImg2, 200), sizeof(unsigned char) * imageSize);
    memcpy(copyImg2, highBoost(copyImg,4),sizeof(unsigned char) * imageSize);
    memcpy(copyImg, ADFilter(copyImg2, 2), sizeof(unsigned char) * imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(copyImg2 + i) = LIMIT_UBYTE( (avg + (*(copyImg+i)-avg) * 0.88));
        *(copyImg + i) =  LIMIT_UBYTE( qPow(*(copyImg2 + i) / 255.f , abs(1.f / 0.8 )) * 255 + 0.f   );
    }
    memcpy(outimg, ADFilter(copyImg, 2), sizeof(unsigned char) * imageSize);


    presetImg3 = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
}
void CephPreset::setPreset_4(){
    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg2, 0, sizeof(unsigned char) * imageSize);

    int contrastValue = -10;
    float contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(blendImg+i)-avg) * contrast) - 12 );
        *(copyImg2 + i) =  LIMIT_UBYTE( qPow(*(copyImg + i) / 255.f , abs(1.f / 1.1 )) * 255 + 0.f   );
    }

    memcpy(copyImg, lowPassFFT(copyImg2, 200), sizeof(unsigned char) * imageSize);
    memcpy(copyImg2, highBoost(copyImg,4),sizeof(unsigned char) * imageSize);
    memcpy(outimg, ADFilter(copyImg2, 4), sizeof(unsigned char) * imageSize);

    presetImg4 = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();

}
void CephPreset::setPreset_5(){
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg2, 0, sizeof(unsigned char) * imageSize);


    int histo[256], sum_of_h[256];
    int value,k;
    float constant;

    for(int i = 0; i < 256; i ++) {
        histo[i] =0;
        sum_of_h[i] = 0;
    }

    for (int i = 0; i < imageSize; i++) {   //histogram 분포
        value = inimg[i];
        histo[value] += 1;
        copyImg[i] = value;
    }

    //histogram
    for (int i = 0, sum = 0; i < 256; i++){
        sum += histo[i];
        sum_of_h[i] = sum;
    }

    /* constant = new # of gray levels div by area */
    constant = (float)(256) / (float)(height * width);
    for (int i = 0; i < imageSize; i++) {
        k = copyImg[i];
        copyImg[i] = LIMIT_UBYTE( sum_of_h[k] * constant ); //copyImg에 평탄화 한 value 저장.
        *(copyImg2 + i) = LIMIT_UBYTE( (avg + (*(copyImg+i)-avg) * 0.9) - 40 ); //밝기 및 대조 연산
        //*(copyImg + i) =  LIMIT_UBYTE( qPow(*(copyImg2 + i) / 255.f , abs(1.f / 1.1 )) * 255 + 0.f   );
    }
    memcpy(copyImg, highBoost(copyImg2,4), sizeof(unsigned char)* imageSize);
    memcpy(outimg, ADFilter(copyImg, 6), sizeof(unsigned char)*imageSize);

    presetImg5 = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();

}
void CephPreset::setPreset_6(){
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg2, 0, sizeof(unsigned char) * imageSize);


    int histo[256], sum_of_h[256];
    int value,k;
    float constant;

    for(int i = 0; i < 256; i ++) {
        histo[i] =0;
        sum_of_h[i] = 0;
    }

    for (int i = 0; i < imageSize; i++) {   //histogram 분포
        value = inimg[i];
        histo[value] += 1;
        copyImg[i] = value;
    }

    //histogram
    for (int i = 0, sum = 0; i < 256; i++){
        sum += histo[i];
        sum_of_h[i] = sum;
    }

    /* constant = new # of gray levels div by area */
    constant = (float)(256) / (float)(height * width);
    for (int i = 0; i < imageSize; i++) {
        k = copyImg[i];
        copyImg[i] = LIMIT_UBYTE( sum_of_h[k] * constant ); //copyImg에 평탄화 한 value 저장.
        *(copyImg2 + i) = LIMIT_UBYTE( (avg + (*(copyImg+i)-avg) * 0.9) - 40 ); //밝기 및 대조 연산
    }
    memcpy(copyImg, highBoost(copyImg2,4), sizeof(unsigned char)* imageSize);
    memcpy(copyImg2, ADFilter(copyImg, 6), sizeof(unsigned char)*imageSize);

    for(int i =0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * 1.2) + 12 ); //밝기 및 대조 연산
        *(copyImg2 + i) =  LIMIT_UBYTE( qPow(*(copyImg + i) / 255.f , abs(1.f / 1.1 )) * 255 + 0.f   );
    }

    presetImg6 = QImage(copyImg2, width, height, QImage::Format_Grayscale8).copy();
    }

unsigned char* CephPreset::highBoost(unsigned char* in, int sbValue){
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    int sharpen = sbValue * 2.5;

    for (int i = 0; i < imageSize; i += 1) {
        *(outimg + i) = LIMIT_UBYTE ( *(in + i) + sharpen * *(mask + i) );    //highBoost = 원본이미지 + k * mask 값
    }

    return outimg;
}
unsigned char* CephPreset::ADFilter(unsigned char* inimg ,int iter){
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    float lambda = 0.25;
    float k = 4;

    auto copy(inimg);

    /* iter 횟수만큼 비등방성 확산 알고리즘 수행 */
    int i;
    float gradn = 0.0, grads = 0.0, grade = 0.0, gradw = 0.0;
    float gcn = 0.0, gcs = 0.0, gce = 0.0, gcw = 0.0;
    float k2 = k * k;

    for (i = 0; i < iter; i++)
    {
        int widthCnt = 0, heightCnt = -1;
        for (int i = 0; i < imageSize; i += 1) {
            widthCnt = i % width;
            if(i % width == 0) heightCnt++;

            gradn = copy[(heightCnt - 1) * width + widthCnt] - copy[heightCnt * width + widthCnt];
            grads = copy[(heightCnt + 1) * width + widthCnt] - copy[heightCnt * width + widthCnt];
            grade = copy[heightCnt * width + (widthCnt-1)] - copy[heightCnt * width + widthCnt];
            gradw = copy[heightCnt * width + (widthCnt+1)] - copy[heightCnt * width + widthCnt];

            gcn = gradn / (1.0f + gradn * gradn / k2);
            gcs = grads / (1.0f + grads * grads / k2);
            gce = grade / (1.0f + grade * grade / k2);
            gcw = gradw / (1.0f + gradw * gradw / k2);

            outimg[heightCnt * width + widthCnt] = copy[heightCnt * width + widthCnt] + lambda * (gcn + gcs + gce + gcw);
        }
        if (i < iter - 1)
            std::memcpy((unsigned char*)copy, outimg, sizeof(unsigned char) * width * height);
    }

    return outimg;
}
void CephPreset::set3x3MaskValue(){
    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(mask, 0, sizeof(unsigned char) * imageSize);

    double kernel[3][3] = { {1/9.0, 1/9.0, 1/9.0},  //평균값 필터를 이용한 mask 값
                            {1/9.0, 1/9.0, 1/9.0},
                            {1/9.0, 1/9.0, 1/9.0}};

    int arr[9] = {0};

    int widthCnt = 0, heightCnt = -1, cnt = 0;
    for(int i = 0; i < imageSize; i ++){
        widthCnt = i % width;
        if(i % width == 0) heightCnt++;

        if(widthCnt==0){
            //LeftUpVertex
            if(heightCnt==0){
                arr[0] = arr[1] = arr[3] = arr[4] = blendImg[widthCnt+(heightCnt*width) ];
                arr[2] = arr[5] = blendImg[widthCnt+1 + (heightCnt*width) ];
                arr[6] = arr[7] = blendImg[widthCnt+ ((heightCnt+1)*width)  ];
                arr[8] = blendImg[widthCnt+1+((heightCnt+1)*width) ];
            }
            //LeftDownVertex
            else if(heightCnt==height-1){
                arr[0] = arr[1] =blendImg[widthCnt+((heightCnt-1)*width) ];
                arr[2] = blendImg[widthCnt+1 + ((heightCnt-1)*width) ];
                arr[3] = arr[6] = arr[7] = arr[4] = blendImg[widthCnt+(heightCnt*width)  ];
                arr[8] = arr[5] = blendImg[widthCnt+1 + (heightCnt*width)  ];
            }
            else{
                arr[0] = arr[1] = blendImg[widthCnt+( (heightCnt-1)*width)  ];
                arr[2] = blendImg[widthCnt+1+( (heightCnt-1)*width)  ];
                arr[3] = arr[4] = blendImg[widthCnt+(heightCnt*width) ];
                arr[5] = blendImg[widthCnt+1+(heightCnt*width) ];
                arr[6] = arr[7] = blendImg[widthCnt+ ( (heightCnt+1)*width)  ];
                arr[8] = blendImg[widthCnt+1+( (heightCnt+1)*width)  ];
            }

            cnt=0;
            float sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*arr[cnt++];
                }
            }
            *(outimg + i) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(blendImg + i) - *(outimg + i));
        }

        else if( widthCnt==(width*1 -1) ){
            //RightUpVertex
            if(heightCnt==0){
                arr[0] = arr[3] = blendImg[widthCnt-1 + (heightCnt*width)  ];
                arr[1] = arr[2] = arr[5] = arr[4] = blendImg[widthCnt + (heightCnt*width)  ];
                arr[6] = blendImg[widthCnt-1 + ((heightCnt-1)*width)  ];
                arr[7] = arr[8] = blendImg[widthCnt+((heightCnt+1)*width) ];
            }
            //RightDownVertex
            else if(heightCnt==height-1){
                arr[0] = blendImg[widthCnt-1 + ((heightCnt-1)*width)  ];
                arr[1] = arr[2] = blendImg[widthCnt-1 +((heightCnt-1)*width)  ];
                arr[3] = arr[6] = blendImg[widthCnt-1+(heightCnt*width) ];
                arr[4] = arr[5] = arr[7] = arr[8] = blendImg[widthCnt+(heightCnt*width) ];
            }
            else{
                arr[0] = blendImg[widthCnt-1 + ((heightCnt-1)*width)  ];
                arr[2] = arr[1] = blendImg[widthCnt + ((heightCnt-1)*width)  ];
                arr[3] = blendImg[widthCnt-1 + (heightCnt*width)  ];
                arr[5] = arr[4] = blendImg[widthCnt+(heightCnt*width)  ];
                arr[6] = blendImg[widthCnt-1 + ((heightCnt+1)*width)  ];
                arr[8] = arr[7] = blendImg[widthCnt+((heightCnt+1)*width)  ];
            }
            cnt=0;
            float sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*arr[cnt++];
                }
            }
            *(outimg + i ) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(blendImg + i) - *(outimg + i) );
        }
        else if(heightCnt==0){
            if( widthCnt!=1 && widthCnt!=width-1 ){
                arr[0] = arr[3] = blendImg[widthCnt-1+(heightCnt*width)  ];
                arr[1] = arr[4] = blendImg[widthCnt+(heightCnt*width) ];
                arr[2] = arr[5] = blendImg[widthCnt+1+(heightCnt*width)  ];
                arr[6] = blendImg[widthCnt-1+((heightCnt+1)*width)  ];
                arr[7] = blendImg[widthCnt+((heightCnt+1)*width)  ];
                arr[8] = blendImg[widthCnt+1 + ((heightCnt+1)*width)  ];

                cnt=0;
                float sum = 0.0;
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        sum += kernel[i+1][j+1]*arr[cnt++];
                    }
                }
                *(outimg + i ) = LIMIT_UBYTE(sum);
                *(mask + i) = LIMIT_UBYTE( *(blendImg + i) - *(outimg + i) );
            }
        }
        else if( heightCnt ==(height -1) ){
            if( widthCnt!=1 && widthCnt!=width-1 ){
                arr[0] = blendImg[widthCnt-1+((heightCnt-1)*width) ];
                arr[1] = blendImg[widthCnt+((heightCnt-1)*width) ];
                arr[2] = blendImg[widthCnt+1+((heightCnt-1)*width) ];
                arr[3] = arr[6] = blendImg[widthCnt-1+(heightCnt*width) ];
                arr[4] = arr[7] = blendImg[widthCnt+(heightCnt*width) ];
                arr[5] = arr[8] = blendImg[widthCnt+1+(heightCnt*width) ];
                cnt=0;
                float sum = 0.0;
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        sum += kernel[i+1][j+1]*arr[cnt++];
                    }
                }
                *(outimg + i ) = LIMIT_UBYTE(sum);
                *(mask + i) =LIMIT_UBYTE( *(blendImg + i) - *(outimg + i) );
            }
        }
        else{
            float sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*blendImg[((widthCnt+i*1)+(heightCnt+j)*width) ];
                }
            }
            *(outimg + i) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(blendImg + i) - *(outimg + i) );
        }
    }

}
unsigned char* CephPreset::lowPassFFT(unsigned char* in, int cutoff){
    memset(fftImg, 0, sizeof(unsigned char) * cephViewWidth*cephViewHeight);

    QImage currentImg;

    FourierProcessing fourier(cephViewWidth, cephViewHeight, in);

    fourier.lowPassGaussian(fftImg, cutoff);

    currentImg = QImage(fftImg, cephViewWidth, cephViewHeight, QImage::Format_Grayscale8);

    QPixmap fourierPixmap;
    fourierPixmap = pixmap.fromImage(currentImg);

    fourier.deleteMemory();

    return currentImg.bits();
}
