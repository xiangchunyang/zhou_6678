## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,e66 linker.cmd \
  package/cfg/client_pe66.oe66 \

linker.cmd: package/cfg/client_pe66.xdl
	$(SED) 's"^\"\(package/cfg/client_pe66cfg.cmd\)\"$""\"C:/Users/hxl/workspace_v5_2/SocketEchoServerAndSRIO_evmc6678l_1/.config/xconfig_client/\1\""' package/cfg/client_pe66.xdl > $@
