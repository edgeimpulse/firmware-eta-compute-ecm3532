#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
typedef struct
{
    //
    //! Pointer to the image name.
    //
    const char *pcImageName;

    //
    //! P/X/Ymem sizes.
    //
    uint32_t ui32PmemSize;
    uint32_t ui32XmemSize;
    uint32_t ui32YmemSize;

    //
    //! Pointer to the P/X/Ymem data.
    //
    const uint32_t *pui32PmemData;
    const uint16_t *pui16XmemData;
    const uint16_t *pui16YmemData;
}
tDspMem;

#include "dsp_new.h"
char * buf = 0;

#define DBG
#define ARRAY_NAME(X) #X
#define ARRAY_2_FILE(X)   writeToFile(ARRAY_NAME(X)".bin",X, sizeof(X))
int writeToFile ( char * filename , const char * ptr , unsigned int size);

int writeToFile ( char * filename, const char * ptr , unsigned int size)
{
	int index = 0;	 
	FILE *  write_ptr=0;
	FILE *  read_ptr =0;
	char data = 0;
	write_ptr = fopen (filename,"wb");
	if( !write_ptr )
		printf (" could not create file\n");
	else 
		fwrite(ptr,size,1,write_ptr); 
	fclose ( write_ptr);
#ifdef DBG
	read_ptr = fopen (filename,"rb");
	for ( index =0 ;  index <size; index ++)
	{
		fread(&data,1,1,read_ptr);
	   // printf("%d\n", data);
		if (ptr[index] != data)
			printf (" ### Error in reading abck data\n");
	}
	fclose ( read_ptr);
#endif
	
}

unsigned char binFile[0x40000] = {0};
const char deli[] = ".";

void combineBinFiles(tDspMem* header,const char* pmem, const char* xmem, const char* ymem, char* filename)
{
	unsigned int count = 0;
	unsigned char* ptr = binFile;
	FILE *  write_ptr=0;

	ptr += sizeof(tDspMem);
	count += sizeof(tDspMem);

	header->pcImageName = count;

	memcpy(ptr, DSP_IMAGE_NAME, sizeof(DSP_IMAGE_NAME));
	ptr += sizeof(DSP_IMAGE_NAME);
	count += sizeof(DSP_IMAGE_NAME);

	header->pui32PmemData = count;

	memcpy(ptr, pmem, sizeof(pmem_image_data));
	ptr += sizeof(pmem_image_data);
	count += sizeof(pmem_image_data);

	header->pui16XmemData = count;

	memcpy(ptr, xmem, sizeof(xmem_image_data));
	ptr += sizeof(xmem_image_data);
	count += sizeof(xmem_image_data);

	header->pui16YmemData = count;

	memcpy(ptr, ymem, sizeof(ymem_image_data));
	ptr += sizeof(ymem_image_data);
	count += sizeof(ymem_image_data);

	memcpy(binFile, header, sizeof(tDspMem));

	char* name = strtok(filename, deli);

	strcat(name,".bin");

	write_ptr = fopen (name,"wb");
	if( !write_ptr )
		printf (" could not create file\n");
	else
		fwrite(binFile,count,1,write_ptr);
	fclose ( write_ptr);
}

main(int argc, char* argv[])
{
	// ARRAY_2_FILE(pmem_image_data);
	// ARRAY_2_FILE(xmem_image_data);	
	// ARRAY_2_FILE(ymem_image_data);	

	combineBinFiles(&dsp_mem, pmem_image_data, xmem_image_data, ymem_image_data, argv[1]);
}



