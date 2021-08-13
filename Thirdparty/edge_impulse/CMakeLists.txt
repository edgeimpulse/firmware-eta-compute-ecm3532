file(GLOB EI_SDK "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/*.c")
file(GLOB PLATFORM "${CMAKE_CURRENT_LIST_DIR}/ingestion-sdk-platform/eta-compute/*.cpp")
file(GLOB REPL "${CMAKE_CURRENT_LIST_DIR}/repl/*.cpp")
file(GLOB INGESTION "${CMAKE_CURRENT_LIST_DIR}/ingestion-sdk-c/*.cpp")
file(GLOB QCBOR "${CMAKE_CURRENT_LIST_DIR}/QCBOR/src/*.c")
file(GLOB MBEDTLS "${CMAKE_CURRENT_LIST_DIR}/mbedtls_hmac_sha256_sw/mbedtls/src/*.c")
file(GLOB TF "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/tensorflow/lite/micro/*.cc"
             "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/tensorflow/lite/micro/memory_planner/*.cc"
             "${CMAKE_CURRENT_LIST_DIR}/tflite-model/*.cpp")
file(GLOB TFKERNEL "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/tensorflow/lite/micro/kernels/*.cc")
file(GLOB TFCORE "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/tensorflow/lite/core/api/*.cc")
file(GLOB TFCOMMON "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/tensorflow/lite/c/*.c")
file(GLOB TFINTERNAL "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/tensorflow/lite/kernels/internal/*.cc"
                     "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/tensorflow/lite/kernels/*.cc")
file(GLOB EIPORT "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/porting/ecm3532/*.cpp")
file(GLOB DSP "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/dsp/kissfft/*.cpp"
              "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/dsp/dct/*.cpp")
file(GLOB CMSIS "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/CMSIS/DSP/Source/SupportFunctions/*.c"
                "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/CMSIS/DSP/Source/MatrixFunctions/*.c"
                "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/CMSIS/DSP/Source/StatisticsFunctions/*.c"
                "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/CMSIS/DSP/Source/TransformFunctions/*.c"
                "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/CMSIS/DSP/Source/CommonTables/*.c"
                "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/CMSIS/DSP/Source/FastMathFunctions/*.c")
file(GLOB TENSAIMODEL "${CMAKE_CURRENT_LIST_DIR}/tensaiflow-model/*.c")
foreach (ei_sdk ${EI_SDK})
    target_sources(edgeimpulse PRIVATE ${ei_sdk})
endforeach()

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk")

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}")

foreach (ei_platform ${PLATFORM})
    target_sources(edgeimpulse PRIVATE ${ei_platform})
endforeach()

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/ingestion-sdk-platorm/eta-compute")

foreach (ei_repl ${REPL})
    target_sources(edgeimpulse PRIVATE ${ei_repl})
endforeach()

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/repl")

foreach (ei_ingestion ${INGESTION})
    target_sources(edgeimpulse PRIVATE ${ei_ingestion})
endforeach()

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/ingestion-sdk-c")

foreach (ei_qcbor ${QCBOR})
    target_sources(edgeimpulse PUBLIC ${ei_qcbor})
endforeach()

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/QCBOR/inc")

foreach (ei_mbedtls ${MBEDTLS})
    target_sources(edgeimpulse PRIVATE ${ei_mbedtls})
endforeach()

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/mbedtls_hmac_sha256_sw/")

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/model-parameters")

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/tflite-model")

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/dsp")

foreach (ei_tf ${TF})
    target_sources(edgeimpulse PRIVATE ${ei_tf})
endforeach()

foreach (ei_tfkernel ${TFKERNEL})
    target_sources(edgeimpulse PRIVATE ${ei_tfkernel})
endforeach()

foreach (ei_tfcore ${TFCORE})
    target_sources(edgeimpulse PRIVATE ${ei_tfcore})
endforeach()

foreach (ei_tfcommon ${TFCOMMON})
    target_sources(edgeimpulse PRIVATE ${ei_tfcommon})
endforeach()

foreach (ei_tfinternal ${TFINTERNAL})
    target_sources(edgeimpulse PRIVATE ${ei_tfinternal})
endforeach()

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/tensorflow/lite/micro")

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/third_party/flatbuffers/include")

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/third_party/gemmlowp")

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/third_party/ruy")

foreach (ei_port ${EIPORT})
    target_sources(edgeimpulse PRIVATE ${ei_port})
endforeach()

foreach (ei_dsp ${DSP})
    target_sources(edgeimpulse PRIVATE ${ei_dsp})
endforeach()

target_include_directories(edgeimpulse PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/CMSIS/DSP/PrivateInclude")

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/edge-impulse-sdk/CMSIS/DSP/Include")

foreach (ei_cmsis ${CMSIS})
    target_sources(edgeimpulse PRIVATE ${ei_cmsis})
endforeach()

target_include_directories(edgeimpulse PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/tensaiflow-model")

foreach (ei_tensaimodel ${TENSAIMODEL})
    target_sources(edgeimpulse PUBLIC ${ei_tensaimodel})
endforeach()