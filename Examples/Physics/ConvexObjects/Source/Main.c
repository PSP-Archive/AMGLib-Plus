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

// Cabeceras de funciones		// Function headers
void makeCopies(AMG_Object *obj, AMG_Object *dest, u8 ncopies);

// Funcion Main()					// Main() function
// Bloque principal del programa	// Program entrypoint
int main(int argc, char **argv){

	// Inicializa la PSP		// Init PSP
	AMG_SetupCallbacks();							// Inicializa los callbacks		// Setup callbacks
	AMG_Init3D(GU_PSM_5650 | AMG_DOUBLEBUFFER);		// Inicializa el motor 3D		// Init 3D Engine
	AMG_InitTimeSystem();							// Inicializa el tiempo			// Init time system
	
	// Carga la fuente de texto		// Load text font
	AMG_Texture *font = AMG_LoadTexture("Files/Font.png", AMG_TEX_RAM);	// Cargala en VRAM		// Load it in VRAM
	AMG_DeleteColor(font, GU_RGBA(0, 0, 0, 0xFF));			// Borra el color negro (transparente)	// Delete black color
	AMG_Create2dObject(font, GU_PSM_5551, 1);				// Conviertela en un objeto 2D			// Create 2D Object
	AMG_SwizzleTexture(font);								// Aplicamos Swizzling (más velocidad)	// Swizzle (speed-up)
	
	// Ponemos un contexto			// A context ^^
	AMG_MessageBox(AMG_MESSAGE_STRING, 0, 0, "Once upon a time...\nA little child was so angry\nAnd this is the result...");
	
	// Inicializa el motor Bullet		// Init Bullet Engine
	AMG_InitBulletPhysics(world, 32);
	
	// Establece opciones de texturas		// Set texture options
	AMG.TextureQuality = GU_PSM_5650;
	AMG.TextureSwizzle = 1;
	AMG.TextureDest = AMG_TEX_VRAM;
	
	// Carga los modelos 3D				// Load 3D models
	AMG_Model *scene = AMG_LoadModel("Files/lego.obj");
	
	// Configura las físicas de la escena		// Configurate scene physics
	u8 i;
	for(i=0;i<(scene->NObjects-1);i++){
		AMG_Object *o = &scene->Object[i];
		AMG_ObjectConfPhysics(o, o->Centre.x, o->Centre.y, o->Centre.z, 0.0f, 10.0f, 0.0f, 3.5f, AMG_BULLET_SHAPE_CONVEXHULL);
	}
	AMG_ObjectConfPhysics(&scene->Object[8], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, AMG_BULLET_SHAPE_CONVEXHULL);
	
	// Inicializa las físicas de la escena	// Init scene physics
	AMG_InitModelPhysics(scene);
	
	// Crea una cámara					// Create a camera
	AMG_Camera *cam = AMG_InitCamera();
	cam->Pos.y = 10.0f;
	cam->Pos.z = 8.0f;
	
	// Color de fondo gris		// Grey clear color
	AMG.ClearColor = GU_RGBA(0x7F, 0x7F, 0x7F, 0xFF);
	
	// Nuestro buffer de texto
	char text[256];
	
	// Bucle infinito para mantener el programa funcionando
	// Infinite loop to keep the program running
	while(!AMG.Exit){
		AMG_Begin3D();		// Comienza el dibujado 3D		// Begin 3D rendering
		AMG_ReadButtons();	// Lee el gamepad				// Read gamepad
		
		// Posiciona la cámara		// Set camera
		AMG_SetCamera(cam);
		
		// Rota el suelo 			// Rotate floor
		scene->Object[8].Rot.x += AMG_Deg2Rad(((float)AMG_Button.JoyY / 64.0f));
		scene->Object[8].Rot.y += AMG_Deg2Rad(((float)AMG_Button.JoyX / 64.0f));
		
		// Renderiza la escena		// Render scene
		AMG_RenderModel(scene);
		
		// Selecciona el modo 2D		// 2D stuff
		AMG_OrthoMode(1);
		sprintf(text, "~ Convex Hull Example ~\nFPS: %d", AMG.FPS);
		AMG_Printf(font, 0, 0, GU_RGBA(rand()&0xFF, rand()&0xFF, rand()&0xFF, 0xFF), text);
		AMG_OrthoMode(0);
		
		AMG_Update3D();				// Actualiza el 3D				// Update 3D
		AMG_UpdateTime();			// Actualiza el tiempo			// Update Time
		AMG_UpdateBulletPhysics();	// Actualiza las físicas	// Update physics
	}
	
	// Fin del programa		// Program end
	AMG_UnloadTexture(font);	// Elimina la fuente de texto	// Delete text font
	AMG_UnloadModel(scene);		// Elimina el suelo				// Delete floor
	AMG_Finish3D();				// Termina con el 3D	// Finish 3D engine
	return AMG_ReturnXMB();		// Vuelve al XMB		// Go back to XMB
}
