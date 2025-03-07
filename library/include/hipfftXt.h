/******************************************************************************
 * Copyright (C) 2021 - 2022 Advanced Micro Devices, Inc. All rights
 * reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************/

#ifndef HIPFFTXT_H_
#define HIPFFTXT_H_
#include "hipfft.h"

#ifdef __HIP_PLATFORM_NVIDIA__
#include <cufftXt.h>
#endif

#ifndef _WIN32
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum hipfftXtCallbackType_t
{
    HIPFFT_CB_LD_COMPLEX        = 0x0,
    HIPFFT_CB_LD_COMPLEX_DOUBLE = 0x1,
    HIPFFT_CB_LD_REAL           = 0x2,
    HIPFFT_CB_LD_REAL_DOUBLE    = 0x3,
    HIPFFT_CB_ST_COMPLEX        = 0x4,
    HIPFFT_CB_ST_COMPLEX_DOUBLE = 0x5,
    HIPFFT_CB_ST_REAL           = 0x6,
    HIPFFT_CB_ST_REAL_DOUBLE    = 0x7,
    HIPFFT_CB_UNDEFINED         = 0x8

} hipfftXtCallbackType;

typedef hipfftComplex (*hipfftCallbackLoadC)(void*  dataIn,
                                             size_t offset,
                                             void*  callerInfo,
                                             void*  sharedPointer);
typedef hipfftDoubleComplex (*hipfftCallbackLoadZ)(void*  dataIn,
                                                   size_t offset,
                                                   void*  callerInfo,
                                                   void*  sharedPointer);
typedef hipfftReal (*hipfftCallbackLoadR)(void*  dataIn,
                                          size_t offset,
                                          void*  callerInfo,
                                          void*  sharedPointer);
typedef hipfftDoubleReal (*hipfftCallbackLoadD)(void*  dataIn,
                                                size_t offset,
                                                void*  callerInfo,
                                                void*  sharedPointer);

typedef void (*hipfftCallbackStoreC)(
    void* dataOut, size_t offset, hipfftComplex element, void* callerInfo, void* sharedPointer);
typedef void (*hipfftCallbackStoreZ)(void*               dataOut,
                                     size_t              offset,
                                     hipfftDoubleComplex element,
                                     void*               callerInfo,
                                     void*               sharedPointer);
typedef void (*hipfftCallbackStoreR)(
    void* dataOut, size_t offset, hipfftReal element, void* callerInfo, void* sharedPointer);
typedef void (*hipfftCallbackStoreD)(
    void* dataOut, size_t offset, hipfftDoubleReal element, void* callerInfo, void* sharedPointer);

/*! @brief Set a callback on a plan
   *
   * @details Set either a load or store callback to run with a plan.
   * The type of callback is specified with the 'cbtype' parameter.
   * An array ofcallback and callback data pointers must be given -
   * one per device executing the plan.
   *
   * @param[in] plan The FFT plan.
   * @param[in] callbacks Array of callback function pointers.
   * @param[in] cbtype Type of callback being set.
   * @param[in] callbackData Array of callback function data pointers
   */
HIPFFT_EXPORT hipfftResult hipfftXtSetCallback(hipfftHandle         plan,
                                               void**               callbacks,
                                               hipfftXtCallbackType cbtype,
                                               void**               callbackData);

/*! @brief Remove a callback from a plan
   *
   * @details Remove a previously-set callback from a plan.
   *
   * @param[in] plan The FFT plan.
   * @param[in] cbtype Type of callback being removed.
   */
HIPFFT_EXPORT hipfftResult hipfftXtClearCallback(hipfftHandle plan, hipfftXtCallbackType cbtype);

/*! @brief Set shared memory size for callback.
   *
   * @details Set shared memory required for a callback.  The
   * callback of the specified type must have already been set on the
   * plan.
   *
   * @param[in] plan The FFT plan.
   * @param[in] cbtype Type of callback being modified.
   * @param[in] sharedSize Amount of shared memory required, in bytes.
   */
HIPFFT_EXPORT hipfftResult hipfftXtSetCallbackSharedSize(hipfftHandle         plan,
                                                         hipfftXtCallbackType cbtype,
                                                         size_t               sharedSize);

/*! @brief Initialize a batched rank-dimensional FFT plan with
    advanced data layout and specified input, output, execution data
    types.
   *
   *  @details Assumes that the plan has been created already, and
   *  modifies the plan associated with the plan handle. The number 
   *  of elements to transform in each direction of the input data 
   *  in the FFT plan is specified in n.
   * 
   *  The batch parameter tells hipFFT how many transforms to perform. 
   *  The distance between the first elements of two consecutive batches 
   *  of the input and output data are specified with the idist and odist 
   *  parameters.
   * 
   *  The inembed and onembed parameters define the input and output data
   *  layouts. The number of elements in the data is assumed to be larger 
   *  than the number of elements in the transform. Strided data layouts 
   *  are also supported. Strides along the fastest direction in the input
   *  and output data are specified via the istride and ostride parameters.  
   * 
   *  If both inembed and onembed parameters are set to NULL, all the 
   *  advanced data layout parameters are ignored and reverted to default 
   *  values, i.e., the batched transform is performed with non-strided data
   *  access and the number of data/transform elements are assumed to be  
   *  equivalent.
   *
   *  The inputType, outputType, executionType parameters specify the
   *  data types (precision, and whether the data is real-valued or
   *  complex-valued) of the transform input, output, and internal
   *  representation respectively.  Currently, the precision of all
   *  three parameters must match, and the execution type must always
   *  be complex-valued.  At least one of inputType and outputType
   *  must be complex.  A half-precision transform can be requested
   *  by using either the HIP_R_16F or HIP_C_16F types.
   *
   *  @param[out] plan Pointer to the FFT plan handle.
   *  @param[in] rank Dimension of transform (1, 2, or 3).
   *  @param[in] n Number of elements to transform in the x/y/z directions.
   *  @param[in] inembed Number of elements in the input data in the x/y/z directions.
   *  @param[in] istride Distance between two successive elements in the input data.
   *  @param[in] idist Distance between input batches.
   *  @param[in] inputType Format of FFT input.
   *  @param[in] onembed Number of elements in the output data in the x/y/z directions.
   *  @param[in] ostride Distance between two successive elements in the output data.
   *  @param[in] odist Distance between output batches.
   *  @param[in] outputType Format of FFT output.
   *  @param[in] batch Number of batched transforms to perform.
   *  @param[out] workSize Pointer to work area size (returned value).
   *  @param[in] executionType Internal data format used by the library during computation.
   */
HIPFFT_EXPORT hipfftResult hipfftXtMakePlanMany(hipfftHandle   plan,
                                                int            rank,
                                                long long int* n,
                                                long long int* inembed,
                                                long long int  istride,
                                                long long int  idist,
                                                hipDataType    inputType,
                                                long long int* onembed,
                                                long long int  ostride,
                                                long long int  odist,
                                                hipDataType    outputType,
                                                long long int  batch,
                                                size_t*        workSize,
                                                hipDataType    executionType);

/*! @brief Return size of the work area size required for a
    rank-dimensional plan, with specified input, output, execution
    data types.

 * @details See ::hipfftXtMakePlanMany for restrictions on inputType,
 * outputType, executionType parameters.
 *
 *  @param[in] plan Pointer to the FFT plan.
 *  @param[in] rank Dimension of FFT transform (1, 2, or 3).
 *  @param[in] n Number of elements in the x/y/z directions.
 *  @param[in] inembed Number of elements in the input data in the x/y/z directions.
 *  @param[in] istride Distance between two successive elements in the input data.
 *  @param[in] idist Distance between input batches.
 *  @param[in] inputType Format of FFT input.
 *  @param[in] onembed Number of elements in the output data in the x/y/z directions.
 *  @param[in] ostride Distance between two successive elements in the output data.
 *  @param[in] odist Distance between output batches.
 *  @param[in] outputType Format of FFT output.
 *  @param[in] batch Number of batched transforms to perform.
 *  @param[out] workSize Pointer to work area size (returned value).
 *  @param[in] executionType Internal data format used by the library during computation.
 *  */
HIPFFT_EXPORT hipfftResult hipfftXtGetSizeMany(hipfftHandle   plan,
                                               int            rank,
                                               long long int* n,
                                               long long int* inembed,
                                               long long int  istride,
                                               long long int  idist,
                                               hipDataType    inputType,
                                               long long int* onembed,
                                               long long int  ostride,
                                               long long int  odist,
                                               hipDataType    outputType,
                                               long long int  batch,
                                               size_t*        workSize,
                                               hipDataType    executionType);

/*! @brief Execute an FFT plan for any precision and type.

 * @details An in-place transform is performed if the input and
 * output pointers have the same value.
 *
 * The direction parameter is ignored if for real-to-complex and
 * complex-to-real transforms, as the direction is already implied by
 * the data types.
 *
 *  @param[in] plan Pointer to the FFT plan.
 *  @param[in] input Pointer to input data for the transform.
 *  @param[in] output Pointer to output data for the transform.
 *  @param[in] direction Either `HIPFFT_FORWARD` or `HIPFFT_BACKWARD`.
 *  */
HIPFFT_EXPORT hipfftResult hipfftXtExec(hipfftHandle plan,
                                        void*        input,
                                        void*        output,
                                        int          direction);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HIPFFT_H_
