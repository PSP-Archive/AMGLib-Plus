/*
 * AMGlib Template
 * Powered by Andresmargar
 * from SCENEBETA
 */
 
// Includes AMG
#include <AMG/AMGLib.h>
#include <dirent.h>

// Module info
AMG_MODULE_INFO("AMG_Template", AMG_USERMODE, 1, 1);

// Variables adicionales para el explorador
// Additional variables (for the file explorer)
DIR *pdir = NULL;
struct dirent *pent = NULL;
u16 cur_file = 0;
u16 nfiles = 0;
char *workDir = NULL;
int cur_file_mode = 0;
char cur_file_path[128];

// Navegador de archivos		// File explorer
void fileExplorer(void){
	// Abre la MS0				// Open MS0
	pdir = opendir(workDir);
	
	// Limpia la pantalla		// Clear screen
	pspDebugScreenClear();
	pspDebugScreenSetXY(0, 0);
		
	// Pequeño navegador de archivos	// Small file explorer
	if(pdir){
		pspDebugScreenPrintf("\n %s\n\n", workDir);
		nfiles = 0;
		while((pent = readdir(pdir)) != NULL) {
			char a = ' ';
			if(nfiles == cur_file){
				a = '>';
				cur_file_mode = pent->d_stat.st_mode;
				sprintf(cur_file_path, pent->d_name);
			}
	    	if(strcmp(".", pent->d_name) == 0 || strcmp("..", pent->d_name) == 0)
	        	continue;
	    	if(FIO_S_ISDIR(pent->d_stat.st_mode))
	        	pspDebugScreenPrintf("%c [%s]\n", a, pent->d_name);
	    	else
	        	pspDebugScreenPrintf("%c %s\n", a, pent->d_name);
			nfiles ++;
		}
		closedir(pdir);
	}else{
		pspDebugScreenPrintf("Couldn't open %s", workDir);
	}
}

// Funcion Main()					// Main() function
// Bloque principal del programa	// Program entrypoint
int main(int argc, char **argv){

	// Inicializa los callbacks		// Setup callbacks
	AMG_SetupCallbacks();
	
	// Inicializa el motor multimedia	// Init multimedia engine
	AMG_InitMultimedia();
	
	// Inicializa el motor de audio		// Init audio system
	AMG_InitAudio();
	AMG_InitTimeSystem();
	
	// Inicializa la consola de texto	// Init text console
	pspDebugScreenInit();
	
	// Ruta inicial		// Initial path
	workDir = (char*) calloc (256, sizeof(char));
	sprintf(workDir, "ms0:/PSP");
	int len = 0;
	
	// El archivo MP3		// MP3 file
	AMG_MP3 *music = NULL;
	
	// Bucle infinito para mantener el programa funcionando
	// Infinite loop to keep the program running
	while(!AMG.Exit){
		
		if(music == NULL){
			// Explorador de archivos		// File explorer
			fileExplorer();
			
			// Moverse entre archivos y directorios		// Moving between files and folders
			AMG_ReadButtons();
			if(AMG_Button.Down &PSP_CTRL_UP){
				if(cur_file > 0) cur_file --;
				else cur_file = (nfiles - 1);
			}else if(AMG_Button.Down &PSP_CTRL_DOWN){
				if(cur_file < nfiles) cur_file ++;
			}else if(AMG_Button.Down &PSP_CTRL_CROSS){
				if(FIO_S_ISDIR(cur_file_mode)){
					strcat(workDir, "/");
					strcat(workDir, cur_file_path);
				}else{
					strcat(workDir, "/");
					strcat(workDir, cur_file_path);
					len = strlen(workDir);
					if(workDir[len - 3] == 'm' &&
					   workDir[len - 2] == 'p' &&
					   workDir[len - 1] == '3'){
							// Es un archivo MP3		// If it's a MP3 file...
							music = AMG_LoadMP3(workDir);
							AMG_LinkMP3(music);
							AMG_SetLoopMP3(music, 1);
					   }
				}
			}else if(AMG_Button.Down &PSP_CTRL_CIRCLE){
				len = strlen(workDir);
				while(workDir[len - 1] != '/'){
					len --;
					workDir[len] = 0;
				}
				workDir[len - 1] = 0;
			}
		}else{
			pspDebugScreenClear();
			pspDebugScreenSetXY(0, 0);
			pspDebugScreenPrintf("\n Playing \"%s\" [%d FPS]\n\n ", workDir, AMG.FPS);
			pspDebugScreenPrintf("Bit Rate: %d kb/s\n Sample Rate: %d Hz\n Channels: %d", music->BitRate, music->SampleRate, music->NChannels);
			pspDebugScreenPrintf("\n Played Samples: %d", (int)music->playedSamples);
			music->Play = 1;
		}
		
		// Espera hasta el VBlank (actualiza la pantalla)		// Wait for VBlank (update screen)
		sceDisplayWaitVblankStart();
		AMG_UpdateTime();
		AMG_UpdateAudio();
	}
	
	// Sal al XMB
	return AMG_ReturnXMB();
}
