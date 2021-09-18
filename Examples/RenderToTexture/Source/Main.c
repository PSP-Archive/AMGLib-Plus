/*
 * AMGlib Template
 * Powered by Andresmargar
 * from SCENEBETA
 */
 
// Includes AMG
#include <AMG/AMGLib.h>

// Module info
AMG_MODULE_INFO("AMG_Celshading", AMG_USERMODE, 1, 1);

// Funcion Main()					// Main() function
// Bloque principal del programa	// Program entrypoint
int main(int argc, char **argv){

	// Inicializa la PSP
	AMG_SetupCallbacks();						// Inicializa los callbacks			// Setup callbacks
	AMG_Init3D(GU_PSM_5650 | AMG_DOUBLEBUFFER);	// Inicializa el motor 3D			// Init 3D engine
	AMG_InitTimeSystem();						// Inicializa el motor de tiempo	// Init time system
	
	// Carga la fuente de texto		// Load text font
	AMG_Texture *font = AMG_LoadTexture("Files/Font.png", AMG_TEX_RAM);	// Cargala en RAM						// Load it in RAM
	AMG_DeleteColor(font, GU_RGBA(0, 0, 0, 0xFF));			// Borra el color negro (transparente)				// Delete black color
	AMG_Create2dObject(font, GU_PSM_5551, 1);				// Conviertela en un objeto 2D (y pasala a VRAM)	// Create 2D Object
	AMG_SwizzleTexture(font);
	
	// Carga los modelos 3D		// Load 3D models
	AMG_Model *model[2];
	model[0] = AMG_LoadModel("Files/abeja.obj");
	model[1] = AMG_LoadModel("Files/SStars.obj");
	model[1]->Object[0].Group[0].Texture->TFX = GU_TFX_BLEND;
	
	// Crea las cámaras		// Create cameras
	AMG_Camera *camera1 = AMG_InitCamera();
	AMG_Camera *camera2 = AMG_InitCamera();
	
	// Crea 2 texturas vacías (aquí se renderizarán los 2 modelos)
	// Create 2 empty textures (where rendering will be performed)
	AMG_Texture *render[2];
	render[0] = AMG_CreateTexture(256, 256, GU_PSM_5650, AMG_TEX_VRAM);
	render[0]->X = 240; render[0]->Y = 136;
	render[1] = AMG_CreateTexture(256, 256, GU_PSM_5650, AMG_TEX_VRAM);
	render[1]->X = 240; render[1]->Y = 136;
	
	float angle = 0.0f;	// El ángulo de la animación		// Animation angle
	char text[256];		// Nuestro texto					// Text buffer
	
	// Bucle infinito para mantener el programa funcionando
	// Infinite loop to keep the program running
	while(!AMG.Exit){
		
		// Comienza el dibujado 3D		// Start 3D drawing
		AMG.ClearColor = GU_RGBA(0x7F, 0x7F, 0, 0xFF);
		AMG_Begin3D();
		
		// Lee el gamepad				// Read gamepad
		AMG_ReadButtons();
		
		/****************************************************/
		/*********** RENDERIZA EL PRIMER MODELO 3D **********/
		/*********** RENDER FIRST MODEL *********************/
		/****************************************************/
		
		// Activa el Render To Texture	// Enable Render To Texture
		AMG.ClearColor = GU_RGBA(0, 162, 232, 0xFF);
		AMG_EnableRenderToTexture(render[0]);
		
		// Posiciona la camara				// Set camera
		camera1->Pos.y = -11.0f;
		camera1->Pos.z = -9.0f;
		camera1->Pos.x = 4.0f;
		AMG_SetCamera(camera1);
		
		// Renderiza el modelo 3D			// Render 3D model
		model[0]->Object[0].Pos.x = -8.0f;
		model[0]->Object[0].Pos.z = 6.0f;
		model[0]->Object[0].Pos.y = 3.0f;
		model[0]->Object[0].Rot.y += AMG_Deg2Rad(1.0f);
		AMG_RenderModel(model[0]);
		
		/****************************************************/
		/*********** RENDERIZA EL SEGUNDO MODELO 3D *********/
		/*********** RENDER SECOND MODEL ********************/
		/****************************************************/
		
		// Activa el Render To Texture		// Enable Render To Texture
		AMG.ClearColor = GU_RGBA(0, 0, 0xFF, 0xFF);
		AMG_EnableRenderToTexture(render[1]);
		
		// Posiciona la camara				// Set camera
		camera2->Pos.y = 11.0f;
		camera2->Pos.z = -12.0f;
		camera2->Pos.x = 4.0f;
		AMG_SetCamera(camera2);
		
		// Renderiza el modelo 3D			// Render 3D model
		model[1]->Object[0].Pos.x = -8.0f;
		model[1]->Object[0].Pos.z = 6.0f;
		model[1]->Object[0].Pos.y = 0.0f;
		model[1]->Object[0].Scale.x = 0.8f;
		model[1]->Object[0].Scale.y = 0.8f;
		model[1]->Object[0].Scale.z = 0.8f;
		model[1]->Object[0].Rot.y += AMG_Deg2Rad(1.0f);
		AMG_RenderModel(model[1]);
		
		// Desactiva el Render To Texture		// Disable Render To Texture
		AMG_DisableRenderToTexture();
		
		// Selecciona el modo 2D				// 2D Mode
		AMG_OrthoMode(1);
			AMG_DrawSprite(render[0]);
			AMG_DrawSprite(render[1]);
			sprintf(text, "FPS: %d\n3D Models by Mills", AMG.FPS);
			AMG_Printf(font, 0, 0, GU_RGBA(rand()&0xFF, rand()&0xFF, rand()&0xFF, 0xFF), text);
		AMG_OrthoMode(0);
		
		// Animación de las imágenes			// Image animation
		render[0]->X = (AMG_Cos(AMG_Deg2Rad(angle)) * 200) + 240;
		render[0]->ScaleX = (AMG_Sin(AMG_Deg2Rad(angle)) + 1.0f)/2.0f; render[0]->ScaleY = render[0]->ScaleX;
		render[1]->X = (AMG_Cos(AMG_Deg2Rad(angle+180.0f)) * 200) + 240;
		render[1]->ScaleX = (AMG_Sin(AMG_Deg2Rad(angle+180.0f)) + 1.0f)/2.0f; render[1]->ScaleY = render[1]->ScaleX;
		angle ++;
		
		// Capturas de pantalla					// Screenshots
		if(AMG_Button.Down &PSP_CTRL_CIRCLE) AMG_Screenshot("Photo.png");
		
		// Actualiza el 3D		// Update 3D
		AMG_Update3D();
		AMG_UpdateTime();	// Actualiza el tiempo	// Update time system
	}
	
	// Fin del programa			// Program end
	AMG_UnloadModel(model[0]);
	AMG_UnloadModel(model[1]);		// Elimina el modelo 3D				// Delete 3D model
	AMG_UnloadTexture(font);		// Elimina la fuente de texto		// Delete textfont
	AMG_UnloadTexture(render[0]);
	AMG_UnloadTexture(render[1]);
	AMG_Finish3D();					// Termina con el modo 3D			// Finish 3D engine
	return AMG_ReturnXMB();			// Vuelve al XMB					// Go back to XMB
}
