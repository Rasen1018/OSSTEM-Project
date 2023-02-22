#include "cephvalueadjustment.h"
#include "fourierprocessing.h"

#include <QImage>

#define LIMIT_UBYTE(n) (n > UCHAR_MAX) ? UCHAR_MAX : (n < 0) ? 0 : n
#define PI 3.1416926535f
#define MAX_SIZE 9
#define SWAP(x, y, temp) ( (temp)=(x), (x)=(y), (y)=(temp) )

CephValueAdjustment::CephValueAdjustment(QObject *parent)
    : QObject{parent}
{
}
/* unsharp mask filter 적용을 위한 (원본 - 평균값) mask 값 연산 함수
 * 영상의 mask 값은 *mask에 저장
 */
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
             * blenImg = 블렌딩 한 이미지(연조직과 골조직의 분리를 위한 이미지)
             * mask = 원본 이미지 - 블러된 이미지 */
            *(outimg + i ) = LIMIT_UBYTE(sum);
            *(mask + i) = LIMIT_UBYTE( *(blenImg + i) - *(outimg + i) );
        }
        else if(y==0){
            if( x!=0 && x!=width-1 ){
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
            if( x!=0 && x!=width-1 ){
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
/* 하이부스트 필터(선예도) 함수
 * @param 연산할 이미지의 픽셀 데이터
 * @param unsharp Value
 * 함수 호출 후 연산 결과는 prevImg에 저장
 */
void CephValueAdjustment::highBoost(unsigned char* in, int sbValue)
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
            if( x!=1 && x!=width-1 ){
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
            if( x!=1 && x!=width-1 ){
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
    prevImg = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
}
/* 가우시안 필터(블러) 함수
 * 2차원 가우시안 함수 값을 이용하여 마스크를 생성, 입력 영상과 마스크 연산을 수행
 * x 방향과 y 방향으로의 1차원 마스크 연산을 각각 수행
 *
 * @param 연산할 이미지의 픽셀 데이터
 * @param sigma 값
 * 함수 호출 후 연산 결과는 prevImg에 저장
 */
void CephValueAdjustment::gaussian(unsigned char* in, float sigma)
{
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    //연산을 위한 임시 이미지 배열
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
    prevImg = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();

    free(pBuf);
    delete[] pMask;
}
/* 비등방성 확산 필터(DeNoise) 함수
 * 상하 좌우 대칭의 형태를 갖는 필터 마스크를 사용하는 필터를 등방성 필터
 *
 * @param 연산할 이미지의 픽셀 데이터
 * @param 비등방성 필터 반복 수
 * 함수 호출 후 연산 결과는 prevImg에 저장
*/
void CephValueAdjustment::ADFilter(unsigned char * in, int iter)
{    //deNoising , 다른 연산 수행 함수
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    unsigned char * copy;
    copy = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    memcpy(copy, in, sizeof(unsigned char) * imageSize);

    int i;
    float gradn=0.0, grads=0.0, grade=0.0, gradw=0.0;
    float gcn=0.0, gcs=0.0, gce=0.0, gcw=0.0;
    float lambda = 0.25;
    float k = 4;
    float k2 = k * k;

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
            memcpy(copy, outimg, sizeof(unsigned char) * width * height);
    }
    free(copy);
    prevImg = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
}

// 1. 피벗을 기준으로 2개의 부분 리스트로 나눈다.
// 2. 피벗보다 작은 값은 모두 왼쪽 부분 리스트로, 큰 값은 오른쪽 부분 리스트로 옮긴다.
/* 2개의 비균등 배열 list[left...pivot-1]와 list[pivot+1...right]의 합병 과정 */
/* (실제로 숫자들이 정렬되는 과정) */
int CephValueAdjustment::partition(int list[], int left, int right){
  int pivot, temp;
  int low, high;

  low = left;
  high = right + 1;
  pivot = list[left]; // 정렬할 리스트의 가장 왼쪽 데이터를 피벗으로 선택(임의의 값을 피벗으로 선택)

  /* low와 high가 교차할 때까지 반복(low<high) */
  do{
    /* list[low]가 피벗보다 작으면 계속 low를 증가 */
    do {
      low++; // low는 left+1 에서 시작
    } while (low<=right && list[low]<pivot);

    /* list[high]가 피벗보다 크면 계속 high를 감소 */
    do {
      high--; //high는 right 에서 시작
    } while (high>=left && list[high]>pivot);

    // 만약 low와 high가 교차하지 않았으면 list[low]를 list[high] 교환
    if(low<high){
      SWAP(list[low], list[high], temp);
    }
  } while (low<high);

  // low와 high가 교차했으면 반복문을 빠져나와 list[left]와 list[high]를 교환
  SWAP(list[left], list[high], temp);

  // 피벗의 위치인 high를 반환
  return high;
}

// 퀵 정렬
void CephValueAdjustment::quick_sort(int list[], int left, int right){

  /* 정렬할 범위가 2개 이상의 데이터이면(리스트의 크기가 0이나 1이 아니면) */
  if(left<right){
    // partition 함수를 호출하여 피벗을 기준으로 리스트를 비균등 분할 -분할(Divide)
    int q = partition(list, left, right); // q: 피벗의 위치

    // 피벗은 제외한 2개의 부분 리스트를 대상으로 순환 호출
    quick_sort(list, left, q-1); // (left ~ 피벗 바로 앞) 앞쪽 부분 리스트 정렬 -정복(Conquer)
    quick_sort(list, q+1, right); // (피벗 바로 뒤 ~ right) 뒤쪽 부분 리스트 정렬 -정복(Conquer)
  }
}

/* 영상 load 시 연산 클래스 메모리 할당 및 설정
 * @param cephaloForm 에서 Load 하거나 DB에서 load 한 영상 Pixmap
 */
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

    outimg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    mask = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    sharpenImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    copyImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    gammaImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    fftImg = (unsigned char*)malloc(sizeof(unsigned char) * cephViewWidth * cephViewHeight);
    medianFilterImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);
    blenImg = (unsigned char*)malloc(sizeof(unsigned char) * imageSize);

    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(mask, 0, sizeof(unsigned char) * imageSize);
    memset(sharpenImg, 0, sizeof(unsigned char) * imageSize);
    memset(copyImg, 0, sizeof(unsigned char) * imageSize);
    memset(gammaImg, 0, sizeof(unsigned char) * imageSize);
    memset(fftImg, 0, sizeof(unsigned char) * cephViewWidth * cephViewHeight);
    memset(medianFilterImg, 0, sizeof(unsigned char) * imageSize);
    memset(blenImg, 0, sizeof(unsigned char) * imageSize);

    /* 연조직과 골조직의 분리를 위해 감마 조절 후 blending 한 이미지 생성*/
    for(int i = 0; i < imageSize; i ++){
        *(copyImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / 0.6 )) * 255 + 0.f   );
        *(sharpenImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / 1.4 )) * 255 + 0.f   );
        *(blenImg + i) = *(copyImg + i)*0.5 + *(sharpenImg + i)*0.5;
    }

    set3x3MaskValue();  // 영상의 Mask 값은 blending 한 이미지로 구현.

    /* 영상의 평균 value를 저장하기 위한 연산 */
    for(int i = 0; i < imageSize; i ++){
        avg += inimg[i];
    }
    avg = avg/imageSize;
}
/* 프리셋 버튼 클릭 시, 설정 슬롯
 * @param 프리셋 클래스의 presetImg
 */
void CephValueAdjustment::receiveSetPresetImg(QPixmap& prePixmap)
{
    memset(inimg, 0, sizeof(unsigned char) * imageSize);

    QImage presetImg;
    presetImg = prePixmap.scaled(cephViewWidth, cephViewHeight).toImage();
    currentImg = presetImg.convertToFormat(QImage::Format_Grayscale8).copy();

    inimg = currentImg.bits();
}
/* cephaloForm의 slider 값에 따라 연산하는 슬롯
 * @param 밝기 값
 * @param 대조 값
 * @param 선예도
 * @param deNoising
 * @param 감마 값
 */
void CephValueAdjustment::changeCephValue(int brightValue, int contrastValue,int sbValue,
                                          int deNoiseValue, int gammaValue )
{
    QImage image;

    float gamma;
    float contrast;
    memset(outimg, 0, sizeof(unsigned char) * imageSize);
    memset(gammaImg, 0, sizeof(unsigned char) * imageSize);

    if(brightValue == 0 && contrastValue == 0 && sbValue == 0  && gammaValue !=0 && deNoiseValue != 0){
        gamma = 1.0 + gammaValue*0.02;

        for(int i = 0; i < imageSize; i ++){
            *(gammaImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / gamma )) * 255 + 0.f   );
        }
        ADFilter(gammaImg, deNoiseValue * 2);
        image = prevImg;
    }
    /* 밝기값만 조정되는 case */
    if(contrastValue == 0 && sbValue == 0 && deNoiseValue == 0 && gammaValue ==0){
        int value =  brightValue / 2.5;
        for(int i = 0; i < imageSize; i ++){
            *(outimg + i) = LIMIT_UBYTE( *(inimg + i) + value );
        }
    }
    /* 대조값만 조정되는 case */
    if(brightValue == 0 && sbValue == 0 && deNoiseValue == 0 && gammaValue ==0){
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
    /* unsharp값만 조정되는 case */
    if(brightValue == 0 && contrastValue == 0 && deNoiseValue == 0 && gammaValue ==0){
        if(sbValue < 0)
            gaussian(inimg, (float)sbValue*(-0.5));
        else if(sbValue > 0)
            highBoost(inimg, sbValue);
        else  prevImg = defaultImg;

        image = prevImg;
    }

    /* DeNoising 만 조정되는 case */
    if(brightValue == 0 && contrastValue == 0 && sbValue == 0 && gammaValue ==0) {
        int adfValue = 2 * deNoiseValue;

        ADFilter(inimg, adfValue);

        image = prevImg;
    }
    /* Gamma 조정 case */
    if(brightValue == 0 && contrastValue == 0 && sbValue == 0 && deNoiseValue ==0){
        if(gammaValue ==0){
            for(int i = 0; i < imageSize; i ++){
                *(outimg + i) = *(inimg + i);
            }
        }
        else{   //gammaValue가 0이 아닌 경우
            gamma = 1.0 + gammaValue*0.02;

            for(int i = 0; i < imageSize; i ++){
                *(outimg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / gamma )) * 255 + 0.f   );
            }
        }

    }
    /* 두 개 이상의 값이 조정되는 case */
    //else{
    if(brightValue != 0){
        int value =  brightValue / 2.5;
        if(gammaValue ==0){
            for(int i = 0; i < imageSize; i ++){
                *(gammaImg + i) = *(inimg + i);
            }
        }
        else{   //gammaValue가 0이 아닌 경우
            gamma = 1.0 + gammaValue*0.02;

            for(int i = 0; i < imageSize; i ++){
                *(gammaImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / gamma )) * 255 + 0.f   );
            }
        }
        if(deNoiseValue == 0){  // deNoising이 조정되지 않을 경우
            if(sbValue != 0){   // unsharp이 조정된 경우
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
                    gaussian(gammaImg,1.5);
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
                sharpenImg = image.bits();  //sharpen한 연산 후 bright, contrast 연산.
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
            else if(sbValue == 0){ // unsharp이 조정되지 않은 경우
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
        else { //deNoising 이 조정 된 경우

            int adfValue = 2 * deNoiseValue;

            if(sbValue != 0){   // unsharp이 조정된 경우
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
                sharpenImg = image.bits();  //sharpen한 연산 후 bright, contrast 연산.
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
            else if(sbValue == 0){ // unsharp이 조정되지 않은 경우
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
    else{
        int value =  brightValue / 2.5;
        if(gammaValue ==0){
            for(int i = 0; i < imageSize; i ++){
                *(gammaImg + i) = *(inimg + i);
            }
        }
        else{   //gammaValue가 0이 아닌 경우
            gamma = 1.0 + gammaValue*0.02;

            for(int i = 0; i < imageSize; i ++){
                *(gammaImg + i) = LIMIT_UBYTE( qPow(*(inimg + i) / 255.f , abs(1.f / gamma )) * 255 + 0.f   );
            }
        }
        if(deNoiseValue == 0){  // deNoising이 조정되지 않을 경우
            if(sbValue != 0){   // unsharp이 조정된 경우
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
                    gaussian(gammaImg,1.5);
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
                sharpenImg = image.bits();  //sharpen한 연산 후 bright, contrast 연산.
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
            else if(sbValue == 0){ // unsharp이 조정되지 않은 경우
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
        else { //deNoising 이 조정 된 경우

            int adfValue = 2 * deNoiseValue;

            if(sbValue != 0){   // unsharp이 조정된 경우
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
                sharpenImg = image.bits();  //sharpen한 연산 후 bright, contrast 연산.
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
            else if(sbValue == 0){ // unsharp이 조정되지 않은 경우
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
    emit cephImgSend(pixmap);   //후처리 연산 결과를 cephaloForm으로 시그널 전송

    //현재 연상중인 이미지 저장
    calcImg = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();      //연산 결과 이미지 저장
}

/* 평탄화버튼 클릭 시, 평탄화 연산 슬롯
 * @param 현재 View에 있는 후처리 영상
 */
void CephValueAdjustment::receivePrev(QPixmap& pixmap)
{
    memset(outimg, 0, sizeof(unsigned char) * imageSize);

    //현재 View의 QPixmap -> QImage 변환
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
     * constant = ( max gray value x 이미지 사이즈)  */
    constant = (float)(256) / (float)(height * width);
    for (int i = 0; i < imageSize; i++) {
        k = outimg[i];
        outimg[i] = LIMIT_UBYTE( sum_of_h[k] * constant );

        inimg[i] = outimg[i];   //평활화 후 이미지 연산은 평활화 한 이미지로 진행
    }

    image = QImage(outimg, width, height, QImage::Format_Grayscale8).copy();
    pixmap = pixmap.fromImage(image);
    emit cephImgSend(pixmap);   //평탄화 연산 결과를 panoramaForm으로 시그널 전송
}
/* 리셋 버튼 클릭 시, 초기 설정 슬롯 */
void CephValueAdjustment::setResetImg()
{
    memset(inimg, 0, sizeof(unsigned char) * imageSize);

    image = defaultImg.convertToFormat(QImage::Format_Grayscale8).copy();
    inimg = image.bits();

    calcImg = QImage();
}
/* median filter 연산 슬롯
 * @parama UNUSED
 */
void CephValueAdjustment::median(int value)
{
    Q_UNUSED(value);

    memset(medianFilterImg, 0, sizeof(unsigned char) * imageSize);
    if(calcImg.isNull() != 1) inimg = calcImg.bits();

    int imageSize = width * height;
    int rowSize = width;
    int widthCnt = 0, heightCnt = -1;
    int cnt = 0;

    int arr[9] = { 0, };

    for (int i = 0; i < imageSize; i++) {
        widthCnt = i % width;
        if (i % width == 0) heightCnt++;

        if (widthCnt == 0) {
            //좌측 상단 Vertex
            if (heightCnt == 0) {
                arr[0] = arr[1] = arr[3] = arr[4] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[2] = arr[5] = inimg[widthCnt + 1 + (heightCnt * rowSize)];
                arr[6] = arr[7] = inimg[widthCnt + ((heightCnt + 1) * rowSize)];
                arr[8] = inimg[widthCnt + 1 + ((heightCnt + 1) * rowSize)];
            }
            //좌측 하단 Vertex
            else if (heightCnt == height - 1) {
                arr[0] = arr[1] = inimg[widthCnt + ((heightCnt - 1) * rowSize)];
                arr[2] = inimg[widthCnt + 1 + ((heightCnt - 1) * rowSize)];
                arr[3] = arr[6] = arr[7] = arr[4] = inimg[widthCnt + ((heightCnt * rowSize))];
                arr[8] = arr[5] = inimg[widthCnt + 1 + (heightCnt * rowSize)];
            }
            else {
                arr[0] = arr[1] = inimg[widthCnt + ((heightCnt - 1) * rowSize)];
                arr[2] = inimg[widthCnt + 1 + ((heightCnt - 1) * rowSize)];
                arr[3] = arr[4] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[5] = inimg[widthCnt + 1 + (heightCnt * rowSize)];
                arr[6] = arr[7] = inimg[widthCnt + ((heightCnt + 1) * rowSize)];
                arr[8] = inimg[widthCnt + 1 + ((heightCnt + 1) * rowSize)];
            }

            quick_sort(arr,0, 8);
            medianFilterImg[(widthCnt + heightCnt * rowSize)] = arr[4];
        }
        else if (widthCnt == (rowSize - 1)) {
            //우측 상단 Vertex
            if (heightCnt == 0) {
                arr[0] = arr[3] = inimg[widthCnt - 1 + (heightCnt * rowSize)];
                arr[1] = arr[2] = arr[5] = arr[4] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[6] = inimg[widthCnt - 1 + ((heightCnt - 1) * rowSize)];
                arr[7] = arr[8] = inimg[widthCnt + ((heightCnt + 1) * rowSize)];
            }
            //우측 하단 Vertex
            else if (heightCnt == height - 1) {
                arr[0] = inimg[widthCnt - 1 + ((heightCnt - 1) * rowSize)];
                arr[1] = arr[2] = inimg[widthCnt - 1 + ((heightCnt - 1) * rowSize)];
                arr[3] = arr[6] = inimg[widthCnt - 1 + (heightCnt * rowSize)];
                arr[4] = arr[5] = arr[7] = arr[8] = inimg[widthCnt + (heightCnt * rowSize)];
            }
            else {
                arr[0] = inimg[widthCnt - 1 + ((heightCnt - 1) * rowSize)];
                arr[2] = arr[1] = inimg[widthCnt + ((heightCnt - 1) * rowSize)];
                arr[3] = inimg[widthCnt - 1 + (heightCnt * rowSize)];
                arr[5] = arr[4] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[6] = inimg[widthCnt - 1 + ((heightCnt + 1) * rowSize)];
                arr[8] = arr[7] = inimg[widthCnt + ((heightCnt + 1) * rowSize)];
            }

            quick_sort(arr,0, 8);
            medianFilterImg[(widthCnt + heightCnt * rowSize)] = arr[4];
        }
        else if (heightCnt == 0) {
            if (widthCnt != 1 && widthCnt != rowSize - 1) {
                arr[0] = arr[3] = inimg[widthCnt - 1 + (heightCnt * rowSize)];
                arr[1] = arr[4] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[2] = arr[5] = inimg[widthCnt + 1 + (heightCnt * rowSize)];
                arr[6] = inimg[widthCnt - 1 + ((heightCnt + 1) * rowSize)];
                arr[7] = inimg[widthCnt + ((heightCnt + 1) * rowSize)];
                arr[8] = inimg[widthCnt + 1 + ((heightCnt + 1) * rowSize)];
            }

            quick_sort(arr,0, 8);
            medianFilterImg[(widthCnt + heightCnt * rowSize)] = arr[4];
        }
        else if (heightCnt == (height - 1)) {
            if (widthCnt != 1 && widthCnt != rowSize - 1) {
                arr[0] = inimg[widthCnt - 1 + ((heightCnt - 1) * rowSize)];
                arr[1] = inimg[widthCnt + ((heightCnt - 1) * rowSize)];
                arr[2] = inimg[widthCnt + 1 + ((heightCnt - 1) * rowSize)];
                arr[3] = arr[6] = inimg[widthCnt - 1 + (heightCnt * rowSize)];
                arr[4] = arr[7] = inimg[widthCnt + (heightCnt * rowSize)];
                arr[5] = arr[8] = inimg[widthCnt + 1 + (heightCnt * rowSize)];
            }

            quick_sort(arr,0, 8);
            medianFilterImg[(widthCnt + heightCnt * rowSize)] = arr[4];
        }
        else {
            cnt = 0;
            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    arr[cnt++] = inimg[((widthCnt + i) + (heightCnt + j) * width)];
                }
            }
            quick_sort(arr,0, 8);
            medianFilterImg[(widthCnt + heightCnt * rowSize)] = arr[4];
        }
    }
    QPixmap medianPixmap;

    currentImg = QImage(medianFilterImg, width, height, QImage::Format_Grayscale8).copy();
    medianPixmap = pixmap.fromImage(currentImg);

    inimg = currentImg.bits();      //inimg를 연산한 이미지 데이터로 복사
    emit cephImgSend(medianPixmap); //후처리 연산 영상 결과를 panoramaForm으로 시그널 전송
    emit exitFilterSignal();        //필터 연산 후 panoramaForm으로 시그널 전송
}

/* 주파수 내 저역 가우시안 필터링 적용 함수
 * @param 저역 통과 대역 설정 (filterButton에서 입력한 값)
 */
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
    emit cephImgSend(fourierPixmap);    //후처리 연산 영상 결과를 panoramaForm으로 시그널 전송

    inimg = currentImg.bits();

    fourier.deleteMemory();             //메모리 제거
    emit exitFilterSignal();            //필터 연산 후 panoramaForm으로 시그널 전송
}

/* 주파수 내 고역 가우시안 필터링 적용 함수
 * @param 고역 통과 대역 설정 (filterButton에서 입력한 값)
 */
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
    emit cephImgSend(fourierPixmap);     //후처리 연산 영상 결과를 panoramaForm으로 시그널 전송

    inimg = currentImg.bits();

    fourier.deleteMemory();              //메모리 제거
    emit exitFilterSignal();             //필터 연산 후 panoramaForm으로 시그널 전송
}
