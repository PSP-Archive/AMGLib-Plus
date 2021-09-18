#ifndef _AMGLIB_H_
#define _AMGLIB_H_

#ifdef __cplusplus
	extern "C" {
#endif

// Includes C
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <psprtc.h>
#include <psptypes.h>
#include <pspctrl.h>
#include <pspaudio.h>
#include <pspmp3.h>

// Includes PSP
#include <pspge.h>
#include <pspgu.h>

// Includes AMGLib
#include "AMG_Config.h"		// Opciones de compilación
#include "AMG_Model.h"
#include "AMG_Thread.h"
#include "AMG_User.h"
#include "AMG_Physics.h"	// + AMG_Texture.h
#include "AMG_Multimedia.h"
#include "AMG_3D.h"

#ifdef AMG_DOC_ENGLISH
/**
 * @file AMGLib.h
 * @brief Library's main header
 * @author Andrés Martínez (Andresmargar)
 */
#else
/**
 * @file AMGLib.h
 * @brief Cabecera principal de la librería
 * @author Andrés Martínez (Andresmargar)
 */
#endif
 
#ifdef AMG_DOC_ENGLISH
/**
 * @def
 * @brief User mode
 */
#else
/**
 * @def
 * @brief El modo Usuario
 */
#endif
#define AMG_USERMODE 0

#ifdef AMG_DOC_ENGLISH
/**
 * @def
 * @brief Kernel mode
 */
#else
/**
 * @def
 * @brief El modo Kernel
 */
#endif
#define AMG_KERNELMODE 0x1000

#ifdef AMG_DOC_ENGLISH
/**
 * @def
 * @brief Defines a PSP program
 * @param title Module title
 * @param kernel AMG_USERMODE or AMG_KERNELMODE
 * @param major Major version
 * @param minor Minor version
 */
#else
/**
 * @def
 * @brief Define un programa de PSP
 * @param title El título del programa
 * @param kernel AMG_USERMODE o AMG_KERNELMODE
 * @param major Versión Major
 * @param minor Versión Minor
 */
#endif
#define AMG_MODULE_INFO(title, kernel, major, minor) \
	PSP_MODULE_INFO(title, kernel, major, minor);	\
	PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU); \
	PSP_HEAP_SIZE_KB(20 << 10)

#ifdef AMG_DOC_ENGLISH
/**
 * @def
 * @brief Get the physical address of a ponter (to flush it from cache)
 * @param p Pointer to uncache
 */
#else
/**
 * @def
 * @brief Obtén la dirección física de un puntero (para que no actúe el caché)
 * @param p El puntero
 */
#endif
#define AMG_UNCACHE_POINTER(p) ((void*)((int)(p)|0x40000000))

#ifdef __cplusplus
	}
#endif

#endif
