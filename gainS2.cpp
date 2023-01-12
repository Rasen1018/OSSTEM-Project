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
	FILE* fp, * darkFP;
	ubyte16* inimg, * outimg;
	int width = 1536, height = 1536;

	struct _finddata_t fd;
	long handle;
	int result = 1;

	if ((fp = fopen("./S2/MTF_H.raw", "rb")) == NULL) {
		fprintf(stderr, "please input usage : input.raw");
		return -1;
	}

	// �Է� raw ����, dark calib�� ���� raw ���� �޸� �Ҵ�
	int imgSize = width * height;
	inimg = (ubyte16*)malloc((sizeof(ubyte16) * imgSize));
	fread(inimg, sizeof(ubyte16), imgSize, fp);

	float* rawimg = (float*)malloc(sizeof(float) * imgSize);
	memset(rawimg, 0, (sizeof(float) * imgSize));

	fclose(fp);
	// ���� �� raw ���� ��� �о����
	std::string path = "./S2/Gain2/";

	handle = _findfirst("./S2/Gain2/*.raw", &fd);
	if (handle == -1) return -1;
	while (result != -1)
	{
		// �ӽ� raw ���� �޸� �Ҵ�
		ubyte16* oneimg = (ubyte16*)malloc(sizeof(ubyte16) * imgSize);

		// raw ���� ��� ����
		std::string name = path + fd.name;

		if ((darkFP = fopen(name.c_str(), "rb")) == NULL) {
			printf("never give up, %d", fd.size);
			return -1;
		}
		printf("���ϸ� : %s\n", name.c_str());

		memset(oneimg, 0, (sizeof(ubyte16) * imgSize));
		fread(oneimg, sizeof(ubyte16) * imgSize, 1, darkFP);

		// pixel �� ���ϱ�
		for (int i = 0; i < imgSize; i++) {
			rawimg[i] += (float)oneimg[i];
			//printf("%d\n", oneimg[i]);
		}

		free(oneimg);
		fclose(darkFP);

		result = _findnext(handle, &fd);
	}
	_findclose(handle);

	//'���� gray value - ��հ�' ���� �� ���ο� raw ���Ϸ� ����
	outimg = (ubyte16*)malloc((sizeof(ubyte16) * imgSize));
	memset(outimg, 0, sizeof(ubyte16) * imgSize);

	for (int i = 0; i < imgSize; i++) {

		if (inimg[i] > rawimg[i])
			outimg[i] = inimg[i] - (ubyte16)(rawimg[i] / 101);

		else if (inimg[i] < rawimg[i])
			outimg[i] = abs(inimg[i] - (ubyte16)(rawimg[i] / 101));
	}


	if ((fp = fopen("./S2/GAIN_MTF_H.raw", "wb")) == NULL) {
		fprintf(stderr, "cannot open this file");
		return -1;
	}

	fwrite(outimg, sizeof(ubyte16), imgSize, fp);

	fclose(fp);

	// gain ��հ� raw ���� ����
	memset(outimg, 0, sizeof(ubyte16) * imgSize);

	for (int i = 0; i < imgSize; i++) {
		outimg[i] = (ubyte16)(rawimg[i] / 101);
		//printf("%f  ", rawimg[i]);
	}

	fp = fopen("./S2/avgGain2.raw", "wb");
	fwrite(outimg, sizeof(ubyte16), imgSize, fp);

	fclose(fp);

	free(inimg);
	free(outimg);
	free(rawimg);

	return 0;
}