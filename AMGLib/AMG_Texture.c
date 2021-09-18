// Includes
#include "AMG_Texture.h"
#include "AMG_User.h"
#include <pspgu.h>
#include <pspgum.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pspdisplay.h>
#include <pspjpeg.h>
#include "AMG_Multimedia.h"
#include "AMG_3D.h"
#include <malloc.h>

// OSLIB o LODEPNG
#ifdef AMG_COMPAT_OSLIB
#include <oslib/oslib.h>
#else
#include <AMG/lodepng.h>
#endif

// Variables locales
char amgt2[256];
void *amg_shared_vram = NULL;
void *amg_svram_curtex = NULL;
u32 amg_svram_size = 0;

// Calcula la siguiente potencia de 2
u32 nextPow2(u32 a){
	if(a < 8) return 8;
	else if(a > 8 && a < 16) return 16;
	else if(a > 16 && a < 32) return 32;
	else if(a > 32 && a < 64) return 64;
	else if(a > 64 && a < 128) return 128;
	else if(a > 128 && a < 256) return 256;
	else if(a > 256) return 512;
	return a;
}

// Reserva espacio para las texturas compartidas
void AMG_AllocateSharedTexture(u32 w, u32 h, u32 psm){
	amg_shared_vram = AMG_VramAlloc(w, h, psm);
	amg_svram_size = AMG_GetVramSize(w, h, psm);
	if(amg_shared_vram == sceGeEdramGetAddr()) AMG_Error(AMG_OUT_OF_VRAM, AMG_GetVramSize(w, h, psm), "AMG_AllocateSharedTexture");
}

// Elimina el espacio para las texturas compartidas
void AMG_FreeSharedTexture(void){
	AMG_FreeVram(amg_shared_vram, amg_svram_size);
	amg_shared_vram = NULL;
	amg_svram_size = 0;
}

// Activa una textura
void AMG_EnableTexture(AMG_Texture *tex){
	if(tex == NULL) return;
	u8 *ptr;
	if(tex->Frame > (tex->NFrames-1)) tex->Frame = 0;
	u32 offset = (AMG_GetVramSize(tex->Width, tex->Height, tex->TexFormat) * tex->Frame);
	if((tex->Load == AMG_TEX_RAM) && (amg_shared_vram) && (AMG_GetVramSize(tex->Width, tex->Height, tex->TexFormat) <= amg_svram_size)){
		if(amg_svram_curtex != tex->Data){		// Cargamos la textura en VRAM compartida, si se puede...
			sceGuCopyImage(tex->TexFormat, 0, 0, tex->Width, tex->Height, tex->Width, 
				&(((u8*)tex->Data)[AMG_GetVramSize(tex->Width, tex->Height, tex->TexFormat) * tex->Frame]), 0, 0, tex->Width, amg_shared_vram);
			offset = 0;
			amg_svram_curtex = tex->Data;
			sceGuTexSync();
		}
		ptr = (u8*) amg_shared_vram;
	}else ptr = (u8*)tex->Data;
	sceGuEnable(GU_TEXTURE_2D);
	sceGuTexMode(tex->TexFormat, tex->NMipmaps, 0, tex->Swizzle);
	sceGuTexImage(0, tex->rw, tex->rh, tex->Width, &ptr[offset]);
	sceGuTexFunc(tex->TFX, tex->TCC);
	sceGuTexEnvColor(tex->EnvColor);
	sceGuTexFilter(tex->MinFilter, tex->MagFilter);
	sceGuTexScale(1.0f, 1.0f);
	sceGuTexOffset(tex->U, tex->V);
	sceGuTexWrap(tex->WrapX, tex->WrapY);
	sceGuTexMapMode(tex->Mapping, tex->MappingLights[0], tex->MappingLights[1]);
	if((tex->Mapping == GU_ENVIRONMENT_MAP) && (tex->MappingLights[0] < 4) && (tex->MappingLights[1] < 4)){		// Si estamos hablando de un Environment Map..
		// Calcula la matriz de 2x3
		AMG_Light[tex->MappingLights[0]].Pos.x = AMG_Cos(tex->EnvMapRot);
		AMG_Light[tex->MappingLights[0]].Pos.y = AMG_Sin(tex->EnvMapRot);
		AMG_Light[tex->MappingLights[0]].Pos.z = tex->EnvMapX;
		AMG_Light[tex->MappingLights[1]].Pos.x = -AMG_Sin(tex->EnvMapRot);
		AMG_Light[tex->MappingLights[1]].Pos.y = AMG_Cos(tex->EnvMapRot);
		AMG_Light[tex->MappingLights[1]].Pos.z = tex->EnvMapY;
		// Activa la matriz
		sceGuDisable(GU_LIGHT0 + tex->MappingLights[0]);
		sceGuDisable(GU_LIGHT0 + tex->MappingLights[1]);
		sceGuLight(tex->MappingLights[0], GU_DIRECTIONAL, GU_DIFFUSE, (ScePspFVector3*)&AMG_Light[tex->MappingLights[0]].Pos);
		sceGuLight(tex->MappingLights[1], GU_DIRECTIONAL, GU_DIFFUSE, (ScePspFVector3*)&AMG_Light[tex->MappingLights[1]].Pos);
	}
}

// Crea una textura vacía
AMG_Texture *AMG_CreateTexture(u16 width, u16 height, u32 psm, u8 load){
	// Comprueba si es NULL
	AMG_Texture *tex = (AMG_Texture*) calloc (1, sizeof(AMG_Texture));
	// Inicializa la estructura
	tex->NFrames = 1; tex->Frame = 0;
	tex->Load = load;
	tex->Data = NULL;
	tex->MinFilter = GU_LINEAR; tex->MagFilter = GU_LINEAR;
	tex->NMipmaps = 0; tex->Swizzle = 0;
	tex->TFX = GU_TFX_MODULATE;
	tex->TCC = GU_TCC_RGBA;
	tex->EnvColor = 0xFFFFFFFF;
	tex->TexFormat = psm;
	tex->U = 0.0f; tex->V = 0.0f;
	tex->X = tex->Y = 0;
	tex->WrapX = GU_REPEAT; tex->WrapY = GU_REPEAT;
	tex->Mapping = GU_TEXTURE_COORDS;
	tex->MappingLights[0] = 2; tex->MappingLights[1] = 3;
	tex->EnvMapRot = 0.0f; tex->EnvMapX = 0.0f; tex->EnvMapY = 0.0f;
	tex->SprColor = 0xFFFFFFFF;
	tex->Rot = 0.0f; tex->ScaleX = 1.0f; tex->ScaleY = 1.0f;
	tex->Width = width; tex->Height = height;
	tex->rw = tex->Width; tex->rh = tex->Height;
	// Crea los datos
	if(load == AMG_TEX_RAM) tex->Data = (u32*) malloc (AMG_GetVramSize(width, height, psm));
	else tex->Data = (u32*) AMG_VramAlloc(width, height, psm);
	if(tex->Data == NULL){ AMG_Error(AMG_OUT_OF_RAM, AMG_GetVramSize(width, height, psm), "AMG_CreateTexture"); return NULL;}
	if(tex->Data == sceGeEdramGetAddr()){ AMG_Error(AMG_OUT_OF_VRAM, AMG_GetVramSize(width, height, psm), "AMG_CreateTexture"); return NULL;}
	memset(tex->Data, 0, AMG_GetVramSize(width, height, psm));
	// Devuelve la textura creada
	return tex;
}

// Carga una textura en formato PNG o JPG
AMG_Texture *AMG_LoadTexture(char *path, u8 load){
	
	// Crea la textura
	AMG_Texture *tex = (AMG_Texture*) calloc (1, sizeof(AMG_Texture));
	
	// Inicializa la estructura
	tex->NFrames = 1; tex->Frame = 0;
	tex->Load = AMG_TEX_RAM;
	tex->Data = NULL;
	tex->MinFilter = GU_LINEAR; tex->MagFilter = GU_LINEAR;
	tex->NMipmaps = 0; tex->Swizzle = 0;
	tex->TFX = GU_TFX_MODULATE;
	tex->TCC = GU_TCC_RGBA;
	tex->EnvColor = 0xFFFFFFFF;
	tex->TexFormat = GU_PSM_8888;
	tex->U = tex->V = 0.0f;
	tex->X = tex->Y = 0;
	tex->WrapX = GU_REPEAT; tex->WrapY = GU_REPEAT;
	tex->Mapping = GU_TEXTURE_COORDS;
	tex->MappingLights[0] = 2; tex->MappingLights[1] = 3;
	tex->EnvMapRot = tex->EnvMapX = tex->EnvMapY = 0.0f;
	tex->SprColor = 0xFFFFFFFF;
	tex->Rot = 0.0f; tex->ScaleX = 1.0f; tex->ScaleY = 1.0f;
	tex->Swizzle = 0;

#ifdef AMG_COMPAT_OSLIB
	OSL_IMAGE *img = oslLoadImageFilePNG(path, OSL_IN_RAM, OSL_PF_8888);
	if(img == NULL){ AMG_Error(AMG_CUSTOM_ERROR, 0, "AMG_LoadTexture: Couldn't load %s", path); return NULL;}
	tex->TexFormat = img->pixelFormat;
	tex->Data = (u32*) img->data;
	tex->Width = (u32) img->sysSizeX;
	tex->Height = (u32) img->sysSizeY;
	oslUnswizzleImage(img);		// Si tiene swizzling no podemos voltear...
#else
	// Abre el archivo
	FILE *f = fopen(path, "rb");
	if(f == NULL){ AMG_Error(AMG_OPEN_FILE, 0, path); return NULL;}
	
	// Comprueba si es un archivo PNG
	char header[4];
	fread(header, 4, 1, f);
	if(strncmp(header+1, "PNG", 3) == 0){
		// Lee el archivo PNG
		fclose(f); f = NULL;
		u8 *buf = NULL;
		size_t fsize = 0;
		lodepng_load_file(&buf, &fsize, path);
		int error = lodepng_decode32((u8**)&tex->Data, (unsigned*)&tex->Width, (unsigned*)&tex->Height, buf, fsize);
		if(error){ AMG_Error(AMG_CUSTOM_ERROR, 0, lodepng_error_text(error)); return NULL;}
	}else{
		// Comprueba si es un JPEG
		fseek(f, 0, SEEK_SET);
		if((fgetc(f) == 0xFF) && (fgetc(f) == 0xD8)){
			// Cargalo en RAM
			fseek(f, 0, SEEK_END);
			u32 size = ftell(f);
			u8 *buf = (u8*) calloc (size, sizeof(u8));
			fseek(f, 0, SEEK_SET);
			fread(buf, size, 1, f);
			fclose(f); f = NULL;
			// Obtén ancho y alto
			u32 i=2;
			while(i < size){
				// Si es un marcador...
				if(buf[i] == 0xFF){
					i++;
					switch(buf[i]){
						case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC5: case 0xC6: case 0xC7: case 0xC9: case 0xCA: case 0xCB: case 0xCD: case 0xCE: case 0xCF:
							i += 4;
							tex->Height = (buf[i] << 8) | (buf[i+1]);
							tex->Width = (buf[i+2] << 8) | (buf[i+3]);
							tex->Data = (u32*) memalign (16, tex->Width*tex->Height*sizeof(u32));
							if(tex->Data == NULL){ AMG_Error(AMG_OUT_OF_RAM, (tex->Width*tex->Height)<<2, "AMG_LoadTexture(JPG)"); return NULL;}
							i = size; break;
						case 0xDA: case 0xD9: break;
						default:
							i += ((buf[i+1] << 8) | (buf[i+2])) + 1;
							break;
					}
				}else i++;
			}
			// Decodifica la imagen
			AMG_InitMultimedia();	// Iniciamos los módulos necesarios (si no han sido iniciados)
			int r;
			if((r = sceJpegInitMJpeg()) != 0){ AMG_Error(AMG_MODULE_INIT, r, "SceJpeg"); return NULL;}
			sceJpegCreateMJpeg(tex->Width, tex->Height);
			if(sceJpegDecodeMJpeg(buf, size, tex->Data, 0) <= 0){ AMG_Error(AMG_CUSTOM_ERROR, 0, "Couldn't decode JPEG: \"%s\"", path); return NULL;}
			sceJpegDeleteMJpeg();
			sceJpegFinishMJpeg();
			free(buf); buf = NULL;
		}else{		// Textura incompatible
			AMG_Error(AMG_CUSTOM_ERROR, 0, "Unknown texture format: \"%s\"", path); return NULL;
		}
	}
#endif

	// Stride
	if(tex->Width > 512) AMG_Error(AMG_TEXTURE_OVERFLOW_SIZE, 0, "AMG_LoadTexture");
	tex->rw = tex->Width; tex->rh = tex->Height;
	AMG_MirrorTextureY(tex);
	
	// Convierte la textura, si es necesario
	if(AMG.TextureQuality != GU_PSM_8888){
		AMG_ConvertTexture(tex, AMG.TextureQuality);
	}
	
	// Haz Swizzle a la textura, si es necesario
	if(AMG.TextureSwizzle) AMG_SwizzleTexture(tex);
	
	// Transfiere la textura a VRAM, si es necesario
	if(load == AMG_TEX_VRAM){
		tex->Load = AMG_TEX_RAM;
		AMG_TransferTextureVram(tex);
	}
	
	// Devuelve la textura cargada
	return tex;
}

// Convierte el formato de una textura
void AMG_ConvertTexture(AMG_Texture *tex, u32 psm){
	if(tex->TexFormat != GU_PSM_8888){ AMG_Error(AMG_CONVERT_TEXTURE, 0, "not 32 BPP texture"); return;}
	if(tex->Swizzle) return;
	// Crea el nuevo buffer
	u16 *data = (u16*) calloc (AMG_GetVramSize(tex->Width, tex->Height, psm), sizeof(u8));
	if(data == NULL){
		AMG_Error(AMG_OUT_OF_RAM, AMG_GetVramSize(tex->Width, tex->Height, psm), "AMG_ConvertTexture"); return;
	}
	u32 i; u8 *clr;
	// Convierte la textura, pues
	switch(psm){
		case GU_PSM_4444:
			for(i=0;i<(tex->Width*tex->Height);i++){
				// Obten el RGB
				clr = (u8*)&tex->Data[i];
				clr[0] = (clr[0] >> 4) &0xF;
				clr[1] = (clr[1] >> 4) &0xF;
				clr[2] = (clr[2] >> 4) &0xF;
				clr[3] = (clr[3] >> 4) &0xF;
				// Recombínalo a R4G4B4A4
				data[i] = AMG_RGBA4444(clr[0], clr[1], clr[2], clr[3]);
			} break;
		case GU_PSM_5650:
			tex->TCC = GU_TCC_RGB;		// Es tontería poner alpha donde no lo hay
			for(i=0;i<(tex->Width*tex->Height);i++){
				// Obten el RGB
				clr = (u8*)&tex->Data[i];
				clr[0] = (clr[0] >> 3) &0x1F;
				clr[1] = (clr[1] >> 2) &0x3F;
				clr[2] = (clr[2] >> 3) &0x1F;
				// Recombínalo a R5G6B5
				data[i] = AMG_RGBA5650(clr[0], clr[1], clr[2]);
			} break;
		case GU_PSM_5551:
			for(i=0;i<(tex->Width*tex->Height);i++){
				// Obten el RGB
				clr = (u8*)&tex->Data[i];
				clr[0] = (clr[0] >> 3) &0x1F;
				clr[1] = (clr[1] >> 3) &0x1F;
				clr[2] = (clr[2] >> 3) &0x1F;
				// Recombínalo a R5G5B5A1
				data[i] = AMG_RGBA5551(clr[0], clr[1], clr[2], clr[3]?1:0);
			} break;
		default: AMG_Error(AMG_CONVERT_TEXTURE, 0, "Wrong PSM"); return;
	}
	// Actualiza el buffer
	tex->TexFormat = psm;
	if(tex->Load == AMG_TEX_RAM){
		free(tex->Data); tex->Data = NULL; tex->Data = (u32*) data;
	}else{
		tex->Load = AMG_TEX_RAM;
		AMG_FreeVram(tex->Data, (tex->Width*tex->Height)<<2); tex->Data = (u32*) data;
		AMG_TransferTextureVram(tex);
	}
}

// Transfiere una textura a VRAM
void AMG_TransferTextureVram(AMG_Texture *tex){
	if(tex->Load == AMG_TEX_RAM){
		u32 *vram_ptr = (u32*) AMG_VramAlloc(tex->Width, tex->Height, tex->TexFormat);
		if(vram_ptr != sceGeEdramGetAddr()){	// Si ha habido éxito pasamos la textura a VRAM
			memcpy(vram_ptr, tex->Data, AMG_GetVramSize(tex->Width, tex->Height, tex->TexFormat));
			free(tex->Data); tex->Data = NULL; tex->Data = vram_ptr;
			tex->Load = AMG_TEX_VRAM;
		}
	}
}

// Elimina una textura
void AMG_UnloadTexture(AMG_Texture *tex){
	if(tex == NULL) return;
	if(tex->Load == AMG_TEX_RAM){
		free(tex->Data);
	}else{
		AMG_FreeVram(tex->Data, AMG_GetVramSize(tex->Width, tex->Height, tex->TexFormat));
	}
	tex->Data = NULL;
}

// Voltea una imagen en el eje Y
void AMG_MirrorTextureY(AMG_Texture *tex){
	if(tex == NULL) return;
	if(tex->TexFormat != GU_PSM_8888){ AMG_Error(AMG_TEXTURE_WRONG_DATA, 0, "not 32 BPP texture (AMG_MirrorTextureY)"); return;}
	if(tex->Swizzle) return;
	u32 *buffer = NULL;
	if(tex->Load == AMG_TEX_RAM) buffer = (u32*) calloc (tex->Width * tex->Height, sizeof(u32));
	else buffer = (u32*) AMG_VramAlloc(tex->Width, tex->Height, tex->TexFormat);
	u32 x, y;
	for(y=0;y<tex->Height;y++){
		for(x=0;x<tex->Width;x++){
			buffer[x + (y * tex->Width)] = tex->Data[x + ((tex->Height - 1 - y) * tex->Width)];
		}
	}
	if(tex->Load == AMG_TEX_RAM) free(tex->Data);
	else AMG_FreeVram(tex->Data, AMG_GetVramSize(tex->Width, tex->Height, tex->TexFormat));
	tex->Data = buffer;
}

// Dibuja un Sprite 2D
void AMG_DrawSprite(AMG_Texture *tex){
	if(tex == NULL) return;
	AMG_EnableTexture(tex);
	AMG_Vertex_intTV *data2D = (AMG_Vertex_intTV*) sceGuGetMemory(sizeof(AMG_Vertex_intTV) * 4);
	float w2 = (float)(tex->Width >> 1);
	float h2 = (float)(tex->Height >> 1);
	float s = AMG_Sin(tex->Rot);
	float c = AMG_Cos(tex->Rot);
	data2D[0].u = 0;
	data2D[0].v = 0;
	data2D[0].x = (s16)(((-w2*c) - (-h2*s)) * tex->ScaleX) + tex->X;
	data2D[0].y = (s16)(((-w2*s) + (-h2*c)) * tex->ScaleY) + tex->Y;
	data2D[0].z = 0;
	data2D[1].u = tex->Width;
	data2D[1].v = 0;
	data2D[1].x = (s16)(((w2*c) - (-h2*s)) * tex->ScaleX) + tex->X;
	data2D[1].y = (s16)(((w2*s) + (-h2*c)) * tex->ScaleY) + tex->Y;
	data2D[1].z = 0;
	data2D[2].u = 0;
	data2D[2].v = tex->Height;
	data2D[2].x = (s16)(((-w2*c) - (h2*s)) * tex->ScaleX) + tex->X;
	data2D[2].y = (s16)(((-w2*s) + (h2*c)) * tex->ScaleY) + tex->Y;
	data2D[2].z = 0;
	data2D[3].u = tex->Width;
	data2D[3].v = tex->Height;
	data2D[3].x = (s16)(((w2*c) - (h2*s)) * tex->ScaleX) + tex->X;
	data2D[3].y = (s16)(((w2*s) + (h2*c)) * tex->ScaleY) + tex->Y;
	data2D[3].z = 0;
	sceGuColor(tex->SprColor);
	sceGuDrawArray(GU_TRIANGLE_STRIP, (GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D), 4, 0, data2D);
}

#ifndef AMG_COMPILE_ONELUA

// Crea un objeto 2D
void AMG_Create2dObject(AMG_Texture *tex, u32 psm, u8 vram){
	if(tex == NULL) return;
	// Voltea la imagen en el eje Y
	AMG_MirrorTextureY(tex);
	tex->TFX = GU_TFX_MODULATE; tex->MinFilter = GU_NEAREST; tex->MagFilter = GU_NEAREST;
	if((psm != GU_PSM_8888) && (psm != tex->TexFormat)){		// Si hay que convertir...
		if(tex->Load == AMG_TEX_VRAM){ AMG_Error(AMG_CUSTOM_ERROR, 0, "AMG_Create2dObject: Texture is in Vram"); return;}
		AMG_ConvertTexture(tex, psm);
	}
	
	// Si la imagen no es potencia de 2...
	tex->rw = nextPow2(tex->rw);
	tex->rh = nextPow2(tex->rh);
	
	// Transfiere la textura a VRAM, si es necesario
	if(vram && (tex->Load != AMG_TEX_VRAM)) AMG_TransferTextureVram(tex);
}

// Printf al estilo AMGLib
void AMG_Printf(AMG_Texture *tex, int x, int y, u32 color, const char *text){

	// Obtén la cadena de texto
	u32 l = strlen(text);
	AMG_Vertex_intTV *data2D = (AMG_Vertex_intTV*) sceGuGetMemory((l << 1) * sizeof(AMG_Vertex_intTV));
	
	// Parámetros de la fuente de texto
	u16 fontw = (tex->Width >> 4);
	u16 fonth = (tex->Height >> 4);
	s16 _x = x, _y = y;
	char c = 0;
	
	// Calcula la cadena de texto
	u32 i;
	for(i=0;i<l;i++){
		c = text[i];
		if(c == '\n'){	// Si es un salto de línea...
			_y += fonth;
			_x = (x - fontw);
			c = 0;
		}
		data2D[(i<<1)].u = ((c &0xF)*fontw);
		data2D[(i<<1)].v = ((c &0xF0)>>4)*fonth;
		data2D[(i<<1)].x = _x;
		data2D[(i<<1)].y = _y;
		data2D[(i<<1)].z = 0;
		data2D[(i<<1)+1].u = ((c &0xF)*fontw) + fontw;
		data2D[(i<<1)+1].v = (((c &0xF0)>>4)*fonth) + fonth;
		data2D[(i<<1)+1].x = _x+fontw;
		data2D[(i<<1)+1].y = _y+fonth;
		data2D[(i<<1)+1].z = 0;
		_x += fontw;
	}
	
	// Dibuja la cadena de texto
	AMG_EnableTexture(tex);
	sceGuColor(color);
	sceGuDrawArray(GU_SPRITES, (GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D), (l << 1), 0, data2D);
}

// Dibuja un sprite con texture-caché
void AMG_DrawSpriteCache(AMG_Texture *tex){
	if(tex == NULL) return;
	AMG_EnableTexture(tex);
	sceGuColor(tex->SprColor);
	
	// Dibuja el sprite
	int start, end;
	int dx = 0;
	int x = (tex->X - (tex->Width >> 1));
	int y = (tex->Y - (tex->Height >> 1));
	for(start = 0, end = tex->Width; start < end; start += tex->Width, dx += tex->Width){
		AMG_Vertex_intTV *vertices = (AMG_Vertex_intTV*) sceGuGetMemory(2 * sizeof(AMG_Vertex_intTV));
		int width = (start + tex->Width) < end ? tex->Width : end-start;
		vertices[0].u = start; vertices[0].v = 0;
		vertices[0].x = dx+x; vertices[0].y = y; vertices[0].z = 0;
		vertices[1].u = start + width; vertices[1].v = tex->Height;
		vertices[1].x = dx + width + x; vertices[1].y = tex->Height + y; vertices[1].z = 0;
		sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
	}
}

// Elimina un color de la imagen
void AMG_DeleteColor(AMG_Texture *tex, u32 color){
	if(tex == NULL) return;
	if(tex->TexFormat == GU_PSM_5650){ AMG_Error(AMG_CUSTOM_ERROR, 0, "AMG_DeleteColor: Texture is 5650"); return;}
	u32 i;
	u16 *d = (u16*) tex->Data;
	
	// Borra el color requerido
	for(i=0;i<(tex->Width*tex->Height);i++){
		if(tex->TexFormat == GU_PSM_8888){
			if(tex->Data[i] == color){
				tex->Data[i] &= ~(0xFF000000);
			}
		}else{
			if(tex->TexFormat == GU_PSM_5551){		// 5551
				if(d[i] == (color &0xFFFF)) d[i] &= ~(1 << 15);
			}else{									// 4444
				if(d[i] == (color &0xFFFF)) d[i] &= ~(0xF << 12);
			}
		}
	}
}

#endif

// Establece el numero de frames de la textura
void AMG_TextureFrames(AMG_Texture *tex, u8 nframes){
	if(tex == NULL) return;
	tex->NFrames = nframes;
	tex->Height /= nframes;
	if(tex->Height > 512){ AMG_Error(AMG_TEXTURE_OVERFLOW_SIZE, 0, "AMG_TextureFrames"); return;}
	tex->rh = nextPow2(tex->Height);
}

// Haz el efecto Swizzle a una textura (gana velocidad)
void AMG_SwizzleTexture(AMG_Texture *tex){
	if(tex == NULL) return;
	if(tex->Load == AMG_TEX_VRAM) return;
	if(tex->Swizzle) return;
	
	// Crea el buffer
	u32 size = AMG_GetVramSize(tex->Width, tex->Height, tex->TexFormat);
	u8 *out = (u8*) calloc (size, sizeof(u8));
	
	// Variables adicionales
	const u8 *in = (const u8*)tex->Data;
	u32 width = (tex->Width * (size / (tex->Width*tex->Height)));
	u32 blockx, blocky, j;
	u32 width_blocks = (width / 16);
	u32 height_blocks = (tex->Height / 8);
	u32 src_pitch = (width - 16)/4;
	u32 src_row = width * 8;
	const u8* ysrc = in;
	u32* dst = (u32*)out;
	
	// Haz Swizzling
	for(blocky = 0; blocky < height_blocks; ++blocky){
		const u8* xsrc = ysrc;
		for(blockx = 0; blockx < width_blocks; ++blockx){
			const u32* src = (u32*)xsrc;
			for (j = 0; j < 8; ++j){
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				*(dst++) = *(src++);
				src += src_pitch;
			}
			xsrc += 16;
		}
		ysrc += src_row;
	}
	
	// Libera datos usados
	free(tex->Data); tex->Data = NULL; tex->Data = (u32*)out;
	tex->Swizzle = 1;
}

#ifndef AMG_COMPILE_ONELUA

// Haz una screenshot en formato BMP24
void AMG_Screenshot(const char *path){
	// Obten el framebuffer
	void *fbv; int w, pf;
	u8 *data = (u8*) calloc (AMG.ScreenWidth*AMG.ScreenHeight*4, sizeof(u8));
	sceDisplayWaitVblankStart();
	sceDisplayGetFrameBuf(&fbv, &w, &pf, PSP_DISPLAY_SETBUF_NEXTFRAME);
	u8 *fb = (u8*) fbv;
	
	// Transforma la imagen a RGB32
	if(data == NULL) return;
	u32 x, y;
	u16 *fb16 = (u16*) fb; u16 clr;
	switch(AMG.PSM){
		case GU_PSM_8888:
			for(y=0;y<AMG.ScreenHeight;y++){
				for(x=0;x<AMG.ScreenWidth;x++){
					data[((x+(y*AMG.ScreenWidth))<<2)] = fb[((x+(y*AMG.ScreenStride))<<2)];
					data[((x+(y*AMG.ScreenWidth))<<2)+1] = fb[((x+(y*AMG.ScreenStride))<<2)+1];
					data[((x+(y*AMG.ScreenWidth))<<2)+2] = fb[((x+(y*AMG.ScreenStride))<<2)+2];
					data[((x+(y*AMG.ScreenWidth))<<2)+3] = 0xFF;
				}
			} break;
		case GU_PSM_5551:
			for(y=0;y<AMG.ScreenHeight;y++){
				for(x=0;x<AMG.ScreenWidth;x++){
					clr = fb16[(x+(y*AMG.ScreenStride))];
					data[((x+(y*AMG.ScreenWidth))<<2)] = (clr &0x1F) << 3;
					data[((x+(y*AMG.ScreenWidth))<<2)+1] = ((clr >> 5) &0x1F) << 3;
					data[((x+(y*AMG.ScreenWidth))<<2)+2] = ((clr >> 10) &0x1F) << 3;
					data[((x+(y*AMG.ScreenWidth))<<2)+3] = 0xFF;
				}
			} break;
		case GU_PSM_4444:
			for(y=0;y<AMG.ScreenHeight;y++){
				for(x=0;x<AMG.ScreenWidth;x++){
					clr = fb16[(x+(y*AMG.ScreenStride))];
					data[((x+(y*AMG.ScreenWidth))<<2)] = (clr &0xF) << 4;
					data[((x+(y*AMG.ScreenWidth))<<2)+1] = ((clr >> 4) &0xF) << 4;
					data[((x+(y*AMG.ScreenWidth))<<2)+2] = ((clr >> 8) &0xF) << 4;
					data[((x+(y*AMG.ScreenWidth))<<2)+3] = 0xFF;
				}
			} break;
		case GU_PSM_5650:
			for(y=0;y<AMG.ScreenHeight;y++){
				for(x=0;x<AMG.ScreenWidth;x++){
					clr = fb16[(x+(y*AMG.ScreenStride))];
					data[((x+(y*AMG.ScreenWidth))<<2)] = (clr &0x1F) << 3;
					data[((x+(y*AMG.ScreenWidth))<<2)+1] = ((clr >> 5) &0x3F) << 2;
					data[((x+(y*AMG.ScreenWidth))<<2)+2] = ((clr >> 11) &0x1F) << 3;
					data[((x+(y*AMG.ScreenWidth))<<2)+3] = 0xFF;
				}
			} break;
		default: free(data); data = NULL; return;
	}
	
	// Abre el archivo y guarda la imagen
	int error = lodepng_encode32_file(path, data, AMG.ScreenWidth, AMG.ScreenHeight);
	if(error){ AMG_Error(AMG_CUSTOM_ERROR, 0, lodepng_error_text(error)); return;}
	free(data); data = NULL;
}

#endif

// Establece el mapeo de una textura
void AMG_SetTextureMapping(AMG_Texture *tex, u32 mapping, u8 l0, u8 l1){
	// Mapea la textura correspondiente
	if(tex == NULL) return;
	tex->Mapping = mapping;
	tex->MappingLights[0] = l0;
	tex->MappingLights[1] = l1;
}

// Activa el Render To Texture
void AMG_EnableRenderToTexture(AMG_Texture *tex){
	if(tex == NULL) return;
	if(tex->Load != AMG_TEX_VRAM) return;
	sceGuDrawBufferList(tex->TexFormat, (void*)((u32)tex->Data - (u32)sceGeEdramGetAddr()), tex->Width);
	sceGuOffset(2048 - (tex->Width>>1), 2048 - (tex->Height>>1));
	sceGuViewport(2048, 2048, tex->Width, tex->Height);
	sceGuClearColor(AMG.ClearColor);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT | GU_STENCIL_BUFFER_BIT);
}

// Desactiva el Render To Texture
void AMG_DisableRenderToTexture(void){
	sceGuDrawBufferList(AMG.PSM, AMG.CurrentFB, AMG.ScreenStride);
	sceGuOffset(2048 - (AMG.ScreenWidth >> 1), 2048 - (AMG.ScreenHeight >> 1));
	sceGuViewport(2048, 2048, AMG.ScreenWidth, AMG.ScreenHeight);
}

// Cambia el pixel de una textura
void AMG_ChangeTexturePixel(AMG_Texture *tex, u32 x, u32 y, u32 color){
	if(tex == NULL) return;
	if(tex->Swizzle) return;
	if((x >= tex->Width) || (y >= tex->Height)) return;
	u32 offset = (x + (y * tex->Width));
	if(tex->TexFormat == GU_PSM_8888) tex->Data[offset] = color;
	else ((u16*)tex->Data)[offset] = (color &0xFFFF);
}

// Obtén un pixel de una textura
u32 AMG_GetTexturePixel(AMG_Texture *tex, u32 x, u32 y){
	if(tex == NULL) return 0;
	if(tex->Swizzle) return 0;
	if((x >= tex->Width) || (y >= tex->Height)) return 0;
	u32 offset = (x + (y * tex->Width));
	if(tex->TexFormat == GU_PSM_8888) return tex->Data[offset];
	return ((((u16*)tex->Data)[offset]) &0xFFFF);
}
