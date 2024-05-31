#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <sndfile.h>
#include <portaudio.h>

#define FRAMES 1024

typedef struct PaData PaData;
struct PaData
{
    float *frames;
    sf_count_t frame_count;
    int channels;
    _Bool filled;
};

static int
pa_stream_callback(
    const void *input,
    void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo *timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData);

int
main(int argc, const char** argv)
{
    if (argc == 1)
    {
        printf("Audio file required\n");
        return 1;
    }

    if (!glfwInit())
    {
        printf("Failed to initialise GLFW!\n");
        return 1;
    }

    {
        PaError err = Pa_Initialize();
        if (err != paNoError)
        {
            printf("Failed to initialise PortAudio: %s\n",
                   Pa_GetErrorText(err));
            glfwTerminate();
            return 1;
        }
    }

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    GLFWwindow *window = glfwCreateWindow(
        640, 480,
        "Super Tic-Tac-Toe",
        NULL,
        NULL
    );
    if (!window)
    {
        printf("Failed to create window!\n");
        return 1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialise OpenGL context!\n");
        return 1;
    }

    glClearColor(0.5f, 0.f, 1.f, 1.f);

    SF_INFO info = {0};
    SNDFILE *file = sf_open(argv[1], SFM_READ, &info);
    if (!file)
    {
        printf("Failed to open audio file %s!\n", argv[1]);
        puts(sf_strerror(NULL));
    }

    PaData data;
    data.channels = info.channels;
    data.frames = calloc(FRAMES * info.channels, sizeof(float));
    data.frame_count = sf_readf_float(file, data.frames, FRAMES);
    data.filled = true;

    PaStream *stream = NULL;
    {
        PaError err = Pa_OpenDefaultStream(
            &stream, 0, info.channels,
            paFloat32,
            info.samplerate,
            FRAMES,
            pa_stream_callback,
            &data
        );
        if (err != paNoError)
        {
            printf("Failed to open default stream: %s\n",
                   Pa_GetErrorText(err));
        }
    }

    if (stream)
    {
        PaError err = Pa_StartStream(stream);
        if (err != paNoError)
        {
            printf("Failed to start stream: %s\n", Pa_GetErrorText(err));
        }
    }
    
    while (!glfwWindowShouldClose(window))
    {
        if (data.frame_count != 0 && !data.filled)
        {
            data.frame_count = sf_readf_float(file, data.frames, FRAMES);
            data.filled = true;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (stream)
    {
        if (Pa_IsStreamActive(stream) > 0)
        {
            PaError err = Pa_AbortStream(stream);
            if (err != paNoError)
            {
                printf("Failed to abort stream: %s\n", Pa_GetErrorText(err));
            }
        }

        PaError err = Pa_CloseStream(stream);
        if (err != paNoError)
        {
            printf("Failed to close stream: %s\n", Pa_GetErrorText(err));
        }
    }

    free(data.frames);
    data.frames = NULL;
    
    sf_close(file);
    file = NULL;

    {
        PaError err = Pa_Terminate();
        if (err != paNoError)
        {
            printf("Failed to terminate PortAudio: %s\n",
                   Pa_GetErrorText(err));
            glfwTerminate(); // could be better ik
            return 1;
        }
    }

    glfwTerminate();

    return 0;
}

static int
pa_stream_callback(
    const void *input,
    void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo *timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData)
{
    PaData *data = (PaData*)userData;
    float *out = (float*)output;

    sf_count_t frame_count = data->frame_count;
    int channels = data->channels;
    if (data->filled)
    {
        for (int i = 0; i < frame_count; ++i)
        {
            for (int j = 0; j < channels; ++j)
            {
                size_t index = i * channels + j;
                out[index] = data->frames[index];
            }
        }
        data->filled = false;
    }

    if (frame_count < FRAMES) return paComplete;
    return paContinue;
}
