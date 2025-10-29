#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include<string.h>

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    //set the offset back to starting
    fseek(fptr_image,0,SEEK_SET);

    // Return image capacity
    return width * height * 3;
}

//Function to get secret file size
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    long size=ftell(fptr);
    fseek(fptr,0,SEEK_SET);
    return size;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");   //to print error
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

//To check what operation need to be done
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==e_success)
    {
        printf("INFO: Operation - ENCODING\n");
        return e_encode;
    }
    else if(strcmp(argv[1],"-d")==e_success)
    {
        printf("INFO: Operation - DECODING\n");
        return e_decode;
    }
    else
    {
        printf("INFO: INVALID OPERATION\n");
        return e_unsupported;
    }
}

//To read and validate the commendline arguments
static const char* extension[3]={".txt",".c",".sh"};
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    printf("INFO: Reading and validating commendline argumets\n");
    if(strstr(argv[2],".bmp"))
    {
        encInfo->src_image_fname=argv[2];
        for(int i=0;i<3;i++)
        {
            if(strstr(argv[3],extension[i]))
            {
                encInfo->secret_fname=argv[3];
                strcpy(encInfo->extn_secret_file, extension[i]);
                int extn_size=strlen(encInfo->extn_secret_file);
                if(argv[4])
                {
                    encInfo->stego_image_fname=argv[4];
                    return e_success;
                }
                else
                {
                    printf("INFO: Output file Not mentioned, Creating encodedimage.bmp as default\n");
                    encInfo->stego_image_fname="encodedimage.bmp";
                    return e_success;         
                }
            }
        }
        printf("ERROR: Invalid secret file\n");
        return e_failure;
    }
    else
    {
        printf("ERROR: Invalid source image\n");
        return e_failure;
    }
}

//Function to check capacity of the source image to store the secret file data
Status check_capacity(EncodeInfo *encInfo)
{
    int total_bits = (strlen(MAGIC_STRING) + 4 + strlen(encInfo->extn_secret_file) + 4 + encInfo->size_secret_file) * 8;
    if (encInfo->image_capacity >= total_bits)
    return e_success;
    else
    return e_failure;

}

//Function to copy bmp header to the new image file
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    unsigned char bmp_header[54];
    //Read header from file to bmp header 
    fread(bmp_header,1,54,fptr_src_image);
    //Write bmp header data to the new image
    fwrite(bmp_header,1,54,fptr_dest_image);
    printf("INFO: bmp header data copied\n");
    
}

//Function to encode magic string
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("INFO: Magic string encoding begins\n");
    int Magic_string_size=strlen(magic_string);
    encode_data_to_image(magic_string, Magic_string_size, encInfo);  
    printf("INFO: Done✅\n");

}

//Function to read data from image to which we need to encode
Status encode_data_to_image(const char *data, int size, EncodeInfo *encInfo)
{
    //printf("INFO: Encoding %s data to image begins\n",data);
    for(int i=0;i<size;i++)
    {
        fread(encInfo->image_data,1,MAX_IMAGE_BUF_SIZE,encInfo->fptr_src_image);
        encInfo->secret_data[0]=data[i];
        encode_byte_to_lsb(encInfo->secret_data,encInfo->image_data);
        //printf("INFO: Encoding %s data to image \n",encInfo->secret_data);
        fwrite(encInfo->image_data,1,MAX_IMAGE_BUF_SIZE,encInfo->fptr_stego_image);
    }
    //printf("INFO: Encoding %s done \n",data);
}

//Function to encode bit by bit
Status encode_byte_to_lsb(char *data, char *image_buffer)
{
    //printf("INFO: Encode byte to LSB begins\n");
    for(int i=7;i>=0;i--)
    {
        image_buffer[7-i]=image_buffer[7-i]&(~1);
        image_buffer[7-i]=image_buffer[7-i]|((data[0]>>i)&1);
    }
    //printf("INFO: Encode byte to LSB done\n");
}

//Function to encode secret file extn size
Status encode_secret_file_extn_size(EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret File extension \n");
    int extn_size=strlen(encInfo->extn_secret_file);
    unsigned char *usize=(unsigned char*)&extn_size;
    for(int i=0;i<sizeof(int);i++)
    {
        fread(encInfo->image_data,1,MAX_IMAGE_BUF_SIZE,encInfo->fptr_src_image);
        //printf("size =%d",*(usize+i));
        encode_size_to_lsb(usize+i, encInfo->image_data);
        fwrite(encInfo->image_data,1,MAX_IMAGE_BUF_SIZE,encInfo->fptr_stego_image);
    }
    printf("INFO: Done✅\n");
}

//Function to encode size to the new image
Status encode_size_to_lsb(unsigned char *size,char *image_buffer)
{
    printf("*size =%d\n",*size);
    //printf("INFO: Encode size to LSB begins\n");
    for(int i=7;i>=0;i--)
    {
        image_buffer[7-i]=image_buffer[7-i]&(~1);
        image_buffer[7-i]=image_buffer[7-i]|((*size>>i)&1);
    }
    //printf("INFO: Encode size to LSB done\n");
}

//Function to encode secret file extn
Status encode_secret_file_extn(EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret File extension\n");
    int extn_size=strlen(encInfo->extn_secret_file);
    encode_data_to_image(encInfo->extn_secret_file, extn_size, encInfo);
    printf("INFO: Done✅\n");
}

//Function to encode secret file size
Status encode_secret_file_size(EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret File size \n");
    int file_size=encInfo->size_secret_file;
    unsigned char *usize=(unsigned char*)&file_size;
    for(int i=0;i<sizeof(int);i++)
    {
        fread(encInfo->image_data,1,MAX_IMAGE_BUF_SIZE,encInfo->fptr_src_image);
        encode_size_to_lsb(usize+i, encInfo->image_data);
        fwrite(encInfo->image_data,1,MAX_IMAGE_BUF_SIZE,encInfo->fptr_stego_image);
        memset(encInfo->image_data,0,MAX_IMAGE_BUF_SIZE);
    }
    printf("INFO: Done✅\n");
}

//Function to encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret data\n");
    for(int i=0;i<encInfo->size_secret_file;i++)
    {
        fread(encInfo->secret_data,1,MAX_SECRET_BUF_SIZE,encInfo->fptr_secret);
        encode_data_to_image(encInfo->secret_data, MAX_SECRET_BUF_SIZE, encInfo);
    }
    printf("INFO: Done✅\n");
}

//Function to copy remianing data
Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    printf("INFO: Copying leftover data\n");
    unsigned char ch;
    int n=0;
    while (n=fread(&ch, 1, 1, encInfo->fptr_src_image))
    {
        fwrite(&ch, 1, 1, encInfo->fptr_stego_image);
        memset(encInfo->image_data,0,MAX_IMAGE_BUF_SIZE);
        //printf("src image:%lu \n stego image: %lu\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
    }
    printf("INFO: Done✅\n");
    //printf("src image:%lu \n stego image: %lu\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));

}



//Function to encode data to image
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_failure)
    {
        return e_failure;
    } 
    //Get_image_size_for_bmp
    uint img_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    printf("INFO: Image size = %u\n", img_size);
    encInfo->image_capacity=img_size;

    //Get secret file size
    uint secretfile_size = get_file_size(encInfo->fptr_secret);
    printf("INFO: secret file size = %u\n", secretfile_size);
    encInfo->size_secret_file=secretfile_size;

    //To check source image capacity
    if(check_capacity(encInfo)==e_failure)
    {
        printf("ERROR: No enough space in source file to encode data\n");
        return e_failure;
    }
    printf("INFO: Source image is set to encode\n");

    //Copy bmp header into new image file
    copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    fseek(encInfo->fptr_src_image, 54, SEEK_SET);


    //Copy magic string to the new image file
    encode_magic_string(MAGIC_STRING, encInfo);

    //Encode secret file extn size
    encode_secret_file_extn_size(encInfo);

    //Encode secret file extn
    encode_secret_file_extn(encInfo);

    //Encode secret file size
    encode_secret_file_size(encInfo);

    //Encode secret file data
    encode_secret_file_data(encInfo);

    //copy remaining data to the new image
    copy_remaining_img_data(encInfo);

    printf("\n\n## ENCODING DONE SUCCESSFULLY ✅\n\n");

    
}



