#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "extmem.h"


int build_data(Buffer *p_buf)
{
	unsigned int *blk;
	unsigned int disk_addr = 0;
	int i = 0;
	int buf_index = 0;

	printf("insert data\n");
	srand(time(NULL));
	printf("R\n");
	blk = (unsigned int*)getNewBlockInBuffer(p_buf);
	buf_index = 0;
	for (i = 0; i < 112 * 2; i++)
	{
		if (!(i % 2))
		{
			*(blk + buf_index) = rand() % 40 + 1;
			printf("(%d,", *(blk + buf_index));
		}
		else
		{
			*(blk + buf_index) = rand() % 1000 + 1;
			printf("%d)", *(blk + buf_index));
		}
		buf_index++;
		if (buf_index >= 7 * 2)
		{
			if (i == 112 * 2 - 1)
			{
				*(blk + 15) = -1;
				disk_addr++;
			}
			else
			    *(blk + 15) = ++disk_addr;
			printf("next block %d\n", *(blk + 15));
			writeBlockToDisk((unsigned char *)blk, disk_addr - 1, p_buf);
			blk = (unsigned int*)getNewBlockInBuffer(p_buf);
			buf_index = 0;
		}
	}
	if (buf_index) 
	{
		printf("into");
		*(blk + 15) = -1;
		disk_addr++;
		writeBlockToDisk((unsigned char *)blk, disk_addr - 1, p_buf);
	}
	else
		freeBlockInBuffer((unsigned char *)blk, p_buf);
	printf("S\n");
	blk = (unsigned int*)getNewBlockInBuffer(p_buf);
	buf_index = 0;
	for (i = 0; i < 224 * 2; i++)
	{
		if (!(i % 2))
		{
			*(blk + buf_index) = rand() % 41 + 20;
			printf("(%d,", *(blk + buf_index));
		}
		else
		{
			*(blk + buf_index) = rand() % 1000 + 1;
			printf("%d)", *(blk + buf_index));
		}
		buf_index++;
		if (buf_index >= 7 * 2)
		{
			if (i == 224 * 2 - 1)
			{
				*(blk + 15) = -1;
				disk_addr++;
			}
			else
				*(blk + 15) = ++disk_addr;
			printf("next block %d\n", *(blk + 15));
			writeBlockToDisk((unsigned char *)blk, disk_addr - 1, p_buf);
			blk = (unsigned int*)getNewBlockInBuffer(p_buf);
			buf_index = 0;
		}
	}
	if (buf_index)
	{
		printf("into");
		*(blk + 15) = -1;
		disk_addr++;
		writeBlockToDisk((unsigned char *)blk, disk_addr - 1, p_buf);
	}
	else
		freeBlockInBuffer((unsigned char *)blk, p_buf);

	printf("end insert\n\n");

	return 0;
}
