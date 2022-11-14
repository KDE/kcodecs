add_library(KF5Codecs)
add_library(KF5::Codecs ALIAS KF5Codecs)

ecm_create_qm_loader(KF5Codecs kcodecs5_qt)

ecm_gperf_generate(kentities.gperf ${CMAKE_CURRENT_BINARY_DIR}/kentities.h KF5Codecs
                   GENERATION_FLAGS "--key-positions=* -D -s 2")

# gperf < 3.1 always adds a "register" keyword to function arguments,
# which itself is removed from C++17,
# so sources including the generated header need to use C++14 only.
# Only kcharsets.cpp is affected, but right now nothing else needs C++17,
# so simply set standard for the while library
# Current version of FindGperf in ECM does not support a version check yet,
# so for now always lower to C++14.
set_target_properties(KF5Codecs PROPERTIES CXX_STANDARD 14)

target_sources(KF5Codecs PRIVATE
    kcharsets.cpp
    kcharsets.h
    kcodecsbase45.cpp
    kcodecsbase64.cpp
    kcodecsbase64.h
    kcodecs.cpp
    kcodecs.h
    kcodecsidentity.cpp
    kcodecsidentity.h
    kcodecs_p.h
    kcodecsqp.cpp
    kcodecsqp.h
    kcodecsuuencode.cpp
    kcodecsuuencode.h
    kemailaddress.cpp
    kemailaddress.h
    kencodingprober.cpp
    kencodingprober.h
    kusasciitextcodec.cpp
    kusasciitextcodec.h
    probers/CharDistribution.cpp
    probers/CharDistribution.h
    probers/ChineseGroupProber.cpp
    probers/ChineseGroupProber.h
    probers/JapaneseGroupProber.cpp
    probers/JapaneseGroupProber.h
    probers/JpCntx.cpp
    probers/JpCntx.h
    probers/LangBulgarianModel.cpp
    probers/LangCyrillicModel.cpp
    probers/LangGreekModel.cpp
    probers/LangHebrewModel.cpp
    probers/LangHungarianModel.cpp
    probers/LangThaiModel.cpp
    probers/nsBig5Prober.cpp
    probers/nsBig5Prober.h
    probers/nsCharSetProber.cpp
    probers/nsCharSetProber.h
    probers/nsCodingStateMachine.h
    probers/nsEscCharsetProber.cpp
    probers/nsEscCharsetProber.h
    probers/nsEscSM.cpp
    probers/nsEUCJPProber.cpp
    probers/nsEUCJPProber.h
    probers/nsEUCKRProber.cpp
    probers/nsEUCKRProber.h
    probers/nsGB2312Prober.cpp
    probers/nsGB2312Prober.h
    probers/nsHebrewProber.cpp
    probers/nsHebrewProber.h
    probers/nsLatin1Prober.cpp
    probers/nsLatin1Prober.h
    probers/nsMBCSGroupProber.cpp
    probers/nsMBCSGroupProber.h
    probers/nsMBCSSM.cpp
    probers/nsPkgInt.h
    probers/nsSBCharSetProber.cpp
    probers/nsSBCharSetProber.h
    probers/nsSBCSGroupProber.cpp
    probers/nsSBCSGroupProber.h
    probers/nsSJISProber.cpp
    probers/nsSJISProber.h
    probers/nsUniversalDetector.cpp
    probers/nsUniversalDetector.h
    probers/UnicodeGroupProber.cpp
    probers/UnicodeGroupProber.h
)

ecm_qt_declare_logging_category(KF5Codecs
    HEADER kcodecs_debug.h
    IDENTIFIER KCODECS_LOG
    CATEGORY_NAME kf.codecs
    OLD_CATEGORY_NAMES kf5.kcodecs
    DESCRIPTION "KCodecs"
    EXPORT KCODECS
)

ecm_generate_export_header(KF5Codecs
    BASE_NAME KCodecs
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS 5.5 5.56 5.101
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

target_include_directories(KF5Codecs INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/KCodecs>")

target_link_libraries(KF5Codecs PUBLIC Qt${QT_MAJOR_VERSION}::Core)
if (TARGET Qt6::Core5Compat)
    target_link_libraries(KF5Codecs PUBLIC Qt6::Core5Compat)  # QTextCodec
endif()

set_target_properties(KF5Codecs PROPERTIES VERSION   ${KCODECS_VERSION}
                                           SOVERSION ${KCODECS_SOVERSION}
                                           EXPORT_NAME Codecs
)

ecm_generate_headers(KCodecs_HEADERS
  HEADER_NAMES
  KCharsets
  KCodecs
  KEncodingProber
  KEmailAddress
  REQUIRED_HEADERS KCodecs_HEADERS
)

install(TARGETS KF5Codecs EXPORT KF5CodecsTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kcodecs_export.h
  ${KCodecs_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KCodecs COMPONENT Devel
)

ecm_qt_install_logging_categories(
    EXPORT KCODECS
    FILE kcodecs.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

if(BUILD_QCH)
    ecm_add_qch(
        KF5Codecs_QCH
        NAME KCodecs
        BASE_NAME KF5Codecs
        VERSION ${KF_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${KCodecs_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt5Core_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            KCODECS_EXPORT
            KCODECS_DEPRECATED
            KCODECS_DEPRECATED_EXPORT
            "KCODECS_DEPRECATED_VERSION(x, y, t)"
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KCodecs LIB_NAME KF5Codecs DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/KCodecs)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
