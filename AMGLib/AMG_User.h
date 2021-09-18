#ifndef _AMG_USER_H_
#define _AMG_USER_H_

#ifdef __cplusplus
	extern "C" {
#endif

// Includes
#include <psptypes.h>
#include <pspkerneltypes.h>
#include <psputility.h>
#include "AMG_Config.h"

#ifndef AMG_COMPILE_ONELUA

#ifdef AMG_DOC_ENGLISH
/**
 * @file AMG_User.h
 * @brief Functions related to user module and devices
 * @author Andrés Martínez (Andresmargar)
 */
#else
/**
 * @file AMG_User.h
 * @brief Funciones encargadas del módulo de usuario y periféricos
 * @author Andrés Martínez (Andresmargar)
 */
#endif
 
/******************************************************/
/************** USUARIO *******************************/
/******************************************************/

#ifdef AMG_DOC_ENGLISH
/**
 * @def
 * @brief Get a bitmask
 */
#else
/**
 * @def
 * @brief Obten el bit "n" de un byte
 */
#endif
#define AMG_BIT(n) (1 << (n))

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Struct holding the PSP's buttons (use AMG_Button)
 * @example Know if you push X: if(AMG_Button.Held &PSP_CTRL_CROSS) ...
 */
#else
/**
 * @struct
 * @brief Estructura de los botones de la PSP (usar AMG_Button)
 * @example Saber si se está pulsando la X: if(AMG_Button.Held &PSP_CTRL_CROSS) ...
 */
#endif
typedef struct{
	s16 JoyX;			/**< Valor X del joystick / Joystick X */
	s16 JoyY;			/**< Valor Y del joystick / Joystick Y */
	u32 Held;			/**< Valor de las teclas HELD / HELD keys*/
	u32 Down;			/**< Valor de las teclas DOWN / DOWN keys */
	u32 Up;				/**< Valor de las teclas UP / UP keys */
}AMG_Button_;
extern AMG_Button_ AMG_Button;

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief PSP clock struct (use AMG_Time)
 * @example: Know the hours: AMG_Time.Hour
 */
#else
/**
 * @struct
 * @brief Estructura del reloj de la PSP (usar AMG_Time)
 * @example: Saber la hora: AMG_Time.Hour
 */
#endif
typedef struct{
	u8 Hour;			/**< La hora (0-24) / Hour (0-24) */
	u8 Minute;			/**< Los minutos (0-60) / Minutes (0-60) */
	u8 Second;			/**< Los segundos (0-60) / Seconds (0-60) */
	u16 Year;			/**< El año / Year */
	u16 Month;			/**< El mes (1-12) Month (1-12) */
	u16 Day;			/**< El día (1-31) Day (1-31) */
	u32 Microsecond;	/**< Microsegundos / Microseconds */
}AMG_Time_;
extern AMG_Time_ AMG_Time;

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Inits the time system
 * @return It returns nothing
 */
#else
/**
 * @brief Inicializa el motor de tiempo
 * @return No devuelve nada
 */
#endif
void AMG_InitTimeSystem(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Update the time system
 * @return It returns nothing
 */
#else
/**
 * @brief Actualiza el motor de tiempo
 * @return No devuelve nada
 */
#endif
void AMG_UpdateTime(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Read the PSP's buttons state
 * @return No devuelve nada
 */
#else
/**
 * @brief Lee el estado de los botones de la PSP
 * @return No devuelve nada
 */
#endif
void AMG_ReadButtons(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Inits savedata
 * @return It returns nothing
 */
#else
/**
 * @brief Inicializa el guardado de datos
 * @return No devuelve nada
 */
#endif
void AMG_InitSavedata(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Load savedata banners
 * @param icon0 ICON0.PNG path
 * @param pic1 PIC1.PNG path
 * @return It returns nothing
 */
#else
/**
 * @brief Carga los banners del savedata
 * @param icon0 Ruta del icon0
 * @param pic1 Ruta del pic1
 * @return No devuelve nada
 */
#endif
void AMG_SavedataSetBanner(const char *icon0, const char *pic1);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Save or read savedata
 * @param mode AMG_SAVEDATA_SAVE or AMG_SAVEDATA_LOAD
 * @param data Pointer where data is read/saved
 * @param Data size
 * @return It returns nothing
 */
#else
/**
 * @brief Guarda o lee los datos de la partida
 * @param mode El modo, AMG_SAVEDATA_SAVE o AMG_SAVEDATA_LOAD
 * @param data El puntero a donde se guardan/leen los datos
 * @param El tamaño de los datos
 * @return No devuelve nada
 */
#endif
void AMG_SavedataDialog(PspUtilitySavedataMode mode, void *data, u32 size);

typedef struct{
	char Key[16];	/**< La clave para encriptar la partida (solo en modo KERNEL) / Key to encrypt savedata (KERNEL mode only) */
	char GameName[13];	/**< El nombre identificador del juego / Game Identifier name*/
	char Title[64];		/**< Título del juego / Game Title*/
	char Title2[64];	/**< El segundo título del juego / Second title*/
	char Title3[128];	/**< Detalles del juego / Game details*/
	u32 ParentalLevel;	/**< Nivel de control parental / Parental Level*/
	char TitleShow[16];	/**< El título a mostrar en caso de no haber una partida guardada / Title to show when no savedata is found */
	void (*DrawCallback)(void);		/**< El callback para dibujar mientras actuamos sobre la partida / Callback to draw while saving/reading data */
}AMG_Savedata_;
extern AMG_Savedata_ AMG_Savedata;

// Define el modo de escritura/lectura de la partida
#define AMG_SAVEDATA_SAVE 	PSP_UTILITY_SAVEDATA_LISTSAVE
#define AMG_SAVEDATA_LOAD 	PSP_UTILITY_SAVEDATA_LISTLOAD
#define AMG_MESSAGE_ERROR 	PSP_UTILITY_MSGDIALOG_MODE_ERROR
#define AMG_MESSAGE_STRING 	PSP_UTILITY_MSGDIALOG_MODE_TEXT
#define AMG_MESSAGE_YES		PSP_UTILITY_MSGDIALOG_RESULT_YES
#define AMG_MESSAGE_NO		PSP_UTILITY_MSGDIALOG_RESULT_NO
#define AMG_MESSAGE_BACK	PSP_UTILITY_MSGDIALOG_RESULT_BACK
#define AMG_MESSAGE_EXCEPTION (int)0xFF0AFF01

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Show a system message
 * @param mode (0 for an error, 1 for a text message)
 * @param yesno If it's a yes/no message or not
 * @param errorvalue Error code
 * @param text Text to show
 * @return Wether yes or no have been pushed, or AMG_MESSAGE_EXCEPTION in case it fails
 */
#else
/**
 * @brief Manda un mensaje del sistema
 * @param mode (0 para un error, 1 para un mensaje de texto)
 * @param yesno Si es un mensaje de sí/no o un aceptar
 * @param errorvalue Código de error
 * @param text El texto a mostrar
 * @return Si se ha pulsado SI o NO, o AMG_MESSAGE_EXCEPTION si ha habido un error
 */
#endif
int AMG_MessageBox(pspUtilityMsgDialogMode mode, u8 yesno, u32 errorvalue, const char *text);

#endif

#ifdef __cplusplus
	}
#endif

#endif
