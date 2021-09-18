// Includes
#include "AMG_Model.h"
#include "AMG_User.h"
#include "AMG_3D.h"
#include <pspgu.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

u32 l = 0;
ScePspFVector3 s;		// Lo siento, no me gustan las variables locales :P
AMG_Object *amg_curfloor;	// El suelo actual (para reflejos y sombras)

// Obten el directorio de un archivo
char *getdir(const char *path){
	char *dir = strdup(path);
	char *s = strrchr(dir, '/');
	if(s) s[1]   = '\0';
	else dir[0] = '\0';
	return dir;
}

// Carga un modelo en formato OBJ y MTL
AMG_Model *AMG_LoadModel(const char *path){

	// Define variables
	u32 j=0;
	AMG_Vertex_TV *tcv = NULL; AMG_Vertex_TNV *tcnv = NULL; AMG_Vertex_NV *cnv = NULL; AMG_Vertex_V *cv = NULL;
	u32 face_idx = 0;
	u8 ng = 0;
	int i = 0, faceformat = 0;
	float *vtx = NULL, *nrm = NULL, *txc = NULL;
	u8 noObjects = 0;
	u32 nobj = 0, nvtx = 0, nnrm = 0, ntxc = 0;
	char *line = (char*) calloc (128, sizeof(char));
	char *mtlpath = (char*) calloc (128, sizeof(char));
	char *straux = (char*) calloc (64, sizeof(char));
	
	// Crea el modelo 3D
	AMG_Model *model = NULL;
	model = (AMG_Model*) calloc (1, sizeof(AMG_Model));
	
	// Abre el archivo OBJ
	FILE *f = fopen(path, "rb");
	if(f == NULL){ AMG_Error(AMG_OPEN_FILE, 0, path); goto error;}
	model->CelShading = 0;
	
	// Primera lectura
	fseek(f, 0, SEEK_SET);
	while(!feof(f)){
		memset(line, 0, 128);
		fgets(line, 128, f);
		switch(line[0]){
			case '#': break;
			case 'm': // mtllib
				sscanf(line, "mtllib %s", straux);
				sprintf(mtlpath, "%s%s", getdir(path), straux);
				break;
			case 'o': nobj ++; break;
			case 'v':
				switch(line[1]){
					case ' ': nvtx ++; break;
					case 'n': nnrm ++; break;
					case 't': ntxc ++; break;
					default: break;
				} break;
			default: break;
		}
	}
	
	// Si no hay objetos, pon uno al menos (para los modelos exportados con Sketchup)
	if(nobj == 0){
		nobj = 1;
		noObjects = 1;
	}
	
	// Prepara buffers
	vtx = (float*) calloc (nvtx*3, sizeof(float));
	if(ntxc) txc = (float*) calloc (ntxc<<1, sizeof(float));
	if(nnrm) nrm = (float*) calloc (nnrm*3, sizeof(float));
	model->NObjects = nobj;
	model->Object = (AMG_Object*) calloc (model->NObjects, sizeof(AMG_Object));
	nobj = nvtx = nnrm = ntxc = 0;
	for(i=0;i<model->NObjects;i++){
		model->Object[i].NGroups = 0;
		model->Object[i].Group = NULL;
		model->Object[i].Data = NULL;
		model->Object[i].Shadow = NULL;
		model->Object[i].Flags = (GU_VERTEX_32BITF | GU_TRANSFORM_3D);	// Flags por defecto
		model->Object[i].BBox = NULL;
		model->Object[i].BBox = (ScePspFVector3*) calloc (2, sizeof(ScePspFVector3));
		model->Object[i].tBBox = NULL;
		model->Object[i].tBBox = (ScePspFVector3*) calloc (2, sizeof(ScePspFVector3));
		model->Object[i].CelShadingScale = 1.025f;
		model->Object[i].Lighting = true;
		model->Object[i].Pos.x = 0.0f; model->Object[i].Pos.y = 0.0f; model->Object[i].Pos.z = 0.0f;
		model->Object[i].Origin.x = 0.0f; model->Object[i].Origin.y = 0.0f; model->Object[i].Origin.z = 0.0f;
		model->Object[i].Rot.x = 0.0f; model->Object[i].Rot.y = 0.0f; model->Object[i].Rot.z = 0.0f;
		model->Object[i].Scale.x = 1.0f; model->Object[i].Scale.y = 1.0f; model->Object[i].Scale.z = 1.0f;
		// BULLET
		model->Object[i].Mass = 0.0f;
		model->Object[i].isGround = 0;
		model->Object[i].ShapeType = 0;
		model->Object[i].Collision = 0;
		model->Object[i].CollideWith = 0xFFFF;
	}
	
	// Variables temporales de lectura de caras
	int tmp_v0, tmp_v1, tmp_v2, tmp_n0, tmp_n1, tmp_n2, tmp_t0, tmp_t1, tmp_t2;
	nobj = noObjects;
	
	// Segunda lectura
	fseek(f, 0, SEEK_SET);
	while(!feof(f)){
		memset(line, 0, 128);
		fgets(line, 128, f);
		switch(line[0]){
			case '#': break;
			case 'g':		// Grupos de materiales (ahora soportados :D )
				break;
			case 'o': nobj ++; break;
			case 'u':	// usemtl
				model->Object[nobj-1].NGroups ++;
				break;
			case 'v':
				switch(line[1]){
					case ' ': sscanf(line, "v %f %f %f", &vtx[nvtx*3], &vtx[(nvtx*3)+1], &vtx[(nvtx*3)+2]); nvtx ++; break;
					case 'n': sscanf(line, "vn %f %f %f", &nrm[nnrm*3], &nrm[(nnrm*3)+1], &nrm[(nnrm*3)+2]); nnrm ++; break;
					case 't': sscanf(line, "vt %f %f", &txc[ntxc<<1], &txc[(ntxc<<1)+1]); ntxc ++; break;
					default: break;
				} break;
			default: break;
		}
	}
	
	// Crea los grupos de materiales
	for(i=0;i<model->NObjects;i++){
		model->Object[i].Group = (AMG_ObjectGroup*) calloc (model->Object[i].NGroups, sizeof(AMG_ObjectGroup));
		for(u8 k=0;k<model->Object[i].NGroups;k++){
			model->Object[i].Group[k].Ambient = GU_RGBA(0x7F, 0x7F, 0x7F, 0xFF);
			model->Object[i].Group[k].Emmision = GU_RGBA(0, 0, 0, 0xFF);
			model->Object[i].Group[k].Diffuse = GU_RGBA(0xFF, 0xFF, 0xFF, 0xFF);
			model->Object[i].Group[k].Specular = GU_RGBA(0xFF, 0xFF, 0xFF, 0xFF);
			model->Object[i].Group[k].Texture = NULL;
			model->Object[i].Group[k].MultiTexture = NULL;
			model->Object[i].Group[k].Start = 0;
			model->Object[i].Group[k].End = 0;
			model->Object[i].Group[k].mtlname = (char*) calloc (64, sizeof(char));
		}
	}
	
	// Lee el archivo y obtén el número de caras
	nobj = noObjects;
	fseek(f, 0, SEEK_SET);
	while(!feof(f)){
		memset(line, 0, 128);
		fgets(line, 128, f);
		switch(line[0]){
			case '#': break;
			case 'g':		// Grupos de materiales (ahora soportados :D )
				break;
			case 'o':
				nobj ++; ng = 0; break;
			case 'u':	// usemtl (guardamos el nombre del material)
				ng ++;
				sscanf(line, "usemtl %s", model->Object[nobj-1].Group[ng-1].mtlname);
				if(ng > 1){
					model->Object[nobj-1].Group[ng-1].Start = (model->Object[nobj-1].Group[ng-2].End);
					model->Object[nobj-1].Group[ng-1].End = (model->Object[nobj-1].Group[ng-2].End);
				} break;
			case 'f': 	// Averigua el formato de caras
				if(strstr(line, "//")){		// v//n o v//
					if(sscanf(line, "f %d//%d %d//%d %d//%d", &tmp_v0, &tmp_n0, &tmp_v1, &tmp_n1, &tmp_v2, &tmp_n2) == 6){
						faceformat = 0;
					}else{
						faceformat = 1;
					}
				}else if(sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &tmp_v0, &tmp_t0, &tmp_n0, &tmp_v1, &tmp_t1, &tmp_n1, &tmp_v2, &tmp_t2, &tmp_n2) == 9){
					faceformat = 2;
				}else if(sscanf(line, "f %d/%d/ %d/%d/ %d/%d/", &tmp_v0, &tmp_t0, &tmp_v1, &tmp_t1, &tmp_v2, &tmp_t2) == 6) faceformat = 3;
				else if(sscanf(line, "f %d/%d %d/%d %d/%d", &tmp_v0, &tmp_t0, &tmp_v1, &tmp_t1, &tmp_v2, &tmp_t2) == 6) faceformat = 4;
				else if(sscanf(line, "f %d %d %d", &tmp_v0, &tmp_v1, &tmp_v2) == 3) faceformat = 5;
				else{ AMG_Error(AMG_CUSTOM_ERROR, 0, "Model \"%s\": Wrong face format", path); goto error;}
				model->Object[nobj-1].Group[ng-1].End ++;
				break;
			default: break;
		}
	}
	
	// Crea buffers de vertices finales
	for(i=0;i<model->NObjects;i++){
		model->Object[i].NFaces = model->Object[i].Group[model->Object[i].NGroups-1].End;
		model->Object[i].Shadow = memalign (16, model->Object[i].NFaces*3*sizeof(AMG_Vertex_V));	// Sombra para Cel-Shading
		switch(faceformat){
			case 0:			// vertices y normales
				model->Object[i].Flags |= GU_NORMAL_32BITF;
				model->Object[i].Data = (AMG_Vertex_NV*) memalign (16, model->Object[i].NFaces*3*sizeof(AMG_Vertex_NV));
				model->Object[i].TriangleSize = sizeof(AMG_Vertex_NV)*3;
				break;
			case 1: case 5: // vertices
				model->Object[i].Data = (AMG_Vertex_V*) memalign (16, model->Object[i].NFaces*3*sizeof(AMG_Vertex_V));
				model->Object[i].TriangleSize = sizeof(AMG_Vertex_V)*3;
				break;
			case 2: // vertices, texcoords y normales
				model->Object[i].Flags |= (GU_TEXTURE_32BITF | GU_NORMAL_32BITF);
				model->Object[i].Data = (AMG_Vertex_TNV*) memalign (16, model->Object[i].NFaces*3*sizeof(AMG_Vertex_TNV));
				model->Object[i].TriangleSize = sizeof(AMG_Vertex_TNV)*3;
				break;
			case 3: case 4:	// vertices y texcoords
				model->Object[i].Flags |= GU_TEXTURE_32BITF;
				model->Object[i].Data = (AMG_Vertex_TV*) memalign (16, model->Object[i].NFaces*3*sizeof(AMG_Vertex_TV));
				model->Object[i].TriangleSize = sizeof(AMG_Vertex_TV)*3;
				break;
			default: break;
		}
		
		// Crea el buffer temporal de caras
		model->Object[i].face = NULL;
		model->Object[i].face = (AMG_FaceOBJ*) calloc (model->Object[i].NFaces, sizeof(AMG_FaceOBJ));
		if((model->Object[i].Data == NULL) || (model->Object[i].Shadow == NULL) || (model->Object[i].face == NULL)){
			AMG_Error(AMG_OUT_OF_RAM, -1, "AMG_LoadModel"); goto error;
		}
	}
	
	// Tercera lectura, leer las caras
	nobj = noObjects;
	fseek(f, 0, SEEK_SET);
	while(!feof(f)){
		memset(line, 0, 128);
		fgets(line, 128, f);
		if(line[0] == 'o'){
			nobj ++; face_idx = 0;
		}else if(line[0] == 'f' && line[1] == ' '){
			// Segun el formato de caras...
			switch(faceformat){
				case 0: sscanf(line, "f %d//%d %d//%d %d//%d", &model->Object[nobj-1].face[face_idx].v[0], &model->Object[nobj-1].face[face_idx].n[0], 
															   &model->Object[nobj-1].face[face_idx].v[1], &model->Object[nobj-1].face[face_idx].n[1], 
															   &model->Object[nobj-1].face[face_idx].v[2], &model->Object[nobj-1].face[face_idx].n[2]); break;
				case 1: sscanf(line, "f %d// %d// %d//", &model->Object[nobj-1].face[face_idx].v[0], &model->Object[nobj-1].face[face_idx].v[1], &model->Object[nobj-1].face[face_idx].v[2]); break;
				case 2: sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &model->Object[nobj-1].face[face_idx].v[0], &model->Object[nobj-1].face[face_idx].t[0], &model->Object[nobj-1].face[face_idx].n[0],
																	 &model->Object[nobj-1].face[face_idx].v[1], &model->Object[nobj-1].face[face_idx].t[1], &model->Object[nobj-1].face[face_idx].n[1],
																	 &model->Object[nobj-1].face[face_idx].v[2], &model->Object[nobj-1].face[face_idx].t[2], &model->Object[nobj-1].face[face_idx].n[2]);
																	 break;
				case 3: sscanf(line, "f %d/%d/ %d/%d/ %d/%d/", &model->Object[nobj-1].face[face_idx].v[0], &model->Object[nobj-1].face[face_idx].t[0], 
															   &model->Object[nobj-1].face[face_idx].v[1], &model->Object[nobj-1].face[face_idx].t[1], 
															   &model->Object[nobj-1].face[face_idx].v[2], &model->Object[nobj-1].face[face_idx].t[2]); break;
				case 4: sscanf(line, "f %d/%d %d/%d %d/%d", &model->Object[nobj-1].face[face_idx].v[0], &model->Object[nobj-1].face[face_idx].t[0], 
															   &model->Object[nobj-1].face[face_idx].v[1], &model->Object[nobj-1].face[face_idx].t[1], 
															   &model->Object[nobj-1].face[face_idx].v[2], &model->Object[nobj-1].face[face_idx].t[2]); break;
				case 5: sscanf(line, "f %d %d %d", &model->Object[nobj-1].face[face_idx].v[0], &model->Object[nobj-1].face[face_idx].v[1], &model->Object[nobj-1].face[face_idx].v[2]);
															   break;
				default: break;
			}
			// Resta 1 a cada indice de la cara
			for(i=0;i<3;i++){
				model->Object[nobj-1].face[face_idx].v[i] --;
				model->Object[nobj-1].face[face_idx].t[i] --;
				model->Object[nobj-1].face[face_idx].n[i] --;
			}
			// Suma el indice de caras para este objeto
			face_idx ++;
		}
	}
	
	// Compila el modelo para AMGLib
	for(i=0;i<model->NObjects;i++){
		for(face_idx=0;face_idx<model->Object[i].NFaces;face_idx++){
			AMG_Vertex_V *s = (AMG_Vertex_V*) model->Object[i].Shadow;
			switch(faceformat){
				case 0:					// vertices y normales
					cnv = (AMG_Vertex_NV*) model->Object[i].Data;
					for(j=0;j<3;j++){
						s[(face_idx*3)+j].x = cnv[(face_idx*3)+j].x = vtx[model->Object[i].face[face_idx].v[j] * 3];
						s[(face_idx*3)+j].y = cnv[(face_idx*3)+j].y = vtx[(model->Object[i].face[face_idx].v[j] * 3)+1];
						s[(face_idx*3)+j].z = cnv[(face_idx*3)+j].z = vtx[(model->Object[i].face[face_idx].v[j] * 3)+2];
						cnv[(face_idx*3)+j].nx = nrm[model->Object[i].face[face_idx].n[j] * 3];
						cnv[(face_idx*3)+j].ny = nrm[(model->Object[i].face[face_idx].n[j] * 3)+1];
						cnv[(face_idx*3)+j].nz = nrm[(model->Object[i].face[face_idx].n[j] * 3)+2];
					} break;
				case 1: case 5:			// vertices
					cv = (AMG_Vertex_V*) model->Object[i].Data;
					for(j=0;j<3;j++){
						cv[(face_idx*3)+j].x = vtx[model->Object[i].face[face_idx].v[j] * 3];
						cv[(face_idx*3)+j].y = vtx[(model->Object[i].face[face_idx].v[j] * 3)+1];
						cv[(face_idx*3)+j].z = vtx[(model->Object[i].face[face_idx].v[j] * 3)+2];
					}
					memcpy(s, cv, model->Object[i].NFaces*3*sizeof(AMG_Vertex_V));
					break;
				case 2:					// todo
					tcnv = (AMG_Vertex_TNV*) model->Object[i].Data;
					for(j=0;j<3;j++){
						s[(face_idx*3)+j].x = tcnv[(face_idx*3)+j].x = vtx[model->Object[i].face[face_idx].v[j] * 3];
						s[(face_idx*3)+j].y = tcnv[(face_idx*3)+j].y = vtx[(model->Object[i].face[face_idx].v[j] * 3)+1];
						s[(face_idx*3)+j].z = tcnv[(face_idx*3)+j].z = vtx[(model->Object[i].face[face_idx].v[j] * 3)+2];
						tcnv[(face_idx*3)+j].nx = nrm[model->Object[i].face[face_idx].n[j] * 3];
						tcnv[(face_idx*3)+j].ny = nrm[(model->Object[i].face[face_idx].n[j] * 3)+1];
						tcnv[(face_idx*3)+j].nz = nrm[(model->Object[i].face[face_idx].n[j] * 3)+2];
						tcnv[(face_idx*3)+j].u = txc[(model->Object[i].face[face_idx].t[j] << 1)];
						tcnv[(face_idx*3)+j].v = txc[(model->Object[i].face[face_idx].t[j] << 1)+1];
					} break;
				case 3: case 4:			// vertices y texcoords
					tcv = (AMG_Vertex_TV*) model->Object[i].Data;
					for(j=0;j<3;j++){
						s[(face_idx*3)+j].x = tcv[(face_idx*3)+j].x = vtx[model->Object[i].face[face_idx].v[j] * 3];
						s[(face_idx*3)+j].y = tcv[(face_idx*3)+j].y = vtx[(model->Object[i].face[face_idx].v[j] * 3)+1];
						s[(face_idx*3)+j].z = tcv[(face_idx*3)+j].z = vtx[(model->Object[i].face[face_idx].v[j] * 3)+2];
						tcv[(face_idx*3)+j].u = txc[(model->Object[i].face[face_idx].t[j] << 1)];
						tcv[(face_idx*3)+j].v = txc[(model->Object[i].face[face_idx].t[j] << 1)+1];
					}
					break;
				default: break;
			}
		}
	}
	
	// Abre el archivo MTL
	fclose(f); f = NULL;
	f = fopen(mtlpath, "rb");
	if(f == NULL){ AMG_Error(AMG_OPEN_FILE, 0, mtlpath); goto error;}
	
	// Leelo linea a linea
	float clr[3];
	u8 first;
	while(!feof(f)){
		fgets(line, 128, f);
		switch(line[0]){
			case '#': break;
			case 'n':	// newmtl
				sscanf(line, "newmtl %s", straux);
				for(i=0;i<model->NObjects;i++) for(u8 k=0;k<model->Object[i].NGroups;k++) model->Object[i].Group[k].sel = 0;
				// Busca los grupos que tengan ese material...
				for(i=0;i<model->NObjects;i++){
					for(u8 k=0;k<model->Object[i].NGroups;k++){
						if(strcmp(straux, model->Object[i].Group[k].mtlname) == 0){
							model->Object[i].Group[k].sel = 1;
						}
					}
				} break;
			case 'm':	// map_Kd
				sscanf(line, "map_Kd %s", straux);
				first = 1;
				for(i=0;i<model->NObjects;i++){
					for(u8 k=0;k<model->Object[i].NGroups;k++){
						if(model->Object[i].Group[k].sel){
							if(first){
								sprintf(model->Object[i].Group[k].mtlname, "%s%s", getdir(path), straux);
								model->Object[i].Group[k].Texture = AMG_LoadTexture(model->Object[i].Group[k].mtlname, AMG.TextureDest);
								first = 0;
							}else{
								// Busca la primera textura
								for(u8 i0=0;i0<model->NObjects;i0++){
									for(u8 i1=0;i1<model->Object[i0].NGroups;i1++){
										if(model->Object[i0].Group[i1].sel){
											model->Object[i].Group[k].Texture = model->Object[i0].Group[i1].Texture;
											i1 = model->Object[i0].NGroups; i0 = model->NObjects;	// Para salir del bucle doble...
										}
									}
								}
							}
						}
					}
				} break;
			case 'K':
				switch(line[1]){
					case 'd':		// Kd
						sscanf(line, "Kd %f %f %f", &clr[0], &clr[1], &clr[2]);
						for(i=0;i<model->NObjects;i++){
							for(u8 k=0;k<model->Object[i].NGroups;k++){
								if(model->Object[i].Group[k].sel) model->Object[i].Group[k].Diffuse = GU_COLOR(clr[0], clr[1], clr[2], 1.0f);
							}
						} break;
					case 'a':		// Ka
						sscanf(line, "Ka %f %f %f", &clr[0], &clr[1], &clr[2]);
						for(i=0;i<model->NObjects;i++){
							for(u8 k=0;k<model->Object[i].NGroups;k++){
								if(model->Object[i].Group[k].sel) model->Object[i].Group[k].Ambient = GU_COLOR(clr[0], clr[1], clr[2], 1.0f);
							}
						} break;
					case 's':		// Ks
						sscanf(line, "Ks %f %f %f", &clr[0], &clr[1], &clr[2]);
						for(i=0;i<model->NObjects;i++){
							for(u8 k=0;k<model->Object[i].NGroups;k++){
								if(model->Object[i].Group[k].sel) model->Object[i].Group[k].Specular = GU_COLOR(clr[0], clr[1], clr[2], 1.0f);
							}
						} break;
					default: break;
				} break;
			case 'd':
				sscanf(line, "d %f", &clr[0]);
				for(i=0;i<model->NObjects;i++){
					for(u8 k=0;k<model->Object[i].NGroups;k++){
						if(model->Object[i].Group[k].sel){
							model->Object[i].Group[k].Diffuse &= 0x00FFFFFF;
							model->Object[i].Group[k].Diffuse |= ((u8)(clr[0] * 255.0f) << 24);
						}
					}
				} break;
			default: break;
		}
	}
	
	// Cierra el archivo MTL
	fclose(f); f = NULL;
	model->FaceFormat = faceformat;
	
	// Calcula las bounding boxes
	u8 k;
	for(i=0;i<model->NObjects;i++){
		// Calcula la bounding box
		model->Object[i].BBox[0].x = vtx[(model->Object[i].face[0].v[0]*3)];
		model->Object[i].BBox[0].y = vtx[(model->Object[i].face[0].v[0]*3)+1];
		model->Object[i].BBox[0].z = vtx[(model->Object[i].face[0].v[0]*3)+2];
		model->Object[i].BBox[1].x = model->Object[i].BBox[0].x;
		model->Object[i].BBox[1].y = model->Object[i].BBox[0].y;
		model->Object[i].BBox[1].z = model->Object[i].BBox[0].z;
		for(j=1;j<model->Object[i].NFaces;j++){
			for(k=0;k<3;k++){
				if(model->Object[i].BBox[0].x >= vtx[(model->Object[i].face[j].v[k]*3)]) model->Object[i].BBox[0].x = vtx[(model->Object[i].face[j].v[k]*3)];	// XMIN
				if(model->Object[i].BBox[0].y >= vtx[(model->Object[i].face[j].v[k]*3)+1]) model->Object[i].BBox[0].y = vtx[(model->Object[i].face[j].v[k]*3)+1];	// YMIN
				if(model->Object[i].BBox[0].z >= vtx[(model->Object[i].face[j].v[k]*3)+2]) model->Object[i].BBox[0].z = vtx[(model->Object[i].face[j].v[k]*3)+2];	// ZMIN
				if(model->Object[i].BBox[1].x <= vtx[(model->Object[i].face[j].v[k]*3)]) model->Object[i].BBox[1].x = vtx[(model->Object[i].face[j].v[k]*3)];	// XMAX
				if(model->Object[i].BBox[1].y <= vtx[(model->Object[i].face[j].v[k]*3)+1]) model->Object[i].BBox[1].y = vtx[(model->Object[i].face[j].v[k]*3)+1];	// YMAX
				if(model->Object[i].BBox[1].z <= vtx[(model->Object[i].face[j].v[k]*3)+2]) model->Object[i].BBox[1].z = vtx[(model->Object[i].face[j].v[k]*3)+2];	// ZMAX
			}
		}
	}
	
	//AMG_MessageBox(AMG_MESSAGE_STRING, 0, 0, "Gotcha!");
	/*f = fopen("log.txt", "wb");
	fprintf(f, "FaceFormat: %d\n", (int)model->FaceFormat);
	fprintf(f, "NObjects: %d noObjects: %d\n", (int)model->NObjects, (int)noObjects);
	fprintf(f, "NV: %d, NT: %d, NN: %d\n", (int)nvtx, (int)ntxc, (int)nnrm);
	for(u16 i=0;i<model->NObjects;i++){
		fprintf(f, "\nObject[%d]\n\n", (int)i);
		fprintf(f, "NFaces: %d\nNGroups: %d\nTriangleSize: %d bytes\n", (int)model->Object[i].NFaces, (int)model->Object[i].NGroups, (int)model->Object[i].TriangleSize);
		fprintf(f, "Data: %p\n", model->Object[i].Data);
		for(u8 k=0;k<model->Object[i].NGroups;k++){
			fprintf(f, "\n\tGroup[%d]\n\n\t", (int)k);
			fprintf(f, "%d to %d\n", (int)model->Object[i].Group[k].Start, (int)model->Object[i].Group[k].End); 
		}
	}
	fclose(f); f = NULL;*/
	
	// Libera TODOS los datos temporales usados
	tcnv = NULL; tcv = NULL; cv = NULL; cnv =  NULL;
	free(line); line = NULL; free(mtlpath); mtlpath = NULL; free(straux); straux = NULL;
	free(vtx); vtx = NULL;
	if(ntxc) free(txc); txc = NULL;
	if(nnrm) free(nrm); nrm = NULL;
	for(i=0;i<model->NObjects;i++){
		free(model->Object[i].face); model->Object[i].face = NULL;
		for(u8 k=0;k<model->Object[i].NGroups;k++){
			free(model->Object[i].Group[k].mtlname); model->Object[i].Group[k].mtlname = NULL;
		}
	}
	
	// Devuelve el modelo creado
	return model;
error:
	// Libera datos del modelo
	if(f) fclose(f);
	if(model){
		if(model->Object){
			for(i=0;i<model->NObjects;i++){
				if(model->Object[i].face) free(model->Object[i].face);
				if(model->Object[i].Data) free(model->Object[i].Data);
				if(model->Object[i].Shadow) free(model->Object[i].Shadow);
				if(model->Object[i].BBox) free(model->Object[i].BBox);
				if(model->Object[i].tBBox) free(model->Object[i].tBBox);
				for(u8 k=0;k<model->Object[i].NGroups;k++){
					if(model->Object[i].Group[k].Texture){
						AMG_UnloadTexture(model->Object[i].Group[k].Texture);
						free(model->Object[i].Group[k].Texture);
					}
					if(model->Object[i].Group[k].mtlname) free(model->Object[i].Group[k].mtlname);
				}
			}
			free(model->Object);
		}
		free(model);
	}
	// Libera buffers temporales
	if(vtx) free(vtx);
	if(txc) free(txc);
	if(nrm) free(nrm);
	if(straux) free(straux);
	if(line) free(line);
	if(mtlpath) free(mtlpath);
	// Devuelve NULL
	return NULL;
}

// Renderiza un modelo 3D
void AMG_RenderModel(AMG_Model *model){
	if(model == NULL) return;
	for(u8 i=0;i<model->NObjects;i++){
		AMG_RenderObject(&model->Object[i], model->CelShading);
	}
}

// Renderiza un objeto 3D
void AMG_RenderObject(AMG_Object *model, u8 cs){
	// Comprueba si es NULL
	if(model == NULL) return;
	// Control de la iluminación
	u8 l2 = 0;
	if((!model->Lighting) || (cs)){
		l2 = sceGuGetStatus(GU_LIGHTING);
		sceGuDisable(GU_LIGHTING);
	}
	// Aplica las transformaciones necesarias
	AMG_PushMatrix(GU_MODEL);
	AMG_Translate(GU_MODEL, &model->Pos);
	AMG_Translate(GU_MODEL, &model->Origin);
	AMG_Rotate(GU_MODEL, &model->Rot);
	AMG_Scale(GU_MODEL, &model->Scale);
	model->Origin.x = -model->Origin.x; model->Origin.y = -model->Origin.y; model->Origin.z = -model->Origin.z;
	AMG_Translate(GU_MODEL, &model->Origin);
	model->Origin.x = -model->Origin.x; model->Origin.y = -model->Origin.y; model->Origin.z = -model->Origin.z;
	AMG_UpdateMatrices();											// Actualiza las matrices
	sceGuColorMaterial(GU_DIFFUSE | GU_SPECULAR | GU_AMBIENT);		// Define los componentes materiales a usar
	sceGuSpecular(AMG.WorldSpecular);								// Define el valor especular del mundo 3D
	
	// Dibuja cada grupo de este objeto
	for(u8 i=0;i<model->NGroups;i++){
		// Define los materiales
		sceGuMaterial(GU_DIFFUSE, model->Group[i].Diffuse); 
		sceGuMaterial(GU_SPECULAR, model->Group[i].Specular); 
		sceGuMaterial(GU_AMBIENT, model->Group[i].Ambient);
		sceGuColor(model->Group[i].Diffuse); sceGuAmbient(model->Group[i].Ambient);
		if(model->Group[i].Texture != NULL){
			AMG_EnableTexture(model->Group[i].Texture);	// Activa la textura
			if(cs){
				AMG_SetObjectMultiTexture(&model->Group[i], (AMG_Texture*)AMG.Lightmap);
				// Calcula el ENVMAP (distancia de 10.0f)
				u8 l = (cs >> 1) &0x7;
				if(l < 4){
					ScePspFVector3 *p = (ScePspFVector3*)&AMG_Light[l].Pos;
					model->Group[i].MultiTexture->EnvMapX = -((p->x - model->Pos.x) / 10.0f);
					model->Group[i].MultiTexture->EnvMapY = -((p->y - model->Pos.y) / 10.0f);
				}					
			}else if((!cs) && (model->Group[i].MultiTexture == (AMG_Texture*)AMG.Lightmap)){	// Si no hay Cel-shading y sigue activado el lightmap...
				model->Group[i].MultiTexture = NULL;		// Desactívalo
			}
		}else{
			if(cs){	// Si no hay textura, pero si cel-shading, aplica el lightmap
				AMG_EnableTexture((AMG_Texture*)AMG.Lightmap);
			}else sceGuDisable(GU_TEXTURE_2D);	// Sin textura ni celshading, nada
		}
		u16 nfaces = (model->Group[i].End - model->Group[i].Start);
		sceGuDrawArray(GU_TRIANGLES, model->Flags, nfaces*3, 0, (void*)&(((u8*)model->Data)[model->Group[i].Start*model->TriangleSize]));
		// Dibuja la parte MultiTextura
		if((model->Group[i].MultiTexture != NULL) && (model->Group[i].Texture != NULL)){
			AMG_EnableTexture(model->Group[i].MultiTexture);
			sceGuDrawArray(GU_TRIANGLES, model->Flags, nfaces*3, 0, (void*)&((u8*)model->Data)[model->Group[i].Start*model->TriangleSize]);
			AMG.DrawnVertices += (nfaces*3);
		}
		// Dibuja la parte Cel-Shading (el outline)
		if(cs && model->Shadow){
			AMG_Light[2].Pos.x = AMG_Light[3].Pos.y = 1.0f;		// Configura la matriz del envmap
			s.x = model->CelShadingScale; s.y = model->CelShadingScale; s.z = model->CelShadingScale;
			AMG_Scale(GU_MODEL, &s);
			sceGuDisable(GU_TEXTURE_2D);
			l = sceGuGetStatus(GU_LIGHTING);
			sceGuDisable(GU_LIGHTING);
			sceGuEnable(GU_CULL_FACE);
			sceGuFrontFace(GU_CW);
			sceGuColor(model->Group[i].Diffuse &0xFF000000);
			AMG_UpdateMatrices();
			sceGuDrawArray(GU_TRIANGLES, (GU_VERTEX_32BITF | GU_TRANSFORM_3D), model->NFaces*3, 0, model->Shadow);
			sceGuDisable(GU_CULL_FACE);
			if(l == GU_TRUE) sceGuEnable(GU_LIGHTING);
			AMG.DrawnVertices += (nfaces*3);
		}
	}
	// Actualiza el número de vértices dibujados
	AMG.DrawnVertices += (model->NFaces*3);
	/** Actualiza la Bounding Box **/
	model->tBBox[0].x = model->BBox[0].x + model->Pos.x;
	model->tBBox[0].y = model->BBox[0].y + model->Pos.y;
	model->tBBox[0].z = model->BBox[0].z + model->Pos.z;
	model->tBBox[1].x = model->BBox[1].x + model->Pos.x;
	model->tBBox[1].y = model->BBox[1].y + model->Pos.y;
	model->tBBox[1].z = model->BBox[1].z + model->Pos.z;
	model->Centre.x = (model->BBox[0].x + model->BBox[1].x)/2.0f;
	model->Centre.y = (model->BBox[0].y + model->BBox[1].y)/2.0f;
	model->Centre.z = (model->BBox[0].z + model->BBox[1].z)/2.0f;
	model->tCentre.x = model->Centre.x + model->Pos.x;
	model->tCentre.y = model->Centre.y + model->Pos.y;
	model->tCentre.z = model->Centre.z + model->Pos.z;
	/*******************************/
	AMG_PopMatrix(GU_MODEL);
	// Control de la iluminación
	if((!model->Lighting) && (l2)) sceGuEnable(GU_LIGHTING);
}

// Elimina un objeto 3D
void AMG_UnloadObject(AMG_Object *model){
	// Libera el objeto
	if(model == NULL) return;
	if(model->Data != NULL) free(model->Data); model->Data = NULL;
	if(model->BBox != NULL) free(model->BBox); model->BBox = NULL;
	if(model->tBBox != NULL) free(model->tBBox); model->tBBox = NULL;
	if(model->Shadow) free(model->Shadow); model->Shadow = NULL;
	// Libera los grupos de materiales
	for(u8 i=0;i<model->NGroups;i++){
		if(model->Group[i].Texture != NULL){
			AMG_UnloadTexture(model->Group[i].Texture);
			free(model->Group[i].Texture); model->Group[i].Texture = NULL;
		}
	}
	free(model->Group); model->Group = NULL;
}

// Elimina un modelo 3D
void AMG_UnloadModel(AMG_Model *model){
	if(model == NULL) return;
	u16 i;
	for(i=0;i<model->NObjects;i++){
		AMG_UnloadObject(&model->Object[i]);
	}
	model->FaceFormat = 0; model->NObjects = 0;
	free(model->Object); model->Object = NULL;
}

// Comprueba la colision entre 2 objetos con Bounding Box
u8 AMG_CheckBBoxCollision(const AMG_Object *obj1, const AMG_Object *obj2){
	if((obj1 == NULL) || (obj2 == NULL)) return 0;
	if(((obj1->tBBox[0].x) <= (obj2->tBBox[1].x))  // Borde derecho obj1 > Borde izquierdo obj2
	 && ((obj1->tBBox[1].x) >= (obj2->tBBox[0].x))  // Borde izquierdo obj1 < Borde derecho obj2
	 && ((obj1->tBBox[0].y) <= (obj2->tBBox[1].y))  // Borde inferior obj1 < Borde superior obj2
	 && ((obj1->tBBox[1].y) >= (obj2->tBBox[0].y))  // Borde superior obj1 > Borde inferior obj2
	 && ((obj1->tBBox[0].z) <= (obj2->tBBox[1].z))  
	 && ((obj1->tBBox[1].z) >= (obj2->tBBox[0].z))) return 1;
	return 0;
}

// Normaliza un modelo 3D
void AMG_NormalizeModel(AMG_Model *model){
	if(model == NULL) return;
	u8 i; u32 j;
	ScePspFVector3 vec;
	AMG_Vertex_TNV *tcnv; AMG_Vertex_NV *cnv; AMG_Vertex_V *cv; AMG_Vertex_TV *tcv;
	// Procesa cada vertice
	for(i=0;i<model->NObjects;i++){
		// Normaliza las caras
		switch(model->FaceFormat){
			case 0:				// v+n
				cnv = (AMG_Vertex_NV*) model->Object[i].Data;
				for(j=0;j<(model->Object[i].NFaces*3);j++){
					vec.x = cnv[j].x; vec.y = cnv[j].y; vec.z = cnv[j].z;
					AMG_Normalize(&vec);
					cnv[j].nx = vec.x; cnv[j].ny = vec.y; cnv[j].nz = vec.z;
				}
				break;
			case 2:				// v+n+t
				tcnv = (AMG_Vertex_TNV*) model->Object[i].Data;
				for(j=0;j<(model->Object[i].NFaces*3);j++){
					vec.x = tcnv[j].x; vec.y = tcnv[j].y; vec.z = tcnv[j].z;
					AMG_Normalize(&vec);
					tcnv[j].nx = vec.x; tcnv[j].ny = vec.y; tcnv[j].nz = vec.z;
				}
				break;
			case 1: case 5:		// v
				// Transforma a CNV
				cnv = (AMG_Vertex_NV*) calloc (model->Object[i].NFaces*3, sizeof(AMG_Vertex_NV));
				cv = (AMG_Vertex_V*) model->Object[i].Data;
				model->FaceFormat = 1;
				for(j=0;j<(model->Object[i].NFaces*3);j++){
					vec.x = cv[j].x; vec.y = cv[j].y; vec.z = cv[j].z;
					AMG_Normalize(&vec);
					cnv[j].nx = vec.x; cnv[j].ny = vec.y; cnv[j].nz = vec.z;
					cnv[j].x = cv[j].x; cnv[j].y = cv[j].y; cnv[j].z = cv[j].z;
				}
				model->Object[i].Flags = (GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D);
				free(model->Object[i].Data); model->Object[i].Data = (void*) cnv;
				break;
			case 3: case 4:		// v+t
				// Transforma a TCNV
				tcnv = (AMG_Vertex_TNV*) calloc (model->Object[i].NFaces*3, sizeof(AMG_Vertex_TNV));
				tcv = (AMG_Vertex_TV*) model->Object[i].Data;
				model->FaceFormat = 3;
				for(j=0;j<(model->Object[i].NFaces*3);j++){
					vec.x = tcv[j].x; vec.y = tcv[j].y; vec.z = tcv[j].z;
					AMG_Normalize(&vec);
					tcnv[j].nx = vec.x; tcnv[j].ny = vec.y; tcnv[j].nz = vec.z;
					tcnv[j].x = tcv[j].x; tcnv[j].y = tcv[j].y; tcnv[j].z = tcv[j].z;
				}
				model->Object[i].Flags = (GU_TEXTURE_32BITF | GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D);
				free(model->Object[i].Data); model->Object[i].Data = (void*) tcnv;
				break;
			default: AMG_Error(AMG_CUSTOM_ERROR, 0, "AMG_NormalizeModel: Couldn't normalize model"); return;
		}
	}
}

// Renderiza un objeto (mirror)
void AMG_RenderMirrorObject(AMG_Object *obj, u8 axis, u8 inv){
	// Cuidado con la iluminacion
	if(obj == NULL) return;
	u8 l = sceGuGetStatus(GU_LIGHTING);
	sceGuDisable(GU_LIGHTING);
	// Activa el stencil buffer
	if(AMG.PSM == GU_PSM_8888){
		sceGuStencilFunc(GU_EQUAL, 1, 1);
		sceGuStencilOp(GU_KEEP, GU_KEEP, GU_KEEP);
	}
	// Guarda valores temporales
	ScePspFVector3 tmp_pos, tmp_scl;
	tmp_pos.x = obj->Pos.x; tmp_pos.y = obj->Pos.y; tmp_pos.z= obj->Pos.z;
	tmp_scl.x = obj->Scale.x; tmp_scl.y = obj->Scale.y; tmp_scl.z= obj->Scale.z;
	u32 *dif = (u32*) sceGuGetMemory(obj->NGroups << 2);
	for(u8 i=0;i<obj->NGroups;i++){
		dif[i] = obj->Group[i].Diffuse;
		obj->Group[i].Diffuse &= 0x00FFFFFF;
		obj->Group[i].Diffuse |= 0x7F000000;	// Añadimos ALPHA al 50%
	}
	// Cambia la posición
	float d = 0.0f;		// Distancia del objeto al suelo
	switch(axis){
		case 0:			// Eje X
			d = ((obj->Origin.x + obj->Pos.x) - (amg_curfloor->Origin.x + amg_curfloor->Pos.x));
			if(inv) obj->Pos.x += (d * 2.0f);
			else obj->Pos.x -= (d * 2.0f);
			obj->Scale.x = -obj->Scale.x;
			break;
		case 1:			// Eje Y
			d = ((obj->Origin.y + obj->Pos.y) - (amg_curfloor->Origin.y + amg_curfloor->Pos.y));
			if(inv) obj->Pos.y += (d * 2.0f);
			else obj->Pos.y -= (d * 2.0f);
			obj->Scale.y = -obj->Scale.y;
			break;
		case 2:			// Eje Z
			d = ((obj->Origin.z + obj->Pos.z) - (amg_curfloor->Origin.z + amg_curfloor->Pos.z));
			if(inv) obj->Pos.z += (d * 2.0f);
			else obj->Pos.z -= (d * 2.0f);
			obj->Scale.z = -obj->Scale.z;
			break;
		default: return;
	}
	// Renderiza el objeto
	AMG_RenderObject(obj, 0);
	obj->Pos.x = tmp_pos.x; obj->Pos.y = tmp_pos.y; obj->Pos.z = tmp_pos.z;
	obj->Scale.x = tmp_scl.x; obj->Scale.y = tmp_scl.y; obj->Scale.z = tmp_scl.z;
	for(u8 i=0;i<obj->NGroups;i++) obj->Group[i].Diffuse = dif[i];
	if(l) sceGuEnable(GU_LIGHTING);
}


// Genera los puntos para el suelo
void AMG_GenerateFloorPoints(AMG_Object *obj, ScePspFVector3 *points){
	if((obj == NULL) || (points == NULL)) return;
	
	// Crea la matriz de transformación
	ScePspFMatrix4 m;
	AMG_LoadIdentityUser(&m);
	AMG_TranslateUser(&m, &obj->Pos);
	AMG_RotateUser(&m, &obj->Rot);
	AMG_ScaleUser(&m, &obj->Scale);
	
	// Crea un triángulo con la Bounding Box
	ScePspFVector3 p[3];
	/*p[0] = ((ScePspFVector3*)obj->Shadow)[21];
	p[1] = ((ScePspFVector3*)obj->Shadow)[22];
	p[2] = ((ScePspFVector3*)obj->Shadow)[23];*/
	p[0].x = obj->BBox[1].x; p[0].y = obj->BBox[1].y; p[0].z = obj->BBox[1].z;
	p[1].x = obj->BBox[1].x; p[1].y = obj->BBox[1].y; p[1].z = obj->BBox[0].z;
	p[2].x = obj->BBox[0].x; p[2].y = obj->BBox[1].y; p[2].z = obj->BBox[1].z;
	
	// Transforma los puntos
	points[0].x = (m.x.x * p[0].x) + (m.y.x * p[0].y) + (m.z.x * p[0].z);
	points[0].y = (m.x.y * p[0].x) + (m.y.y * p[0].y) + (m.z.y * p[0].z);
	points[0].z = (m.x.z * p[0].x) + (m.y.z * p[0].y) + (m.z.z * p[0].z);
	
	points[1].x = (m.x.x * p[1].x) + (m.y.x * p[1].y) + (m.z.x * p[1].z);
	points[1].y = (m.x.y * p[1].x) + (m.y.y * p[1].y) + (m.z.y * p[1].z);
	points[1].z = (m.x.z * p[1].x) + (m.y.z * p[1].y) + (m.z.z * p[1].z);
	
	points[2].x = (m.x.x * p[2].x) + (m.y.x * p[2].y) + (m.z.x * p[2].z);
	points[2].y = (m.x.y * p[2].x) + (m.y.y * p[2].y) + (m.z.y * p[2].z);
	points[2].z = (m.x.z * p[2].x) + (m.y.z * p[2].y) + (m.z.z * p[2].z);
}

// Renderiza una sombra
void AMG_RenderShadow(AMG_Object *obj, u8 l, ScePspFVector4 *plane){
	if(obj == NULL) return;
	if(obj->Shadow == NULL) return;
	if(l > 3) return;
	// Cuidado con la iluminacion
	u8 le = sceGuGetStatus(GU_LIGHTING);
	sceGuDisable(GU_LIGHTING);
	// Desactiva Alpha-Blending
	sceGuDisable(GU_BLEND);
	// Activa el stencil buffer
	if(AMG.PSM == GU_PSM_8888){
		sceGuStencilFunc(GU_EQUAL, 1, 1);
		sceGuStencilOp(GU_KEEP, GU_KEEP, GU_KEEP);
	}
	// Calcula el producto mixto entre el plano y la posición de la luz
	float dot = AMG_DotProduct(&AMG_Light[l].Pos, plane);
	// Calcula la matriz de sombra
	ScePspFMatrix4 mtx;
	mtx.x.x = dot - (AMG_Light[l].Pos.x * plane->x);
	mtx.y.x = 	  - (AMG_Light[l].Pos.x * plane->y);
	mtx.z.x = 	  - (AMG_Light[l].Pos.x * plane->z);
	mtx.w.x = 	  - (AMG_Light[l].Pos.x * plane->w);
	mtx.x.y =	  - (AMG_Light[l].Pos.y * plane->x);
	mtx.y.y = dot - (AMG_Light[l].Pos.y * plane->y);
	mtx.z.y = 	  - (AMG_Light[l].Pos.y * plane->z);
	mtx.w.y = 	  - (AMG_Light[l].Pos.y * plane->w);
	mtx.x.z =	  - (AMG_Light[l].Pos.z * plane->x);
	mtx.y.z = 	  - (AMG_Light[l].Pos.z * plane->y);
	mtx.z.z = dot - (AMG_Light[l].Pos.z * plane->z);
	mtx.w.z = 	  - (AMG_Light[l].Pos.z * plane->w);
	mtx.x.w =	  - (AMG_Light[l].Pos.w * plane->x);
	mtx.y.w = 	  - (AMG_Light[l].Pos.w * plane->y);
	mtx.z.w = 	  - (AMG_Light[l].Pos.w * plane->z);
	mtx.w.w = dot - (AMG_Light[l].Pos.w * plane->w);
	// Renderiza el objeto
	AMG_PushMatrix(GU_MODEL);
	AMG_MultMatrix(GU_MODEL, &mtx);
	AMG_Translate(GU_MODEL, &obj->Pos);
	AMG_Rotate(GU_MODEL, &obj->Rot);
	AMG_Scale(GU_MODEL, &obj->Scale);
	sceGuColor(0);
	sceGuDisable(GU_TEXTURE_2D);
	AMG_UpdateMatrices();
	sceGuDrawArray(GU_TRIANGLES, GU_VERTEX_32BITF | GU_TRANSFORM_3D, obj->NFaces*3, 0, obj->Shadow);
	AMG.DrawnVertices += (obj->NFaces*3);
	AMG_PopMatrix(GU_MODEL);
	if(le) sceGuEnable(GU_LIGHTING);
	sceGuEnable(GU_BLEND);
}

// Comienza el motor de reflejos
void AMG_StartReflection(AMG_Object *obj){
	if(AMG.PSM == GU_PSM_8888){
		sceGuEnable(GU_STENCIL_TEST);
		sceGuStencilFunc(GU_ALWAYS, 1, 1);
		sceGuStencilOp(GU_KEEP, GU_KEEP, GU_REPLACE);
	}
	sceGuDepthMask(GU_TRUE);
	AMG_RenderObject(obj, 0);
	sceGuDepthMask(GU_FALSE);
	amg_curfloor = obj;
}

// Calcula la ecuación del plano
void AMG_PlaneEquation(ScePspFVector3 *points, ScePspFVector4 *p){
	if((points == NULL) || (p == NULL)) return;
	// Haz el producto cruz para hallar el vector normal
	ScePspFVector3 v1, v2, v3;
	v1.x = points[1].x - points[0].x;
	v1.y = points[1].y - points[0].y;
	v1.z = points[1].z - points[0].z;
	v2.x = points[2].x - points[1].x;
	v2.y = points[2].y - points[1].y;
	v2.z = points[2].z - points[1].z;
	AMG_CrossProduct(&v1, &v2, &v3);
	AMG_Normalize(&v3);			// Normaliza el vector
	// Calcula la ecuación del plano
	p->x = v3.x;
	p->y = v3.y;
	p->z = v3.z;
	p->w = -((p->x * points[2].x) + (p->y * points[2].y) + (p->z * points[2].z));
}
