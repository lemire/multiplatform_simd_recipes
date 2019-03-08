#pragma once
// credit: https://github.com/skeeto/mandel-simd/
struct spec {
    /* Image Specification */
    int width;
    int height;
    int depth;
    /* Fractal Specification */
    float xlim[2];
    float ylim[2];
    int iterations;
};
