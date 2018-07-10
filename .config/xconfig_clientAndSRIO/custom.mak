## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,e66 linker.cmd \
  package/cfg/clientAndSRIO_pe66.oe66 \

linker.cmd: package/cfg/clientAndSRIO_pe66.xdl
	$(SED) 's"^\"\(package/cfg/clientAndSRIO_pe66cfg.cmd\)\"$""\"D:/workspace_v8/zhou/20180326/32ChannelSystem_DSP_release_v1.0_2018_01_12/.config/xconfig_clientAndSRIO/\1\""' package/cfg/clientAndSRIO_pe66.xdl > $@
