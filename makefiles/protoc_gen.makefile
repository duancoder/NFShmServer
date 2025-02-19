include ./define.makefile
.PHONY:all

all:${ALL_PROTOCGEN_FILE} ${ALL_PROTOC_DESCRIPTOR_FILE}

${PROTOCOL_COMM_H} ${PROTOCOL_COMM_CPP} ${PROTOCOL_COMM_DESCRIPTOR}:${PROTOCOL_COMM_XML}
	mkdir -p ${PROTOCGEN_FILE_PATH}
	${PROTOC} $^ -I${THIRD_PARTY_INC_PATH} -I${PROTOCOL_COMM_PATH} -I${PROTOCOL_KERNEL_PATH}\
		--include_imports --descriptor_set_out=${PROTOCOL_COMM_DESCRIPTOR} --cpp_out=${PROTOCGEN_FILE_PATH}
	${PROTO2STRUCT} --proto_ds=${PROTOCOL_COMM_DESCRIPTOR} --proto_fname=proto_common.proto --out_path=${PROTOCGEN_FILE_PATH}/
	cp ${PROTOCOL_COMM_H} ${PROTOCOL_COMM_CPP} ${NEW_PROTOCGEN_FILE_PATH} -rf

${PROTOCOL_EVENT_H} ${PROTOCOL_EVENT_CPP} : ${PROTOCOL_EVENT_XML}
	mkdir -p ${PROTOCGEN_FILE_PATH}
	${PROTOC} $^ -I${THIRD_PARTY_INC_PATH} -I${PROTOCOL_COMM_PATH}  -I${PROTOCOL_KERNEL_PATH}\
		--cpp_out=${PROTOCGEN_FILE_PATH}
	cp ${PROTOCOL_EVENT_H} ${PROTOCOL_EVENT_CPP} ${NEW_PROTOCGEN_FILE_PATH} -rf

${PROTOCOL_SS_H} ${PROTOCOL_SS_CPP} :${PROTOCOL_SS_XML} ${PROTOCOL_COMM_XML} ${PROTOCOL_CS_XML} ${FIELD_OPTIONS_XML}
	${PROTOC} $^ -I${THIRD_PARTY_INC_PATH} -I${PROTOCOL_COMM_PATH} -I${PROTOCOL_CS_PATH} -I${PROTOCOL_SS_PATH}  -I${PROTOCOL_KERNEL_PATH}\
		--cpp_out=${PROTOCGEN_FILE_PATH}
	cp ${PROTOCOL_SS_H} ${PROTOCOL_SS_CPP} ${NEW_PROTOCGEN_FILE_PATH} -rf

${RESDB_META_CS_H} ${RESDB_META_CS_CPP} ${RESDB_META_DESCRIPTOR}:${RESDB_META_CS_XML} ${PROTOCOL_COMM_XML} ${FIELD_OPTIONS_XML}
	mkdir -p ${PROTOCGEN_FILE_PATH}
	${PROTOC} $^ -I${THIRD_PARTY_INC_PATH} -I${RESDB_META_PATH} -I${PROTOCOL_COMM_PATH}  -I${PROTOCOL_KERNEL_PATH}\
			--include_imports --descriptor_set_out=${RESDB_META_DESCRIPTOR} --cpp_out=${PROTOCGEN_FILE_PATH}
	${PROTO2STRUCT} --proto_ds=${RESDB_META_DESCRIPTOR} --proto_fname=ResMeta.proto --out_path=${PROTOCGEN_FILE_PATH}/
	cp ${RESDB_META_CS_H} ${RESDB_META_CS_CPP} ${NEW_PROTOCGEN_FILE_PATH} -rf

${PROTOCOL_CS_H} ${PROTOCOL_CS_CPP} :${PROTOCOL_CS_XML}
	mkdir -p ${PROTOCGEN_FILE_PATH}
	${PROTOC} $^ -I${PROTOCOL_CS_PATH} --cpp_out=${PROTOCGEN_FILE_PATH}
	cp ${PROTOCOL_CS_H} ${PROTOCOL_CS_CPP} ${NEW_PROTOCGEN_FILE_PATH} -rf



