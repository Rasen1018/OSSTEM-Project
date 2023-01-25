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
	FILE* fp, * cephFP;
	ubyte16* outimg;

	// pano ���� ũ��� ����
	int width = 80, height = 1628, cnt = 0;
	int outWidth = 3693;
	int imgSize = width * height;
	bool flag = true;

	struct _finddata_t fd;
	long handle;
	int result = 1;

	// ���� ��� ���� ����� �޸� �Ҵ�
	outimg = (ubyte16*)malloc(sizeof(ubyte16) * outWidth * height);
	memset(outimg, 0, sizeof(ubyte16) * outWidth * height);

	// ���� �� raw ���� ��� �о����
	std::string path = "./Pano_80x1628/";

	handle = _findfirst("./Pano_80x1628/*.raw", &fd);
	if (handle == -1) return -1;

	while (result != -1)
	{
		// ���� �� raw ���� �޸� �Ҵ�
		ubyte16* oneimg = (ubyte16*)malloc(sizeof(ubyte16) * imgSize);

		// raw ���� ��� ����
		std::string name = path + fd.name;

		// ���ϸ��� ���� ���ۿ� ����
		if ((cephFP = fopen(name.c_str(), "rb")) == NULL) {
			printf("never give up, %d", fd.size);
			return -1;
		}
		printf("���ϸ� : %s\n", name.c_str());

		// ������ ���۷� �о����
		memset(oneimg, 0, (sizeof(ubyte16) * imgSize));
		fread(oneimg, sizeof(ubyte16) * imgSize, 1, cephFP);

		// 2�ȼ��� ����
		for (int y = 0; y < height; y++) {
			int i = 0;
			for (int x = 0; x < width; x++) {
				if (width / 2 < x && x < width / 2 + 4) {
					outimg[y * outWidth + (i + cnt)] = ~oneimg[y * width + x];
					i++;
				}
			}
		}
		cnt += 3;

		free(oneimg);
		fclose(cephFP);

		result = _findnext(handle, &fd);
	}
	_findclose(handle);

	// ���� ��� ���� ����
	if ((fp = fopen("./Pano_3693x1628.raw", "wb")) == NULL) {
		fprintf(stderr, "cannot open this file");
		return -1;
	}

	fwrite(outimg, sizeof(ubyte16), outWidth * height, fp);

	fclose(fp);
	free(outimg);

	return 0;
}