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
#define LIGHT_DISTANCE 2.5f

// Renderiza el lego		// Render lego
void render_lego(const AMG_Model *model){
	for(u8 k=0;k<model->NObjects;k++){
		if(k != 8) AMG_RenderObject(&model->Object[k], model->CelShading);
	}
}

// Renderiza la sombra del lego			// Render lego shadows
void render_lego_shadow(const AMG_Model *model, u8 light, ScePspFVector4 *plane){
	for(u8 k=0;k<model->NObjects;k++){
		if(k != 8) AMG_RenderShadow(&model->Object[k], light, plane);
	}
}

// Funcion Main()					// Main() function
// Bloque principal del programa	// Program entrypoint
int main(int argc, char **argv){
	
	// Inicializa la PSP		// Init PSP
	AMG_SetupCallbacks();						// Inicializa los callbacks		// Setup callbacks
	AMG_Init3D(GU_PSM_8888 | AMG_DOUBLEBUFFER);	// Inicializa el motor 3D (El Stencil Buffer solo funciona con esta configuración)
												// Init 3D Engine (Stencil Buffer only works this way)
	AMG_InitTimeSystem();						// Inicializa el motor de tiempo	// Init time system
	
	// Carga la fuente de texto		// Load text font
	AMG_Texture *font = AMG_LoadTexture("Files/Font.png", AMG_TEX_RAM);	// Cárgala en VRAM						// Load it in VRAM
	AMG_DeleteColor(font, GU_RGBA(0, 0, 0, 0xFF));						// Borra el color negro (transparente)	// Delete black color
	AMG_Create2dObject(font, GU_PSM_5551, 1);							// Conviertela en un objeto 2D			// Create 2D object
	
	// Carga el modelo 3D			// Load 3D model
	AMG.TextureQuality = GU_PSM_5650;
	AMG.TextureSwizzle = true;
	AMG_Model *lego = AMG_LoadModel("Files/lego.obj");
	
	// Quitamos el outline del suelo		// Delete the outline
	lego->Object[8].Lighting = false;		// A este objeto no le afecta la iluminación	// This object isn't affected by lighting
	
	// Crea una cámara y posiciónala		// Create a camera
	AMG_Camera *camera = AMG_InitCamera();
	camera->Pos.z = 8.0f;
	camera->Pos.y = 10.0f;
	
	// Inicializa las luces		// Init lights
	AMG_Light[0].Type = GU_DIRECTIONAL;
	AMG_Light[0].Pos.x = 0.0f;
	AMG_Light[0].Pos.y = 3.0f;
	AMG_Light[0].Pos.z = 0.0f;
	AMG_Light[1].Type = GU_DIRECTIONAL;
	AMG_Light[1].Pos.x = 0.0f;
	AMG_Light[1].Pos.y = 3.0f;
	AMG_Light[1].Pos.z = 0.0f;
	float a = 0.0f;		// Ángulo de rotación de las luces	// Lights rotation angle
	
	// Variables adicionales		// Additional stuff
	ScePspFVector3 *tp = (ScePspFVector3*) calloc (3, sizeof(ScePspFVector3));	// Los puntos del suelo		// Floor points
	ScePspFVector4 plane;				// La ecuación del plano	// Plane equation
	char text[256];						// El texto					// Text buffer
	
	// Bucle infinito para mantener el programa funcionando
	// Infinite loop to keep the program running
	while(!AMG.Exit){
		
		// Comienza el dibujado 3D		// Begin 3D drawing
		AMG_Begin3D();
		
		// Lee el gamepad				// Read gamepad data
		AMG_ReadButtons();
		
		// Rotamos el suelo				// Rotate floor
		lego->Object[8].Rot.x += AMG_Deg2Rad(((float)AMG_Button.JoyY / 64.0f));
		lego->Object[8].Rot.y += AMG_Deg2Rad(((float)AMG_Button.JoyX / 64.0f));
		
		// Posiciona la cámara			// Set camera
		AMG_SetCamera(camera);
		
		// Posiciona las luces			// Enable lights
		AMG_EnableLight(0);
		AMG_EnableLight(1);
		
		// Genera los puntos del suelo		// Generate floor points
		AMG_GenerateFloorPoints(&lego->Object[8], tp);
		
		// Calcula la ecuación del plano (damos 3 puntos del suelo, y sacamos su vector normal)
		// Calculate the plane equation (with 3 floor points given, we get its normal vector)
		AMG_PlaneEquation(tp, &plane);
		
		// Mueve las luces		// Lights movement
		AMG_Light[0].Pos.x = (AMG_Cos(a) * LIGHT_DISTANCE);
		AMG_Light[0].Pos.z = (AMG_Sin(a) * LIGHT_DISTANCE);
		AMG_Light[1].Pos.x = -(AMG_Cos(a) * LIGHT_DISTANCE);
		AMG_Light[1].Pos.z = -(AMG_Sin(a) * LIGHT_DISTANCE);
		a += AMG_Deg2Rad(1.0f);
		
		// Renderiza el modelo 3D (solo el lego, no el suelo)
		// Render 3D models (only lego, not floor)
		render_lego(lego);
		
		// Renderiza la sombra		// Render shadows
		AMG_StartReflection(&lego->Object[8]);		// Primero iniciamos el motor de reflejos (Stencil Buffer), especificando el objeto que hará de suelo
													// First of all, init the Stencil Buffer engine, setting the floor object
		render_lego_shadow(lego, 0, &plane);		// Renderizamos las sombras con la luz 0 (dando la ecuación del plano)
													// Render light 0 shadows (giving the plane equation)
		render_lego_shadow(lego, 1, &plane);		// Y con la luz 1				// And light 1
		AMG_FinishReflection();						// Terminamos con el motor de Stencil Buffer
													// Finish Stencil Buffer engine
		
		// Selecciona el modo 2D		// 2D stuff
		AMG_OrthoMode(1);
		sprintf(text, "~ Lights Example ~\nFPS: %d\nVertices: %d", AMG.FPS, (int)AMG.DrawnVertices);
		AMG_Printf(font, 0, 0, GU_RGBA(rand()&0xFF, rand()&0xFF, rand()&0xFF, 0xFF), text);
		AMG_OrthoMode(0);
		
		// Capturas de pantalla			// Screenshots
		if(AMG_Button.Down &PSP_CTRL_CIRCLE) AMG_Screenshot("Photo.png");
		
		// Actualiza el 3D				// Update 3D
		AMG_Update3D();
		AMG_UpdateTime();	// Actualiza el tiempo		// Update time
	}
	
	// Fin del programa		// Program end
	AMG_UnloadModel(lego);			// Elimina el modelo 3D			// Delete 3D model
	AMG_UnloadTexture(font);		// Elimina la fuente de texto	// Delete textfont
	AMG_Finish3D();					// Termina con el modo 3D		// Finish 3D engine
	return AMG_ReturnXMB();			// Vuelve al XMB				// Go back to XMB
}
