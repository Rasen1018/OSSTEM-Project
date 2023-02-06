#include "cephpreset.h"

#include <QImage>
#include <QPixmap>

#define LIMIT_UBYTE(n) (n > USHRT_MAX) ? USHRT_MAX : (n < 0) ? 0 : n
#define PI 3.1416926535f

typedef unsigned short ushort;

CephPreset::CephPreset(QObject *parent)
    : QObject{parent}
{
}
void CephPreset::receiveFile(QPixmap& roadPixmap){
    pixmap = roadPixmap;
    defaultImg = pixmap.scaled(cephViewWidth, cephViewHeight).toImage();

    image = defaultImg.convertToFormat(QImage::Format_Grayscale16);

    inimg = reinterpret_cast<ushort*>(image.bits() );

    width = image.width();
    height = image.height();
    imageSize = width * height;

    outimg = (ushort*)malloc(sizeof(ushort) * imageSize);
    mask = (ushort*)malloc(sizeof(ushort) * imageSize);

    memset(outimg, 0, sizeof(ushort) * imageSize);
    memset(mask, 0, sizeof(ushort) * imageSize);

    sharpenImg = (ushort*)malloc(sizeof(ushort) * imageSize);
    memset(sharpenImg, 0, sizeof(ushort) * imageSize);

    copyImg = (ushort*)malloc(sizeof(ushort) * imageSize);
    memset(copyImg, 0, sizeof(ushort) * imageSize);
    set3x3MaskValue();  // 영상의 Mask 값 구함

    for(int i = 0; i < imageSize; i ++){ //영상의 평균 value를 저장하기 위함
        avg += inimg[i];
    }

    avg = avg/imageSize;

}

void CephPreset::receievePreset(int preset){
    /* Preset Num에 따라 반환 */

    switch(preset) {
    case 1:
        setPreset_1();
        pixmap = pixmap.fromImage(presetImg);
        break;

    case 2:
        setPreset_2();
        pixmap = pixmap.fromImage(presetImg);
        break;

    case 3:
        setPreset_3();
        pixmap = pixmap.fromImage(presetImg);
        break;

    case 4:
        setPreset_4();
        pixmap = pixmap.fromImage(presetImg);
        break;

    case 5:
        setPreset_5();
        pixmap = pixmap.fromImage(presetImg);
        break;

    case 6:
        setPreset_6();
        pixmap = pixmap.fromImage(presetImg);
        break;
    }

    emit panoPresetSend(pixmap);
    emit panoPresetAdj(pixmap);

}

void CephPreset::setPreset_1(){
    // bright : -20
    // contrast : 50
    // sb : 4
    // DeNoising : 3

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = reinterpret_cast<ushort*>(image.bits());       //inimg 초기화

    memset(outimg, 0, sizeof(ushort) * imageSize);
    memset(copyImg, 0, sizeof(ushort) * imageSize);
    memset(sharpenImg, 0, sizeof(ushort) * imageSize);

    int brightValue = -20;
    int sbValue = 4;
    int contrastValue = 50;
    int deNoiseValue = 3;

    int bright = brightValue / 2.5;
    int adfValue = 2 * deNoiseValue;
    float contrast;

    highBoost(sbValue);

    sharpenImg = reinterpret_cast<ushort*>(prevImg.bits());  //sharpen한 연산 후 bright, contrast 연산.

    contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(sharpenImg+i)-avg) * contrast)  + bright );
    }

    ADFilter(copyImg, adfValue);

    presetImg = prevImg;
}

void CephPreset::setPreset_2(){
    // bright : 30
    // contrast : -30
    // sb : 5
    // DeNoising : 6

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = reinterpret_cast<ushort*>(image.bits());       //inimg 초기화

    memset(outimg, 0, sizeof(ushort) * imageSize);
    memset(copyImg, 0, sizeof(ushort) * imageSize);
    memset(sharpenImg, 0, sizeof(ushort) * imageSize);

    int brightValue = 30;
    int sbValue = 5;
    int contrastValue = -30;
    int deNoiseValue = 6;

    int bright = brightValue / 2.5;
    int adfValue = 2 * deNoiseValue;
    float contrast;

    highBoost(sbValue); // 5

    sharpenImg = reinterpret_cast<ushort*>(prevImg.bits());  //sharpen한 연산 후 bright, contrast 연산.

    contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(sharpenImg+i)-avg) * contrast)  + bright );
    }
    ADFilter(copyImg, adfValue);

    presetImg = prevImg;
}
void CephPreset::setPreset_3(){
    // 평탄화 후
    // bright : -20
    // contrast : -30
    // sb : 3
    // DeNoising : 3

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = reinterpret_cast<ushort*>(image.bits());       //inimg 초기화

    memset(outimg, 0, sizeof(ushort) * imageSize);
    memset(copyImg, 0, sizeof(ushort) * imageSize);
    memset(sharpenImg, 0, sizeof(ushort) * imageSize);

    int brightValue = -20;
    int sbValue = 3;
    int contrastValue = -30;
    int deNoiseValue = 4;

    int bright = brightValue / 2.5;
    int adfValue = 2 * deNoiseValue;
    float contrast;

    int histo[256], sum_of_h[256];
    int value,k;
    float constant;

    /* 히스토그램 평탄화 진행 */
    ushort *histoInimg;
    histoInimg = reinterpret_cast<ushort*>(image.bits());

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
        inimg[i] = outimg[i];
    }

    //평탄화 후 필터
    highBoost(sbValue);

    sharpenImg = reinterpret_cast<ushort*>(prevImg.bits());  //sharpen한 연산 후 bright, contrast 연산.

    contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(sharpenImg+i)-avg) * contrast)  + bright );
    }
    ADFilter(copyImg, adfValue);
    presetImg = prevImg;


}
void CephPreset::setPreset_4(){
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = reinterpret_cast<ushort*>(image.bits());       //inimg 초기화

    memset(outimg, 0, sizeof(ushort) * imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(outimg + i) = 0;
    }

    presetImg = QImage((unsigned char*)outimg, width, height, QImage::Format_Grayscale8);
}
void CephPreset::setPreset_5(){
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = reinterpret_cast<ushort*>(image.bits());       //inimg 초기화

    memset(outimg, 0, sizeof(ushort) * imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(outimg + i) = 128;
    }

    presetImg = QImage((unsigned char*)outimg, width, height, QImage::Format_Grayscale8);
}
void CephPreset::setPreset_6(){
    image = defaultImg.convertToFormat(QImage::Format_Grayscale8);
    inimg = reinterpret_cast<ushort*>(image.bits() );       //inimg 초기화

    memset(outimg, 0, sizeof(ushort) * imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(outimg + i) = 255;
    }

    presetImg = QImage((unsigned char*)outimg, width, height, QImage::Format_Grayscale8);
}

void CephPreset::gaussian(float sigma){
    memset(outimg, 0, sizeof(ushort) * imageSize);

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
    prevImg = QImage((unsigned char*)outimg, width, height, QImage::Format_Grayscale8);

    free(pBuf);
    delete[] pMask;
}

void CephPreset::highBoost(int sbValue){
    memset(outimg, 0, sizeof(ushort) * imageSize);
    int sharpen = sbValue * 2.5;

    for (int i = 0; i < imageSize; i += 1) {
        *(outimg + i) = LIMIT_UBYTE ( *(inimg + i) + sharpen * *(mask + i) );    //highBoost = 원본이미지 + k * mask 값
    }
    prevImg = QImage((unsigned char*)outimg, width, height, QImage::Format_Grayscale8);
}
void CephPreset::ADFilter(ushort* inimg ,int iter){
    memset(outimg, 0, sizeof(ushort) * imageSize);

    float lambda = 0.25;
    float k = 4;

    QImage copyImage;
    copyImage = QImage((unsigned char*)inimg,width,height,QImage::Format_Grayscale8);

    const ushort* copy = reinterpret_cast<ushort*>(copyImage.bits());

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
            std::memcpy((ushort*)copy, outimg, sizeof(ushort) * width * height);
    }
    prevImg = QImage((unsigned char*)outimg, width, height, QImage::Format_Grayscale8);
}

void CephPreset::set3x3MaskValue(){
    memset(outimg, 0, sizeof(ushort) * imageSize);
    memset(mask, 0, sizeof(ushort) * imageSize);

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
