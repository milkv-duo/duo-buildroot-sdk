# Audio rpc two process example
version: 20221026
#### Contain two folder
 - up_example
 - down_example

up_example:

> -AI_TEST : creates ain getframe thread ...saving file:[ai_getframe.raw] [8k,2chn]
> -AI_BIND_AENC_TEST :creates aenc get stream thread for bind mode ...saving file:[aenc_get.g711a][8k,2chn]
> -AI_UBIND_AENC_TEST :creates aenc get stream thread for unbind mode ...saving file:[aenc_ubind.g711a][8k,2chn]

down_example:

> -AO_TEST : play file [record0.raw][8k,2chn]
> -ADEC_BIND_AO_TEST : creates adec thread for bind mode to decode and play file [adec.g711a][8k,2chn]
> -ADEC_UBIND_AO_TEST : creates adec thread for unbind mode to decode and play file [adec.g711a][8k,2chn]

#### How to use the "audio_master" "audio_slave"
```sh

#### upexample
$ cd up_example; make all;
#Put those two to your board, under tmp folder
#run audio_master in the background then run audio_slave in forground
$ cp audio_master /tmp/.
$ cp audio_slave /tmp/.
$ cd /tmp/
$ audio_master (0/1/2) &
$ audio_slave (0/1/2)
> - 0: AI_TEST
> - 1: AI_BIND_AENC_TEST
> - 2: AI_UBIND_AENC_TEST

#### downexample
$ cd down_example; make all;

#Put those two to your board, under tmp folder
#run audio_master in the background then run audio_slave in forground
$ cp audio_master /tmp/.
$ cp audio_slave /tmp/.
$ cd /tmp/
$ audio_master (0/1/2) &
$ audio_slave (0/1/2)
> - 0: AO_TEST
> - 1: ADEC_BIND_AO_TEST
> - 2: ADEC_UBIND_AO_TEST
#Check the outcome file in tmp folder...
```
