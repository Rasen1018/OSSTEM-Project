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
	ubyte16* firstimg, *outimg;
	int width = 48, height = 2400, cnt = 1;
	int imgSize = width * height;
	int outImgSize = 0;
	bool flag = true;

	struct _finddata_t fd;
	long handle;
	int result = 1;

	// 폴더 내 raw 파일 모두 읽어오기
	//std::string path = "./Ceph_10/";
	std::string path = "./Ceph_48x2400/";

	//handle = _findfirst("./Ceph_10/*.raw", &fd);
	handle = _findfirst("./Ceph_48x2400/*.raw", &fd);
	if (handle == -1) return -1;

	while (result != -1)
	{
		// 임시 raw 파일 메모리 할당
		outImgSize = (width + (2 * cnt)) * height;
		outimg = (ubyte16*)malloc((sizeof(ubyte16) * outImgSize));

		ubyte16* nextimg = (ubyte16*)malloc(sizeof(ubyte16) * imgSize);

		// raw 파일 경로 저장
		std::string name = path + fd.name;

		if (flag) {
			fp = fopen(name.c_str(), "rb");
			firstimg = (ubyte16*)malloc(sizeof(ubyte16) * imgSize);
			fread(firstimg, sizeof(ubyte16), imgSize, fp);
			flag = false;
		}
		else {

			if ((cephFP = fopen(name.c_str(), "rb")) == NULL) {
				printf("never give up, %d", fd.size);
				return -1;
			}

			printf("파일명 : %s, cnt : %d \n", name.c_str(), cnt);

			fread(nextimg, sizeof(ubyte16), imgSize, cephFP);

			memset(outimg, 0, sizeof(ubyte16) * outImgSize);

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < (width + (2 * cnt)); x++) {
					if (x < (width + (2 * cnt) - 2))
						outimg[y * (width + (2 * cnt)) + x] = firstimg[y * width + x];
					else
						outimg[y * (width + (2 * cnt)) + x] = nextimg[y * width + (x-(2*cnt))];
				}
			}

			//free(firstimg);

			//firstimg = (ubyte16*)malloc((sizeof(ubyte16) * outImgSize));
			//memset(firstimg, 0, (sizeof(ubyte16) * outImgSize));
			//memcpy(firstimg, outimg, sizeof(outimg));
			memmove(firstimg, outimg, sizeof(outimg));
			printf("pixel : first [ %d ], next [ %d ], outimg [ %d ] \n", firstimg[33], nextimg[33], outimg[outImgSize - 480]);			
			//firstimg = (ubyte16*)malloc(sizeof(ubyte16) * outImgSize);
			fclose(cephFP);
			cnt++;

		}

		free(nextimg);

		result = _findnext(handle, &fd);
	}
	_findclose(handle);

	fclose(fp);

	
	if ((fp = fopen("./Ceph_10.raw", "wb")) == NULL) {
		fprintf(stderr, "cannot open this file");
		return -1;
	}

	fwrite(outimg, sizeof(ubyte16), outImgSize, fp);

	fclose(fp);
	free(outimg);
	free(firstimg);

	return 0;
}