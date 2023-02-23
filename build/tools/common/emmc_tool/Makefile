.PHONY: clean out
PARTITION_XML ?= partition8G_emmc.xml
INSTALL_DIR ?= ./
out:
	gcc mk_gpt.c crc.c -luuid -lxml2 -L ./ -lm -I ./ -o mk_gpt
	gcc creat_pack.c crc32_uboot.c -lxml2 -L ./ -lm -I ./ -o crt_p
mk_gpt:mk_gpt.c
	gcc mk_gpt.c crc.c -luuid -lxml2 -L ./ -lm -I ./ -o mk_gpt
gpt.img:mk_gpt $(PARTITION_XML)
	./mk_gpt -p $(PARTITION_XML) -d $(INSTALL_DIR)/gpt.img
clean:
	rm -f mk_gpt crt_p mk_sdscript
