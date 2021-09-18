/*
 * AMGlib Template
 * Powered by Andresmargar
 * from SCENEBETA
 */

// Includes AMG
#include <AMG/AMGLib.h>

// Module info
AMG_MODULE_INFO("AMG_Model", 0, 1, 1);

// Variables adicionales	// Additional variables
float Angle1 = 0;
float SCALING = 1;
float SCALING1 = 1;
int Ob = 0;
float ALT = -15.0;
float CAM = 28;
char text[256];		// Para el texto	// Text buffer

// Funcion Main()					// Main() function
// Bloque principal del programa	// Program's entrypoint
int main(int argc, char **argv){
	
	// Inicializa la PSP	// Inits PSP
	AMG_SetupCallbacks();			// Inicializa los callbacks	// Init homebutton callbacks
	AMG_Init3D(GU_PSM_5650 | AMG_DOUBLEBUFFER);	// Inicializa el motor 3D	// Init 3D engine
	AMG_InitTimeSystem();
	
	// Carga la fuente de texto		// Load a text font
	AMG_Texture *font = AMG_LoadTexture("Files/font.png", AMG_TEX_RAM);
	AMG_DeleteColor(font, 0xFF000000);
	AMG_Create2dObject(font, GU_PSM_5551, 1);		// Cárgala en VRAM	// Load it in VRAM
	AMG_SwizzleTexture(font);						// Aplica Swizzling	// Swizzle it
	
	// Carga los modelos 3D (el planeta contiene muchos dentro, se exportó usando Blender)
	// Load Models (Planet model contains a lot of models inside, it was exported using Blender)
	AMG_Model *Planet = AMG_LoadModel("Files/Worms3D_Planet.obj");
	AMG_Texture *Brillo = AMG_LoadTexture("Files/Brillo.png", AMG_TEX_VRAM);
	// Establece el env_map		// Set env_map
	AMG_SetTextureMapping(Brillo, GU_ENVIRONMENT_MAP, 1, 2);
	// Establece el brillo como multitextura
	// Set second texture on top of the default to sub-objects
	AMG_SetObjectMultiTexture(&Planet->Object[2].Group[0], Brillo);
	AMG_SetObjectMultiTexture(&Planet->Object[11].Group[0], Brillo);
	AMG_SetObjectMultiTexture(&Planet->Object[16].Group[0], Brillo);
	AMG_SetObjectMultiTexture(&Planet->Object[20].Group[0], Brillo);
	AMG_SetObjectMultiTexture(&Planet->Object[23].Group[0], Brillo);
	AMG_SetObjectMultiTexture(&Planet->Object[25].Group[0], Brillo);
	AMG_SetObjectMultiTexture(&Planet->Object[26].Group[0], Brillo);
	AMG_SetObjectMultiTexture(&Planet->Object[31].Group[0], Brillo);

	// Esfera de fondo	// Background sphere
    AMG_Model *Space = AMG_LoadModel("Files/BKG.obj");
	// Desactiva la luz para este objeto	// Disable Light
	Space->Object[0].Lighting = 0;
	
	/** CONFIGURA LA LUZ 0 / CONFIGURATE LIGHT 0 **/
	
	// Tipo de luz	// Light type
    AMG_Light[0].Type = GU_DIRECTIONAL;
	// Parámetros	// Light parameters
	AMG_Light[0].Diffuse = 0xFFFFFF;
	AMG_Light[0].Ambient = 0x777777;
	// Posición de la luz (vector director)	// Light position (direction vector)
	AMG_Light[0].Pos.x = 4.0f;
	AMG_Light[0].Pos.y = 4.0f;
	AMG_Light[0].Pos.z = 4.0f;
	// Inicializa una cámara	// Set a camera
	AMG_Camera *camera = AMG_InitCamera();
	camera->Pos.z = 35.0f;
	camera->Pos.y = 0.0f;
	camera->Pos.x = 0.0f;
	camera->Rot.x = AMG_Deg2Rad(0.0f);

	// Configura la proyección	// Fix camera projection
    AMG_SetPerspectiveFOV(36.0f);

	// Bucle infinito	// Infinite loop
	while(!AMG.Exit){
		AMG_Begin3D();		// Comienza el dibujado 3D	// Start 3D drawing
		AMG_ReadButtons();	// Lee el gamepad	// Read gamepad data
        
		// Calcula rotación y escalado		// Calculate rotation and scale
        SCALING += AMG_Sin(Angle1) * 0.004f;
		SCALING1 += AMG_Cos(Angle1) * 0.008f;
		Angle1 += 0.06f;
		
		// Escala y rota el planeta			// Scale rotate Planet
		for (Ob = 0; Ob < Planet->NObjects; Ob++){
		    Planet->Object[Ob].Pos.y = 0.0;
    		Planet->Object[Ob].Rot.y += 0.02;
			//Planet->Object[Ob].Scale.x = SCALING;
			Planet->Object[Ob].Scale.y = SCALING1;
		}
		
		// Rota la Skybox	// Rotate Skybox
		Space->Object[0].Rot.y += 0.006;
		
		// Establece la cámara	// Set camera
		AMG_SetCamera(camera);
		
		AMG_EnableLight(0);		// Activa luz 0		// Enable light 0
		
        AMG_RenderModel(Planet);	// Renderiza el planeta		// Render planet
        AMG_RenderModel(Space);	// Renderiza el espacio		// Render space
	
		AMG_DisableLight(0);	// Desactiva luz 0	// Disable light 0	
		
		// Modo 2D	// 2D mode
		AMG_OrthoMode(1);
		sprintf(text, "~ MultiTexturing example ~\nBy Mills [%d FPS]\nModels from Worms 3D", AMG.FPS);
		AMG_Printf(font, 0, 0, 0xFFFFFFFF, text);
		AMG_OrthoMode(0);
		
		// Screenshot
		if(AMG_Button.Down &PSP_CTRL_CIRCLE) AMG_Screenshot("Photo.png");
		
		// Actualiza el 3D		// Update 3D
		AMG_Update3D();
		AMG_UpdateTime();
	}
	
	// Fin del programa, eliminar recursos
	// End of program
	AMG_UnloadModel(Planet);
	AMG_UnloadModel(Space);
	AMG_UnloadTexture(font);
	AMG_Finish3D();				// Termina con el 3D	// Finish 3D
	return AMG_ReturnXMB();		// Vuelve al XMB		// Go to XMB
}
