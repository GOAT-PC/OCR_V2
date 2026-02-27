#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define S_FUNCTION_NAME OCR_RSM
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"


static const double cs_0[1] = { /* K0_0 */
     -2167.09631591974
};

static const double cs_1[1] = { /* K0_1 */
     27.5727032612092
};

static const double cs_2[1] = { /* K0_2 */
     -0.08756382937157
};

static const double cs_3[1] = { /* K0_3 */
     6.55864558769723
};

static const double cs_4[1] = { /* K0_4 */
     -0.00495225583753974
};

static const double cs_5[1] = { /* K0_5 */
     103.414266250187
};

static const double cs_6[1] = { /* K0_6 */
     -2.68181548635217
};

static const double cs_7[1] = { /* K0_7 */
     -0.0416575018376616
};

static const double cs_8[1] = { /* K0_8 */
     -0.573157331962168
};

static const double cs_9[1] = { /* K0_9 */
     -0.160772850229475
};


typedef struct
{
   double* _v0;   /* local_0 */
   double _v1[1];   /* local_1 */
   double _v2[1];   /* local_2 */
   double _v3[1];   /* local_3 */
   double _v4[1];   /* local_4 */
   double _v5[1];   /* local_5 */
   double _v6[1];   /* local_6 */
   double _v7[1];   /* local_7 */
   double _v8[1];   /* local_8 */
   double _v9[1];   /* local_9 */
   double _v10[1];   /* local_10 */
   double _v11[1];   /* local_11 */
   double _v12[1];   /* local_12 */
   double _v13[1];   /* local_13 */
   double _v14[1];   /* local_14 */
   double _v15[1];   /* local_15 */
   double _v16[1];   /* local_16 */
} S_OCR_RSM;
static S_OCR_RSM _OCR_RSM;


static void _model_computation(double *U, double *Xin, double *Y, double *Xout)
{
   _OCR_RSM._v0 = &U[0];

   /* Xsplitted node */
   /* local_1, local_2, local_3 = Split(local_0) */
   {
      _OCR_RSM._v1[0] = _OCR_RSM._v0[0];
      _OCR_RSM._v2[0] = _OCR_RSM._v0[1];
      _OCR_RSM._v3[0] = _OCR_RSM._v0[2];
   }
   /* X[2]^2 node */
   /* local_4 = Power(local_1) */
   {
         _OCR_RSM._v4[0] = pow(_OCR_RSM._v1[0], 2);
   }
   /* X[3]^2 node */
   /* local_5 = Power(local_2) */
   {
         _OCR_RSM._v5[0] = pow(_OCR_RSM._v2[0], 2);
   }
   /* X[4]^2 node */
   /* local_6 = Power(local_3) */
   {
         _OCR_RSM._v6[0] = pow(_OCR_RSM._v3[0], 2);
   }
   /* Y0_1 node */
   /* local_7 = mMul(local_1, K0_1) */
   {
      _OCR_RSM._v7[0] = _OCR_RSM._v1[0];
      _OCR_RSM._v7[0] *= cs_1[0];
   }
   /* Y0_3 node */
   /* local_9 = mMul(local_2, K0_3) */
   {
      _OCR_RSM._v9[0] = _OCR_RSM._v2[0];
      _OCR_RSM._v9[0] *= cs_3[0];
   }
   /* Y0_5 node */
   /* local_11 = mMul(local_3, K0_5) */
   {
      _OCR_RSM._v11[0] = _OCR_RSM._v3[0];
      _OCR_RSM._v11[0] *= cs_5[0];
   }
   /* Y0_7 node */
   /* local_13 = mMul(local_1, local_2, K0_7) */
   {
      _OCR_RSM._v13[0] = _OCR_RSM._v1[0];
      _OCR_RSM._v13[0] *= _OCR_RSM._v2[0];
      _OCR_RSM._v13[0] *= cs_7[0];
   }
   /* Y0_8 node */
   /* local_14 = mMul(local_1, local_3, K0_8) */
   {
      _OCR_RSM._v14[0] = _OCR_RSM._v1[0];
      _OCR_RSM._v14[0] *= _OCR_RSM._v3[0];
      _OCR_RSM._v14[0] *= cs_8[0];
   }
   /* Y0_9 node */
   /* local_15 = mMul(local_2, local_3, K0_9) */
   {
      _OCR_RSM._v15[0] = _OCR_RSM._v2[0];
      _OCR_RSM._v15[0] *= _OCR_RSM._v3[0];
      _OCR_RSM._v15[0] *= cs_9[0];
   }
   /* Y0_2 node */
   /* local_8 = mMul(local_4, K0_2) */
   {
      _OCR_RSM._v8[0] = _OCR_RSM._v4[0];
      _OCR_RSM._v8[0] *= cs_2[0];
   }
   /* Y0_4 node */
   /* local_10 = mMul(local_5, K0_4) */
   {
      _OCR_RSM._v10[0] = _OCR_RSM._v5[0];
      _OCR_RSM._v10[0] *= cs_4[0];
   }
   /* Y0_6 node */
   /* local_12 = mMul(local_6, K0_6) */
   {
      _OCR_RSM._v12[0] = _OCR_RSM._v6[0];
      _OCR_RSM._v12[0] *= cs_6[0];
   }
   /* S(Y0) node */
   /* local_16 = mAdd(K0_0, local_7, local_8, local_9, local_10, local_11, local_12, local_13, local_14, local_15) */
   {
      _OCR_RSM._v16[0] = cs_0[0];
      _OCR_RSM._v16[0] += _OCR_RSM._v7[0];
      _OCR_RSM._v16[0] += _OCR_RSM._v8[0];
      _OCR_RSM._v16[0] += _OCR_RSM._v9[0];
      _OCR_RSM._v16[0] += _OCR_RSM._v10[0];
      _OCR_RSM._v16[0] += _OCR_RSM._v11[0];
      _OCR_RSM._v16[0] += _OCR_RSM._v12[0];
      _OCR_RSM._v16[0] += _OCR_RSM._v13[0];
      _OCR_RSM._v16[0] += _OCR_RSM._v14[0];
      _OCR_RSM._v16[0] += _OCR_RSM._v15[0];
   }

   Y[0] = *_OCR_RSM._v16;

}


static void mdlInitializeSizes(SimStruct *S)
{
   /* Declare parameters*/
   ssSetNumSFcnParams(S, 0);
   if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) return;

   /* Declare states*/
   ssSetNumContStates(S, 0);
   ssSetNumDiscStates(S, 0);

   /* Declare inputs */
   if(!ssSetNumInputPorts(S, 1)) return;

   ssSetInputPortWidth(S, 0, 3);
   ssSetInputPortDataType(S, 0, SS_DOUBLE);
   ssSetInputPortRequiredContiguous(S, 0, 1);
   ssSetInputPortDirectFeedThrough(S, 0, 1);

   /* Declare outputs */
   if(!ssSetNumOutputPorts(S, 1)) return;

   ssSetOutputPortWidth(S, 0, 1);
   ssSetOutputPortDataType(S, 0, SS_DOUBLE);

   ssSetNumSampleTimes(S, 1);
   ssSetNumNonsampledZCs(S, 0);
}

#define MDL_INITIALIZE_SAMPLE_TIMES
void mdlInitializeSampleTimes(SimStruct *S)
{
   ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
   ssSetOffsetTime(S, 0, 0.0);
}

#define MDL_START
void mdlStart(SimStruct *S)
{
}



#define MDL_OUTPUTS
void mdlOutputs(SimStruct *S, int_T tid)
{
   double U[3];
   double Y[1];
   double *Xin = NULL;
   double *Xout = NULL;

   UNUSED_ARG(tid); /* not used in single tasking mode */
   {
      const double *uptrs = ssGetInputPortRealSignal(S, 0);
      memcpy(&U[0], uptrs,3*sizeof(double));
   }

   _model_computation(U, Xin, Y, Xout);

   {
      double *yptrs = ssGetOutputPortRealSignal(S, 0);
      yptrs[0] = Y[0];
   }
}

void mdlTerminate(SimStruct *S)
{
   UNUSED_ARG(S); /* unused input argument */
}





#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
