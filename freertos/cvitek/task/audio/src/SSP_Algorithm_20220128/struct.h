
#ifndef STRUCT_H
#define STRUCT_H

/* Include files */
#include "tmwtypes.h"
#include "define.h"


/* Cascaded IIR Filter */
typedef struct {
    float coef[20];
    float *pstate;    /*point to state of the cascaded IIR-filtering */
    unsigned short stage;    /* number of stage */
    float b0;
} cascaded_iir_struct;

/* Delay */
typedef struct {
    short *addr;    /* start address of circular buffer */
    short *rdptr;    /* current read address of CB */
    short *wrptr;    /* current write address of CB */
    unsigned short size;    /* size of CB */
    short *dly_ref_buf;
} DelayState;

/* DG */
typedef struct {
    float cut_gain;
    float boost_gain;
} dg_struct;

/* DC Filter */
typedef struct {
    float dcfilter_radius;
    float dcfilter_mem[2];    /* assign two elements per channel */
} dcfilter_struct;

/* Quadratic Filter */
typedef struct {
    short *qcoef;
    int *pstate;    /*point to state of the quad-filtering */
    unsigned short nq;    /* number of quad filter */
} qfilter_struct;

/* If TWO_FILTER enabled, AEC would adopt a foreground filter and a background filter to enhance the robustness for double-talk and difficult signals, the cost is extra FFT and matrix-vector multiplication */
#define TWO_FILTER

/* LP AEC */
typedef struct {
    int frame_size;    /* Number of samples processed each time */
    int window_size;
    int M;
    int cancel_count;
    int adapted;
    int saturated;
    int screwed_up;
    int sampling_rate;
    float spec_average;
    float beta0;
    float beta_max;
    float sum_adapt;
    float leak_estimate;

    float *e;
    float *x;    /* Far-end input buffer (2N) */
    float *X;    /* Far-end buffer (M+1 frames) in frequency domain */
    float *input;
    float *y;
    float *last_y;
    float *Y;
    float *E;
    float *PHI;
    float *W;    /* (Background) filter weights */
#ifdef TWO_FILTER
    float *foreground; /* Foreground filter weights */
    float Davg1; /* 1st recursive average of the residual power difference */
    float Davg2; /* 2nd recurstve average of the residual power dtfference */
    float Dvar1;       /* Estimated variance of 1st estimator */
    float Dvar2;       /* Estimated variance of 2nd estimator */
#endif
    float *power;  /* Power of the far-end signal */
    float *power_1;   /* Inverse power of far-end */
    float *wtmp;

    float *Rf;
    float *Yf;
    float *Xf;
    float *Eh;
    float *Yh;
    float Pey;
    float Pyy;
    float *window;
    float *prop;
    void *fft_table;
    float *memX, *memD, *memE;
    float preemph;
    float notch_radius;
    float *notch_mem;

    float aec_min_leak;
    float aec_var1_smooth;
    float aec_var2_smooth;
    float aec_var1_update;
    float aec_var2_update;
    float aec_var_backtrack;

    float ss;    /* far-end smooth coefficient */
    float RER;
    long echo_cnt;
    short initial_filter_len;
} LinearEchoState;

/* Filter Bank */
typedef struct {
    int *bank_left;
    int *bank_right;
    float *filter_left;
    float *filter_right;
    //float *scaling;
    int nb_banks;
    int len;
} FilterBank;

/* NLP AES */
typedef struct {
    int    frame_size;        /*  Number of samples processed in each frame */
    int    ps_size;           /* Number of points in the power spectrum */
    int    sampling_rate;     /*  Sampling rate of the input/output */
    FilterBank *bank;

    LinearEchoState *echo_state;

    float *frame;      /* Processing frame (2*ps_size) */
    float *ft;         /* Processing frame in freq domain (2*ps_size) */
    float *ps;         /* Current power spectrum */
    float *gain2;      /* Adjusted gains */
    float *gain_floor; /* Minimum gain allowed */
    float *window;     /* Analysis/Synthesis window */
    float *old_ps;     /* Power spectrum for last frame */
    float *gain;
    float *prior;      /* Priori SNR */
    float *post;       /* Posteriori SNR */
    float *zeta;       /* Smoothed priori SNR */
    float *echo_noise;
    float *residual_echo;
    int    echo_sup;
    int    echo_sup_active;

    float *inbuf;      /* Input OLA buffer (overlapped analysis) */
    float *outbuf;     /* Output OLA buffer (for overlap and add) */

    int    nb_adapt;          /* Number of frames used for adaptation so far */
    int    min_count;         /* Number of frames processed so far */
    void *fft_lookup;        /* Lookup table for the FFT */
    int    nbands;

    float RER_smooth;
    int RER_vad;
    int st_vad;

    /* FD STD/DTD */
    short *aes_std_in;
    short *mic_raw_ola;
    //float *fdstd_mic_in_sm;
    //float *fdstd_linear_echo_sm;
    float fdstd_tau_mean;
    float fdstd_thrd;
    int fdstd_vad;

    float aes_supp_coef1;    /* Parameters */
    float aes_supp_coef2;    /* Parameters */
    float frame1_last_sample;
} NonLinearEchoState;

/* NR */
typedef struct {
    void *nr_fft_table;
    float *ana_win;    /* analysis window before performing fft */
    float *syn_win;    /* synthesis window after performing ifft */
    short *x_old;    /* input overlap buffer */
    float *y_old;    /* output OLA buffer */
    float *Xk_prev;    /* estimated clean speech power */
    float *noise_mean;    /* noise mean */
    float *noise_mu2;    /* estimated noise power */
    float *amp_inst_band;
    float *noisy_st_trough;
    float *noisy_st_peak;
    float *amp_inst_sum_hist;
    float *float_x;    /* near-end and floating-point mic input */
    float vad_slope_cnt;
    float vad_slope;
    float vad_dr_cnt;
    float vad_dr;
    float vad_mmse;
    float vad_mmse_cnt;
    float eta;
    float ksi_min;
    float aa;
    float mu;
    float vad_mmse_cnt_para;
    float nr_frame1_last_sample;
    int frame_size;
    float Fs;
} NRState;

/* AGC */
typedef struct {
    short max_gain;
    short unit_gain;
    int      gain32;
    short env;
    short target_low;
    short target_low_Q15;
    short target_high;
    short target_high_Q15;

    short fixed_step_size;
    short stepsize_release;
    short stepsize_attack;

    short pcb_noise_thrd;
    short pcb_noise_cnt;
    short pcb_noise_hold_time;

    boolean_T unit_gain_change;
    boolean_T unit_gain_change_det;
    short sout_hist;
    short smooth_cnt;
    short alpha;
    short alpha2;

    boolean_T agc_vad1;
    short agc_vad1_cnt;
    boolean_T agc_vad2;
    short agc_vad2_cnt;
    boolean_T agc_vad3;    /* the activated AGC, take speech, noise and echo signal into account */
    boolean_T agc_vad_hist;
    short frame_size;
    short Fs;
} agc_struct;

/* SSP Algorithm Parameter */
typedef struct {
    /* SSP Parameter in MIC Path */
    unsigned short para_client_config;                    /* for choosing client's prototype machine, [0, 6] */
    unsigned short para_fun_config;                         /* functionality on/off configuration in mic path, [0, 255] */
    unsigned short para_delay_sample;                  /* the delay sample for ref signal, [1, 3000] */
    unsigned short para_dg_target;                           /* the digital gain target, [1, 12] */
    unsigned short para_aec_init_filter_len;         /* the initial filter length of linear AEC to support up for echo tail, [1, 13] */
    unsigned short para_aec_filter_len;                  /* the filter length of linear AEC to support up for echo tail, [1, 13] */
    unsigned short para_aes_std_thrd;                    /* the threshold level for residual echo, [0, 39] */
    unsigned short para_aes_supp_coeff;               /* the suppression level for residual echo, [0, 100] */
    unsigned short para_nr_init_sile_time;            /* the initial silence time supported by NR to avoid the initial bad/random signals from CODEC, [0, 250] */	
    unsigned short para_nr_snr_coeff;                     /* the coefficient of NR priori SNR tracking, [0, 25] */
    unsigned short para_agc_max_gain;                  /* the max boost gain for AGC release processing, [0, 6] */
    unsigned short para_agc_target_high;              /* the gain level of target high of AGC, [0, 36] */
    unsigned short para_agc_target_low;               /* the gain level of target low of AGC, [0, 72] */
    boolean_T          para_agc_vad_ena;                    /* speech-activated AGC functionality, [0, 1] */
    unsigned short para_notch_freq;                        /* the notch frequency, [0, 1] */
    /* Not open for tuning */
    unsigned short para_nr_noise_coeff;
    unsigned short para_agc_vad_cnt;
    boolean_T          para_agc_cut6_ena;

    /* SSP Parameter in SPK Path */
    unsigned short para_spk_fun_config;                /* functionality on/off configuration in spk path, [0, 1] */
    unsigned short para_spk_agc_max_gain;         /* the max boost gain for spk-path AGC release processing, [0, 6] */
    unsigned short para_spk_agc_target_high;     /* the gain level of target high of spk-path AGC, [0, 36] */
    unsigned short para_spk_agc_target_low;      /* the gain level of target low of spk-path AGC, [0, 72] */
    unsigned short para_spk_eq_nband;                 /* the number of bands (stages) in spk-path EQ, [1, 5] */
    unsigned short para_spk_eq_freq[5];               /* the center frequency of each band in spk-path EQ, [0, 9]@8kFs, [0, 10]@16kFs */
    unsigned short para_spk_eq_gain[5];               /* the dB gain of each band in spk-path EQ, [0, 60] */
    unsigned short para_spk_eq_qfactor[5];         /* the quality factor of each band in spk-path EQ, [0, 17] */
    /* Not open for tuning */
    boolean_T          para_spk_agc_vad_ena;
    unsigned short para_spk_agc_vad_cnt;
    boolean_T          para_spk_agc_cut6_ena;
} ssp_para_struct;

#endif


