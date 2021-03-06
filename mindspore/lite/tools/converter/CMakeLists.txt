add_definitions(-DPRIMITIVE_WRITEABLE)
string(REPLACE    " -Werror " " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

file (GLOB_RECURSE CORE_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/*.cc
        )
list(REMOVE_ITEM CORE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/utils/log_adapter.cc)
list(REMOVE_ITEM CORE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../../../core/ir/graph_utils_extends.cc)

set(CCSRC_SRC
        ${CCSRC_DIR}/backend/optimizer/common/pattern_engine.cc
        ${CCSRC_DIR}/backend/optimizer/common/visit.cc
        ${CCSRC_DIR}/backend/optimizer/common/optimizer.cc
)


if (WIN32)
    set(LITE_SRC
            #src
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/tensor.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/model.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/lite_session.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/kernel_registry.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/common/graph_util.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/runtime/runtime_api.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/runtime/thread_pool.c
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/runtime/workspace_pool.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/runtime/allocator.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/executor.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/scheduler.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/lite_kernel.cc
            ${CMAKE_CURRENT_SOURCE_DIR}../../nnacl/pack.c
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/populate_parameter.cc
            )
endif()

file(GLOB_RECURSE OPS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../../src/ops/*.cc)

file(GLOB_RECURSE CONVERTER_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../flag/flag_parser.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/converter.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/converter_flags.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/anf_transform.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/graphdef_transform.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/optimizer.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src/common/file_utils.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/graph_util.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/node_util.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/tensor_util.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/protobuf_utils.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/flag_parser.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/storage.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src/ir/primitive_t_value.cc

        ../optimizer/common/node_pass_extends.cc
        ../optimizer/common/pass_manager_extends.cc
        ../optimizer/common/gllo_utils.cc
        ../optimizer/fusion/conv_biasadd_fusion.cc
        ../optimizer/fusion/conv_activation_fusion.cc
        ../optimizer/fusion/conv_tuple_activation_fusion.cc
        ../optimizer/fusion/conv_transform_fusion.cc
        ../optimizer/fusion/conv_scale_fusion.cc
        ../optimizer/fusion/conv_bn_fusion.cc
        ../optimizer/fusion/constant_folding_fusion.cc
        )

add_subdirectory(../anf_importer anf_importer)
add_subdirectory(../anf_exporter anf_exporter)
add_subdirectory(parser/caffe)
add_subdirectory(parser/tflite)
add_subdirectory(parser/onnx)
add_subdirectory(legacy_optimizer)
add_subdirectory(quantizer)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
set(LITE_SRC
        ${SRC_DIR}/common/graph_utils_extends.cc
        ${SRC_DIR}/common/log_adapter.cc
        ${SRC_DIR}/common/graph_util.cc
        ${SRC_DIR}/runtime/allocator.cc
        ${SRC_DIR}/runtime/runtime_api.cc
        ${SRC_DIR}/runtime/thread_pool.c
        ${SRC_DIR}/runtime/workspace_pool.cc
        ${SRC_DIR}/tensor.cc
        ${SRC_DIR}/kernel_registry.cc
        ${SRC_DIR}/lite_kernel.cc
        ${SRC_DIR}/populate_parameter.cc
        ${SRC_DIR}/scheduler.cc
        ${SRC_DIR}/lite_session.cc
        ${SRC_DIR}/executor.cc
        ${SRC_DIR}/model.cc
        )
if (SUPPORT_TRAIN)
set(LITE_SRC 
        ${LITE_SRC}
 )

endif ()
set(ARM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src/runtime/kernel/arm)
file(GLOB KERNEL_SRC
        ${ARM_DIR}/base/*.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../../nnacl/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../../nnacl/fp32/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../../nnacl/int8/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../../nnacl/quantization/*.c
        ${ARM_DIR}/fp32/*.cc
        ${ARM_DIR}/int8/*.cc
        )
list(REMOVE_ITEM KERNEL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../../nnacl/opt_op_handler.c)

if (PLATFORM_ARM64)
    # assembly
    file(GLOB ASSEMBLY_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../../nnacl/assembly/arm64/*.s
            ${CMAKE_CURRENT_SOURCE_DIR}/../../nnacl/assembly/arm64/*.S)
    set_property(SOURCE ${ASSEMBLY_SRC} PROPERTY LANGUAGE C)
    set(KERNEL_SRC ${KERNEL_SRC} ${ASSEMBLY_SRC})
endif ()

if (WIN32)
    set(PROTO_LIBRARY mindspore::protobuf)
else()
    set(PROTO_LIBRARY protobuf)
endif()

add_executable(converter_lite
        main.cc
        ${CORE_SRC}
        ${CCSRC_SRC}
        ${CONVERTER_SRC}
        ${OPS_SRC}
        ${KERNEL_SRC}
        ${LITE_SRC}
        )

if (WIN32)
    add_library(converter_mid OBJECT
            ${CORE_SRC}
            ${CCSRC_SRC}
            ${CONVERTER_SRC}
            ${OPS_SRC}
            ${KERNEL_SRC}
            ${LITE_SRC}
            )
    add_library(converter_parser STATIC
            $<TARGET_OBJECTS:tflite_parser_mid>
            $<TARGET_OBJECTS:caffe_parser_mid>
            $<TARGET_OBJECTS:onnx_parser_mid>
            $<TARGET_OBJECTS:anf_importer_mid>
            $<TARGET_OBJECTS:anf_exporter_mid>
            $<TARGET_OBJECTS:graph_pass_mid>
            $<TARGET_OBJECTS:fusion_mid>
            $<TARGET_OBJECTS:quantizer_mid>
            $<TARGET_OBJECTS:converter_mid>
            )
endif()

target_link_libraries(converter_lite PRIVATE
        tflite_parser_mid
        caffe_parser_mid
        onnx_parser_mid
        anf_importer_mid
        anf_exporter_mid
        graph_pass_mid
        fusion_mid
        quantizer_mid
        quantizer_mid
        pthread
        ${PROTO_LIBRARY}
        ${SECUREC_LIBRARY}
        mindspore::json
        mindspore::eigen
        )

if (NOT PLATFORM_ARM64 AND NOT PLATFORM_ARM32)
    install(TARGETS converter_lite
            RUNTIME DESTINATION ${MAIN_DIR}-${COMPONENT_NAME}/converter COMPONENT ${COMPONENT_NAME})
endif ()
