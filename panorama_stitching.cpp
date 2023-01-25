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

	// pano 영상 크기로 선언
	int width = 80, height = 1628, cnt = 0;
	int outWidth = 3693;
	int imgSize = width * height;
	bool flag = true;

	struct _finddata_t fd;
	long handle;
	int result = 1;

	// 정합 결과 영상에 사용할 메모리 할당
	outimg = (ubyte16*)malloc(sizeof(ubyte16) * outWidth * height);
	memset(outimg, 0, sizeof(ubyte16) * outWidth * height);

	// 폴더 내 raw 파일 모두 읽어오기
	std::string path = "./Pano_80x1628/";

	handle = _findfirst("./Pano_80x1628/*.raw", &fd);
	if (handle == -1) return -1;

	while (result != -1)
	{
		// 정합 전 raw 파일 메모리 할당
		ubyte16* oneimg = (ubyte16*)malloc(sizeof(ubyte16) * imgSize);

		// raw 파일 경로 저장
		std::string name = path + fd.name;

		// 파일마다 파일 버퍼에 저장
		if ((cephFP = fopen(name.c_str(), "rb")) == NULL) {
			printf("never give up, %d", fd.size);
			return -1;
		}
		printf("파일명 : %s\n", name.c_str());

		// 파일을 버퍼로 읽어오기
		memset(oneimg, 0, (sizeof(ubyte16) * imgSize));
		fread(oneimg, sizeof(ubyte16) * imgSize, 1, cephFP);

		// 2픽셀씩 정합
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

	// 정합 결과 영상 저장
	if ((fp = fopen("./Pano_3693x1628.raw", "wb")) == NULL) {
		fprintf(stderr, "cannot open this file");
		return -1;
	}

	fwrite(outimg, sizeof(ubyte16), outWidth * height, fp);

	fclose(fp);
	free(outimg);

	return 0;
}