// Includes
#include "AMG_Multimedia.h"
#include "AMG_3D.h"
#include <stdio.h>
#include <stdlib.h>
#include <pspaudio.h>
#include <psputility.h>
#include <pspiofilemgr.h>
#include <malloc.h>
#include <pspkernel.h>
#include <pspmoduleinfo.h>
#include <pspthreadman.h>

#ifndef AMG_COMPILE_ONELUA
 
// Variables locales
int audio_id = -1;

// Espacio para 4 MP3
AMG_MP3 *snd[4];

// Rellena el buffer de transmisión de un MP3
int amg_fillmp3buffer(int f, int handle){
	SceUChar8* dst; SceInt32 write, pos, read;
	// Obtén información de la transmisión
	int s = sceMp3GetInfoToAddStreamData(handle, &dst, &write, &pos);
	if(s < 0){ AMG_Error(AMG_MODULE_EXCEPTION, s, "sceMp3GetInfoToAddStreamData"); return 0;}
	// Ponte en la posición adecuada en el archivo
	s = sceIoLseek32(f, pos, SEEK_SET);
	if(s < 0){ AMG_Error(AMG_MODULE_EXCEPTION, s, "amg_fillmp3buffer: sceIoLseek32"); return 0;}
	// Lee los datos
	read = sceIoRead(f, dst, write);
	if(read < 0){ AMG_Error(AMG_MODULE_EXCEPTION, s, "amg_fillmp3buffer: sceIoRead"); return 0;}
	else if(read == 0) return 0;
	// Añade los datos leídos al buffer original
	s = sceMp3NotifyAddStreamData(handle, read);
	if(s < 0){ AMG_Error(AMG_MODULE_EXCEPTION, s, "sceMp3NotifyAddStreamData"); return 0;}
	return (pos > 0);
}

// Reproduce un MP3
void AMG_UpdateMP3(AMG_MP3 *mp3){
	// Comprueba si es NULL
	if(mp3 == NULL) return;
	// Reproduce el MP3
	if(mp3->Play){
		// Rellena el buffer, si es necesario
		if(sceMp3CheckStreamDataNeeded(mp3->Handle) > 0) amg_fillmp3buffer(mp3->f, mp3->Handle);
		// Decodifica los samples
		short* buf;
		int bytesDecoded;
		int retries = 0;
		// Nos aseguramos si hemos salido del archivo...
		for (;retries<1;retries++){
			bytesDecoded = sceMp3Decode(mp3->Handle, &buf);
			if(bytesDecoded > 0) break;
			if(sceMp3CheckStreamDataNeeded(mp3->Handle) <= 0) break;
			if(!amg_fillmp3buffer(mp3->f, mp3->Handle)) mp3->playedSamples = 0;
		}
		if ((bytesDecoded < 0) && (bytesDecoded != (int)0x80671402)){ AMG_Error(AMG_MODULE_EXCEPTION, bytesDecoded, "sceMp3Decode"); return;}
		// Si hemos llegado al final del archivo...
		if ((bytesDecoded == 0) || (bytesDecoded==(int)0x80671402)){
			mp3->Play = 0;
			sceMp3ResetPlayPosition(mp3->Handle);
			mp3->playedSamples = 0;
		}else{
			// Reproduce los samples, primero, reserva un canal de audio
			if ((mp3->channel_id < 0) || (mp3->oldDecoded != bytesDecoded)){
				if(mp3->channel_id >= 0) sceAudioSRCChRelease();
				mp3->channel_id = sceAudioSRCChReserve(bytesDecoded/(mp3->NChannels*2), mp3->SampleRate, mp3->NChannels);
				mp3->oldDecoded = bytesDecoded;
			}
			// Reproduce este frame de audio
			mp3->playedSamples += sceAudioSRCOutputBlocking(mp3->Volume, buf);
		}
	}
}

// Thread de audio
int audioThread(SceSize args, void *argp){
	while(!AMG.Exit){
		// Procesa los 4 sonidos
		for(u8 i=0;i<4;i++){
			if(snd[i] != NULL) AMG_UpdateMP3(snd[i]);
		}
		// Vuelve a thread principal
		sceKernelSleepThread();
	}
	return 0;
}

// Inicializa el motor multimedia
void AMG_InitMultimedia(void){
	if(AMG.MultimediaInited) return;
	if(sceUtilityLoadModule(PSP_MODULE_AV_AVCODEC) < 0){ AMG_Error(AMG_MODULE_INIT, -1, "PSP_MODULE_AV_AVCODEC"); return;}
	AMG.MultimediaInited = true;
}

// Acaba con el motor multimedia
void AMG_FinishMultimedia(void){
	sceUtilityUnloadModule(PSP_MODULE_AV_AVCODEC);
}

// Inicializa el motor de audio
void AMG_InitAudio(void){
	// Carga los módulos necesarios
	if(sceUtilityLoadModule(PSP_MODULE_AV_MP3) < 0){ AMG_Error(AMG_MODULE_INIT, -1, "PSP_MODULE_AV_MP3"); return;}
	// Crea el Thread de audio
	audio_id = sceKernelCreateThread("AMG_AudioThread", audioThread, 0x18, 0x10000, 0, NULL);
	if(audio_id >= 0) sceKernelStartThread(audio_id, 0, 0);
	else{ AMG_Error(AMG_THREAD_SETUP, audio_id, "AMG_AudioThread"); return;}
	for(u8 i=0;i<4;i++) snd[i] = NULL;
}

// Actualiza el motor de audio
void AMG_UpdateAudio(void){
	sceKernelWakeupThread(audio_id);
}

// Finaliza el motor de audio
void AMG_FinishAudio(void){
	// Elimina el Thread de audio
	sceKernelTerminateDeleteThread(audio_id);
	// Elimina los módulos cargados
	sceUtilityUnloadModule(PSP_MODULE_AV_MP3);
}

// Carga un MP3 en RAM
AMG_MP3 *AMG_LoadMP3(const char *path){
	// Comprueba si es NULL
	AMG_MP3 *mp3 = (AMG_MP3*) calloc (1, sizeof(AMG_MP3));
	// Abre el archivo
	mp3->f = sceIoOpen(path, PSP_O_RDONLY, 0777);
	if(mp3->f < 0){ AMG_Error(AMG_OPEN_FILE, 0, path); return NULL;}
	// Inicializa los recursos de pspmp3
	if(sceMp3InitResource() < 0){ AMG_Error(AMG_MODULE_EXCEPTION, -1, "sceMp3InitResource(\"%s\")", path); return NULL;}
	// Inicializa el streaming
	mp3->data.mp3StreamStart = 0;
	mp3->data.mp3StreamEnd = sceIoLseek32(mp3->f, 0, SEEK_END);
	mp3->data.unk1 = 0;
	mp3->data.unk2 = 0;
	mp3->data.mp3Buf = (u8*) memalign (64, 16<<10);
	mp3->data.mp3BufSize = (16 << 10);
	mp3->data.pcmBuf = (u16*) memalign (64, 16*1152);
	mp3->data.pcmBufSize = (16*1152);
	mp3->Handle = sceMp3ReserveMp3Handle(&mp3->data);
	if(mp3->Handle < 0){ AMG_Error(AMG_MODULE_EXCEPTION, mp3->Handle, "sceMp3ReserveMp3Handle(\"%s\")", path); return NULL;}
	if((mp3->data.mp3Buf == NULL) || (mp3->data.pcmBuf == NULL)){ AMG_Error(AMG_OUT_OF_RAM, (16<<10)+(16*1152), NULL); return NULL;}
	// Rellena el buffer de transmisión
	amg_fillmp3buffer(mp3->f, mp3->Handle);
	// Inicializa el MP3
	if(sceMp3Init(mp3->Handle) < 0){ AMG_Error(AMG_MODULE_EXCEPTION, -1, "sceMp3Init"); return NULL;}
	// Obtén información sobre el MP3
	mp3->SampleRate = sceMp3GetSamplingRate(mp3->Handle);
	mp3->NChannels = sceMp3GetMp3ChannelNum(mp3->Handle);
	mp3->BitRate = sceMp3GetBitRate(mp3->Handle);
	mp3->Volume = PSP_AUDIO_VOLUME_MAX;
	mp3->Play = 0; mp3->Loop = 0;
	mp3->playedSamples = 0;
	mp3->oldDecoded = 0;
	mp3->channel_id = -1;
	// Devuelve el MP3 creado
	return mp3;
}

// Enlaza un MP3
void AMG_LinkMP3(AMG_MP3 *mp3){
	if(mp3 == NULL) return;
	u8 i;
	for(i=0;i<4;i++){
		if(snd[i] == NULL){
			snd[i] = mp3; return;
		}
	}
	AMG_Error(AMG_CUSTOM_ERROR, 0, "AMG_LinkMP3: No free slots");
}

// Desenlaza un MP3
void AMG_DelinkMP3(AMG_MP3 *mp3){
	if(mp3 == NULL) return;
	u8 i;
	for(i=0;i<4;i++){
		if(snd[i] == mp3){
			snd[i] = NULL; return;
		}
	}
}

// Elimina un MP3
void AMG_UnloadMP3(AMG_MP3 *mp3){
	if(mp3 == NULL) return;
	// Elimina todos los datos
	if(mp3->channel_id >= 0) sceAudioSRCChRelease();
	sceMp3ReleaseMp3Handle(mp3->Handle);
	sceMp3TermResource();
	sceIoClose(mp3->f);
}

#endif
