/*
 * simple2.cpp
 *
 *  Created on: 23 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void    add3(float *dst, const float *src1, const float *src2, size_t count);
    void    sub3(float *dst, const float *src1, const float *src2, size_t count);
    void    mul3(float *dst, const float *src1, const float *src2, size_t count);
    void    div3(float *dst, const float *src1, const float *src2, size_t count);
    void    mod3(float *dst, const float *src1, const float *src2, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    add3(float *dst, const float *src1, const float *src2, size_t count);
        void    sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    div3(float *dst, const float *src1, const float *src2, size_t count);
    }

    namespace sse2
    {
        void    mod3(float *dst, const float *src1, const float *src2, size_t count);
    }

    namespace avx
    {
        void    add3(float *dst, const float *src1, const float *src2, size_t count);
        void    sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    div3(float *dst, const float *src1, const float *src2, size_t count);
        void    mod3(float *dst, const float *src1, const float *src2, size_t count);
        void    mod3_fma3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    add3(float *dst, const float *src1, const float *src2, size_t count);
        void    sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    div3(float *dst, const float *src1, const float *src2, size_t count);
        void    mod3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void    add3(float *dst, const float *src1, const float *src2, size_t count);
        void    sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    div3(float *dst, const float *src1, const float *src2, size_t count);
//        void    mod3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

typedef void (* func3)(float *dst, const float *src1, const float *src2, size_t count);

//-----------------------------------------------------------------------------
// Unit test for simple operations
UTEST_BEGIN("dsp.pmath", op3)

    void call(const char *label, size_t align, func3 func1, func3 func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src1(count, align, mask & 0x01);
                FloatBuffer src2(count, align, mask & 0x02);
                FloatBuffer dst1(count, align, mask & 0x04);
                FloatBuffer dst2(count, align, mask & 0x04);

                // Call functions
                src1.randomize_sign();
                src2.randomize_sign();
                func1(dst1, src1, src2, count);
                func2(dst2, src1, src2, count);

                UTEST_ASSERT_MSG(src1.valid(), "Source buffer 1 corrupted");
                UTEST_ASSERT_MSG(src2.valid(), "Source buffer 2 corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2, 1e-4))
                {
                    src1.dump("src1");
                    src2.dump("src2");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    printf("index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(native, func, align) \
            call(#func, align, native, func)

        IF_ARCH_X86(CALL(native::add3, sse::add3, 16));
        IF_ARCH_X86(CALL(native::sub3, sse::sub3, 16));
        IF_ARCH_X86(CALL(native::mul3, sse::mul3, 16));
        IF_ARCH_X86(CALL(native::div3, sse::div3, 16));
        IF_ARCH_X86(CALL(native::mod3, sse2::mod3, 16));

        IF_ARCH_X86(CALL(native::add3, avx::add3, 32));
        IF_ARCH_X86(CALL(native::sub3, avx::sub3, 32));
        IF_ARCH_X86(CALL(native::mul3, avx::mul3, 32));
        IF_ARCH_X86(CALL(native::div3, avx::div3, 32));
        IF_ARCH_X86(CALL(native::mod3, avx::mod3, 32));
        IF_ARCH_X86(CALL(native::mod3, avx::mod3_fma3, 32));

        IF_ARCH_ARM(CALL(native::add3, neon_d32::add3, 16));
        IF_ARCH_ARM(CALL(native::sub3, neon_d32::sub3, 16));
        IF_ARCH_ARM(CALL(native::mul3, neon_d32::mul3, 16));
        IF_ARCH_ARM(CALL(native::div3, neon_d32::div3, 16));
        IF_ARCH_ARM(CALL(native::mod3, neon_d32::mod3, 16));

        IF_ARCH_AARCH64(CALL(native::add3, asimd::add3, 16));
        IF_ARCH_AARCH64(CALL(native::sub3, asimd::sub3, 16));
        IF_ARCH_AARCH64(CALL(native::mul3, asimd::mul3, 16));
        IF_ARCH_AARCH64(CALL(native::div3, asimd::div3, 16));
//        IF_ARCH_AARCH64(CALL(native::mod3, asimd::mod3, 16));
    }
UTEST_END


