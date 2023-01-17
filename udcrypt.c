/*
 * Author:      Upayan De (de-upayan)
 * Date:        15-01-2023
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _ 512 /* Define maximum length of string */

int weight;
int keylen;
char key[_];
char inPath[_];
char outPath[_];
char extension[8];
char *lastDot;
long long fileSize;
FILE *inFilePtr;
FILE *outFilePtr;

/* Function to encrypt/decrypt a byte using the previous byte, character at current index in key and key weight */
char crypt(char word , char prev , int idx)
{
        return word ^ prev ^ key[idx % keylen] ^ (weight % 256);
}

int getInput()
{
        int i;
        int j;
        
        /* Read input file path */
        printf("Enter input filepath: \t\e[1;38;5;123m");
        scanf("%[^\n]s" , inPath);
        printf("\e[0m");
        getchar(); /* Consumes trailing newline */
        
        /* Open input file, show error message if unsuccessful */
        if((inFilePtr = fopen(inPath , "rb")) == NULL)
        {
                printf("\e[1;91mERROR! \e[3;91mFile not found\e[0m\n");
                return 0;
        }
        
        /* Find and store the size of file */
        fseek(inFilePtr , 0 , SEEK_END);
        fileSize = ftell(inFilePtr);
        rewind(inFilePtr);
        
        /* Locate the leftmost dot in the file path (extension dot) */
        lastDot = strrchr(inPath , '.');
        
        i = 0;
        
        /* Copy all characters from inPath to outPath sequentially till lastDot */
        for(j = 0 ; inPath[i] && (inPath + i < lastDot) ; i++ , j++)
        {
                outPath[j] = inPath[i];
        }
        outPath[j] = 0; /* Terminate string with null character */
        
        /* Copy rest of the characters to extension */
        for(j = 0 ; inPath[i] ; i++ , j++)
        {
                extension[j] = inPath[i];
        }
        extension[j] = 0; /* Terminate string with null character */
        
        /* Read key */
        printf("Enter key: \t\t\e[1;38;5;123m");
        scanf("%[^\n]s" , key);
        printf("\e[0m");
        getchar(); /* Consumes trailing newline */
        
        /* Compute keylen and weight (sum of ASCII values of all chars in key) */
        keylen = strlen(key);
        weight = 0;
        for(i = 0 ; key[i] ; i++)
        {
                weight += key[i];
        }
        
        return 1;
}

void updateProgressBar(int barSize , long long curUnit , long long totUnit)
{
        long long i;
        
        /* Calculate number of filled bars */
        int fill = (curUnit * barSize) / totUnit;
        
        /* Move the cursor to the beginning of the current line */
        printf("\e[100D");
        
        printf("\e[1;97mProgress: ");
        
        /* Print filled bars */
        printf("\e[48;5;28m");
        for(i = 0 ; i < fill ; i++)
        {
                printf(" ");
        }
        
        /* Print empty bars */
        printf("\e[0m");
        for(; i < barSize ; i++)
        {
                printf(" ");
        }
        
        /* Print progress percentage */
        printf(" %.1f%%" , 100.0 * curUnit / totUnit);
}

void processAndWriteEnc()
{
        int j;
        long long i;
        long long prev_i;
        long long threshold;
        char readByte;
        char prevByte;
        char encrByte;
        
        i = 0;
        prevByte = 0;
        
        /* Open output file with .ud extension */
        strcat(outPath , "_enc.ud"); /* Append _enc to the end of output file name */
        outFilePtr = fopen(outPath , "wb");
        
        /* Encrypt and write the reversed key to the beginning of the file for verification */
        for(j = 0 ; key[j] ; j++)
        {
                encrByte = crypt(key[keylen - 1 - j] , prevByte , i);
                fwrite(&encrByte , 1 , 1 , outFilePtr);
                prevByte = key[keylen - 1 - j];
                i++;
        }
        
        /* Encrypt and write file extension */
        for(j = 0 ; extension[j] ; j++)
        {
                encrByte = crypt(extension[j] , prevByte , i);
                fwrite(&encrByte , 1 , 1 , outFilePtr);
                prevByte = extension[j];
                i++;
        }
        
        /* Encrypt and write record separator character (ASCII 30) to mark end of extension */
        encrByte = crypt(30 , prevByte , i);
        fwrite(&encrByte , 1 , 1 , outFilePtr);
        prevByte = 30;
        i++;
        
        /* Encrypt and write file contents */
        threshold = (keylen + strlen(extension) + 1 + fileSize) / 5000;
        prev_i = i;
        while(fread(&readByte , 1 , 1 , inFilePtr))
        {
                encrByte = crypt(readByte , prevByte , i);
                fwrite(&encrByte , 1 , 1 , outFilePtr);
                prevByte = readByte;
                i++;
                
                /* If increase in progress > 0.1%, update the progress bar */
                if(i - prev_i >= threshold)
                {
                        updateProgressBar(50 , i , threshold * 5000);
                        prev_i = i;
                }
        }
        
        /* Update the progress bar to 100% */
        updateProgressBar(50 , 1 , 1);
        printf("\n");
        
        /* Close file pointers */
        fclose(inFilePtr);
        fclose(outFilePtr);
}

void processAndWriteDec()
{
        /* Check if the extension of input file is ".ud", otherwise display error message */
        if(strcmp(extension , ".ud"))
        {
                printf("\e[1;38;5;197mERROR! \e[3;38;5;197mInvalid encrypted file extension\e[0m\n");
                return;
        }
        
        int j;
        long long i;
        long long prev_i;
        long long threshold;
        char readByte;
        char decrByte;
        
        i = 0;
        decrByte = 0;
        
        /* Validate the key */
        for(j = 0 ; key[j] ; j++)
        {
                fread(&readByte , 1 , 1 , inFilePtr);
                decrByte = crypt(readByte , decrByte , i);
                if(decrByte != key[keylen - 1 - j])
                {
                        printf("\e[1;38;5;197mValidation FAILED! \e[3;38;5;197mIncorrect key\e[0m\n");
                        return;
                }
                i++;
        }
        printf("\e[1;38;5;46mValidation SUCCESS!\e[0m\n");
        
        /* Decrypt the file extension */
        for(j = 0 ; j < 8 ; j++)
        {
                fread(&readByte , 1 , 1 , inFilePtr);
                decrByte = crypt(readByte , decrByte , i);
                i++;
                
                /* If current byte is a record separator, break */
                if(decrByte != 30)
                {
                        extension[j] = decrByte;
                }
                else
                {
                        extension[j] = 0; /* Terminate string with null character */
                        break;
                }
        }
        
        /* Open output file with correct extension */
        strcat(outPath , "_dec"); /* Append _dec to the end of output file name */
        strcat(outPath , extension);
        outFilePtr = fopen(outPath , "wb");
        
        /* Decrypt file contents and write to output file */
        threshold = (keylen + strlen(extension) + 1 + fileSize) / 5000;
        prev_i = i;
        while(fread(&readByte , 1 , 1 , inFilePtr))
        {
                decrByte = crypt(readByte , decrByte , i);
                fwrite(&decrByte , 1 , 1 , outFilePtr);
                i++;
                
                /* If increase in progress > 0.1%, update the progress bar */
                if(i - prev_i >= threshold)
                {
                        updateProgressBar(50 , i , threshold * 5000);
                        prev_i = i;
                }
        }
        
        /* Update the progress bar to 100% */
        updateProgressBar(50 , 1 , 1);
        printf("\n");
        
        /* Close file pointers */
        fclose(inFilePtr);
        fclose(outFilePtr);
}

int main()
{       
        int i;
         
        /* Driver loop */
        while(1)
        {
                /* Print UDCrypt menu */
                printf("\e[1;48;5;23mUDCrypt\e[0m\n");
                printf("1. \e[1;38;5;37mEncrypt\e[0m file\n");
                printf("2. \e[1;38;5;44mDecrypt\e[0m file\n");
                printf("0. \e[1;97mExit\e[0m\n");
                
                /* Get user choice */
                printf(">> ");
                scanf("%d" , &i);
                getchar(); /* Consumes trailing newline */
                
                /* If choice is Exit, break from driver loop */
                if(i == 0)
                {
                        break;
                }
                
                /* Perform action (encryption/decryption) according to user choice */
                switch(i)
                {
                        case 1:
                                if(getInput())
                                {
                                        processAndWriteEnc();
                                }
                                break;
                        case 2:
                                if(getInput())
                                {
                                        processAndWriteDec();
                                }
                                break;
                        default:
                                printf("\e[1;38;5;197mERROR! \e[3;38;5;197mInvalid choice\e[0m\n");
                }
                printf("\n");
        }
        return 0;
}
