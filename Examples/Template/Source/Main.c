/*
 * AMGlib Template
 * Powered by Andresmargar
 * from SCENEBETA
 */
 
// Includes AMG
#include <AMG/AMGLib.h>

// Module info
AMG_MODULE_INFO("AMG_Template", AMG_USERMODE, 1, 1);

// Funcion Main()					// Main() function
// Bloque principal del programa	// Program entrypoint
int main(int argc, char **argv){

	// Inicializa los callbacks		// Setup callbacks
	AMG_SetupCallbacks();
	
	// Inicializa la consola de texto	// Init text console
	pspDebugScreenInit();
	
	// Imprime un texto en pantalla		// Print text on screen
	pspDebugScreenPrintf("\n HelloWorld!!\n\n");
	pspDebugScreenPrintf(" Press HOME to exit");
	
	// Bucle infinito para mantener el programa funcionando
	// Infinite loop to keep the program running
	while(!AMG.Exit){
		
		// Espera hasta el VBlank (actualiza la pantalla)
		// Wait for VBlank (update screen)
		sceDisplayWaitVblankStart();
	}
	
	// Sal al XMB		// Go back to XMB
	return AMG_ReturnXMB();
}
