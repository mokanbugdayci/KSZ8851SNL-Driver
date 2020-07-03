 /******************************************************************************
 * @filename	: 	ksz8851_config.h
 * @description : 	This file provides configuration settings to source and header files of ksz8851 that is how to compiling or working
 * @author      : 	M.Okan BUĞDAYCI
 * @copyright   : 	GNU licence.
 * @date        : 	03.07.2020
 * @revision	: 	v.1.0.0 - Driver files created
 
 This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
 as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/
 
 ******************************************************************************/
 
 /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KSZ8851_CONFIG_H
#define __KSZ8851_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/


/* Defines -------------------------------------------------------------------*/

#define KSZ_INIT_USE_DEFAULT_DRIVER_SETTINGS						1				// if user wnats to use its own settings for ksz configuration, this defination must be disable.
//#define KSZ_SPI_CONFIG_CS_CONTROLING_BY_USER						1				// if user wants to use non blocking spi functions (with interrupt or dma) or to control CS pin on upper layer, this defination must be enable
//#define KSZ_SPI_CONFIG_USE_SPI_IN_NON_BLOCKING_MODE				1				// if user wants to use non blocking spi functions (with interrupt or dma), this defination must be enable

#ifdef __cplusplus
}
#endif

#endif /* __KSZ8851_H */

