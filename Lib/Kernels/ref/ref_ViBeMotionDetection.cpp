/*
    File: ref_VibeMotionDetection.c
    Содержит релизацию алгоритма ViBe

    Date: 7 декабря 2019
*/

#include "../ref.h"
#include "cmath.h"
#include "cstdlib.h"

vx_status ref_ViBeMotionDetection(const vx_image src_image,
    vx_image dst_image,
    vx_image samples,
    vx_image background,
    vx_image foreground,
    const vx_ViBeMotionDetection N,
    const vx_ViBeMotionDetection R,
    const vx_ViBeMotionDetection h_min,
    const vx_ViBeMotionDetection phi
)
{
    if ((N->ViBeMotionDetection_type != VX_VIBEMOTIONDETECTION_TYPE_BINARY && N->ViBeMotionDetection_type != VX_VIBEMOTIONDETECTION_TYPE_RANGE) &&
        (R->ViBeMotionDetection_type != VX_VIBEMOTIONDETECTION_TYPE_BINARY && R->ViBeMotionDetection_type != VX_VIBEMOTIONDETECTION_TYPE_RANGE) && 
        (h_min->ViBeMotionDetection_type != VX_VIBEMOTIONDETECTION_TYPE_BINARY && h_min->ViBeMotionDetection_type != VX_VIBEMOTIONDETECTION_TYPE_RANGE) &&
        (phi->ViBeMotionDetection_type != VX_VIBEMOTIONDETECTION_TYPE_BINARY && phi->ViBeMotionDetection_type != VX_VIBEMOTIONDETECTION_TYPE_RANGE)&&((phi<0)||(phi>1)))
    {
        return VX_ERROR_INVALID_PARAMETERS;
    }

    const uint32_t src_width = src_image->width;
    const uint32_t src_height = src_image->height;
    const uint32_t dst_width = dst_image->width;
    const uint32_t dst_height = dst_image->height;
    const uint32_t sam_height = samples->height;
    const uint32_t sam_width = samples->width;

    const uint32_t back_height = background>height;
    const uint32_t back_width = background->width;

    const uint32_t fore_height = foreground > height;
    const uint32_t fore_width = foreground->width;

    if (src_width != dst_width || src_height != dst_height)
    {
        return VX_ERROR_INVALID_PARAMETERS;
    }

    const uint8_t* src_data = src_image->data;
    uint8_t* dst_data = dst_image->data;
    uint8_t* samples_data = samples->data;
    uint8_t* background_data = background->data;
    uint8_t* foreground_data = foreground->data;


    for (uint32_t ind = 0; ind < src_width * src_height; ++ind)
    {

            int count = 0;
            int index = 0;
            double dist = 0;
            while ((count < h_min) && (index < N)) {
                dist = sqrt(sqr(src_image[ind]) + sqr(samples[ind]));
                if (dist < R) {
                    count = count + 1;
                }
                index = index + 1
            }
            double rand_N = 0;
            if (count >= h_min) {
                dst_image[ind] = background;
                rand_N = -rand(100 * phi);
                rand_N /= 100;

            if (rand_N == 0) {
                rand_N = rand(N);
                samples[rand_N] = src_image[ind];
            };

            rand_N = -rand(100 * phi);
            rand_N /= 100;
            int x, y;
            if (rand_N == 0) {
                x = rand(2) - 1;
                y = rand(2) - 1;
                if (x ==0 && y == 0) {
                    while (x != 0 || y != 0) {
                        x = rand(2) - 1;
                        y = rand(2) - 1;
                    }
                }
                int neighbour_x = src_width[ind] + x;
                int neighbour_y = src_height[ind] + y;
                rand_N = rand(N);
                samples[neighbour_y][neighbour_x][rand] = src_image[ind];
            }
            else {
                dst_image[ind] = foreground;
            }
    }

    return VX_SUCCESS;
}
