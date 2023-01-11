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
	FILE* fp, *darkFP;
	ubyte16* inimg, * outimg;
	int width = 1628, height = 1628;

	struct _finddata_t fd;
	long handle;
	int result = 1;

	if ((fp = fopen("./S1/MTF_V.raw", "rb")) == NULL) {
		fprintf(stderr, "please input usage : input.raw");
		return -1;
	}

	// �Է� raw ����, dark calib�� ���� raw ���� �޸� �Ҵ�
	int imgSize = width * height;
	inimg = (ubyte16*)malloc((sizeof(ubyte16) * imgSize));
	fread(inimg, sizeof(ubyte16), imgSize, fp);

	ubyte16* rawimg = (ubyte16*)malloc(sizeof(ubyte16) * imgSize);
	memset(rawimg, 0, (sizeof(ubyte16) * imgSize));

	fclose(fp);
	// ���� �� raw ���� ��� �о����
	std::string path = "./S1/Dark/";

	handle = _findfirst("./S1/Dark/*.raw", &fd);
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
			rawimg[i] += oneimg[i];
			//printf("%d\n", rawimg[i]);
		}

		free(oneimg);
		fclose(darkFP);

		result = _findnext(handle, &fd);
	}
	_findclose(handle);

	// dark ��հ� raw ���� ����
	for (int i = 0; i < imgSize; i++) {
		rawimg[i] /= 101;
	}

	fp = fopen("./S1/avgDark.raw", "wb");
	fwrite(rawimg, sizeof(ubyte16), imgSize, fp);

	fclose(fp);
	 
	//'���� gray value - ��հ�' ���� �� ���ο� raw ���Ϸ� ����
	outimg = (ubyte16*)malloc((sizeof(ubyte16) * imgSize));
	memset(outimg, 0, sizeof(ubyte16) * imgSize);

		for (int i = 0; i < imgSize; i++) {

			if (inimg[i] > rawimg[i])
				outimg[i] = inimg[i] - (rawimg[i]);

			else if (inimg[i] < rawimg[i])
				outimg[i] = abs(inimg[i] - (rawimg[i]));
		}

	if ((fp = fopen("./S1/DARK_MTF_V.raw", "wb")) == NULL) {
		fprintf(stderr, "cannot open this file");
		return -1;
	}

	fwrite(outimg, sizeof(ubyte16), imgSize, fp);

	fclose(fp);

	free(inimg);
	free(outimg);
	free(rawimg);


	return 0;
}