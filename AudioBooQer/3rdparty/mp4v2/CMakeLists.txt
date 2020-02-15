### Project ##################################################################

list(APPEND bmff_SOURCES
  mp4v2-src/src/bmff/bmff.h
  mp4v2-src/src/bmff/impl.h
  mp4v2-src/src/bmff/typebmff.cpp
  mp4v2-src/src/bmff/typebmff.h
  )

list(APPEND itmf_SOURCE
  mp4v2-src/src/itmf/CoverArtBox.cpp
  mp4v2-src/src/itmf/CoverArtBox.h
  mp4v2-src/src/itmf/Tags.cpp
  mp4v2-src/src/itmf/Tags.h
  mp4v2-src/src/itmf/generic.cpp
  mp4v2-src/src/itmf/generic.h
  mp4v2-src/src/itmf/impl.h
  mp4v2-src/src/itmf/itmf.h
  mp4v2-src/src/itmf/type.cpp
  mp4v2-src/src/itmf/type.h
  )

list(APPEND mp4v2_SOURCES
  mp4v2-src/src/3gp.cpp
  mp4v2-src/src/atom_ac3.cpp
  mp4v2-src/src/atom_amr.cpp
  mp4v2-src/src/atom_avc1.cpp
  mp4v2-src/src/atom_avcC.cpp
  mp4v2-src/src/atom_chpl.cpp
  mp4v2-src/src/atom_colr.cpp
  mp4v2-src/src/atom_d263.cpp
  mp4v2-src/src/atom_dac3.cpp
  mp4v2-src/src/atom_damr.cpp
  mp4v2-src/src/atom_dref.cpp
  mp4v2-src/src/atom_elst.cpp
  mp4v2-src/src/atom_enca.cpp
  mp4v2-src/src/atom_encv.cpp
  mp4v2-src/src/atom_free.cpp
  mp4v2-src/src/atom_ftyp.cpp
  mp4v2-src/src/atom_ftab.cpp
  mp4v2-src/src/atom_gmin.cpp
  mp4v2-src/src/atom_hdlr.cpp
  mp4v2-src/src/atom_hinf.cpp
  mp4v2-src/src/atom_hnti.cpp
  mp4v2-src/src/atom_href.cpp
  mp4v2-src/src/atom_mdat.cpp
  mp4v2-src/src/atom_mdhd.cpp
  mp4v2-src/src/atom_meta.cpp
  mp4v2-src/src/atom_mp4s.cpp
  mp4v2-src/src/atom_mp4v.cpp
  mp4v2-src/src/atom_mvhd.cpp
  mp4v2-src/src/atom_nmhd.cpp
  mp4v2-src/src/atom_ohdr.cpp
  mp4v2-src/src/atom_pasp.cpp
  mp4v2-src/src/atom_root.cpp
  mp4v2-src/src/atom_rtp.cpp
  mp4v2-src/src/atom_s263.cpp
  mp4v2-src/src/atom_sdp.cpp
  mp4v2-src/src/atom_sdtp.cpp
  mp4v2-src/src/atom_smi.cpp
  mp4v2-src/src/atom_sound.cpp
  mp4v2-src/src/atom_standard.cpp
  mp4v2-src/src/atom_stbl.cpp
  mp4v2-src/src/atom_stdp.cpp
  mp4v2-src/src/atom_stsc.cpp
  mp4v2-src/src/atom_stsd.cpp
  mp4v2-src/src/atom_stsz.cpp
  mp4v2-src/src/atom_stz2.cpp
  mp4v2-src/src/atom_text.cpp
  mp4v2-src/src/atom_tfhd.cpp
  mp4v2-src/src/atom_tkhd.cpp
  mp4v2-src/src/atom_treftype.cpp
  mp4v2-src/src/atom_trun.cpp
  mp4v2-src/src/atom_tx3g.cpp
  mp4v2-src/src/atom_udta.cpp
  mp4v2-src/src/atom_url.cpp
  mp4v2-src/src/atom_urn.cpp
  mp4v2-src/src/atom_uuid.cpp
  mp4v2-src/src/atom_video.cpp
  mp4v2-src/src/atom_vmhd.cpp
  mp4v2-src/src/atoms.h
  mp4v2-src/src/cmeta.cpp
  mp4v2-src/src/descriptors.cpp
  mp4v2-src/src/descriptors.h
  mp4v2-src/src/exception.cpp
  mp4v2-src/src/exception.h
  mp4v2-src/src/enum.h
  mp4v2-src/src/enum.tcc
  mp4v2-src/src/impl.h
  mp4v2-src/src/isma.cpp
  mp4v2-src/src/log.h
  mp4v2-src/src/log.cpp
  mp4v2-src/src/mp4.cpp
  mp4v2-src/src/mp4array.h
  mp4v2-src/src/mp4atom.cpp
  mp4v2-src/src/mp4atom.h
  mp4v2-src/src/mp4container.cpp
  mp4v2-src/src/mp4container.h
  mp4v2-src/src/mp4descriptor.cpp
  mp4v2-src/src/mp4descriptor.h
  mp4v2-src/src/mp4file.cpp
  mp4v2-src/src/mp4file.h
  mp4v2-src/src/mp4file_io.cpp
  mp4v2-src/src/mp4info.cpp
  mp4v2-src/src/mp4property.cpp
  mp4v2-src/src/mp4property.h
  mp4v2-src/src/mp4track.cpp
  mp4v2-src/src/mp4track.h
  mp4v2-src/src/mp4util.cpp
  mp4v2-src/src/mp4util.h
  mp4v2-src/src/ocidescriptors.cpp
  mp4v2-src/src/ocidescriptors.h
  mp4v2-src/src/odcommands.cpp
  mp4v2-src/src/odcommands.h
  mp4v2-src/src/qosqualifiers.cpp
  mp4v2-src/src/qosqualifiers.h
  mp4v2-src/src/rtphint.cpp
  mp4v2-src/src/rtphint.h
  mp4v2-src/src/src.h
  mp4v2-src/src/text.cpp
  mp4v2-src/src/text.h
  mp4v2-src/src/util.h
  )

list(APPEND platform_SOURCES
  mp4v2-src/libplatform/endian.h
  mp4v2-src/libplatform/impl.h
  mp4v2-src/libplatform/io/File.cpp
  mp4v2-src/libplatform/io/File.h
  mp4v2-src/libplatform/io/FileSystem.cpp
  mp4v2-src/libplatform/io/FileSystem.h
  mp4v2-src/libplatform/number/random.h
  mp4v2-src/libplatform/platform.h
  mp4v2-src/libplatform/platform_base.h
  mp4v2-src/libplatform/platform_posix.h
  mp4v2-src/libplatform/platform_win32.h
  mp4v2-src/libplatform/process/process.h
  mp4v2-src/libplatform/prog/option.cpp
  mp4v2-src/libplatform/prog/option.h
  mp4v2-src/libplatform/sys/error.cpp
  mp4v2-src/libplatform/sys/error.h
  mp4v2-src/libplatform/time/time.cpp
  mp4v2-src/libplatform/time/time.h
  mp4v2-src/libplatform/warning.h
  )

if(WIN32)
  list(APPEND platform_SOURCES
    mp4v2-src/libplatform/io/File_win32.cpp
    mp4v2-src/libplatform/io/FileSystem_win32.cpp
    mp4v2-src/libplatform/number/random_win32.cpp
    mp4v2-src/libplatform/process/process_win32.cpp
    mp4v2-src/libplatform/time/time_win32.cpp
	mp4v2-src/libplatform/platform_win32.cpp
    )
endif()

list(APPEND qtff_SOURCES
  mp4v2-src/src/qtff/ColorParameterBox.cpp
  mp4v2-src/src/qtff/ColorParameterBox.h
  mp4v2-src/src/qtff/PictureAspectRatioBox.cpp
  mp4v2-src/src/qtff/PictureAspectRatioBox.h
  mp4v2-src/src/qtff/coding.cpp
  mp4v2-src/src/qtff/coding.h
  mp4v2-src/src/qtff/impl.h
  mp4v2-src/src/qtff/qtff.h
  )

list(APPEND util_SOURCES
  mp4v2-src/libutil/Database.cpp
  mp4v2-src/libutil/Database.h
  mp4v2-src/libutil/Timecode.cpp
  mp4v2-src/libutil/Timecode.h
  mp4v2-src/libutil/TrackModifier.cpp
  mp4v2-src/libutil/TrackModifier.h
  mp4v2-src/libutil/Utility.cpp
  mp4v2-src/libutil/Utility.h
  mp4v2-src/libutil/crc.cpp
  mp4v2-src/libutil/crc.h
  mp4v2-src/libutil/impl.h
  mp4v2-src/libutil/other.cpp
  mp4v2-src/libutil/other.h
  mp4v2-src/libutil/util.h
  )

### Target ###################################################################

add_library(mp4v2 STATIC
  ${bmff_SOURCES}
  ${itmf_SOURCE}
  ${mp4v2_SOURCES}
  ${platform_SOURCES}
  ${qtff_SOURCES}
  ${util_SOURCES}
  )

format_output_name(mp4v2 "mp4v2")

target_link_libraries(mp4v2)

target_compile_definitions(mp4v2
  PUBLIC MP4V2_USE_STATIC_LIB
  )

target_include_directories(mp4v2
  PRIVATE mp4v2-src
  PUBLIC  mp4v2-src/include
  )

### Macros ###################################################################

macro(mp4_test target)
  set(sources ${ARGN})
  add_executable(${target} ${sources})
  format_output_name(${target} ${target})
  target_compile_definitions(${target} PRIVATE
    _CRT_SECURE_NO_WARNINGS
    )
  target_link_libraries(${target} mp4v2)
endmacro()

### Tests ####################################################################

mp4_test(mp4create src/mp4create.cpp)
mp4_test(mp4info src/mp4info.cpp)
