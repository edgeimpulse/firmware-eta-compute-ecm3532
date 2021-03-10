#ifndef H_RPC_
#define H_RPC_

#define NO_OF_MODULE_ID_BITS              2
#define NO_OF_USER_DEFINED_FIELD_BITS     3
#define NO_OF_HEADER_FILTER_BITS      (NO_OF_MODULE_ID_BITS + NO_OF_USER_DEFINED_FIELD_BITS)

typedef int (*tnotifyEventCb) (uint32_t header,uint32_t data);
int rpcInit(void);
void  rpcDeinit(void);
void rpcRegisterEventCb(uint8_t eventHeaderMask, tnotifyEventCb cbFn);
int rpcSubmitWork(uint8_t moduleId, uint8_t operation, void* params);


/*TODO*/
#define DSP_WAKEUP_MSG  (1 << 0)

#endif //
