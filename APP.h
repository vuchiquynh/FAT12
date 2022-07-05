#ifndef _READ_FILE_H_
#define _READ_FILE_H_
/*******************************************************************************
* Include
*******************************************************************************/
#include "FATFS.h"


typedef enum
{
    ERROR_APP_NULL = 0,
}APP_error;

/*******************************************************************************
* API
*******************************************************************************/

/**
 * @brief initialization function
 *
 */
uint8_t APP(const char * FileFath);

/**
 * @brief show all elements of an entry
 *
 * @param[in] entry entry need to display
 */
void APP_DirectoryEntry(FATFS_DirectoryEntry_Struct_t* entry);

/**
 * @brief display all nodes of link list
 *
 * @param[in_out] head first address of the list list
 */
void APP_Display(FATFS_EntryList_Struct_t **head);

#endif /* _READ_FILE_H_ */

/*******************************************************************************
* End of file
*******************************************************************************/
