#include <stdio.h>
#include <stdlib.h>
#include <portaudio.h>

#define SAMPLE_RATE (44100)
#define FRAMES_PER_BUFFER (1024)
#define NUM_CHANNELS (1)
#define PA_SAMPLE_TYPE paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE (0)

static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo *timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData) {
    SAMPLE *out = (SAMPLE *)outputBuffer;
    const SAMPLE *in = (const SAMPLE *)inputBuffer;
    unsigned int i;
    (void)timeInfo; /* Prevent unused variable warnings. */
    (void)statusFlags;
    (void)userData;

    for (i = 0; i < framesPerBuffer; i++) {
        *out++ = *in++; /* left input channel */
#if NUM_CHANNELS == 2
        *out++ = *in++; /* right input channel */
#endif
    }

    return paContinue;
}

int main(void) {
    PaStream *stream;
    PaError err;

    err = Pa_Initialize();
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    err = Pa_OpenDefaultStream(&stream, NUM_CHANNELS, NUM_CHANNELS, PA_SAMPLE_TYPE,
                               SAMPLE_RATE, FRAMES_PER_BUFFER, paCallback, NULL);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    printf("Recording and playing...\n");
    printf("Press Enter to stop.\n");
    getchar();

    err = Pa_StopStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    Pa_CloseStream(stream);
    Pa_Terminate();

    return 0;
}

