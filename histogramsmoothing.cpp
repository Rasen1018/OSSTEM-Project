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
	FILE* fp;
	ubyte16* inimg, * outimg;
	float r, g, b, gray;
	int i, x, y, k, index;
	unsigned long histogram[65536], sum, sum_of_h[65536];
	double constant;
	int width = 3693, height = 1628;

	/* Pano_3693x1628, Ceph_2500x2400*/
	if ((fp = fopen("./Ceph_2500x2400.raw", "rb")) == NULL) {
		fprintf(stderr, "please input usage : input.raw");
		return -1;
	}

	// �Է� raw ����, dark calib�� ���� raw ���� �޸� �Ҵ�
	int imgSize = width * height;
	inimg = (ubyte16*)malloc((sizeof(ubyte16) * imgSize));


	fread(inimg, sizeof(ubyte16), imgSize, fp);


	fclose(fp);

	//'���� gray value - ��հ�' ���� �� ���ο� raw ���Ϸ� ����
	outimg = (ubyte16*)malloc((sizeof(ubyte16) * imgSize));
	memset(outimg, 0, sizeof(ubyte16) * imgSize);

	/* ������׷� �ʱ�ȭ */
	for (i = 0; i < 65536; i++) histogram[i] = 0;

	/* gray value�� ���� ������׷��� ���� ����*/
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			gray = inimg[x + y * width];
			histogram[(ubyte16)(gray)] += 1;
			outimg[x + y * width] = gray;
		};
	};

	/*���� ������׷��� ���� ���� �� ����*/
	for (i = 0, sum = 0; i <= 65535; i++) {
		sum += histogram[i];
		sum_of_h[i] = sum;
	}

	/* ����ȭ ���� (�ִ� ��ϰ�/��ü �ȼ� ��) */
	constant = (float)(65536) / (float)(height * width);

	/*gray value�� ����ȭ�� ���� ����*/
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			k = outimg[x + y*width];
			outimg[x + y * width] = sum_of_h[k] * constant + 0.5;
		}
	}

	if ((fp = fopen("./Ceph.raw", "wb")) == NULL) {
		fprintf(stderr, "cannot open this file");
		return -1;
	}

	printf("\ngood JOB\n");

	fwrite(outimg, sizeof(ubyte16), imgSize, fp);

	fclose(fp);

	free(inimg);
	free(outimg);

	return 0;

}