// Includes
#include "AMG_Physics.h"
#include "AMG_User.h"
#include <stdio.h>
#include "AMG_3D.h"

#ifdef AMG_ADDON_BULLET

// Includes BULLET
#include <LinearMath/btVector3.h>
#include <LinearMath/btMatrix3x3.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btQuickprof.h>
#include <LinearMath/btAlignedObjectArray.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

// Objetos principales
btDynamicsWorld* AMG_DynamicWorld;
btBroadphaseInterface* AMG_WorldBroadphase;
btCollisionDispatcher* AMG_WorldDispatcher;
btConstraintSolver*	AMG_PhysicsSolver;
btDefaultCollisionConfiguration* AMG_CollisionConfiguration;
btVector3 AMG_world[2];

typedef struct{
	AMG_Object *md;
	btRigidBody *Body;
	btCollisionShape *Shape;
	btTransform Transform;
	btVector3 Inertia;
	// Datos adicionales (btConvexHullShape)
	u32 ConvexVertices;
	btTriangleMesh *tri;
	btConvexShape *sh;
	btShapeHull *hull;
}amg_mdh;
amg_mdh *amg_model_ptr = NULL;
u32 amg_max_objects = 0;

// Funciones locales
void amg_get_position(amg_mdh *obj, ScePspFVector3 &pos);
void amg_get_rotation(amg_mdh *obj, ScePspFVector3 &rot);
void amg_save_object_stack(AMG_Object *md);
void amg_set_position(amg_mdh *obj, ScePspFVector3 &pos);
void amg_set_rotation(amg_mdh *obj, ScePspFVector3 &rot);

// Callback de BULLET
bool amg_bcallback(btManifoldPoint &cp, const btCollisionObjectWrapper *obj1, int id0, int idx0, const btCollisionObjectWrapper *obj2, int id1, int idx1){
	AMG_Object *o1 = (AMG_Object*) obj1->getCollisionObject()->getUserPointer();
	AMG_Object *o2 = (AMG_Object*) obj2->getCollisionObject()->getUserPointer();
	o1->Collision = true;
	o2->Collision = true;
	o1->CollideWith = o2->bullet_id;
	o2->CollideWith = o1->bullet_id;
	return false;
}

// Inicializa el motor BULLET
void AMG_InitBulletPhysics(ScePspFVector3 *world_size, u32 max_objects){
	if(world_size){
		AMG_world[0] = btVector3(world_size[0].x, world_size[0].y, world_size[0].z);
		AMG_world[1] = btVector3(world_size[1].x, world_size[1].y, world_size[1].z);
	}
	AMG_CollisionConfiguration = new btDefaultCollisionConfiguration();
	AMG_WorldDispatcher = new btCollisionDispatcher(AMG_CollisionConfiguration);
	if(world_size) AMG_WorldBroadphase = new btAxisSweep3(AMG_world[0], AMG_world[1], max_objects);
	else AMG_WorldBroadphase = new btDbvtBroadphase();
	AMG_PhysicsSolver = new btSequentialImpulseConstraintSolver();
	AMG_DynamicWorld = new btDiscreteDynamicsWorld(AMG_WorldDispatcher, AMG_WorldBroadphase, AMG_PhysicsSolver, AMG_CollisionConfiguration);
	AMG_DynamicWorld->setGravity(btVector3(0.0f, -9.8f, 0.0f));		// Gravedad por defecto (la terrestre)
	// Crea la pila de modelos 3D
	amg_model_ptr = (amg_mdh*) calloc (max_objects, sizeof(amg_mdh));
	amg_max_objects = max_objects;
	for(u32 i=0;i<amg_max_objects;i++){
		amg_model_ptr[i].md = NULL;
	}
	// Establece el callback de colisión
	gContactAddedCallback = amg_bcallback;
}

// Establece la gravedad del mundo simulado
void AMG_SetWorldGravity(float x, float y, float z){
	AMG_DynamicWorld->setGravity(btVector3(x, y, z));
}

// Haz un test de Ray Tracing
u8 AMG_RayTracingTest(ScePspFVector3 *pos, ScePspFVector3 *vec){
	u8 c=0;
	btCollisionWorld::ClosestRayResultCallback cb(btVector3(pos->x, pos->y, pos->z), btVector3(vec->x, vec->y, vec->z));
	AMG_DynamicWorld->rayTest(btVector3(pos->x, pos->y, pos->z), btVector3(vec->x, vec->y, vec->z), cb);
	c = cb.hasHit();		// Nos dice si el rayo ha chocado con algo
	if(c){
		AMG_Object *o = (AMG_Object*)(cb.m_collisionObject->getUserPointer());
		o->Collision = true;
		o->CollideWith = AMG_COLLISION_RAY;
	}
	return c;
}

// Inicializa las fisicas de un modelo 3D
void AMG_InitModelPhysics(AMG_Model *model){
	for(u8 i=0;i<model->NObjects;i++){
		if(model->Object[i].ShapeType == AMG_BULLET_SHAPE_NONE) continue;
		// Guarda el objeto en la pila
		amg_save_object_stack(&model->Object[i]);
		// Obtén el objeto
		amg_mdh *obj = &amg_model_ptr[model->Object[i].bullet_id];
		// Calcula los márgenes
		float x, y, z;
		x = ((model->Object[i].BBox[1].x - model->Object[i].BBox[0].x)/2.0f)*model->Object[i].Scale.x;
		y = ((model->Object[i].BBox[1].y - model->Object[i].BBox[0].y)/2.0f)*model->Object[i].Scale.y;
		z = ((model->Object[i].BBox[1].z - model->Object[i].BBox[0].z)/2.0f)*model->Object[i].Scale.z;
		// Según el tipo de objeto que sea...
		switch(model->Object[i].ShapeType){
			case AMG_BULLET_SHAPE_BOX:
				obj->Shape = new btBoxShape(btVector3(btScalar(x), btScalar(y), btScalar(z)));
				break;
			case AMG_BULLET_SHAPE_SPHERE:
				obj->Shape = new btSphereShape(x);
				break;
			case AMG_BULLET_SHAPE_CONE:
				model->Object[i].Origin.y -= y;
				obj->Shape = new btConeShape(x, y*2.0f);
				break;
			case AMG_BULLET_SHAPE_CYLINDER:
				obj->Shape = new btCylinderShape(btVector3(x, y, z));
				break;
			case AMG_BULLET_SHAPE_CONVEXHULL:
			{
				// Guarda los triángulos en un buffer
				obj->tri = new btTriangleMesh();
				for(u32 a=0;a<(model->Object[i].NFaces);a++){
					ScePspFVector3 *t = &(((ScePspFVector3*)model->Object[i].Shadow)[a*3]);
					obj->tri->addTriangle(btVector3(t[0].x, t[0].y, t[0].z),
										  btVector3(t[1].x, t[1].y, t[1].z),
										  btVector3(t[2].x, t[2].y, t[2].z));
				}
				
				// Crea el Convex Hull
				if(model->Object[i].isGround){
					btBvhTriangleMeshShape *trimesh = new btBvhTriangleMeshShape(obj->tri, false);
					//trimesh->buildOptimizedBvh();
					obj->Shape = trimesh;
				}else{
					obj->sh = new btConvexTriangleMeshShape(obj->tri);
					obj->hull = new btShapeHull(obj->sh);
					obj->hull->buildHull(obj->sh->getMargin());
					obj->sh->setUserPointer(obj->hull);
					btConvexHullShape *convexHull = new btConvexHullShape((btScalar*)obj->hull->getVertexPointer(), obj->hull->numVertices());
					//convexHull->initializePolyhedralFeatures();
					obj->ConvexVertices = obj->hull->numVertices();
					obj->Shape = convexHull;
				}
				
				// Configura la escala
				obj->Shape->setLocalScaling(btVector3(model->Object[i].Scale.x, model->Object[i].Scale.y, model->Object[i].Scale.z));
			} break;
			default: AMG_Error(AMG_CUSTOM_ERROR, 0, "Undefined BULLET shape type"); return;
		}
		obj->Transform.setIdentity();
		obj->Transform.setOrigin(btVector3(model->Object[i].Origin.x, model->Object[i].Origin.y, model->Object[i].Origin.z));
		btDefaultMotionState* MotionState = new btDefaultMotionState(obj->Transform);
		obj->Inertia = btVector3(0.0f, 0.0f, 0.0f);
		if(model->Object[i].Mass > 0.0f) obj->Shape->calculateLocalInertia(model->Object[i].Mass, obj->Inertia);
		obj->Body = new btRigidBody(model->Object[i].Mass, MotionState, obj->Shape, obj->Inertia);
		AMG_DynamicWorld->addRigidBody(obj->Body);
		if(!model->Object[i].isGround) obj->Body->activate();		// Activa el objeto
		// Establece el Callback
		obj->Body->setCollisionFlags(obj->Body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		obj->Body->setUserPointer(&model->Object[i]);
		// Corrige el centro de rotación si es un cono
		if(model->Object[i].ShapeType == AMG_BULLET_SHAPE_CONE) model->Object[i].Origin.y += y;
	}
}

// Quita un modelo 3D de la pila
void AMG_DeleteModelPhysics(AMG_Model *model){
	for(u8 i=0;i<model->NObjects;i++){		// Elimina de Bullet cada objeto 3D
		if(model->Object[i].ShapeType == AMG_BULLET_SHAPE_NONE) return;
		amg_mdh *obj = &amg_model_ptr[model->Object[i].bullet_id];
		// Elimina datos adicionales
		if(obj->tri) delete obj->tri;
		if(obj->sh) delete obj->sh;
		if(obj->hull) delete obj->hull;
		// Eliminalos del motor Bullet
		AMG_DynamicWorld->removeRigidBody(obj->Body);
		delete obj->Body; obj->Body = NULL;
		delete obj->Shape; obj->Shape = NULL;
		model->Object[i].Mass = 0.0f; model->Object[i].isGround = 0;
		// Quitalos de la pila
		amg_model_ptr[model->Object[i].bullet_id].md = NULL;
		model->Object[i].bullet_id = 0;
	}
}

// Crea un mapa de alturas
AMG_Object *AMG_CreateHeightmap(char *map, char *tex, float minheight, float maxheight, ScePspFVector3 *scale, ScePspFVector3 *pos){
	// Carga el mapa de alturas en RAM
	if(map == NULL) return NULL;
	u16 tq = AMG.TextureQuality;
	AMG.TextureQuality = GU_PSM_8888;
	AMG_Texture *hmap = AMG_LoadTexture(map, AMG_TEX_RAM);
	if((hmap->Width * hmap->Height * 6) > 65535){
		AMG_UnloadTexture(hmap);
		AMG_Error(AMG_CUSTOM_ERROR, 0, "Exceded heightmap size (>65535 vertices)");
		return NULL;
	}
	AMG.TextureQuality = tq;
	// Prepara el buffer donde se guardará el mapa (formato FLOAT)
	float *mapf = (float*) calloc (hmap->Width*hmap->Height, sizeof(float));
	if(mapf == NULL){ AMG_Error(AMG_OUT_OF_RAM, hmap->Width*hmap->Height*sizeof(float), "AMG_CreateHeightMap"); return NULL;}
	for(u32 i=0;i<(hmap->Width*hmap->Height);i++){
		mapf[i] = (((((float)(hmap->Data[i] &0x000000FF))/255.0f)*(maxheight-minheight))+minheight);
	}
	// Inicializa el objeto del terreno
	AMG_Object *obj = (AMG_Object*) calloc (1, sizeof(AMG_Object));
	obj->Data = NULL;
	obj->Data = (AMG_Vertex_TV*) calloc (hmap->Width*hmap->Height*6, sizeof(AMG_Vertex_TV));
	if(obj->Data == NULL){ AMG_Error(AMG_OUT_OF_RAM, (hmap->Width*hmap->Height*6*sizeof(AMG_Vertex_TV)), "AMG_CreateHeightmap"); return NULL;}
	AMG_Vertex_TV *v = (AMG_Vertex_TV*)obj->Data;
	obj->Shadow = NULL;
	obj->NFaces = (hmap->Width*hmap->Height*2);
	obj->Flags = (GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D);	// Flags por defecto
	obj->NGroups = 1;
	obj->Group = (AMG_ObjectGroup*) calloc (1, sizeof(AMG_ObjectGroup));
	if(obj->Group == NULL) return NULL;
	if(pos){
		obj->Pos.x = pos->x; obj->Pos.y = pos->y; obj->Pos.z = pos->z;
	}else{
		obj->Pos.x = obj->Pos.y = obj->Pos.z = 0.0f;
	}
	obj->Origin.x = 0.0f; obj->Origin.y = 0.0f; obj->Origin.z = 0.0f;
	obj->Rot.x = 0.0f; obj->Rot.y = 0.0f; obj->Rot.z = 0.0f;
	if(scale){
		obj->Scale.x = scale->x; obj->Scale.y = scale->y; obj->Scale.z = scale->z;
	}else{
		obj->Scale.x = obj->Scale.y = obj->Scale.z = 1.0f;
	}
	obj->Group[0].Texture = NULL;
	obj->Group[0].Start = 0;
	obj->Group[0].End = obj->NFaces;
	if(tex != NULL){
		obj->Group[0].Texture = AMG_LoadTexture(tex, AMG.TextureDest);
		obj->Group[0].Texture->TFX = GU_TFX_DECAL;
	}
	obj->BBox = NULL;
	obj->BBox = (ScePspFVector3*) calloc (2, sizeof(ScePspFVector3));
	obj->tBBox = NULL;
	obj->tBBox = (ScePspFVector3*) calloc (2, sizeof(ScePspFVector3));
	obj->CelShadingScale = 1.025f;
	obj->Group[0].MultiTexture = NULL;
	obj->Group[0].Ambient = 0xFF7F7F7F;
	obj->Group[0].Emmision = 0xFF000000;
	obj->Group[0].Diffuse = 0xFF4F4F4F;
	obj->Group[0].Specular = 0xFFFFFFFF;
	obj->Lighting = false;
	obj->Mass = 0.0f;
	obj->isGround = 1;
	obj->ShapeType = AMG_BULLET_SHAPE_HEIGHTMAP;
	obj->Collision = 0;
	obj->CollideWith = 0xFFFF;
	// Compila el mapa de alturas
	for(u16 y=0;y<(hmap->Height-1);y++){
		for(u16 x=0;x<(hmap->Width-1);x++){
			u32 o = (x+(y*hmap->Width));
			u32 o6 = o*6;
			v[o6].u = ((float)x / (float)hmap->Width);
			v[o6].v = ((float)y / (float)hmap->Height);
			v[o6].x = (float)x;
			v[o6].y = mapf[o];
			v[o6].z = (float)y;
			v[o6+1].u = ((float)(x+1) / (float)hmap->Width);
			v[o6+1].v = ((float)y / (float)hmap->Height);
			v[o6+1].x = (float)(x+1);
			v[o6+1].y = mapf[o+1];
			v[o6+1].z = (float)y;
			v[o6+2].u = ((float)x / (float)hmap->Width);
			v[o6+2].v = ((float)(y+1) / (float)hmap->Height);
			v[o6+2].x = (float)x;
			v[o6+2].y = mapf[o+hmap->Width];
			v[o6+2].z = (float)(y+1);
			v[o6+3].u = ((float)(x+1) / (float)hmap->Width);
			v[o6+3].v = ((float)y / (float)hmap->Height);
			v[o6+3].x = (float)(x+1);
			v[o6+3].y = mapf[o+1];
			v[o6+3].z = (float)y;
			v[o6+4].u = ((float)x / (float)hmap->Width);
			v[o6+4].v = ((float)(y+1) / (float)hmap->Height);
			v[o6+4].x = (float)x;
			v[o6+4].y = mapf[o+hmap->Width];
			v[o6+4].z = (float)(y+1);
			v[o6+5].u = ((float)(x+1) / (float)hmap->Width);
			v[o6+5].v = ((float)(y+1) / (float)hmap->Height);
			v[o6+5].x = (float)(x+1);
			v[o6+5].y = mapf[o+1+hmap->Width];
			v[o6+5].z = (float)(y+1);
			for(u8 i=0;i<6;i++){
				v[o6+i].x -= (float)(hmap->Width >> 1);
				v[o6+i].z -= (float)(hmap->Height >> 1);
			}
		}
	}
	// Crea el objeto 3D en BULLET (no implementado todavía)
	if(mapf != NULL){
		// Guarda el objeto en la pila
		/*amg_save_object_stack(obj);
		// Obtén el objeto
		amg_mdh *obj0 = &amg_model_ptr[obj->bullet_id];
		// Según el tipo de objeto que sea...
		obj0->Shape = new btHeightfieldTerrainShape(hmap.Width, hmap.Height, (void*)mapf, maxheight, 1, true, true);
		obj0->Shape->setLocalScaling(btVector3(scale->x, scale->y/2.0f, scale->z));
		obj0->Transform.setIdentity();
		obj0->Transform.setOrigin(btVector3(obj->Pos.x, obj->Pos.y, obj->Pos.z));
		btDefaultMotionState* MotionState = new btDefaultMotionState(obj0->Transform);
		obj0->Inertia = btVector3(0.0f, 0.0f, 0.0f);
		obj0->Body = new btRigidBody(obj->Mass, MotionState, obj0->Shape, obj0->Inertia);
		AMG_DynamicWorld->addRigidBody(obj0->Body);
		obj0->Body->activate();
		// Establece el Callback
		obj0->Body->setCollisionFlags(obj0->Body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		obj0->Body->setUserPointer(obj);*/
	}
	// Elimina datos temporales
	if(obj->Group[0].Texture == NULL) obj->Group[0].Texture = hmap;
	else AMG_UnloadTexture(hmap);
	// Devuelve el modelo creado
	return obj;
}

// Actualiza el motor de fisicas
void AMG_UpdateBulletPhysics(void){
	if(AMG_DynamicWorld == NULL) return;
	// Establece componentes del mundo 3D
	u32 i;
	for(i=0;i<amg_max_objects;i++){
		if(amg_model_ptr[i].md != NULL){
			amg_model_ptr[i].Body->activate();	// Activa el objeto
			amg_set_position(&amg_model_ptr[i], amg_model_ptr[i].md->Pos);
			amg_set_rotation(&amg_model_ptr[i], amg_model_ptr[i].md->Rot);
			// Pon a 0 toda colisión posible
			amg_model_ptr[i].md->Collision = 0;
			amg_model_ptr[i].md->CollideWith = 0xFFFF;
		}
	}
	// Simula el mundo 3D
	AMG_DynamicWorld->stepSimulation(1.0f/60.0f);
	// Actualiza componentes en los modelos 3D
	for(i=0;i<amg_max_objects;i++){
		if(amg_model_ptr[i].md != NULL){
			amg_get_position(&amg_model_ptr[i], amg_model_ptr[i].md->Pos);
			amg_get_rotation(&amg_model_ptr[i], amg_model_ptr[i].md->Rot);
		}
	}
}

// Termina con el motor Bullet
void AMG_FinishBulletPhysics(void){
	// Elimina todos los objetos de la pila
	for(u32 i=0;i<amg_max_objects;i++){
		if(amg_model_ptr[i].md != NULL){
			// Eliminalo del motor Bullet
			AMG_DynamicWorld->removeRigidBody(amg_model_ptr[i].Body);
			delete amg_model_ptr[i].Body; amg_model_ptr[i].Body = NULL;
			delete amg_model_ptr[i].Shape; amg_model_ptr[i].Shape = NULL;
			amg_model_ptr[i].md->Mass = 0.0f; amg_model_ptr[i].md->isGround = 0;
			// Quitalo de la pila
			amg_model_ptr[i].md->bullet_id = 0;
			amg_model_ptr[i].md = NULL;
			amg_model_ptr[i].Shape = NULL;
			amg_model_ptr[i].Body = NULL;
		}
	}
	// Elimina la pila
	free(amg_model_ptr); amg_model_ptr = NULL;
	amg_max_objects = 0;
	// Elimina los demas datos
	delete AMG_DynamicWorld; AMG_DynamicWorld = NULL;
	delete AMG_PhysicsSolver;
	delete AMG_WorldBroadphase;
	delete AMG_WorldDispatcher;
	delete AMG_CollisionConfiguration;
}

/******************************************************/
/************** FUNCIONES LOCALES *********************/
/******************************************************/

// Establece la posicion de un objeto simulado
void amg_set_position(amg_mdh *obj, ScePspFVector3 &pos){
	ScePspFVector3 tpos;
	amg_get_position(obj, tpos);
	float x = pos.x - tpos.x;
	float y = pos.y - tpos.y;
	float z = pos.z - tpos.z;
	obj->Body->translate(btVector3(x, y, z));
}

// Obten la posicion de un objeto simulado
void amg_get_position(amg_mdh *obj, ScePspFVector3 &pos){
	if(obj->Body && obj->Body->getMotionState()){
		btVector3 p = obj->Body->getCenterOfMassPosition();
		pos.x = (p.getX() - obj->md->Origin.x);
		pos.y = (p.getY() - obj->md->Origin.y);
		pos.z = (p.getZ() - obj->md->Origin.z);
	}
}

// Transforma un vector en modo Cuartenion (IJK) a modo Vectorial (XYZ)
void amg_quaternion2vector(const btQuaternion &quat, btVector3 &vec){
	float w = quat.getW();	float x = quat.getX();	float y = quat.getY();	float z = quat.getZ();
	float sqw = w*w; float sqx = x*x; float sqy = y*y; float sqz = z*z; 
	vec.setZ((atan2f(2.0 * (x*y + z*w),(sqx - sqy - sqz + sqw))));
	vec.setX((atan2f(2.0 * (y*z + x*w),(-sqx - sqy + sqz + sqw))));
	vec.setY((asinf(-2.0 * (x*z - y*w))));
}

// Establece la rotacion de un objeto simulado
void amg_set_rotation(amg_mdh *obj, ScePspFVector3 &rot){
	ScePspFVector3 trot;
	amg_get_rotation(obj, trot);
	float x = rot.x - trot.x;
	float y = rot.y - trot.y;
	float z = rot.z - trot.z;
	btMatrix3x3 orn = obj->Body->getWorldTransform().getBasis();
	orn *= btMatrix3x3(btQuaternion(btVector3(1, 0, 0), x));
	orn *= btMatrix3x3(btQuaternion(btVector3(0, 1, 0), y));
	orn *= btMatrix3x3(btQuaternion(btVector3(0, 0, 1), z));
	obj->Body->getWorldTransform().setBasis(orn);
}

// Obten la rotacion de un objeto simulado
void amg_get_rotation(amg_mdh *obj, ScePspFVector3 &rot){
	btVector3 btv;
	amg_quaternion2vector(obj->Body->getOrientation(), btv);
	rot.x = btv.getX();
	rot.y = btv.getY();
	rot.z = btv.getZ();
}

// Guarda un objeto 3D en la pila
void amg_save_object_stack(AMG_Object *md){
	u32 i; u8 done = 0;
	for(i=0;i<amg_max_objects;i++){
		if(!amg_model_ptr[i].md){	// Busca el primer slot libre
			amg_model_ptr[i].md = md; done = 1;
			md->bullet_id = i; i = amg_max_objects;
		}
	}
	if(!done) AMG_Error(AMG_CUSTOM_ERROR, 0, "Couldn't allocate physic object, stack(%d) is full!!", amg_max_objects);	// Error si no se ha encontrado
}

// Establece la velocidad lineal de un objeto 3D
void AMG_SetObjectLinearVelocity(AMG_Object *obj, float x, float y, float z){
	amg_mdh *o = &amg_model_ptr[obj->bullet_id];
	if(o->Body == NULL) return;
	o->Body->setLinearVelocity(btVector3(x, y, z));
}

#endif	// AMG_ADDON_BULLET
