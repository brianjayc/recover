#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//define a BYTE type
typedef uint8_t BYTE;

//define a constant 512 block size
const int BLOCK_SIZE = 512;

int main(int argc, char *argv[])
{
    //check if there is exactly one command line argument
    if (argc != 2)
    {
        printf("Invalid usage. Usage: recover IMAGE\n");
        return 1;
    }

    printf("Opening %s...\n", argv[1]);

    // Open raw file
    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        printf("Could not open card.\n");
        return 1;
    }

    printf("Success!\n");

    //Read raw file
    BYTE *buffer = malloc(BLOCK_SIZE);

    printf("Reading card...\n");

    int count = 0;

    FILE *img = NULL;

    while (fread(buffer, sizeof(BYTE), BLOCK_SIZE, f) == BLOCK_SIZE)
    {
        //check for jpeg signature
        if (buffer[0] == 0xff &&
            buffer[1] == 0xd8 &&
            buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
        {
            //if first jpg file
            if (count == 0)
            {
                char filename[8];

                sprintf(filename, "%03i.jpg", count);

                printf("Found %s!\n", filename);

                // Open output img file
                img = fopen(filename, "w");
                if (img == NULL)
                {
                    printf("Could not create image file.\n");
                    return 1;
                }

                // write current block to image file
                fwrite(buffer, sizeof(BYTE), BLOCK_SIZE, img);

                //increment count
                count += 1;
            }
            // if not first jpg file
            else
            {
                //close previous img file
                fclose(img);

                char filename[8];

                sprintf(filename, "%03i.jpg", count);

                printf("Found %s!\n", filename);

                // Open new output img file
                img = fopen(filename, "w");
                if (img == NULL)
                {
                    printf("Could not create image file.\n");
                    return 1;
                }

                // write current block to new image file
                fwrite(buffer, sizeof(BYTE), BLOCK_SIZE, img);

                //increment count
                count += 1;
            }
        }

        // continue writing if not start of a new jpg file
        else if (count > 0)
        {
            // write current block to current image file
            fwrite(buffer, sizeof(BYTE), BLOCK_SIZE, img);
        }
    }

    //free memory allocation and close all files used

    fclose(img);

    free(buffer);

    fclose(f);

    return 0;
}
