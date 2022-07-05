/*******************************************************************************
* Include
*******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "FATFS.h"

/*******************************************************************************
* Define
*******************************************************************************/

#define HOUR(x) ((x) >> 11)
#define MIN(x) (((x)>>5) & 0x3f)
#define SEC(x) (((x)&0x1F) << 1)

#define DAY(x) ((x)& 0x1F)
#define MONTH(x) (((x)>>5) & 0x0F)
#define YEAR(x) (1980 + ((x)>>9))

/*******************************************************************************
* Prototypes
*******************************************************************************/

/**
 * @brief
 *
 * @param[in_out] head first address of the list list
 * @return uint8_t number node
 */
static uint8_t APP_NumNode(FATFS_EntryList_Struct_t **head);

/**
 * @brief
 *
 * @param arr Array string
 * @return uint8_t number interger
 */
static uint8_t charArrayToInt(char *arr);

/* count the number of nodes */
static uint8_t APP_NumNode(FATFS_EntryList_Struct_t **head)
{
    FATFS_EntryList_Struct_t *ptr = NULL;
    uint8_t ans = 0;

    ptr = *head;
    while (ptr != NULL)
    {
        ans++;
        ptr = ptr->next;
    }

    return ans;
}

static uint8_t charArrayToInt(char *arr)
{
    uint8_t index = 0;
    uint8_t inter = 0;
    uint8_t flag = 0;
    uint8_t value = 0;
    uint8_t size = 0;

    size = strlen(arr);
    /* convert char array to int */
    for( index = 0 ; index < size; ++index){
        if ((arr[index] >= '0') &&  (arr[index] <= '9'))
        {
            inter = arr[index] - '0';
            value = value * 10 + inter;
        }
        else {
            flag = 1;
        }
    }
    if (flag)
    {
        value = -1;
    }

    return value;
}

void APP_DirectoryEntry(FATFS_DirectoryEntry_Struct_t* entry)
{
    uint8_t index = 0;
    uint8_t sizeName = 0;
    if (entry == NULL) {
        return;
    }
    /* display file name */
    for (index = 0; index < sizeof(entry->fileName) - 3; index++) {
        if (entry->fileName[index] != ' ') {
            printf ("%c", entry->fileName[index]);
        } else {
            break;
        }
    }
    if ((((entry->attributes) & ATTR_DIRECTORY) == 0x00) && ((entry->attributes & ATTR_VOLUME_ID) == 0x00)) {
        printf (".");
        sizeName = sizeof(entry->fileName);
        printf ("%c", entry->fileName[sizeName - 3]);
        printf ("%c", entry->fileName[sizeName - 2]);
        printf ("%c", entry->fileName[sizeName - 1]);
    } else {
        printf ("    ");
    }
    if (((entry->attributes) & ATTR_DIRECTORY) == 0x00) {
        printf ("\t%i bytes\t", entry->fileSize);
    }
    else
    {
        printf("\t\t\t\t");
    }
    printf("\t%02d/%02d/%04d ",DAY(entry->writeDate), MONTH(entry->writeDate), YEAR(entry->writeDate));
    printf("%02d:%02d:%02d\n",HOUR(entry->writeTime), MIN(entry->writeTime), SEC(entry->writeTime));

}

void APP_Display(FATFS_EntryList_Struct_t **head)
{
    FATFS_EntryList_Struct_t *ptr = NULL;
    uint8_t count = 0;

    ptr = *head;
    printf("\n-------------------------------------------------------------------\n");
    printf("STT\tName\t\tSize\t\t\tDate modified\n");
    while (ptr != NULL)
    {
        printf("  %d\t", ++count);
        APP_DirectoryEntry(&ptr->entry);
        ptr=ptr->next ;
    }
    printf("-------------------------------------------------------------------\n");
}

uint8_t APP(const char * FileFath)
{
    FATFS_EntryList_Struct_t *head = NULL;
    FATFS_PosEntry_Struct_t posEntry;
    uint8_t *buffer = NULL;
    uint32_t startOfRoot = 0;
    uint32_t select = 0;
    uint8_t ans = 0;
    uint32_t index = 0;
    uint16_t sizeSector = 0;
    char str[10];
    uint8_t error = 0;

    FATFS_Init(FileFath, &startOfRoot, &sizeSector);
    FATFS_ReadDirectory(&head, startOfRoot);
    APP_Display(&head);
    ans = APP_NumNode(&head);
    while(1)
    {
        do
        {
            printf("\nEnter 1->%d: ", ans);
            gets(str);
            select = charArrayToInt(str);
            system("cls");
        } while((select > ans) || (select < 1));

        FATFS_PosEntry(&head, &posEntry, select);
        if(posEntry.attributes == 0x10) /* read folder */
        {
            FATFS_LoadDirectory(&head, posEntry.cluster);
        }
        else /* read file */
        {

            if(posEntry.fileSize != 0)
            {
                buffer = (uint8_t *)malloc(((posEntry.fileSize/sizeSector) + 1) * sizeSector);
                if(buffer == NULL)
                {
                    error = 1;
                }
                FATFS_LoadFile(posEntry.cluster, buffer);
                for (index = 0; index < posEntry.fileSize; index++)
                {
                    printf("%c", buffer[index]);
                }
                free(buffer);
            }
        }
        APP_Display(&head); /* display all nodes of link list*/
        ans = APP_NumNode(&head);
    }

    return error;
}

/*******************************************************************************
* End of file
*******************************************************************************/
