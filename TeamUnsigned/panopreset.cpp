#include "panopreset.h"
#include "qdebug.h"

#include <QImage>
#include <QPixmap>

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n
#define PI 3.1416926535f

typedef quint8 ubyte8;

PanoPreset::PanoPreset(QObject *parent)
    : QObject{parent}
{
}
void PanoPreset::receiveFile(QPixmap& roadPixmap){
    pixmap = roadPixmap;
    defaultImg = pixmap.scaled(dentalViewWidth, dentalViewHeight).toImage();

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);

    inimg = image.bits();

    width = image.width();
    height = image.height();
    imageSize = width * height;

    outimg = (ubyte8*)malloc(sizeof(ubyte8) * imageSize);
    mask = (ubyte8*)malloc(sizeof(ubyte8) * imageSize);
    copyImg = (ubyte8*)malloc(sizeof(ubyte8) * imageSize);
    copyImg2 = (ubyte8*)malloc(sizeof(ubyte8) * imageSize);

    memset(outimg, 0, sizeof(ubyte8) * imageSize);
    memset(mask, 0, sizeof(ubyte8) * imageSize);
    memset(copyImg, 0, sizeof(ubyte8) * imageSize);
    memset(copyImg2, 0, sizeof(ubyte8) * imageSize);

    set3x3MaskValue(inimg);  // 영상의 Mask 값 구함

    for(int i = 0; i < imageSize; i ++){ //영상의 평균 value를 저장하기 위함
        avg += inimg[i];
    }

    avg = avg/imageSize;

}

void PanoPreset::receievePreset(int preset){
    /* Preset Num에 따라 반환 */

    switch(preset) {
    case 1:
        setPreset_1();
        if(presetImg.isNull()) return;
        pixmap = pixmap.fromImage(presetImg);
        break;

    case 2:
        setPreset_2();
        if(presetImg.isNull()) return;
        pixmap = pixmap.fromImage(presetImg);
        break;

    case 3:
        setPreset_3();
        if(presetImg.isNull()) return;
        pixmap = pixmap.fromImage(presetImg);
        break;

    case 4:
        setPreset_4();
        if(presetImg.isNull()) return;
        pixmap = pixmap.fromImage(presetImg);
        break;

    case 5:
        setPreset_5();
        if(presetImg.isNull()) return;
        pixmap = pixmap.fromImage(presetImg);
        break;

    case 6:
        setPreset_6();
        if(presetImg.isNull()) return;
        pixmap = pixmap.fromImage(presetImg);
        break;
    }

    emit panoPresetSend(pixmap);
    emit panoPresetAdj(pixmap);

}

void PanoPreset::setPreset_1(){
    //gamma 보정,
    // sharpen 3
    // contrast Value = 20

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(ubyte8) * imageSize);
    memset(copyImg, 0, sizeof(ubyte8) * imageSize);
    memset(copyImg2, 0, sizeof(ubyte8) * imageSize);

    int sbValue = 3;
    int contrastValue = 20;

    double gammaValue = 0.8;

    float contrast;
    contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs((50/21.0) - gammaValue )) * 255 + 0.f   );
    }

    copyImg2 = (highBoost(copyImg, sbValue));

    contrastValue = 20;
    contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * contrast)  );
    }

    sbValue = 2;
    copyImg2 = highBoost(copyImg2, sbValue);

    int brightValue = -40;
    int bright = brightValue / 2.5;
    contrastValue = 30;
    contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(outimg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * contrast) +bright );
    }

    presetImg = QImage(outimg, width, height, QImage::Format_Grayscale8);

}

void PanoPreset::setPreset_2(){
    //gamma 보정,
    // sharpen 3
    // contrast Value = 20

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(ubyte8) * imageSize);
    memset(copyImg, 0, sizeof(ubyte8) * imageSize);
    memset(copyImg2, 0, sizeof(ubyte8) * imageSize);

    int sbValue = 3;
    int contrastValue = 20;

    double gammaValue = 0.8;

    float contrast;
    contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs((50/21.0) - gammaValue )) * 255 + 0.f   );
    }

    copyImg2 = (highBoost(copyImg, sbValue));

    contrastValue = 20;
    contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(outimg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * contrast)  );
    }

    presetImg = QImage(outimg, width, height, QImage::Format_Grayscale8);

}   //preset2
void PanoPreset::setPreset_3(){
    // 평탄화 후
    // bright : -20
    // contrast : -40
    // sb : 3

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(ubyte8) * imageSize);
    memset(copyImg, 0, sizeof(ubyte8) * imageSize);
    memset(copyImg2, 0, sizeof(ubyte8) * imageSize);

    int brightValue = -20;
    int sbValue = 3;
    int contrastValue = -40;
    //    int deNoiseValue = 4;

    int bright = brightValue / 2.5;
    //    int adfValue = 2 * deNoiseValue;
    float contrast;
    contrast = (100.0+contrastValue/2)/100.0;

    int histo[256], sum_of_h[256];
    int value,k;
    float constant;

    /* 히스토그램 평탄화 진행 */
    ubyte8 *histoInimg;
    histoInimg = image.bits();

    for(int i = 0; i < 256; i ++) {
        histo[i] =0;
        sum_of_h[i] = 0;
    }
    for (int i = 0; i < imageSize; i++) {   //histogram 분포
        value = histoInimg[i];
        histo[value] += 1;
        outimg[i] = value;
    }
    //histogram
    for (int i = 0, sum = 0; i < 256; i++){
        sum += histo[i];
        sum_of_h[i] = sum;
    }
    /* constant = new # of gray levels div by area */
    constant = (float)(256) / (float)(height * width);
    for (int i = 0; i < imageSize; i++) {
        k = outimg[i];
        outimg[i] = LIMIT_UBYTE( sum_of_h[k] * constant );
        copyImg[i] = outimg[i];
    }

    copyImg2 = highBoost(copyImg, sbValue);

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * contrast)  + bright );
    }

    float gammaValue = 0.7;
    for(int i = 0; i < imageSize; i ++){
        *(copyImg2 + i) = LIMIT_UBYTE( qPow(*(copyImg + i) / 255.f , 1.f/ gammaValue ) * 255 + 0.f   );
    }

    copyImg = highBoost(copyImg2, 4);

    brightValue = 20;
    bright = brightValue / 2.5;
    contrastValue = 20;
    contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg2 + i) = LIMIT_UBYTE( (avg + (*(copyImg+i)-avg) * contrast)  + bright );
    }

    int deNoiseValue = 3;
    int adfValue = 2 * deNoiseValue;

    outimg = ADFilter(copyImg2,adfValue);

    presetImg = QImage(outimg, width, height, QImage::Format_Grayscale8);

}   //preset 3

void PanoPreset::setPreset_4(){
    // gammaValue = 0.8
    // gamma Value 후
    // brightValue : 20
    // contrast : 50
    // sb : 6
    // DeNoising : 5
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(ubyte8) * imageSize);
    memset(copyImg, 0, sizeof(ubyte8) * imageSize);
    memset(copyImg2, 0, sizeof(ubyte8) * imageSize);

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

    set3x3MaskValue(copyImg);  // 영상의 Mask 값 구함
    copyImg2 = highBoost(copyImg, sbValue);

    memset(copyImg, 0, sizeof(ubyte8) * imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * contrast)  + bright );
    }

    outimg = ADFilter(copyImg, adfValue);

    presetImg = QImage(outimg, width, height, QImage::Format_Grayscale8);

}

void PanoPreset::setPreset_5(){
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(ubyte8) * imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(outimg + i) = 128;
    }

    presetImg = QImage(outimg, width, height, QImage::Format_Grayscale8);
}
void PanoPreset::setPreset_6(){
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = image.bits();       //inimg 초기화

    memset(outimg, 0, sizeof(ubyte8) * imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(outimg + i) = 255;
    }

    presetImg = QImage(outimg, width, height, QImage::Format_Grayscale8);
}

ubyte8 *PanoPreset::gaussian(float sigma){
    ubyte8 *outimg;
    outimg = (ubyte8*)malloc(sizeof(ubyte8) * imageSize);
    memset(outimg, 0, sizeof(ubyte8) * imageSize);

    float* pBuf;
    pBuf = (float*)malloc(sizeof(float) * width * height);

    int i, j, k, x;

    int dim = static_cast<int>(2 * 4 * sigma + 1.0);

    if (dim < 3) dim = 3;
    if (dim % 2 == 0) dim++;
    int dim2 = dim / 2;

    float* pMask = new float[dim];

    for (i = 0; i < dim; i++) {
        x = i - dim2;
        pMask[i] = exp(-(x*x) / (2 * sigma * sigma)) / (sqrt(2 * PI) * sigma);
    }

    float sum1, sum2;

    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {

            sum1 = sum2 = 0.f;
            for (k = 0; k < dim; k++) {

                x = k - dim2 + j;
                if (x>= 0 && x <height) {
                    sum1 += pMask[k];
                    sum2 += (pMask[k] * inimg[x + i*height]);
                }
            }
            pBuf[j+ i*height] = sum2 / sum1;
        }
    }

    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            sum1 = sum2 = 0.f;

            for (k = 0; k < dim; k++) {

                x = k - dim2 + i;
                if ( x>= 0 && x < width) {
                    sum1 += pMask[k];
                    sum2 += (pMask[k] * pBuf[j*width + x]);
                }
            }
            outimg[i + j * width] = sum2 / sum1;
        }
    }

    free(pBuf);
    delete[] pMask;
    return outimg;
}

ubyte8* PanoPreset::highBoost(int sbValue){
    ubyte8 *outimg;
    outimg = (ubyte8*)malloc(sizeof(ubyte8) * imageSize);
    memset(outimg, 0, sizeof(ubyte8) * imageSize);

    int sharpen = sbValue * 2.5;

    for (int i = 0; i < imageSize; i += 1) {
        *(outimg + i) = LIMIT_UBYTE ( *(inimg + i) + sharpen * *(mask + i) );    //highBoost = 원본이미지 + k * mask 값
    }
    return outimg;
    //hiBoostImg = QImage(outimg, width, height, QImage::Format_Grayscale8);
}
ubyte8* PanoPreset::highBoost(ubyte8* in, int sbValue){

    ubyte8 *outimg;
    outimg = (ubyte8*)malloc(sizeof(ubyte8) * imageSize);
    memset(outimg, 0, sizeof(ubyte8) * imageSize);

    int sharpen = sbValue * 2.5;

    for (int i = 0; i < imageSize; i += 1) {
        *(outimg + i) = LIMIT_UBYTE ( *(in + i) + sharpen * *(mask + i) );    //highBoost = 원본이미지 + k * mask 값
    }

    return outimg;
}
ubyte8* PanoPreset::ADFilter(ubyte8* inimg ,int iter){
    ubyte8 *outimg;
    outimg = (ubyte8*)malloc(sizeof(ubyte8) * imageSize);
    memset(outimg, 0, sizeof(ubyte8) * imageSize);

    float lambda = 0.25;
    float k = 4;

    QImage copyImage;
    copyImage = QImage(inimg,width,height,QImage::Format_Grayscale8);

    ubyte8* copy = copyImage.bits();

    /* iter 횟수만큼 비등방성 확산 알고리즘 수행 */
    int i;
    float gradn, grads, grade, gradw;
    float gcn, gcs, gce, gcw;
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
            std::memcpy((ubyte8*)copy, outimg, sizeof(ubyte8) * width * height);
    }

    memcpy(copy, outimg, sizeof(ubyte8) * width * height);
    free(outimg);
    return copy;
    //adfImg = QImage(outimg, width, height, QImage::Format_Grayscale8);
}

void PanoPreset::set3x3MaskValue(ubyte8* in){
    memset(outimg, 0, sizeof(ubyte8) * imageSize);
    memset(mask, 0, sizeof(ubyte8) * imageSize);

    double kernel[3][3] = { {1/9.0, 1/9.0, 1/9.0},  //평균값 필터를 이용한 mask 값
                            {1/9.0, 1/9.0, 1/9.0},
                            {1/9.0, 1/9.0, 1/9.0}};
    //    double kernel[3][3] = { {-1, -1, -1},         //라플라시안 필터를 이용한 mask 값
    //                           {-1, 9, -1},
    //                           {-1, -1, -1}};
    int arr[9] = {0};


    int widthCnt = 0, heightCnt = -1, cnt = 0;
    for(int i = 0; i < imageSize; i ++){
        widthCnt = i % width;
        if(i % width == 0) heightCnt++;

        if(widthCnt==0){
            //LeftUpVertex
            if(heightCnt==0){
                arr[0] = arr[1] = arr[3] = arr[4] = in[widthCnt+(heightCnt*width) ];
                arr[2] = arr[5] = in[widthCnt+1 + (heightCnt*width) ];
                arr[6] = arr[7] = in[widthCnt+ ((heightCnt+1)*width)  ];
                arr[8] = in[widthCnt+1+((heightCnt+1)*width) ];
            }
            //LeftDownVertex
            else if(heightCnt==height-1){
                arr[0] = arr[1] =in[widthCnt+((heightCnt-1)*width) ];
                arr[2] = in[widthCnt+1 + ((heightCnt-1)*width) ];
                arr[3] = arr[6] = arr[7] = arr[4] = in[widthCnt+(heightCnt*width)  ];
                arr[8] = arr[5] = in[widthCnt+1 + (heightCnt*width)  ];
            }
            else{
                arr[0] = arr[1] = in[widthCnt+( (heightCnt-1)*width)  ];
                arr[2] = in[widthCnt+1+( (heightCnt-1)*width)  ];
                arr[3] = arr[4] = in[widthCnt+(heightCnt*width) ];
                arr[5] = in[widthCnt+1+(heightCnt*width) ];
                arr[6] = arr[7] = in[widthCnt+ ( (heightCnt+1)*width)  ];
                arr[8] = in[widthCnt+1+( (heightCnt+1)*width)  ];
            }

            cnt=0;
            float sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*arr[cnt++];
                }
            }
            *(outimg + i) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(in + i) - *(outimg + i));
        }

        else if( widthCnt==(width*1 -1) ){
            //RightUpVertex
            if(heightCnt==0){
                arr[0] = arr[3] = in[widthCnt-1 + (heightCnt*width)  ];
                arr[1] = arr[2] = arr[5] = arr[4] = in[widthCnt + (heightCnt*width)  ];
                arr[6] = in[widthCnt-1 + ((heightCnt-1)*width)  ];
                arr[7] = arr[8] = in[widthCnt+((heightCnt+1)*width) ];
            }
            //RightDownVertex
            else if(heightCnt==height-1){
                arr[0] = in[widthCnt-1 + ((heightCnt-1)*width)  ];
                arr[1] = arr[2] = in[widthCnt-1 +((heightCnt-1)*width)  ];
                arr[3] = arr[6] = in[widthCnt-1+(heightCnt*width) ];
                arr[4] = arr[5] = arr[7] = arr[8] = in[widthCnt+(heightCnt*width) ];
            }
            else{
                arr[0] = in[widthCnt-1 + ((heightCnt-1)*width)  ];
                arr[2] = arr[1] = in[widthCnt + ((heightCnt-1)*width)  ];
                arr[3] = in[widthCnt-1 + (heightCnt*width)  ];
                arr[5] = arr[4] = in[widthCnt+(heightCnt*width)  ];
                arr[6] = in[widthCnt-1 + ((heightCnt+1)*width)  ];
                arr[8] = arr[7] = in[widthCnt+((heightCnt+1)*width)  ];
            }
            cnt=0;
            float sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*arr[cnt++];
                }
            }
            *(outimg + i ) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(in + i) - *(outimg + i) );
        }
        else if(heightCnt==0){
            if( widthCnt!=1 && widthCnt!=width-1 ){
                arr[0] = arr[3] = in[widthCnt-1+(heightCnt*width)  ];
                arr[1] = arr[4] = in[widthCnt+(heightCnt*width) ];
                arr[2] = arr[5] = in[widthCnt+1+(heightCnt*width)  ];
                arr[6] = in[widthCnt-1+((heightCnt+1)*width)  ];
                arr[7] = in[widthCnt+((heightCnt+1)*width)  ];
                arr[8] = in[widthCnt+1 + ((heightCnt+1)*width)  ];

                cnt=0;
                float sum = 0.0;
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        sum += kernel[i+1][j+1]*arr[cnt++];
                    }
                }
                *(outimg + i ) = LIMIT_UBYTE(sum);
                *(mask + i) = LIMIT_UBYTE( *(in + i) - *(outimg + i) );
            }
        }
        else if( heightCnt ==(height -1) ){
            if( widthCnt!=1 && widthCnt!=width-1 ){
                arr[0] = in[widthCnt-1+((heightCnt-1)*width) ];
                arr[1] = in[widthCnt+((heightCnt-1)*width) ];
                arr[2] = in[widthCnt+1+((heightCnt-1)*width) ];
                arr[3] = arr[6] = in[widthCnt-1+(heightCnt*width) ];
                arr[4] = arr[7] = in[widthCnt+(heightCnt*width) ];
                arr[5] = arr[8] = in[widthCnt+1+(heightCnt*width) ];
                cnt=0;
                float sum = 0.0;
                for(int i = -1; i < 2; i++) {
                    for(int j = -1; j < 2; j++) {
                        sum += kernel[i+1][j+1]*arr[cnt++];
                    }
                }
                *(outimg + i ) = LIMIT_UBYTE(sum);
                *(mask + i) =LIMIT_UBYTE( *(in + i) - *(outimg + i) );
            }
        }
        else{
            float sum = 0.0;
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    sum += kernel[i+1][j+1]*in[((widthCnt+i*1)+(heightCnt+j)*width) ];
                }
            }
            *(outimg + i) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(in + i) - *(outimg + i) );
        }
    }

    qDebug() << "setting highBoost : mask Value" ;
}
