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
	ubyte16* inimg;
	int width = 80, height = 1628;

	struct _finddata_t fd;
	long handle;
	int result = 1;

	if ((fp = fopen("./dark200_avg.raw", "rb")) == NULL) {
		fprintf(stderr, "please input usage : input.raw");
		return -1;
	}

	// �Է� raw ����, dark calib�� ���� raw ���� �޸� �Ҵ�
	int imgSize = width * height;
	inimg = (ubyte16*)malloc((sizeof(ubyte16) * imgSize));
	fread(inimg, sizeof(ubyte16), imgSize, fp);

	fclose(fp);
	// ���� �� raw ���� ��� �о����
	std::string path = "./Pano_80x1628/";
	std::string savePath = "./result/";

	handle = _findfirst("./Pano_80x1628/*.raw", &fd);
	if (handle == -1) return -1;
	while (result != -1)
	{
		// �ӽ� raw ���� �޸� �Ҵ�
		ubyte16* oneimg = (ubyte16*)malloc(sizeof(ubyte16) * imgSize);
		ubyte16* outimg = (ubyte16*)malloc(sizeof(ubyte16) * imgSize);

		// raw ���� ��� ����
		std::string name = path + fd.name;
		std::string saveName = savePath + fd.name;

		if ((darkFP = fopen(name.c_str(), "rb")) == NULL) {
			printf("never give up, %d", fd.size);
			return -1;
		}
		printf("���ϸ� : %s\n", name.c_str());

		memset(oneimg, 0, (sizeof(ubyte16) * imgSize));
		fread(oneimg, sizeof(ubyte16) * imgSize, 1, darkFP);

		memset(outimg, 0, sizeof(ubyte16) * imgSize);

		// pixel �� ���ϱ�
		for (int i = 0; i < imgSize; i++) {

			if (oneimg[i] > inimg[i])
				outimg[i] = oneimg[i] - (ubyte16)(inimg[i]);

			else if (inimg[i] > oneimg[i])
				outimg[i] = abs(oneimg[i] - (ubyte16)(inimg[i]));
		}

		free(oneimg);
		fclose(darkFP);

		if ((fp = fopen(saveName.c_str(), "wb")) == NULL) {
			fprintf(stderr, "cannot open this file");
			return -1;
		}

		fwrite(outimg, sizeof(ubyte16), imgSize, fp);

		fclose(fp);
		free(outimg);

		result = _findnext(handle, &fd);
	}
	_findclose(handle);

	printf("\nSuccess Dark Calibration!!\n");
	return 0;
}