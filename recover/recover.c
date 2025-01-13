#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Check for correct number of command-line arguments
    if (argc != 2)
    {
        printf("Usage: ./recover File\n");
        return 1;
    }

    // Open the memory card file
    FILE *card = fopen(argv[1], "r");
    if (card == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    }

    // Create a buffer for a block of data
    uint8_t buffer[512];
    FILE *img = NULL;  // File pointer for recovered images
    int img_count = 0; // Counter for image filenames
    char filename[8];  // Filename string buffer

    // Read the memory card, one block at a time
    while (fread(buffer, sizeof(uint8_t), 512, card) == 512)
    {
        // Check if the block is the beginning of a JPEG file
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
        {
            // If an image file is already open, close it
            if (img != NULL)
            {
                fclose(img);
            }

            // Create a new file for the new JPEG
            sprintf(filename, "%03i.jpg", img_count++);
            img = fopen(filename, "w");
            if (img == NULL)
            {
                printf("Could not create file.\n");
                return 1;
            }
        }

        // If an image file is open, write the buffer to the image file
        if (img != NULL)
        {
            fwrite(buffer, sizeof(uint8_t), 512, img);
        }
    }

    // Close any remaining files
    if (img != NULL)
    {
        fclose(img);
    }
    fclose(card);

    return 0;
}
