#ifndef ETA_CSP_COMMON_SEMAPHORE_H
#define ETA_CSP_COMMON_SEMAPHORE_H

//
//! Processor to Processor Semaphore Enumeration
//
typedef enum
{
    //
    //! Source Ready 0-3
    //  These can be used as Source Ready in DMA commands.
    //  Either side can write / read these semaphore bits
    //  without restriction.
    //
    eSemaphoreSrcReady0=0,
    eSemaphoreSrcReady1=1,
    eSemaphoreSrcReady2=2,
    eSemaphoreSrcReady3=3,
    
    // These are the same as 0-3 above, but have a name that more closely
    // matches the DMA usage. 
    eSemaphoreSrcReady12=0,
    eSemaphoreSrcReady13=1,
    eSemaphoreSrcReady14=2,
    eSemaphoreSrcReady15=3,

    //
    //! Destination Ready 0-3
    //  These can be used as Destination Ready in DMA commands.
    //  Either side can write / read these semaphore bits
    //  without restriction.
    //
    eSemaphoreDstReady0=4,
    eSemaphoreDstReady1=5,
    eSemaphoreDstReady2=6,
    eSemaphoreDstReady3=7,
    
    // These are the same as 0-3 above, but have a name that more closely
    // matches the DMA usage. 
    eSemaphoreDstReady12=4,
    eSemaphoreDstReady13=5,
    eSemaphoreDstReady14=6,
    eSemaphoreDstReady15=7,

    //
    //! Semaphore 0-3
    //  These are arbitrated semaphores, where either side
    //  can read. A read of a 0 indicates the reading
    //  processor has just reserved the semaphore and it
    //  is now set to 1. Either side can write the
    //  register without restriction, but the intention
    //  is to only write it to 0 when done with the
    //  semaphore. 
    //
    eSemaphoreReadRequest0=8,
    eSemaphoreReadRequest1=9,
    eSemaphoreReadRequest2=10,
    eSemaphoreReadRequest3=11,
}
tSemaphoreType;


//
//! Processor to Processor Semaphore Values
//
typedef enum
{
    //
    //! Semaphore Value
    //
    eSemaphoreFree=0,
    eSemaphoreReserved=1,
}
tSemaphoreOwnership;


#endif // ETA_CSP_COMMON_SEMAPHORE_H

