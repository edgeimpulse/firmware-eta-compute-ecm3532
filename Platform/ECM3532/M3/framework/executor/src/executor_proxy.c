#include "config.h"
#include "executor_public.h"
#include "executor_config.h"
#include "executor_common.h"
#include "executor_private.h"
#include "executor_trace.h"
#include "executor_op.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#if defined(CONFIG_OP_M3_CONV2D_Q7) ||  defined(CONFIG_OP_DSP_CHW_3X3_CONV2D_STRIDE2_PAD0_RELU) || defined(CONFIG_OP_DSP_CHW_3X3_CONV2D_STRIDE1_PAD0_RELU) ||  defined(EXEC_OP_DSP_CHW_2X2_CONV2D_STRIDE2_PAD0_RELU)
 // proxy funnctions
ExecStatus Exec_conv2d_q7 ( uint8_t execHwId, ExecOperand_t * inArray, ExecOperand_t * wt, ExecOperand_t * bias, ExecOperand_t * outArray, ExecOperand_t * buffIn, const conv2d_opt * opt)
{
    ExecStatus status = EXEC_STATUS_OK;
    exec_conv2d_q7_t * params;
    ExecWork_t work;
    uint8_t index;
       // Get the param allocated and fill all the details
     params = ( exec_conv2d_q7_t *) pvPortMalloc(sizeof (exec_conv2d_q7_t));
     params->wt =  (  const q7_t *) ExecGetBufAddr(wt);
     params->bias =  (  const q7_t *) ExecGetBufAddr(bias);
     memcpy( &(params->opt),opt,sizeof(conv2d_opt));
     work.params = (void *) params;
     work.execHwId = execHwId;
     work.numInputs = 2;
     work.inbufs[0] = inArray;
     work.inbufs[1] = buffIn;
     work.outBuf = outArray;
      if (EXEC_HW_ID_M3 == execHwId){
      work. opID = EXEC_OP_M3_CONV2D_Q7;
     index = CheckAndScheduleWork(&work,0);
      SubmitM3Work(index);
    }
    if (EXEC_HW_ID_DSP == execHwId){
       // Find the variant
       if ((opt->filt_rows == 3) && ( opt->filt_cols == 3) && (opt->row_stride == 2) && (opt->row_pad == 0)){
            work. opID = EXEC_OP_DSP_CHW_3X3_CONV2D_STRIDE2_PAD0_RELU;

       }
      if ((opt->filt_rows == 3) && ( opt->filt_cols == 3) && (opt->row_stride == 1)){
            work. opID = EXEC_OP_DSP_CHW_3X3_CONV2D_STRIDE1_PAD0_RELU;
       }
       if ((opt->filt_rows == 2) && ( opt->filt_cols == 2) && (opt->row_stride == 2) && (opt->row_pad == 0)){
            work. opID = EXEC_OP_DSP_CHW_2X2_CONV2D_STRIDE2_PAD0_RELU;
       }
     index = CheckAndScheduleWork(&work,0);
      SubmitDSPWork(index);
    }
    return status;
}
#endif
#if defined (CONFIG_OP_M3_DS_CONV2D_Q7) || defined(CONFIG_OP_DSP_DS_CHW_3X3_CONV2D_STRIDE2_PAD0_RELU) || \
    defined (CONFIG_OP_DSP_DS_CHW_2X2_CONV2D_STRIDE2_PAD0_RELU) ||\
    defined (CONFIG_OP_DSP_DS_CHW_3X3_CONV2D_STRIDE1_PAD0_RELU)
ExecStatus Exec_ds_conv2d_q7 ( uint8_t execHwId, ExecOperand_t * inArray, ExecOperand_t * wt, ExecOperand_t * bias, ExecOperand_t * outArray, ExecOperand_t * buffIn, const conv2d_opt * opt)
{
    ExecStatus status = EXEC_STATUS_OK;
    exec_conv2d_q7_t * params;
    ExecWork_t work;
    uint8_t index;
       // Get the param allocated and fill all the details
     params = ( exec_conv2d_q7_t *) pvPortMalloc(sizeof (exec_conv2d_q7_t));
     params->wt =  (  const q7_t *) ExecGetBufAddr(wt);
     params->bias =  (  const q7_t *) ExecGetBufAddr(bias);
     memcpy( &(params->opt),opt,sizeof(conv2d_opt));
     work.params = (void *) params;
     work.execHwId = execHwId;
     work.numInputs = 2;
     work.inbufs[0] = inArray;
     work.inbufs[1] = buffIn;
     work.outBuf = outArray;
      if (EXEC_HW_ID_M3 == execHwId){
      work. opID = EXEC_OP_M3_DS_CONV2D_Q7;
     index = CheckAndScheduleWork(&work,0);
      SubmitM3Work(index);
    }
    if (EXEC_HW_ID_DSP == execHwId){
       // Find the variant
       if ((opt->filt_rows == 3) && ( opt->filt_cols == 3) && (opt->row_stride == 2) && (opt->row_pad == 0)){
            work. opID = EXEC_OP_DSP_DS_CHW_3X3_CONV2D_STRIDE2_PAD0_RELU;

       }
      if ((opt->filt_rows == 3) && ( opt->filt_cols == 3) && (opt->row_stride == 1)){
            work. opID = EXEC_OP_DSP_DS_CHW_3X3_CONV2D_STRIDE1_PAD0_RELU;
       }
       if ((opt->filt_rows == 2) && ( opt->filt_cols == 2) && (opt->row_stride == 2) && (opt->row_pad == 0)){
            work. opID = EXEC_OP_DSP_DS_CHW_2X2_CONV2D_STRIDE2_PAD0_RELU;
       }
       work.variant = DEPTHWISE_CONV_2D;

     index = CheckAndScheduleWork(&work,0);

      SubmitDSPWork(index);
    }
    return status;
}
#endif
#if defined(CONFIG_OP_DSP_DS_CHW_3X3_CONV2D_STRIDE1_PAD0_RELU_INPLACE) ||\
    defined(CONFIG_OP_DSP_DS_CHW_3X3_CONV2D_STRIDE2_PAD0_RELU_INPLACE)
ExecStatus Exec_ds_conv2d_q7_inplace ( uint8_t execHwId, ExecOperand_t * inOutArray, ExecOperand_t * wt, ExecOperand_t * bias, ExecOperand_t * buffIn, const conv2d_opt * opt)
{
    ExecStatus status = EXEC_STATUS_OK;
    exec_conv2d_q7_t * params;
    ExecWork_t work;
    uint8_t index;
       // Get the param allocated and fill all the details
     params = ( exec_conv2d_q7_t *) pvPortMalloc(sizeof (exec_conv2d_q7_t));
     params->wt =  (  const q7_t *) ExecGetBufAddr(wt);
     params->bias =  (  const q7_t *) ExecGetBufAddr(bias);
     memcpy( &(params->opt),opt,sizeof(conv2d_opt));
     work.params = (void *) params;
     work.execHwId = execHwId;
     work.numInputs = 2;
     work.inbufs[0] = inOutArray;
     work.inbufs[1] = buffIn;
     /* NO Out buf, Output saved in inOutArray
      * inOutArray is BUF_TYPE_INOUT*/
     work.outBuf = inOutArray;
     if (EXEC_HW_ID_M3 == execHwId){
       //work. opID = EXEC_OP_M3_DS_CONV2D_SB_LAYER3_Q7;
       //index = CheckAndScheduleWork(&work,1);
       //SubmitM3Work(index);
     }
     if (EXEC_HW_ID_DSP == execHwId){
       // Find the variant
       if ((opt->filt_rows == 3) && ( opt->filt_cols == 3) && (opt->row_stride == 2) &&
           (opt->row_pad == 0))
       {
         work. opID = EXEC_OP_DSP_DS_CHW_3X3_CONV2D_STRIDE2_PAD0_RELU_INPLACE;
       }
       if ((opt->filt_rows == 3) && ( opt->filt_cols == 3) && (opt->row_stride == 1))
       {
         work. opID = EXEC_OP_DSP_DS_CHW_3X3_CONV2D_STRIDE1_PAD0_RELU_INPLACE;
       }
       //if ((opt->filt_rows == 2) && ( opt->filt_cols == 2) && (opt->row_stride == 2) && (opt->row_pad == 0))
       //{
       //  work. opID = EXEC_OP_DSP_DS_CHW_2X2_CONV2D_STRIDE2_PAD0_RELU_INPLACE;
       //}
       work.variant = DEPTHWISE_CONV_2D;

       index = CheckAndScheduleWork(&work,1);

       SubmitDSPWork(index);

     }
     return status;
}
#endif
#if defined(CONFIG_OP_DSP_HWC_PW_CONV2D_Q7) || defined(CONFIG_OP_M3_PW_CONV2D_Q7)
ExecStatus Exec_pw_conv2d_q7 ( uint8_t execHwId, ExecOperand_t * inArray, ExecOperand_t * wt, ExecOperand_t * bias, ExecOperand_t * outArray, ExecOperand_t * buffIn, const conv2d_opt *opt)
{
    ExecStatus status = EXEC_STATUS_OK;
    exec_conv2d_q7_t * params;
    ExecWork_t work;
    uint8_t index;
       // Get the param allocated and fill all the details
     params = ( exec_conv2d_q7_t *) pvPortMalloc(sizeof (exec_conv2d_q7_t));
     params->wt =  (  const q7_t *) ExecGetBufAddr(wt);
     params->bias =  (  const q7_t *) ExecGetBufAddr(bias);
     memcpy( &(params->opt),opt,sizeof(conv2d_opt));
     work.params = (void *) params;
     work.execHwId = execHwId;
     work.numInputs = 2;
     work.inbufs[0] = inArray;
     work.inbufs[1] = buffIn;
     work.outBuf = outArray;
      //ecm35xx_printf("got index back = %d...\r\n",index);
    if (EXEC_HW_ID_M3 == execHwId)
    {

        work. opID = EXEC_OP_M3_PW_CONV2D_Q7;
        index = CheckAndScheduleWork(&work,0);
        SubmitM3Work(index);
    }
     if (EXEC_HW_ID_DSP == execHwId)
    {
        //ecm35xx_printf(" submit to  DSP  work\r\n");
        work. opID = EXEC_OP_DSP_HWC_PW_CONV2D_Q7;
        index = CheckAndScheduleWork(&work,0);
        SubmitDSPWork(index);
    }
     return status;
    }
#endif
#ifdef CONFIG_OP_DSP_HWC_PW_CONV2D_Q7_FAST
ExecStatus Exec_pw_conv2d_q7_fast ( uint8_t execHwId, ExecOperand_t * inArray, ExecOperand_t * wt, ExecOperand_t * bias, ExecOperand_t * outArray, ExecOperand_t * buffIn, const conv2d_opt *opt)
{
    ExecStatus status = EXEC_STATUS_OK;
    exec_conv2d_q7_t * params;
    ExecWork_t work;
    uint8_t index;
       // Get the param allocated and fill all the details
     params = ( exec_conv2d_q7_t *) pvPortMalloc(sizeof (exec_conv2d_q7_t));
     params->wt =  (  const q7_t *) ExecGetBufAddr(wt);
     params->bias =  (  const q7_t *) ExecGetBufAddr(bias);
     memcpy( &(params->opt),opt,sizeof(conv2d_opt));
     work.params = (void *) params;
     work.execHwId = execHwId;
     work.numInputs = 2;
     work.inbufs[0] = inArray;
     work.inbufs[1] = buffIn;
     work.outBuf = outArray;
     work. opID = EXEC_OP_DSP_HWC_PW_CONV2D_Q7_FAST;
     index = CheckAndScheduleWork(&work,0);
      //ecm35xx_printf("got index back = %d...\r\n",index);

     if (EXEC_HW_ID_DSP == execHwId)
    {
        //ecm35xx_printf(" submit to  DSP  work\r\n");

        SubmitDSPWork(index);
    }
     return status;

}
#endif
#ifdef CONFIG_OP_M3_AVPOOL2D_Q7
ExecStatus Exec_avepool2d_q7( uint8_t execHwId, ExecOperand_t * inArray, ExecOperand_t  * outArray, pool2d_opt *opt)
{
    exec_avepool2d_q7_t * params;
    ExecStatus status = EXEC_STATUS_OK;
    ExecWork_t work;
    uint8_t index;
    params = (exec_avepool2d_q7_t *) pvPortMalloc(sizeof (exec_avepool2d_q7_t));
    memcpy( &(params->opt),opt,sizeof(pool2d_opt));
    work.params = (void *) params;
    work.execHwId = execHwId;
    work. opID = EXEC_OP_M3_AVPOOL2D_Q7;
    work.numInputs = 1;
    work.inbufs[0] = inArray;
    work.outBuf = outArray;
    index = CheckAndScheduleWork(&work,0);
    if (EXEC_HW_ID_M3 == execHwId)
    {
        SubmitM3Work(index);
    }
    return status;
}
#endif
#ifdef CONFIG_OP_M3_RELU_Q7
void  Exec_relu_q7( uint8_t execHwId, ExecOperand_t * data, uint16_t size)
{
    ExecWork_t work;
     uint8_t index;
    work.execHwId = execHwId;
     work. opID = EXEC_OP_M3_RELU_Q7;
     work.numInputs = 1;
     work.inbufs[0] = data;
     work.outBuf = data;
     work.params = 0;
     index = CheckAndScheduleWork(&work,1); // in out case
    if (EXEC_HW_ID_M3 == execHwId)
    {

        SubmitM3Work(index);
    }
     //TBD:  Handle DSP later. Depepnds if they ensure same prototype. Assuming in future generic functions
    return;
}
#endif
#ifdef CONFIG_OP_DSP_CHW_2X2_CONV2D_RELU_AVGPOOL_Q7
ExecStatus Exec_conv2d_q7_CHW_ker2x2_stride1_pad0_relu_avgPool_2x2_stride2_pad0( uint8_t execHwId, ExecOperand_t * inArray, ExecOperand_t * wt,  ExecOperand_t * bias, ExecOperand_t * outArray, ExecOperand_t * buffIn, const conv2d_relu_avgpool_opt *opt )
{
    ExecStatus status = EXEC_STATUS_OK;
    exec_conv2d_relu_avgpool_q7_t  * params;
    ExecWork_t work;
    uint8_t index;
    // Get the param allocated and fill all the details
     params = ( exec_conv2d_relu_avgpool_q7_t *) pvPortMalloc(sizeof (exec_conv2d_relu_avgpool_q7_t));
     params->wt =  (  const q7_t *) ExecGetBufAddr(wt);
     params->bias =  (  const q7_t *) ExecGetBufAddr(bias);
     memcpy( &(params->opt),opt,sizeof(conv2d_relu_avgpool_opt));
     work. opID = EXEC_OP_DSP_CHW_2X2_CONV2D_RELU_AVGPOOL_Q7;
     work.params = (void *) params;
     work.execHwId = execHwId;
     work.numInputs = 2;
     work.inbufs[0] = inArray;
     work.inbufs[1] = buffIn;
     work.outBuf = outArray;
     //ecm35xx_printf(" check and schedule for  DSP  work\r\n");
     index = CheckAndScheduleWork(&work,0);
     if (EXEC_HW_ID_DSP == execHwId)
    {
        //ecm35xx_printf(" submit to  DSP  work\r\n");
        SubmitDSPWork(index);
    }
     //TBD:  Handle M3 later. Depepnds if they ensure same prototype. Assuming in symmetry in future kernel  functions
     return status;
}
#endif
#ifdef  CONFIG_OP_DSP_CHW_3X3_CONV2D_RELU_AVGPOOL_Q7
ExecStatus Exec_conv2d_q7_CHW_ker3x3_stride1_pad0_relu_avgPool_2x2_stride2_pad0( uint8_t execHwId, ExecOperand_t * inArray, ExecOperand_t * wt,ExecOperand_t * bias,ExecOperand_t * outArray, ExecOperand_t * buffIn, const conv2d_relu_avgpool_opt *opt)
{
     ExecStatus status = EXEC_STATUS_OK;
    exec_conv2d_relu_avgpool_q7_t  * params;
    ExecWork_t work;
    uint8_t index;
    // Get the param allocated and fill all the details
     params = ( exec_conv2d_relu_avgpool_q7_t *) pvPortMalloc(sizeof (exec_conv2d_relu_avgpool_q7_t));
     params->wt =  (  const q7_t *) ExecGetBufAddr(wt);
     params->bias =  (  const q7_t *) ExecGetBufAddr(bias);
     memcpy( &(params->opt),opt,sizeof(conv2d_relu_avgpool_opt));
     work. opID = EXEC_OP_DSP_CHW_3X3_CONV2D_RELU_AVGPOOL_Q7;
     work.params = (void *) params;
     work.execHwId = execHwId;
     work.numInputs = 2;
     work.inbufs[0] = inArray;
     work.inbufs[1] = buffIn;
     work.outBuf = outArray;
     index = CheckAndScheduleWork(&work,0);
     if (EXEC_HW_ID_DSP == execHwId)
    {
        SubmitDSPWork(index);
    }
     //TBD:  Handle M3 later. Depepnds if they ensure same prototype. Assuming in symmetry in future kernel  functions
     return status;

}
#endif
#ifdef CONFIG_OP_M3_DS_CONV2D_LAYER2_Q7
ExecStatus Exec_ds_conv2d_layer2_q7 ( uint8_t execHwId, ExecOperand_t * inArray, ExecOperand_t * wt, ExecOperand_t * bias, ExecOperand_t * outArray, ExecOperand_t * buffIn, const conv2d_opt * opt)
{
    ExecStatus status = EXEC_STATUS_OK;
    exec_conv2d_q7_t * params;
    ExecWork_t work;
    uint8_t index;
       // Get the param allocated and fill all the details
     params = ( exec_conv2d_q7_t *) pvPortMalloc(sizeof (exec_conv2d_q7_t));
     params->wt =  (  const q7_t *) ExecGetBufAddr(wt);
     params->bias =  (  const q7_t *) ExecGetBufAddr(bias);
     memcpy( &(params->opt),opt,sizeof(conv2d_opt));
     work.params = (void *) params;
     work.execHwId = execHwId;
     work.numInputs = 2;
     work.inbufs[0] = inArray;
     work.inbufs[1] = buffIn;
     work.outBuf = outArray;
      if (EXEC_HW_ID_M3 == execHwId){
      work. opID = EXEC_OP_M3_DS_CONV2D_LAYER2_Q7;
     index = CheckAndScheduleWork(&work,0);
      SubmitM3Work(index);
    }
    if (EXEC_HW_ID_DSP == execHwId){
    }
    return status;
}
#endif
#ifdef CONFIG_OP_M3_DS_CONV2D_SB_LAYER3_Q7
ExecStatus Exec_ds_conv2d_sb_layer3_q7 (uint8_t execHwId, ExecOperand_t * inOutArray,
                                        ExecOperand_t * wt, ExecOperand_t * bias,
                                        ExecOperand_t * buffIn, const conv2d_opt * opt)
{
    ExecStatus status = EXEC_STATUS_OK;
    exec_conv2d_q7_t * params;
    ExecWork_t work;
    uint8_t index;
       // Get the param allocated and fill all the details
     params = ( exec_conv2d_q7_t *) pvPortMalloc(sizeof (exec_conv2d_q7_t));
     params->wt =  (  const q7_t *) ExecGetBufAddr(wt);
     params->bias =  (  const q7_t *) ExecGetBufAddr(bias);
     memcpy( &(params->opt),opt,sizeof(conv2d_opt));
     work.params = (void *) params;
     work.execHwId = execHwId;
     work.numInputs = 2;
     work.inbufs[0] = inOutArray;
     work.inbufs[1] = buffIn;
     /* NO Out buf, Output saved in inOutArray
      * inOutArray is BUF_TYPE_INOUT*/
     work.outBuf = inOutArray;
      if (EXEC_HW_ID_M3 == execHwId){
      work. opID = EXEC_OP_M3_DS_CONV2D_SB_LAYER3_Q7;
     index = CheckAndScheduleWork(&work,1);
      SubmitM3Work(index);
    }
    if (EXEC_HW_ID_DSP == execHwId){

         }
    return status;
}
#endif
#ifdef CONFIG_OP_M3_SIGMOID_Q7
 // proxy funnctions
ExecStatus Exec_sigmoid_q7 ( uint8_t execHwId, ExecOperand_t * inArray, int size, int width)
{
    ExecStatus status = EXEC_STATUS_OK;
    exec_sigmoid_q7_t* params;
    ExecWork_t work;
    uint8_t index;

       // Get the param allocated and fill all the details
     params = (exec_sigmoid_q7_t *) pvPortMalloc(sizeof(exec_sigmoid_q7_t));
     params->size =  size;
     params->width =  width;
     work.params = (void *) params;
     work.execHwId = execHwId;
     work.numInputs = 1;
     work.inbufs[0] = inArray;
     work.outBuf = inArray;
      if (EXEC_HW_ID_M3 == execHwId){
      work. opID = EXEC_OP_M3_SIGMOID_Q7;
     index = CheckAndScheduleWork(&work,1);
      SubmitM3Work(index);
    }
    if (EXEC_HW_ID_DSP == execHwId){
    }
    return status;
}
#endif
#ifdef CONFIG_OP_M3_CONCAT_HEIGHT_Q7
 // proxy funnctions
ExecStatus Exec_concat_height_q7 ( uint8_t execHwId,
                            ExecOperand_t * inArray1, ExecOperand_t * inArray2,
                            ExecOperand_t *outArray, concat_opt *opt)
{
    ExecStatus status = EXEC_STATUS_OK;
    exec_concat_q7_t* params;
    ExecWork_t work;
    uint8_t index;

       // Get the param allocated and fill all the details
     params = (exec_concat_q7_t *) pvPortMalloc(sizeof(exec_concat_q7_t));
     memcpy( &(params->opt),opt,sizeof(concat_opt));
     work.params = (void *) params;
     work.execHwId = execHwId;
     work.numInputs = 2;
     work.inbufs[0] = inArray1;
     work.inbufs[1] = inArray2;
     work.outBuf = outArray;
     if (EXEC_HW_ID_M3 == execHwId){
         work. opID = EXEC_OP_M3_CONCAT_HEIGHT_Q7;
         index = CheckAndScheduleWork(&work,0);
         SubmitM3Work(index);
     }
    return status;
}
#endif
#ifdef CONFIG_OP_M3_PW_DS_CONV2D_LAYER4AND5_Q7
 // proxy funnctions
ExecStatus Exec_pw_ds_conv2d_layer4and5_q7 (uint8_t execHwId, ExecOperand_t * inArray,
                ExecOperand_t * wt_pw, ExecOperand_t * wt_ds,
                ExecOperand_t * bias_pw, ExecOperand_t * bias_ds,
                ExecOperand_t * outArray,
                ExecOperand_t * buffpsIn,
                ExecOperand_t * buffdsIn,
                ExecOperand_t * buffpw2ds,
                ExecOperand_t * buffoutArray,
                const conv2d_opt * opt_pw,
                const conv2d_opt * opt_ds)
{
    ExecStatus status = EXEC_STATUS_OK;
    exec_pw_ds_conv2d_q7_t* params;
    ExecWork_t work;
    uint8_t index;
       // Get the param allocated and fill all the details
     params = (exec_pw_ds_conv2d_q7_t *) pvPortMalloc(sizeof(exec_pw_ds_conv2d_q7_t));
     params->wt_pw =  (const q7_t *) ExecGetBufAddr(wt_pw);
     params->wt_ds =  (const q7_t *) ExecGetBufAddr(wt_ds);
     params->bias_pw =  (const q7_t *) ExecGetBufAddr(bias_pw);
     params->bias_ds =  (const q7_t *) ExecGetBufAddr(bias_ds);
     memcpy( &(params->opt_pw),opt_pw,sizeof(conv2d_opt));
     memcpy( &(params->opt_ds),opt_ds,sizeof(conv2d_opt));
     work.params = (void *) params;
     work.execHwId = execHwId;
     work.numInputs = 5;
     work.inbufs[0] = inArray;
     work.inbufs[1] = buffpsIn;
     work.inbufs[2] = buffdsIn;
     work.inbufs[3] = buffpw2ds;
     work.inbufs[4] = buffoutArray;
     work.outBuf = outArray;
      if (EXEC_HW_ID_M3 == execHwId){
      work. opID = EXEC_OP_M3_PW_DS_CONV2D_LAYER4AND5_Q7;
     index = CheckAndScheduleWork(&work,0);
      SubmitM3Work(index);
    }
    if (EXEC_HW_ID_DSP == execHwId){
    }
    return status;
}
#endif
#ifdef CONFIG_OP_M3_MAXPOOL_Q7
ExecStatus Exec_maxpool_q7 (uint8_t execHwId, ExecOperand_t * inArray,
                ExecOperand_t * outArray,
                const pool2d_opt * opt)
{

    exec_maxpool2d_q7_t * params;
    ExecStatus status = EXEC_STATUS_OK;
    ExecWork_t work;
    uint8_t index;
    params = (exec_maxpool2d_q7_t *) pvPortMalloc(sizeof (exec_maxpool2d_q7_t));
    memcpy( &(params->opt), opt, sizeof(pool2d_opt));
    work.params = (void *) params;
    work.execHwId = execHwId;
    work. opID = EXEC_OP_M3_MAXPOOL_Q7;
    work.numInputs = 1;
    work.inbufs[0] = inArray;
    work.outBuf = outArray;
    index = CheckAndScheduleWork(&work,0);
    if (EXEC_HW_ID_M3 == execHwId)
    {
        SubmitM3Work(index);
    }
    return status;
}
#endif
#ifdef CONFIG_OP_M3_FULLY_CONNECTED_Q7
ExecStatus Exec_fully_connected_q7(uint8_t execHwId, ExecOperand_t * inArray,
        ExecOperand_t * wt, ExecOperand_t * bias,
        ExecOperand_t  * outArray, fc_opt *opt)
{
    exec_fc_q7_t *params;
    ExecStatus status = EXEC_STATUS_OK;
    ExecWork_t work;
    uint8_t index;

    params = (exec_fc_q7_t *) pvPortMalloc(sizeof(exec_fc_q7_t));
    params->wt =  (const q7_t *) ExecGetBufAddr(wt);
    params->bias =  (const q7_t *) ExecGetBufAddr(bias);
    memcpy( &(params->opt), opt, sizeof(fc_opt));
    work.params = (void *) params;
    work.execHwId = execHwId;
    work. opID = EXEC_OP_M3_FULLY_CONNECTED_Q7;
    work.numInputs = 1;
    work.inbufs[0] = inArray;
    work.outBuf = outArray;
    index = CheckAndScheduleWork(&work,0);
    if (EXEC_HW_ID_M3 == execHwId)
    {
        SubmitM3Work(index);
    }
    return status;
}
#endif
#ifdef CONFIG_OP_M3_ADD_Q7
ExecStatus Exec_add_q7(uint8_t execHwId, ExecOperand_t * inArray0,
        ExecOperand_t * inArray1, ExecOperand_t  * outArray, add_opt *opt)
{
    exec_add_q7_t *params;
    ExecStatus status = EXEC_STATUS_OK;
    ExecWork_t work;
    uint8_t index;

    params = (exec_add_q7_t *) pvPortMalloc(sizeof(exec_add_q7_t));
    memcpy( &(params->opt), opt, sizeof(add_opt));
    work.params = (void *) params;
    work.execHwId = execHwId;
    work. opID = EXEC_OP_M3_ADD_Q7;
    work.numInputs = 2;
    work.inbufs[0] = inArray0;
    work.inbufs[1] = inArray1;
    work.outBuf = outArray;
    index = CheckAndScheduleWork(&work,0);
    if (EXEC_HW_ID_M3 == execHwId)
    {
        SubmitM3Work(index);
    }
    return status;
}
#endif

#ifdef CONFIG_OP_DSP_FFT_Q15
ExecStatus Exec_fft_q15(uint8_t execHwId, ExecOperand_t * inArray0, ExecOperand_t  * outArray, uint16_t fft_length)
{
    tDsp_math_fft_opt *params;
    ExecStatus status = EXEC_STATUS_OK;
    ExecWork_t work;
    uint8_t index;

    params = (tDsp_math_fft_opt *) pvPortMalloc(sizeof(tDsp_math_fft_opt));
    params->fft_length =  fft_length;
    work.params = (void *) params;
    work.execHwId = execHwId;
    work. opID = EXEC_OP_DSP_MATH_FFT;
    work.numInputs = 1;
    work.inbufs[0] = inArray0;
    work.outBuf = outArray;
    index = CheckAndScheduleWork(&work,0);
    if (EXEC_HW_ID_DSP == execHwId)
    {
        SubmitDSPWork(index);
    }
    return status;
}
#endif

#if defined(CONFIG_OP_DSP_SQRT_IN_Q15_OUT_Q15) || defined(CONFIG_OP_DSP_EXP_IN_Q15_OUT_Q3_13) || defined(CONFIG_OP_DSP_LOG_IN_Q15_OUT_Q5_11)
ExecStatus Exec_math_func_sqrt_exp_log_q15(uint8_t mathFunId, uint8_t execHwId, ExecOperand_t * inArray0, ExecOperand_t  * outArray, uint16_t len)
{
    tDsp_math_func_opt *params;
    ExecStatus status = EXEC_STATUS_OK;
    ExecWork_t work;
    uint8_t index;

    params = (tDsp_math_func_opt *) pvPortMalloc(sizeof(tDsp_math_func_opt));
    params->len =  len;
    work.params = (void *) params;
    work.execHwId = execHwId;
    work. opID = mathFunId;
    work.numInputs = 1;
    work.inbufs[0] = inArray0;
    work.outBuf = outArray;
    index = CheckAndScheduleWork(&work,0);
    if (EXEC_HW_ID_DSP == execHwId)
    {
        SubmitDSPWork(index);
    }
    return status;
}
#endif

#ifdef CONFIG_OP_DSP_SQRT_IN_Q15_OUT_Q15
ExecStatus Exec_sqrt_q15(uint8_t execHwId, ExecOperand_t * inArray0, ExecOperand_t  * outArray, uint16_t len)
{
    return Exec_math_func_sqrt_exp_log_q15(EXEC_OP_DSP_MATH_SQRT,execHwId,inArray0,outArray,len);
}
#endif

#ifdef CONFIG_OP_DSP_EXP_IN_Q15_OUT_Q3_13
ExecStatus Exec_exp_q15(uint8_t execHwId, ExecOperand_t * inArray0, ExecOperand_t  * outArray, uint16_t len)
{
    return Exec_math_func_sqrt_exp_log_q15(EXEC_OP_DSP_MATH_EXP,execHwId,inArray0,outArray,len);
}
#endif

#ifdef CONFIG_OP_DSP_LOG_IN_Q15_OUT_Q5_11
ExecStatus Exec_log_q15(uint8_t execHwId, ExecOperand_t * inArray0, ExecOperand_t  * outArray, uint16_t len)
{
    return Exec_math_func_sqrt_exp_log_q15(EXEC_OP_DSP_MATH_LOG,execHwId,inArray0,outArray,len);
}
#endif