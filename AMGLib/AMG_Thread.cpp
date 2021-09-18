// Includes
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspmoduleinfo.h>
#include <pspthreadman.h>
#include <psppower.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "AMG_Thread.h"
#include "AMG_3D.h"

#ifndef AMG_COMPILE_ONELUA

// Variables
AMG_Power_ AMG_Power;

// Callback de salida
int AMG_ExitCallback(int arg1, int arg2, void *common){
	AMG.Exit = 1;
	return 0;
}

// Callback de funciones POWER
int AMG_PowerCallback(int unknown, int pwrflags, void *common){
	AMG_Power.Suspended = (pwrflags &PSP_POWER_CB_POWER_SWITCH || pwrflags &PSP_POWER_CB_SUSPENDING);
    AMG_Power.Resuming = (pwrflags & PSP_POWER_CB_RESUMING);
	AMG_Power.Resumed = (pwrflags & PSP_POWER_CB_RESUME_COMPLETE);
	AMG_Power.StandBy = (pwrflags & PSP_POWER_CB_STANDBY);
    sceDisplayWaitVblankStart();
	return 0;
}

// main() de los callbacks
int callbackThread(SceSize args, void *argp){
	AMG.CBID = sceKernelCreateCallback("AMG_ExitCallback", AMG_ExitCallback, NULL);
	sceKernelRegisterExitCallback(AMG.CBID);
	AMG.CBID = sceKernelCreateCallback("AMG_PowerCallback", AMG_PowerCallback, NULL);
	scePowerRegisterCallback(0, AMG.CBID);
	sceKernelSleepThreadCB();
	return 0;
}

// Inicializa los callbacks
void AMG_SetupCallbacks(void){
	// Crea el thread principal
	AMG.MainThreadID = sceKernelCreateThread("AMG_HomeThread", callbackThread, 0x11, 0xFA0, 0, 0);
	if(AMG.MainThreadID >= 0) sceKernelStartThread(AMG.MainThreadID, 0, 0);
	else{
		AMG_Error(AMG_THREAD_SETUP, AMG.MainThreadID, "AMG_HomeThread");
		return;
	}
	// Inicializa los perifericos
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
}

// Devuelve al XMB
int AMG_ReturnXMB(void){
	sceKernelExitGame();
	return 0;
}

// Actualiza el modulo POWER
void AMG_UpdatePower(void){
	AMG_Power.BatteryExists = scePowerIsBatteryExist();
	AMG_Power.ExternalPower = scePowerIsPowerOnline();
	AMG_Power.LowBattery = scePowerIsLowBattery();
	AMG_Power.BatteryCharging = scePowerIsBatteryCharging();
	AMG_Power.BatteryLifeTime = scePowerGetBatteryLifeTime();
	AMG_Power.BatteryLife = scePowerGetBatteryLifePercent();
	AMG_Power.BatteryVolt = scePowerGetBatteryVolt();
	AMG_Power.BatteryTemp = scePowerGetBatteryTemp();
	AMG_Power.CPUFreq = scePowerGetCpuClockFrequency();
	AMG_Power.BUSFreq = scePowerGetBusClockFrequency();
}

// Cambia la velocidad de CPU
void AMG_SetCpuSpeed(int mhz){
	scePowerSetClockFrequency(mhz, mhz, (mhz >> 1));
}

// Crea un Thread
void AMG_CreateThread(AMG_Thread *th){
	th->ThreadID = sceKernelCreateThread(th->Name, th->EntryPoint, th->Priority, th->StackSize, 0, NULL);
	if(th->ThreadID >= 0) sceKernelStartThread(th->ThreadID, 0, 0);
	else{
		AMG_Error(AMG_THREAD_SETUP, th->ThreadID, th->Name);
		return;
	}
}

// Elimina un Thread
void AMG_DeleteThread(AMG_Thread *th){
	sceKernelTerminateDeleteThread(th->ThreadID);
}

#endif
