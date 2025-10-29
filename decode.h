#ifndef DECODE_H
#define DECODE_H
#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret data from source Image to secret file 
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname;  //to store beautiful.bmp name
    FILE *fptr_src_image;   //to hold the beautiful.bmp offset
    char image_data[8];

    /* Secret File Info */
    char *secret_fname;     //to store the secret.txt name
    FILE *fptr_secret;      //to hold the secret.txt offset
    char extn_secret_file[MAX_FILE_SUFFIX];     //to store the extension of secret file
    char secret_data[MAX_SECRET_BUF_SIZE];      //to store the secret data
    char magic_string[15];

} DecodeInfo;


/* Encoding function prototype */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(int file_count,char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
//Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status do_decoding(int file_count,char *argv[],DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(int size,DecodeInfo *deccInfo);

/*Get secret file extension*/
Status extract_secret_file_extn(DecodeInfo *decInfo);

/*Get file size*/
uint extract_Data_size(DecodeInfo *decInfo);

/* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);


/* Decode data from LSB of image data */
Status decode_lsb_to_byte(int size,char *data,DecodeInfo *decInfo);



#endif