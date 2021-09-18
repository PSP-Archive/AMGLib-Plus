#ifndef _AMG_3D_H_
#define _AMG_3D_H_

#ifdef __cplusplus
	extern "C" {
#endif

// Includes
#include <psptypes.h>
#include <pspgu.h>
#include "AMG_Config.h"
#include <pspkernel.h>

#ifdef AMG_DOC_ENGLISH
/**
 * @file AMG_3D.h
 * @brief 3D engine functions
 * @author Andrés Martínez (Andresmargar)
 */
#else
/**
 * @file AMG_3D.h
 * @brief Funciones encargadas del módulo 3D
 * @author Andrés Martínez (Andresmargar)
 */
#endif
 
/******************************************************/
/************** TIPOS DE VERTICES *********************/
/******************************************************/
#ifdef AMG_DOC_ENGLISH
/**
 * @def
 * @brief Aligns a buffer to "n" bytes
 */
#else
/**
 * @def
 * @brief Alinea un buffer a "n" bytes
 */
#endif
#define AMG_ALIGN(n) __attribute__((aligned(n)))

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Struct which packs a vertex with texture 'n' point
 */
#else
/**
 * @struct
 * @brief Estructura que empaqueta un vértice con textura y punto
 */
#endif
typedef struct{
	float u, v;		/**< Coordenadas UV del vértice */
	float x, y, z;	/**< Coordenadas del punto */
}AMG_Vertex_TV;

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Struct which packs a vertex with texture 'n' point (integer)
 */
#else
/**
 * @struct
 * @brief Estructura que empaqueta un vértice con textura y punto (entero)
 */
#endif
typedef struct{
	u16 u, v;		/**< Coordenadas UV del vértice */
	s16 x, y, z;	/**< Coordenadas del punto */
}AMG_Vertex_intTV;

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Struct which packs a vertex with texture, normal 'n' point
 */
#else
/**
 * @struct
 * @brief Estructura que empaqueta un vértice con textura, normal y punto
 */
#endif
typedef struct{
	float u, v;			/**< Coordenadas UV del vértice */
	float nx, ny, nz;	/**< Normal del vértice */
	float x, y, z;		/**< Coordenadas del punto */
}AMG_Vertex_TNV;

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Struct which packs a vertex with normal 'n' point
 */
#else
/**
 * @struct
 * @brief Estructura que empaqueta un vértice con normal y punto
 */
#endif
typedef struct{
	float nx, ny, nz;	/**< Normal del vértice */
	float x, y, z;		/**< Coordenadas del punto */
}AMG_Vertex_NV;

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Struct which packs a simple vertex
 */
#else
/**
 * @struct
 * @brief Estructura que empaqueta un vértice simple
 */
#endif
typedef struct{
	float x, y, z;		/**< Coordenadas del punto */
}AMG_Vertex_V;

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Struct which packs a vertex with color 'n' point
 */
#else
/**
 * @struct
 * @brief Estructura que empaqueta un vértice con color y punto
 */
#endif
typedef struct{
	u32 color;			/**< Color del vértice */
	float x, y, z;		/**< Coordenadas del punto */
}AMG_Vertex_CV;

/******************************************************/
/************** 3D ************************************/
/******************************************************/

#ifdef AMG_DOC_ENGLISH
/**
 * @def
 * @brief Convert degrees to radians
*/
#else
/**
 * @def
 * @brief Pasa de grados a radianes
 */
#endif
#define AMG_Deg2Rad(n) (((n)*GU_PI)/180.0f)

#ifdef AMG_DOC_ENGLISH
/**
 * @def
 * @brief Convert radians to degrees
*/
#else
/**
 * @def
 * @brief Pasa de radianes a grados
 */
#endif
#define AMG_Rad2Deg(n) (((n)*180.0f)/GU_PI)

#ifdef AMG_DOC_ENGLISH
/**
 * @def
 * @brief A 3D vector
 */
#else
/**
 * @def
 * @brief Un vector tridimensional
 */
#endif
#define AMG_Vector ScePspFVector3

#ifdef AMG_DOC_ENGLISH
/**
 * @def
 * @brief Number of entries on the matrix stack
 */
#else
/**
 * @def 
 * @brief El número de entradas en la pila de matrices
 */
#endif
#define AMG_MATRIX_STACKSIZE 8

#ifndef AMG_DOC_ENGLISH
/**
 * @def
 * @brief La alineación de bloques en VRAM (cuanto más alto, menos memoria pero menos efectividad)
 */
#else
/**
 * @def
 * @brief VRAM block alignment (when higher, less memory usable but more effectivity)
 */
#endif
#define AMG_VRAM_ALIGN 64

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Light source structure
 * @note IMPORTANT: Don't use this struct directly, use the library-defined one: AMG_Light[n]
 */
#else
/**
 * @struct
 * @brief Estructura de una fuente lumínica
 * @note IMPORTANTE: No usar esta estructura directamente, sino la definida por la librería (AMG_Light[n])
 */
#endif
typedef struct{
	ScePspFVector4 Pos;
	u32 Type, Diffuse, Ambient, Specular, Component;
	float Attenuation[3];
	ScePspFVector3 SpotDirection;
	float SpotExponent, SpotCutoff;
}AMG_Light_;
extern AMG_Light_ AMG_Light[4];

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Camera struct
 */
#else
/**
 * @struct
 * @brief Estructura de una cámara
 */
#endif
typedef struct{
	ScePspFVector3 Pos, Eye, Up, Rot;
}AMG_Camera;

// Estructura de las variables globales
typedef struct{
	u32 ClearColor;					/**< El color de fondo de la pantalla / Screen's clear color*/
	u32 ScreenFadeColor;			/**< El color y transparencia de la pantalla posterior (para hacer efecto "Fade In" y "Fade Out")
										 / Color and transparency of FadeIn - FadeOut effects*/
	u32 *FB0, *FB1, *List;			/**< Buffers del motor 3D, NO MODIFICAR SIN SABER LO QUE SE ESTÁ HACIENDO / 3D engine buffers, DON'T MODIFY BY YOURSELF */
	u16 *ZB;
	int Exit;						/**< ¿Está ejecutándose AMGLib? / Is AMGLib running?*/
	u32 PSM;						/**< Calidad de imagen de Framebuffer / Framebuffer pixelformat */
	void *CurrentFB;				/**< El framebuffer actual / Current framebuffer */
	u8 TextureDest;					/**< Dónde se cargarán las texturas de modelos 3D (AMG_TEX_RAM o AMG_TEX_VRAM) / Where 3D model textures are loaded (AMG_TEX_RAM or AMG_TEX_VRAM) */
	u16 ScreenWidth, ScreenHeight, ScreenStride;	/**< Ancho y alto de la pantalla de la PSP / Width and height of PSP's screen */
	u8 TextureQuality;		/**< Calidad de las texturas al cargar / Texture quality when loading */
	u8 TextureSwizzle;		/**< Hacer Swizzle o no a las texturas / Swizzle textures when loading? */
	u8 DebugMode;			/**< Modo de DEBUG / DEBUG Mode */
	int ErrorCode;			/**< Último error ocurrido / Last error happened */
	char ErrorString[128];	/**< Cadena del error / String explaining the error */
	int ErrorDetail;		/**< Datos adicionales del error / Additional data of the error */
	int MainThreadID, CBID;		/**< ID del Thread del botón HOME / Homebutton Thread ID */
	u8 FPS;					/**< Los FPS / Frames per second */
	int GMT;				/**< Escribe aquí el GMT de tu zona / GMT of your zone (write it by yourself) */
	u8 Rendering;			/**< Estamos renderizando? / Rendering? */
	u8 WaitVblank;			/**< Esperar al VBlank? / Wait for VBlank? */
	u8 DoubleBuffer;		/**< Usando DoubleBuffer? / Using doublebuffer? */
	u8 Inited;				/**< Se ha iniciado el 3D? / 3D inited? */
	u8 MultimediaInited;	/**< Se ha iniciado el motor multimedia? / Multimedia engine inited? */
	u8 DrawMode;			/**< Modo de dibujado (GU_DIRECT o GU_CALL) / Drawing mode (GU_DIRECT or GU_CALL) */
	float WorldSpecular;	/**< La cantidad de brillo en la escena global / Brightness power on lighting */
	void *Lightmap;			/**< El lightmap usado para el Cel-Shading / Used lightmap for Cel-Shading */
	u32 DrawnVertices;		/**< El número de vértices dibujado / Amount of vertices drawn for this frame */
}AMG_;
extern AMG_ AMG;

// Variables privadas
extern u8 amg_updated, amg_curlist;

// Defines del DEBUG
#define AMG_ERRORMSG 0x001
#define AMG_ERRORCODE 0x0F

// Defines del AMG_Init3D()
#define AMG_DOUBLEBUFFER (0 << 4)
#define AMG_SINGLEBUFFER (1 << 4)
#define AMG_SCREEN_DIMENSIONS(w, h) ((((w) &0x3FF) << 5) | (((h) &0x3FF) << 15))

/****************************************************/
/****************** FUNCIONES ***********************/
/****************************************************/

// Enumera los códigos de error
enum AMG_ERRORCODE_ {
	AMG_CUSTOM_ERROR = 0,
	AMG_OUT_OF_RAM,
	AMG_OPEN_FILE,
	AMG_NULL_POINTER,
	AMG_THREAD_SETUP,
	AMG_OUT_OF_VRAM,
	AMG_MODULE_INIT,
	AMG_MODULE_EXCEPTION,
	AMG_CONVERT_TEXTURE,
	AMG_TEXTURE_WRONG_DATA,
	AMG_TEXTURE_OVERFLOW_SIZE
};

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Shows a message and stops the PSP
 * @param errorcode The errorcode
 * @param aux Additional data
 * @param text Text to show
 * @param ... Additional data (classic printf())
 * @return It returns nothing
 */
#else
/**
 * @brief Muestra un mensaje en pantalla y detiene el sistema
 * @param errorcode El código de error
 * @param aux Datos auxiliares
 * @param text El texto a mostrar
 * @param ... Datos adicionales (como el clásico printf())
 * @return No devuelve nada
 */
#endif
void AMG_Error(u8 errorcode, u32 aux, const char *text, ...);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Clears Data Cache
 * @param inv Invalidate writeback data?
 * @return It returns nothing
 */
#else
/**
 * @brief Limpia el DCACHE
 * @param inv Invalidar datos?
 * @return No devuelve nada
 */
#endif
static inline void AMG_ClearDCache(u8 inv){
	inv ? sceKernelDcacheWritebackInvalidateAll() : sceKernelDcacheWritebackAll();
}

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Get the size of a VRAM block
 * @param width Block width (in pixels)
 * @param height Block height (in pixels)
 * @param psm Block quality (GU_PSM_8888, GU_PSM_5551...)
 * @see <pspgu.h>
 * @return The size of the specified block
 */
#else
/**
 * @brief Obtener el tamaño de un bloque de VRAM
 * @param width El ancho del bloque (en pixeles)
 * @param height El alto del bloque (en pixeles)
 * @param psm La calidad de imagen del bloque (GU_PSM_8888, GU_PSM_5551...)
 * @see <pspgu.h>
 * @return El tamaño del bloque
 */
#endif
u32 AMG_GetVramSize(u32 width, u32 height, u32 psm);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Get a block of VRAM
 * @param width Block width (in pixels)
 * @param height Block height (in pixels)
 * @param psm Block quality (GU_PSM_8888, GU_PSM_5551...)
 * @see <pspgu.h>
 * @return A pointer to the allocated block (it returns sceGeEdramGetAddr() when failing)
 */
#else
/**
 * @brief Obtener un bloque de VRAM
 * @param width El ancho del bloque (en pixeles)
 * @param height El alto del bloque (en pixeles)
 * @param psm La calidad de imagen del bloque (GU_PSM_8888, GU_PSM_5551...)
 * @see <pspgu.h>
 * @return Un puntero al bloque (devuelve sceGeEdramGetAddr() si no queda espacio)
 */
#endif
void* AMG_VramAlloc(u32 width, u32 height, u32 psm);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Free a block of VRAM
 * @param buffer VRAM buffer to free
 * @param i0 Internal usage, send 0
 * @return It returns nothing
 */
#else
/**
 * @brief Libera un bloque de VRAM
 * @param buffer El buffer de VRAM a liberar
 * @param i0 Uso interno del sistema, poner 0
 * @return No devuelve nada
 */
#endif
void AMG_FreeVram(void *buffer, u32 i0);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Inits the PSP's 3D Engine
 * @param options Options, such as pixelformat and screen dimensions (for TV mode)
 * @return It returns nothing
 * @note To let shadows and reflection to work, pixelformat must be GU_PSM_8888
 */
#else
/**
 * @brief Inicializar el motor 3D de la PSP
 * @param options Opciones, como el formato de imagen o las dimensiones de la pantalla (modo TV)
 * @return No devuelve nada
 * @note Para que funcionen las sombras y reflejos, debe estar en GU_PSM_8888
 */
#endif
void AMG_Init3D(int options);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Begins 3D drawing
 * @return It returns nothing
 * @note Obligatory before drawing anything
 */
#else
/**
 * @brief Comenzar el dibujado en 3D
 * @return No devuelve nada
 * @note Obligatorio ponerlo antes de dibujar nada
 */
#endif
void AMG_Begin3D(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Update screen and render 3D contents
 * @return It returns nothing
 * @note Obligatory to show what you draw on screen
 */
#else
/**
 * @brief Actualizar la pantalla y dibujar contenido 3D
 * @return No devuelve nada
 * @note Obligatorio ponerlo para mostrar lo dibujado en pantalla
 */
#endif
void AMG_Update3D(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Free used data and close 3D engine
 * @return It returns nothing
 */
#else
/**
 * @brief Liberar datos usados y cerrar el motor 3D
 * @return No devuelve nada
 */
#endif
void AMG_Finish3D(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Configurates 3D
 * @param dmode Drawing mode (GU_DIRECT or GU_CALL)
 * @param dlsize Display List size (send 0 if you don't want to change it)
 * @return It returns nothing
 */
#else
/**
 * @brief Configura el 3D
 * @param dmode El modo de dibujado (GU_DIRECT, GU_CALL)
 * @param dlsize El tamaño de la display list (0 si no quieres cambiarlo)
 * @return No devuelve nada
 */
#endif
void AMG_Config3D(u8 dmode, u32 dlsize);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Set the lightmap for Cel-Shading
 * @param path Lightmap's path
 * @return Ir returns nothing
 */
#else
/**
 * @brief Cambia el Lightmap para el Cel-Shading
 * @param path La ruta del lightmap
 * @return No devuelve nada
 */
#endif
void AMG_SetLightmap(char *path);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Enable or disable 2D rendering
 * @param _2d true/false to enable/disable
 * @return It returns nothing
 */
#else
/**
 * @brief Activar o desactivar el modo 2D
 * @param _2d true/false si se activa/desactiva
 * @return No devuelve nada
 */
#endif
void AMG_OrthoMode(u8 _2d);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Enables a light source
 * @param n light ID (0-3)
 * @return It returns nothing
 */
#else
/**
 * @brief Activar una fuente de luz
 * @param n ID de la luz (0-3)
 * @return No devuelve nada
 */
#endif
void AMG_EnableLight(u8 n);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Disables a light source
 * @param n Light ID (0-3)
 * @return It returns nothing
 */
#else
/**
 * @brief Desactivar una fuente de luz
 * @param n ID de la luz (0-3)
 * @return No devuelve nada
 */
#endif
static inline void AMG_DisableLight(u8 n){
	sceGuDisable(GU_LIGHT0 + n);
}

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Disables global lighting
 * @return It returns nothing
 */
#else
/**
 * @brief Desactivar la iluminación global
 * @return No devuelve nada
 */
#endif
static inline void AMG_DisableLightAll(void){
	sceGuDisable(GU_LIGHT0);
	sceGuDisable(GU_LIGHT1);
	sceGuDisable(GU_LIGHT2);
	sceGuDisable(GU_LIGHT3);
	sceGuDisable(GU_LIGHTING);
}

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Enables fog
 * @param near Value where fog starts (in Z axis)
 * @param far Value where fog ends (in Z axis)
 * @param color Fog color (GU_RGBA(r, g, b, a) macro)
 * @return It returns nothing
 */
#else
/**
 * @brief Activar la niebla
 * @param near Valor donde empieza la niebla en el eje Z
 * @param far Valor donde termina la niebla en el eje Z
 * @param color Color de la niebla (macro GU_RGBA(r, g, b, a))
 * @return No devuelve nada
 */
#endif
void AMG_EnableFog(float near, float far, u32 color);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Disables fog
 * @return It returns nothing
 */
#else
/**
 * @brief Desactivar la niebla
 * @return No devuelve nada
 */
#endif
static inline void AMG_DisableFog(void){
	sceGuDisable(GU_FOG);
}

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Creates a camera with default parameters
 * @return Pointer to the created camera
 */
#else
/**
 * @brief Inicializar una cámara con sus valores por defecto
 * @return Puntero hacia la cámara
 */
#endif
AMG_Camera *AMG_InitCamera(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Set a camera
 * @param cam Pointer to a camera
 * @return It returns nothing
 */
#else
/**
 * @brief Establecer una cámara
 * @param cam Puntero hacia la cámara
 * @return No devuelve nada
 */
#endif
void AMG_SetCamera(AMG_Camera *cam);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Enables/Disable antialiasing
 * @param i Wether to enable/disable this state
 * @return It returns nothing
 */
#else
/**
 * @brief Activa/Desactiva el Antialias
 * @param i Activa/Desactiva
 * @return No devuelve nada
 */
#endif
static inline void AMG_SetAntialias(u8 i){
	if(i) sceGuEnable(GU_LINE_SMOOTH);
	else sceGuDisable(GU_LINE_SMOOTH);
}

/******************************************************/
/************** VFPU **********************************/
/******************************************************/

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Loads identity matrix
 * @param mt Default matrices, or your own ones
 * @return It returns nothing
 */
#else
/**
 * @brief Carga la matriz identidad
 * @param mt Las matrices de la librería, o tu propia matriz
 * @return No devuelve nada
 */
#endif
void AMG_LoadIdentity(u32 mt);
void AMG_LoadIdentityUser(ScePspFMatrix4 *mtx);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Translates a matrix
 * @param mt Default matrices, or your own ones
 * @param v Translation vector
 * @return It returns nothing
 */
#else
/**
 * @brief Translada una matriz
 * @param mt Las matrices de la librería, o tu propia matriz
 * @param v El vector de traslación
 * @return No devuelve nada
 */
#endif
void AMG_Translate(u32 mt, ScePspFVector3 *v);
void AMG_TranslateUser(ScePspFMatrix4 *mtx, ScePspFVector3* v);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Scales a matrix
 * @param mt Default matrices, or your own ones
 * @param v scale vector
 * @return It returns nothing
 */
#else
/**
 * @brief Escala una matriz
 * @param mt Las matrices de la librería, o tu propia matriz
 * @param v El vector de escalado
 * @return No devuelve nada
 */
#endif
void AMG_Scale(u32 mt, ScePspFVector3 *v);
void AMG_ScaleUser(ScePspFMatrix4 *mtx, ScePspFVector3 *v);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Rotates a matrix
 * @param mt Default matrices, or your own ones
 * @param v Rotation vector
 * @return It returns nothing
 */
#else
/**
 * @brief Rota una matriz
 * @param mt Las matrices de la librería, o tu propia matriz
 * @param v El vector de rotación
 * @return No devuelve nada
 */
#endif
void AMG_Rotate(u32 mt, ScePspFVector3 *v);
void AMG_RotateUser(ScePspFMatrix4 *mtx, ScePspFVector3 *v);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Simulates a camera (lookAt)
 * @param eye Camera position
 * @param center Where it looks at
 * @param up Normalized vector which points to up
 * @return It returns nothing
 */
#else
/**
 * @brief Simula una cámara (lookAt)
 * @param eye La posición de la cámara
 * @param center Dónde mira
 * @param up El vector que apunta hacia arriba (normalizado)
 * @return No devuelve nada
 */
#endif
void AMG_LookAt(ScePspFVector3 *eye, ScePspFVector3 *center, ScePspFVector3 *up);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Normalizes a vector
 * @param v The vector to normalize
 * @return It returns nothing
 */
#else
/**
 * @brief Normaliza un vector
 * @param v El vector a normalizar
 * @return No devuelve nada
 */
#endif
void AMG_Normalize(ScePspFVector3 *v);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Calculates cross product of 2 vectores
 * @param v1 First vector
 * @param v2 Second vector
 * @param r Result vector
 * @return It returns nothing
 */
#else
/**
 * @brief Calcula el producto cruz entre 2 vectores
 * @param v1 El primer vector
 * @param v2 El segundo vector
 * @param r El vector resultante
 * @return No devuelve nada
 */
#endif
void AMG_CrossProduct(ScePspFVector3 *v1, ScePspFVector3 *v2, ScePspFVector3 *r);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Calculates dot product of 2 vectors
 * @param v1 First vector
 * @param v2 Second vector
 * @return Dot product
 */
#else
/**
 * @brief Calcula el producto mixto entre 2 vectores
 * @param v1 El primer vector
 * @param v2 El segundo vector
 * @return El producto mixto
 */
#endif
float AMG_DotProduct(ScePspFVector4 *v1, ScePspFVector4 *v2);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Inits matrix system
 * @param fov Perspective FOV
 * @return It returns nothing
 */
#else
/**
 * @brief Inicializa las matrices
 * @param fov El FOV de la perspectiva
 * @return No devuelve nada
 */
#endif
void AMG_InitMatrixSystem(float fov);

/** Wrapper por si quieres usarlo así también / Wrapper if you want to use it this way **/
#define AMG_SetPerspectiveFOV(fov) AMG_InitMatrixSystem(fov)

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Destroys matrix system
 * @return It returns nothing
 */
#else
/**
 * @brief Destruye las matrices
 * @return No devuelve nada
 */
#endif
void AMG_DestroyMatrixSystem(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Multiplies two matrices
 * @param mt Matrix type (defined by GU)
 * @param m Matrix to multiply with
 * @return It returns nothing
 */
#else
/**
 * @brief Multiplica dos matrices
 * @param mt El tipo de matriz (definido por GU)
 * @param m La matriz a multiplicar
 * @return No devuelve nada
 */
#endif
void AMG_MultMatrix(u8 mt, ScePspFMatrix4 *m);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Updates matrices in GPU memory
 * @return It returns nothing
 */
#else
/**
 * @brief Actualiza las matrices en la GPU
 * @return No devuelve nada
 */
#endif
void AMG_UpdateMatrices(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Pushes a matrix onto the stack
 * @param mt Matrix to push
 * @return It returns nothing
 */
#else
/**
 * @brief Guarda una matriz en la pila
 * @param mt La matriz a guardar
 * @return No devuelve nada
 */
#endif
void AMG_PushMatrix(u8 mt);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Pops a matrix onto the stack
 * @param mt Matrix to pop
 * @return It returns nothing
 */
#else
/**
 * @brief Restaura una matriz de la pila
 * @param mt La matriz a restaurar
 * @return No devuelve nada
 */
#endif
void AMG_PopMatrix(u8 mt);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Sine function applied to VFPU
 * @param angle Angle
 * @return sin(angle)
 */
#else
/**
 * @brief La función seno aplicada a VFPU
 * @param angle angulo a tratar
 * @return El seno de ese angulo
 */
#endif
float AMG_Sin(float angle);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Cosine function applied to VFPU
 * @param angle Angle
 * @return cos(angle)
 */
#else
/**
 * @brief La función coseno aplicada a VFPU
 * @param angle angulo a tratar
 * @return El coseno de ese angulo
 */
#endif
float AMG_Cos(float angle);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Square root function applied to VPFU
 * @param val Value to calculate
 * @return sqrt(val)
 */
#else
/**
 * @brief La función raíz cuadrada aplicada a VFPU
 * @param val El radicando
 * @return El resultado de la raíz
 */
#endif
float AMG_SquareRoot(float val);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Random number between "min" & "max", applied to VFPU
 * @param min Minimum value
 * @param max Maximum value
 * @return A random number between these values
 */
#else
/**
 * @brief Un número aleatorio entre "min" y "max" aplicado a VFPU
 * @param min Valor mínimo
 * @param max Valor máximo
 * @return El número aleatorio calculado
 */
#endif
float AMG_Randf(float min, float max);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Get length of a 3D vector, using VFPU
 * @param x X coordinate
 * @param y Y coordinate
 * @param z Z coordinate
 * @return Vector length, as its name says ^^
 */
#else
/**
 * @brief Hallar la longitud de un vector tridimensional usando la VFPU
 * @param x Coordenada X del vector
 * @param y Coordenada Y del vector
 * @param z Coordenada Z del vector
 * @return El módulo del vector
 */
#endif
float AMG_VectorLength(float x, float y, float z);

#ifdef __cplusplus
	}
#endif

#endif
