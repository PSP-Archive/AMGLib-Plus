// Includes
#include "AMG_User.h"
#include <time.h>
#include <psprtc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pspdebug.h>
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspgu.h>
#include "AMG_3D.h"

#ifndef AMG_COMPILE_ONELUA

// Variables de apoyo
u32 res, frames=0;
u64 now, prev;
AMG_Button_ AMG_Button;
SceCtrlData pad;
u32 old_buttons=0;
AMG_Time_ AMG_Time;
pspTime amg_time__;

// Datos constantes del SAVEDATA
AMG_Savedata_ AMG_Savedata;
PspUtilitySavedataListSaveNewData newData;
char nameMultiple[][20] = {
	"0000",
	"0001",
	"0002",
	"0003",
	"0004",
	""
};

struct amg_eboot{
	u32 icon0s, pic1s;
	u8 *icon0, *pic1;
}amg_eboot;

// Carga los banners	
void AMG_SavedataSetBanner(const char *icon0, const char *pic1){
	if(amg_eboot.icon0) free(amg_eboot.icon0);
	if(amg_eboot.pic1) free(amg_eboot.pic1);
	FILE *f = fopen(icon0, "rb");
	if(f){
		fseek(f, 0, SEEK_END);
		amg_eboot.icon0s = ftell(f);
		rewind(f);
		amg_eboot.icon0 = (u8*) calloc (amg_eboot.icon0s, sizeof(u8));
		fread(amg_eboot.icon0, amg_eboot.icon0s, 1, f);
		fclose(f); f = NULL;
	}
	f = fopen(pic1, "rb");
	if(f){
		fseek(f, 0, SEEK_END);
		amg_eboot.pic1s = ftell(f);
		rewind(f);
		amg_eboot.pic1 = (u8*) calloc (amg_eboot.pic1s, sizeof(u8));
		fread(amg_eboot.pic1, amg_eboot.pic1s, 1, f);
		fclose(f); f = NULL;
	}
}

// Inicializa el Savedata
void AMG_InitSavedata(void){
	// Establece la clave por defecto
	sprintf(AMG_Savedata.Key, "QTAK319JQKJ952HA");
	// Establece los textos por defecto
	sprintf(AMG_Savedata.GameName, "AMG_GAME");
	sprintf(AMG_Savedata.Title, "Title");
	sprintf(AMG_Savedata.Title2, "Title 2");
	sprintf(AMG_Savedata.Title3, "Details");
	AMG_Savedata.ParentalLevel = 1;
	sprintf(AMG_Savedata.TitleShow, "New Save");
	AMG_Savedata.DrawCallback = NULL;
	// Obten el ICON0, PIC1 y SND0
	u32 offset = 0, o2 = 0;
	FILE *f = fopen("EBOOT.PBP", "rb");
	if(f){
		// ICON0
		fseek(f, 0x0C, SEEK_SET);
		fread(&offset, 4, 1, f);
		fread(&o2, 4, 1, f);
		amg_eboot.icon0s = (o2 - offset);
		if(amg_eboot.icon0s){
			amg_eboot.icon0 = (u8*) calloc (amg_eboot.icon0s, 1);
			fseek(f, offset, SEEK_SET);
			fread(amg_eboot.icon0, amg_eboot.icon0s, 1, f);
		}
		// PIC1
		fseek(f, 0x18, SEEK_SET);
		fread(&offset, 4, 1, f);
		fread(&o2, 4, 1, f);
		amg_eboot.pic1s = (o2 - offset);
		if(amg_eboot.pic1s){
			amg_eboot.pic1 = (u8*) calloc (amg_eboot.pic1s, 1);
			fseek(f, offset, SEEK_SET);
			fread(amg_eboot.pic1, amg_eboot.pic1s, 1, f);
		}
		fclose(f); f = NULL;
	}
}

// Función local: Inicializa el SAVEDATA
void initSavedata(SceUtilitySavedataParam* savedata, PspUtilitySavedataMode mode, void *data, u32 size){
	
	// Inicializa el Savedata
	memset(savedata, 0, sizeof(SceUtilitySavedataParam));
	savedata->base.size = sizeof(SceUtilitySavedataParam);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &savedata->base.language);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &savedata->base.buttonSwap);
	savedata->base.graphicsThread = 0x11;
	savedata->base.accessThread = 0x13;
	savedata->base.fontThread = 0x12;
	savedata->base.soundThread = 0x10;
	savedata->mode = mode;
	savedata->overwrite = 1;
	savedata->focus = PSP_UTILITY_SAVEDATA_FOCUS_LATEST;

#if _PSP_FW_VERSION >= 200
	strncpy(savedata->key, AMG_Savedata.Key, 16);
#endif

	strcpy(savedata->gameName, AMG_Savedata.GameName);	// Nombre identificador del juego
	strcpy(savedata->saveName, "0000");
	savedata->saveNameList = nameMultiple;
	strcpy(savedata->fileName, "SECURE.BIN");

	// Apunta a los datos del juego
	savedata->dataBuf = data;
	savedata->dataBufSize = size;
	savedata->dataSize = size;

	// Si vas a guardar la partida...
	if (mode == PSP_UTILITY_SAVEDATA_LISTSAVE){

		strcpy(savedata->sfoParam.title, AMG_Savedata.Title);
		strcpy(savedata->sfoParam.savedataTitle, AMG_Savedata.Title2);
		strcpy(savedata->sfoParam.detail, AMG_Savedata.Title3);
		savedata->sfoParam.parentalLevel = AMG_Savedata.ParentalLevel;
		
		savedata->icon1FileData.buf = NULL;
		savedata->icon1FileData.bufSize = 0;
		savedata->icon1FileData.size = 0;

		savedata->pic1FileData.buf = amg_eboot.pic1;
		savedata->pic1FileData.bufSize = amg_eboot.pic1s;
		savedata->pic1FileData.size = amg_eboot.pic1s;

		savedata->icon0FileData.buf = amg_eboot.icon0;
		savedata->icon0FileData.bufSize = amg_eboot.icon0s;
		savedata->icon0FileData.size = amg_eboot.icon0s;
		
		savedata->snd0FileData.buf = NULL;
		savedata->snd0FileData.bufSize = 0;
		savedata->snd0FileData.size = 0;

		newData.title = AMG_Savedata.TitleShow;
		savedata->newData = &newData;
		savedata->focus = PSP_UTILITY_SAVEDATA_FOCUS_FIRSTEMPTY;
	}
}

// Guarda o carga los datos del SAVEDATA
void AMG_SavedataDialog(PspUtilitySavedataMode mode, void *data, u32 size){
	if(!AMG.Inited) return;
	AMG_Update3D();		// Asegurate de que se acaba el frame
	SceUtilitySavedataParam dialog;
    initSavedata(&dialog, mode, data, size);
    if(sceUtilitySavedataInitStart(&dialog) != 0) return;

    while(!AMG.Exit) {
		AMG_Begin3D();
		if(AMG_Savedata.DrawCallback) AMG_Savedata.DrawCallback();
		if(!amg_updated){
			AMG_PopMatrix(GU_MODEL);
			sceGuFinish();
			sceGuSync(0, 0);
		}
		
		switch(sceUtilitySavedataGetStatus()){
			case PSP_UTILITY_DIALOG_VISIBLE :
				sceUtilitySavedataUpdate(1);
				break;
			case PSP_UTILITY_DIALOG_QUIT :
				sceUtilitySavedataShutdownStart();
				break;
			case PSP_UTILITY_DIALOG_FINISHED :
			case PSP_UTILITY_DIALOG_NONE :
				return;
		}
		
		if(!amg_updated){
			sceDisplayWaitVblankStart();
			sceGuSwapBuffers();
			amg_updated = 1;
		}
    }
}

// Inicializa el motor de tiempo
void AMG_InitTimeSystem(void){
	srand(time(NULL));
	sceRtcGetCurrentTick(&prev);
	res = sceRtcGetTickResolution();
}

// Actualiza el tiempo
void AMG_UpdateTime(void){
	// Reloj
	sceRtcGetCurrentClock(&amg_time__, AMG.GMT);
	AMG_Time.Hour = amg_time__.hour;
	AMG_Time.Minute = amg_time__.minutes;
	AMG_Time.Second = amg_time__.seconds;
	AMG_Time.Microsecond = amg_time__.microseconds;
	AMG_Time.Year = amg_time__.year;
	AMG_Time.Month = amg_time__.month;
	AMG_Time.Day = amg_time__.day;
	// Contador de FPS
	frames ++;
	sceRtcGetCurrentTick(&now);
	if(((now - prev)/((float)res)) >= 1.0f){
		prev = now;
		AMG.FPS = frames;
		frames = 0;
	}
}

// Lee los botones
void AMG_ReadButtons(void){
	// Lee los datos
	sceCtrlPeekBufferPositive(&pad, 1);
	// Lee el joystick
	AMG_Button.JoyX = (s16)(pad.Lx - 128);
	AMG_Button.JoyY = (s16)(pad.Ly - 128);
	// Botones HELD
	AMG_Button.Held = pad.Buttons;
	u8 i;
	for(i=0;i<32;i++){
		// Botones DOWN
		if((old_buttons &AMG_BIT(i)) < (AMG_Button.Held &AMG_BIT(i))) AMG_Button.Down |= AMG_BIT(i);
		// Botones UP
		else if((old_buttons &AMG_BIT(i)) > (AMG_Button.Held &AMG_BIT(i))) AMG_Button.Up |= AMG_BIT(i);
		// Ninguno de los 2 casos
		else{
			AMG_Button.Down &= ~AMG_BIT(i);
			AMG_Button.Up &= ~AMG_BIT(i);
		}
	}
	old_buttons = AMG_Button.Held;	// Actualiza los botones viejos
}

// Manda un mensaje de la PSP
int AMG_MessageBox(pspUtilityMsgDialogMode mode, u8 yesno, u32 errorvalue, const char *text){
	if(!AMG.Inited) return AMG_MESSAGE_EXCEPTION;
	AMG_Update3D();		// Termina lo que estabas dibujando
	// Rellena la estructura
	pspUtilityMsgDialogParams msg;
	memset(&msg, 0, sizeof(pspUtilityMsgDialogParams));
	msg.base.size = sizeof(pspUtilityMsgDialogParams);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &msg.base.language);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &msg.base.buttonSwap);
	msg.base.graphicsThread = 0x11;
	msg.base.accessThread = 0x13;
	msg.base.fontThread = 0x12;
	msg.base.soundThread = 0x10;
	msg.mode = mode;
	if(mode == PSP_UTILITY_MSGDIALOG_MODE_ERROR){
		msg.options = PSP_UTILITY_MSGDIALOG_OPTION_ERROR;
		msg.errorValue = errorvalue;
	}else{
		msg.options = PSP_UTILITY_MSGDIALOG_OPTION_TEXT;
		sprintf(msg.message, text);
	}
	if(yesno) msg.options |= (PSP_UTILITY_MSGDIALOG_OPTION_YESNO_BUTTONS | PSP_UTILITY_MSGDIALOG_OPTION_DEFAULT_NO);
	// Inicializa el mensaje
	if(sceUtilityMsgDialogInitStart(&msg) != 0) return AMG_MESSAGE_EXCEPTION;
	
	// Procesa el mensaje
	while(!AMG.Exit){
		AMG_Begin3D();
		if(AMG_Savedata.DrawCallback) AMG_Savedata.DrawCallback();
		if(!amg_updated){
			AMG_PopMatrix(GU_MODEL);
			sceGuFinish();
			sceGuSync(0, 0);
		}
		
		switch(sceUtilityMsgDialogGetStatus()){
			case PSP_UTILITY_DIALOG_VISIBLE :
				sceUtilityMsgDialogUpdate(1);
				break;
			case PSP_UTILITY_DIALOG_QUIT :
				sceUtilityMsgDialogShutdownStart();
				break;
			case PSP_UTILITY_DIALOG_FINISHED :
			case PSP_UTILITY_DIALOG_NONE :
				return msg.buttonPressed;
		}
		
		if(!amg_updated){
			sceDisplayWaitVblankStart();
			sceGuSwapBuffers();
			amg_updated = 1;
			AMG.Rendering = false;
			sceGuStart(GU_DIRECT, AMG.List);
		}
	}
	
	// Devuelve el resultado
	return msg.buttonPressed;
}

#endif
