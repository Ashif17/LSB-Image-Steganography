#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include<string.h>

/*Function definitions*/

/*Function to read and validate commandline arguments for decoding*/
Status read_and_validate_decode_args(int file_count,char *argv[], DecodeInfo *decInfo)
{
    printf("INFO : Reading and validating commandline arguments\n");
    for(int i=2;i<file_count;i++)
    {
        if(strstr(argv[i],".bmp")==0)
        {
            return e_failure;
        }
    }
    printf("INFO: Done✅\n");
    return e_success;
}

/*Function to open the file having the provided magic string*/
Status do_decoding(int file_count,char *argv[],DecodeInfo *decInfo)
{
    printf("INFO : Decoding starts\n");
    for(int i=2;i<file_count;i++)
    {
        decInfo->fptr_src_image=fopen(argv[i],"rb");
        int size=strlen(MAGIC_STRING);
        memset(decInfo->magic_string,0,15);
        decode_magic_string(size,decInfo);
        if(strcmp(decInfo->magic_string,MAGIC_STRING)==e_success)
        {
            extract_secret_file_extn(decInfo);
        }
        static const char* extension[3]={".txt",".c",".sh"};
        for(int j=0;j<3;j++)
        {
            if(strcmp(decInfo->extn_secret_file,extension[j])==e_success)
            {
                decode_secret_file_data(decInfo);
            }
        }

    }
    fclose(decInfo->fptr_src_image);
    printf("\n\n## DECODED SUCCESSFULLY ✅\n\n");
}

/*Function to decode magic string*/
Status decode_magic_string(int size,DecodeInfo *decInfo)
{
    //skip 54 byte header
    printf("INFO : Decoding magic string\n");
    fseek(decInfo->fptr_src_image,54,SEEK_SET);
    decode_lsb_to_byte(size,decInfo->magic_string,decInfo);
    printf("INFO: Done✅\n");

}

/*Function to take bits brom LSB*/
Status decode_lsb_to_byte(int size,char *data,DecodeInfo *decInfo)
{
    for(int i=0;i<size;i++)
    {
        fread(decInfo->image_data,1,MAX_IMAGE_BUF_SIZE,decInfo->fptr_src_image);
        for(int j=0;j<8;j++)
        {
            unsigned char bit;
            bit=(decInfo->image_data[j])&1;
            bit=bit<<(7-j);
            data[i]=data[i]|bit;
        }
        if(decInfo->fptr_secret)
        {
            fwrite(data,1,MAX_SECRET_BUF_SIZE,decInfo->fptr_secret);
        }
    }
}

/*Function to decode secret file extension size*/
Status extract_secret_file_extn(DecodeInfo *decInfo)
{
    printf("INFO : Decoding secret file extn\n");
    memset(decInfo->extn_secret_file,0,4);
    int secret_file_extn_size=extract_Data_size(decInfo);
    printf("INFO : file size =%d\n",secret_file_extn_size);
    decode_lsb_to_byte(secret_file_extn_size,decInfo->extn_secret_file,decInfo);
    printf("INFO: Done✅\n");
}

/*Function to decode data size*/
uint extract_Data_size(DecodeInfo *decInfo)
{
    int value=0;
    unsigned char *value2=(unsigned char*)&value;
    for(int i=0;i<sizeof(int);i++)
    {
        unsigned char ch=0;
        fread(decInfo->image_data,1,8,decInfo->fptr_src_image);
        for(int j=7;j>=0;j--)
        {
            unsigned char bit=0;
            bit=(decInfo->image_data[7-j])&1;
            bit=bit<<(j);
            ch=ch|bit;
        }
        value2[i]=ch;
    }
    return value;
}

/*Function to extract secret file size and write it to a new file*/
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    printf("INFO : Decoding secret data\n");
    int data_size=extract_Data_size(decInfo);
    printf("INFO: Secret file size =%d\n",data_size);
    char decoded_file_name[20]="decoded_data";
    strcat(decoded_file_name,decInfo->extn_secret_file);
    decInfo->fptr_secret=fopen(decoded_file_name,"w");
    for(int i=0;i<data_size;i++)
    {
        memset(decInfo->secret_data,0,1);
        decode_lsb_to_byte(1,decInfo->secret_data,decInfo);
    }
    fclose(decInfo->fptr_secret);
    printf("INFO: Done✅\n");
}
