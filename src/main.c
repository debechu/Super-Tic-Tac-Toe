#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <sndfile.h>
#include <portaudio.h>

typedef struct Audio Audio;
struct Audio
{
    SF_INFO info;
    float *data;
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

    Audio audio = {0};
    SNDFILE *file = sf_open(argv[1], SFM_READ, &audio.info);
    if (!file)
    {
        printf("Failed to open audio file %s!\n", argv[1]);
        puts(sf_strerror(NULL));
    }

    // TODO(debe): use a smaller buffer and put in loop.
    audio.data = calloc(
        audio.info.frames * audio.info.channels,
        sizeof(float)
    );
    if (!audio.data)
    {
        printf("Ran out of memory!\n");
        sf_close(file);
        {
            PaError err = Pa_Terminate();
            if (err != paNoError)
            {
                printf("Failed to terminate PortAudio: %s\n",
                       Pa_GetErrorText(err));
            }
        }
        glfwTerminate();
        return 1;
    }
    audio.info.frames = (int)sf_readf_float(file, audio.data, audio.info.frames);

    PaStream *stream = NULL;
    {
        PaError err = Pa_OpenDefaultStream(
            &stream, 0, audio.info.channels,
            paFloat32,
            audio.info.samplerate,
            audio.info.frames,
            pa_stream_callback,
            &audio
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
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (stream)
    {
        PaError err = Pa_AbortStream(stream);
        if (err != paNoError)
        {
            printf("Failed to abort stream: %s\n", Pa_GetErrorText(err));
        }
    }

    free(audio.data);
    audio.data = NULL;
    
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
    Audio *audio = (Audio*)userData;
    float *out = (float*)output;

    for (int i = 0; i < audio->info.frames; ++i)
    {
        for (int j = 0; j < audio->info.channels; ++j)
        {
            size_t index = i * audio->info.channels + j;
            out[index] = audio->data[index];
        }
    }
}
