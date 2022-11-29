#pragma once



///OFFSETS
#define DATA_OFFSET_OFFSET 0x000A
#define WIDTH_OFFSET  0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C

#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define NO_COMPRESION 0
#define MAX_NUMBER_OF_COLORS 0
#define ALL_COLORS_REQUIRED 0

typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte;

void ReadImage(const char* fileName, byte** pixels, int32* width, int32* height, int32* bytesPerPixel)
{
	FILE* imageFile = fopen(fileName, "rb");
	int32 dataOffset;
	int16 bitsPerPixel;
	fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET);
	fread(&dataOffset, 4, 1, imageFile);
	printf("DATA OFFSET:%d\n", dataOffset);
	fseek(imageFile, WIDTH_OFFSET, SEEK_SET);
	fread(width, 4, 1, imageFile);
	printf("WIDTH: %d\n", *width);
	fseek(imageFile, HEIGHT_OFFSET, SEEK_SET);
	fread(height, 4, 1, imageFile);
	printf("HEIGHT: %d\n", *height);
	fseek(imageFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
	fread(&bitsPerPixel, 2, 1, imageFile);
	*bytesPerPixel = ((int32)bitsPerPixel) / 8;

	int paddedRowSize = (int)(4 * ceil((float)(*width) / 4.0f)) * (*bytesPerPixel);
	int unpaddedRowSize = (*width) * (*bytesPerPixel);
	int totalSize = unpaddedRowSize * (*height);
	*pixels = (byte*)malloc(totalSize);
	int i = 0;
	byte* currentRowPointer = *pixels + ((*height - 1) * unpaddedRowSize);
	for (int i = 0; i < *height; i++) {
		fseek(imageFile, dataOffset + (i * paddedRowSize), SEEK_SET);
		fread(currentRowPointer, 1, unpaddedRowSize, imageFile);
		currentRowPointer -= unpaddedRowSize;
	}

	fclose(imageFile);

}
