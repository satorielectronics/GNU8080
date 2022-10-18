#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
//#include <gtk/gtk.h>
//#include <epoxy/gl.h>

#include "8080emu.h"
#include "cpu.c"
//#include "8080_dis.c"
//#include "shader.h"
//#include "main.h"

#define BYTES_PER_PIXEL 3

void readFileIntoMemoryAt(State8080* state, char* filename, uint32_t offset) {
    FILE *f= fopen(filename, "rb");
    if (f==NULL) {
        printf("error: Couldn't open %s\n", filename);
        exit(1);
    }
    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    uint8_t *buffer = &state->memory[offset];
    fread(buffer, fsize, 1, f);
    fclose(f);
}

State8080* init8080(void) {
    State8080* state = calloc(1, sizeof(State8080));
    state->memory = malloc(0x10000);  //16K
    return state;
}

//Returns time in microseconds
double timeusec() {
    //get time
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    //convert from seconds to microseconds
    return ((double)currentTime.tv_sec * 1E6) + ((double)currentTime.tv_usec);
}

void* emulatorThreadFun(void* arg) {
    State8080* state = (State8080*) arg;

    int done = 1;
    double now;
    int whichInt = 1;
    double lastInterrupt = 0.0;
    while (done == 1) {
        done = emulate8080(state);
        now = timeusec();
        if ( now - lastInterrupt > 16667) // 1/60 seconds has elapsed
        {
            // only do an interrupt if they are enabled
            if (state->int_enable) {
                if (whichInt == 1) {
                    whichInt = 2;
                }
                if (whichInt == 2) {
                    whichInt = 1;
                }
                generateInterrupt(state, 2); // Interrupt 2
                // Save the time we did this
                lastInterrupt = now;
            }
        }
    }
    return 0;
}

int main (int argc, char**argv) {

    State8080* state = init8080();

    readFileIntoMemoryAt(state, "invaders.h", 0);
    readFileIntoMemoryAt(state, "invaders.g", 0x800);
    readFileIntoMemoryAt(state, "invaders.f", 0x1000);
    readFileIntoMemoryAt(state, "invaders.e", 0x1800);

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, emulatorThreadFun, state);

    //GtkApplication *app;
    int status;

    //app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    //g_signal_connect (app, "activate", G_CALLBACK (start_window), state);
    //status = g_application_run (G_APPLICATION (app), argc, argv);
    //g_object_unref (app);

    return 0;
}

