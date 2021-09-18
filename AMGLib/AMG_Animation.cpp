// Includes
#include "AMG_Model.h"
#include "AMG_User.h"
#include "AMG_3D.h"
#include <pspgu.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
/*
// Estructura de control del script
typedef struct{
	float version;
	int angle_unit;
	u32 line_idx;
	char *path;
}amg_anim1;
amg_anim1 amg_anim;

// Número de comandos y definiciones
const u32 amg_ncmd = 9;
const u32 amg_ndef = 11;

// Tabla con las funciones (AMG.***)
const char* amg_command[] = {
	"SetVersion",
	"SetAngleUnit",
	"AutoFrameSet",
	"Alloc",
	"SelectAnimation",
	"FrameFormat",
	"FrameSet",
	"SetName",
	"MatrixFormat",
};

// Tabla con las definiciones (AMG::***)
const char *amg_defines[] = {
	"Degrees",
	"Radians",
	"DirectX",
	"OpenGL",
	"Animation",
	"Frame",
	"Pos", "Rot", "Scale", "Diffuse", "Matrix"
};

// Obtén una definición
int amg_getDefinition(char *l){
	return 0;
}

// Procesa comentarios
void amg_processComments(FILE *f, char *l){
	switch(l[0]){
		case '#':
		case '-':
			fgets(l, 1024, f); amg_anim.line_idx ++;
			break;
		case '/':
			if(l[1] == '/'){
				fgets(l, 1024, f); amg_anim.line_idx ++;
			}else if(l[1] == '*'){	// Comentarios multilínea
				char c;
				while((c = fgetc(f)) != '*') if(c == '\n') amg_anim.line_idx ++;
				fgetc(f);
			} break;
		default: break;
	}
}

// Procesa una función
int amg_execute(u32 id, char *l){
	// Posiciónate en los argumentos
	char *line = l;
	while(*line != 0) line ++;
	line ++;
	// Procesa la función
	switch(id){
		case 0:		// AMG.SetVersion
			if(sscanf(line, "%f", &amg_anim.version) != 1) goto exc;
			break;
		case 1:		// AMG.SetAngleUnit
			amg_anim.angle_unit = amg_getDefinition(line);
			if(amg_anim.angle_unit < 0) goto exc;
			break;
		default: return -1;
	}
	return 0;
	// Excepción en la función
exc:
	AMG_Error(AMG_CUSTOM_ERROR, 0, "[%s: %d] Wrong arguments for function \"AMG.%s\"", amg_anim.path, amg_anim.line_idx, amg_command[id], id);
	return -1;
}

// Carga una animación
AMG_Animation *AMG_LoadAnimation(char *path){
	// Define variables
	FILE *f = NULL;
	char *line = (char*) calloc (1024, sizeof(char));
	
	// Crea la animación
	memset(&amg_anim, 0, sizeof(amg_anim));
	amg_anim.path = path;
	AMG_Animation *anim = (AMG_Animation*) calloc (1, sizeof(AMG_Animation));
	if(anim == NULL) return NULL;
	
	// Abre el archivo
	f = fopen(path, "rb");
	if(f == NULL){ AMG_Error(AMG_OPEN_FILE, 0, path); goto error;}
	
	// Carga el archivo
	while(!feof(f)){
		// Obtén la siguiente línea
		memset(line, 0, 1024);
		fgets(line, 1024, f);
		amg_anim.line_idx ++;
		
		// Comentarios...
		amg_processComments(f, line);
		
		// Procesa las funciones
		if(strncmp(line, "AMG.", 4) == 0){
			// Encuentra el ID de la función
			u32 id = 0;
			u8 found = 0;
			for(;((id < amg_ncmd) && (!found));id++){
				u32 l = strlen(amg_command[id]);
				if(strncmp(&line[4], amg_command[id], strlen(amg_command[id])) == 0){
					found = 1; line[5+l] = 0;
				}
			}
			if(!found){
				AMG_Error(AMG_CUSTOM_ERROR, 0, "[%s:%d] Function \"AMG.%s\" not found", path, amg_anim.line_idx, &line[4]); goto error;
			}
			// Ejecuta la función
			if(amg_execute(id, line) < 0) goto error;
		}
	}
	
	// Devuelve la animación
	fclose(f); f = NULL;
	return anim;
	
	// En caso de error...
error:
	if(line) free(line);
	if(f) fclose(f);
	if(anim){
		AMG_UnloadAnimation(anim);
		free(anim); anim = NULL;
	}
	return NULL;
}

// Elimina una animación
void AMG_UnloadAnimation(AMG_Animation *anim){
	if(anim == NULL) return;
}*/
