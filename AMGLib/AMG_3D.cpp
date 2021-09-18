// Includes
#include "AMG_3D.h"
#include <pspgu.h>
#include <pspdisplay.h>
#include <pspkernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "AMG_User.h"
#include <malloc.h>
#include "lightmap.h"
#include "AMG_Texture.h"

// OSLIB
#ifdef AMG_COMPAT_OSLIB
#include <oslib/oslib.h>
#endif

// Variables
char amgt[256];
AMG_ AMG;
AMG_Vertex_CV *amg_scrfade = NULL;
AMG_Light_ AMG_Light[4];
u8 amg_lightenabled = 0;
u8 amg_updated = 0, amg_curlist = 0;
u32 amg_dlist_size = 0;
u32 *amg_callList = NULL;

// Sistema de VRAM
typedef struct{
	void *buffer;
}amg_vram_block;

typedef struct{
	u32 *vram;		// Puntero a VRAM
	u32 vram_size;	// Tamaño de la VRAM
	u16 align;		// Alineación de los bloques (cuanto más alto, menos memoria a usar, pero menos efectivo)
	u32 nblocks;
	u32 free_blocks;	// Bloques libres
	amg_vram_block *block;
}amg_vram_;
amg_vram_ amg_vram;

// Obten un bloque de VRAM
u32 AMG_GetVramSize(u32 width, u32 height, u32 psm){
	switch(psm){
		case GU_PSM_T4:
			return (width * height) >> 1;
		case GU_PSM_T8:
			return width * height;
		case GU_PSM_5650:
		case GU_PSM_5551:
		case GU_PSM_4444:
		case GU_PSM_T16:
			return (width * height) << 1;
		case GU_PSM_8888:
		case GU_PSM_T32:
			return (width * height) << 2;
		default:
			return 0;
	}
}

// Obten un bloque de VRAM
void* AMG_VramAlloc(u32 width, u32 height, u32 psm){
#ifdef AMG_COMPAT_OSLIB
	void *r = oslVramMgrAllocBlock(AMG_GetVramSize(width, height, psm));
	if(r == NULL) return sceGeEdramGetAddr();
	return r;
#else
	u32 size = AMG_GetVramSize(width, height, psm);
	u16 align = (size % amg_vram.align);
	if(align != 0) size += (amg_vram.align - align);		// Alinea el bloque
	u32 nblocks = (size / amg_vram.align);					// Número de bloques de VRAM a ocupar
	void *result = NULL;
	// No te molestes si no hay espacio libre...
	if((nblocks > amg_vram.free_blocks) || (size == 0)){
		result = sceGeEdramGetAddr();
	}else{
		// Busca el primer bloque vacío
		for(u32 i=0;i<amg_vram.nblocks;i++){
			if(amg_vram.block[i].buffer == NULL){	// Si encuentras uno vacío...
				// Asegúrate de que el buffer cabe ahí
				u32 j, tmp_size = 0;
				for(j=i;j<amg_vram.nblocks;j++){
					if(amg_vram.block[j].buffer == NULL) tmp_size += amg_vram.align;
					else j = amg_vram.nblocks;		// No cabe
				}
				if(tmp_size >= size){		// Si el buffer cabe...
					// Obtén la dirección de memoria
					result = (void*)((i * amg_vram.align) + (u32)sceGeEdramGetAddr());
					// Marca como usados los bloques necesarios
					for(j=0;j<nblocks;j++){
						amg_vram.block[i + j].buffer = result;
					}
					i = amg_vram.nblocks;				// Deja de buscar
					amg_vram.free_blocks -= nblocks;	// Actualiza los bloques libres
				}
			}
		}
	}
	// LOG
	/*FILE *f = fopen("log.txt", "a+");
	fprintf(f, "ALLOC:\nsize: %d result: %p nblocks: %d blocks left: %d\n", size, result, nblocks, amg_vram.free_blocks);
	fclose(f); f = NULL;*/
	return result;		// Devuelve el bloque encontrado
#endif
}

// Libera un bloque de VRAM
void AMG_FreeVram(void *buffer, u32 i0){
#ifdef AMG_COMPAT_OSLIB
	oslVramMgrFreeBlock(buffer, i0);
#else
	u32 size = 0;	// Tamaño del buffer
	if(buffer != sceGeEdramGetAddr()){		// Comprueba si el buffer no es NULL
		// Obten el bloque inicial donde se encuentra
		for(u32 i=0;i<amg_vram.nblocks;i++){
			if(amg_vram.block[i].buffer == buffer){		// Si lo encuentras...
				// Obtén el tamaño del buffer
				for(u32 j=i;j<amg_vram.nblocks;j++){
					if(amg_vram.block[j].buffer == buffer){
						size += amg_vram.align;		// Incrementa el tamaño
						amg_vram.block[j].buffer = NULL;	// Y a su vez libera la memoria virtual
					}else j = amg_vram.nblocks;
				}
				amg_vram.free_blocks += (size / amg_vram.align);	// Actualiza el espacio disponible
				i = amg_vram.nblocks;		// Deja de buscar
			}
		}
	}
	/*FILE *f = fopen("log.txt", "a+");
	fprintf(f, "FREE:\nsize: %d blocks left: %d\n", size, amg_vram.free_blocks);
	fclose(f); f = NULL;*/
#endif
}

// Muestra un error fatal
void AMG_Error(u8 errorcode, u32 aux, const char *text, ...){
	memset(amgt, 0, 256);
	// Según el código de error
	switch(errorcode){
		case AMG_CUSTOM_ERROR:
			va_list list;
			va_start(list, text);
			vsprintf(amgt, text, list);
			va_end(list); break;
		case AMG_OUT_OF_RAM:
			sprintf(amgt, "[%s] Couldn't allocate %d kb in RAM", text, int(aux >> 10)); break;
		case AMG_OPEN_FILE:
			sprintf(amgt, "No such file or directory: \"%s\"", text); break;
		case AMG_NULL_POINTER:
			sprintf(amgt, "[%s] NULL pointer", text); break;
		case AMG_THREAD_SETUP:
			sprintf(amgt, "Error creating thread: \"%s\"", text); break;
		case AMG_OUT_OF_VRAM:
			sprintf(amgt, "[%s] Couldn't allocate %d kb in VRAM", text, int(aux >> 10)); break;
		case AMG_MODULE_INIT:
			sprintf(amgt, "Couldn't init \"%s\" module: Errorcode %d", text, (int)aux); break;
		case AMG_CONVERT_TEXTURE:
			sprintf(amgt, "Couldn't convert texture: \"%s\"", text); break;
		case AMG_TEXTURE_WRONG_DATA:
			sprintf(amgt, "Wrong data in texture: \"%s\"", text); break;
		case AMG_TEXTURE_OVERFLOW_SIZE:
			sprintf(amgt, "[%s] Texture is too big! (>512x512)", text); break;
		case AMG_MODULE_EXCEPTION:
			sprintf(amgt, "Exception in %s: Errorcode %d", text, (int)aux); break;
		default:
			sprintf(amgt, "Unknown error");  break;
	}
	if(AMG.DebugMode == AMG_ERRORMSG){
#ifndef AMG_COMPILE_ONELUA
		// Primero intentalo con el mensaje del sistema
		if(AMG_MessageBox(AMG_MESSAGE_STRING, false, 0, amgt) == AMG_MESSAGE_EXCEPTION){
			// Inicializa la consola de texto
			pspDebugScreenInit();
			pspDebugScreenSetXY(0, 0);
			pspDebugScreenPrintf("\n FATAL ERROR:\n \"%s\"\n Press HOME to exit", amgt);
			while(!AMG.Exit) sceDisplayWaitVblankStart();
		}
		sceKernelExitGame();
#endif
	}else if(AMG.DebugMode == AMG_ERRORCODE){
		// Manda el código del error
		AMG.ErrorCode = errorcode;
		sprintf(AMG.ErrorString, amgt);
		AMG.ErrorDetail = aux;
	}
}

// Inicializa el 3D
void AMG_Init3D(int options){

	// Inicializa las variables globales
	memset(&AMG, 0, sizeof(AMG_));
	AMG.WaitVblank = true;
	AMG.Inited = false;
	AMG.TextureQuality = GU_PSM_8888;
	AMG.TextureSwizzle = false;
	AMG.ClearColor = 0xFFFFFFFF;
	AMG.TextureDest = 1;		// AMG_TEX_VRAM
	AMG.WorldSpecular = 12.0f;
	AMG.DoubleBuffer = !((options >> 4) &1);
	AMG.PSM = (options &0xF);
	
	// Modo de DEBUG
#ifdef AMG_COMPILE_ONELUA
	AMG.DebugMode = AMG_ERRORCODE;
#else
	AMG.DebugMode = AMG_ERRORMSG;
#endif

	// Configura las dimensiones de la pantalla
	AMG.ScreenWidth = ((options >> 5) &0x3FF);
	AMG.ScreenHeight = ((options >> 15) &0x3FF);
	if(AMG.ScreenWidth == 0) AMG.ScreenWidth = 480;
	if(AMG.ScreenHeight == 0) AMG.ScreenHeight = 272;
	if(AMG.ScreenWidth > 512) AMG.ScreenStride = 768;
	else AMG.ScreenStride = 512;

#ifndef AMG_COMPAT_OSLIB
	// Inicializa la VRAM dinámica
	amg_vram.vram = (u32*) sceGeEdramGetAddr();
	amg_vram.vram_size = sceGeEdramGetSize();
	amg_vram.align = AMG_VRAM_ALIGN;
	amg_vram.nblocks = (amg_vram.vram_size / amg_vram.align);
	amg_vram.free_blocks = amg_vram.nblocks;
	amg_vram.block = (amg_vram_block*) calloc (amg_vram.nblocks, sizeof(amg_vram_block));
	if(amg_vram.block == NULL){ AMG_Error(AMG_OUT_OF_RAM, amg_vram.nblocks*sizeof(amg_vram_block), "amg_vram.block"); return;}
	for(u32 i=0;i<amg_vram.nblocks;i++){
		amg_vram.block[i].buffer = NULL;
	}
#endif
	
	// Inicializa los buffers
#ifndef AMG_COMPAT_OSLIB
	AMG.FB0 = (u32*)((u32) AMG_VramAlloc(AMG.ScreenStride, AMG.ScreenHeight, AMG.PSM) - (u32)amg_vram.vram);
	if(AMG.DoubleBuffer){
		AMG.FB1 = (u32*)((u32) AMG_VramAlloc(AMG.ScreenStride, AMG.ScreenHeight, AMG.PSM) - (u32)amg_vram.vram);
	}else{
		AMG.FB1 = AMG.FB0;
	}
	AMG.ZB = (u16*)((u32) AMG_VramAlloc(AMG.ScreenStride, AMG.ScreenHeight, GU_PSM_4444) - (u32)amg_vram.vram);
	AMG.List = (u32*) memalign (16, 262144*sizeof(u32));		// 1MB de lista
	amg_callList = (u32*) memalign(16, 64*sizeof(u32));
	amg_dlist_size = (262144 << 2);
#else
	AMG.FB0 = NULL;
	AMG.FB1 = NULL;
	AMG.ZB = NULL;
#endif
	
#ifndef AMG_COMPAT_OSLIB
	// Comprueba si hay espacio suficiente
	if((AMG.List == NULL) || (amg_callList == NULL)){ AMG_Error(AMG_OUT_OF_RAM, 262144*sizeof(u32), "AMG.List"); return;}
	if(AMG.ScreenStride > 512){	// Modo TV (para PSP SLIM)
		if((AMG.ZB == NULL) || (AMG.DoubleBuffer && (AMG.FB1 == NULL))){ AMG_Error(AMG_CUSTOM_ERROR, 0, "You have to unlock PSP SLIM VRAM to enable TV mode"); return;}
	}
#endif
	
	// Inicializa GU
	AMG.Rendering = true;
#ifndef AMG_COMPAT_OSLIB	// Inicia sceGu* si no tenemos OSLIB
	sceGuInit();
	sceGuStart(GU_DIRECT, AMG.List);
	sceGuDrawBuffer(AMG.PSM, AMG.FB0, AMG.ScreenStride);
	sceGuDispBuffer(AMG.ScreenWidth, AMG.ScreenHeight, AMG.FB1, AMG.ScreenStride);
	sceGuDepthBuffer(AMG.ZB, AMG.ScreenStride);
#else
	oslStartDrawing();
#endif
	sceGuOffset(2048 - (AMG.ScreenWidth >> 1), 2048 - (AMG.ScreenHeight >> 1));
	sceGuViewport(2048, 2048, AMG.ScreenWidth, AMG.ScreenHeight);
	sceGuDepthRange(65535, 0);
	sceGuScissor(0, 0, AMG.ScreenWidth, AMG.ScreenHeight);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuDisable(GU_CULL_FACE);
	sceGuEnable(GU_BLEND);
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
	sceGuDisable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuClearColor(0xFFFFFFFF);
	sceGuClearDepth(0);
	sceGuClearStencil(0);
	if(AMG.PSM != GU_PSM_8888){		// Activa DITHERING (la matriz se pone en sceGuStart())
		sceGuEnable(GU_DITHER);
	}else{
		sceGuDisable(GU_DITHER);
	}
	// Inicializa matrices
	AMG_InitMatrixSystem(75.0f);
#ifndef AMG_COMPAT_OSLIB
	sceGuFinish();
	sceGuSync(0, 0);
#else
	oslEndDrawing();
#endif
	
	// Inicializa el display
	sceDisplayWaitVblankStart();
#ifndef AMG_COMPAT_OSLIB
	sceGuDisplay(GU_TRUE);
#endif
	amg_updated = 1;
	AMG.Rendering = false;
	AMG.Inited = true;
	
	// Inicializa el Screen Fading
	amg_scrfade = (AMG_Vertex_CV*) calloc (2, sizeof(AMG_Vertex_CV));
	amg_scrfade[0].x = 0.0f; amg_scrfade[0].y = 0.0f; amg_scrfade[0].z = 0.0f;
	amg_scrfade[1].x = (float)AMG.ScreenWidth; amg_scrfade[1].y = (float)AMG.ScreenHeight; amg_scrfade[1].z = 0.0f;
	
	// Inicializa las fuentes de luz
	u8 i;
	for(i=0;i<4;i++){
		AMG_Light[i].Pos.x = AMG_Light[i].Pos.y = AMG_Light[i].Pos.z = AMG_Light[i].Pos.w = 0.0f;
		AMG_Light[i].Type = GU_DIRECTIONAL;
		AMG_Light[i].Diffuse = 0xFFFFFFFF; AMG_Light[i].Ambient = 0xFF3F3F3F; AMG_Light[i].Specular = 0xFFFFFFFF;
		AMG_Light[i].Attenuation[0] = AMG_Light[i].Attenuation[2] = 0.0f; AMG_Light[i].Attenuation[1] = 1.0f;
		AMG_Light[i].Component = GU_DIFFUSE_AND_SPECULAR;
	}
	
	// Inicializa el lightmap
	AMG.Lightmap = (void*) AMG_CreateTexture(32, 32, GU_PSM_8888, AMG_TEX_VRAM);
	AMG_Texture *l = (AMG_Texture*)AMG.Lightmap;
	memcpy(l->Data, lightmap, 64*64*4);
	AMG_SetTextureMapping(l, GU_ENVIRONMENT_MAP, 2, 3);
	
	// Comienza el dibujado
	AMG_Config3D(GU_DIRECT, 0);
	
	// Borra el DCACHE
	sceKernelDcacheWritebackInvalidateAll();
}

// Comienza el dibujado 3D
void AMG_Begin3D(void){
	if(!amg_updated) return;
	if(AMG.DrawMode == GU_DIRECT){
		sceGuClearColor(AMG.ClearColor);
#ifndef AMG_COMPAT_OSLIB
		sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT | GU_STENCIL_BUFFER_BIT);
#endif
	}
	AMG_PushMatrix(GU_MODEL);
	amg_updated = 0;
	AMG.Rendering = true;
	AMG.DrawnVertices = 0;
}

// Actualiza el dibujado 3D
void AMG_Update3D(void){
	if(amg_updated) return;
	AMG_PopMatrix(GU_MODEL);
#ifndef AMG_COMPAT_OSLIB
	sceGuFinish();
	sceGuSync(0, 0);
	if(AMG.WaitVblank) sceDisplayWaitVblankStart();
	if(AMG.DoubleBuffer) AMG.CurrentFB = sceGuSwapBuffers();

	// Procesa la display list
	if(AMG.DrawMode == GU_DIRECT) sceGuStart(GU_DIRECT, AMG.List);
	else if(AMG.DrawMode == GU_CALL){
		sceGuStart(GU_DIRECT, amg_callList);
			sceGuClearColor(AMG.ClearColor);
			sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT | GU_STENCIL_BUFFER_BIT);	// <-- ESTO ME LLEVO UNA SEMANA DARME CUENTA
			sceGuCallList(&AMG.List[amg_curlist * amg_dlist_size]);
		sceGuFinish();
		amg_curlist ^= 1;
		sceGuStart(GU_CALL, &AMG.List[amg_curlist * amg_dlist_size]);
	}else{
		/** EXPERIMENTAL **/
		/*static PspGeContext ctx;
		sceGuStart(GU_DIRECT, AMG.List);
			sceGuClearColor(AMG.ClearColor);
			sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT | GU_STENCIL_BUFFER_BIT);
		sceGuFinish();
		sceGeDrawSync(0);
		sceGuSendList(GU_TAIL, AMG.List, &ctx);
		sceGeDrawSync(0);
		sceGuStart(GU_SEND, AMG.List);*/
	}
#endif
	amg_updated = 1;
	AMG.Rendering = false;
}

// Acaba con el 3D
void AMG_Finish3D(void){
#ifndef AMG_COMPAT_OSLIB
	// Deja de dibujar
	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_FALSE);
	sceGuTerm();
#else
	oslStartDrawing();
	sceGuEnable(GU_TEXTURE_2D);
	sceGuDisable(GU_LIGHTING);
	sceGuDisable(GU_DITHER);
	sceGuDisable(GU_DEPTH_TEST);
	sceGuClearColor(0);
#endif
	// Libera buffers
	free(amg_scrfade); amg_scrfade = NULL;
	AMG_UnloadTexture((AMG_Texture*)AMG.Lightmap);
	AMG_DestroyMatrixSystem();
#ifndef AMG_COMPAT_OSLIB
	free(AMG.List); AMG.List = NULL;
	free(amg_callList); amg_callList = NULL;
	// Elimina el sistema de VRAM dinámica
	free(amg_vram.block);
#endif
}

// Configura el 3D
void AMG_Config3D(u8 dmode, u32 dlsize){
	amg_curlist = 0;
	AMG.DrawMode = dmode;
#ifdef AMG_COMPAT_OSLIB
	if(dlsize > 0) oslSetSysDisplayListSize(dlsize);
	oslStartDrawing();
#else
	if(AMG.Rendering){
		sceGuFinish();
		sceGuSync(0, 0);
		sceDisplayWaitVblankStart();
	}
	if(dlsize > 0){
		if(AMG.List) free(AMG.List);
		AMG.List = (u32*) calloc (dlsize, sizeof(u8));
		if(AMG.List == NULL) AMG_Error(AMG_OUT_OF_RAM, dlsize, "AMG_Config3D");
		amg_dlist_size = dlsize;	// Actualiza el tamaño de la Display List
	}
	sceGuStart(AMG.DrawMode, AMG.List);
#endif
	AMG.Rendering = true;
}

// Selecciona el lightmap
void AMG_SetLightmap(char *path){
	if(path == NULL) return;
	if(AMG.Lightmap) AMG_UnloadTexture((AMG_Texture*)AMG.Lightmap);
	AMG.Lightmap = (void*) AMG_LoadTexture(path, AMG_TEX_VRAM);
}

// Modo Ortho 2D
void AMG_OrthoMode(u8 _2d){
	if(_2d){
		// Desactiva depth-testing
		sceGuDisable(GU_DEPTH_TEST);
		sceGuEnable(GU_TEXTURE_2D);
		// Luces y blend
		amg_lightenabled = sceGuGetStatus(GU_LIGHTING);
		sceGuDisable(GU_LIGHTING);
	}else{
		// Dibuja el screenfade
		amg_scrfade[0].color = amg_scrfade[1].color = AMG.ScreenFadeColor;
		sceGuDrawArray(GU_SPRITES, (GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D), 2, 0, amg_scrfade);
		// Activa depth-testing
		sceGuEnable(GU_DEPTH_TEST);
		sceGuDisable(GU_TEXTURE_2D);
		// Luces
		if(amg_lightenabled) sceGuEnable(GU_LIGHTING);
	}
}

// Activa una fuente de luz
void AMG_EnableLight(u8 n){
	if(n > 3) return;
	sceGuEnable(GU_LIGHTING);
	sceGuEnable((GU_LIGHT0+n));
	AMG_Light[n].Pos.w = (AMG_Light[n].Type == GU_DIRECTIONAL) ? 0.0f : 1.0f;
	// Establece la luz seleccionada
	sceGuLightMode(GU_SEPARATE_SPECULAR_COLOR);
	sceGuLight(n, AMG_Light[n].Type, AMG_Light[n].Component, (ScePspFVector3*)&AMG_Light[n].Pos);
	sceGuLightColor(n, GU_DIFFUSE, AMG_Light[n].Diffuse);
	sceGuLightColor(n, GU_SPECULAR, AMG_Light[n].Specular);
	sceGuLightColor(n, GU_AMBIENT, AMG_Light[n].Ambient);
	sceGuLightAtt(n, AMG_Light[n].Attenuation[0], AMG_Light[n].Attenuation[1], AMG_Light[n].Attenuation[2]);
	if(AMG_Light[n].Type == GU_SPOTLIGHT) sceGuLightSpot(n, &AMG_Light[n].SpotDirection, AMG_Light[n].SpotExponent, AMG_Light[n].SpotCutoff);
}

// Activa la niebla
void AMG_EnableFog(float near, float far, u32 color){
	AMG.ClearColor = color;
	sceGuEnable(GU_FOG);
	sceGuFog(near, far, color);
}

// Inicializa una camara
AMG_Camera *AMG_InitCamera(void){
	AMG_Camera *cam = (AMG_Camera*) calloc (1, sizeof(AMG_Camera));
	cam->Pos.x = cam->Pos.y = cam->Pos.z = 0.0f;
	cam->Eye.x = cam->Eye.y = cam->Eye.z = 0.0f;
	cam->Up.x = cam->Up.z = 0.0f; cam->Up.y = 1.0f;
	cam->Rot.x = cam->Rot.y = cam->Rot.z = 0.0f;
	return cam;
}

// Establece una camara
void AMG_SetCamera(AMG_Camera *cam){
	if(cam == NULL) return;
	AMG_LoadIdentity(GU_VIEW);
	AMG_LoadIdentity(GU_MODEL);
	AMG_LookAt(&cam->Pos, &cam->Eye, &cam->Up);
	AMG_Rotate(GU_MODEL, &cam->Rot);
}
