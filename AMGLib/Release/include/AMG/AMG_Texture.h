#ifndef _AMG_TEXTURE_H_
#define _AMG_TEXTURE_H_

#ifdef __cplusplus
	extern "C" {
#endif

// Includes
#include <psptypes.h>
#include <stdarg.h>
#include "AMG_Config.h"
#include <pspgu.h>

#ifdef AMG_DOC_ENGLISH
/**
 * @file AMG_Texture.h
 * @brief Functions related to texture loading, and 2D sprite management
 * @author Andrés Martínez (Andresmargar)
 */
#else
/**
 * @file AMG_Texture.h
 * @brief Funciones encargadas de la carga de texturas, y manejo de sprites 2D
 * @author Andrés Martínez (Andresmargar)
 */
#endif

/******************************************************/
/************** TEXTURAS ******************************/
/******************************************************/

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Struct that holds a texture
 */
#else
/**
 * @struct
 * @brief Estructura que almacena una textura
 */
#endif
typedef struct{
	u8 Load;			/**< AMG_TEX_RAM o AMG_TEX_VRAM, no modificar / AMG_TEX_RAM or AMG_TEX_VRAM, don't write */
	u32 Width;			/**< Ancho de la textura en píxeles / Texture width in pixels */
	u32 Height;			/**< Alto de la textura en píxeles / Texture height in pixels */
	u32 TexFormat;		/**< Formato de la textura, no modificar / Texture Format, don't write*/
	u32 TFX;			/**< GU_TFX_MODULATE, GU_TFX_DECAL etc */
	u32 TCC;			/**< No modificar / Don't write */
	u32 EnvColor;		/**< Color de la textura si TFX != GU_TFX_DECAL / Texture color if TFX != GU_TFX_DECAL */
	u32 MinFilter;		/**< Filtro cuando la textura está cerca de la cámara (GU_NEAREST, GU_LINEAR etc) / Filter when this texture is near from the frustum (GU_NEAREST, GU_LINEAR etc) */
	u32 MagFilter;		/**< Filtro cuando la textura está lejos de la cámara (GU_NEAREST, GU_LINEAR etc) / Filter when this texture is far from the frustum (GU_NEAREST, GU_LINEAR etc) */
	u8 Swizzle, NMipmaps;	/**< Número de Mipmaps / Number of Mipmaps */
	u8 NFrames;			/**< Número de frames de la textura, no modificar / Number of frames, don't write */
	u8 Frame;			/**< Frame a mostrar de la textura / Current frame of the texture*/
	u32 *Data;			/**< Datos de píxeles de la textura (1 elemento = 1 pixel RGBA) / Pixel data (1 element = 1 RGBA pixel) */
	float U;			/**< Coordenada U de traslación / U translation*/
	float V;			/**< Coordenada V de traslación / V translation */
	s16 X;			/**< Coordenada X del sprite / Sprite X */
	s16 Y;			/**< Coordenada Y del sprite / Sprite Y */
	u32 WrapX, WrapY;	/**< Valor de repetición de la textura / Wrapping value */
	u32 Mapping;		/**< Mapeo de la textura (normal o environment mapping) / Texture mapping (normal or environment mapping) */
	u8 MappingLights[2];/**< Las luces que harán de columnas de la matriz en environment mapping / Light which will work as env-map matrix's columns */
	float EnvMapRot, EnvMapX, EnvMapY;	/**< Parámetros del Environment Map / Env-Map parameters */
	u32 SprColor;		/**< El color del Sprite / Sprite color */
	float ScaleX, ScaleY;	/**< Escala del Sprite / Sprite Scale */
	float Rot;				/**< Rotación del Sprite / Sprite rotation */
	// USO INTERNO, NO MODIFICAR
	u32 rw, rh;
}AMG_Texture;

// Defines
#define AMG_TEX_RAM 0
#define AMG_TEX_VRAM 1
#define AMG_RGBA5650(r, g, b)		(((b) << 11) | ((g) << 5) | (r))
#define AMG_RGBA4444(r, g, b, a)	(((a) << 12) | ((b) << 8) | ((g) << 4) | (r))
#define AMG_RGBA5551(r, g, b, a)	(((a) << 15) | ((b) << 10) | ((g) << 5) | (r))

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Enables a texture
 * @param tex Texture to enable
 * @return It returns nothing
 */
#else
/**
 * @brief Activa una textura
 * @param tex La textura a activar
 * @return No devuelve nada
 */
#endif
void AMG_EnableTexture(AMG_Texture *tex);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Allocates a VRAM block for shared textures
 * @param w Maximum texture width
 * @param h Maximum texture height
 * @param psm Maximum pixelformat of shared textures
 * @return It returns nothing
 * @note Due to RAM textures are slow to show, and not always there is enough VRAM, you can create
 * @note a reserved block of VRAM to transfer via GE in real time, every RAM texture you want, increasing speed
 */
#else
/**
 * @brief Reserva un espacio de VRAM para texturas compartidas
 * @param w El ancho de la textura (como máximo)
 * @param h El alto de la textura (como máximo)
 * @param psm La calidad máxima de las texturas compartidas
 * @return No devuelve nada
 * @note Debido a que las texturas en RAM son lentas de mostrar, y no siempre hay espacio en VRAM, puedes crear 
 * @note un espacio reservado en VRAM para transferir en tiempo real vía GE, todas las texturas de RAM que desees, aumentando la velocidad
 */
#endif
void AMG_AllocateSharedTexture(u32 w, u32 h, u32 psm);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Deletes the reserved block of VRAM for shared textures
 * @return It returns nothing
 */
#else
/**
 * @brief Elimina el espacio reservado para texturas compartidas
 * @return No devuelve nada
 */
#endif
void AMG_FreeSharedTexture(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Loads a texture
 * @param path Texture file path
 * @param load AMG_TEX_RAM or AMG_TEX_VRAM
 * @return Pointer where the texture is loaded
 */
#else
/**
 * @brief Carga una textura
 * @param path Ruta donde está la textura
 * @param load AMG_TEX_RAM o AMG_TEX_VRAM
 * @return Puntero a la textura donde se carga
 */
#endif
AMG_Texture *AMG_LoadTexture(char *path, u8 load);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Changes texture pixelformat
 * @param tex Texture to convert
 * @param psm GU_PSM_4444, GU_PSM_5650 or GU_PSM_5551
 * @return It returns nothing
 * @note Texture quality MUST be GU_PSM_8888
 * @note To transfer a texture to VRAM, use AMG_TransferTextureVram();
 */
#else
/**
 * @brief Convierte el formato de una textura
 * @param tex La textura a convertir
 * @param psm El PSM: GU_PSM_4444, GU_PSM_5650 o GU_PSM_5551
 * @return No devuelve nada
 * @note Su calidad debe ser GU_PSM_8888
 * @note Para pasar la textura convertida a VRAM, usar la función AMG_TransferTextureVram();
 */
#endif
void AMG_ConvertTexture(AMG_Texture *tex, u32 psm);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Transfers a texture from RAM to VRAM
 * @param tex Texture to transfer
 * @return It returns nothing
 */
#else
/**
 * @brief Transfiere una textura en RAM a VRAM
 * @param tex La textura a transferir
 * @return No devuelve nada
 */
#endif
void AMG_TransferTextureVram(AMG_Texture *tex);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Deletes a texture
 * @param tex Texture to delete
 * @return It returns nothing
 */
#else
/**
 * @brief Elimina una textura
 * @param tex La textura a eliminar
 * @return No devuelve nada
 */
#endif
void AMG_UnloadTexture(AMG_Texture *tex);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Draws a 2D Sprite
 * @param tex Texture which works as a sprite
 * @return It returns nothing
 * @note Important: Convert the texture to sprite using AMG_Create2dObject();
 */
#else
/**
 * @brief Dibuja un Sprite 2D
 * @param tex La textura que hace de sprite
 * @return No devuelve nada
 * @note Importante convertir la textura en sprite con la función AMG_Create2dObject();
 */
#endif
void AMG_DrawSprite(AMG_Texture *tex);

#ifndef AMG_COMPILE_ONELUA

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Prints text in screen
 * @param tex Texture to enable
 * @param x X coordinate
 * @param y Y coordinate
 * @param color Text color (GU_RGBA(r, g, b, a) macro)
 * @param text Text to write
 * @return It returns nothing
 */
#else
/**
 * @brief Imprime texto en pantalla
 * @param tex La textura a activar
 * @param x Coordenada X donde comienza
 * @param y Coordenada Y donde comienza
 * @param color El color del texto (macro GU_RGBA(r, g, b, a))
 * @param text El texto a escribir
 * @return No devuelve nada
 */
#endif
void AMG_Printf(AMG_Texture *tex, int x, int y, u32 color, const char *text);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Draws a 2D Sprite (using texture-cache)
 * @param tex Texture which works as a sprite
 * @return It returns nothing
 * @note Important: Convert the texture to sprite using AMG_Create2dObject(), this version does not allow rotation/scaling
 */
#else
/**
 * @brief Dibuja un Sprite 2D (optimizando el uso del caché de texturas)
 * @param tex La textura que hace de sprite
 * @return No devuelve nada
 * @note Importante convertir la textura en sprite con la función AMG_Create2dObject(), esta versión no soporta rotación/escalado
 */
#endif
void AMG_DrawSpriteCache(AMG_Texture *tex);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Converts a texture to a 2D object
 * @param tex Texture to convert
 * @param psm If you want to change pixelformat to 16 bits (more FPS)
 * @param vram If you want to transfer the texture to VRAM (more FPS)
 * @return It returns nothing
 */
#else
/**
 * @brief Convierte una textura en un objeto 2D
 * @param tex La textura a convertir
 * @param psm Si quieres convertir la imagen a un formato de menor calidad (más FPS)
 * @param vram Si quieres pasar la textura a VRAM (siempre se recomienda para no perder FPS)
 * @return No devuelve nada
 */
#endif
void AMG_Create2dObject(AMG_Texture *tex, u32 psm, u8 vram);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief "Deletes" a color in a texture (makes it transparent)
 * @param tex Texture to modify
 * @param color Color to delete
 * @return It returns nothing
 */
#else
/**
 * @brief "Borra" un color de una textura (lo hace transparente)
 * @param tex La textura a modificar
 * @param color El color a transparentar
 * @return No devuelve nada
 */
#endif
void AMG_DeleteColor(AMG_Texture *tex, u32 color);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Takes a screenshot in PNG format
 * @param path Where the image will be saved
 * @return It returns nothing
 */
#else
/**
 * @brief Toma una captura de pantalla en formato PNG
 * @param path La ruta donde se guardará la captura
 * @return No devuelve nada
 */
#endif
void AMG_Screenshot(const char *path);

#endif

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Flips a texture in Y axis
 * @param tex Texture to flip
 * @return It returns nothing
 */
#else
/**
 * @brief Voltea una textura en el eje Y
 * @param tex La textura a voltear
 * @return No devuelve nada
 */
#endif
void AMG_MirrorTextureY(AMG_Texture *tex);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Swizzles a texture (optimization for the GE)
 * @param tex Texture to swizzle
 * @return It returns nothing
 * @note When a texture is not going to be modified in real time, it's highly recommended to optimize it
 */
#else
/**
 * @brief Haz Swizzle a una textura (optimiza la textura para el GE)
 * @param tex La textura a optimizar
 * @return No devuelve nada
 * @note Siempre que la textura no se vaya a modificar en tiempo real, es altamente recomendable optimizarla
 */
#endif
void AMG_SwizzleTexture(AMG_Texture *tex);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Disables textures
 * @return It returns nothing
 * @note This function only calls to sceGuDisable(GU_TEXTURE_2D);
 * @see <pspgu.h>
 */
#else
/**
 * @brief Desactiva las texturas
 * @return No devuelve nada
 * @note Esta función solo llama a sceGuDisable(GU_TEXTURE_2D);
 * @see <pspgu.h>
 */
#endif
static inline void AMG_DisableTexture(void){
	sceGuDisable(GU_TEXTURE_2D);
}

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Set the number of frames of a texture
 * @param tex Texture
 * @param nframes Number of frames
 * @return It returns nothing
 * @note Frames must be placed VERTICALLY, it's not necessary to convert the texture to Sprite
 */
#else
/**
 * @brief Establece el número de fotogramas (frames) de una textura
 * @param tex La textura
 * @param nframes El número de frames que tiene
 * @return No devuelve nada
 * @note Los frames deben estar organizados en VERTICAL, no es necesario convertir la textura en Sprite
 */
#endif
void AMG_TextureFrames(AMG_Texture *tex, u8 nframes);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Set the texture mapping
 * @param tex Texture
 * @param mapping Mapping (GU_ENVIRONMENT_MAP, GU_TEXTURE_COORDS...)
 * @param l0 Light to use (these don't work as lights, only as columns of the mapping matrix)
 * @param l1 Second light to use (that matrix is 2x3)
 */
#else
/**
 * @brief Establece el mapping de una textura
 * @param tex La textura
 * @param mapping El tipo de mapeado (GU_ENVIRONMENT_MAP, GU_TEXTURE_COORDS...)
 * @param l0 La luz a usar (no funcionan como luces, sino como columnas de la matriz de mapeado)
 * @param l1 La segunda luz a usar (dicha matriz es de 2x3)
 */
#endif
void AMG_SetTextureMapping(AMG_Texture *tex, u32 mapping, u8 l0, u8 l1);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Creates an empty texture
 * @param width Texture width
 * @param height Texture height
 * @param psm Texture pixelformat
 * @param load Where it's located (RAM or VRAM)
 * @return The created texture
 */
#else
/**
 * @brief Crea una textura vacía
 * @param width El ancho de la textura
 * @param height El alto de la textura
 * @param psm La calidad de imagen
 * @param load Dónde está la imagen (RAM o VRAM)
 * @return La textura
 */
#endif
AMG_Texture *AMG_CreateTexture(u16 width, u16 height, u32 psm, u8 load);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Enables "Render to Texture"
 * @param tex Texture where rendering is performed
 * @return It returns nothing
 */
#else
/**
 * @brief Activa el "Render to Texture"
 * @param tex La textura donde se va a renderizar
 * @return No devuelve nada
 */
#endif
void AMG_EnableRenderToTexture(AMG_Texture *tex);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Disables "Render to Texture"
 * @return It returns nothing
 */
#else
/**
 * @brief Desactiva el "Render to Texture"
 * @return No devuelve nada
 */
#endif
void AMG_DisableRenderToTexture(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Changes a texture pixel
 * @param tex Texture
 * @param x X coordinate
 * @param y Y coordinate
 * @param color New color
 * @return It returns nothing
 */
#else
/**
 * @brief Cambia el pixel de una textura
 * @param tex La textura
 * @param x Coordenada X
 * @param y Coordenada Y
 * @param color El nuevo color
 * @return No devuelve nada
 */
#endif
void AMG_ChangeTexturePixel(AMG_Texture *tex, u32 x, u32 y, u32 color);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Get a texture pixel
 * @param tex Texture
 * @param x X coordinate
 * @param y Y coordinate
 * @return Color at this pixel
 */
#else
/**
 * @brief Obtén el pixel de una textura
 * @param tex La textura
 * @param x Coordenada X
 * @param y Coordenada Y
 * @return El color obtenido
 */
#endif
u32 AMG_GetTexturePixel(AMG_Texture *tex, u32 x, u32 y);

#ifdef __cplusplus
	}
#endif

#endif
