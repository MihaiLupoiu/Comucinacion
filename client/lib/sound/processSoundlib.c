
//to round supperior
#include <tgmath.h>

#include "../../include/processSoundlib.h"

#define WORD_LENGTH 50

/*Filtro de altavoces*/

#define c       343
#define pi      3.141592
#define FS      44100
#define land    -1 // porque está fuera del array de altavoces

double comp      = 0;
int ang_max     = 90*(pi/180);// rad
int nalt        = 96; //numero de altavoces

double H_1[61] ={-0.00661109583724264,-0.00650043529566125,-0.00632930310706349,-0.00617482490003971,-0.00602953993132536,-0.00598452022424796,-0.00604361199320415,-0.00630563700873370,-0.00677794258748761,-0.00755797763617839,-0.00864652310346542,-0.0101290944464563,-0.0119893471647713,-0.0142908353440042,-0.0169911140198601,-0.0201243563895989,-0.0236165930309936,-0.0274695564585432,-0.0315773165621993,-0.0359115018748141,-0.0403393430824099,-0.0448101568406542,-0.0491745430350945,-0.0533717619323007,-0.0572496004201327,-0.0607520798261281,-0.0637393605766290,-0.0661751281673016,-0.0679458772409232,-0.0690473584402903,2.29712671743954,-0.0690473584402903,-0.0679458772409232,-0.0661751281673016,-0.0637393605766290,-0.0607520798261281,-0.0572496004201327,-0.0533717619323007,-0.0491745430350945,-0.0448101568406542,-0.0403393430824099,-0.0359115018748141,-0.0315773165621993,-0.0274695564585432,-0.0236165930309936,-0.0201243563895989,-0.0169911140198601,-0.0142908353440042,-0.0119893471647713,-0.0101290944464563,-0.00864652310346542,-0.00755797763617839,-0.00677794258748761,-0.00630563700873370,-0.00604361199320415,-0.00598452022424796,-0.00602953993132536,-0.00617482490003971,-0.00632930310706349,-0.00650043529566125,-0.00661109583724264};

WFS waveFieldSynthesis(ClientSpeakers speakers, float posX, float posY ){

       WFS result;
       result.parray = (int *)calloc(speakers.speakers_number, sizeof(int));;
       result.pos = (int *)calloc(speakers.speakers_number, sizeof(int));;

       result.tn = (float *)malloc(speakers.speakers_number*sizeof(float));
       result.an = (float *)malloc(speakers.speakers_number*sizeof(float));


       int i;

       float fte[2]={posX,posY};

       float *fuente = fte;
       float x = fuente[0];
       float y = fuente[1];

       float difX[speakers.speakers_number];
       float difY[speakers.speakers_number];

       for (i = 0; i < speakers.speakers_number; ++i) {
           difX[i] = speakers.list_positions_speakers[i][0]-x;
           difY[i] = speakers.list_positions_speakers[i][1]-y;
       }

       float alfa[speakers.speakers_number];// Ángulo //necesito esto
       for (i = 0; i < speakers.speakers_number; ++i) {
           alfa[i] = atan2(difY[i],difX[i])*180/pi;
       }

       int sizeOfPos = 0;
       //memset(pos, -1, sizeof(int)*speakers.speakers_number);
       for (i = 0; i < speakers.speakers_number; ++i) {
           if ( ( ( alfa[i] <= speakers.speakers_tecta[i]+90 ) && ( alfa[i] >= speakers.speakers_tecta[i]-90 ) ) ){
               result.pos[i]=i;
               result.parray[i] = 1;
               sizeOfPos++;
           }else{
               result.pos[i]=-1;
               result.parray[i] = 0;
           }
       }

   /*    int parray_act[sizeOfPos];
       int j = 0;
       for (i = 0; i < speakers.speakers_number; i++) {
           if(result.pos[i] > -1){
               parray_act[j] = result.pos[i];
               j++;
           }
       }
   */

       float r;

       for (i = 0; i < speakers.speakers_number; ++i) {
           r = sqrt( (difX[i] * difX[i]) + (difY[i] * difY[i]) );
           result.an[i] = 1/(sqrt(r));
           result.tn[i] =-land*(FS*(r/c));
       }

/*       printf("\n\n Resultados obtenidos: \n\n");
       for (i = 0; i < speakers.speakers_number; ++i) {
            printf("    an\t\t\t[%f]\t", result.an[i]);
            printf("    tn\t\t\t[%f]\t", result.tn[i]);
            printf("    pos\t\t\t[%d]\t", result.pos[i]);
            printf("    parray\t\t\t[%d]\n", result.parray[i]);
       }
*/
       return result;
   }

char * handleWAVFiles(ClientSound soundConfig){
    // -------------------------------------- HANDLE OF WAV FILES -------------------------------------//
    char * archivos_senal;

    /*Modificado la ruta de ./FicherosPrueba/001_piano.wav
    * para que lea desde una carpeta inferior
    * */
    archivos_senal= (char *)calloc(soundConfig.sounds_number* soundConfig.word_length, sizeof(char));

    int i;
    for (i = 0; i < soundConfig.sounds_number; ++i) {
        strcpy(archivos_senal + i*soundConfig.word_length, soundConfig.sounds_list+i*soundConfig.word_length);
    }

// -------------------------------------- HANDLE OF ALSA DEVICE -------------------------------------//

    return archivos_senal;
}

SuperWAV loadFile(ClientSound soundConfig){
    SuperWAV filewav;

    filewav.filewav = (unsigned char **) malloc (soundConfig.sounds_number*sizeof(unsigned char *));
    filewav.leido = (int*) malloc (soundConfig.sounds_number*sizeof(int));

    char * archivos_senal = handleWAVFiles(soundConfig);

    int i;
    for (i = 0; i < soundConfig.sounds_number; ++i) {
        filewav.leido[i] = OpenWavConvert32(&filewav.filewav[i],archivos_senal + i*soundConfig.word_length);
    }
    return filewav;
}

// channels starts in 1.
void generateSongWFS(int** bufferToModify, int index,SuperWAV fileWAV, int songNumber, int buffSize, WFS values, int chanals) {

    int i;
    int j;

    for (j = 0; j < chanals; ++j) {
        if( NULL == bufferToModify[j] ) {
            bufferToModify[j] = (int *) malloc (buffSize * sizeof(int));
        }
    }

    int itn = 0;
    int val = 0;
    int startPosBuffer = index*buffSize;
    int maxPos = 0;

    for (j = 0; j < chanals; ++j) {

            if(values.parray[j] == 1){
                itn = ceil(values.tn[j]);
                maxPos = itn + (fileWAV.leido[songNumber]);
            }

            int actualPosBuff = 0;
            for (i = 0; i < buffSize; ++i) {

                if(values.parray[j] == 1){

                    actualPosBuff = i+startPosBuffer;

                    if(itn <= actualPosBuff && actualPosBuff < maxPos){
                        val = (*((int *) fileWAV.filewav[j] + actualPosBuff - itn ));
                    }
                }
                bufferToModify[j][i] = (int)(values.an[j]*val); //por an1
            }
        }

}
void** castBufferToVoid(int** buffer, int chanals){

    int i;
    for (i = 0; i < chanals; ++i) {
        buffer[i] = (void *) buffer[i];
    }
    return (void**)buffer;
}