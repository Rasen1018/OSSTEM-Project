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

    memcpy(copyImg2, highBoost(inimg, 1), sizeof(unsigned char)*imageSize);

    int contrastValue = 80;
    int contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(outimg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * contrast) -40 );
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

    memcpy(copyImg2, highBoost(inimg, 1), sizeof(unsigned char) * imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(outimg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * 1.4) -40 );
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

    memcpy(copyImg2, highBoost(inimg, 1), sizeof(unsigned char)*imageSize);

    int contrastValue = 80;
    int contrast = (100.0+contrastValue/2)/100.0;

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * contrast) -40 );
    }

    memcpy(copyImg2, highBoost(copyImg, 1), sizeof(unsigned char) * imageSize);
    memcpy(copyImg, ADFilter(copyImg2, 15), sizeof(unsigned char)*imageSize);
    memcpy(outimg, gaussian(copyImg, 0.5), sizeof(unsigned char)*imageSize);

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

    memcpy(copyImg2, highBoost(inimg, 1), sizeof(unsigned char) * imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * 1.4) -40 );
    }

    memcpy(copyImg2, highBoost(copyImg, 1), sizeof(unsigned char) * imageSize);
    memcpy(copyImg, ADFilter(copyImg2, 15), sizeof(unsigned char)*imageSize);
    memcpy(outimg, gaussian(copyImg, 0.5), sizeof(unsigned char)*imageSize);

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
    memcpy(copyImg2, unsharpMask(copyImg, sbValue), sizeof(unsigned char)* imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * contrast)  + bright );
    }
    memcpy(copyImg2, ADFilter(copyImg, adfValue), sizeof(unsigned char)* imageSize);

    gammaValue = 1.2;
    for(int i = 0; i < imageSize; i ++){
        *(outimg + i) = LIMIT_UBYTE( qPow(*(copyImg2 + i) / 255.f , abs(1.f/ gammaValue )) * 255 + 0.f   );
    }

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
    memcpy(copyImg2, unsharpMask(copyImg, sbValue), sizeof(unsigned char)*imageSize);

    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( (avg + (*(copyImg2+i)-avg) * contrast)  + bright );
    }

    memcpy(outimg, ADFilter(copyImg, adfValue), sizeof(unsigned char)*imageSize);

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
            if( widthCnt!=0 && widthCnt!=width-1 ){
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
            if( widthCnt!=0 && widthCnt!=width-1 ){
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
unsigned char* PanoPreset::unsharpMask(unsigned char* in, int sbValue)
{
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    int sharpen = sbValue * 2.5;

    for (int i = 0; i < imageSize; i += 1) {
        *(outimg + i) = LIMIT_UBYTE ( *(in + i) + sharpen * *(mask + i) );
    }

    return outimg;
}

/* 하이부스트 필터(선예도) 함수
 * @param 연산할 이미지의 픽셀 데이터
 * @param unsharp Value
 * @return 하이부스트 필터 연산 결과
*/
unsigned char* PanoPreset::highBoost(unsigned char* in, int sbValue)
{
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    int arr[9];
    int sum;
    float alpha = sbValue*0.03;
    int x = 0, y = -1;
    for(int i = 0; i < imageSize; i ++){
        x = i % width;
        if(i % width == 0) y++;

        if(x==0){
            //LeftUpVertex
            if(y==0){
                arr[0] = arr[1] = arr[3] = arr[4] = in[x+(y*width) ];
                arr[2] = arr[5] = in[x+1 + (y*width) ];
                arr[6] = arr[7] = in[x+ ((y+1)*width)  ];
                arr[8] = in[x+1+((y+1)*width) ];
            }
            //LeftDownVertex
            else if(y==height-1){
                arr[0] = arr[1] =in[x+((y-1)*width) ];
                arr[2] = in[x+1 + ((y-1)*width) ];
                arr[3] = arr[6] = arr[7] = arr[4] = in[x+(y*width)  ];
                arr[8] = arr[5] = in[x+1 + (y*width)  ];
            }
            else{
                arr[0] = arr[1] = in[x+( (y-1)*width)  ];
                arr[2] = in[x+1+( (y-1)*width)  ];
                arr[3] = arr[4] = in[x+(y*width) ];
                arr[5] = in[x+1+(y*width) ];
                arr[6] = arr[7] = in[x+ ( (y+1)*width)  ];
                arr[8] = in[x+1+( (y+1)*width)  ];
            }

            sum = (5  + alpha) *arr[4] - arr[1] - arr[3] - arr[5] - arr[7];
            *(outimg + i ) = LIMIT_UBYTE(sum);
        }

        else if( x==(width*1 -1) ){
            //RightUpVertex
            if(y==0){
                arr[0] = arr[3] = in[x-1 + (y*width)  ];
                arr[1] = arr[2] = arr[5] = arr[4] = in[x + (y*width)  ];
                arr[6] = in[x-1 + ((y-1)*width)  ];
                arr[7] = arr[8] = in[x+((y+1)*width) ];
            }
            //RightDownVertex
            else if(y==height-1){
                arr[0] = in[x-1 + ((y-1)*width)  ];
                arr[1] = arr[2] = in[x-1 +((y-1)*width)  ];
                arr[3] = arr[6] = in[x-1+(y*width) ];
                arr[4] = arr[5] = arr[7] = arr[8] = in[x+(y*width) ];
            }
            else{
                arr[0] = in[x-1 + ((y-1)*width)  ];
                arr[2] = arr[1] = in[x + ((y-1)*width)  ];
                arr[3] = in[x-1 + (y*width)  ];
                arr[5] = arr[4] = in[x+(y*width)  ];
                arr[6] = in[x-1 + ((y+1)*width)  ];
                arr[8] = arr[7] = in[x+((y+1)*width)  ];
            }
            sum = (5  + alpha) *arr[4] - arr[1] - arr[3] - arr[5] - arr[7];
            *(outimg + i ) = LIMIT_UBYTE(sum);
        }
        else if(y==0){
            if( x!=0 && x!=width-1 ){
                arr[0] = arr[3] = in[x-1+(y*width)  ];
                arr[1] = arr[4] = in[x+(y*width) ];
                arr[2] = arr[5] = in[x+1+(y*width)  ];
                arr[6] = in[x-1+((y+1)*width)  ];
                arr[7] = in[x+((y+1)*width)  ];
                arr[8] = in[x+1 + ((y+1)*width)  ];

                sum = (5  + alpha) *arr[4] - arr[1] - arr[3] - arr[5] - arr[7];
                *(outimg + i ) = LIMIT_UBYTE(sum);
            }
        }
        else if( y ==(height -1) ){
            if( x!=0 && x!=width-1 ){
                arr[0] = in[x-1+((y-1)*width) ];
                arr[1] = in[x+((y-1)*width) ];
                arr[2] = in[x+1+((y-1)*width) ];
                arr[3] = arr[6] = in[x-1+(y*width) ];
                arr[4] = arr[7] = in[x+(y*width) ];
                arr[5] = arr[8] = in[x+1+(y*width) ];

                sum = (5  + alpha) *arr[4] - arr[1] - arr[3] - arr[5] - arr[7];
                *(outimg + i ) = LIMIT_UBYTE(sum);
            }
        }
        else{
            sum = (5  + alpha) *in[x+y*width] -
                    in[(x-1)+y*width] - in[(x+1)+y*width] - in[x+(y+1)*width] - in[x+(y-1)*width];
        outimg[x+y*width] = LIMIT_UBYTE(sum + 0.5f);
        }
    }
    return outimg;
}
/* 비등방성 확산 필터(DeNoise) 함수
 * @param 연산할 이미지의 픽셀 데이터
 * @param 반복 수
 * @return 비등방성 확산 필터 연산 결과
*/
unsigned char* PanoPreset::ADFilter(unsigned char* in ,int iter)
{
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    unsigned char * copy;
    copy = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    memcpy(copy, in, sizeof(unsigned char) * imageSize);

    int i;
    float gradn=0.0, grads=0.0, grade=0.0, gradw=0.0;
    float gcn=0.0, gcs=0.0, gce=0.0, gcw=0.0;
    const float lambda = 0.25;
    const float k = 4;
    const float k2 = k * k;

    /* iter 횟수만큼 비등방성 확산 알고리즘 수행 */
    for (i = 0; i < iter; i++)
    {
        int x = 0, y = -1;
        for (int j = 0; j < imageSize; j += 1) {
            x = j % width;
            if(j % width == 0) y++;

            if(y == 0) {
                //(0,0)
                if(x == 0) {
                    grads = copy[(y + 1) * width + x] - copy[y * width + x];
                    gradw = copy[y * width + (x+1)] - copy[y * width + x];

                    gcs = grads / (1.0f + grads * grads / k2);
                    gcw = gradw / (1.0f + gradw * gradw / k2);

                    outimg[y * width + x] = copy[y * width + x] + lambda * (gcs + gcw);
                }
                //(width,0)
                else if(x == width -1) {
                    grads = copy[(y + 1) * width + x] - copy[y * width + x];
                    grade = copy[y * width + (x-1)] - copy[y * width + x];

                    gcs = grads / (1.0f + grads * grads / k2);
                    gce = grade / (1.0f + grade * grade / k2);

                    outimg[y * width + x] = copy[y * width + x] + lambda * (gcs + gce);
                }
                //(x,0)
                else {
                    grads = copy[(y + 1) * width + x] - copy[y * width + x];
                    grade = copy[y * width + (x-1)] - copy[y * width + x];
                    gradw = copy[y * width + (x+1)] - copy[y * width + x];

                    gcs = grads / (1.0f + grads * grads / k2);
                    gce = grade / (1.0f + grade * grade / k2);
                    gcw = gradw / (1.0f + gradw * gradw / k2);

                    outimg[y * width + x] = copy[y * width + x]
                            + lambda * (gcs + gce + gcw);
                }
            }

            else if(y == height - 1) {
                //(0, height)
                if(x == 0) {
                    gradn = copy[(y - 1) * width + x] - copy[y * width + x];
                    gradw = copy[y * width + (x+1)] - copy[y * width + x];

                    gcn = gradn / (1.0f + gradn * gradn / k2);
                    gcw = gradw / (1.0f + gradw * gradw / k2);

                    outimg[y * width + x] = copy[y * width + x] + lambda * (gcn + gcw);
                }
                //(width, height)
                else if (x == width - 1) {
                    gradn = copy[(y - 1) * width + x] - copy[y * width + x];
                    grade = copy[y * width + (x-1)] - copy[y * width + x];

                    gcn = gradn / (1.0f + gradn * gradn / k2);
                    gce = grade / (1.0f + grade * grade / k2);

                    outimg[y * width + x] = copy[y * width + x] + lambda * (gcn + gce);
                }
                //(x, height)
                else {
                    gradn = copy[(y - 1) * width + x] - copy[y * width + x];
                    grade = copy[y * width + (x-1)] - copy[y * width + x];
                    gradw = copy[y * width + (x+1)] - copy[y * width + x];

                    gcn = gradn / (1.0f + gradn * gradn / k2);
                    gce = grade / (1.0f + grade * grade / k2);
                    gcw = gradw / (1.0f + gradw * gradw / k2);

                    outimg[y * width + x] = copy[y * width + x]
                            + lambda * (gcn + gce + gcw);
                }
            }

            else if(x == 0) {
                //(0, y)
                if(0 < y && y < height - 1) {
                    gradn = copy[(y - 1) * width + x] - copy[y * width + x];
                    grads = copy[(y + 1) * width + x] - copy[y * width + x];
                    gradw = copy[y * width + (x+1)] - copy[y * width + x];

                    gcn = gradn / (1.0f + gradn * gradn / k2);
                    gcs = grads / (1.0f + grads * grads / k2);
                    gcw = gradw / (1.0f + gradw * gradw / k2);

                    outimg[y * width + x] = copy[y * width + x]
                            + lambda * (gcn + gcs + gcw);
                }
            }

            else if(x == width - 1) {
                //(width, y)
                if(0 < y && y < height - 1) {
                    gradn = copy[(y - 1) * width + x] - copy[y * width + x];
                    grads = copy[(y + 1) * width + x] - copy[y * width + x];
                    grade = copy[y * width + (x-1)] - copy[y * width + x];

                    gcn = gradn / (1.0f + gradn * gradn / k2);
                    gcs = grads / (1.0f + grads * grads / k2);
                    gce = grade / (1.0f + grade * grade / k2);

                    outimg[y * width + x] = copy[y * width + x]
                            + lambda * (gcn + gcs + gce);
                }
            }

            else {
                //비등방성 확산 필터 수식(안쪽)
                gradn = copy[(y - 1) * width + x] - copy[y * width + x];
                grads = copy[(y + 1) * width + x] - copy[y * width + x];
                grade = copy[y * width + (x-1)] - copy[y * width + x];
                gradw = copy[y * width + (x+1)] - copy[y * width + x];

                gcn = gradn / (1.0f + gradn * gradn / k2);
                gcs = grads / (1.0f + grads * grads / k2);
                gce = grade / (1.0f + grade * grade / k2);
                gcw = gradw / (1.0f + gradw * gradw / k2);

                outimg[y * width + x] = copy[y * width + x]
                        + lambda * (gcn + gcs + gce + gcw);
            }
        }
        if (i < iter - 1)
            std::memcpy((unsigned char*)copy, outimg, sizeof(unsigned char) * width * height);
    }
    free(copy);

    return outimg;
}

/* 가우시안 필터(블러) 함수
 * 2차원 가우시안 함수 값을 이용하여 마스크를 생성, 입력 영상과 마스크 연산을 수행
 * x 방향과 y 방향으로의 1차원 마스크 연산을 각각 수행
 *
 * @param 연산할 이미지의 픽셀 데이터
 * @param sigma 값
 * 함수 호출 후 연산 결과는 prevImg에 저장
 */
unsigned char* PanoPreset::gaussian(unsigned char* in, float sigma)
{
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    float* pBuf;
    pBuf = (float*)malloc(sizeof(float) * width * height);

    int i, j, k, x;

    int dim = static_cast<int>(2 * 4 * sigma + 1.0);    //마스크 크기

    if (dim < 3) dim = 3;                               //최소 마스크 크기 계산(3x3)
    if (dim % 2 == 0) dim++;                            //마스크 크기는 홀수
    int dim2 = dim / 2;

    //1차원 가우시안 마스크 생성
    float* pMask = new float[dim];

    for (i = 0; i < dim; i++) {
        //x 값의 범위는 -4 * sigma부터 +4 * sigma까지
        x = i - dim2;
        //평균이 0이고, 표준 편차가 sigma인 1차원 가우시안 분포의 함수 식 표현
        pMask[i] = exp(-(x*x) / (2 * sigma * sigma)) / (sqrt(2 * PI) * sigma);
    }

    float sum1, sum2;

    //세로 방향 마스크 연산
    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {

            sum1 = sum2 = 0.f;
            for (k = 0; k < dim; k++) {

                x = k - dim2 + j;
                if (x>= 0 && x <height) {
                    sum1 += pMask[k];
                    sum2 += (pMask[k] * in[x + i*height]);
                }
            }
            pBuf[j+ i*height] = sum2 / sum1;
        }
    }
    //가로 방향 마스크 연산
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
