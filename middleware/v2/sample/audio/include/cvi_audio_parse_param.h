#ifndef __CVI_AUDIO_PARSE_PARAM_H__
#define __CVI_AUDIO_PARSE_PARAM_H__

#if defined(__CV181X__) || defined(__CV180X__)
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <linux/cvi_type.h>
#include <linux/cvi_common.h>
#include <string.h>
#else
#include "cvi_common.h"
#endif
#include "acodec.h"

typedef struct {
	int sample_rate;
	int channel;
	int preiod_size;
	int codec;
	//PAYLOAD_TYPE_E eType;
	bool bVqeOn;
	bool bAecOn;
	bool bResmp;
	int bind_mode;
	int record_time;
} stAudioPara;



int audio_parse(int argc, char **argv);
int _get_parseval(stAudioPara stAudparam);
int _parsing_audio_status(void);
int _parsing_request(char *printout, int default_val);

#endif


