/*
 * Copyright (C) 2018 Eta Compute, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "cf6_chess.h"
#include "mathlib.h"
#include "reg.h"
#include "eta_csp_mailbox.h"
//#include "eta_csp_chip.h"
#include "eta_chip.h"
#include "eta_csp_io.h"

#include "fft.h"
#include "m3Interface.h"
#include "utility.h"

volatile fix   _XALIGN(FFT_SIZE) sensor_x[N_FFT]; //sensor raw samples x component
volatile fix   _XALIGN(FFT_SIZE) sensor_y[N_FFT]; //sensor raw samples y component
volatile fix   _XALIGN(FFT_SIZE) sensor_z[N_FFT]; //sensor raw samples z component

volatile fix   _XALIGN(FFT_SIZE) temp_fftX[N_FFT]; //FFT of sensor raw samples x component

#ifdef FFT_WITH_WINDOWING
#include "hamming512.h"
#endif

static uint16_t arrayIndex = 0;
static void resetArrayIndex(void)
{
    arrayIndex = 0;
}

//Performs fft of all 3 components. Input is in de-interleaved arrays: sensor_x[], sensor_y[] and sensor_z[].
//FFT output temporarily stored in temporary buffer and then DMA'ed to IOMEM buffer at "offset"
//offset --> Iomemoffset in bytes.
//length --> Total FFT frame length for all 3 components = (512 * 3 * 2), each components is 512 uint16 type.
void doFFTOnSensorData(uint16_t offset, uint16_t length)
{
    uint16_t dataoffset;
    //dataoffset = offset;
    volatile void * iomemaddr = 0;
    iomemaddr = getSharedMemoryAddress(offset);

    dataoffset = (uint16_t)iomemaddr;

#ifdef FFT_WITH_WINDOWING
    TRACE("Hamming window on sensor data\n\r");
#endif
    TRACE("FFT on sensor data\n\r");

//Perform FFT of sensor data
    const CFX_FFT_TABLE* table; //variable for twiddle factors
    table = cfx_fft_tables[6];

    cfx_fft_r_stagescaling_Process((fix*)&sensor_x[0], (fix*)&temp_fftX[0], N_FFT, table); //fft
    TRACE("X[0]= %d \n\r", temp_fftX[0]);
    //TRACE("X-component DMA:\n\r");
    dma_xmem_to_iomem(iomemaddr , (void *)&temp_fftX[0], length / 3);
    dataoffset += length / 6;
    //iomemaddr += (length / 3) / 2;
    iomemaddr = (volatile void *)dataoffset;


    cfx_fft_r_stagescaling_Process((fix*)&sensor_y[0], (fix*)&temp_fftX[0], N_FFT, table); //fft
    TRACE("Y[0]=%d\n\r", temp_fftX[0]);
    //TRACE("Y-component DMA:\n\r");
    dma_xmem_to_iomem(iomemaddr , (void*)&temp_fftX[0], length / 3);
    dataoffset += length / 6;
    //iomemaddr += (length / 3) / 2;
    iomemaddr = (volatile void *)dataoffset;

    cfx_fft_r_stagescaling_Process((fix*)&sensor_z[0], (fix*)&temp_fftX[0], N_FFT, table); //fft
    TRACE("Z[0]=%d\n\r", temp_fftX[0]);
    //TRACE("Z-component DMA:\n\r");
    dma_xmem_to_iomem(iomemaddr , (void*)&temp_fftX[0], length / 3);

    resetArrayIndex();
}


void deInterleaveSensorData(uint16_t* data)
{
#ifdef FFT_WITH_WINDOWING
    acc a1;
    fix b;

    b = hamming512[arrayIndex];

    a1 = *(fix*)&data[0] * b;
    sensor_x[arrayIndex] = fix(a1);

    a1 = *(fix*)&data[1] * b;
    sensor_y[arrayIndex] = fix(a1);

    a1 = *(fix*)&data[2] * b;
    sensor_z[arrayIndex] = fix(a1);

    //TRACE("Hamming window applied on sensor data\n\r");
#else
    sensor_x[arrayIndex] = *(fix*)&data[0];
    sensor_y[arrayIndex] = *(fix*)&data[1];
    sensor_z[arrayIndex] = *(fix*)&data[2];
    //TRACE("No windowing on sensor data\n\r");
#endif
    arrayIndex++;
}
