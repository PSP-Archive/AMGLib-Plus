#ifndef _AMG_THREAD_H_
#define _AMG_THREAD_H_

#ifdef __cplusplus
	extern "C" {
#endif

// Includes
#include <psptypes.h>
#include "AMG_Config.h"

#ifndef AMG_COMPILE_ONELUA

#ifdef AMG_DOC_ENGLISH
/**
 * @file AMG_Thread.h
 * @brief Related functions to Hardware and MultiThreading
 * @author Andrés Martínez (Andresmargar)
 */
#else
/**
 * @file AMG_Thread.h
 * @brief Funciones encargadas del módulo Hardware y del MultiThreading
 * @author Andrés Martínez (Andresmargar)
 */
#endif
 
/******************************************************/
/************** THREADS *******************************/
/******************************************************/

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Struct holding the Hardware module (use AMG_Power)
 */
#else
/**
 * @struct
 * @brief Estructura del módulo Hardware de la PSP (usar AMG_Power)
 */
#endif
typedef struct{
	int Suspended, Resuming, Resumed, StandBy;
	int BatteryExists, ExternalPower, LowBattery, BatteryCharging;
	int BatteryLifeTime, BatteryLife, BatteryVolt, BatteryTemp, CPUFreq, BUSFreq;
}AMG_Power_;
extern AMG_Power_ AMG_Power;	/**< El módulo POWER de la PSP */

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Structure holding a Thread
 */
#else
/**
 * @struct
 * @brief Estructura de un Thread (hebra)
 */
#endif
typedef struct{
	SceUID ThreadID;					/**< ID del Thread, no modificar / Thread ID, don't write */
	SceKernelThreadEntry EntryPoint;	/**< Función ejecutada en cada llamada al Thread / Thread function which executes for each call */
	char Name[32];						/**< Nombre del Thread / Thread name */
	int Priority;						/**< Prioridad del Thread (0x18 recomendado) / Thread priority (0x18 recommended) */
	int StackSize;						/**< Tamaño de la pila del Thread (0x10000 recomendado) / Thread stack size (0x10000 recommended) */
}AMG_Thread;

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Inits homebutton callbacks
 * @return It returns nothing
 */
#else
/**
 * @brief Inicializa los callbacks del botón HOME
 * @return No devuelve nada
 */
#endif
void AMG_SetupCallbacks(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Exit the program
 * @return Return value for the main() function
 */
#else
/**
 * @brief Sal del programa hacia en XMB
 * @return El valor que hay que devolver a la función "main"
 */
#endif
int AMG_ReturnXMB(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Updates Hardware module
 * @return It returns nothing
 */
#else
/**
 * @brief Actualiza el módulo Hardware
 * @return No devuelve nada
 */
#endif
void AMG_UpdatePower(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Set CPU speed
 * @param mhz MHz to set
 * @return It returns nothing
 * @note By default: 222 MHz, maximum: 333 MHz
 */
#else
/**
 * @brief Establece la velocidad del CPU
 * @param mhz Los Mhz a establecer al procesador
 * @return No devuelve nada
 * @note Por defecto es 222Mhz, máximo 333Mhz
 */
#endif
void AMG_SetCpuSpeed(int mhz);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Creates a Thread
 * @param th A pointer to the created thread
 * @return It returns nothing
 * @note Important to fill the struct before calling this function
 */
#else
/**
 * @brief Crea una hebra (Thread)
 * @param th Un puntero al thread creado
 * @return No devuelve nada
 * @note Importante rellenar la estructura AMG_Thread antes de llamar a esta función
 */
#endif
void AMG_CreateThread(AMG_Thread *th);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Deletes a Thread
 * @param th Pointer to the thread
 * @return It returns nothing
 */
#else
/**
 * @brief Para y elimina una hebra (Thread)
 * @param th Un puntero al thread a eliminar
 * @return No devuelve nada
 */
#endif
void AMG_DeleteThread(AMG_Thread *th);

#endif

#ifdef __cplusplus
	}
#endif

#endif
