#ifndef _AMG_MULTIMEDIA_H_
#define _AMG_MULTIMEDIA_H_

#ifdef __cplusplus
	extern "C" {
#endif

// Includes
#include <psptypes.h>
#include <pspkerneltypes.h>
#include <pspmp3.h>
#include "AMG_Config.h"

#ifndef AMG_COMPILE_ONELUA

#ifdef AMG_DOC_ENGLISH
/**
 * @file AMG_Multimedia.h
 * @brief Functions related to the multimedia engine (user mode)
 * @author Andrés Martínez (Andresmargar)
 */
#else
/**
 * @file AMG_Multimedia.h
 * @brief Funciones encargadas del motor multimedia (modo usuario)
 * @author Andrés Martínez (Andresmargar)
 */
#endif
 
#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Struct that holds a MP3 file
 */
#else
/**
 * @struct
 * @brief Estructura de un archivo MP3
 */
#endif
typedef struct{
	int Play, Loop;
	int Volume, NChannels, SampleRate, BitRate;
	// USO INTERNO DEL SISTEMA, NO MODIFICAR
	SceMp3InitArg data;
	int f, Handle, channel_id, oldDecoded;
	u32 playedSamples;
}AMG_MP3;

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Inits the multimedia engine
 * @return It returns nothing
 */
#else
/**
 * @brief Inicializa el motor multimedia
 * @return No devuelve nada
 */
#endif
void AMG_InitMultimedia(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Finishes the multimedia engine
 * @return It returns nothing
 */
#else
/**
 * @brief Acaba con el motor multimedia
 * @return No devuelve nada
 */
#endif
void AMG_FinishMultimedia(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Inits audio system
 * @return It returns nothing
 */
#else
/**
 * @brief Inicializa el motor de audio
 * @return No devuelve nada
 */
#endif
void AMG_InitAudio(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Finishes the audio system
 * @return It returns nothing
 */
#else
/**
 * @brief Finaliza el motor de audio
 * @return No devuelve nada
 */
#endif
void AMG_FinishAudio(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Loads a MP3 file
 * @param path The MP3 file path
 * @return A pointer to the MP3 file
 */
#else
/**
 * @brief Carga un archivo MP3
 * @param path La ruta del archivo MP3
 * @return El puntero al archivo MP3
 */
#endif
AMG_MP3 *AMG_LoadMP3(const char *path);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Links a MP3 file
 * @param mp3 The MP3 pointer
 * @return It returns nothing
 */
#else
/**
 * @brief Enlaza un MP3
 * @param mp3 El mp3
 * @return No devuelve nada
 */
#endif
void AMG_LinkMP3(AMG_MP3 *mp3);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Delinks a MP3 file from the system
 * @param mp3 The MP3 file
 * @return It returns nothing
 */
#else
/**
 * @brief Desenlaza un MP3
 * @param mp3 El mp3
 * @return No devuelve nada
 */
#endif
void AMG_DelinkMP3(AMG_MP3 *mp3);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Deletes a MP3 file
 * @param mp3 The MP3 to delete
 * @return It returns nothing
 */
#else
/**
 * @brief Elimina un archivo MP3
 * @param mp3 El MP3 a eliminar
 * @return No devuelve nada
 */
#endif
void AMG_UnloadMP3(AMG_MP3 *mp3);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Updates a MP3
 * @param mp3 The MP3
 * @return It returns nothing
 */
#else
/**
 * @brief Actualiza un MP3
 * @param mp3 El MP3
 * @return No devuelve nada
 */
#endif
void AMG_UpdateMP3(AMG_MP3 *mp3);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Plays an MP3 file
 * @param mp3 The MP3 to play
 * @return It returns nothing
 */
#else
/**
 * @brief Reproduce un archivo MP3
 * @param mp3 El MP3 a reproducir
 * @return No devuelve nada
 */
#endif
static inline void AMG_PlayMP3(AMG_MP3 *mp3){
	mp3->Play = 1;
}

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Pauses an MP3 file
 * @param mp3 The MP3 to pause
 * @return It returns nothing
 */
#else
/**
 * @brief Pausa un archivo MP3
 * @param mp3 El MP3 a pausar
 * @return No devuelve nada
 */
#endif
static inline void AMG_PauseMP3(AMG_MP3 *mp3){
	mp3->Play = 0;
}

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Stops an MP3 file
 * @param mp3 The MP3 to stop
 * @return It returns nothing
 */
#else
/**
 * @brief Detiene un archivo MP3
 * @param mp3 El MP3 a detener
 * @return No devuelve nada
 */
#endif
static inline void AMG_StopMP3(AMG_MP3 *mp3){
	mp3->Play = 0;
	sceMp3ResetPlayPosition(mp3->Handle);
}

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Set the loop for a MP3 file
 * @param mp3 MP3 file
 * @param loop Loop value (true or false)
 * @return It returns nothing
 */
#else
/**
 * @brief Establece el loop de un archivo MP3
 * @param mp3 El archivo MP3
 * @param loop El valor del loop (true o false)
 * @return No devuelve nada
 */
#endif
static inline void AMG_SetLoopMP3(AMG_MP3 *mp3, u8 loop){
	mp3->Loop = (loop == 0 ? -1 : 0);
	sceMp3SetLoopNum(mp3->Handle, mp3->Loop);
}

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Updates the audio Thread
 * @return It returns nothing
 */
#else
/**
 * @brief Actualiza la hebra del motor de audio
 * @return No devuelve nada
 */
#endif
void AMG_UpdateAudio(void);

#endif

#ifdef __cplusplus
	}
#endif

#endif
