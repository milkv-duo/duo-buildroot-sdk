
#ifndef FFTCORE_H
#define FFTCORE_H

/** Discrete Rotational Fourier Transform (DRFT) lookup */
struct drft_lookup{
  int n;
  float *trigcache;
  int *splitcache;
};

/* Function Declarations */
extern void core_drft_forward(struct drft_lookup *l,float *data);
extern void core_drft_backward(struct drft_lookup *l,float *data);
extern void core_drft_init(struct drft_lookup *l,int n);
extern void core_drft_clear(struct drft_lookup *l);

#endif


