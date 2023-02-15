#include "panopreset.h"
#include "fourierprocessing.h"

#include <QImage>
#include <QPixmap>

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n
#define PI 3.1416926535f

PanoPreset::PanoPreset(QObject *parent)
    : QObject{parent}
{
}
/* 1번 프리셋 연산
 * 프리셋 연산 결과를 PresetImg1에 저장
 */
void PanoPreset::setPreset_1()
{
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg2, 0, sizeof(unsigned char) * imageSize);

    int sbValue = 3;
    int contrastValue = 20;

    double gammaValue = 0.6;

    float contrast;
    contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / gammaValue )) * 255 + 0.f   );
    }
    memcpy(copyImg2, highBoost(copyImg, sbValue), sizeof(unsigned char)*imageSize);

    int brightValue = 20;
    int bright = brightValue / 2.5;
    contrastValue = -10;
    contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(outimg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * contrast) + bright );
    }


    presetImg1 = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
}
/* 2번 프리셋 연산
 * 프리셋 연산 결과를 PresetImg2에 저장
 */
void PanoPreset::setPreset_2()
{
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg2, 0, sizeof(unsigned char) * imageSize);

    int brightValue = -20;
    int bright = brightValue / 2.5;
    double gammaValue = 0.6;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / gammaValue )) * 255 + 0.f   );
    }
    memcpy(copyImg2, highBoost(copyImg, 3), sizeof(unsigned char) * imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(outimg + i) = LIMIT_UBYTE( *(copyImg2+i) + bright );
    }

    presetImg2 = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
}
/* 3번 프리셋 연산
 * 프리셋 연산 결과를 PresetImg3에 저장
 */
void PanoPreset::setPreset_3()
{
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg2, 0, sizeof(unsigned char) * imageSize);

    int brightValue = 20;
    int sbValue = 6;
    int contrastValue = 50;
    int deNoiseValue = 5;
    double gammaValue = 0.8;

    int bright = brightValue / 2.5;
    int adfValue = 2 * deNoiseValue;
    float contrast;
    contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs((50/21.0) - gammaValue )) * 255 + 0.f   );
    }

    memcpy(copyImg2, highBoost(copyImg, sbValue), sizeof(unsigned char)* imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * contrast)  + bright );
    }

    memcpy(copyImg2, ADFilter(copyImg, adfValue), sizeof(unsigned char)* imageSize);

    gammaValue = 1.2;
    for(int i = 0; i < imageSize; i ++){
        *(outimg + i) = LIMIT_UBYTE( qPow(*(copyImg2 + i) / 255.f , abs(1.f/ gammaValue )) * 255 + 0.f   );
    }

    presetImg3 = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
}
/* 4번 프리셋 연산
 * 프리셋 연산 결과를 PresetImg4에 저장
 */
void PanoPreset::setPreset_4()
{
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg2, 0, sizeof(unsigned char) * imageSize);

    int brightValue = 20;
    int sbValue = 6;
    int contrastValue = 50;
    int deNoiseValue = 5;
    double gammaValue = 0.6;

    int bright = brightValue / 2.5;
    int adfValue = 2 * deNoiseValue;
    float contrast;
    contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / gammaValue )) * 255 + 0.f   );
    }
    memcpy(copyImg2, highBoost(copyImg, sbValue), sizeof(unsigned char)*imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * contrast)  + bright );
    }

    memcpy(outimg, ADFilter(copyImg, adfValue), sizeof(unsigned char)*imageSize);

    presetImg4 = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
}
/* 5번 프리셋 연산
 * 프리셋 연산 결과를 PresetImg5에 저장
 */
void PanoPreset::setPreset_5()
{
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg2, 0, sizeof(unsigned char) * imageSize);

    int brightValue = 40;
    int sbValue = 6;
    int contrastValue = 70;
    int deNoiseValue = 5;
    double gammaValue = 1.0;

    int bright = brightValue / 2.5;
    int adfValue = 2 * deNoiseValue;
    float contrast;
    contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs((50/21.0) - gammaValue )) * 255 + 0.f   );
    }

    memcpy(copyImg2, highBoost(copyImg, sbValue), sizeof(unsigned char)*imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * contrast)  + bright );
    }

    memcpy(copyImg2, ADFilter(copyImg, adfValue), sizeof(unsigned char)*imageSize);

    gammaValue = 0.8;
    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( qPow(*(copyImg2 + i) / 255.f , abs(1.f / gammaValue )) * 255 + 0.f   );
    }
    memcpy(copyImg2, lowPassFFT(copyImg, 150), sizeof(unsigned char)*imageSize);
    memcpy(copyImg, highBoost(copyImg2, 6), sizeof(unsigned char)*imageSize);
    memcpy(outimg, ADFilter(copyImg, 18), sizeof(unsigned char)*imageSize);

    presetImg5 = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
}
/* 6번 프리셋 연산
 * 프리셋 연산 결과를 PresetImg6에 저장
 */
void PanoPreset::setPreset_6()
{
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg2, 0, sizeof(unsigned char) * imageSize);

    int brightValue = 40;
    int sbValue = 6;
    int contrastValue = 70;
    int deNoiseValue = 5;
    double gammaValue = 0.6;

    int bright = brightValue / 2.5;
    int adfValue = 2 * deNoiseValue;
    float contrast;
    contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs((50/21.0) - gammaValue )) * 255 + 0.f   );
    }

    memcpy(copyImg2, highBoost(copyImg, sbValue), sizeof(unsigned char)*imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * contrast)  + bright );
    }

    memcpy(copyImg2, ADFilter(copyImg, adfValue), sizeof(unsigned char)*imageSize);

    gammaValue = 0.8;
    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( qPow(*(copyImg2 + i) / 255.f , abs(1.f / gammaValue )) * 255 + 0.f   );
    }

    memcpy(copyImg2, lowPassFFT(copyImg, 150), sizeof(unsigned char)*imageSize);
    memcpy(copyImg, highBoost(copyImg2, 6), sizeof(unsigned char)*imageSize);
    memcpy(outimg, ADFilter(copyImg, 18), sizeof(unsigned char)*imageSize);

    presetImg6 = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
}
/* 평균값 필터를 이용한 영상의 mask 값을
 * *mask에 저장
 */
void PanoPreset::set3x3MaskValue()
{
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
                arr[0] = arr[1] = arr[3] = arr[4] = inimg[widthCnt+(heightCnt*width) ];
                arr[2] = arr[5] = inimg[widthCnt+1 + (heightCnt*width) ];
                arr[6] = arr[7] = inimg[widthCnt+ ((heightCnt+1)*width)  ];
                arr[8] = inimg[widthCnt+1+((heightCnt+1)*width) ];
            }
            //LeftDownVertex
            else if(heightCnt==height-1){
                arr[0] = arr[1] =inimg[widthCnt+((heightCnt-1)*width) ];
                arr[2] = inimg[widthCnt+1 + ((heightCnt-1)*width) ];
                arr[3] = arr[6] = arr[7] = arr[4] = inimg[widthCnt+(heightCnt*width)  ];
                arr[8] = arr[5] = inimg[widthCnt+1 + (heightCnt*width)  ];
            }
            else{
                arr[0] = arr[1] = inimg[widthCnt+( (heightCnt-1)*width)  ];
                arr[2] = inimg[widthCnt+1+( (heightCnt-1)*width)  ];
                arr[3] = arr[4] = inimg[widthCnt+(heightCnt*width) ];
                arr[5] = inimg[widthCnt+1+(heightCnt*width) ];
                arr[6] = arr[7] = inimg[widthCnt+ ( (heightCnt+1)*width)  ];
                arr[8] = inimg[widthCnt+1+( (heightCnt+1)*width)  ];
            }

            cnt=0;
            float sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*arr[cnt++];
                }
            }
            *(outimg + i) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(inimg + i) - *(outimg + i));
        }

        else if( widthCnt==(width*1 -1) ){
            //RightUpVertex
            if(heightCnt==0){
                arr[0] = arr[3] = inimg[widthCnt-1 + (heightCnt*width)  ];
                arr[1] = arr[2] = arr[5] = arr[4] = inimg[widthCnt + (heightCnt*width)  ];
                arr[6] = inimg[widthCnt-1 + ((heightCnt-1)*width)  ];
                arr[7] = arr[8] = inimg[widthCnt+((heightCnt+1)*width) ];
            }
            //RightDownVertex
            else if(heightCnt==height-1){
                arr[0] = inimg[widthCnt-1 + ((heightCnt-1)*width)  ];
                arr[1] = arr[2] = inimg[widthCnt-1 +((heightCnt-1)*width)  ];
                arr[3] = arr[6] = inimg[widthCnt-1+(heightCnt*width) ];
                arr[4] = arr[5] = arr[7] = arr[8] = inimg[widthCnt+(heightCnt*width) ];
            }
            else{
                arr[0] = inimg[widthCnt-1 + ((heightCnt-1)*width)  ];
                arr[2] = arr[1] = inimg[widthCnt + ((heightCnt-1)*width)  ];
                arr[3] = inimg[widthCnt-1 + (heightCnt*width)  ];
                arr[5] = arr[4] = inimg[widthCnt+(heightCnt*width)  ];
                arr[6] = inimg[widthCnt-1 + ((heightCnt+1)*width)  ];
                arr[8] = arr[7] = inimg[widthCnt+((heightCnt+1)*width)  ];
            }
            cnt=0;
            float sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*arr[cnt++];
                }
            }
            *(outimg + i ) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(inimg + i) - *(outimg + i) );
        }
        else if(heightCnt==0){
            if( widthCnt!=1 && widthCnt!=width-1 ){
                arr[0] = arr[3] = inimg[widthCnt-1+(heightCnt*width)  ];
                arr[1] = arr[4] = inimg[widthCnt+(heightCnt*width) ];
                arr[2] = arr[5] = inimg[widthCnt+1+(heightCnt*width)  ];
                arr[6] = inimg[widthCnt-1+((heightCnt+1)*width)  ];
                arr[7] = inimg[widthCnt+((heightCnt+1)*width)  ];
                arr[8] = inimg[widthCnt+1 + ((heightCnt+1)*width)  ];

                cnt=0;
                float sum = 0.0;
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        sum += kernel[i+1][j+1]*arr[cnt++];
                    }
                }
                *(outimg + i ) = LIMIT_UBYTE(sum);
                *(mask + i) = LIMIT_UBYTE( *(inimg + i) - *(outimg + i) );
            }
        }
        else if( heightCnt ==(height -1) ){
            if( widthCnt!=1 && widthCnt!=width-1 ){
                arr[0] = inimg[widthCnt-1+((heightCnt-1)*width) ];
                arr[1] = inimg[widthCnt+((heightCnt-1)*width) ];
                arr[2] = inimg[widthCnt+1+((heightCnt-1)*width) ];
                arr[3] = arr[6] = inimg[widthCnt-1+(heightCnt*width) ];
                arr[4] = arr[7] = inimg[widthCnt+(heightCnt*width) ];
                arr[5] = arr[8] = inimg[widthCnt+1+(heightCnt*width) ];
                cnt=0;
                float sum = 0.0;
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        sum += kernel[i+1][j+1]*arr[cnt++];
                    }
                }
                *(outimg + i ) = LIMIT_UBYTE(sum);
                *(mask + i) =LIMIT_UBYTE( *(inimg + i) - *(outimg + i) );
            }
        }
        else{
            float sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*inimg[((widthCnt+i*1)+(heightCnt+j)*width) ];
                }
            }
            *(outimg + i) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(inimg + i) - *(outimg + i) );
        }
    }

}
/* 하이부스트 필터(선예도) 함수
 * @param 연산할 이미지의 픽셀 데이터
 * @param unsharp Value
 * @return 하이부스트 필터 연산 결과
*/
unsigned char* PanoPreset::highBoost(unsigned char* in, int sbValue)
{
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    int sharpen = sbValue * 2.5;

    for (int i = 0; i < imageSize; i += 1) {
        *(outimg + i) = LIMIT_UBYTE ( *(in + i) + sharpen * *(mask + i) );    //highBoost = 원본이미지 + k * mask 값
    }

    return outimg;
}
/* 비등방성 확산 필터(DeNoise) 함수
 * @param 연산할 이미지의 픽셀 데이터
 * @param 반복 수
 * @return 비등방성 확산 필터 연산 결과
*/
unsigned char* PanoPreset::ADFilter(unsigned char* inimg ,int iter)
{
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
/* 저역통과 필터 함수
 * @param 연산할 이미지의 픽셀 데이터
 * @param 주파수 대역
 * @return 저역 통과 필터 연산 결과
 */
unsigned char* PanoPreset::lowPassFFT(unsigned char* in, int cutoff)
{
    memset(fftImg, 0, sizeof(unsigned char) * dentalViewWidth*dentalViewHeight);

    QImage currentImg;

    FourierProcessing fourier(dentalViewWidth, dentalViewHeight, in);
    fourier.lowPassGaussian(fftImg, cutoff);
    currentImg = QImage(fftImg, dentalViewWidth, dentalViewHeight, QImage::Format_Grayscale8);

    QPixmap fourierPixmap;
    fourierPixmap = pixmap.fromImage(currentImg);
    fourier.deleteMemory();

    return currentImg.bits();
}
/* 영상 load 시, 영상 프리셋 연산 슬롯
 * @param panoramaForm 에서 Load 하거나
 *        DB에서 load 한 pano 이미지 Pixmap
*/
void PanoPreset::receiveFile(QPixmap& roadPixmap)
{
    pixmap = roadPixmap;
    defaultImg = pixmap.scaled(dentalViewWidth, dentalViewHeight).toImage();

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);

    inimg = image.bits();

    width = image.width();
    height = image.height();

    imageSize = width * height;

    outimg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    mask = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    copyImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    copyImg2 = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    fftImg = (unsigned char*)malloc(sizeof(unsigned char) * dentalViewWidth * dentalViewHeight);

    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(mask, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg2, 0, sizeof(unsigned char) * imageSize);
    memset(fftImg, 0, sizeof(unsigned char) * dentalViewWidth * dentalViewHeight);

    set3x3MaskValue();  // 영상의 Mask 값 구함

    /* 영상의 평균 value를 저장하기 위한 연산 */
    for(int i = 0; i < imageSize; i ++){
        avg += inimg[i];
    }
    avg = avg/imageSize;

    /* 1~6번까지의 프리셋 설정 */
    setPreset_1();
    setPreset_2();
    setPreset_3();
    setPreset_4();
    setPreset_5();
    setPreset_6();

}
/* panoramaForm에서 입력받은 번호에 따른 영상 반환 슬롯
* @param panoramaForm의 프리셋 번호
*/
void PanoPreset::receievePreset(int preset)
{
    /* Preset에 맞는 영상 SIGNAL 전송 */
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

    emit panoPresetSend(pixmap);    //panoramaForm으로 프리셋 영상 전송
    emit panoPresetAdj(pixmap);     //연산 클래스로 프리셋 영상 전송

    pixmap = QPixmap();             //전송 후 pixmap 초기화
}
