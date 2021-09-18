// Includes
#include "AMG_3D.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pspkernel.h>

// Cabeceras
void __vfpu_perspective_matrix(ScePspFMatrix4 *m, float fov, float aspect, float near, float far);

// Sistema de matrices
typedef struct{
	ScePspFMatrix4 *matrix[4];
	u8 sp[4];
	u8 update[4];
	u8 stackDepth;
}amg_mtxsys;
amg_mtxsys amg_matrix_sys;
u8 amg_mtx_inited = 0;

#define SP(n) amg_matrix_sys.sp[n]
#define AMG_PROJECTION GU_PROJECTION][SP(GU_PROJECTION)
#define AMG_VIEW GU_VIEW][SP(GU_VIEW)
#define AMG_MODEL GU_MODEL][SP(GU_MODEL)
#define AMG_TEXTURE GU_TEXTURE][SP(GU_TEXTURE)

// Inicializa las matrices
void AMG_InitMatrixSystem(float fov){
	// Inicializa los buffers
	if(!amg_mtx_inited){
		memset(&amg_matrix_sys, 0, sizeof(amg_mtxsys));
		amg_matrix_sys.stackDepth = AMG_MATRIX_STACKSIZE;
		for(u8 i=0;i<4;i++) amg_matrix_sys.matrix[i] = (ScePspFMatrix4*) calloc (amg_matrix_sys.stackDepth, sizeof(ScePspFMatrix4));
		amg_mtx_inited = 1;
	}
	// Inicializa las matrices
	for(u8 i=0;i<4;i++) amg_matrix_sys.update[i] = true;
	AMG_LoadIdentity(GU_PROJECTION);
	AMG_LoadIdentity(GU_VIEW);
	AMG_LoadIdentity(GU_MODEL);
	AMG_LoadIdentity(GU_TEXTURE);
	__vfpu_perspective_matrix(&amg_matrix_sys.matrix[AMG_PROJECTION], fov, ((float)AMG.ScreenWidth / (float)AMG.ScreenHeight), 0.5f, 1000.0f);
	// Actualiza las matrices
	AMG_UpdateMatrices();
}

// Destruye las matrices
void AMG_DestroyMatrixSystem(void){
	if(!amg_mtx_inited) return;
	for(u8 i=0;i<4;i++) free(amg_matrix_sys.matrix[i]);
	memset(&amg_matrix_sys, 0, sizeof(amg_mtxsys));
	amg_mtx_inited = false;
}

// Actualiza las matrices
void AMG_UpdateMatrices(void){
	if(AMG.Rendering){
		for(u8 i=0;i<4;i++){
			if(amg_matrix_sys.update[i]){
				sceGuSetMatrix(i, &amg_matrix_sys.matrix[i][SP(i)]);
				amg_matrix_sys.update[i] = false;
			}
		}
	}
}

// Guarda una matriz en la pila
void AMG_PushMatrix(u8 mt){
	if(SP(mt) > amg_matrix_sys.stackDepth) return;
	memcpy(&amg_matrix_sys.matrix[mt][SP(mt)+1], &amg_matrix_sys.matrix[mt][SP(mt)], sizeof(ScePspFMatrix4));
	SP(mt) ++;
}

// Restaura una matriz de la pila
void AMG_PopMatrix(u8 mt){
	if(SP(mt) == 0) return;
	SP(mt) --;
	amg_matrix_sys.update[mt] = true;
}

/**
 * THE FOLLOWING FUNCTIONS HAVE BEEN TAKEN FROM: libpspmath, libpspgum
 * I'M NOT THE AUTHOR OF THESE FUNCTIONS
 */

// Normaliza un vector
void AMG_Normalize(ScePspFVector3 *v){
	 __asm__ volatile (
       "lv.q   C000, %0\n"
       "vdot.t S010, C000, C000\n"
       "vrsq.s S010, S010\n"
       "vscl.t C000, C000, S010\n"
       "sv.q   C000, %0\n"
	: "+m"(*v));
}

// Calcula el producto mixto entre 2 vectores
float AMG_DotProduct(ScePspFVector4 *v1, ScePspFVector4 *v2){
	float dot;
	__asm__ volatile (
		"lv.q C010, %1\n"
		"lv.q C020, %2\n"
		"vdot.t S000, C010, C020\n"
		"mfv %0, S000\n"
	: "=r"(dot) : "m"(*v1), "m"(*v2) : "memory");
	return dot;
}

// Haz el producto cruz
void AMG_CrossProduct(ScePspFVector3 *v1, ScePspFVector3 *v2, ScePspFVector3 *r){
	__asm__ volatile (
		"ulv.q C000, %1\n"
		"ulv.q C010, %2\n"
		"vcrsp.t C020, C000, C010\n"
		"usv.q C020, %0\n"
	:: "m"(*r), "m"(*v1), "m"(*v2));
}

// Simula una cámara
void AMG_LookAt(ScePspFVector3 *eye, ScePspFVector3 *center, ScePspFVector3 *up){
	ScePspFMatrix4 *m = &amg_matrix_sys.matrix[GU_VIEW][SP(GU_VIEW)];
	__asm__ volatile (
		// Carga la matriz VIEW
		"ulv.q C300,  0 + %0\n"
		"ulv.q C310, 16 + %0\n"
		"ulv.q C320, 32 + %0\n"
		"ulv.q C330, 48 + %0\n"
		// Calcula la cámara
		"vmidt.q M100\n"
		"ulv.q C000, %1\n"
		"ulv.q C010, %2\n"
		"ulv.q C020, %3\n"
		"vsub.t R102, C010, C000\n"
		"vdot.t S033, R102, R102\n"
		"vrsq.s S033, S033\n"
		"vscl.t R102, R102, S033\n"
		"vcrsp.t R100, R102, C020\n"
		"vdot.t S033, R100, R100\n"
		"vrsq.s S033, S033\n"
		"vscl.t R100, R100, S033\n"
		"vcrsp.t R101, R100, R102\n"
		"vneg.t R102, R102\n"
		"vmidt.q M200\n"
		"vneg.t C230, C000\n"
		// Multiplica la cámara por la vista
		"vmmul.q M300, M100, M200\n"
		"usv.q C300,  0 + %0\n"
		"usv.q C310, 16 + %0\n"
		"usv.q C320, 32 + %0\n"
		"usv.q C330, 48 + %0\n"
		:: "m"(*m), "m"(*eye), "m"(*center), "m"(*up));
	amg_matrix_sys.update[GU_VIEW] = true;
}

// Carga la identidad de una matriz
void AMG_LoadIdentityUser(ScePspFMatrix4 *mtx){
	__asm__ volatile (
		"vmidt.q M000\n"
		"usv.q C000, 0 + %0\n"
		"usv.q C010, 16 + %0\n"
		"usv.q C020, 32 + %0\n"
		"usv.q C030, 48 + %0\n"
	:"=m"(*mtx));
}

// Carga la identidad de una matriz
void AMG_LoadIdentity(u32 mt){
	if(mt > 3) return;
	ScePspFMatrix4 *mtx = &amg_matrix_sys.matrix[mt][SP(mt)];
	amg_matrix_sys.update[mt] = true;
	AMG_LoadIdentityUser(mtx);
}

// Traslada una matriz
void AMG_TranslateUser(ScePspFMatrix4 *mtx, ScePspFVector3 *v){
	__asm__ volatile (
		"ulv.q C630, %1\n"
		"ulv.q C700,  0 + %0\n"
		"ulv.q C710, 16 + %0\n"
		"ulv.q C720, 32 + %0\n"
		"ulv.q C730, 48 + %0\n"
		"vscl.q	C600, C700, S630\n"
		"vscl.q	C610, C710, S631\n"
		"vscl.q	C620, C720, S632\n"
		"vadd.q	C730, C730, C600\n"
		"vadd.q	C730, C730, C610\n"
		"vadd.q	C730, C730, C620\n"
		"usv.q C730, 48 + %0\n"	// only C730 has changed
	: "+m"(*mtx) : "m"(*v));
}

// Traslada una matriz
void AMG_Translate(u32 mt, ScePspFVector3 *v){
	if(mt > 3) return;
	ScePspFMatrix4 *mtx = &amg_matrix_sys.matrix[mt][SP(mt)];
	amg_matrix_sys.update[mt] = true;
	AMG_TranslateUser(mtx, v);
}

// Escala una matriz
void AMG_ScaleUser(ScePspFMatrix4 *mtx, ScePspFVector3 *v){
	__asm__ volatile (
		"ulv.q C700,  0 + %0\n"
		"ulv.q C710, 16 + %0\n"
		"ulv.q C720, 32 + %0\n"
		"ulv.q C730, 48 + %0\n"
		"ulv.q C600, %1\n"
		"vscl.t C700, C700, S600\n"
		"vscl.t C710, C710, S601\n"
		"vscl.t C720, C720, S602\n"
		"usv.q C700,  0 + %0\n"
		"usv.q C710, 16 + %0\n"
		"usv.q C720, 32 + %0\n"
		"usv.q C730, 48 + %0\n"
	: "+m"(*mtx) : "m"(*v));
}

// Escala una matriz
void AMG_Scale(u32 mt, ScePspFVector3 *v){
	if(mt > 3) return;
	ScePspFMatrix4 *mtx = &amg_matrix_sys.matrix[mt][SP(mt)];
	amg_matrix_sys.update[mt] = true;
	AMG_ScaleUser(mtx, v);
}

// Rota una matriz
void AMG_RotateUser(ScePspFMatrix4 *mtx, ScePspFVector3 *v){
	__asm__ volatile (
		// Carga la matriz y el vector
		"ulv.q C500,  0 + %0\n"
		"ulv.q C510, 16 + %0\n"
		"ulv.q C520, 32 + %0\n"
		"ulv.q C530, 48 + %0\n"
		"ulv.q C400, %1\n"
		"vcst.s S410, VFPU_2_PI\n"
		"vmul.s S400, S410, S400\n"
		"vmul.s S401, S410, S401\n"
		"vmul.s S402, S410, S402\n"
		// Rotación X
		"vmidt.q M600\n"
		"vrot.q C610, S400, [0, c, s, 0]\n"
		"vrot.q C620, S400, [0, -s, c, 0]\n"
		"vmmul.q M700, M500, M600\n"
		// Rotación Y
		"vmidt.q M600\n"
		"vrot.q C600, S401, [c, 0,-s, 0]\n"
		"vrot.q C620, S401, [s, 0, c, 0]\n"
		"vmmul.q M500, M700, M600\n"
		// Rotación Z
		"vmidt.q M600\n"
		"vrot.q C600, S402, [ c, s, 0, 0]\n"
		"vrot.q C610, S402, [-s, c, 0, 0]\n"
		"vmmul.q M700, M500, M600\n"
		// Guarda la matriz
		"usv.q C700,  0 + %0\n"
		"usv.q C710, 16 + %0\n"
		"usv.q C720, 32 + %0\n"
		"usv.q C730, 48 + %0\n"
	: "+m"(*mtx) : "m"(*v));
}

// Rota una matriz
void AMG_Rotate(u32 mt, ScePspFVector3 *v){
	if(mt > 3) return;
	ScePspFMatrix4 *mtx = &amg_matrix_sys.matrix[mt][SP(mt)];
	amg_matrix_sys.update[mt] = true;
	AMG_RotateUser(mtx, v);	
}

// Multiplica dos matrices
void AMG_MultMatrixUser(ScePspFMatrix4 *a, ScePspFMatrix4 *b, ScePspFMatrix4 *result){
	__asm__ volatile(
		"ulv.q C000,  0 + %1\n"
		"ulv.q C010, 16 + %1\n"
		"ulv.q C020, 32 + %1\n"
		"ulv.q C030, 48 + %1\n"
		"ulv.q C100,  0 + %2\n"
		"ulv.q C110, 16 + %2\n"
		"ulv.q C120, 32 + %2\n"
		"ulv.q C130, 48 + %2\n"
		"vmmul.q M200, M000, M100\n"
		"usv.q C200,  0 + %0\n"
		"usv.q C210, 16 + %0\n"
		"usv.q C220, 32 + %0\n"
		"usv.q C230, 48 + %0\n"
	: "=m"(*result) : "m"(*a), "m"(*b) : "memory");
}

// Multiplica dos matrices (oficial)
void AMG_MultMatrix(u8 mt, ScePspFMatrix4 *m){
	if(mt > 3) return;
	ScePspFMatrix4 *_m = &amg_matrix_sys.matrix[mt][SP(mt)];
	amg_matrix_sys.update[mt] = true;
	AMG_MultMatrixUser(_m, m, _m);
}
 
// Carga la perspectiva en una matriz
void __vfpu_perspective_matrix(ScePspFMatrix4 *m, float fov, float aspect, float near, float far){
	__asm__ volatile (
		"vmzero.q M100\n"					// set M100 to all zeros
		"mtv     %1, S000\n"				// S000 = fovy
		"viim.s  S001, 90\n"				// S002 = 90.0f
		"vrcp.s  S001, S001\n"				// S002 = 1/90
		"vmul.s  S000, S000, S000[1/2]\n"	// S000 = fovy * 0.5 = fovy/2
		"vmul.s  S000, S000, S001\n"		// S000 = (fovy/2)/90
		"vrot.p  C002, S000, [c, s]\n"		// S002 = cos(angle), S003 = sin(angle)
		"vdiv.s  S100, S002, S003\n"		// S100 = m->x.x = cotangent = cos(angle)/sin(angle)
		"mtv     %3, S001\n"				// S001 = near
		"mtv     %4, S002\n"				// S002 = far
		"vsub.s  S003, S001, S002\n"		// S003 = deltaz = near-far
		"vrcp.s  S003, S003\n"				// S003 = 1/deltaz
		"mtv     %2, S000\n"				// S000 = aspect
		"vmov.s  S111, S100\n"				// S111 = m->y.y = cotangent
		"vdiv.s  S100, S100, S000\n"		// S100 = m->x.x = cotangent / aspect
		"vadd.s  S122, S001, S002\n"        // S122 = m->z.z = far + near
		"vmul.s  S122, S122, S003\n"		// S122 = m->z.z = (far+near)/deltaz
		"vmul.s  S132, S001, S002\n"        // S132 = m->w.z = far * near
		"vmul.s  S132, S132, S132[2]\n"     // S132 = m->w.z = 2 * (far*near)
		"vmul.s  S132, S132, S003\n"        // S132 = m->w.z = 2 * (far*near) / deltaz
		"vsub.s   S123, S123, S123[1]\n"	// S123 = m->z.w = -1.0
		"sv.q	 C100, 0  + %0\n"
		"sv.q	 C110, 16 + %0\n"
		"sv.q	 C120, 32 + %0\n"
		"sv.q	 C130, 48 + %0\n"
	:"=m"(*m): "r"(fov),"r"(aspect),"r"(near),"r"(far));
}

/******************************************************/
/************** VFPU **********************************/
/******************************************************/

// Funcion seno
float AMG_Sin(float angle){
	float a; 
	__asm__ volatile (
	"mtv %1, S000\n"
	"vcst.s S001, VFPU_2_PI\n"
	"vmul.s S000, S000, S001\n"
	"vsin.s S000, S000\n"
	"mfv %0, S000\n"
	: "=r"(a)
	: "r"(angle));
   return a;
}

// Funcion coseno
float AMG_Cos(float angle){
	float a; 
	__asm__ volatile (
	"mtv %1, S000\n"
	"vcst.s S001, VFPU_2_PI\n"
	"vmul.s S000, S000, S001\n"
	"vcos.s S000, S000\n"
	"mfv %0, S000\n"
	: "=r"(a)
	: "r"(angle));
   return a;
}

// Raíz cuadrada
float AMG_SquareRoot(float val){
	float ret;
	__asm__ volatile (
		"mtv     %1, S000\n"
		"vsqrt.s S000, S000\n"
		"mfv     %0, S000\n"
	: "=r"(ret) : "r"(val));
	return ret;
}

// Número aleatorio
float AMG_Randf(float min, float max){ 
	float ret;
	__asm__ volatile (
		"mtv      %1, S000\n"
		"mtv      %2, S001\n"
		"vsub.s   S001, S001, S000\n"
		"vone.s   S002\n"
		"vrndf1.s S003\n"
		"vsub.s   S003, S003, S002\n"
		"vmul.s   S001, S003, S001\n"
		"vadd.s   S000, S000, S001\n"
		"mfv      %0, S000\n"
    : "=r"(ret) : "r"(min), "r"(max));
	return ret;
}

// Longitud de un vector
float AMG_VectorLength(float x, float y, float z){
	return AMG_SquareRoot((x*x)+(y*y)+(z*z));
}
