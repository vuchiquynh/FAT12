#ifndef _FATFS_H_
#define _FATFS_H_
/*******************************************************************************
* Define
*******************************************************************************/
#define ATTR_VOLUME_ID 		0x08
#define ATTR_DIRECTORY      0x10
#define ATTR_ARCHIVE        0x20
#define END_OF_FILE         0xFF0
#define FAT12_END           0xFF8
#define DIRENTRY_DELETED    0xE5
#define DIRENTRY_END        0x00
#define BYTE_ENTRY          32

typedef enum
{
    ERROR_FATFS_NULL = 0,
    ERROR_FATFS_READ
}FATFS_error;

typedef struct
{
    uint16_t    bytesPerSect;   /* number sector */
    uint16_t    reserved;       /* sector number before the FAT table */
    uint8_t     numFAT;         /* FAT table number */
    uint16_t    sectPerFAT;     /* sector number of FAT */
    uint16_t    RootDirEntries; /* entry number of RDET */
}FATFS_BootSector_Struct_t;

/* format of entry */
typedef struct {
    uint8_t     fileName[11];   /* filename extenstion */
    uint8_t     attributes;     /* file attribute */
    uint8_t     reserved[2];    /* reserved */
    uint16_t    timeStamp;      /* creation time */
    uint16_t    dateStamp;      /* creation date */
    uint16_t    accessDate;     /* last access date */
    uint16_t    clusterHi;      /* cluster Hight */
    uint16_t    writeTime;      /* last modification time */
    uint16_t    writeDate;      /* last modification date */
    uint16_t    cluster;        /* starting cluster */
    uint32_t    fileSize;       /*file size  */
} FATFS_DirectoryEntry_Struct_t;

typedef struct Link_list
{
    FATFS_DirectoryEntry_Struct_t entry;
    struct Link_list *next;
}FATFS_EntryList_Struct_t;

typedef struct posEntry
{
    uint8_t     attributes;
    uint32_t    cluster;
    uint32_t    fileSize;
} FATFS_PosEntry_Struct_t;

/*******************************************************************************
* API
*******************************************************************************/

/**
 * @brief read bootsector
 *
 * @param[in] FileFath fath file
 * @param[in_out] startOfRoot position read root
 * @param[in_out] sizeSector number of 1 sector
 * @return uint8_t error
 */
uint8_t FATFS_Init(const char * FileFath, uint32_t *fristartOfRoot, uint16_t *sizeSector);

/**
 * @brief Read the sector at the location , and create a link list for the file folder
 *
 * @param[in_out] head first address of the list list
 * @param[in] position position read
 * @return uint32_t
 */
uint32_t FATFS_ReadDirectory(FATFS_EntryList_Struct_t **head, uint32_t position);

/**
 * @brief read file
 *
 * @param[in] cluster cluster to read
 * @param[in_out] buffer buffer stores the contents of the file
 * @return uint8_t error
 */
uint8_t FATFS_LoadFile(uint32_t cluster, uint8_t *buffer);

/**
 * @brief get attributes, cluster, file size of entry
 *
 * @param[in_out] head first address of the list list
 * @param[in_out] posEntry attributes, cluster, file size of entry
 * @param[in] select entry to read
 */
void FATFS_PosEntry(FATFS_EntryList_Struct_t **head, FATFS_PosEntry_Struct_t *posEntry, uint8_t select);

/**
 * @brief read folder 
 *
 * @param[in_out] head first address of the list list
 * @param[in] cluster cluster to read
 * @return uint8_t error
 */
uint8_t FATFS_LoadDirectory(FATFS_EntryList_Struct_t **head, uint32_t cluster);

/**
 * @brief fclose file
 */
void FATFS_DeInit();

#endif /* _FATFS_H_ */

/*******************************************************************************
* End of file
*******************************************************************************/
