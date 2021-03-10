//////////////////////////////////////////////////////////////////////////
// The confidential and proprietary information contained in this file may
// only be used by a person authorized under and to the extent permitted
// by a subsisting licensing agreement from Eta Compute Inc.
//
//        (C) Copyright 2019 Eta Compute Inc
//                All Rights Reserved
//
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from Eta Compute Inc.
//////////////////////////////////////////////////////////////////////////
#ifndef H_DSP_MALLOC
#define H_DSP_MALLOC

void Dsp_Xmem_heap_Init(void);
void Dsp_Ymem_heap_Init(void);

void *Dsp_Xmem_Alloc(unsigned size);
void _YMEM * Dsp_Ymem_Alloc(unsigned size);

void Dsp_Xmem_Free( void *ptr);
void Dsp_Ymem_Free( void _YMEM *ptr);

void Dsp_Xmem_heap_Info( void);
void Dsp_Ymem_heap_Info( void);


//void Dsp_Xmem_Compact( void );
//void Dsp_Ymem_Compact(void );

#endif //H_DSP_MALLOC
