#ifndef FIELD_H
#define FIELD_H

#include <random>
#include <vector>
#include <fftw3.h>
#include <string>

#include <cuda_runtime.h>
#include <cufft.h>
#include <curand.h>

#include "defines.h"

class term;

struct pres;

class evolver;

class field
{
    private:
    const int sx, sy, sz;
    const float dx, dy, dz;
    const float stepqx, stepqy, stepqz;

    fftwf_plan plan_forward;
    fftwf_plan plan_backward;
    fftwf_plan plan_forward_dealias;
    fftwf_plan plan_backward_dealias;

    fftwf_plan noise_plan;

    cufftHandle plan_gpu;

    curandGenerator_t gen_d;
    cudaStream_t stream_d;
    curandRngType_t rng_d;
    curandOrdering_t order_d;


    public:
    field(int, float);
    field(int, int, float, float);
    field(int, int, int, float, float, float);
    void common_constructor();

    std::string name;

    bool dynamic;
    int integrator;

    // Random things
    bool isNoisy;
    NoiseType noiseType;

    std::random_device rd;
    std::mt19937 rng;
    std::normal_distribution<> dist;
    // float D;

    bool isCUDA;

    bool outputToFile;

    evolver *system_p;

    float2 *real_array;
    float2 *comp_array;

    bool needsaliasing;
    int aliasing_order;
    float2 *comp_dealiased;
    float2 *real_dealiased;
    float2 *comp_dealiased_d;
    float2 *real_dealiased_d;

    float2 *noise_comp;
    float2 *noise_gend;

    float *gen_noise;
    float2 *noise_real;
    float2 *noise_fourier;

    float *noise_comp_d_r;
    float *noise_comp_d_i;

    float2 *real_array_d;
    float2 *comp_array_d;

    float2 **terms_h;
    float2 **terms_d;


    std::vector<term *> terms;
    std::vector<pres> implicit;

    float *precomp_implicit;
    float *precomp_implicit_d;

    pres noise_amplitude;
    float *precomp_noise;
    float *precomp_noise_d;

    pres *implicit_terms;

    // callback functions 
    bool hasCB;
    void (*callback) (evolver *, float2 *, int, int, int);
    bool hasCBFourier;
    void (*callbackFourier) (evolver *, float2 *, int, int, int);

    dim3 threads_per_block;
    dim3 blocks;

    int setRHS(float);
    int updateTerms();
    void createNoise();
    void setToZero();
    void setNotDynamic(float);
    void setDynamic(float);

    void stepEuler(float);
    void stepRK2(float);
    void stepRK4(float);

    void toReal();
    void toComp();
    void normalize();
    void dealias();

    void copyHostToDevice();
    void copyDeviceToHost();
    void copyRealHostToDevice();
    void copyRealDeviceToHost();

    void writeToFile(int , int , int );

    void prepareDevice();

    void precalculateImplicit(float dt);
};

#endif // FIELD_H
