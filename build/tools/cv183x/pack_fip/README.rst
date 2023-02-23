HELP
====
$ python3 pack_fip.py -h
usage: pack_fip.py [-h] [--add-bl33 BL33_BIN] [--add-blp BLP_BIN]
                   [--add-bldp BLDP_BIN] [--output OUTPUT] [--version]
                   [--verbose] [--unpack]
                   FIP_BIN

FIP packer

positional arguments:
  FIP_BIN              Input FIP binary

optional arguments:
  -h, --help           show this help message and exit
  --add-bl33 BL33_BIN  Merge BL33 into FIP
  --add-blp BLP_BIN    Merge BLP into FIP
  --add-bldp BLDP_BIN  Merge BLDP into FIP
  --output OUTPUT      Output filename
  --version            Output version information and exit
  --verbose            Increase output verbosity
  --unpack             Unpack FIP.bin


Merge BL33 into fip.bin
=======================
$ python3 pack_fip.py tmp/fip.bin --add-bl33 ./tmp/BL33.bin


TODO
====
Add BLDP support
