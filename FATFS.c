#include <stdio.h>
/*******************************************************************************
* Include
*******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "HAL.h"
#include "APP.h"
#include "FATFS.h"
/*******************************************************************************
* Define
*******************************************************************************/
#define SIZE_BOOTSECTOR     512
#define UPPER_12_BITS(x)    (x >> 4)
#define LOWER_12_BITS(x)    (x & 0x0FFF)
#define FAT_OFFSET(x)       (x + (x >> 1))
#define FAT_ENTRY(x, y)     (x[y] + (x[y + 1] * 0x100))
#define EVEN(x)             ((x & 1) != 1)
#define BYTE_2(x, y)        (((uint16_t)(y)<<8) | (x))
#define CLUSTER( x, y)      (((uint32_t)(y)<<16) | (x))

uint32_t posRoot = 0;   /* position sector of root */
uint32_t startOfRoot = 0; /* value start of root */
FATFS_BootSector_Struct_t bootInfo; /*bootsector need to used*/

/**
 * @brief detlete all nodes
 *
 * @param[in_out] head first address of the list list
 */
static void FATFS_DeleteList(FATFS_EntryList_Struct_t **head);

/**
 * @brief cluster next
 *
 * @param[in] fat fat table
 * @param[in_out] cluster next cluster
 */
static void Next_Cluster(uint8_t *fat, uint32_t *cluster);

/*******************************************************************************
* Prototypes
*******************************************************************************/

static void FATFS_DeleteList(FATFS_EntryList_Struct_t **head) /* delete all link list */
{
    FATFS_EntryList_Struct_t *current = NULL;
    FATFS_EntryList_Struct_t *next = NULL;

    current = *head;
    while(current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}

/* read cluster of entry */
static void Next_Cluster(uint8_t *fat, uint32_t *cluster)
{
    uint32_t fatOffset = 0;
    uint32_t fatEntry = 0;

    /* multiply the cluster by 1.5 to get the fat position */
    fatOffset = FAT_OFFSET(*cluster);
    /* read 16 bits from the fat at fat_offset */
    fatEntry = FAT_ENTRY(fat, fatOffset);
    /* if the cluster is even then take the upper 12 bits */
    if (EVEN(*cluster))
    {
        *cluster = LOWER_12_BITS(fatEntry);
    }
    else /* otherwise take the lower 12 bits */
    {
        *cluster = UPPER_12_BITS(fatEntry);
    }
}

uint8_t FATFS_Init(const char * FileFath, uint32_t *fristartOfRoot, uint16_t *sizeSector)
{
    uint8_t *buff = NULL;
    uint32_t error = 0;

    HAL_Init(FileFath); /* read file */
    buff = malloc(SIZE_BOOTSECTOR);
    if (buff == NULL)
    {
        error = ERROR_FATFS_NULL;
    }
    if( HAL_ReadSector(0, buff) != 512) /* read bootsector */
    {
        error = ERROR_FATFS_NULL;
    }
    else
    {
        /* get information of bootsector to look for */
        bootInfo.bytesPerSect = BYTE_2(buff[0x0B], buff[0x0C]);
        bootInfo.reserved = BYTE_2(buff[0x0E], buff[0x0F]);
        bootInfo.numFAT = buff[0x10];
        bootInfo.sectPerFAT = BYTE_2(buff[0x16], buff[0x17]);
        bootInfo.RootDirEntries = BYTE_2(buff[0x11], buff[0x12]);
        startOfRoot  = bootInfo.reserved + ((bootInfo.numFAT)*bootInfo.sectPerFAT);
        *sizeSector = bootInfo.bytesPerSect;
        *fristartOfRoot = startOfRoot;
        posRoot = bootInfo.reserved + ((bootInfo.numFAT)*bootInfo.sectPerFAT) + (bootInfo.RootDirEntries * BYTE_ENTRY) / bootInfo.bytesPerSect;
        free(buff);
        /* update number of 1 Sector down HAL */
        HAL_Update(bootInfo.bytesPerSect);
    }

    return error;
}

uint32_t FATFS_ReadDirectory(FATFS_EntryList_Struct_t **head, uint32_t position)
{
    uint8_t* buffer = NULL;
    FATFS_EntryList_Struct_t *temp = NULL;
    FATFS_EntryList_Struct_t *ptr = NULL;
    uint32_t error = 0;
    uint32_t sectorOfRoot = 0;
    uint32_t index = 0;

    /* sector number of FAT */
    sectorOfRoot = (bootInfo.RootDirEntries * BYTE_ENTRY) / bootInfo.bytesPerSect;
    buffer =(uint8_t *)malloc(sectorOfRoot * bootInfo.bytesPerSect);
    if (buffer == NULL)
    {
        error = ERROR_FATFS_NULL;
    }
    if (HAL_ReadMultiSector(position, sectorOfRoot, buffer) != (int32_t)sectorOfRoot * bootInfo.bytesPerSect) /* read buffer to position */
    {
        error = ERROR_FATFS_READ;
    }
    else
    {
        while (buffer[index] != DIRENTRY_END) /* check DIRENTRY_END */
        {
            if ((buffer[index] != DIRENTRY_DELETED)) /* check the file occupying the entry has been deleted */
            {
                if (buffer[0x0B + index] != 0x0F) /* check entry main */
                {
                    temp = (FATFS_EntryList_Struct_t *)malloc(sizeof(FATFS_EntryList_Struct_t));
                    if(temp == NULL)
                    {
                        error = ERROR_FATFS_NULL;
                    }
                    /* copy information of entry */
                    memcpy(temp->entry.fileName, &buffer[index], 11);
                    temp->entry.attributes = buffer[0x0B + index];
                    memcpy(temp->entry.reserved, &buffer[0x0C + index],2);
                    temp->entry.timeStamp = (uint16_t)(buffer[0x0F + index ]<<8) | buffer[0x0E + index];
                    temp->entry.dateStamp = (uint16_t)(buffer[0x11+index]<<8) | buffer[0x10+index];
                    temp->entry.accessDate = (uint16_t)(buffer[0x13+index]<<8) | buffer[0x12+index];
                    temp->entry.clusterHi = (uint16_t)(buffer[0x15+index]<<8) | buffer[0x14+index];
                    temp->entry.writeTime = (uint16_t)(buffer[0x17+index]<<8) | buffer[0x16+index];
                    temp->entry.writeDate = (uint16_t)(buffer[0x19 + index]<<8) | buffer[0x18 + index];
                    temp->entry.cluster = (uint16_t)(buffer[0x1B + index]<<8) | buffer[0x1A + index];
                    temp->entry.fileSize = (uint32_t)(buffer[0x1F+index]<<24) | (uint32_t)(buffer[0x1E + index]<<16) | (uint32_t)(buffer[0x1D +index]<<8)| buffer[0x1C + index];
                    temp->next = NULL;
                    if (*head == NULL)
                    {
                        *head = temp;
                    }
                    else
                    {
                        ptr = *head;
                        while (ptr->next != NULL)
                        {
                            ptr = ptr->next;
                        }
                        ptr->next = temp;
                    }
                }
            }
            /* move to the next entry address */
            index += 32;
        }
    }
    free(buffer);

    return error;
}

uint8_t FATFS_LoadFile(uint32_t cluster, uint8_t *buffer)
{
    uint32_t position = 0;
    uint8_t error = 0;
    uint8_t *fat = NULL;
    uint32_t fatSize = 0;
    uint32_t count = 0;

    fatSize = bootInfo.sectPerFAT* bootInfo.bytesPerSect;
    fat = (uint8_t *)malloc(fatSize); /* allocate some memory for FAT */
    if (fat == NULL)
    {
        error = ERROR_FATFS_NULL;
    }
    if(HAL_ReadMultiSector(bootInfo.reserved, bootInfo.sectPerFAT, fat) != (int32_t)fatSize) /* read table FAT*/
    {
        error = ERROR_FATFS_READ;
    }
    while (cluster < FAT12_END) /* check end of file */
    {
        position = (cluster - 2) + posRoot; /* position of sector need to read */
        HAL_ReadMultiSector(position, 1, buffer + bootInfo.bytesPerSect*count);
        count++;
        Next_Cluster(fat, &cluster); /* next cluster*/
    }

    free(fat);

    return error;
}

/* get attributes, cluster, file size of entry */
void FATFS_PosEntry(FATFS_EntryList_Struct_t **head, FATFS_PosEntry_Struct_t *posEntry, uint8_t select)
{
    FATFS_EntryList_Struct_t *ptr = NULL;
    uint8_t count = 0;

    ptr = *head;
    while (ptr != NULL)
    {
        if((select - 1) == count)
        {
            posEntry->attributes = ptr->entry.attributes;
            posEntry->cluster = CLUSTER(ptr->entry.cluster, ptr->entry.clusterHi);
            posEntry->fileSize = ptr->entry.fileSize;
        }
        count++;
        ptr = ptr->next;
    }
}

uint8_t FATFS_LoadDirectory(FATFS_EntryList_Struct_t **head,
                            uint32_t cluster)
{
    uint8_t *fat = NULL;
    uint32_t error = 0;
    uint32_t position = 0;
    uint32_t fatSize = 0;

    fatSize = bootInfo.sectPerFAT* bootInfo.bytesPerSect;
    fat = (uint8_t *)malloc (fatSize); /* allocate some memory for FAT */
    if (fat == NULL)
    {
        error = ERROR_FATFS_NULL;
    }
    if(HAL_ReadMultiSector(bootInfo.reserved, bootInfo.sectPerFAT, fat) != (int32_t)fatSize) /* read table FAT*/
    {
        error = ERROR_FATFS_READ;
    }
    else
    {
        FATFS_DeleteList(head); /* delete all link list */
        /* position sector need to read */
        if( cluster == 0)
        {
            position = startOfRoot;
        }
        else
        {
            position = (cluster - 2) + posRoot;
        }
        /* read the file's contents */
        while (cluster < FAT12_END)
        {
            FATFS_ReadDirectory(head, position);
            if(cluster == 0)
            {
                cluster = FAT12_END;
            }
            else
            {
                Next_Cluster(fat, &cluster); /* next cluster */
            }
        }
    }
    free(fat);

    return error;
}

/*******************************************************************************
* End of file
*******************************************************************************/
