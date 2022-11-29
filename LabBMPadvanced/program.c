#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "readBMP.h"


#define  FILE_OPEN_ERROR -1
#define  BMP_DATA_ERROR -2
#define  NEW_BMP_CREATION_ERROR -3
#define  OPENING_NOTBMP_ERROR -4
#define  BLACKWHITE_FILE_NAME "BW_version.bmp"
typedef unsigned int int32;
typedef unsigned short int int16;
typedef unsigned char byte;
typedef struct BMP_HEADER {
	char name[2];
	int32 size;
	int32 aboba;
	int32 image_offset;
} bmp_header;

typedef struct DIB_header {
	int32 header_size;
	int32 width;
	int32 height;
	int16 corolplanes;
	int16 bitperpixel;
	int32 compression;
	int32 imagesize;
	/*int32 pixelpermeterX;
	int32 pixelpermeterY;
	int32 colorused;
	int32 importantcolor;*/
	int32 temp[4];
} dib_header;

typedef struct RGB {
	byte blue;
	byte green;
	byte red;
	//byte alpha;
} rgb;

typedef struct IMAGE {
	int height;
	int width;
	rgb  **rgb;

}Image;

Image readImage(FILE* fp, int height, int width) {

	Image pic;
	pic.rgb = (rgb**)malloc(height * sizeof(void*));
	pic.height = height;
	pic.width = width;
	int bytestoread = ((24 * pic.width + 31) / 32)*4;
	int numOfrgb = bytestoread / sizeof(rgb) + 1;

	for (int i = height - 1; i >= 0; i--) {
		pic.rgb[i] = (rgb*)malloc(numOfrgb * sizeof(rgb));
		fread(pic.rgb[i], 1, bytestoread, fp);
	}

	return pic;

}

void freeImage(Image pic) {
	for (int i = pic.height - 1; i >= 0; i--) {
		free(pic.rgb[i]);
	}
	free(pic.rgb);

}

byte toBlackWhite(rgb rgb) {
	return ( (0.3f*rgb.red) + (0.6f*rgb.blue) + (0.1f*rgb.green)) ;
}

void RGBImageToBlackWhite(Image pic) {
	byte newcolor;
	for (int i = pic.height - 1; i >= 0; i--) {
		for (int j = pic.width; j >=0; j--) {
			newcolor = toBlackWhite(pic.rgb[i][j]);
			pic.rgb[i][j].red = newcolor;
			pic.rgb[i][j].green = newcolor;
			pic.rgb[i][j].blue = newcolor;
		}
	}
}

int createBlackWhiteBMP(bmp_header header, dib_header dibheader, Image pic) {

	FILE* fpnew = fopen(BLACKWHITE_FILE_NAME, "w");
	if (fpnew == NULL) return -3;


	RGBImageToBlackWhite(pic);
	
	fwrite(header.name, 2, 1, fpnew);
	fwrite(&header.size, 3 * sizeof(int), 1,fpnew);
	fwrite(&dibheader, sizeof(dib_header), 1, fpnew);

	int bytestoread = ((24 * pic.width + 31) / 32) * 4;
	
	for (int i = pic.height-1; i >= 0; i--) {
		fwrite(pic.rgb[i],bytestoread,1, fpnew);
	}

	
	fclose(fpnew);
	return 0;
}

void imageToText(Image pic) {

	byte color;
	//0-31,32-63,64-95, ...
	char textpixels[] = {'@', '#', '%', 'O', 'a', '-','.',' '};
	for (int i = 0; i < pic.height; i++) {
		for (int j = 0; j < pic.width; j++) {
			color = toBlackWhite(pic.rgb[i][j]);
			printf("%c", textpixels[7 - color / 32]);
		}
		printf("\n");
	}
}

int openbmpfile(char *filename) {
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL) return FILE_OPEN_ERROR;

	bmp_header header;
	dib_header dibheader;
	
	fread(header.name, 2, 1, fp);
	fread(&header.size, 3 * sizeof(int), 1, fp);

	if ((header.name[0] != 'B') || (header.name[1] != 'M')) {
		fclose(fp);
		return OPENING_NOTBMP_ERROR;
	}

	fread(&dibheader, sizeof(dib_header), 1, fp); 

	if ( (dibheader.bitperpixel != 24)) { 
		printf("%d\n",dibheader.header_size); //header-size norma;=40
		printf("%d\n", dibheader.compression);// normal = 0
		printf("%d\n", dibheader.bitperpixel);
		fclose(fp);
		return BMP_DATA_ERROR;
	}
	printf("Header size: %d\n", dibheader.header_size);
	printf("Width: %d\n", dibheader.width);
	printf("Height: %d\n", dibheader.height);
	printf("CorolPanel: %d\n", dibheader.corolplanes);
	printf("BPP: %d\n", dibheader.bitperpixel);
	printf("Compression: %d\n", dibheader.compression);
	printf("Image size: %d\n", dibheader.imagesize);

	fseek(fp, header.image_offset, SEEK_SET);
	Image image = readImage(fp, dibheader.height, dibheader.width);
	if (createBlackWhiteBMP(header, dibheader, image) == -3)
	{
		fclose(fp);
		freeImage(image);
		remove(BLACKWHITE_FILE_NAME);
		return NEW_BMP_CREATION_ERROR;
	}

	imageToText(image);

	fclose(fp);
	freeImage(image);
	remove(BLACKWHITE_FILE_NAME);
	return 0;
}



int main(void) {

	int getState = openbmpfile("labphoto.bmp");
	switch (getState) {
	case 0:
		return 0;
	case FILE_OPEN_ERROR:
		printf("Exception: File creation error");
		break;
	case BMP_DATA_ERROR:
		printf("Exception: Incorrect image data (T_T) \nTry to check the compression and bpp of your picture");
		
		break;
	case NEW_BMP_CREATION_ERROR:
		printf("Exception: Creation BlackWhite version");
		break;
	case OPENING_NOTBMP_ERROR:
		printf("Exception: Opened file is not BMP");
		break;
	default:
		printf("Unexpected error");
	}

	return 0;
}