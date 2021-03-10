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

//
// Dsp support functions
// 1.EOPI instruction for dsp sleep operation
// 2. enable interrupts and disable interrupts functions.
//
#ifndef H_DSP_SUPPORT_
#define H_DSP_SUPPORT_
inline assembly void sleep(void)  property(loop_free functional) clobbers()
{
    asm_begin
        eopi
    asm_end
}

inline assembly void enable_interrupt() property(loop_free functional) clobbers()
{
    asm_begin
        sr.ie = 1;
    asm_end
}

inline assembly void disable_interrupt() property(loop_free functional) clobbers()
{
    asm_begin
        sr.ie = 0;
    asm_end
}
#endif //H_DSP_SUPPORT_
