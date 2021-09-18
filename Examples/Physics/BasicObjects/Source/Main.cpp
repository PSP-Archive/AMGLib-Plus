/*
 * AMGlib Template
 * Powered by Andresmargar
 * from SCENEBETA
 */
 
// Includes AMG
#include <AMG/AMGLib.h>

// Module info
AMG_MODULE_INFO("AMG_Template", AMG_USERMODE, 1, 1);

// Límites del mundo 3D		// World limits
ScePspFVector3 world[2] = {
	{-1000.0f, -1000.0f, -1000.0f},
	{1000.0f, 1000.0f, 1000.0f}
};

// Nuestro texto		// Our text
char text[256];

// Funcion Main()					// Main() function
// Bloque principal del programa	// Program entrypoint
int main(int argc, char **argv){

	// Inicializa la PSP
	AMG_SetupCallbacks();						// Inicializa los callbacks			// Setup callbacks
	AMG_Init3D(GU_PSM_5551 | AMG_DOUBLEBUFFER);	// Inicializa el motor 3D			// Init 3D engine
	AMG_InitTimeSystem();						// Inicializa el motor de tiempo	// Init time system
	AMG_InitBulletPhysics(world, 32);
	AMG.ClearColor = GU_RGBA(0x7F, 0x7F, 0x7F, 0xFF);
	
	// Carga la fuente de texto		// Load text font
	AMG_Texture *font = AMG_LoadTexture("Files/Font.png", AMG_TEX_RAM);	// Cargala en VRAM		// Load it in VRAM
	AMG_DeleteColor(font, GU_RGBA(0, 0, 0, 0xFF));			// Borra el color negro (transparente)	// Delete black color
	AMG_Create2dObject(font, GU_PSM_5551, true);			// Conviertela en un objeto 2D			// Create 2D Object
	AMG_SwizzleTexture(font);								// Aplicamos Swizzling (más velocidad)	// Swizzle (speed-up)
	
	// Carga el modelo 3D	// Load 3D model
	AMG_Model *scene = AMG_LoadModel("Files/scene.obj");
	
	// Posiciona los objetos 3D		// Set position for 3D objects
	AMG_ObjectConfPhysics(&scene->Object[0], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, AMG_BULLET_SHAPE_BOX);
	AMG_ObjectConfPhysics(&scene->Object[1], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, AMG_BULLET_SHAPE_BOX);
	AMG_ObjectConfPhysics(&scene->Object[2], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, AMG_BULLET_SHAPE_BOX);
	AMG_ObjectConfPhysics(&scene->Object[3], 0.0f, 0.0f, 0.0f, 2.5f, 0.0f, 0.0f, 0.0f, AMG_BULLET_SHAPE_BOX);
	AMG_ObjectConfPhysics(&scene->Object[4], 0.0f, 0.0f, 0.0f, -2.5f, 0.0f, 0.0f, 0.0f, AMG_BULLET_SHAPE_BOX);
	AMG_ObjectConfPhysics(&scene->Object[5], 0.0f, 0.0f, 0.0f, 0.0f, 2.5f, 0.0f, 0.0f, AMG_BULLET_SHAPE_BOX);
	AMG_ObjectConfPhysics(&scene->Object[6], 0.0f, 0.0f, 0.0f, 0.0f, -2.5f, 0.0f, 0.0f, AMG_BULLET_SHAPE_BOX);
	scene->Object[0].Scale.x = 0.5f; scene->Object[0].Scale.y = 0.5f; scene->Object[0].Scale.z = 0.5f; 
	scene->Object[1].Scale.x = 0.5f; scene->Object[1].Scale.y = 0.5f; scene->Object[1].Scale.z = 0.5f;
	scene->Object[2].Scale.x = 0.5f; scene->Object[2].Scale.y = 0.5f; scene->Object[2].Scale.z = 0.5f;
	
	// Inicializa las físicas en el modelo 3D	// Init physics for this model
	AMG_InitModelPhysics(scene);
	
	// Crea una camara y posiciónala		// Create a camera
	AMG_Camera *camera = AMG_InitCamera();
	camera->Pos.z = 7.0f;
	
	// Bucle infinito para mantener el programa funcionando
	// Infinite loop to keep the program running
	while(!AMG.Exit){
		
		// Comienza el dibujado 3D		// Begin 3D drawing
		AMG_Begin3D();
		
		// Lee el gamepad			// Read gamepad
		AMG_ReadButtons();
		
		// Posiciona la camara		// Set camera
		AMG_SetCamera(camera);
		
		// Cambios en la gravedad		// Change gravity
		if(AMG_Button.Down &PSP_CTRL_DOWN) AMG_SetWorldGravity(0, -9.8, 0);
		if(AMG_Button.Down &PSP_CTRL_UP) AMG_SetWorldGravity(0, 9.8, 0);
		if(AMG_Button.Down &PSP_CTRL_RIGHT) AMG_SetWorldGravity(9.8, 0, 0);
		if(AMG_Button.Down &PSP_CTRL_LEFT) AMG_SetWorldGravity(-9.8, 0, 0);
		
		// Renderiza el modelo 3D		// Render 3D model
		AMG_RenderModel(scene);
		
		// Selecciona el modo 2D		// 2D stuff
		AMG_OrthoMode(true);
		sprintf(text, "FPS: %d\nPress + to change gravity", AMG.FPS);
		AMG_Printf(font, 0, 0, GU_RGBA(rand()&0xFF, rand()&0xFF, rand()&0xFF, 0xFF), text);
		AMG_OrthoMode(false);
		
		// Capturas de pantalla		// Screenshots
		if(AMG_Button.Down &PSP_CTRL_CIRCLE) AMG_Screenshot("Photo.png");
		
		// Actualiza el 3D		// Update 3D
		AMG_Update3D();
		AMG_UpdateTime();	// Actualiza el tiempo		// Update time
		AMG_UpdateBulletPhysics();
	}
	
	// Fin del programa		// Program end
	AMG_UnloadModel(scene);			// Elimina el modelo 3D			// Delete 3D model
	AMG_UnloadTexture(font);		// Elimina la fuente de texto	// Delete text font
	AMG_Finish3D();					// Termina con el modo 3D		// Finish 3D engine
	return AMG_ReturnXMB();			// Vuelve al XMB				// Go back to XMB
}
