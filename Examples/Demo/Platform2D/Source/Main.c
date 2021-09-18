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
#define GRAVITY 4			// La gravedad del mundo 2D		// World gravity
#define SPEED_X 4			// La velocidad del personaje	// Player speed
#define JUMP_POWER 24		// La potencia del salto		// Jump power

// Define los datos a usar	// Data
AMG_Texture *mario;		// Nuestro personaje	// Our player
AMG_Texture *level[7];	// Nuestro nivel (había que dividirlo en partes, la PSP solo soporta hasta 512x512)
						// Our level (divided in parts, PSP only supports until 512x512)

// Variables
u8 touchingFloor = 0;	// Tocando el suelo?	// Touching floor?
u8 jumping = 0;			// Saltando?			// Jumping?
char text[256];			// Para el texto		// Text buffer

// Funciones	// Functions
u8 *loadCollisionMap(const char *path);
u8 getCollisionMapPoint(u8 *cmap, u32 x, u32 y);
void moveMario(AMG_Texture *m, int *cam_x);
void moveLevel(int cam_x);
void processColisions(u8 *cmap, AMG_Texture *p, int *mapx, int *mapy);

// Funcion Main()					// Main() function
// Bloque principal del programa	// Program entrypoint
int main(int argc, char **argv){

	// Inicializa la PSP	// Init PSP
	AMG_SetupCallbacks();							// Inicializa los callbacks		// Init callbacks
	AMG_Init3D(GU_PSM_5650 | AMG_DOUBLEBUFFER);		// Inicializa el motor gráfico	// Init 3D Engine
	AMG_InitTimeSystem();							// Inicializa el tiempo			// Init time system
	
	// Carga a mario	// Load mario
	mario = AMG_LoadTexture("Data/mario.png", AMG_TEX_RAM);	// Cargamos la textura					// Load it as a texture
	AMG_DeleteColor(mario, 0xFFFF00FF);						// Borramos el color magenta (0xFF00FF)	// Delete magenta color (0xFF00FF)
	AMG_Create2dObject(mario, GU_PSM_5551, 1);				// La convertimos en objeto 2D			// Create a 2D object
	AMG_TextureFrames(mario, 3);							// Decimos los frames que tiene			// It has 3 frames
	mario->X = 240; mario->Y = -128;						// Posicionamos el personaje			// Initial position
	
	// Carga el nivel	// Load level
	level[0] = AMG_LoadTexture("Data/back.png", AMG_TEX_RAM);
	level[1] = AMG_LoadTexture("Data/hills_0.png", AMG_TEX_RAM);
	level[2] = AMG_LoadTexture("Data/hills_1.png", AMG_TEX_RAM);
	level[3] = AMG_LoadTexture("Data/world_0.png", AMG_TEX_RAM);
	level[4] = AMG_LoadTexture("Data/world_1.png", AMG_TEX_RAM);
	level[5] = AMG_LoadTexture("Data/world_2.png", AMG_TEX_RAM);
	level[6] = AMG_LoadTexture("Data/world_3.png", AMG_TEX_RAM);
	u8 i;
	for(i=0;i<7;i++){
		AMG_DeleteColor(level[i], 0xFFFF00FF);					// Borra el color magenta	// Delete magenta color
		AMG_Create2dObject(level[i], GU_PSM_5551, 0);			// Crea el objeto 2D		// Create 2D object
		AMG_SwizzleTexture(level[i]);							// Haz Swizzle (esto incrementa la velocidad de renderizado, recomendable
																// 				usarlo con texturas cargadas en RAM)
																// Swizzle (this speeds-up rendering speed, use it when textures are loaded in RAM)
	}
	
	// Posiciona las capas del nivel		// Level layers' position
	level[0]->X = 240; level[0]->Y = 136;
	level[1]->X = 240; level[1]->Y = 200;
	level[2]->X = 752; level[2]->Y = 200;
	level[3]->X = 240; level[3]->Y = 136;
	level[4]->X = 752; level[4]->Y = 136;
	level[5]->X = 1264; level[5]->Y = 136;
	level[6]->X = 1776; level[6]->Y = 136;
	
	// Definimos una cámara 2D		// Define a 2D camera
	int cam_x = (int)mario->X, cam_y = (int)mario->Y;
	
	// Carga el mapa de colisiones	// Load a collision map
	u8 *colmap = loadCollisionMap("data/colmap.raw");
	if(colmap == NULL) AMG_Error(AMG_OPEN_FILE, 0, "data/colmap.raw");
	
	// Cargamos la fuente de texto	// Load a text font
	AMG_Texture *font = AMG_LoadTexture("data/font.png", AMG_TEX_RAM);
	AMG_DeleteColor(font, 0xFF000000);
	AMG_Create2dObject(font, GU_PSM_5551, 1);
	
	// Bucle infinito para mantener el programa funcionando
	// Infinite loop to keep the program running
	while(!AMG.Exit){
		AMG_Begin3D();		// Comienza el dibujado 3D	// Start 3D drawing
		AMG_ReadButtons();	// Lee el Gamepad			// Read gamepad
		AMG_OrthoMode(1);	// Activa el modo 2D		// Enable 2D mode
		
		// Procesamos el fondo	// Process background
		moveLevel(cam_x);
		
		// Procesamos el personaje	// Process player
		AMG_DrawSprite(mario);
		moveMario(mario, &cam_x);
		
		// Procesamos las colisiones	// Process collisions
		processColisions(colmap, mario, &cam_x, &cam_y);
		
		// Dibujamos el texto	// Draw text
		sprintf(text, "~ Platform Game Sample ~\nFPS: %d", AMG.FPS);
		AMG_Printf(font, 0, 0, 0xFFFFFFFF, text);
		
		// Capturas de pantalla		// Screenshots
		if(AMG_Button.Down &PSP_CTRL_CIRCLE) AMG_Screenshot("Picture.png");
		
		AMG_OrthoMode(0);	// Desactiva el modo 2D		// Disable 2D mode
		AMG_Update3D();		// Actualiza el dibujado 3D	// Update 3D rendering
		AMG_UpdateTime();
	}
	
	// Fin del programa		// Program end
	AMG_UnloadTexture(font);
	AMG_UnloadTexture(mario);
	for(i=0;i<6;i++) AMG_UnloadTexture(level[i]);
	AMG_Finish3D();
	return AMG_ReturnXMB();
}

// Mueve a Mario	// Move mario
void moveMario(AMG_Texture *m, int *cam_x){
	if(m == NULL) return;	// Comprobamos que existe...	// Check if it exists...
	
	// Mueve el personaje	// Player movement
	if(AMG_Button.Held &PSP_CTRL_LEFT){		// Si presionas izquierda...						// When pressing left...
		m->ScaleX = -1.0f;					// Miramos hacia la izquierda						// Look at left side
		if((*cam_x < 240) || (*cam_x > (2024-240))){	// Si la cámara no está en los límites	// If camera isn't limited
			m->X -= SPEED_X;	// Movemos a mario	// Move mario
			// Comprobamos los límites del mundo
			// Check world limits
			if((m->X >= 240) && (*cam_x < 240)){
				m->X = 240; *cam_x = 240;
			}else if((m->X <= 240) && (*cam_x > (2024-240))){
				m->X = 240; *cam_x = (2024-240);
			}
		}else *cam_x -= SPEED_X;	// Si no, movemos la cámara 2D	// Else, move 2D camera
	}else if(AMG_Button.Held &PSP_CTRL_RIGHT){			// Si presionamos derecha...			// When pressing right...
		m->ScaleX = 1.0f;								// Miramos hacia la derecha				// Look at right side
		if((*cam_x < 240) || (*cam_x > (2024-240))){ 	// Si la cámara no está en los límites	// If camera isn't limited
			m->X += SPEED_X;	// Movemos a mario		// Move mario
			// Comprobamos los límites del mundo	// Check world limits
			if((m->X >= 240) && (*cam_x < 240)){
				m->X = 240; *cam_x = 240;
			}else if((m->X <= 240) && (*cam_x > (2024-240))){
				m->X = 240; *cam_x = (2024-240);
			}
		}else *cam_x += SPEED_X;	// Si no, movemos la cámara 2D	// Else, move 2D camera
	}
	
	// Controla los saltos	// Jump control
	static u8 jump_power = JUMP_POWER;	// La potencia del salto (píxeles/frame)	// Jump power (pixels/frame)
	if((AMG_Button.Down &PSP_CTRL_CROSS) && (!jumping) && (touchingFloor)){	// Puedes saltar si estás tocando el suelo	// Jump if touching floor
		jumping = 1;	// Estamos saltando						// I believe I can fly!!
		m->Frame = 2;	// Cambiamos el frame a "saltando"		// Change the frame to "jumping"
		touchingFloor = 0;	// No tocas suelo					// Not touching floor
	}
	if(jumping){	// Si está saltando...		// If jumping...
		m->Y -= (jump_power - GRAVITY);		// Calculamos la altura						// Calculate height
		jump_power --;						// La potencia de salto va disminuyendo		// Jump power decreasing...
		if(jump_power == 0){				// Ahora empezamos a bajar					// Now we go down
			jump_power = JUMP_POWER;	// Restablece la potencia de salto				// Reset jump power
			m->Frame = 0;
			jumping = 0;
		}
	}
	
	// Si hay que mover, animamos al personaje		// If moving, animate player
	static u8 delay;
	if((AMG_Button.Held &(PSP_CTRL_RIGHT | PSP_CTRL_LEFT)) && (!jumping)){	// Animamos el personaje si no hay salto	// Animate player if not jumping
		if(delay == 0){
			m->Frame ^= 1;
			delay = 10;
		}else{
			delay --;
		}
	}
	
	// Si sueltas el botón, animación original	// If releasing the button, set original animation
	if(AMG_Button.Up &(PSP_CTRL_RIGHT | PSP_CTRL_LEFT)){
		delay = 10;
		m->Frame = 0;
	}
	
	// No te salgas de la pantalla	// Stay in the screen
	if(m->X < 32.0f) m->X = 32.0f;
	else if(m->X > 448.0f) m->X = 448.0f;
}

// Mueve el nivel	// Move level
void moveLevel(int cam_x){
	// Calcula la cámara	// Calculate camera
	int cx = (cam_x - 240);

	// Mueve las nubes		// Scroll clouds
	static int scroll_x=-512;
	level[0]->X = 0+scroll_x;
	AMG_DrawSpriteCache(level[0]);		// <-- Esta versión usa el caché de texturas (recomendable cuando se usan fondos grandes sin rotación/escalado)
										// This version uses texture-cache (use it when drawing big background without rotation/scaling)
	level[0]->X = 512+scroll_x;
	AMG_DrawSpriteCache(level[0]);
	level[0]->X = 1024+scroll_x;
	AMG_DrawSpriteCache(level[0]);
	scroll_x += 2;
	if(scroll_x >= 0) scroll_x = -512;
	
	// Mueve las colinas	// Scroll hills
	static int hills_x = 0;
	level[1]->X = 0+hills_x;
	level[2]->X = 384+hills_x;
	AMG_DrawSpriteCache(level[2]);
	level[2]->X = -384+95+hills_x;
	AMG_DrawSpriteCache(level[2]);
	hills_x ++;
	if(hills_x >= 417) hills_x = 0;
	AMG_DrawSpriteCache(level[1]);
	
	// Mueve el nivel		// Scroll level
	if(cx < 512) AMG_DrawSpriteCache(level[3]);
	if(cx > 16 && cx < 1024) AMG_DrawSpriteCache(level[4]);
	if(cx > 512 && cx < 1536) AMG_DrawSpriteCache(level[5]);
	if(cx > 1024) AMG_DrawSpriteCache(level[6]);
	level[3]->X = 240-cx;
	level[4]->X = 752-cx;
	level[5]->X = 1264-cx;
	level[6]->X = 1776-cx;
}

// Procesa las colisiones		// Process collisions
void processColisions(u8 *cmap, AMG_Texture *p, int *mapx, int *mapy){
	// Haz que la gravedad actúe		// Gravity action
	p->Y += GRAVITY;
	// Obtén el pixel del mapa de colisiones	// Get a pixel from the collision map
	u8 val[6];	// Los valores en las 4 esquinas del personaje	// 4 corners of the player
	val[0] = getCollisionMapPoint(cmap, (*mapx + p->X - 240)-32, (*mapy)-32);
	val[1] = getCollisionMapPoint(cmap, (*mapx + p->X - 240)+32, (*mapy)-32);
	val[2] = getCollisionMapPoint(cmap, (*mapx + p->X - 240)-32, (*mapy)+32);
	val[3] = getCollisionMapPoint(cmap, (*mapx + p->X - 240)+16, (*mapy)+32);
	val[4] = getCollisionMapPoint(cmap, (*mapx + p->X - 240)+32, (*mapy)+16);	// Estos son extras para mayor precisión	// Extra precision
	val[5] = getCollisionMapPoint(cmap, (*mapx + p->X - 240)-32, (*mapy)+16);
	// Controlamos la altura del personaje	// Control player height
	if((val[3] == 0x80) || (val[3] == 0xFF)){
		p->Y -= GRAVITY;	// Añadimos la fuerza normal del suelo	// Add floor's normal force
		touchingFloor = 1;	// Tocando suelo						// Touching floor right now
	}
	// Controlamos los choques horizontales		// Horizontal collisions
	if(val[4] == 0xFF){
		*mapx -= SPEED_X;	// Acción-Reacción	// Action-Reaction
	}
	if(val[5] == 0xFF){
		*mapx += SPEED_X;	// Acción-Reacción	// Action-Reaction
	}
	// Actualiza la posición de la cámara	// Update camera position
	*mapy = (u32)p->Y;
	if(*mapx > 2024) *mapx = 0;
}

// Carga en RAM un mapa de colisiones	// Load a collision map in RAM
u8 *loadCollisionMap(const char *path){
	// Abre el archivo		// Open the file
	FILE *f = fopen(path, "rb");
	if(f == NULL) return NULL;
	// La imagen es de 2048x272, con 3 canales (RGB)				// Image is 2048x272, with RGB data
	// Así que cargamos solo el componente G para ahorrar memoria	// Only loading G component to save memory
	// Obtenemos el tamaño del archivo								// Get file size
	fseek(f, 0, SEEK_END);
	u32 size = (ftell(f) / 3);	// Solo el componente G		// Only G component
	rewind(f);
	// Creamos el buffer		// Create buffer
	u8 *buffer = (u8*) calloc (size, sizeof(u8));
	if(buffer == NULL) AMG_Error(AMG_OUT_OF_RAM, 0, "loadCollisionMap");
	// Leemos solo el componente G		// Read only G component
	u32 i = 0;
	while(!feof(f)){
		fgetc(f);
		buffer[i] = fgetc(f);
		fgetc(f);
		i ++;
	}
	// Cierra el archivo	// Close file handle
	fclose(f); f = NULL;
	// Devuelve el mapa creado	// Return created map
	return buffer;
}

// Obten un punto del mapa de colisiones		// Get a pixel from the collision map
u8 getCollisionMapPoint(u8 *cmap, u32 x, u32 y){
	// Comprueba que no te salgas		// Check for limits
	if((x > 2048) || (y > 272)) return 0;
	// Devuelve el pixel				// Return the pixel
	return cmap[(x + (y << 11))];
}
