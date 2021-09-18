#ifndef _AMG_MODEL_H_
#define _AMG_MODEL_H_

#ifdef __cplusplus
	extern "C" {
#endif

// Includes
#include <psptypes.h>
#include "AMG_Texture.h"
#include "AMG_Config.h"

#ifdef AMG_DOC_ENGLISH
/**
 * @file AMG_Model.h
 * @brief Functions to load and render 3D models
 * @author Andrés Martínez (Andresmargar)
 */
#else
/**
 * @file AMG_Model.h
 * @brief Funciones encargadas de la carga y el renderizado de modelos 3D
 * @author Andrés Martínez (Andresmargar)
 */
#endif
 
/******************************************************/
/************** ANIMACIONES ***************************/
/******************************************************/

// Defines
#define AMG_MATRIX_FORMAT_DIRECTX	0
#define AMG_MATRIX_FORMAT_OPENGL	1
#define AMG_FRAME_DATAFORMAT_MATRIX 	(1 << 0)
#define AMG_FRAME_DATAFORMAT_POSITION 	(1 << 1)
#define AMG_FRAME_DATAFORMAT_ROTATION	(1 << 2)
#define AMG_FRAME_DATAFORMAT_SCALE		(1 << 3)
#define AMG_FRAME_DATAFORMAT_DIFFUSE	(1 << 4)

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Animation frame structure
 */
#else
/**
 * @struct
 * @brief Estructura a un frame de la animación
 */
#endif
typedef struct{
	u32 DataFormat;	/**< Formato de los datos */
	void *Data;		/**< Datos del frame */
}AMG_AnimationFrame;

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Struct to a single animation
 */
#else
/**
 * @struct
 * @brief Estructura a una sola animación
 */
#endif
typedef struct{
	char *Name;		/**< Nombre de la animación */
	u32 NFrames;	/**< Número de frames de la animación */
	u8 MatrixFormat;/**< Formato de las matrices (DirectX u OpenGL) */
	AMG_AnimationFrame *Frame;	/**< Puntero a las animaciones */
}AMG_AnimationChunk;

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Struct of a single or multiple animation
 */
#else
/**
 * @struct
 * @brief Estructura de una animación o varias animaciones
 */
#endif
typedef struct{
	u8 NAnimations;					/**< Número de animaciones */
	AMG_AnimationChunk *Animation;	/**< Puntero a las animaciones */
}AMG_Animation;

/******************************************************/
/************** MODELOS 3D ****************************/
/******************************************************/

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @deprecated
 * @brief Internal structure, don't use
 */
#else
/**
 * @struct
 * @deprecated
 * @brief Estructura interna del sistema, no usar
 */
#endif
typedef struct{
	int v[3], t[3], n[3];
}AMG_FaceOBJ;

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Structure which hold a material group
 */
#else
/**
 * @struct
 * @brief Estructura de un grupo de materiales
 */
#endif
typedef struct{
	u32 Emmision, Ambient, Specular, Diffuse;	/**< Valores de material / Material Components */
	AMG_Texture *Texture;						/**< La textura del objeto, si no tiene esto es NULL / Object texture (NULL if it does not exist) */
	AMG_Texture *MultiTexture;					/**< Textura creada para hacer  Multitextura / Second texture to combine with the first one */
	u32 Start, End;								/**< Las caras que se van a dibujar (número de triángulos) / Faces to draw (triangle number) */
	// INTERNO
	char *mtlname;								/**< Uso interno del sistema, no modificar / Internal usage */
	u8 sel;										/**< Uso interno del sistema, no modificar / Internal usage */
}AMG_ObjectGroup;

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Struct which holds a 3D object (included in AMG_Model struct)
 */
#else
/**
 * @struct
 * @brief Estructura de un objeto 3D (incluido en AMG_Model)
 */
#endif
typedef struct{
	u32 NFaces;									/**< Número de caras de este objeto / Number of faces */
	u16 NGroups;								/**< Número de grupos de este objeto (tanto el comando "g" como el "usemtl") / Number of groups (commands "g" and "usemtl") */
	AMG_ObjectGroup *Group;						/**< Todos los materiales usados por este objeto / List of material groups */
	void *Data;									/**< Los datos de vértices del objeto (no escribir) / Vertex data (don't write) */
	void *Shadow;								/**< La sombra del objeto entero (no escribir) / Shadow data (don't write) */
	u32 Flags;									/**< Los flags de renderizado (no escribir) / Rendering flags (don't write) */
	u8 TriangleSize;							/**< Tamaño de un triángulo / Triangle size in bytes (don't write) */
	ScePspFVector3 Pos;							/**< La posición en el espacio del objeto / Object position*/
	ScePspFVector3 Rot;							/**< La rotación en el espacio del objeto (Euler XYZ) / Object rotation (Euler XYZ)*/
	ScePspFVector3 Scale;						/**< La escala del objeto 3D / Object scale*/
	ScePspFVector3 *BBox;						/**< Bounding Box */
	ScePspFVector3 *tBBox;						/**< Bounding Box transformada / transformed Bounding Box*/
	ScePspFVector3 Centre, tCentre;				/**< Centro del objeto 3D (transformado) */
	float CelShadingScale;						/**< Tamaño del contorno en Cel-Shading (1.0 - 1.5) / Outline size for Cel-Shading (1.0 - 1.5) */
	u8 Lighting;								/**< Iluminamos el objeto o no? / Object affected by illumination? */
	// Motor fisico BULLET
	float Mass;									/**< Masa del objeto / Object mass */
	u8 isGround;								/**< true/false si el objeto es suelo o no / Is this object a floor? */
	ScePspFVector3 Origin;						/**< El origen del objeto (centro de rotación) / Rotation centre */
	u32 ShapeType;								/**< El tipo de objeto que es: una caja, una esfera... / Kind of object (box, sphere...) */
	u8 Collision;								/**< Si el objeto está en colisión con otro / If this object collides with another */
	u16 CollideWith;							/**< El objeto con el que colisiona nuestro cuerpo / Object which collides with this one */
	// USO INTERNO DEL SISTEMA (NO MODIFICAR)
	u32 bullet_id;								/**< Uso interno del sistema, no modificar / Internal usage */
	AMG_FaceOBJ *face;							/**< Uso interno del sistema, no modificar / Internal usage */
}AMG_Object;

#ifdef AMG_DOC_ENGLISH
/**
 * @struct
 * @brief Struct which holds a 3D model
 */
#else
/**
 * @struct
 * @brief Estructura de un modelo 3D
 */
#endif
typedef struct{
	u8 FaceFormat;								/**< El formato de caras del modelo, no modificar / Face Format (don't write) */
	u16 NObjects;								/**< El número de objetos del modelo 3D, no modificar / Number of objects (don't write) */
	AMG_Object *Object;							/**< Puntero a los objetos 3D, no modificar / Pointer to 3D objects (don't write) */
	u8 CelShading;								/**< true/false para activar/desactivar el Cel-Shading (activar luces antes de usarlo) / true/false to enable/disable Cel-Shading (enable light before using this) */
}AMG_Model;

// Defines
#define AMG_SHADE_LIGHT(n) ((n &0x7) << 1)

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Loads a 3D model in OBJ and MTL format
 * @param path OBJ file path (supports directories)
 * @return Pointer to the loaded 3D model
 * @note Model faces MUST be triangles
 */
#else
/**
 * @brief Carga un modelo 3D en formato OBJ y MTL
 * @param path La ruta del archivo .OBJ (soporta directorios)
 * @return El modelo 3D cargado
 * @note Las caras del modelo deben estar triangulizadas
 */
#endif
AMG_Model *AMG_LoadModel(const char *path);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Renders a 3D model
 * @param model Pointer to the 3D model loaded with AMG_LoadModel();
 * @return It returns nothing
 */
#else
/**
 * @brief Renderiza un modelo 3D
 * @param model Puntero al modelo 3D cargado con AMG_LoadModel();
 * @return No devuelve nada
 */
#endif
void AMG_RenderModel(AMG_Model *model);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Renders a 3D object
 * @param model Pointer to the 3D object
 * @param cs Render it using Cel-Shading?
 * @return It returns nothing
 */
#else
/**
 * @brief Renderiza un objeto 3D
 * @param model Puntero al objeto 3D
 * @param cs ¿Renderizar con Cel-Shading?
 * @return No devuelve nada
 */
#endif
void AMG_RenderObject(AMG_Object *model, u8 cs);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Deletes a 3D model
 * @param model Model to delete
 * @return It returns nothing
 */
#else
/**
 * @brief Elimina un objeto 3D
 * @param model El objeto a borrar
 * @return No devuelve nada
 */
#endif
void AMG_UnloadObject(AMG_Object *model);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Deletes a 3D model previously loaded
 * @param model Pointer to the 3D model
 * @return It returns nothing
 */
#else
/**
 * @brief Elimina un modelo 3D previamente cargado
 * @param model Puntero al modelo 3D
 * @return No devuelve nada
 */
#endif
void AMG_UnloadModel(AMG_Model *model);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Calculates the lighting normals of a 3D model
 * @param model Pointer to the model
 * @return It returns nothing
 * @note It doesn't matter if the model doesn't have normals, they're created automatically
 */
#else
/**
 * @brief Calcula las normales de iluminación de un modelo 3D
 * @param model Puntero al modelo 3D
 * @return No devuelve nada
 * @note No importa si el modelo no tiene normales, se crean automáticamente
 */
#endif
void AMG_NormalizeModel(AMG_Model *model);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Check if two 3D objects are colliding
 * @param obj1 Object 1
 * @param obj2 Object 2
 * @return Wether there is collision between them
 */
#else
/**
 * @brief Comprueba si dos objetos 3D están en colisión
 * @param obj1 El objeto 1
 * @param obj2 El objeto 2
 * @return Si hay colisión o no entre ellos
 */
#endif
u8 AMG_CheckBBoxCollision(const AMG_Object *obj1, const AMG_Object *obj2);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Enables reflection and shadows using Stencil Buffer
 * @param obj The 3D object which is gonna be a "floor"
 * @return It returns nothing
 */
#else
/**
 * @brief Activa el motor de reflejos y sombras con Stencil Buffer
 * @param obj El objeto 3D que hará de "suelo"
 * @return No devuelve nada
 */
#endif
void AMG_StartReflection(AMG_Object *obj);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Renders a 3D object which makes reflection with the "floor"
 * @param obj The reflected 3D object
 * @param axis The axis (0, 1 or 2 for X, Y and Z)
 * @param inv Flip the reflection?
 * @return It returns nothing
 */
#else
/**
 * @brief Renderiza un objeto 3D que refleje con el "suelo"
 * @param obj El objeto 3D que será reflejado
 * @param axis El eje donde se hará el reflejado (0, 1 o 2 para X, Y y Z)
 * @param inv Invertir el reflejado?
 * @return No devuelve nada
 */
#endif
void AMG_RenderMirrorObject(AMG_Object *obj, u8 axis, u8 inv);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Disables reflection and shadows engine and Stencil Buffer
 * @return It returns nothing
 */
#else
/**
 * @brief Desactiva el motor de reflejos y sombras con Stencil Buffer
 * @return No devuelve nada
 */
#endif
static inline void AMG_FinishReflection(void){
	sceGuDisable(GU_STENCIL_TEST);
}

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Renders a real-time shadow of and object
 * @param obj The 3D object which will be projected as a shadow
 * @param l The light ID which affects the shadow (0-3)
 * @param plane Plane equation of the floor (see AMG_PlaneEquation)
 * @return It returns nothing
 */
#else
/**
 * @brief Renderiza la sombra de un objeto 3D en tiempo real
 * @param obj El objeto 3D del que se hará la sombra
 * @param l El ID de la luz que provoca esa sombra (0-3)
 * @param plane La ecuación del plano del suelo (mirar AMG_PlaneEquation)
 * @return No devuelve nada
 */
#endif
void AMG_RenderShadow(AMG_Object *obj, u8 l, ScePspFVector4 *plane);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Set a second texture of a 3D object
 * @param obj Material Group
 * @param tex The loaded texture
 * @return It returns nothing
 */
#else
/**
 * @brief Establece la segunda textura de un objeto 3D
 * @param obj El grupo de materiales
 * @param tex La textura ya cargada
 * @return No devuelve nada
 */
#endif
static inline void AMG_SetObjectMultiTexture(AMG_ObjectGroup *obj, AMG_Texture *tex){
	tex->TFX = GU_TFX_MODULATE;
	obj->MultiTexture = tex;
}

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Calculates plane equation
 * @param points 3 points of the plane
 * @param p Where the plane equation will be saved
 * @return It returns nothing
 */
#else
/**
 * @brief Calcula la ecuación del plano
 * @param points 3 puntos del plano
 * @param p Donde se guardará la ecuación del plano
 * @return No devuelve nada
 */
#endif
void AMG_PlaneEquation(ScePspFVector3 *points, ScePspFVector4 *p);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Generate floor points (to calculate shadows)
 * @param obj The 3D object that works as a floor
 * @param points Where the calculated points will be saved (use them at AMG_PlaneEquation)
 * @return It returns nothing
 */
#else
/**
 * @brief Genera los puntos del suelo (para el cálculo de sombras)
 * @param obj El objeto 3D que hará de suelo
 * @param points Donde se almacenarán los puntos calculados (úsalos en AMG_PlaneEquation)
 * @return No devuelve nada
 */
#endif
void AMG_GenerateFloorPoints(AMG_Object *obj, ScePspFVector3 *points);

// Los TFX disponibles
#define AMG_NO_LIGHT 					GU_TFX_DECAL
#define AMG_LIGHT						GU_TFX_MODULATE
#define AMG_LIGHT_AND_COLOR_BLENDING	GU_TFX_BLEND

/*
AMG_Animation *AMG_LoadAnimation(char *path);

void AMG_UnloadAnimation(AMG_Animation *anim);
*/

#ifdef __cplusplus
	}
#endif

#endif
