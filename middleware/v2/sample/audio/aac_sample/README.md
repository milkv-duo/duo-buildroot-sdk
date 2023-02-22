# audio AAC sample code
#### [Notice]:
The aac sample code is based on the 3rdparty library fdk_aac built by Cvitek tool-chain.
User Can download the source code to build out the library personally.
Or contact Cvitek FAE to get the library required into adjust embedded platform.
#### [COPYRIGHT LICENSE Claim]:
* From the fdk_aac license claim:
Redistribution and use in source and binary forms, with or without modification, are permitted without payment of copyright license fees provided that you satisfy the following conditions:
You must retain the complete text of this software license in redistributions of the FDK AAC Codec or your modifications thereto in source code form.
See more through webpage: https://fedoraproject.org/wiki/Licensing/FDK-AAC
* From Cvitek:
Cvitek only suggest the using of fdk_aac codec for AAC transcode. We(Cvitek) do not provide the source code inside the SDK package. The usage of the AAC codec should follow the origin fdk_aac license rule .
Cvitek only provide the sample for usage of AAC codec. Further payment or copyright under business concern will not be the responsibility of Cvitek without contacting Cvitek product.

#### [Build Code]:
You can:
  - Contact the FAE of Cvitek to get the libs.
  - Or check the [build by user] paragraph in below section.
  -
####
[Code flow/ Sample code]:
> Check the main function in cvi_audio_aac_adp.c
> User can see the encode and decode procedure.


This text you see here is *actually* written in Markdown! To get a feel for Markdown's syntax, type some text into the left window and watch the results in the right.

#### [build by user]
-Download the toolchain for your project (ex. aarch64 / arm /...)
-Download fdk_aac source in current folder, and rename it to fdkaac.
 All the source of fdk aac should under the folder of fdkaac.
-The fdk_aac version we used is 2.0.1, you can check the release note below:
 (https://github.com/mstorsjo/fdk-aac/releases)
-make the directory 64bit, 32bit
-modified the makefile to setup the gcc toolchain path
-make clean; make
-the output locate in lib64/lib32 folders: libaacdec2.so, libaacenc2.so,
 update to your embedded board and try the encode decode.



