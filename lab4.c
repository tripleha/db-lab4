/*
	balabala
*/

#include <stdlib.h>
#include <stdio.h>
#include "extmem.h"
#include "build_data.h"


void show_64b_block(unsigned int *blk)
{
	int i = 0;
	for (i = 0; i < 7; i++)
	{
		printf("(%d,%d)", *(blk + i * 2), *(blk + i * 2 + 1));
	}
	printf("next block %d\n", *(blk + 15));
}


void show_project_block(unsigned int *blk)
{
	int i = 0;
	for (i = 0; i < 15; i++)
	{
		printf("%d, ", *(blk + i));
	}
	printf("next block %d\n", *(blk + 15));
}


void show_join_block(unsigned int *blk)
{
	int i = 0;
	for (i = 0; i < 5; i++)
	{
		printf("%d:(%d, %d) ", *(blk + i * 3), *(blk + i * 3 + 1), *(blk + i * 3 + 2));
	}
	printf("next block %d\n", *(blk + 15));
}


void show_all_block(unsigned int begin_addr, Buffer *p_buf, void (*p_func)(unsigned int*))
{
	unsigned int *blk;
	unsigned int next_block = begin_addr;
	while (next_block != -1)
	{
		blk = (unsigned int*)readBlockFromDisk(next_block, p_buf);
		if (blk == NULL)
		{
			printf("no data file found!\n");
			break;
		}
		p_func(blk);
		next_block = *(blk + 15);
		freeBlockInBuffer((unsigned char *)blk, p_buf);
	}
}


void sort_e_block(unsigned int *blks[8])
{
	int i = 0;
	int j = 0;
	unsigned int *t_blk;
	unsigned int *n_blk;
	int t_index;
	int n_index;
	int tmp;
	int blk_count = 0;

	for (i = 0; i < 8; i++)
		if (blks[i])
			blk_count++;

	for (i = 0; i < 7 * blk_count; i++)
	{
		for (j = 1; j < 7 * blk_count - i; j++)
		{
			t_blk = blks[(j - 1) / 7];
			n_blk = blks[j / 7];
			t_index = (j - 1) % 7;
			n_index = j % 7;
			if (*(t_blk + t_index * 2) > *(n_blk + n_index * 2))
			{
				tmp = *(n_blk + n_index * 2);
				*(n_blk + n_index * 2) = *(t_blk + t_index * 2);
				*(t_blk + t_index * 2) = tmp;
				tmp = *(n_blk + n_index * 2 + 1);
				*(n_blk + n_index * 2 + 1) = *(t_blk + t_index * 2 + 1);
				*(t_blk + t_index * 2 + 1) = tmp;
			}
			else if (*(t_blk + t_index * 2) == *(n_blk + n_index * 2) && *(t_blk + t_index * 2 + 1) > *(n_blk + n_index * 2 + 1))
			{
				tmp = *(n_blk + n_index * 2);
				*(n_blk + n_index * 2) = *(t_blk + t_index * 2);
				*(t_blk + t_index * 2) = tmp;
				tmp = *(n_blk + n_index * 2 + 1);
				*(n_blk + n_index * 2 + 1) = *(t_blk + t_index * 2 + 1);
				*(t_blk + t_index * 2 + 1) = tmp;
			}
		}
	}
}


void merge_sort(unsigned int base_addr, unsigned int join_addr, Buffer *p_buf)
{
	unsigned int *base_blk;
	unsigned int *join_blk;
	unsigned int *using_blk;
	unsigned int t_base = base_addr;
	unsigned int t_join = join_addr;
	int join_count = 0;
	int base_index = 0;
	int join_index = 0;
	int using_index = 0;
	unsigned int tmp_base = base_addr + 500;

	using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
	base_blk = (unsigned int*)readBlockFromDisk(t_base, p_buf);
	//printf("create using_blk ");
	join_blk = (unsigned int*)readBlockFromDisk(t_join, p_buf);
	//printf("=create using_blk ");
	while (t_join != -1 && join_count < 8 && t_base != join_addr && t_base != -1)
	{
		if (*(base_blk + base_index * 2) > *(join_blk + join_index * 2))
		{
			*(using_blk + using_index * 2) = *(join_blk + join_index * 2);
			*(using_blk + using_index * 2 + 1) = *(join_blk + join_index * 2 + 1);
			using_index++;
			join_index++;
		}
		else if (*(base_blk + base_index * 2) < *(join_blk + join_index * 2))
		{
			*(using_blk + using_index * 2) = *(base_blk + base_index * 2);
			*(using_blk + using_index * 2 + 1) = *(base_blk + base_index * 2 + 1);
			using_index++;
			base_index++;
		}
		else if (*(base_blk + base_index * 2 + 1) > *(join_blk + join_index * 2 + 1))
		{
			*(using_blk + using_index * 2) = *(join_blk + join_index * 2);
			*(using_blk + using_index * 2 + 1) = *(join_blk + join_index * 2 + 1);
			using_index++;
			join_index++;
		}
		else if (*(base_blk + base_index * 2 + 1) < *(join_blk + join_index * 2 + 1))
		{
			*(using_blk + using_index * 2) = *(base_blk + base_index * 2);
			*(using_blk + using_index * 2 + 1) = *(base_blk + base_index * 2 + 1);
			using_index++;
			base_index++;
		}
		else
		{
			*(using_blk + using_index * 2) = *(base_blk + base_index * 2);
			*(using_blk + using_index * 2 + 1) = *(base_blk + base_index * 2 + 1);
			using_index++;
			base_index++;
		}
		if (using_index >= 7)
		{
			*(using_blk + 15) = tmp_base + 1;
			writeBlockToDisk((unsigned char *)using_blk, tmp_base++, p_buf);
			using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
			using_index = 0;
		}
		if (base_index >= 7)
		{
			t_base = *(base_blk + 15);
			if (t_base != join_addr && t_base != -1)
			{
				freeBlockInBuffer((unsigned char *)base_blk, p_buf);
				//printf("free using_blk\n");
				base_blk = (unsigned int*)readBlockFromDisk(t_base, p_buf);
				//printf("create using_blk ");
				base_index = 0;
			}
		}
		if (join_index >= 7)
		{
			t_join = *(join_blk + 15);
			if (t_join != -1 && join_count < 8)
			{
				freeBlockInBuffer((unsigned char *)join_blk, p_buf);
				//printf("=free using_blk\n");
				join_blk = (unsigned int*)readBlockFromDisk(t_join, p_buf);
				//printf("=create using_blk ");
				join_index = 0;
				join_count++;
			}
		}
	}
	if (t_join != -1 && join_count < 8)
	{
		freeBlockInBuffer((unsigned char *)base_blk, p_buf);
		//printf("free using_blk\n");
		t_join = *(join_blk + 15);
		if (using_index)
		{
			while (using_index < 7)
			{
				*(using_blk + using_index * 2) = *(join_blk + join_index * 2);
				*(using_blk + using_index * 2 + 1) = *(join_blk + join_index * 2 + 1);
				using_index++;
				join_index++;
			}
			if (t_join != -1 && join_count < 8)
				*(using_blk + 15) = tmp_base + 1;
			else
				*(using_blk + 15) = -1;
			freeBlockInBuffer((unsigned char *)join_blk, p_buf);
			//printf("=free using_blk\n");
			writeBlockToDisk((unsigned char *)using_blk, tmp_base++, p_buf);
			join_count++;
		}
		else
		{
			freeBlockInBuffer((unsigned char *)using_blk, p_buf);
		}
		while (t_join != -1 && join_count < 8)
		{
			join_blk = (unsigned int*)readBlockFromDisk(t_join, p_buf);
			//printf("=create using_blk ");
			t_join = *(join_blk + 15);
			if (t_join != -1)
				*(join_blk + 15) = tmp_base + 1;
			writeBlockToDisk((unsigned char *)join_blk, tmp_base++, p_buf);
			//printf("=free using_blk\n");
			join_count++;
		}
	}
	else if (t_base != join_addr && t_base != -1)
	{
		freeBlockInBuffer((unsigned char *)join_blk, p_buf);
		//printf("=free using_blk\n");
		t_base = *(base_blk + 15);
		if (using_index)
		{
			while (using_index < 7)
			{
				*(using_blk + using_index * 2) = *(base_blk + base_index * 2);
				*(using_blk + using_index * 2 + 1) = *(base_blk + base_index * 2 + 1);
				using_index++;
				base_index++;
			}
			if (t_base != join_addr && t_base != -1)
				*(using_blk + 15) = tmp_base + 1;
			else
				*(using_blk + 15) = -1;
			freeBlockInBuffer((unsigned char *)base_blk, p_buf);
			//printf("free using_blk\n");
			writeBlockToDisk((unsigned char *)using_blk, tmp_base++, p_buf);
		}
		else
		{
			freeBlockInBuffer((unsigned char *)using_blk, p_buf);
		}
		while (t_base != join_addr && t_base != -1)
		{
			base_blk = (unsigned int*)readBlockFromDisk(t_base, p_buf);
			//printf("create using_blk ");
			t_base = *(base_blk + 15);
			if (t_base != join_addr)
				*(base_blk + 15) = tmp_base + 1;
			else
				*(base_blk + 15) = -1;
			writeBlockToDisk((unsigned char *)base_blk, tmp_base++, p_buf);
			//printf("free using_blk\n");
		}
	}
	printf("write back\n");
	tmp_base = base_addr + 500;
	t_base = base_addr;
	while (tmp_base != -1)
	{
		base_blk = (unsigned int*)readBlockFromDisk(tmp_base, p_buf);
		//printf("create using_blk ");
		dropBlockOnDisk(tmp_base);
		tmp_base = *(base_blk + 15);
		if (tmp_base != -1)
			*(base_blk + 15) = t_base + 1;
		else
			*(base_blk + 15) = -1;
		writeBlockToDisk((unsigned char *)base_blk, t_base++, p_buf);
		//printf("free using_blk\n");
	}
}


void sort_all(unsigned int begin_addr, Buffer *p_buf)
{
	unsigned int *blk;
	unsigned int using_addr = 500 + begin_addr;
	unsigned int next_block = begin_addr;
	unsigned int base_begin_block = 500 + begin_addr;
	unsigned int tmp_begin_block = -1;
	int i = 0;
	int has_base = 0;

	unsigned int* sort_blk[8] = { NULL };

	next_block = begin_addr;
	i = 0;
	while (next_block != -1)
	{
		blk = (unsigned int*)readBlockFromDisk(next_block, p_buf);
		sort_blk[i] = blk;
		next_block = *(blk + 15);
		i++;
		if (i >= 8)
		{
			sort_e_block(sort_blk);
			if (has_base)
			{
				tmp_begin_block = using_addr;
			}
			for (i = 0; i < 8; i++)
			{
				blk = sort_blk[i];
				if (i == 7)
					*(blk + 15) = -1;
				else
					*(blk + 15) = using_addr + 1;
				writeBlockToDisk((unsigned char *)blk, using_addr++, p_buf);
				sort_blk[i] = NULL;
			}
			i = 0;
			if (!has_base)
				has_base = 1;
			else
			{
				merge_sort(base_begin_block, tmp_begin_block, p_buf);
			}
		}
	}
	if (i)
	{
		sort_e_block(sort_blk);
		if (has_base)
		{
			tmp_begin_block = using_addr;
		}
		for (i = 0; i < 8; i++)
		{
			blk = sort_blk[i];
			if (blk)
			{
				if (i == 7 || sort_blk[i + 1] == NULL)
					*(blk + 15) = -1;
				else
					*(blk + 15) = using_addr + 1;
				writeBlockToDisk((unsigned char *)blk, using_addr++, p_buf);
			}
		}
		if (has_base)
		{
			merge_sort(base_begin_block, tmp_begin_block, p_buf);
		}
	}
}


void linear_search(unsigned int begin_addr, int search_key, Buffer *p_buf)
{
	unsigned int *blk;
	unsigned int base_addr = begin_addr;
	int i = 0;
	int found_count = 0;

	printf("begin linear search\n");
	do {
		blk = (unsigned int*)readBlockFromDisk(base_addr, p_buf);
		base_addr = *(blk + 15);
		for (i = 0; i < 7; i++)
		{
			if (*(blk + i * 2) == search_key)
			{
				printf("(%d,%d) ", *(blk + i * 2), *(blk + i * 2 + 1));
				found_count++;
			}
		}
		freeBlockInBuffer((unsigned char*)blk, p_buf);
	} while (base_addr != -1);
	printf("total = %d\n", found_count);
	printf("end linear search\n\n");
}


void div_search(unsigned int begin_addr, int search_key, Buffer *p_buf)
{
	unsigned int *blk;
	unsigned int base_addr = begin_addr;
	int i = 0;
	int found_count = 0;

	int left = 0;
	int right = 6;
	int mid = 0;
	int is_find = 0;

	printf("begin div search\n");
	do {
		blk = (unsigned int*)readBlockFromDisk(base_addr, p_buf);
		base_addr = *(blk + 15);
		if (*(blk + 12) > search_key)
		{
			// end of block
			left = 0;
			right = 6;
			while (right != left)
			{
				mid = (right + left) / 2;
				if (*(blk + mid * 2) < search_key)
				{
					left = mid + 1;
				}
				else if (*(blk + mid * 2) > search_key)
				{
					right = mid - 1;
				}
				else
				{
					is_find = 1;
					right = mid;
					break;
				}
			}
			if (*(blk + right * 2) == search_key)
			{
				is_find = 1;
			}
			if (is_find)
			{
				while (*(blk + right * 2) == search_key)
				{
					right--;
				}
				right++;
				while (*(blk + right * 2) == search_key)
				{
					printf("(%d,%d) ", *(blk + right * 2), *(blk + right * 2 + 1));
					found_count++;
					right++;
				}
			}
			break;
		}
		else if (*(blk + 12) == search_key)
		{
			i = 6;
			while (*(blk + i * 2) == search_key)
			{
				i--;
			}
			i++;
			while (*(blk + i * 2) == search_key)
			{
				printf("(%d,%d) ", *(blk + i * 2), *(blk + i * 2 + 1));
				found_count++;
				i++;
			}
		}
		freeBlockInBuffer((unsigned char*)blk, p_buf);
	} while (base_addr != -1);
	printf("total = %d\n", found_count);
	printf("end div search\n\n");
}


void build_sort_index(unsigned int begin_addr, Buffer *p_buf)
{
	unsigned int *base_blk;
	unsigned int *using_blk;
	unsigned int base_addr = begin_addr;
	unsigned int using_addr = 10000 + (begin_addr - 500);
	int last_key = -1;
	int i = 0;
	int using_index = 0;

	using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
	do {
		base_blk = (unsigned int*)readBlockFromDisk(base_addr, p_buf);
		for (i = 0; i < 7; i++)
		{
			if (*(base_blk + i * 2) != last_key)
			{
				*(using_blk + using_index++) = *(base_blk + i * 2);
				*(using_blk + using_index++) = base_addr;
				*(using_blk + using_index++) = i;
				last_key = *(base_blk + i * 2);
				if (using_index > 14)
				{
					*(using_blk + 15) = using_addr + 1;
					writeBlockToDisk((unsigned char*)using_blk, using_addr++, p_buf);
					using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
					using_index = 0;
				}
			}
		}
		base_addr = *(base_blk + 15);
		freeBlockInBuffer((unsigned char*)base_blk, p_buf);
	} while (base_addr != -1);
	if (using_index)
	{
		for (i = using_index; i < 15; i++)
			*(using_blk + i) = -1;
		*(using_blk + 15) = -1;
		writeBlockToDisk((unsigned char*)using_blk, using_addr, p_buf);
	}
	else
	{
		freeBlockInBuffer((unsigned char*)using_blk, p_buf);
		using_blk = (unsigned int*)readBlockFromDisk(using_addr - 1, p_buf);
		*(using_blk + 15) = -1;
		writeBlockToDisk((unsigned char*)using_blk, using_addr - 1, p_buf);
	}
}


void index_search(unsigned int begin_addr, int search_key, Buffer *p_buf)
{
	unsigned int *blk;
	unsigned int *using_blk;
	unsigned int base_addr = begin_addr;
	unsigned int using_addr = -1;
	int i = 0;
	int j = 0;
	int end_found = 0;
	int using_index = 0;
	int found_count = 0;

	printf("begin index search\n");
	do {
		blk = (unsigned int*)readBlockFromDisk(base_addr, p_buf);
		base_addr = *(blk + 15);
		for (i = 0; i < 5; i++)
		{
			if (*(blk + i * 3) == search_key)
			{
				using_addr = *(blk + i * 3 + 1);
				using_index = *(blk + i * 3 + 2);
				do{
					using_blk = (unsigned int*)readBlockFromDisk(using_addr, p_buf);
					using_addr = *(using_blk + 15);
					for (j = using_index; j < 7; j++)
					{
						if (*(using_blk + j * 2) != search_key)
						{
							end_found = 1;
							break;
						}
						printf("(%d,%d) ", *(using_blk + j * 2), *(using_blk + j * 2 + 1));
						found_count++;
					}
					freeBlockInBuffer((unsigned char*)using_blk, p_buf);
					if (end_found)
						break;
					using_index = 0;
				} while (using_addr != -1);
				end_found = 1;
				break;
			}
		}
		freeBlockInBuffer((unsigned char*)blk, p_buf);
		if (end_found)
			break;
	} while (base_addr != -1);
	printf("total = %d\n", found_count);
	printf("end index search\n\n");
}


void make_project(unsigned int begin_addr, unsigned int save_addr, Buffer *p_buf)
{
	unsigned int *base_blk;
	unsigned int *using_blk;
	unsigned int base_addr = begin_addr;
	unsigned int using_addr = save_addr;
	int project_index = 0;
	int i = 0;

	using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
	do {
		base_blk = (unsigned int*)readBlockFromDisk(base_addr, p_buf);
		base_addr = *(base_blk + 15);
		for (i = 0; i < 7; i++)
		{
			*(using_blk + project_index++) = *(base_blk + i * 2);
			if (project_index > 14)
			{
				if (base_addr == -1 && i >= 6)
					*(using_blk + 15) = -1;
				else
					*(using_blk + 15) = using_addr + 1;
				writeBlockToDisk((unsigned char*)using_blk, using_addr++, p_buf);
				using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
				project_index = 0;
			}
		}
		freeBlockInBuffer((unsigned char*)base_blk, p_buf);
	} while (base_addr != -1);
	if (project_index)
	{
		for (i = project_index; i < 15; i++)
			*(using_blk + i) = -1;
		*(using_blk + 15) = -1;
		writeBlockToDisk((unsigned char*)using_blk, using_addr, p_buf);
	}
	else
		freeBlockInBuffer((unsigned char*)using_blk, p_buf);
}


void nest_loop_join(unsigned int p_begin_addr, unsigned int s_begin_addr, Buffer *p_buf)
{
	unsigned int *p_blk;
	unsigned int *s_blk;
	unsigned int *using_blk;
	unsigned int p_addr = p_begin_addr;
	unsigned int s_addr = s_begin_addr;
	unsigned int using_addr = 2000;
	int using_index = 0;
	int i = 0;
	int j = 0;

	using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
	do {
		p_blk = (unsigned int*)readBlockFromDisk(p_addr, p_buf);
		p_addr = *(p_blk + 15);
		for (i = 0; i < 7; i++)
		{
			do {
				s_blk = (unsigned int*)readBlockFromDisk(s_addr, p_buf);
				s_addr = *(s_blk + 15);
				for (j = 0; j < 7; j++)
				{
					if (*(p_blk + i * 2) == *(s_blk + j * 2))
					{
						*(using_blk + using_index++) = *(p_blk + i * 2);
						*(using_blk + using_index++) = *(p_blk + i * 2 + 1);
						*(using_blk + using_index++) = *(s_blk + j * 2 + 1);
						if (using_index >= 14)
						{
							*(using_blk + 15) = using_addr + 1;
							writeBlockToDisk((unsigned char*)using_blk, using_addr++, p_buf);
							using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
							using_index = 0;
						}
					}
				}
				freeBlockInBuffer((unsigned char*)s_blk, p_buf);
			} while (s_addr != -1);
			s_addr = s_begin_addr;
		}
		freeBlockInBuffer((unsigned char*)p_blk, p_buf);
	} while (p_addr != -1);
	if (using_index)
	{
		for (i = using_index; i < 15; i++)
			*(using_blk + i) = -1;
		*(using_blk + 15) = -1;
		writeBlockToDisk((unsigned char*)using_blk, using_addr, p_buf);
	}
	else
	{
		freeBlockInBuffer((unsigned char*)using_blk, p_buf);
		using_blk = (unsigned int*)readBlockFromDisk(using_addr - 1, p_buf);
		*(using_blk + 15) = -1;
		writeBlockToDisk((unsigned char*)using_blk, using_addr - 1, p_buf);
	}
}


void sort_merge_join(unsigned int p_begin_addr, unsigned int s_begin_addr, Buffer *p_buf)
{
	unsigned int *p_blk;
	unsigned int *s_blk;
	unsigned int *using_blk;
	unsigned int p_addr = p_begin_addr;
	unsigned int s_addr = s_begin_addr;
	unsigned int s_eq_addr = s_begin_addr;
	unsigned int using_addr = 2500;
	int using_index = 0;
	int s_eq_index = 0;
	int check_a = 0;
	int last_check_a = -1;
	int i = 0;
	int j = 0;
	int j_begin = 0;
	int check_begin = 0;
	int end_loop = 0;

	using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
	s_blk = (unsigned int*)readBlockFromDisk(s_addr, p_buf);
	do {
		p_blk = (unsigned int*)readBlockFromDisk(p_addr, p_buf);
		p_addr = *(p_blk + 15);
		for (i = 0; i < 7; i++)
		{
			check_a = *(p_blk + i * 2);
			if (check_begin && check_a == last_check_a)
			{
				//printf("reset s to %d begin\n", check_a);
				freeBlockInBuffer((unsigned char*)s_blk, p_buf);
				s_addr = s_eq_addr;
				j_begin = s_eq_index;
				s_blk = (unsigned int*)readBlockFromDisk(s_addr, p_buf);
			}
			do {
				for (j = j_begin; j < 7; j++)
				{
					if (check_a == *(s_blk + j * 2))
					{
						if (!check_begin){
							//printf("begin check in %d \n", check_a);
							check_begin = 1;
							last_check_a = check_a;
							s_eq_addr = s_addr;
							s_eq_index = j;
						}
						//printf("into %d\n", check_a);
						*(using_blk + using_index++) = check_a;
						*(using_blk + using_index++) = *(p_blk + i * 2 + 1);
						*(using_blk + using_index++) = *(s_blk + j * 2 + 1);
						if (using_index >= 14)
						{
							*(using_blk + 15) = using_addr + 1;
							writeBlockToDisk((unsigned char*)using_blk, using_addr++, p_buf);
							using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
							using_index = 0;
						}
					}
					else if (check_a < *(s_blk + j * 2))
					{
						//printf("end loop of r %d < s %d\n", check_a, *(s_blk + j * 2));
						end_loop = 1;
						break;
					}
					else
					{
						//printf("end check of r %d > s %d\n", check_a, *(s_blk + j * 2));
						check_begin = 0;
					}
				}
				j_begin = 0;
				if (end_loop)
				{
					end_loop = 0;
					break;
				}
				else
				{
					s_addr = *(s_blk + 15);
					freeBlockInBuffer((unsigned char*)s_blk, p_buf);
					if (s_addr != -1)
						s_blk = (unsigned int*)readBlockFromDisk(s_addr, p_buf);
				}
			} while (s_addr != -1);
			if (!check_begin && s_addr == -1 && j >= 7 && check_a > *(s_blk + 12))
			{
				freeBlockInBuffer((unsigned char*)s_blk, p_buf);
				p_addr = -1;
			}
		}
		freeBlockInBuffer((unsigned char*)p_blk, p_buf);
	} while (p_addr != -1);
	if (using_index)
	{
		for (i = using_index; i < 15; i++)
			*(using_blk + i) = -1;
		*(using_blk + 15) = -1;
		writeBlockToDisk((unsigned char*)using_blk, using_addr, p_buf);
	}
	else
	{
		freeBlockInBuffer((unsigned char*)using_blk, p_buf);
		using_blk = (unsigned int*)readBlockFromDisk(using_addr - 1, p_buf);
		*(using_blk + 15) = -1;
		writeBlockToDisk((unsigned char*)using_blk, using_addr - 1, p_buf);
	}
}


void build_hash(unsigned int begin_addr, unsigned int save_addr, Buffer *p_buf)
{
	unsigned int *base_blk;
	unsigned int *using_blk;
	unsigned int base_addr = begin_addr;
	unsigned int using_addr[10];
	int using_index[10];
	int i = 0;
	int j = 0;
	int hash_t = -1;
	int last_hash = -1;

	for (i = 0; i < 10; i++)
	{
		using_addr[i] = save_addr + i;
		using_index[i] = 0;
	}

	hash_t = 0;
	using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
	for (j = 0; j < 16; j++)
	{
		*(using_blk + j) = -1;
	}
	last_hash = 0;
	do {
		base_blk = (unsigned int*)readBlockFromDisk(base_addr, p_buf);
		base_addr = *(base_blk + 15);
		for (i = 0; i < 7; i++)
		{
			hash_t = *(base_blk + i * 2) % 10;
			if (hash_t != last_hash)
			{
				writeBlockToDisk((unsigned char*)using_blk, using_addr[last_hash], p_buf);
				if (using_index[hash_t])
					using_blk = (unsigned int*)readBlockFromDisk(using_addr[hash_t], p_buf);
				else
				{
					using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
					for (j = 0; j < 16; j++)
					{
						*(using_blk + j) = -1;
					}
				}
				last_hash = hash_t;
			}
			*(using_blk + using_index[hash_t] * 2) = *(base_blk + i * 2);
			*(using_blk + using_index[hash_t] * 2 + 1) = *(base_blk + i * 2 + 1);
			using_index[hash_t]++;
			if (using_index[hash_t] >= 7)
			{
				*(using_blk + 15) = using_addr[hash_t] + 10;
				writeBlockToDisk((unsigned char*)using_blk, using_addr[hash_t], p_buf);
				using_addr[hash_t] += 10;
				using_index[hash_t] = 0;
				using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
				for (j = 0; j < 16; j++)
				{
					*(using_blk + j) = -1;
				}
			}
		}
		freeBlockInBuffer((unsigned char*)base_blk, p_buf);
	} while (base_addr != -1);
	writeBlockToDisk((unsigned char*)using_blk, using_addr[hash_t], p_buf);
	for (i = 0; i < 10; i++)
	{
		if (using_addr[i] != save_addr + i && using_index[i] == 0)
		{
			using_blk = (unsigned int*)readBlockFromDisk(using_addr[i] - 10, p_buf);
			*(using_blk + 15) = -1;
			writeBlockToDisk((unsigned char*)using_blk, using_addr[i] - 10, p_buf);
		}
	}
}


void hash_join(unsigned int p_begin_addr, unsigned int s_begin_addr, Buffer *p_buf)
{
	unsigned int *p_blk;
	unsigned int *s_blk;
	unsigned int *using_blk;
	unsigned int p_addr;
	unsigned int s_addr;
	unsigned int using_addr = 3000;
	int using_index = 0;
	int i = 0;
	int j = 0;
	int k = 0;

	using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
	for (i = 0; i < 10; i++)
	{
		p_addr = p_begin_addr + i;
		s_addr = s_begin_addr + i;

		while (p_addr != -1) 
		{
			p_blk = (unsigned int*)readBlockFromDisk(p_addr, p_buf);
			for (j = 0; j < 7; j++)
			{
				if (*(p_blk + j * 2) != -1)
				{
					printf("into p %d (%d, %d)\n", *(p_blk + j * 2), p_addr, j);
					while (s_addr != -1)
					{
						s_blk = (unsigned int*)readBlockFromDisk(s_addr, p_buf);
						for (k = 0; k < 7; k++)
						{
							if (*(s_blk + k * 2) != -1)
							{
								printf("s %d (%d, %d)\n", *(s_blk + k * 2), s_addr, k);
								if (*(p_blk + j * 2) == *(s_blk + k * 2))
								{
									printf("into p %d s %d\n", *(p_blk + j * 2), *(s_blk + k * 2));
									*(using_blk + using_index++) = *(p_blk + j * 2);
									*(using_blk + using_index++) = *(p_blk + j * 2 + 1);
									*(using_blk + using_index++) = *(s_blk + k * 2 + 1);
									if (using_index >= 14)
									{
										*(using_blk + 15) = using_addr + 1;
										writeBlockToDisk((unsigned char*)using_blk, using_addr++, p_buf);
										using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
										using_index = 0;
									}
								}
							}
							else
								break;
						}
						s_addr = *(s_blk + 15);
						freeBlockInBuffer((unsigned char*)s_blk, p_buf);
					}
					s_addr = s_begin_addr + i;
				}
				else
					break;
			}
			p_addr = *(p_blk + 15);
			freeBlockInBuffer((unsigned char*)p_blk, p_buf);
		}
	}
	if (using_index)
	{
		for (i = using_index; i < 15; i++)
			*(using_blk + i) = -1;
		*(using_blk + 15) = -1;
		writeBlockToDisk((unsigned char*)using_blk, using_addr, p_buf);
	}
	else
	{
		freeBlockInBuffer((unsigned char*)using_blk, p_buf);
		using_blk = (unsigned int*)readBlockFromDisk(using_addr - 1, p_buf);
		*(using_blk + 15) = -1;
		writeBlockToDisk((unsigned char*)using_blk, using_addr - 1, p_buf);
	}
}


void set_union(unsigned int p_begin_addr, unsigned int s_begin_addr, Buffer *p_buf)
{
	unsigned int *p_blk;
	unsigned int *s_blk;
	unsigned int *using_blk;
	unsigned int p_addr = p_begin_addr;
	unsigned int s_addr = s_begin_addr;
	unsigned int using_addr = 3500;
	int using_index = 0;
	int i = 0;
	int s_index = 0;
	int last_in_a = -1;
	int last_in_b = -1;

	using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
	s_blk = (unsigned int*)readBlockFromDisk(s_addr, p_buf);
	s_addr = *(s_blk + 15);
	p_blk = (unsigned int*)readBlockFromDisk(p_addr, p_buf);
	p_addr = *(p_blk + 15);
	while (!(p_addr == -1 && i >= 7))
	{
		if (s_addr == -1 && s_index >= 7)
		{
			if (last_in_a != *(p_blk + i * 2) || last_in_b != *(p_blk + i * 2 + 1))
			{
				*(using_blk + using_index++) = *(p_blk + i * 2);
				*(using_blk + using_index++) = *(p_blk + i * 2 + 1);
				last_in_a = *(p_blk + i * 2);
				last_in_b = *(p_blk + i * 2 + 1);
			}
			i++;
		}
		else 
		{
			if (*(p_blk + i * 2) == *(s_blk + s_index * 2))
			{
				if (*(p_blk + i * 2 + 1) == *(s_blk + s_index * 2 + 1))
				{
					if (last_in_a != *(p_blk + i * 2) || last_in_b != *(p_blk + i * 2 + 1))
					{
						*(using_blk + using_index++) = *(p_blk + i * 2);
						*(using_blk + using_index++) = *(p_blk + i * 2 + 1);
						last_in_a = *(p_blk + i * 2);
						last_in_b = *(p_blk + i * 2 + 1);
					}
					i++;
					s_index++;
				}
				else if (*(p_blk + i * 2 + 1) < *(s_blk + s_index * 2 + 1))
				{
					if (last_in_a != *(p_blk + i * 2) || last_in_b != *(p_blk + i * 2 + 1))
					{
						*(using_blk + using_index++) = *(p_blk + i * 2);
						*(using_blk + using_index++) = *(p_blk + i * 2 + 1);
						last_in_a = *(p_blk + i * 2);
						last_in_b = *(p_blk + i * 2 + 1);
					}
					i++;
				}
				else
				{
					if (last_in_a != *(s_blk + s_index * 2) || last_in_b != *(s_blk + s_index * 2 + 1))
					{
						*(using_blk + using_index++) = *(s_blk + s_index * 2);
						*(using_blk + using_index++) = *(s_blk + s_index * 2 + 1);
						last_in_a = *(s_blk + s_index * 2);
						last_in_b = *(s_blk + s_index * 2 + 1);
					}
					s_index++;
				}
			}
			else if (*(p_blk + i * 2) < *(s_blk + s_index * 2))
			{
				if (last_in_a != *(p_blk + i * 2) || last_in_b != *(p_blk + i * 2 + 1))
				{
					*(using_blk + using_index++) = *(p_blk + i * 2);
					*(using_blk + using_index++) = *(p_blk + i * 2 + 1);
					last_in_a = *(p_blk + i * 2);
					last_in_b = *(p_blk + i * 2 + 1);
				}
				i++;
			}
			else
			{
				if (last_in_a != *(s_blk + s_index * 2) || last_in_b != *(s_blk + s_index * 2 + 1))
				{
					*(using_blk + using_index++) = *(s_blk + s_index * 2);
					*(using_blk + using_index++) = *(s_blk + s_index * 2 + 1);
					last_in_a = *(s_blk + s_index * 2);
					last_in_b = *(s_blk + s_index * 2 + 1);
				}
				s_index++;
			}
			if (s_index >= 7)
			{
				freeBlockInBuffer((unsigned char*)s_blk, p_buf);
				if (s_addr != -1)
				{
					s_blk = (unsigned int*)readBlockFromDisk(s_addr, p_buf);
					s_addr = *(s_blk + 15);
					s_index = 0;
				}
			}
		}
		if (using_index >= 14)
		{
			*(using_blk + 15) = using_addr + 1;
			writeBlockToDisk((unsigned char*)using_blk, using_addr++, p_buf);
			using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
			using_index = 0;
		}
		if (i >= 7)
		{
			freeBlockInBuffer((unsigned char*)p_blk, p_buf);
			if (p_addr != -1)
			{
				p_blk = (unsigned int*)readBlockFromDisk(p_addr, p_buf);
				p_addr = *(p_blk + 15);
				i = 0;
			}
			else
				break;
		}
	}
	while (!(s_addr == -1 && s_index >= 7))
	{
		*(using_blk + using_index++) = *(s_blk + s_index * 2);
		*(using_blk + using_index++) = *(s_blk + s_index * 2 + 1);
		s_index++;
		if (s_index >= 7)
		{
			freeBlockInBuffer((unsigned char*)s_blk, p_buf);
			if (s_addr != -1)
			{
				s_blk = (unsigned int*)readBlockFromDisk(s_addr, p_buf);
				s_addr = *(s_blk + 15);
				s_index = 0;
			}
		}
		if (using_index >= 14)
		{
			*(using_blk + 15) = using_addr + 1;
			writeBlockToDisk((unsigned char*)using_blk, using_addr++, p_buf);
			using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
			using_index = 0;
		}
	}
	if (using_index)
	{
		for (i = using_index; i < 15; i++)
			*(using_blk + i) = -1;
		*(using_blk + 15) = -1;
		writeBlockToDisk((unsigned char*)using_blk, using_addr, p_buf);
	}
	else
	{
		freeBlockInBuffer((unsigned char*)using_blk, p_buf);
		using_blk = (unsigned int*)readBlockFromDisk(using_addr - 1, p_buf);
		*(using_blk + 15) = -1;
		writeBlockToDisk((unsigned char*)using_blk, using_addr - 1, p_buf);
	}
}


void set_inter(unsigned int p_begin_addr, unsigned int s_begin_addr, Buffer *p_buf)
{
	unsigned int *p_blk;
	unsigned int *s_blk;
	unsigned int *using_blk;
	unsigned int p_addr = p_begin_addr;
	unsigned int s_addr = s_begin_addr;
	unsigned int using_addr = 4000;
	int using_index = 0;
	int i = 0;
	int s_index = 0;
	int last_in_a = -1;
	int last_in_b = -1;

	using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
	s_blk = (unsigned int*)readBlockFromDisk(s_addr, p_buf);
	s_addr = *(s_blk + 15);
	p_blk = (unsigned int*)readBlockFromDisk(p_addr, p_buf);
	p_addr = *(p_blk + 15);
	while (!(p_addr == -1 && i >= 7))
	{
		if (s_addr == -1 && s_index >= 7)
		{
			freeBlockInBuffer((unsigned char*)p_blk, p_buf);
			break;
		}
		else
		{
			if (*(p_blk + i * 2) == *(s_blk + s_index * 2))
			{
				if (*(p_blk + i * 2 + 1) == *(s_blk + s_index * 2 + 1))
				{
					if (last_in_a != *(p_blk + i * 2) || last_in_b != *(p_blk + i * 2 + 1))
					{
						*(using_blk + using_index++) = *(p_blk + i * 2);
						*(using_blk + using_index++) = *(p_blk + i * 2 + 1);
						last_in_a = *(p_blk + i * 2);
						last_in_b = *(p_blk + i * 2 + 1);
					}
					i++;
					s_index++;
				}
				else if (*(p_blk + i * 2 + 1) < *(s_blk + s_index * 2 + 1))
				{
					i++;
				}
				else
				{
					s_index++;
				}
			}
			else if (*(p_blk + i * 2) < *(s_blk + s_index * 2))
			{
				i++;
			}
			else
			{
				s_index++;
			}
			if (s_index >= 7)
			{
				freeBlockInBuffer((unsigned char*)s_blk, p_buf);
				if (s_addr != -1)
				{
					s_blk = (unsigned int*)readBlockFromDisk(s_addr, p_buf);
					s_addr = *(s_blk + 15);
					s_index = 0;
				}
			}
			if (using_index >= 14)
			{
				*(using_blk + 15) = using_addr + 1;
				writeBlockToDisk((unsigned char*)using_blk, using_addr++, p_buf);
				using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
				using_index = 0;
			}
			if (i >= 7)
			{
				freeBlockInBuffer((unsigned char*)p_blk, p_buf);
				if (p_addr != -1)
				{
					p_blk = (unsigned int*)readBlockFromDisk(p_addr, p_buf);
					p_addr = *(p_blk + 15);
					i = 0;
				}
				else
				{
					if (!(s_addr == -1 && s_index >= 7))
						freeBlockInBuffer((unsigned char*)s_blk, p_buf);
					break;
				}
			}
		}
	}
	if (using_index)
	{
		for (i = using_index; i < 15; i++)
			*(using_blk + i) = -1;
		*(using_blk + 15) = -1;
		writeBlockToDisk((unsigned char*)using_blk, using_addr, p_buf);
	}
	else
	{
		freeBlockInBuffer((unsigned char*)using_blk, p_buf);
		if (using_addr != 4000)
		{
			using_blk = (unsigned int*)readBlockFromDisk(using_addr - 1, p_buf);
			*(using_blk + 15) = -1;
			writeBlockToDisk((unsigned char*)using_blk, using_addr - 1, p_buf);
		}
	}
}


void set_diff(unsigned int a_begin_addr, unsigned int b_begin_addr, Buffer *p_buf)
{
	unsigned int *a_blk;
	unsigned int *b_blk;
	unsigned int *using_blk;
	unsigned int a_addr = a_begin_addr;
	unsigned int b_addr = b_begin_addr;
	unsigned int using_addr = 4500;
	int using_index = 0;
	int a_index = 0;
	int b_index = 0;
	int i = 0;
	int last_in_1 = -1;
	int last_in_2 = -1;

	using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
	b_blk = (unsigned int*)readBlockFromDisk(b_addr, p_buf);
	b_addr = *(b_blk + 15);
	a_blk = (unsigned int*)readBlockFromDisk(a_addr, p_buf);
	a_addr = *(a_blk + 15);
	while (!(a_addr == -1 && a_index >= 7))
	{
		if (b_addr == -1 && b_index >= 7)
		{
			if (last_in_1 != *(a_blk + a_index * 2) || last_in_2 != *(a_blk + a_index * 2 + 1))
			{
				*(using_blk + using_index++) = *(a_blk + a_index * 2);
				*(using_blk + using_index++) = *(a_blk + a_index * 2 + 1);
				last_in_1 = *(a_blk + a_index * 2);
				last_in_2 = *(a_blk + a_index * 2 + 1);
			}
			a_index++;
		}
		else
		{
			if (*(a_blk + a_index * 2) == *(b_blk + b_index * 2))
			{
				if (*(a_blk + a_index * 2 + 1) == *(b_blk + b_index * 2 + 1))
				{
					last_in_1 = *(a_blk + a_index * 2);
					last_in_2 = *(a_blk + a_index * 2 + 1);
					a_index++;
					b_index++;
				}
				else if (*(a_blk + a_index * 2 + 1) < *(b_blk + b_index * 2 + 1))
				{
					if (last_in_1 != *(a_blk + a_index * 2) || last_in_2 != *(a_blk + a_index * 2 + 1))
					{
						*(using_blk + using_index++) = *(a_blk + a_index * 2);
						*(using_blk + using_index++) = *(a_blk + a_index * 2 + 1);
						last_in_1 = *(a_blk + a_index * 2);
						last_in_2 = *(a_blk + a_index * 2 + 1);
					}
					a_index++;
				}
				else
				{
					b_index++;
				}
			}
			else if (*(a_blk + a_index * 2) < *(b_blk + b_index * 2))
			{
				if (last_in_1 != *(a_blk + a_index * 2) || last_in_2 != *(a_blk + a_index * 2 + 1))
				{
					*(using_blk + using_index++) = *(a_blk + a_index * 2);
					*(using_blk + using_index++) = *(a_blk + a_index * 2 + 1);
					last_in_1 = *(a_blk + a_index * 2);
					last_in_2 = *(a_blk + a_index * 2 + 1);
				}
				a_index++;
			}
			else
			{
				b_index++;
			}
			if (b_index >= 7)
			{
				freeBlockInBuffer((unsigned char*)b_blk, p_buf);
				if (b_addr != -1)
				{
					b_blk = (unsigned int*)readBlockFromDisk(b_addr, p_buf);
					b_addr = *(b_blk + 15);
					b_index = 0;
				}
			}
		}
		if (using_index >= 14)
		{
			*(using_blk + 15) = using_addr + 1;
			writeBlockToDisk((unsigned char*)using_blk, using_addr++, p_buf);
			using_blk = (unsigned int*)getNewBlockInBuffer(p_buf);
			using_index = 0;
		}
		if (a_index >= 7)
		{
			freeBlockInBuffer((unsigned char*)a_blk, p_buf);
			if (a_addr != -1)
			{
				a_blk = (unsigned int*)readBlockFromDisk(a_addr, p_buf);
				a_addr = *(a_blk + 15);
				a_index = 0;
			}
			else
			{
				if (!(b_addr == -1 && b_index >= 7))
					freeBlockInBuffer((unsigned char*)b_blk, p_buf);
				break;
			}
		}
	}
	if (using_index)
	{
		for (i = using_index; i < 15; i++)
			*(using_blk + i) = -1;
		*(using_blk + 15) = -1;
		writeBlockToDisk((unsigned char*)using_blk, using_addr, p_buf);
	}
	else
	{
		freeBlockInBuffer((unsigned char*)using_blk, p_buf);
		if (using_addr != 4500)
		{
			using_blk = (unsigned int*)readBlockFromDisk(using_addr - 1, p_buf);
			*(using_blk + 15) = -1;
			writeBlockToDisk((unsigned char*)using_blk, using_addr - 1, p_buf);
		}
	}
}


int main()
{
	Buffer buf;
	unsigned int p_begin_addr = 0;
	unsigned int s_begin_addr = 16;
	unsigned int sort_base = 500;
	unsigned int project_base = 1500;
	unsigned int index_base = 10000;
	unsigned int nest_join_base = 2000;
	unsigned int sort_merge_base = 2500;
	unsigned int hash_join_base = 3000;
	unsigned int union_base = 3500;
	unsigned int inter_base = 4000;
	unsigned int diff_base = 4500;
	unsigned int p_hash_base = 20000;
	unsigned int s_hash_base = 30000;

	int i = 0;

	initBuffer(520, 64, &buf);

	build_data(&buf);

	printf("sort r\n");
	sort_all(p_begin_addr, &buf);

	show_all_block(p_begin_addr, &buf, show_64b_block);
	printf("r sorted\n");
	show_all_block(sort_base + p_begin_addr, &buf, show_64b_block);

	printf("sort s\n");
	sort_all(s_begin_addr, &buf);

	show_all_block(s_begin_addr, &buf, show_64b_block);
	printf("s sorted\n");
	show_all_block(sort_base + s_begin_addr, &buf, show_64b_block);

	linear_search(p_begin_addr, 1, &buf);
	linear_search(s_begin_addr, 46, &buf);

	div_search(sort_base + p_begin_addr, 1, &buf);
	div_search(sort_base + s_begin_addr, 46, &buf);

	printf("build sort index\n");
	build_sort_index(sort_base + p_begin_addr, &buf);
	build_sort_index(sort_base + s_begin_addr, &buf);

	printf("P sort index\n");
	show_all_block(index_base + p_begin_addr, &buf, show_join_block);
	printf("S sort index\n");
	show_all_block(index_base + s_begin_addr, &buf, show_join_block);

	index_search(index_base + p_begin_addr, 1, &buf);
	index_search(index_base + s_begin_addr, 46, &buf);

	printf("project P\n");
	make_project(p_begin_addr, project_base, &buf);

	show_all_block(project_base, &buf, show_project_block);

	printf("project S\n");
	make_project(s_begin_addr, project_base + 100, &buf);

	show_all_block(project_base + 100, &buf, show_project_block);

	printf("nest_loop_join begin\n");

	nest_loop_join(p_begin_addr + sort_base, s_begin_addr + sort_base, &buf);

	printf("sort_merge_join begin\n");

	sort_merge_join(p_begin_addr + sort_base, s_begin_addr + sort_base, &buf);

	printf("build P hash\n");
	build_hash(p_begin_addr, p_hash_base, &buf);
	for (i = 0; i < 10; i++)
	{
		show_all_block(p_hash_base + i, &buf, show_64b_block);
	}
	printf("build S hash\n");
	build_hash(s_begin_addr, s_hash_base, &buf);
	for (i = 0; i < 10; i++)
	{
		show_all_block(s_hash_base + i, &buf, show_64b_block);
	}

	printf("hash_join begin\n");
	hash_join(p_hash_base, s_hash_base, &buf);

	printf("nest_loop_join result\n");
	show_all_block(nest_join_base, &buf, show_join_block);
	printf("sort_merge_join result\n");
	show_all_block(sort_merge_base, &buf, show_join_block);
	printf("hash_join result\n");
	show_all_block(hash_join_base, &buf, show_join_block);

	printf("union set\n");
	set_union(p_begin_addr + sort_base, s_begin_addr + sort_base, &buf);
	show_all_block(union_base, &buf, show_64b_block);

	printf("inter set\n");
	set_inter(p_begin_addr + sort_base, s_begin_addr + sort_base, &buf);
	show_all_block(inter_base, &buf, show_64b_block);

	printf("diff set\n");
	set_diff(p_begin_addr + sort_base, s_begin_addr + sort_base, &buf);
	show_all_block(diff_base, &buf, show_64b_block);

	freeBuffer(&buf);
	getchar();
	return 0;
}
