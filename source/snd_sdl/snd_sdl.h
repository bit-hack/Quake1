#pragma once
#include <stdbool.h>


typedef struct
{
    bool gamealive;
    bool soundalive;
    bool splitbuffer;
    int channels;
    int samples; // mono samples in buffer
    int submission_chunk; // don't mix less than this #
    int samplepos; // in mono samples
    int samplebits;
    int speed;
    unsigned char* buffer;
} dma_t;

extern volatile dma_t* shm;
extern volatile dma_t sn;

void SNDDMA_Submit(void); // <--- submit buffers to waveout device

void SND_InitScaletable(void);

// initializes cycling through a DMA buffer and returns information on it
bool SNDDMA_Init(void);

// gets the current DMA position
int SNDDMA_GetDMAPos(void);

// shutdown the DMA xfer.
void SNDDMA_Shutdown(void);
