

#include "stdafx.h" 
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <math.h>



#pragma pack(push,1)
typedef struct {
	unsigned char signature[2];
	unsigned long file_length;
	unsigned long notused;
	unsigned long arr_pointer;

	unsigned long header_length;
	unsigned long img_width;
	unsigned long img_height;
	unsigned short color_pl;
	unsigned short bpp;
	unsigned long additional[6];
} headers;
#pragma pop


FILE* open_file(char* filename)
{
	FILE* file;
	fopen_s(&file, filename, "r+b");
	return file;
}
void gauss(unsigned char* bmp, unsigned char* bmw, unsigned long h, unsigned long w) {
	
	int i = 0, j, iy, ix, r = 5, y,x;
	double val, wght, rs, wsum, dsq;
	rs = ceil(r*2.57);
	for (i = 0; i < h; i++){
		for (j = 0; j < w; j++) {
			val = 0, wsum = 0;
			for (iy = i - rs; iy < i + rs + 1; iy++){
				for (ix = j - rs; ix < j + rs + 1; ix++) {
					x = min(w - 1, max(0, ix));
					y = min(h - 1, max(0, iy));
					dsq = (ix - j)*(ix - j) + (iy - i)*(iy - i);
					wght = exp(-dsq / (2 * r*r)) / (3.14 * 2 * r*r);
					val += bmp[y*w + x] * wght;  wsum += wght;
				}
			}
			bmw[i*w + j] = round(val / wsum);
		}
	}	
}

void print_file(FILE* file, headers* hdrs, unsigned char* bmp, long width_with_alignment)
{
	fwrite(hdrs, sizeof(*hdrs), 1, file);
	fseek(file, hdrs->arr_pointer, 0);
	fwrite(bmp, hdrs->img_height*width_with_alignment*sizeof(unsigned char), 1, file);
	fclose(file);
}

int main(int argc, char* argv[])
{
	int i;
	unsigned char alignment;
	unsigned long width_with_alignment;
	unsigned char* bmp, *bmw;
	unsigned char* buff;
	headers hdrs;
	FILE* file, *file_anna;

	if (argc != 2)
		return 1;
	file = open_file(argv[1]);

	fread_s(&hdrs, sizeof(headers), sizeof(headers), 1, file);
	if (hdrs.signature[0] != 'B' || hdrs.signature[1] != 'M')//проверка заголовка
	{
		fclose(file);
		return 2;
	}
	if (hdrs.bpp != 24)
	{
		fclose(file);
		return 3;
	}
	alignment = 3 * hdrs.img_width % 4 == 0 ? 0 : (4 - 3 * hdrs.img_width % 4);
	width_with_alignment = 3 * hdrs.img_width + alignment;

	buff = (unsigned char*)malloc(width_with_alignment*sizeof(unsigned char));
	bmp = (unsigned char*)malloc(hdrs.img_height*width_with_alignment*sizeof(unsigned char));
	bmw = (unsigned char*)malloc(hdrs.img_height*width_with_alignment*sizeof(unsigned char));

	fread_s(bmp, hdrs.img_height*width_with_alignment*sizeof(unsigned char), hdrs.img_height*width_with_alignment*sizeof(unsigned char), 1, file);
	gauss(bmp, bmw, hdrs.img_height, width_with_alignment);
	fopen_s(&file_anna, "C:\\output_anna.bmp", "wb");
	
	print_file(file_anna, &hdrs, bmw, width_with_alignment);

	
	free(bmp);
	free(buff);
	fclose(file);
	fclose(file_anna);
	
	
	getchar();


	return 0;
}