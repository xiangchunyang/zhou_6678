## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,e66 linker.cmd \
  package/cfg/clientAndSRIO_pe66.oe66 \

linker.cmd: package/cfg/clientAndSRIO_pe66.xdl
	$(SED) 's"^\"\(package/cfg/clientAndSRIO_pe66cfg.cmd\)\"$""\"D:/workspace_v8/zhou/20180703/master/Debug/configPkg/\1\""' package/cfg/clientAndSRIO_pe66.xdl > $@
