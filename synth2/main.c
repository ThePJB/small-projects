#include "application.h"
#include "gef.h"

#include "aud.h"
#include "portaudio.h"

#define NUM_SECONDS   (5)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define TABLE_SIZE   (200)
typedef struct
{
    float sine[TABLE_SIZE];
    int left_phase;
    int right_phase;
    char message[20];
}
paTestData;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int patestCallback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    paTestData *data = (paTestData*)userData;
    float *out = (float*)outputBuffer;
    unsigned long i;

    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) inputBuffer;
    
    for( i=0; i<framesPerBuffer; i++ )
    {
        *out++ = sin(2*M_PI*440 * ((float)i / (float)framesPerBuffer));
        *out++ = sin(2*M_PI*440 * ((float)i / (float)framesPerBuffer));
        /*
        *out++ = data->sine[data->left_phase];
        *out++ = data->sine[data->right_phase];
        data->left_phase += 1;
        if( data->left_phase >= TABLE_SIZE ) data->left_phase -= TABLE_SIZE;
        data->right_phase += 3;
        if( data->right_phase >= TABLE_SIZE ) data->right_phase -= TABLE_SIZE;
        */
    }
    
    return paContinue;
}
static void StreamFinished( void* userData )
{
   paTestData *data = (paTestData *) userData;
   printf( "Stream Completed: %s\n", data->message );
}


int main(int argc, char** argv) {
    application app = {0};
    application_init(&app);

    while (app.keep_going) {
        gef_start_frame(&app.gc);
        application_handle_input(&app);
        application_draw(&app);
        gef_end_frame(&app.gc);
    }

    return 0;
}