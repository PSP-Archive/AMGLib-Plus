/*
 * AMGlib Template
 * Powered by Andresmargar
 * from SCENEBETA
 */
 
// Includes AMG
#include <AMG/AMGLib.h>

// Module info
AMG_MODULE_INFO("Mode 2D Demo", AMG_USERMODE, 1, 1);

// Defines
#define TEXT_SPEED 4

// Funcion Main()					// Main() function
// Bloque principal del programa	// Program entrypoint
int main(int argc, char **argv){

	// Inicializa la PSP		// Init PSP
	AMG_SetupCallbacks();						// Inicializa los callbacks			// Setup callbacks
	AMG_Init3D(GU_PSM_5650 | AMG_DOUBLEBUFFER);	// Inicializa el motor 3D			// Init 3D engine
	AMG_InitTimeSystem();						// Inicializa el motor de tiempo	// Init time system
	
	// Inicializa la VRAM compartida		// Allocate shared VRAM
	AMG_AllocateSharedTexture(256, 256, GU_PSM_8888);
	
	// Carga la fuente de texto		// Load text font
	AMG_Texture *font = AMG_LoadTexture("Files/Font.png", AMG_TEX_RAM);		// Cargala en RAM							// Load it in RAM
	AMG_DeleteColor(font, GU_RGBA(0, 0, 0, 0xFF));							// Borra el color negro (transparente)		// Delete black color
	AMG_Create2dObject(font, GU_PSM_5551, false);							// Conviertela en un objeto 2D				// Create 2D Object
	AMG_SwizzleTexture(font);												// Aplicamos Swizzling (aumenta mucho la velocidad de renderizado)	// Swizzle
	
	// Carga la textura que hará de "lienzo" xD		// Load a canvas (for painting)
	AMG_Texture *canvas = AMG_LoadTexture("Files/Canvas.png", AMG_TEX_RAM);	// Carga el lienzo en RAM		// Load it in RAM
	AMG_Create2dObject(canvas, GU_PSM_8888, false);							// Conviértelo en un sprite 2D	// Create 2D Object
	canvas->X = (480 / 2);													// Centramos el lienzo en el eje X...		// Center in X
	canvas->Y = (272 / 2) + 32;							// Y en el eje Y (añadimos 32 para que no se tope con el texto)	// Center in Y (adding 32 to give space)
	
	// Carga el puntero		// Load a mouse
	AMG_Texture *mouse = AMG_LoadTexture("Files/Mouse.png", AMG_TEX_RAM);	// Carga el puntero en RAM		// Load it in RAM
	AMG_DeleteColor(mouse, GU_RGBA(0, 0, 0xFF, 0xFF));						// Elimina el color azul		// Delete blue color
	AMG_Create2dObject(mouse, GU_PSM_5551, false);							// Conviértelo en un sprite 2D	// Create 2D Object
	AMG_SwizzleTexture(mouse);
	mouse->X = (480 / 2);					// Centramos en el eje X	// Center in X
	mouse->Y = (272 / 2);					// Y en el eje Y			// Center in Y
	
	// Color de fondo gris		// Gray clear color
	AMG.ClearColor = GU_RGBA(0x7F, 0x7F, 0x7F, 0xFF);
	
	/******************** EFECTO DE TEXTO MOVIÉNDOSE **********************************/
	// Crea el mensaje a mostrar		// Create a buffer for our message
	char *message = (char*) calloc (64, sizeof(char));
	sprintf(message, "Press O to take a screenshot");
	u8 message_size = strlen(message);		// Numero de caracteres del mensaje		// Our text length
	
	// Crea los colores random		// Create random colors
	u32 *colors = (u32*) calloc (message_size, sizeof(u32));
	for(u8 i=0;i<message_size;i++){
		colors[i] = GU_RGBA(rand() &0xFF, rand() &0xFF, rand() &0xFF, 0xFF);
	}
	
	// Crea el buffer de posicion del texto e inicializalo		// Create another buffer for storing position
	ScePspFVector2 *text_pos = (ScePspFVector2*) calloc (message_size, sizeof(ScePspFVector2));
	for(u8 j=0;j<message_size;j++){
		text_pos[j].x = float((font->Width / 16) * j);	// El ancho de la fuente entre 16 caracteres por fila...	// Font width divided 16 characters per row...
		text_pos[j].y = 16.0f;							// El texto se moverá de 16 a 32 en el eje Y				// Text will move from 16 to 32 in Y axis
	}
	
	// Variables adicionales		// Additional stuff
	u8 curchar = 0;			// Variable que almacena la posición del caracter que se está moviendo		// It stores the current character position in text buffer
	char text[64];
	
	// Bucle infinito para mantener el programa funcionando
	// Infinite loop to keep the program running
	while(!AMG.Exit){
	
		// Comienza el dibujado 3D											// Start 3D drawing
		// Aunque no usemos el modo 3D, hay que ponerlo de todas formas		// It doesn't matter if we don't use 3D, it's obligatory to call this
		AMG_Begin3D();
		
		// Lee el gamepad		// Read gamepad
		AMG_ReadButtons();
		
		// Selecciona el modo 2D		// Select 2D mode
		AMG_OrthoMode(true);
		
		// Selecciona la fuente y muestra los FPS	// Show FPS
		sprintf(text, "FPS: %d", AMG.FPS);
		AMG_Printf(font, 0, 0, GU_RGBA(rand()&0xFF, rand()&0xFF, rand()&0xFF, 0xFF), text);
		
		// Muestra el mensaje		// Show our message
		for(u8 k=0;k<message_size;k++){
			if(text_pos[k].y < 16.0f) text_pos[k].y = 16.0f;
			char c = message[k];
			AMG_Printf(font, text_pos[k].x, text_pos[k].y, colors[k], &c);
		}
		
		// Procesa el movimiento del texto		// Process text movement
		text_pos[curchar].y += TEXT_SPEED;		// Mueve el caracter actual hacia abajo		// Current character decreasing
		if(curchar == 0) text_pos[message_size - 1].y -= TEXT_SPEED;	// Ahora mueve hacia arriba el anterior, si es 0, mueve el último caracter
																			// Now the previous increases, if the current one is 0
		else text_pos[curchar - 1].y -= TEXT_SPEED;						// Si no es 0, mueve hacia arriba el caracter anterior	// Else, the previous one increases
		if(text_pos[curchar].y >= 32.0f){								// Si ya ha avanzado el caracter hacia abajo...			// If this character has finished...
			curchar ++;													// Pasa al siguiente	// Next one
			// Rota los colores del texto	// Rotate text color
			u32 clr = colors[message_size - 1];
			for(u8 l=0;l<message_size;l++){
				u32 clr2 = colors[l];
				colors[l] = clr;
				clr = clr2;
			}
		}
		if(curchar == message_size) curchar = 0;		// Vuelve al principio si llegas al final	// Reset if the animation finishes
		
		// Dibuja el lienzo		// Draw the canvas
		AMG_DrawSprite(canvas);
		
		// Dibuja el puntero	// Draw the mouse
		AMG_DrawSprite(mouse);
		
		// Mueve el puntero con el Joystick		// Mouse movement with the joystick
		mouse->X += (AMG_Button.JoyX / 64);
		mouse->Y += (AMG_Button.JoyY / 64);
		
		// Dibujamos con el Joystick y la X		// Drawing with Joystick and X
		if(AMG_Button.Held &PSP_CTRL_CROSS){
			// Averigua la coordenada a escribir en la textura	// Get texture coordinate (where to paint)
			int tex_x = (int)(mouse->X - canvas->X + (canvas->Width >> 1)) - 16;
			int tex_y = (int)(mouse->Y - canvas->Y + (canvas->Height >> 1)) - 16;
			if((tex_x >= 0) && (tex_y >= 0)){		// Si el puntero está dentro del lienzo...	// If the mouse is inside...
				// Dibuja 4 puntos (como un cuadrado, para tener más grosor)
				// Draw 4 dots (like a square, to have more thickness
				AMG_ChangeTexturePixel(canvas, tex_x, tex_y, colors[0]);
				AMG_ChangeTexturePixel(canvas, tex_x+1, tex_y, colors[0]);
				AMG_ChangeTexturePixel(canvas, tex_x, tex_y+1, colors[0]);
				AMG_ChangeTexturePixel(canvas, tex_x+1, tex_y+1, colors[0]);
			}
		}
		
		// Por último, saca una captura de pantalla		// Screenshots
		if(AMG_Button.Down &PSP_CTRL_CIRCLE) AMG_Screenshot("Photo.png");
		
		// Finaliza el modo 2D		// Finish 2D mode
		AMG_OrthoMode(false);
		
		// Actualiza el 3D		// Update 3D
		AMG_Update3D();
		AMG_UpdateTime();	// Actualiza el tiempo		// Update time
	}
	
	// Fin del programa		// Program end
	AMG_UnloadTexture(mouse);		// Elimina el puntero		// Delete mouse
	AMG_UnloadTexture(canvas);		// Elimina el sprite 2D		// Delete canvas
	AMG_UnloadTexture(font);		// Elimina la fuente de texto	// Delete textfont
	AMG_Finish3D();				// Termina con el modo 3D			// Finish 3D engine
	return AMG_ReturnXMB();		// Vuelve al XMB					// Go back to XMB
}
