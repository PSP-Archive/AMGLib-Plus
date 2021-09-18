/*
 * AMGlib Template
 * Powered by Andresmargar
 * from SCENEBETA
 */
 
// Includes AMG
#include <AMG/AMGLib.h>

// Module info
AMG_MODULE_INFO("AMG_Template", AMG_USERMODE, 1, 1);

// Defines
#define SPEED 0.2f		// Velocidad de movimiento	// Speed movement
#define SCALE 0.15f		// Escala de nuestra nave	// Out ship scale

// Funcion Main()					// Main() function
// Bloque principal del programa	// Program entrypoint
int main(int argc, char **argv){

	// Inicializa la PSP
	AMG_SetupCallbacks();							// Inicializa los callbacks		// Setup callbacks
	AMG_Init3D(GU_PSM_5650 | AMG_DOUBLEBUFFER);		// Inicializa el motor 3D		// Init 3D Engine
	AMG_InitTimeSystem();							// Inicializa el tiempo			// Init time system
	
	// Carga la fuente de texto		// Load text font
	AMG_Texture *font = AMG_LoadTexture("Files/font.png", AMG_TEX_RAM);
	AMG_DeleteColor(font, 0xFF000000);
	AMG_Create2dObject(font, GU_PSM_5551, 1);
	AMG_SwizzleTexture(font);
	
	// Carga el modelo 3D			// Load 3D model
	AMG_Model *tie_fighter = AMG_LoadModel("Files/Tie-Fighter.obj");
	tie_fighter->Object[0].Pos.x = 0.0f;		// Posicionamos el modelo		// Set model position
	tie_fighter->Object[0].Pos.y = 7.0f;
	tie_fighter->Object[0].Pos.z = 4.0f;
	tie_fighter->Object[0].Scale.x = SCALE;		// Configuramos la escala		// Set model scale
	tie_fighter->Object[0].Scale.y = SCALE;
	tie_fighter->Object[0].Scale.z = SCALE;
	
	// Crea el mapa de alturas		// Create a heightmap
	AMG_Vector scale = {10.0f, 10.0f, 10.0f};	// Escala de nuestro desierto		// Scale of our desert
	AMG_Object *hmap = AMG_CreateHeightmap("Files/hmap.png",		// Ruta del mapa de alturas					// Heightmap file path
										   "Files/hmap_tex.png", 	// Textura del mapa (pasa NULL si no tiene)	// Map texture (NULL if it doesn't have one)
										   -1.0f, 					// Altura mínima del mapa	// Minimum height
										   1.0f, 					// Altura máxima del mapa	// Maximum height
										   &scale, 					// Escala (pasa NULL para tener el tamaño original)	// Scale (NULL for original size)
										   NULL);					// Posición (pasa NULL para que esté en el origen)	// Position (NULL will set it to origin)
	
	// Crea una cámara		// Create a camera
	AMG_Camera *cam = AMG_InitCamera();
	cam->Pos.z = 7.0f;
	cam->Pos.y = tie_fighter->Object[0].Pos.y;
	cam->Eye.y = tie_fighter->Object[0].Pos.y;
	
	// Variables adicionales	// Additional stuff
	float angle = 0.0f;
	char text[256];			// Para el texto	// Text buffer
	
	// Bucle infinito para mantener el programa funcionando
	// Infinite loop to keep the program running
	while(!AMG.Exit){
		AMG_Begin3D();		// Comienza el dibujado 3D		// Begin 3D drawing
		AMG_ReadButtons();	// Leemos el GamePad			// Read Gamepad
		
		// Animación de la nave al moverse
		// Ship animation
		if(AMG_Button.Held &(PSP_CTRL_UP | PSP_CTRL_DOWN | PSP_CTRL_RIGHT | PSP_CTRL_LEFT)){	// Si nos movemos...	// If moving...
			cam->Pos.y = 7.0f + (AMG_Sin(AMG_Deg2Rad(angle)) / 2.0f);	// Cambia la posición de la nave				// Change ship position
			angle += 4.0f;
		}
		
		// Movemos la nave y la cámara para navegar por el mapa
		// Move our ship and camera to explore the desert
		if(AMG_Button.Held &PSP_CTRL_DOWN){
			tie_fighter->Object[0].Pos.z += SPEED;
			cam->Pos.z += SPEED; cam->Eye.z += SPEED;
			tie_fighter->Object[0].Rot.y = 0.0f, tie_fighter->Object[0].Scale.z = SCALE;
		}else if(AMG_Button.Held &PSP_CTRL_UP){
			tie_fighter->Object[0].Pos.z -= SPEED;
			cam->Pos.z -= SPEED; cam->Eye.z -= SPEED;
			tie_fighter->Object[0].Rot.y = 0.0f, tie_fighter->Object[0].Scale.z = -SCALE;
		}else if(AMG_Button.Held &PSP_CTRL_RIGHT){
			tie_fighter->Object[0].Pos.x += SPEED;
			cam->Pos.x += SPEED; cam->Eye.x += SPEED;
			tie_fighter->Object[0].Rot.y = AMG_Deg2Rad(90.0f), tie_fighter->Object[0].Scale.z = SCALE;
		}else if(AMG_Button.Held &PSP_CTRL_LEFT){
			tie_fighter->Object[0].Pos.x -= SPEED;
			cam->Pos.x -= SPEED; cam->Eye.x -= SPEED;
			tie_fighter->Object[0].Rot.y = AMG_Deg2Rad(-90.0f), tie_fighter->Object[0].Scale.z = SCALE;
		}
		
		// Activamos la niebla (para dar mayor realismo)
		// Enable FOG (more realism)
		AMG_EnableFog(0.0f, 100.0f, GU_RGBA(0, 196, 0xFF, 0xFF));	// Inicio, Final, Color		// Start, End, Color
		AMG_SetCamera(cam);											// Posiciona la cámara		// Set the camera
		
		// Renderiza el mapa de alturas		// Render out heightmap
		AMG_RenderObject(hmap, 0);
		
		// Renderiza la nave				// Render our ship
		AMG_RenderModel(tie_fighter);
		
		AMG_OrthoMode(1);	// Modo 2D	// 2D mode
		sprintf(text, "~ Heightmaps Example ~\nFPS: %d", AMG.FPS);
		AMG_Printf(font, 0, 0, 0xFFFFFFFF, text);
		AMG_OrthoMode(0);	// Fin del modo 2D	// Finish 2D mode
		
		// Capturas de pantalla		// Screenshots
		if(AMG_Button.Down &PSP_CTRL_CROSS) AMG_Screenshot("Photo.png");
		
		AMG_Update3D();		// Actualiza el 3D		// Update 3D rendering
		AMG_UpdateTime();	// Actualiza el tiempo	// Update time
	}
	
	// Fin del programa		// Program end
	AMG_UnloadTexture(font);		// Elimina la fuente de texto	// Unload text font
	AMG_UnloadModel(tie_fighter);	// Elimina el modelo 3D			// Unload 3D model
	AMG_UnloadObject(hmap);			// Elimina el mapa de alturas	// Unload heightmap
	AMG_Finish3D();					// Acaba con el 3D				// Finish 3D
	return AMG_ReturnXMB();			// Ir al XMB					// Go to XMB
}
