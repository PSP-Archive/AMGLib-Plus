/*
 * AMGlib Template
 * Powered by Andresmargar
 * from SCENEBETA
 */
 
// Includes AMG
#include <AMG/AMGLib.h>
#include <oslib/oslib.h>

// Comprueba que usamos la versión adecuada		// Check for version
#ifndef AMG_COMPAT_OSLIB
#error "AMGLib is not compiled with OSLIB"
#endif

// Module info
AMG_MODULE_INFO("AMGLib + OSLib", AMG_USERMODE, 1, 0);

// Variables
u8 fps = 0, frames = 0;
u64 prev, now, res;

// Atributos del modelo 3D		// 3D Model attributes
ScePspFVector3 pos[3], rot[3], scl[3];
float a = 0.0f;		// Variable de la animación		// Animation variable

// Defines
#define DISTANCE 6.0f	// Distancia entre los modelos 3D	// Distance among 3D models

// Valor absoluto	// Absolute value
#define absf(v) (((v) >= 0.0f) ? (v) : -(v))

// Contador de FPS		// FPS Counter
void fps__(void){
	frames ++;
	sceRtcGetCurrentTick(&now);
	if(((now - prev)/((float)res)) >= 1.0f){
		prev = now;
		fps = frames;
		frames = 0;
	}
}

// Screen.flip
void flip(void){
	oslEndDrawing();
    oslSyncFrame();
    oslStartDrawing();
    //oslCls();
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT | GU_STENCIL_BUFFER_BIT);
}

// Buffer de texto		// Text buffer
char text[256];

// Funcion Main()					// Main() function
// Bloque principal del programa	// Program entrypoint
int main(int argc, char **argv){

	// Inicializa OSLIB		// Init OSLIB
	oslInit(0);
	oslInitGfx(OSL_PF_5650, 1);
	oslSetKeyAutorepeatInit(40);
    oslSetKeyAutorepeatInterval(10);
	oslIntraFontInit(INTRAFONT_CACHE_MED);	// Inicializa Intrafont		// Inits Intrafont
	
	// Inicia el contador de FPS		// Init FPS counter
	sceRtcGetCurrentTick(&prev);
	res = sceRtcGetTickResolution();
	
	// Carga la fuente PGF		// Load PGF font
	OSL_FONT *pgfFont = oslLoadFontFile("ltn0.pgf");
    oslIntraFontSetStyle(pgfFont, 0.5f, RGBA(255,255,255,255), RGBA(0,0,0,0), INTRAFONT_ALIGN_LEFT);
    oslSetFont(pgfFont);
	
	// Fondo PNG	// PNG background
	sprintf(text, "bg.png");
	OSL_IMAGE *bg = oslLoadImageFilePNG(text, OSL_IN_RAM | OSL_SWIZZLED, OSL_PF_5650);
	
	// Inicia AMGLib	// Init AMGLib
	AMG_Init3D(GU_PSM_5650 | AMG_DOUBLEBUFFER);	// Inicializa el motor 3D	// Init 3D engine
	
	// Carga el modelo 3D
	AMG.TextureQuality = GU_PSM_5551;	// Establece la calidad de las texturas		// Set Texture quality
	AMG_Model *tie_fighter = AMG_LoadModel("Tie-Fighter.obj");
	tie_fighter->CelShading = true;		// Activa CelShading						// Enable Cel-Shading
	
	// Crea una cámara		// Create a camera
	AMG_Camera *camera = AMG_InitCamera();
	camera->Pos.z = 9.0f;
	camera->Eye.z = 8.0f;
	
	// Inicia la posición, rotación y escalado		// Init position, rotation and scale
	for(u8 i=0;i<3;i++){
		pos[i].y = pos[i].z = rot[i].x = rot[i].y = rot[i].z = 0.0f;
		scl[i].x = scl[i].y = scl[i].z = 0.5f;
		pos[i].x = -DISTANCE + (DISTANCE * i);
	}
	
	// Bucle infinito para mantener el programa funcionando
	// Infinite loop to keep the program running
	while(!osl_quit){
		
		// Comienza el dibujado 3D			// Start 3D drawing
		AMG_Begin3D();
		
		// Dibujamos el fondo al principio	// Draw background
		AMG_OrthoMode(1);
		oslDrawImage(bg);
		AMG_OrthoMode(0);
		
		// Lee el gamepad	// Read gamepad
		oslReadKeys();
		
		// Posiciona la camara		// Set camera
		AMG_SetCamera(camera);
		
		// Mueve los modelos 3D		// Move 3D models
		pos[0].y = AMG_Sin(AMG_Deg2Rad(a));
		pos[0].z = -pos[0].y;
		rot[1].x = rot[1].y = rot[1].z = AMG_Deg2Rad(a);
		scl[2].x = scl[2].y = scl[2].z = absf(AMG_Cos(AMG_Deg2Rad(a)) * 0.5f);
		a += 1.0f;
		
		// Renderiza los modelos 3D		// Render 3D models
		for(u8 i=0;i<3;i++){
			AMG_Object *o = &tie_fighter->Object[0];
			o->Pos.x = pos[i].x;
			o->Pos.y = pos[i].y;
			o->Pos.z = pos[i].z;
			o->Rot.x = rot[i].x;
			o->Rot.y = rot[i].y;
			o->Rot.z = rot[i].z;
			o->Scale.x = scl[i].x;
			o->Scale.y = scl[i].y;
			o->Scale.z = scl[i].z;
			AMG_RenderModel(tie_fighter);
		}
		
		// Selecciona el modo 2D	// 2D mode
		AMG_OrthoMode(true);
		sprintf(text, "FPS: %d", fps);
		oslDrawString(0, 0, text); sceGuDisable(GU_DEPTH_TEST);
        oslDrawString(136, 250, "OSLib + AMGLib is POSSIBLE"); sceGuDisable(GU_DEPTH_TEST);
		AMG_OrthoMode(false);
		
		// Actualiza el 3D	// Update 3D
		AMG_Update3D();
		flip();
		fps__();
	}
	
	// Fin del programa		// Program ending
	AMG_UnloadModel(tie_fighter);	// Elimina el modelo 3D			// Unload the 3D model
	AMG_Finish3D();				// Termina con el modo 3D			// Finish with 3D
	oslQuit();
	return 0;
}
