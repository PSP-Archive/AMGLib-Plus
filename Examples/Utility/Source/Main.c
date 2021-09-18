/*
 * AMGlib Template
 * Powered by Andresmargar
 * from SCENEBETA
 */
 
// Includes AMG
#include <AMG/AMGLib.h>

// Module info
AMG_MODULE_INFO("AMG_Template", AMG_USERMODE, 1, 1);

// Callback de dibujado (solo cuando actuamos sobre un diálogo con la PSP)
// Rendering callback (only called when a PSP dialog is shown)
void DrawCallback(void){
	AMG_OrthoMode(1);		// Activa el modo 2D		// Enable 2D mode
	
	// Solo para ver si funciona...		// Just to see if this works...
	AMG.ClearColor ++;
	
	AMG_OrthoMode(0);		// Desactiva el modo 2D		// Disable 2D mode
}

// Funcion Main()					// Main() function
// Bloque principal del programa	// Program entrypoint
int main(int argc, char **argv){

	// Inicia la PSP			// Init PSP
	AMG_SetupCallbacks();						// Inicia los callbacks		// Setup callbacks
	AMG_Init3D(GU_PSM_5650 | AMG_DOUBLEBUFFER);	// Inicializa el motor 3D	// Init 3D Engine
	AMG_InitTimeSystem();						// Inicializa el tiempo		// Init time system
	
	// Crea los datos		// Create some data
	char *data = (char*) calloc (32, sizeof(char));
	u32 size = 32;
	sprintf(data, "HelloWorld!!");
	
	// Inicia el sistema de partidas		// Init savedata system
	AMG_InitSavedata();
	
	// Configuramos algunos datos de la partida		// Customize the savegame
	sprintf(AMG_Savedata.Title, "AMGLib Adventures");
	sprintf(AMG_Savedata.Title2, "Savegame data");
	sprintf(AMG_Savedata.Title3, "Thanks for using AMGLib!!");
	AMG_Savedata.DrawCallback = DrawCallback;
	
	// Comprueba que podemos mandar mensajes
	// Check if we can send messages
	u8 supported = 1;
	if(AMG_MessageBox(AMG_MESSAGE_STRING, 0, 0, "...") == AMG_MESSAGE_EXCEPTION) supported = 0;
	
	// Si no son soportados...		// If not supported
	if(!supported) AMG_Error(AMG_CUSTOM_ERROR, 0, "WTF?! Message Boxes aren't supported by your PSP");
	
	// Muestra un mensaje del sistema	// Show a system message
	AMG_MessageBox(AMG_MESSAGE_STRING, 0, 0, "Welcome to AMGLib Adventures");
	
	// Pedimos sí o no al usuario para guardar la partida
	// Request wether saving or not data
	int result = AMG_MessageBox(AMG_MESSAGE_STRING, 1, 0, "Would you like to save game data?");
	if(result == AMG_MESSAGE_YES){
		AMG_SavedataDialog(AMG_SAVEDATA_SAVE, data, size);
	}else{
		result = AMG_MessageBox(AMG_MESSAGE_STRING, 1, 0, "Would you like to read data instead?");
		if(result == AMG_MESSAGE_YES){
			AMG_SavedataDialog(AMG_SAVEDATA_LOAD, data, size);
			AMG_MessageBox(AMG_MESSAGE_STRING, 0, 0, data);		// Muestra los datos leídos		// Show read data
		}else{
			AMG_Error(AMG_CUSTOM_ERROR, 0, "User wouldn't like to save data");		// <-- Este error se mostrará con mensajes del sistema (si son soportados)
																					// <-- This error will be shown with system messages (if supported)
		}
	}
	
	// Último mensaje		// Last one :)
	result = AMG_MessageBox(AMG_MESSAGE_STRING, 1, 0, "Would you like to exit NOW?");
	if(result == AMG_MESSAGE_YES){
		AMG_Finish3D();
		return AMG_ReturnXMB();
	}else{
		AMG_MessageBox(AMG_MESSAGE_STRING, 0, 0, "Nope? Well, you'll stay blocked unless you press HOME");
	}
	
	// Bucle infinito para mantener el programa funcionando
	// Infinite loop to keep the program running
	while(!AMG.Exit){
		AMG_Begin3D();			// Comienza el dibujado 3D		// Begin 3D drawing
		
		// Callback de dibujado		// Drawing callback
		DrawCallback();
		
		AMG_Update3D();			// Actualiza el 3D		// Update 3D
	}
	
	// Sal al XMB		// Go back to XMB
	AMG_Finish3D();
	return AMG_ReturnXMB();
}
