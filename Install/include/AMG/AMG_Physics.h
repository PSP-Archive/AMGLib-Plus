#ifndef _AMG_PHYSICS_H_
#define _AMG_PHYSICS_H_

#ifdef __cplusplus
	extern "C" {
#endif

// Includes
#include "AMG_Model.h"
#include "AMG_Config.h"

#ifdef AMG_ADDON_BULLET

// Define los tipos de objetos
#define AMG_BULLET_SHAPE_NONE 0
#define AMG_BULLET_SHAPE_BOX 1
#define AMG_BULLET_SHAPE_SPHERE 2
#define AMG_BULLET_SHAPE_CONE 3
#define AMG_BULLET_SHAPE_CYLINDER 4
#define AMG_BULLET_SHAPE_CONVEXHULL 5
#define AMG_BULLET_SHAPE_HEIGHTMAP 6		// No funciona todavía...

// Define los tipos de colisión
#define AMG_COLLISION_RAY 0xFFFE
#define AMG_COLLISION_NONE 0xFFFF

#ifdef AMG_DOC_ENGLISH
/**
 * @file AMG_Physics.h
 * @brief Physics-related functions (using Bullet)
 * @author Andrés Martínez (Andresmargar)
 */
#else
/**
 * @file AMG_Physics.h
 * @brief Funciones encargadas del motor físico (con Bullet)
 * @author Andrés Martínez (Andresmargar)
 */
#endif

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Inits the physics engine using the Open Source Library "Bullet"
 * @param world_size 3D world size (send NULL for an unlimited world)
 * @param max_objects Maximum number of 3D objects
 * @return It returns nothing
 */
#else
/**
 * @brief Inicializa el motor de físicas usando la librería Open Source "Bullet"
 * @param world_size El tamaño del mundo 3D (pasa NULL para un mundo sin límites)
 * @param max_objects El número máximo de objetos 3D
 * @return No devuelve nada
 */
#endif
void AMG_InitBulletPhysics(ScePspFVector3 *world_size, u32 max_objects);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Set world gravity
 * @param x X component
 * @param y Y component
 * @param z Z component
 * @return It returns nothing
 */
#else
/**
 * @brief Establece la gravedad del mundo 3D
 * @param x El componente X del vector gravitatorio
 * @param y El componente Y del vector gravitatorio
 * @param z El componente Z del vector gravitatorio
 * @return No devuelve nada
 */
#endif
void AMG_SetWorldGravity(float x, float y, float z);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Configurates a 3D object
 * @param obj A 3D object
 * @param ox Origin X
 * @param oy Origin Y
 * @param oz Origin Z
 * @param x X position
 * @param y Y position
 * @param z Z position
 * @param mass Mass
 * @param shapetype Kind of shape
 * @return It returns nothing
 */
#else
/**
 * @brief Configura un modelo 3D
 * @param obj El objeto 3D
 * @param ox Origen X
 * @param oy Origen Y
 * @param oz Origen Z
 * @param x Posición X
 * @param y Posición Y
 * @param z Posición Z
 * @param mass Masa
 * @param shapetype Tipo de objeto
 * @return No devuelve nada
 */
#endif
static inline void AMG_ObjectConfPhysics(AMG_Object *obj, float ox, float oy, float oz, float x, float y, float z, float mass, u32 shapetype){
	obj->Origin.x = ox; obj->Origin.y = oy; obj->Origin.z = oz;
	obj->Pos.x = x; obj->Pos.y = y; obj->Pos.z = z;
	obj->Mass = mass; obj->ShapeType = shapetype;
	obj->isGround = (mass == 0.0f);
}

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Set the linear velocity of a 3D object
 * @param obj A 3D object
 * @param x X Velocity
 * @param y Y Velocity
 * @param z Z Velocity
 * @return It returns nothing
 */
#else
/**
 * @brief Establece la velocidad lineal de un objeto 3D
 * @param obj El objeto 3D
 * @param x Velocidad X
 * @param y Velocidad Y
 * @param z Velocidad Z
 * @return No devuelve nada
 */
#endif
void AMG_SetObjectLinearVelocity(AMG_Object *obj, float x, float y, float z);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Perform a Ray-Tracing test
 * @param pos Ray position
 * @param vec Ray direction
 * @return Wether the ray collided or not
 */
#else
/**
 * @brief Haz un test de Ray Tracing
 * @param pos La posición del rayo
 * @param vec El vector que forma el rayo
 * @return Si el rayo ha colisionado o no
 */
#endif
u8 AMG_RayTracingTest(ScePspFVector3 *pos, ScePspFVector3 *vec);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Links a 3D model to start simulation
 * @param model 3D model to link
 * @return It returns nothing
 * @note Parameters like mass and origin must be set BEFORE calling this function
 */
#else
/**
 * @brief Enlaza un modelo 3D para ser simulado
 * @param model El modelo 3D a enlazar
 * @return No devuelve nada
 * @note Parámetros como la masa y el origen se tiene que especificar ANTES de llamar a esta función
 */
#endif
void AMG_InitModelPhysics(AMG_Model *model);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Delinks a 3D model from the physics engine
 * @param model The 3D model to delink
 * @return It returns nothing
 */
#else
/**
 * @brief Desenlaza un modelo 3D del motor de físicas
 * @param model El modelo 3D a desenlazar
 * @return No devuelve nada
 */
#endif
void AMG_DeleteModelPhysics(AMG_Model *model);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Loads a heightmap
 * @param map Map file path
 * @param tex Texture file path (NULL if it doesn't have one)
 * @param minheight Minimum height of the map
 * @param maxheight Maximum height of the map
 * @param scale Map scale
 * @return The object which holds the heightmap
 */
#else
/**
 * @brief Carga un mapa de alturas
 * @param map La ruta al mapa
 * @param tex La ruta a la textura (NULL si no tiene)
 * @param minheight La altura mínima del mapa
 * @param maxheight La altura máxima del mapa
 * @param scale La escala del mapa de alturas
 * @return El objeto donde se almacenará
 */
#endif
AMG_Object *AMG_CreateHeightmap(char *map, char *tex, float minheight, float maxheight, ScePspFVector3 *scale, ScePspFVector3 *pos);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Deletes a heightmap
 * @param obj the map to delete
 * @return It returns nothing
 */
#else
/**
 * @brief Elimina un mapa de alturas
 * @param obj El mapa a borrar
 * @return No devuelve nada
 */
#endif
inline void AMG_UnloadHeightmap(AMG_Object *obj){
	AMG_UnloadObject(obj);
}

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Update 3D physics
 * @return It returns nothing
 */
#else
/**
 * @brief Actualiza las físicas 3D
 * @return No devuelve nada
 */
#endif
void AMG_UpdateBulletPhysics(void);

#ifdef AMG_DOC_ENGLISH
/**
 * @brief Finishes Bullet Engine
 * @return It returns nothing
 */
#else
/**
 * @brief Termina con el motor Bullet
 * @return No devuelve nada
 */
#endif
void AMG_FinishBulletPhysics(void);

#endif	// AMG_ADDON_BULLET

#ifdef __cplusplus
	}
#endif

#endif
