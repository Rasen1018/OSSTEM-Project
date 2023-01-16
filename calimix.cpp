#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <string>
#include <vector>

typedef unsigned short ubyte16;
typedef std::vector<std::string> stringvec;

inline unsigned int clip(int value, int min, int max);

inline unsigned int clip(int value, int min, int max) {
	return (value > max ? max : value < min ? min : value);
}


int main(int argc, char** argv)
{
	FILE* fp, *gainFP, *darkFP;
	ubyte16* inimg, *darkimg, *gainimg, * outimg;
	int width = 1536, height = 1536;

	if ((fp = fopen("./S2/MTF_H.raw", "rb")) == NULL) {
		fprintf(stderr, "please input usage : input.raw");
		return -1;
	}

	if ((gainFP = fopen("./S2/avgGain2.raw", "rb")) == NULL) {
		fprintf(stderr, "please input usage : input.raw");
		return -1;
	}

	if ((darkFP = fopen("./S2/avgDark2.raw", "rb")) == NULL) {
		fprintf(stderr, "please input usage : input.raw");
		return -1;
	}

	// 입력 raw 파일, dark calib를 위한 raw 파일 메모리 할당
	int imgSize = width * height;
	inimg = (ubyte16*)malloc((sizeof(ubyte16) * imgSize));
	darkimg = (ubyte16*)malloc((sizeof(ubyte16) * imgSize));
	gainimg = (ubyte16*)malloc((sizeof(ubyte16) * imgSize));

	fread(inimg, sizeof(ubyte16), imgSize, fp);
	fread(darkimg, sizeof(ubyte16), imgSize, darkFP);
	fread(gainimg, sizeof(ubyte16), imgSize, gainFP);

	fclose(fp);
	fclose(darkFP);
	fclose(gainFP);


	//'원본 gray value - 평균값' 적용 후 새로운 raw 파일로 저장
	outimg = (ubyte16*)malloc((sizeof(ubyte16) * imgSize));
	memset(outimg, 0, sizeof(ubyte16) * imgSize);

	for (int i = 0; i < imgSize; i++) {

		outimg[i] = (ubyte16)((abs(inimg[i] - darkimg[i]) / (float)gainimg[i]) * gainimg[i]);
	}

	if ((fp = fopen("./S2/CCC.raw", "wb")) == NULL) {
		fprintf(stderr, "cannot open this file");
		return -1;
	}

	printf("\ngood JOB\n");

	fwrite(outimg, sizeof(ubyte16), imgSize, fp);

	fclose(fp);

	free(inimg);
	free(darkimg);
	free(gainimg);
	free(outimg);

	return 0;
}