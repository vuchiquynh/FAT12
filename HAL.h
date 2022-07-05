#ifndef _HAL_H_
#define _HAL_H_
/*******************************************************************************
* Include
*******************************************************************************/
#include <stdint.h>

/*******************************************************************************
* Prototypes
*******************************************************************************/

typedef enum
{
    HAL_ERROR_SEEK = -1,
    HAL_ERROR_NULL
}HAL_error;

/*******************************************************************************
* API
*******************************************************************************/

/**
 * @brief read files
 *
 * @param[in] FileFath file path
 * @return uint8_t ERROR_HAL_NULL
 */
uint8_t HAL_Init(const char * FileFath);

/**
 * @brief update sector
 *
 * @param[in_out] bytesPerSect number of bytes of a sector
 */
void HAL_Update(uint16_t bytesPerSect);

/**
 * @brief read 1 sector
 *
 * @param[in] index number of sectors to read
 * @param[in_out] buff
 * @return int32_t number of bytes read
 */
int32_t HAL_ReadSector(uint32_t index, uint8_t *buff);

/**
 * @brief  read multiple sectors
 *
 * @param[in] index number of sectors to read
 * @param[in] num
 * @param[in_out] buff
 * @return int32_t number of bytes read
 */
int32_t HAL_ReadMultiSector(uint32_t index, uint32_t num, uint8_t *buff);

/**
 * @brief fclose file 
 *
 */
void HAL_DeInit();

#endif /* _HAL_H_ */

/*******************************************************************************
* End of file
*******************************************************************************/

