# Cvitek Platform Support
obj-$(CONFIG_SND_CV1835_I2S) += cv1835_i2s.o

# Cvitek Machine Support
snd-soc-cv1835-adau1372-objs := cv1835_adau1372.o

obj-$(CONFIG_SND_SOC_CV1835_ADAU1372) += snd-soc-cv1835-adau1372.o

obj-$(CONFIG_SND_SOC_CV1835_CV1835PDM) += cv1835_cv1835pdm.o

obj-$(CONFIG_SND_SOC_CV182X_CV182XPDM) += cv182x_cv182xpdm.o

obj-$(CONFIG_SND_SOC_CV1835PDM) += cv1835pdm.o

obj-$(CONFIG_SND_SOC_CV1835_CV1835ADC) += cv1835_cv1835adc.o

ifeq ($(CONFIG_SND_SOC_CV1835ADC),m)
obj-$(CONFIG_SND_SOC_CV1835ADC) += cvi_ai.o
cvi_ai-objs += cv1835adc.o
else
obj-$(CONFIG_SND_SOC_CV1835ADC) += cv1835adc.o
endif


obj-$(CONFIG_SND_SOC_CV1835_CV1835DAC) += cv1835_cv1835dac.o

ifeq ($(CONFIG_SND_SOC_CV1835DAC),m)
obj-$(CONFIG_SND_SOC_CV1835DAC) += cvi_ao.o
cvi_ao-objs += cv1835dac.o
else
obj-$(CONFIG_SND_SOC_CV1835DAC) += cv1835dac.o
endif

#obj-$(CONFIG_SND_SOC_CV183x_DUMMY_CARD) += cv1835_dummy_codec.o
obj-$(CONFIG_SND_SOC_CV183x_DUMMY_CARD) += dummy_codec.o

obj-$(CONFIG_SND_SOC_CV182X_CV182XADC) += cv182x_cv182xadc.o

obj-$(CONFIG_SND_SOC_CV182XA_CV182XAADC) += cv181x_cv181xadc.o

ifeq ($(CONFIG_SND_SOC_CV182XADC),m)
obj-$(CONFIG_SND_SOC_CV182XADC) += cvi_ai.o
cvi_ai-objs += cv182xadc.o
else
obj-$(CONFIG_SND_SOC_CV182XADC) += cv182xadc.o
endif

ifeq ($(CONFIG_SND_SOC_CV182XAADC),m)
obj-$(CONFIG_SND_SOC_CV182XAADC) += cv182xa_ai.o
cv182xa_ai-objs += cv181xadc.o
else
obj-$(CONFIG_SND_SOC_CV182XAADC) += cv181xadc.o
endif

#obj-y += cv1835_dummy_codec.o
#obj-y += dummy_codec.o



obj-$(CONFIG_SND_SOC_CV182X_CV182XDAC) += cv182x_cv182xdac.o

obj-$(CONFIG_SND_SOC_CV182XA_CV182XADAC) += cv181x_cv181xdac.o

ifeq ($(CONFIG_SND_SOC_CV182XDAC),m)
obj-$(CONFIG_SND_SOC_CV182XDAC) += cvi_ao.o
cvi_ao-objs += cv182xdac.o
else
obj-$(CONFIG_SND_SOC_CV182XDAC) += cv182xdac.o
endif

ifeq ($(CONFIG_SND_SOC_CV182XADAC),m)
obj-$(CONFIG_SND_SOC_CV182XADAC) += cv182xa_ao.o
cv182xa_ao-objs += cv181xdac.o
else
obj-$(CONFIG_SND_SOC_CV182XADAC) += cv181xdac.o
endif

# Cvitek audio subsys support
obj-$(CONFIG_CV1835_I2S_SUBSYS) += cv1835_i2s_subsys.o

obj-$(CONFIG_SND_SOC_CV1835_LT9611) += cv1835_lt9611.o
