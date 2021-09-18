/*
 * AMGlib Template
 * Powered by Andresmargar
 * from SCENEBETA
 */
 
// Includes AMG
#include <AMG/AMGLib.h>

// Module info
AMG_MODULE_INFO("AMG_Template", AMG_USERMODE, 1, 1);

// Renderiza el lego		// Render lego
void render_lego(const AMG_Model *model){
	for(u8 k=0;k<model->NObjects;k++){
		if(k != 8) AMG_RenderObject(&model->Object[k], model->CelShading);
	}
}

// Renderiza el lego reflejado en el suelo		// Render lego (mirrored)
void render_lego_mirror(const AMG_Model *model){
	for(u8 k=0;k<model->NObjects;k++){
		if(k != 8){
			// Renderiza el objeto		// Render each object
			AMG_RenderMirrorObject(&model->Object[k], 1, false);
		}
	}
}

// Funcion Main()					// Main() function
// Bloque principal del programa	// Program entrypoint
int main(int argc, char **argv){

	// Inicializa la PSP		// Init PSP
	AMG_SetupCallbacks();						// Inicializa los callbacks			// Setup callbacks
	AMG_Init3D(GU_PSM_8888 | AMG_DOUBLEBUFFER);	// Inicializa el motor 3D (el Stencil Buffer solo funciona con esta configuración)
												// Init 3D Engine (Stencil Buffer only works this way)
	AMG_InitTimeSystem();						// Inicializa el motor de tiempo		// Init time system
	
	// Carga la fuente de texto		// Load text font
	AMG_Texture *font = AMG_LoadTexture("Files/Font.png", AMG_TEX_RAM);	// Cargala en VRAM			// Load in VRAM
	AMG_DeleteColor(font, GU_RGBA(0, 0, 0, 0xFF));			// Borra el color negro (transparente)	// Delete black color
	AMG_Create2dObject(font, GU_PSM_5551, true);					// Conviértela en un objeto 2D	// Create 2D Object
	AMG_SwizzleTexture(font);
	
	// Carga el modelo 3D		// Load 3D model
	AMG_Model *lego = AMG_LoadModel("Files/lego.obj");
	
	// Crea una camara y posiciónala		// Create a camera
	AMG_Camera *camera = AMG_InitCamera();
	camera->Pos.z = 8.0f;
	camera->Pos.y = 10.0f;
	
	// Variables adicionales		// Additional stuff
	char text[256];		// Nuestro texto		// Text buffer
	
	// Bucle infinito para mantener el programa funcionando
	// Infinite loop to keep the program running
	while(!AMG.Exit){
		
		// Comienza el dibujado 3D		// Start 3D drawing
		AMG_Begin3D();
		
		// Lee el gamepad		// Read gamepad
		AMG_ReadButtons();
		
		// Posiciona la camara		// Set camera
		AMG_SetCamera(camera);
		
		// Renderiza el modelo 3D (solo el lego, no el suelo)
		// Render 3D model (only lego, not the floor)
		render_lego(lego);
		
		// Renderiza los reflejos (render reflection)
		AMG_StartReflection(&lego->Object[8]);		// Primero iniciamos el motor de reflejos (Stencil Buffer), especificando el objeto que hará de suelo
														// First of all, init the reflection system (Stencil Buffer), setting the object which will work as a floor
		render_lego_mirror(lego);					// Renderizamos el lego en el espejo			// Render lego mirrored
		AMG_FinishReflection();						// Terminamos con el motor de Stencil Buffer	// Finish Stencil Buffer
		
		// Selecciona el modo 2D		// 2D mode
		AMG_OrthoMode(true);
		sprintf(text, "FPS: %d\nVertices: %d", (int)AMG.FPS, (int)AMG.DrawnVertices);
		AMG_Printf(font, 0, 0, GU_RGBA(rand()&0xFF, rand()&0xFF, rand()&0xFF, 0xFF), text);
		AMG_OrthoMode(false);
		
		// Capturas de pantalla		// Screenshots
		if(AMG_Button.Down &PSP_CTRL_CIRCLE) AMG_Screenshot("Photo.png");
		
		// Actualiza el 3D		// Update 3D
		AMG_Update3D();
		AMG_UpdateTime();	// Actualiza el tiempo		// Update time system
	}
	
	// Fin del programa		// Program end
	AMG_UnloadModel(lego);			// Elimina el modelo 3D			// Delete 3D model
	AMG_UnloadTexture(font);		// Elimina la fuente de texto	// Delete text font
	AMG_Finish3D();				// Termina con el modo 3D			// Finish 3D Engine
	return AMG_ReturnXMB();		// Vuelve al XMB					// Go back to XMB
}
