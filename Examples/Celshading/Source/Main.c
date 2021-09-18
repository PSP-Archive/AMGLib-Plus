/*
 * AMGlib Template
 * Powered by Andresmargar
 * from SCENEBETA
 */
 
// Includes AMG
#include <AMG/AMGLib.h>

// Module info
AMG_MODULE_INFO("AMG_Celshading", AMG_USERMODE, 1, 1);

// Buffer de texto	// Text buffer
char text[256];

// Funcion Main()					// Main() function
// Bloque principal del programa	// Program entrypoint
int main(int argc, char **argv){

	// Inicializa la PSP
	AMG_SetupCallbacks();						// Inicializa los callbacks			// Init callbacks for homebutton
	AMG_Init3D(GU_PSM_5650 | AMG_DOUBLEBUFFER);	// Inicializa el motor 3D			// Init 3D engine
	AMG_InitTimeSystem();						// Inicializa el motor de tiempo	// Init time system
	
	// Carga la fuente de texto		// Load a text font
	AMG_Texture *font = AMG_LoadTexture("Files/Font.png", AMG_TEX_RAM);	// Cargala en RAM						// Load it in RAM
	AMG_DeleteColor(font, GU_RGBA(0, 0, 0, 0xFF));						// Borra el color negro (transparente)	// Make the black colour transparent
	AMG_Create2dObject(font, GU_PSM_5551, 0);							// Conviertela en un objeto 2D			// Convert it into a 2D object
	AMG_SwizzleTexture(font);
	
	// Carga el modelo 3D	// Load the 3D model
	AMG.TextureSwizzle = 1;
	AMG.TextureQuality = GU_PSM_5551;	// Establece la calidad de las texturas		// Set Texture Quality
	AMG_Model *tie_fighter = AMG_LoadModel("Files/Tie-Fighter.obj");
	tie_fighter->CelShading = 1;		// Activa CelShading	// Enable Cel-Shading
	
	// Crea una cámara	// Create a custom camera
	AMG_Camera *camera = AMG_InitCamera();
	camera->Pos.z = 9.0f;
	
	// Configura la luz 0	// Configurates light 0
	AMG_Light[0].Type = GU_DIRECTIONAL;
	
	// Variables adicionales	// Additional stuff
	float alpha = 1.0f;			// Variable para controlar la transparencia del modelo	// Variable used to control model alpha-component
	float angle = 0.0f;			// El ángulo de rotación de la luz						// Light rotation angle
	
	// Bucle infinito para mantener el programa funcionando
	// Infinite loop to keep the program running
	while(!AMG.Exit){
		
		// Comienza el dibujado 3D
		// Begin the 3D drawing
		AMG_Begin3D();
		
		// Lee el gamepad	// Read gamepad data
		AMG_ReadButtons();
		
		// Posiciona la camara	// Set the camera we created before
		AMG_SetCamera(camera);
		
		// Activa la luz 0		// Enable light 0
		AMG_EnableLight(0);
		AMG_Light[0].Pos.x = AMG_Cos(angle) * 6.0f;
		AMG_Light[0].Pos.y = AMG_Sin(angle) * 6.0f;
		angle += AMG_Deg2Rad(1.0f);
		
		// Rotacion del modelo 3D	// 3D model rotation
		if(AMG_Button.Held &PSP_CTRL_UP) tie_fighter->Object[0].Rot.x -= AMG_Deg2Rad(1.0f);
		if(AMG_Button.Held &PSP_CTRL_DOWN) tie_fighter->Object[0].Rot.x += AMG_Deg2Rad(1.0f);
		if(AMG_Button.Held &PSP_CTRL_RIGHT) tie_fighter->Object[0].Rot.y += AMG_Deg2Rad(1.0f);
		if(AMG_Button.Held &PSP_CTRL_LEFT) tie_fighter->Object[0].Rot.y -= AMG_Deg2Rad(1.0f);
		
		// Cambiamos el Alpha	// Time to change alpha-component
		if(AMG_Button.Held &PSP_CTRL_RTRIGGER) alpha += 0.05f;
		else if(AMG_Button.Held &PSP_CTRL_LTRIGGER) alpha -= 0.05f;
		if(alpha > 1.0f) alpha = 1.0f;
		else if(alpha < 0.0f) alpha = 0.0f;
		tie_fighter->Object[0].Group[0].Diffuse = GU_COLOR(1.0f, 1.0f, 1.0f, alpha);		// Actualizamos el Alpha en el modelo	// Update alpha
		
		// Renderiza el modelo 3D	// Render 3D model
		AMG_RenderModel(tie_fighter);
		
		// Selecciona el modo 2D
		// Set-up 2D mode
		AMG_OrthoMode(1);
		sprintf(text, "~ Cel-Shading Example ~\nFPS: %d", AMG.FPS);
		AMG_Printf(font, 0, 0, GU_RGBA(rand()&0xFF, rand()&0xFF, rand()&0xFF, 0xFF), text);
		AMG_OrthoMode(0);
		
		// Capturas de pantalla
		// Take a screenshot if neccesary
		if(AMG_Button.Down &PSP_CTRL_CIRCLE) AMG_Screenshot("Photo.png");
		
		// Actualiza el 3D	// Update 3D rendering
		AMG_Update3D();
		AMG_UpdateTime();	// Actualiza el tiempo	// Update time
	}
	
	// Fin del programa		// Program ending
	AMG_UnloadModel(tie_fighter);	// Elimina el modelo 3D			// Unload the 3D model
	AMG_UnloadTexture(font);		// Elimina la fuente de texto	// Unload text font
	AMG_Finish3D();				// Termina con el modo 3D			// Finish with 3D
	return AMG_ReturnXMB();		// Vuelve al XMB					// And go back to XMB
}
