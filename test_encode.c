#include <stdio.h>
#include "encode.h"
#include"decode.h"
#include "types.h"
#include<string.h>

void display_help()
{
    printf("\nUsage : \n\n");
    printf("For ENCODING  : ./a.out -e <image file name> <secret file name> <New image name IF NEEDED>\n");
    printf("For DECODING  : ./a.out -d add any no.of files but the file should be of .bmp format\n");
    printf("NOTE          : Image file name should be of .bmp type \n");
    printf("NOTE          : Secret file should be of .txt/.c/.sh format\n");
}

int main(int argc,char* argv[])
{
    
    uint img_size;

    if(argc<2)
    {
        display_help();
        return 0;
    }
    else
    {
        if(strcmp(argv[1],"-e")==0 && argc<4)
        {
            display_help();
            return 0;
        }
        //Call check operation function to determine whether to encode or decode
        int ret=check_operation_type(argv);
        if(ret==e_encode)
        {
            EncodeInfo encInfo;
            int ret2=read_and_validate_encode_args(argv,&encInfo);
            if(ret2==e_failure)
            {
                display_help();
                return 0;
            }
            int ret3=do_encoding(&encInfo);
            if(ret3==e_failure)
            {
                return 0;
            } 
        }
        else if(ret==e_decode)
        {
            DecodeInfo decInfo;
            int ret2=read_and_validate_decode_args(argc,argv,&decInfo);
            if(ret2==e_failure)
            {
                printf("hahaha1\n");
                display_help();
                return 0;
            }
            //To check whether the image file have the magic string
            if(do_decoding(argc,argv,&decInfo)==e_failure)
            {
                display_help();
                return 0;
            }
        }
        else if(ret==e_unsupported)
        {
            display_help();
            return 0;   
        }


    }

    

    return 0;
}
