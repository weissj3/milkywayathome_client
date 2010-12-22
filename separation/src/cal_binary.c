/*
Copyright (C) 2010  Matthew Arsenault

This file is part of Milkway@Home.

Milkyway@Home is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Milkyway@Home is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Milkyway@Home.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cal.h>
#include <calcl.h>

#include "milkyway_util.h"
#include "separation_types.h"
#include "calculated_constants.h"
#include "r_points.h"
#include "show_cal_types.h"
#include "cal_binary.h"
#include "separation_cal_kernelgen.h"


/* FIXME: Also defined in CL version */
typedef struct
{
    size_t outMu;
    size_t outStreams;

    size_t nuSteps, muSteps, rSteps;

    size_t rPts;
    size_t rc;
    size_t sg_dx;
    size_t lTrig;
    size_t bTrig;
} CALSeparationSizes;


typedef struct
{
    CALname outMu;
    CALname* outStreams;

    CALname inMu;
    CALname* inStreams;

    CALname nu_id;
    CALname nu_step;
    CALname rPts;
    CALname rc;
    CALname sg_dx;
    CALname lTrig;
    CALname bTrig;
} SeparationCALNames;


typedef struct
{
    CALuint major, minor, patchLevel;
} MWCALVersion;

#define EMPTY_CAL_VERSION { 0, 0, 0 }

typedef struct
{
    MWCALVersion version;
    CALuint numDevices;
    CALdevice devID;    /* Index of device chosen */
    CALdevice dev;
    CALdeviceinfo devInfo;
    CALdeviceattribs devAttribs;
    CALcontext calctx;
    CALmodule module;
    CALimage image;
    CALfunc func;
} MWCALInfo;
#define EMPTY_CAL_INFO { EMPTY_CAL_VERSION, 0, 0, 0, { 0, }, { 0 }, 0, 0, 0, 0, NULL, 0 }

/* Pair of resource and associated CALmem */
typedef struct
{
    CALresource res;
    CALmem mem;
} MWMemRes;

#define EMPTY_MEM_RES { 0, 0 }

typedef struct
{
    MWMemRes outMu;
    MWMemRes* outStreams;

    /* constant, read only buffers */
    MWMemRes rc;        /* r constants */
    MWMemRes rPts;
    MWMemRes sg_dx;
    MWMemRes lTrig;      /* sin, cos of l */
    MWMemRes bTrig;      /* sin, cos of b */
    MWMemRes nu_id;
    MWMemRes nu_step;
    CALuint numberStreams;
} SeparationCALMem;

#define cal_warn(msg, err, ...) fprintf(stderr, msg ": %s (%s)\n", ##__VA_ARGS__, calGetErrorString(), showCALresult(err))


static CALresult releaseMWMemRes(CALcontext ctx, MWMemRes* mr)
{
    CALresult err = CAL_RESULT_OK;

    if (mr->mem)
    {
        err = calCtxReleaseMem(ctx, mr->mem);
        if (err != CAL_RESULT_OK)
            cal_warn("Failed to release CALmem", err);
        mr->mem = 0;
    }

    if (mr->res)
    {
        err = calResFree(mr->res);
        if (err != CAL_RESULT_OK)
            cal_warn("Failed to release CAL resource", err);
        mr->res = 0;
    }

    return err;
}

static CALresult mwDestroyCALInfo(MWCALInfo* ci)
{
    CALresult err = CAL_RESULT_OK;
    CALresult erri;

    if (ci->module)
    {
        erri = calModuleUnload(ci->calctx, ci->module);
        if (erri != CAL_RESULT_OK)
            cal_warn("Failed to unload module", erri);
        ci->module = 0;
        err |= erri;
    }

    if (ci->calctx)
    {
        erri = calCtxDestroy(ci->calctx);
        if (erri != CAL_RESULT_OK)
            cal_warn("Failed to destroy CAL context", erri);
        ci->calctx = 0;
        err |= erri;
    }

    if (ci->dev)
    {
        erri = calDeviceClose(ci->dev);
        if (erri != CAL_RESULT_OK)
            cal_warn("Failed to close device", erri);
        ci->dev = 0;
        err |= erri;
    }

    if (err != CAL_RESULT_OK)
        cal_warn("Failed to cleanup CAL info", err);

    return err;
}

static CALresult mwGetDevice(MWCALInfo* ci, CALuint devID)
{
    CALresult err;

    err = calDeviceGetCount(&ci->numDevices);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to get CAL device count", err);
        return err;
    }

    if (ci->numDevices == 0)
    {
        warn("Didn't find any CAL devices\n");
        return -1;
    }

    if (devID > ci->numDevices)
    {
        warn("Requested device ID %u > found number of devices (%u)\n",
             devID,
             ci->numDevices);
        return -1;
    }

    ci->devID = devID;
    err = calDeviceOpen(&ci->dev, ci->devID);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to open CAL device", err);
        return err;
    }

    err = calDeviceGetInfo(&ci->devInfo, ci->devID);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to get CAL device information", err);
        return err;
    }

    ci->devAttribs.struct_size = sizeof(struct CALdeviceattribsRec);
    err = calDeviceGetAttribs(&ci->devAttribs, ci->devID);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to get CAL device attributes", err);
        return err;
    }

    return CAL_RESULT_OK;
}

/* Find devices and create context */
static CALresult mwGetCALInfo(MWCALInfo* ci, CALuint devID)
{
    CALresult err;

    err = calGetVersion(&ci->version.major, &ci->version.minor, &ci->version.patchLevel);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to get CAL version", err);
        return err;
    }

    err = mwGetDevice(ci, devID);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Error getting device information", err);
        return err;
    }

    err = calCtxCreate(&ci->calctx, ci->dev);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to create CAL context", err);
        return err;
    }

    return CAL_RESULT_OK;
}

#if DOUBLEPREC
  #define formatReal1 CAL_FORMAT_DOUBLE_1
  #define formatReal2 CAL_FORMAT_DOUBLE_2
#else
  #define formatReal1 CAL_FORMAT_FLOAT_1
  #define formatReal2 CAL_FORMAT_FLOAT_2
#endif /* DOUBLEPREC */

/* Try to get memory handle and cleanup resource if that fails */
static CALresult getMemoryHandle(MWMemRes* mr, MWCALInfo* ci)
{
    CALresult err = CAL_RESULT_OK;

    err = calCtxGetMem(&mr->mem, ci->calctx, mr->res);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to get memory handle", err);
        if (calResFree(mr->res) != CAL_RESULT_OK)
            warn("Failed to release CAL resource\n");
        else
            mr->res = 0;
    }

    return err;
}

/* Try to map the resource and free it on failure */
static CALresult mapMWMemRes(MWMemRes* mr, CALvoid** pPtr, CALuint* pitch)
{
    CALresult err;

    err = calResMap(pPtr, pitch, mr->res, 0);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to map resource", err);
        if (calResFree(mr->res) != CAL_RESULT_OK)
            warn("Failed to release CAL resource\n");
        else
            mr->res = 0;
    }

    return err;
}

/* Try to unmap resource and free it on failure */
static CALresult unmapMWMemRes(MWMemRes* mr)
{
    CALresult err;

    err = calResUnmap(mr->res);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to unmap resource", err);
        if (calResFree(mr->res) != CAL_RESULT_OK)
            warn("Failed to release CAL resource\n");
        else
            mr->res = 0;
    }

    return err;
}

/* 2 element double items */
static CALresult writeConstantBufferDouble(MWMemRes* mr,
                                           const CALdouble* dataPtr,
                                           CALuint numberElements,
                                           CALuint width,
                                           CALuint height)
{
    CALuint i, j, k, pitch, idx;
    CALdouble* bufPtr;
    CALdouble* tmp;
    CALresult err = CAL_RESULT_OK;

    err = mapMWMemRes(mr, (CALvoid**) &bufPtr, &pitch);
    if (err != CAL_RESULT_OK)
        return err;

    for (i = 0; i < height; ++i)
    {
        tmp = &bufPtr[i * numberElements * pitch];
        for (j = 0; j < width; ++j)
        {
            idx = numberElements * (j * height + i);
            for (k = 0; k < numberElements; ++k)
            {
                tmp[numberElements * j + k] = dataPtr[idx + k];
            }
        }
    }

    return unmapMWMemRes(mr);
}

static CALresult printBufferDouble(const char* name,
                                   MWMemRes* mr,
                                   CALuint numberElements,
                                   CALuint width,
                                   CALuint height)

{
    CALuint i, j, k, pitch;
    CALdouble* bufPtr;
    CALdouble* tmp;
    CALresult err;

    err = mapMWMemRes(mr, (CALvoid**) &bufPtr, &pitch);
    if (err != CAL_RESULT_OK)
        return err;

    for (i = 0; i < height; ++i)
    {
        tmp = &bufPtr[i * pitch * numberElements];
        for (j = 0; j < width; ++j)
        {
            for (k = 0; k < numberElements; ++k)
            {
                warn("%s[%u][%u] [%u] = %.20lf\n",
                     name, j, i, k, tmp[numberElements * j + k]);
            }
        }
    }

    return unmapMWMemRes(mr);
}

static CALresult createCB(MWMemRes* mr, MWCALInfo* ci, CALformat format, CALuint size)
{
    CALresult err;

    err = calResAllocRemote1D(&mr->res, &ci->dev, 1, size, format, 0);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to allocate constant buffer", err);
        return err;
    }

    return getMemoryHandle(mr, ci);
}

const CALuint formatToNumElements(CALformat x)
{
    switch (x)
    {
        case formatReal1:
            return 1;
        case formatReal2:
            return 2;
        default:
            warn("Unhandled format to number elements: %d\n", x);
            return 0;
    }
}

const size_t formatToSize(CALformat x)
{
    switch (x)
    {
        case formatReal1:
            return sizeof(real);
        case formatReal2:
            return 2 * sizeof(real);
        default:
            warn("Unhandled format to size: %d\n", x);
            return 0;
    }
}

static CALresult createConstantBuffer2D(MWMemRes* mr,
                                        MWCALInfo* ci,
                                        const CALdouble* dataPtr,
                                        CALformat format,
                                        CALuint width,
                                        CALuint height)
{
    CALresult err;

    err = calResAllocLocal2D(&mr->res, ci->dev, width, height, format, 0);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to create 2D constant resource", err);
        return err;
    }

    err = writeConstantBufferDouble(mr, dataPtr, formatToNumElements(format), width, height);
    if (err != CAL_RESULT_OK)
        return err;

    err = getMemoryHandle(mr, ci);
    if (err != CAL_RESULT_OK)
        return err;

    return CAL_RESULT_OK;
}


static CALresult createConstantBuffer1D(MWMemRes* mr,
                                        MWCALInfo* ci,
                                        SeparationCALMem* cm,
                                        const CALvoid* src,
                                        CALformat format,
                                        CALuint width)
{
    CALresult err;
    CALvoid* bufPtr;
    CALuint pitch;

    /* Create buffer */
    err = calResAllocLocal1D(&mr->res, ci->dev, width, format, 0);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to create constant 1D resource", err);
        return err;
    }

    /* Map and write to the buffer */
    err = mapMWMemRes(mr, &bufPtr, &pitch);
    if (err != CAL_RESULT_OK)
        return err;

    memcpy(bufPtr, src, formatToSize(format) * width);

    err = unmapMWMemRes(mr);
    if (err != CAL_RESULT_OK)
        return err;

    err = getMemoryHandle(mr, ci);
    if (err != CAL_RESULT_OK)
        return err;

    return CAL_RESULT_OK;
}

static CALresult zeroBuffer(MWMemRes* mr, MWCALInfo* ci, CALuint size)
{
    CALresult err;
    CALuint pitch;
    CALvoid* ptr;

    err = mapMWMemRes(mr, &ptr, &pitch);
    if (err != CAL_RESULT_OK)
        return err;

    memset(ptr, 0, pitch * size);

    err = unmapMWMemRes(mr);
    if (err != CAL_RESULT_OK)
        return err;

    return CAL_RESULT_OK;
}

/* Output appropriate for width * height real1 elements */
static CALresult createOutputBuffer2D(MWMemRes* mr, MWCALInfo* ci, CALuint width, CALuint height)
{
    CALresult err;

    err = calResAllocLocal2D(&mr->res, ci->dev, width, height, formatReal1, 0);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to create output resource", err);
        return err;
    }

    err = zeroBuffer(mr, ci, width * sizeof(real));
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to zero output buffer", err);
        return err;
    }

    /* Get the handle for the context */
    err = getMemoryHandle(mr, ci);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to create handle for output buffer", err);
        return err;
    }

    return CAL_RESULT_OK;
}

static CALresult createOutMuBuffer(MWCALInfo* ci,
                                   SeparationCALMem* cm,
                                   const CALSeparationSizes* sizes)
{
    CALresult err;

    err = createOutputBuffer2D(&cm->outMu, ci, sizes->muSteps, sizes->rSteps);
    if (err != CAL_RESULT_OK)
        cal_warn("Failed to create output buffer", err);

    return err;
}

/* Create a separate output buffer for each stream */
static CALresult createOutStreamBuffers(MWCALInfo* ci,
                                        SeparationCALMem* cm,
                                        const CALSeparationSizes* sizes)
{
    CALuint i, numberAllocated;
    CALresult err = CAL_RESULT_OK;

    cm->outStreams = mwCalloc(cm->numberStreams, sizeof(MWMemRes));
    for (i = 0; i < cm->numberStreams; ++i)
    {
        err = createOutputBuffer2D(&cm->outStreams[i], ci, sizes->muSteps, sizes->rSteps);
        if (err != CAL_RESULT_OK)
        {
            cal_warn("Failed to create out streams buffer", err);
            break;
        }
    }

    /* Clean up if any failed before */
    numberAllocated = i;
    if (numberAllocated < cm->numberStreams)
    {
        for (i = 0; i < numberAllocated; ++i)
            releaseMWMemRes(ci->calctx, &cm->outStreams[i]);
        free(cm->outStreams);
    }

    return err;
}

static CALresult createRBuffers(MWCALInfo* ci,
                                SeparationCALMem* cm,
                                const AstronomyParameters* ap,
                                const IntegralArea* ia,
                                const StreamGauss sg,
                                const CALSeparationSizes* sizes)
{
    RPoints* r_pts;
    RConsts* rc;
    CALresult err = CAL_RESULT_OK;

    r_pts = precalculateRPts(ap, ia, sg, &rc, FALSE);

    err = createConstantBuffer2D(&cm->rPts, ci, (CALdouble*) r_pts, formatReal2, ia->r_steps, ap->convolve);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to create r_pts buffer", err);
        goto fail;
    }

    err = createConstantBuffer1D(&cm->rc, ci, cm, (CALdouble*) rc, formatReal2, ia->r_steps);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to create rc buffer", err);
        goto fail;
    }

    err = createConstantBuffer1D(&cm->sg_dx, ci, cm, sg.dx, formatReal1, ap->convolve);
    if (err != CAL_RESULT_OK)
        cal_warn("Failed to create sg_dx buffer", err);

fail:
    mwFreeA(r_pts);
    mwFreeA(rc);

    return err;
}

/* Might be more convenient to split l and b stuff for CAL */
static void getSplitLBTrig(const AstronomyParameters* ap,
                           const IntegralArea* ia,
                           TrigPair** lTrigOut,
                           TrigPair** bTrigOut)
{
    CALuint i, j;
    TrigPair* lTrig;
    TrigPair* bTrig;
    LBTrig* lbts;
    size_t idx;
    CALboolean transpose = CAL_FALSE;

    size_t size = ia->mu_steps * ia->nu_steps * sizeof(TrigPair);
    lTrig = (TrigPair*) mwMallocA(size);
    bTrig = (TrigPair*) mwMallocA(size);

    lbts = precalculateLBTrig(ap, ia, transpose);

    for (i = 0; i < ia->nu_steps; ++i)
    {
        for (j = 0; j < ia->mu_steps; ++j)
        {
            idx = transpose ? j * ia->nu_steps + i : i * ia->mu_steps + j;

            lTrig[idx].sinAngle = lbts[idx].lsin;
            lTrig[idx].cosAngle = lbts[idx].lcos;

            bTrig[idx].sinAngle = lbts[idx].bsin;
            bTrig[idx].cosAngle = lbts[idx].bcos;
        }
    }

    mwFreeA(lbts);

    *lTrigOut = lTrig;
    *bTrigOut = bTrig;
}

static CALresult createLBTrigBuffers(MWCALInfo* ci,
                                     SeparationCALMem* cm,
                                     const AstronomyParameters* ap,
                                     const IntegralArea* ia,
                                     const CALSeparationSizes* sizes)
{
    CALresult err = CAL_RESULT_OK;
    TrigPair* lTrig;
    TrigPair* bTrig;

    getSplitLBTrig(ap, ia, &lTrig, &bTrig);

    err = createConstantBuffer2D(&cm->lTrig, ci, (CALdouble*) lTrig, formatReal2, ia->nu_steps, ia->mu_steps);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to create l trig buffer", err);
        goto fail;
    }

    err = createConstantBuffer2D(&cm->bTrig, ci, (CALdouble*) bTrig, formatReal2, ia->nu_steps, ia->mu_steps);
    if (err != CAL_RESULT_OK)
        cal_warn("Failed to create b trig buffer", err);

fail:
    mwFreeA(lTrig);
    mwFreeA(bTrig);

    return err;
}

static CALresult releaseStreamBuffers(MWCALInfo* ci, SeparationCALMem* cm)
{
    CALuint i;
    CALresult err = CAL_RESULT_OK;

    if (!cm->outStreams) /* Nothing to free */
        return err;

    for (i = 0; i < cm->numberStreams; ++i)
        err |= releaseMWMemRes(ci->calctx, &cm->outStreams[i]);

    free(cm->outStreams);
    cm->outStreams = NULL;

    return err;
}

CALresult releaseSeparationBuffers(MWCALInfo* ci, SeparationCALMem* cm)
{
    CALresult err = CAL_RESULT_OK;

    err |= releaseMWMemRes(ci->calctx, &cm->outMu);
    err |= releaseStreamBuffers(ci, cm);
    err |= releaseMWMemRes(ci->calctx, &cm->rPts);
    err |= releaseMWMemRes(ci->calctx, &cm->rc);
    err |= releaseMWMemRes(ci->calctx, &cm->sg_dx);
    err |= releaseMWMemRes(ci->calctx, &cm->lTrig);
    err |= releaseMWMemRes(ci->calctx, &cm->bTrig);

    err |= releaseMWMemRes(ci->calctx, &cm->nu_id);

    /* nu_step and nu_id arguments in same buffer */
    //err |= releaseMWMemRes(ci->calctx, &cm->nu_step);

    if (err != CAL_RESULT_OK)
        cal_warn("Failed to release buffers", err);

    return err;
}

static CALresult createSeparationBuffers(MWCALInfo* ci,
                                         SeparationCALMem* cm,
                                         const AstronomyParameters* ap,
                                         const IntegralArea* ia,
                                         const StreamConstants* sc,
                                         const StreamGauss sg,
                                         const CALSeparationSizes* sizes)
{
    CALresult err = CAL_RESULT_OK;

    cm->numberStreams = ap->number_streams;

    err |= createOutMuBuffer(ci, cm, sizes);
    err |= createOutStreamBuffers(ci, cm, sizes);
    err |= createRBuffers(ci, cm, ap, ia, sg, sizes);
    err |= createLBTrigBuffers(ci, cm, ap, ia, sizes);


    /* both arguments set on nu step share 1 single element buffer */
    err |= createCB(&cm->nu_id, ci, formatReal2, 1);
    cm->nu_step = cm->nu_id;

    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to create buffers", err);
        releaseSeparationBuffers(ci, cm);
    }

    return err;
}


static CALboolean checkDeviceCapabilities(const struct CALdeviceattribsRec* attrs)
{
  #if DOUBLEPREC
    if (!attrs->doublePrecision)
    {
        warn("Device does not support double precision\n");
        return CAL_FALSE;
    }
  #endif

    /* TODO: Memory */

    return CAL_TRUE;
}

static void printCALInfo(const MWCALInfo* ci)
{
    warn("Found %u CAL devices\n"
         "Chose device %u\n"
         "\n"
         "Device target:         %s\n"
         "Revision:              %u\n"
         "Compute shader:        %s\n"
         "Engine clock:          %u Mhz\n"
         "Memory clock:          %u Mhz\n"
         "Wavefront size:        %u Mhz\n"
         "Double precision:      %s\n"
         "Number SIMD:           %u\n"
         "Number shader engines: %u\n"
         "GPU RAM:               %u\n",
         ci->numDevices,
         ci->devID,
         showCALtargetEnum(ci->devInfo.target),
         ci->devAttribs.targetRevision,
         showCALboolean(ci->devAttribs.computeShader),

         ci->devAttribs.engineClock,
         ci->devAttribs.memoryClock,
         ci->devAttribs.wavefrontSize,
         showCALboolean(ci->devAttribs.doublePrecision),
         ci->devAttribs.numberOfSIMD,
         ci->devAttribs.numberOfShaderEngines,
         ci->devAttribs.localRAM);
}

static CALobject createCALBinary(const char* srcIL)
{
    CALobject obj;
    CALresult err;

    if (!srcIL)
        return NULL;

    err = calclCompile(&obj, CAL_LANGUAGE_IL, srcIL, CAL_TARGET_CYPRESS);
    if (err != CAL_RESULT_OK)
    {
        warn("Error compiling kernel (%d) : %s\n", err, calclGetErrorString());
        return NULL;
    }

    return obj;
}

static CALimage createCALImage(const char* src)
{
    CALobject obj;
    CALresult rc;
    CALimage img;

    if (!src)
        return NULL;

    obj = createCALBinary(src);
    rc = calclLink(&img, &obj, 1);
    calclFreeObject(obj);

    if (rc != CAL_RESULT_OK)
    {
        warn("Error linking image (%d) : %s\n", rc, calclGetErrorString());
        return NULL;
    }

    return img;
}

static CALimage createCALImageFromFile(const char* filename)
{
    char* src;
    CALimage img;

    src = mwReadFile(filename);
    if (!src)
    {
        perror("IL source file");
        return NULL;
    }

    img = createCALImage(src);
    free(src);

    return img;
}

static void isaLogFunction(const char* msg)
{
    fputs(msg, stdout);
}

static CALresult printISA(CALimage image)
{
    if (!image)
        return CAL_RESULT_ERROR;

    calclDisassembleImage(image, isaLogFunction);
    return CAL_RESULT_OK;
}

static CALimage createCALImageFromGeneratedKernel(const AstronomyParameters* ap,
                                                  const IntegralArea* ia,
                                                  const StreamConstants* sc)

{
    CALimage img;
    char* src;

    src = separationKernelSrc(ap, ia, sc);
    fputs(src, stderr);

    img = createCALImage(src);
    free(src);

    printISA(img);
    return img;
}


static inline CALresult getNameMWCALInfo(MWCALInfo* ci, CALname* name, const CALchar* varName)
{
    return calModuleGetName(name, ci->calctx, ci->module, varName);
}

static void destroyModuleNames(SeparationCALNames* cn)
{
    free(cn->outStreams);
    free(cn->inStreams);
}

static CALresult getModuleNames(MWCALInfo* ci, SeparationCALNames* cn, CALuint numberStreams)
{
    CALresult err = CAL_RESULT_OK;
    CALuint i;
    char buf[20] = "";

    cn->outStreams = mwCalloc(numberStreams, sizeof(CALname));
    cn->inStreams = mwCalloc(numberStreams, sizeof(CALname));


    err |= getNameMWCALInfo(ci, &cn->sg_dx,   "cb0");
    err |= getNameMWCALInfo(ci, &cn->nu_id,   "cb1");
    err |= getNameMWCALInfo(ci, &cn->nu_step, "cb1");

    err |= getNameMWCALInfo(ci, &cn->rPts,    "i0");
    err |= getNameMWCALInfo(ci, &cn->rc,      "i1");
    err |= getNameMWCALInfo(ci, &cn->lTrig,   "i2");
    err |= getNameMWCALInfo(ci, &cn->bTrig,   "i3");

    err |= getNameMWCALInfo(ci, &cn->outMu, "o0");
    err |= getNameMWCALInfo(ci, &cn->inMu, "i4");
    for (i = 0; i < numberStreams; ++i)
    {
        sprintf(buf, "i%u", i + 5);
        err |= getNameMWCALInfo(ci, &cn->inStreams[i], buf);

        sprintf(buf, "o%u", i + 1);
        err |= getNameMWCALInfo(ci, &cn->outStreams[i], buf);
    }

    if (err != CAL_RESULT_OK)
        cal_warn("Failed to get module names", err);

    return err;
}

static CALresult setKernelArguments(MWCALInfo* ci, SeparationCALMem* cm, SeparationCALNames* cn)
{
    CALresult err = CAL_RESULT_OK;
    CALuint i;

    /* CHECKME: Bind the same output buffer to the input OK? */
    err |= calCtxSetMem(ci->calctx, cn->outMu, cm->outMu.mem);
    err |= calCtxSetMem(ci->calctx, cn->inMu, cm->outMu.mem);
    for (i = 0; i < cm->numberStreams; ++i)
    {
        err |= calCtxSetMem(ci->calctx, cn->outStreams[i], cm->outStreams[i].mem);
        err |= calCtxSetMem(ci->calctx, cn->inStreams[i],  cm->outStreams[i].mem);
    }

    err |= calCtxSetMem(ci->calctx, cn->rPts, cm->rPts.mem);
    err |= calCtxSetMem(ci->calctx, cn->rc, cm->rc.mem);
    err |= calCtxSetMem(ci->calctx, cn->lTrig, cm->lTrig.mem);
    err |= calCtxSetMem(ci->calctx, cn->bTrig, cm->bTrig.mem);
    err |= calCtxSetMem(ci->calctx, cn->sg_dx, cm->sg_dx.mem);

    err |= calCtxSetMem(ci->calctx, cn->nu_step, cm->nu_step.mem);
    err |= calCtxSetMem(ci->calctx, cn->nu_id, cm->nu_id.mem);

    if (err != CAL_RESULT_OK)
        cal_warn("Failed to set kernel arguments", err);

    return err;
}

static CALresult separationSetupCAL(MWCALInfo* ci,
                                    const AstronomyParameters* ap,
                                    const IntegralArea* ia,
                                    const StreamConstants* sc)
{
    CALresult err;

    ci->image = createCALImageFromGeneratedKernel(ap, ia, sc);
    if (!ci->image)
    {
        warn("Failed to load image\n");
        return -1;
    }

    err = calModuleLoad(&ci->module, ci->calctx, ci->image);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to load module", err);
        return err;
    }

    err = calModuleGetEntry(&ci->func, ci->calctx, ci->module, "main");
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to find main in module", err);
        return err;
    }

    return CAL_RESULT_OK;
}

static CALresult runKernel(MWCALInfo* ci, SeparationCALMem* cm, const IntegralArea* ia)
{
    CALresult err;
    CALevent ev = 0;
    CALdomain domain = { 0, 0, ia->mu_steps, ia->r_steps };

    err = calCtxRunProgram(&ev, ci->calctx, ci->func, &domain);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Error running kernel", err);
        return err;
    }

    while (calCtxIsEventDone(ci->calctx, ev) == CAL_RESULT_PENDING);

    return CAL_RESULT_OK;
}
static CALresult setNuKernelArgs(MWCALInfo* ci,
                                 SeparationCALMem* cm,
                                 SeparationCALNames* cn,
                                 const IntegralArea* ia,
                                 CALuint nuStep)
{
    CALresult err;
    CALfloat* nuStepPtr;
    CALdouble* nuIdPtr;
    CALuint pitch = 0;
    NuId nuid;

    err = mapMWMemRes(&cm->nu_step, (CALvoid**) &nuStepPtr, &pitch);
    if (err != CAL_RESULT_OK)
        return err;

    nuid = calcNuStep(ia, nuStep);

    nuStepPtr[0] = (CALfloat) nuStep;
    nuIdPtr = (CALdouble*) &nuStepPtr[2];
    *nuIdPtr = nuid.id;

    err = unmapMWMemRes(&cm->nu_step);
    if (err != CAL_RESULT_OK)
        return err;

    return CAL_RESULT_OK;
}

static real sumResults(MWMemRes* mr, const IntegralArea* ia)
{
    CALuint i, j, pitch;
    real* bufPtr;
    real* tmp;
    Kahan ksum = ZERO_KAHAN;
    CALresult err = CAL_RESULT_OK;

    err = mapMWMemRes(mr, (CALvoid**) &bufPtr, &pitch);
    if (err != CAL_RESULT_OK)
        return NAN;

    for (i = 0; i < ia->r_steps; ++i)
    {
        tmp = &bufPtr[i * pitch];
        for (j = 0; j < ia->mu_steps; ++j)
        {
            //warn("Reading in [%u][%u] = %.20lf\n", j, i, tmp[j]);
            KAHAN_ADD(ksum, tmp[j]);
        }
    }

    err = unmapMWMemRes(mr);
    if (err != CAL_RESULT_OK)
        return NAN;

    return ksum.sum + ksum.correction;
}

static real readResults(MWCALInfo* ci,
                        SeparationCALMem* cm,
                        const IntegralArea* ia,
                        real* probs_results,
                        CALuint numberStreams)
{
    CALuint i;
    real result;

    result = sumResults(&cm->outMu, ia);

    #if 1
    for (i = 0; i < numberStreams; ++i)
        probs_results[i] = sumResults(&cm->outStreams[i], ia);
    #endif

    return result;
}


static real runIntegral(MWCALInfo* ci,
                        SeparationCALMem* cm,
                        const IntegralArea* ia,
                        real* probs_results)
{
    CALresult err;
    unsigned int i;
    SeparationCALNames cn;
    double t1, t2;

    getModuleNames(ci, &cn, cm->numberStreams);

    err = setKernelArguments(ci, cm, &cn);
    if (err != CAL_RESULT_OK)
        return NAN;

    for (i = 0; i < ia->nu_steps; ++i)
    {
        warn("Trying to run step: %u\n", i);
        t1 = mwGetTime();
        err = setNuKernelArgs(ci, cm, &cn, ia, i);
        if (err != CAL_RESULT_OK)
            return NAN;

        err = runKernel(ci, cm, ia);
        if (err != CAL_RESULT_OK)
            return NAN;

        t2 = mwGetTime();
        warn("Time = %fms\n", 1000.0 * (t2 - t1));
    }

    destroyModuleNames(&cn);

    return readResults(ci, cm, ia, probs_results, cm->numberStreams);
}

static void calculateCALSeparationSizes(CALSeparationSizes* sizes,
                                        const AstronomyParameters* ap,
                                        const IntegralArea* ia)
{
    sizes->outMu = sizeof(real) * ia->mu_steps * ia->r_steps;
    sizes->outStreams = sizeof(real) * ia->mu_steps * ia->r_steps * ap->number_streams;
    sizes->rPts = sizeof(RPoints) * ap->convolve * ia->r_steps;
    sizes->rc = sizeof(RConsts) * ia->r_steps;
    sizes->sg_dx = sizeof(real) * ap->convolve;
    sizes->lTrig = sizeof(real) * ia->mu_steps * ia->nu_steps;
    sizes->bTrig = sizes->lTrig;

    sizes->nuSteps = ia->nu_steps;
    sizes->muSteps = ia->mu_steps;
    sizes->rSteps = ia->r_steps;
}

real integrateCAL(const AstronomyParameters* ap,
                  const IntegralArea* ia,
                  const StreamConstants* sc,
                  const StreamGauss sg,
                  real* st_probs,
                  EvaluationState* es,
                  const CLRequest* clr)
{
    real result = NAN;
    MWCALInfo ci;
    SeparationCALMem cm;
    CALresult err;
    CALSeparationSizes sizes;

    err = calInit();
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to init CAL", err);
        return NAN;
    }

    memset(&ci, 0, sizeof(MWCALInfo));
    memset(&cm, 0, sizeof(SeparationCALMem));
    err = mwGetCALInfo(&ci, 0);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to get CAL info", err);
        return NAN;
    }

    err = separationSetupCAL(&ci, ap, ia, sc);
    if (err != CAL_RESULT_OK)
    {
        cal_warn("Failed to setup CAL", err);
        mwDestroyCALInfo(&ci);
        return NAN;
    }

    calculateCALSeparationSizes(&sizes, ap, ia);
    err = createSeparationBuffers(&ci, &cm, ap, ia, sc, sg, &sizes);
    if (err != CAL_RESULT_OK)
        return NAN;

    result = runIntegral(&ci, &cm, ia, st_probs);

    err = releaseSeparationBuffers(&ci, &cm);
    if (err != CAL_RESULT_OK)
        result = NAN;

    err = mwDestroyCALInfo(&ci);
    if (err != CAL_RESULT_OK)
        result = NAN;

    err = calShutdown();
    if (err != CAL_RESULT_OK)
        cal_warn("Failed to shutdown CAL", err);

    return result;
}
