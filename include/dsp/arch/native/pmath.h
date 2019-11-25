/*
 * pmath.h
 *
 *  Created on: 12 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_PMATH_H_
#define DSP_ARCH_NATIVE_PMATH_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{

    void abs_normalized(float *dst, const float *src, size_t count)
    {
        // Calculate absolute values
        dsp::abs2(dst, src, count);

        // Find the maximum value
        float max = dsp::max(dst, count);

        // Divide if it is possible
        if (max != 0.0f)
            dsp::mul_k2(dst, 1.0f / max, count);
    }

    void normalize(float *dst, const float *src, size_t count)
    {
        // Find minimum and maximum
        float max = 0.0f, min = 0.0f;
        dsp::minmax(src, count, &min, &max);

        // Determine maximum possible value
        max = (max > 0.0f) ? max : -max;
        min = (min > 0.0f) ? min : -min;
        if (max < min)
            max = min;

        // Normalize OR copy
        if (max > 0.0f)
            dsp::mul_k3(dst, src, 1.0f / max, count);
        else
            dsp::copy(dst, src, count);
    }

    void accumulate(float *dst, const float *src, float k, float p, size_t count)
    {
        while (count--)
        {
            *dst = (*dst) * k + (*src) * p;
            dst++;
            src++;
        }
    }

    void integrate(float *dst, const float *src, float k, size_t count)
    {
        while (count--)
        {
            *dst = *dst + (*src - *dst) * k;
            dst++;
            src++;
        }
    }

    void exp1(float *dst, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]  = expf(dst[i]);
    }

    void exp2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]  = expf(src[i]);
    }

    void logb1(float *dst, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = logf(dst[i]) * M_LOG2E;
    }

    void logb2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = logf(src[i]) * M_LOG2E;
    }

    void loge1(float *dst, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = logf(dst[i]);
    }

    void loge2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = logf(src[i]);
    }

    void logd1(float *dst, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = log10f(dst[i]);
    }

    void logd2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = log10f(src[i]);
    }

    void powcv1(float *v, float c, size_t count)
    {
        float C = logf(c);
        for (size_t i=0; i<count; ++i)
            v[i] = expf(v[i] * C);
    }

    void powcv2(float *dst, const float *v, float c, size_t count)
    {
        float C = logf(c);
        for (size_t i=0; i<count; ++i)
            dst[i] = expf(v[i] * C);
    }

    void powvc1(float *c, float v, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            c[i] = expf(v * logf(c[i]));
    }

    void powvc2(float *dst, const float *c, float v, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = expf(v * logf(c[i]));
    }

    void powvx1(float *v, const float *x, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            v[i] = expf(x[i] * logf(v[i]));
    }

    void powvx2(float *dst, const float *v, const float *x, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = expf(x[i] * logf(v[i]));
    }
}

#endif /* DSP_ARCH_NATIVE_PMATH_H_ */
