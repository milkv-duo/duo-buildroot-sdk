
#ifndef LT9611_H_
#define LT9611_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum
{
    E_LT9611_VIDEO_1024x768_60HZ,
    E_LT9611_VIDEO_1280x720_60HZ,
    E_LT9611_VIDEO_1280x1024_60HZ,
    E_LT9611_VIDEO_1600x1200_60HZ,
    E_LT9611_VIDEO_1920x1080_30HZ,
    E_LT9611_VIDEO_1920x1080_60HZ,
    E_LT9611_VIDEO_MAX
} LT9611_VIDEO_FORMAT_E;

struct lt9611_data {
	struct i2c_client *client;
    int irq;
	int irq_gpio;
	int rst_gpio;
};

void lt9611_init(LT9611_VIDEO_FORMAT_E enVideoFormat);

void lt9611_set_video_format(LT9611_VIDEO_FORMAT_E enVideoFormat);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

