
ecm_create_qm_loader(kcodecs_QM_LOADER kcodecs5)

set(kcodecs_SRCS
    kcharsets.cpp
    kcodecs.cpp
    kencodingprober.cpp
    probers/CharDistribution.cpp
    probers/ChineseGroupProber.cpp
    probers/JapaneseGroupProber.cpp
    probers/JpCntx.cpp
    probers/LangBulgarianModel.cpp
    probers/LangCyrillicModel.cpp
    probers/LangGreekModel.cpp
    probers/LangHebrewModel.cpp
    probers/LangHungarianModel.cpp
    probers/LangThaiModel.cpp
    probers/UnicodeGroupProber.cpp
    probers/nsBig5Prober.cpp
    probers/nsCharSetProber.cpp
    probers/nsEUCJPProber.cpp
    probers/nsEUCKRProber.cpp
    probers/nsEUCTWProber.cpp
    probers/nsEscCharsetProber.cpp
    probers/nsEscSM.cpp
    probers/nsGB2312Prober.cpp
    probers/nsHebrewProber.cpp
    probers/nsLatin1Prober.cpp
    probers/nsMBCSGroupProber.cpp
    probers/nsMBCSSM.cpp
    probers/nsSBCSGroupProber.cpp
    probers/nsSBCharSetProber.cpp
    probers/nsSJISProber.cpp
    probers/nsUniversalDetector.cpp
  ${kcodecs_QM_LOADER}
)

add_library(KF5Codecs ${kcodecs_SRCS})
generate_export_header(KF5Codecs BASE_NAME KCodecs)
add_library(KF5::Codecs ALIAS KF5Codecs)

target_include_directories(KF5Codecs INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/KCodecs>")

target_link_libraries(KF5Codecs PUBLIC Qt5::Core)

set_target_properties(KF5Codecs PROPERTIES VERSION   ${KCODECS_VERSION_STRING}
                                           SOVERSION ${KCODECS_SOVERSION}
                                           EXPORT_NAME Codecs
)

ecm_generate_headers(KCodecs_HEADERS
  HEADER_NAMES
  KCharsets
  KCodecs
  KEncodingProber

  REQUIRED_HEADERS KCodecs_HEADERS
)

install(TARGETS KF5Codecs EXPORT KF5CodecsTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kcodecs_export.h
  ${KCodecs_HEADERS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/KCodecs COMPONENT Devel
)

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KCodecs LIB_NAME KF5Codecs DEPS "core" FILENAME_VAR PRI_FILENAME)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
