/*******************************************************************************
* Include
*******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include "HAL.h"
/*******************************************************************************
* Define
*******************************************************************************/
uint16_t sizeSector = 512; /* Number of tracks of 1 sector */

static FILE *fp = NULL;

/*******************************************************************************
* Prototype
*******************************************************************************/

uint8_t HAL_Init(const char * FileFath) /* Open file */
{
    HAL_error error = 0;

    fp = fopen(FileFath, "rb"); /*open file to read*/
    if (fp == NULL )
    {
        error = HAL_ERROR_NULL;
    }

    return error;
}

void HAL_Update(uint16_t bytesPerSect)  /* Update sector */
{
    sizeSector = bytesPerSect;
}

int32_t HAL_ReadSector(uint32_t index, uint8_t *buff) /* Read 1 sector */
{
    uint32_t nbyte = 0;

    if (buff == NULL)
    {
        nbyte = HAL_ERROR_NULL;
    }
    if (fseek(fp, (uint64_t)index*sizeSector, SEEK_SET) != 0x00)
    {
        nbyte = HAL_ERROR_SEEK;
    }
    nbyte = fread(buff, 1, sizeSector, fp);
    
    return nbyte;
}

int32_t HAL_ReadMultiSector(uint32_t index, uint32_t num, uint8_t *buff) /* read multiple sectors */
{
    uint32_t nbyte = 0;

    if (buff == NULL)
    {
        nbyte = HAL_ERROR_NULL;
    }
    if (fseek(fp, (uint64_t)index*sizeSector, SEEK_SET) != 0x00)
    {
        nbyte = HAL_ERROR_SEEK;
    }
    nbyte = fread(buff, 1, num * sizeSector, fp);

    return nbyte;
}

void HAL_DeInit() /* close file */
{
    fclose(fp);
}

/*******************************************************************************
* End of file
*******************************************************************************/
