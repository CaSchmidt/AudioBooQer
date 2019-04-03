list(APPEND AACDEC_SRC
  fdk-aac-src/libAACdec/src/FDK_delay.cpp
  fdk-aac-src/libAACdec/src/aac_ram.cpp
  fdk-aac-src/libAACdec/src/aac_rom.cpp
  fdk-aac-src/libAACdec/src/aacdec_drc.cpp
  fdk-aac-src/libAACdec/src/aacdec_hcr.cpp
  fdk-aac-src/libAACdec/src/aacdec_hcr_bit.cpp
  fdk-aac-src/libAACdec/src/aacdec_hcrs.cpp
  fdk-aac-src/libAACdec/src/aacdec_pns.cpp
  fdk-aac-src/libAACdec/src/aacdec_tns.cpp
  fdk-aac-src/libAACdec/src/aacdecoder.cpp
  fdk-aac-src/libAACdec/src/aacdecoder_lib.cpp
  fdk-aac-src/libAACdec/src/block.cpp
  fdk-aac-src/libAACdec/src/channel.cpp
  fdk-aac-src/libAACdec/src/channelinfo.cpp
  fdk-aac-src/libAACdec/src/conceal.cpp
  fdk-aac-src/libAACdec/src/ldfiltbank.cpp
  fdk-aac-src/libAACdec/src/pulsedata.cpp
  fdk-aac-src/libAACdec/src/rvlc.cpp
  fdk-aac-src/libAACdec/src/rvlcbit.cpp
  fdk-aac-src/libAACdec/src/rvlcconceal.cpp
  fdk-aac-src/libAACdec/src/stereo.cpp
  fdk-aac-src/libAACdec/src/usacdec_ace_d4t64.cpp
  fdk-aac-src/libAACdec/src/usacdec_ace_ltp.cpp
  fdk-aac-src/libAACdec/src/usacdec_acelp.cpp
  fdk-aac-src/libAACdec/src/usacdec_fac.cpp
  fdk-aac-src/libAACdec/src/usacdec_lpc.cpp
  fdk-aac-src/libAACdec/src/usacdec_lpd.cpp
  fdk-aac-src/libAACdec/src/usacdec_rom.cpp
  )

list(APPEND AACENC_SRC
  fdk-aac-src/libAACenc/src/aacEnc_ram.cpp
  fdk-aac-src/libAACenc/src/aacEnc_rom.cpp
  fdk-aac-src/libAACenc/src/aacenc.cpp
  fdk-aac-src/libAACenc/src/aacenc_lib.cpp
  fdk-aac-src/libAACenc/src/aacenc_pns.cpp
  fdk-aac-src/libAACenc/src/aacenc_tns.cpp
  fdk-aac-src/libAACenc/src/adj_thr.cpp
  fdk-aac-src/libAACenc/src/band_nrg.cpp
  fdk-aac-src/libAACenc/src/bandwidth.cpp
  fdk-aac-src/libAACenc/src/bit_cnt.cpp
  fdk-aac-src/libAACenc/src/bitenc.cpp
  fdk-aac-src/libAACenc/src/block_switch.cpp
  fdk-aac-src/libAACenc/src/channel_map.cpp
  fdk-aac-src/libAACenc/src/chaosmeasure.cpp
  fdk-aac-src/libAACenc/src/dyn_bits.cpp
  fdk-aac-src/libAACenc/src/grp_data.cpp
  fdk-aac-src/libAACenc/src/intensity.cpp
  fdk-aac-src/libAACenc/src/line_pe.cpp
  fdk-aac-src/libAACenc/src/metadata_compressor.cpp
  fdk-aac-src/libAACenc/src/metadata_main.cpp
  fdk-aac-src/libAACenc/src/mps_main.cpp
  fdk-aac-src/libAACenc/src/ms_stereo.cpp
  fdk-aac-src/libAACenc/src/noisedet.cpp
  fdk-aac-src/libAACenc/src/pnsparam.cpp
  fdk-aac-src/libAACenc/src/pre_echo_control.cpp
  fdk-aac-src/libAACenc/src/psy_configuration.cpp
  fdk-aac-src/libAACenc/src/psy_main.cpp
  fdk-aac-src/libAACenc/src/qc_main.cpp
  fdk-aac-src/libAACenc/src/quantize.cpp
  fdk-aac-src/libAACenc/src/sf_estim.cpp
  fdk-aac-src/libAACenc/src/spreading.cpp
  fdk-aac-src/libAACenc/src/tonality.cpp
  fdk-aac-src/libAACenc/src/transform.cpp
  )

list(APPEND ARITHCODING_SRC
  fdk-aac-src/libArithCoding/src/ac_arith_coder.cpp
  )

list(APPEND DRCDEC_SRC
  fdk-aac-src/libDRCdec/src/FDK_drcDecLib.cpp
  fdk-aac-src/libDRCdec/src/drcDec_gainDecoder.cpp
  fdk-aac-src/libDRCdec/src/drcDec_reader.cpp
  fdk-aac-src/libDRCdec/src/drcDec_rom.cpp
  fdk-aac-src/libDRCdec/src/drcDec_selectionProcess.cpp
  fdk-aac-src/libDRCdec/src/drcDec_tools.cpp
  fdk-aac-src/libDRCdec/src/drcGainDec_init.cpp
  fdk-aac-src/libDRCdec/src/drcGainDec_preprocess.cpp
  fdk-aac-src/libDRCdec/src/drcGainDec_process.cpp
  )

list(APPEND FDK_SRC
  fdk-aac-src/libFDK/src/FDK_bitbuffer.cpp
  fdk-aac-src/libFDK/src/FDK_core.cpp
  fdk-aac-src/libFDK/src/FDK_crc.cpp
  fdk-aac-src/libFDK/src/FDK_decorrelate.cpp
  fdk-aac-src/libFDK/src/FDK_hybrid.cpp
  fdk-aac-src/libFDK/src/FDK_lpc.cpp
  fdk-aac-src/libFDK/src/FDK_matrixCalloc.cpp
  fdk-aac-src/libFDK/src/FDK_qmf_domain.cpp
  fdk-aac-src/libFDK/src/FDK_tools_rom.cpp
  fdk-aac-src/libFDK/src/FDK_trigFcts.cpp
  fdk-aac-src/libFDK/src/autocorr2nd.cpp
  fdk-aac-src/libFDK/src/dct.cpp
  fdk-aac-src/libFDK/src/fft.cpp
  fdk-aac-src/libFDK/src/fft_rad2.cpp
  fdk-aac-src/libFDK/src/fixpoint_math.cpp
  fdk-aac-src/libFDK/src/huff_nodes.cpp
  fdk-aac-src/libFDK/src/mdct.cpp
  fdk-aac-src/libFDK/src/nlc_dec.cpp
  fdk-aac-src/libFDK/src/qmf.cpp
  fdk-aac-src/libFDK/src/scale.cpp
  )

list(APPEND MPEGTPDEC_SRC
  fdk-aac-src/libMpegTPDec/src/tpdec_adif.cpp
  fdk-aac-src/libMpegTPDec/src/tpdec_adts.cpp
  fdk-aac-src/libMpegTPDec/src/tpdec_asc.cpp
  fdk-aac-src/libMpegTPDec/src/tpdec_drm.cpp
  fdk-aac-src/libMpegTPDec/src/tpdec_latm.cpp
  fdk-aac-src/libMpegTPDec/src/tpdec_lib.cpp
  )

list(APPEND MPEGTPENC_SRC
  fdk-aac-src/libMpegTPEnc/src/tpenc_adif.cpp
  fdk-aac-src/libMpegTPEnc/src/tpenc_adts.cpp
  fdk-aac-src/libMpegTPEnc/src/tpenc_asc.cpp
  fdk-aac-src/libMpegTPEnc/src/tpenc_latm.cpp
  fdk-aac-src/libMpegTPEnc/src/tpenc_lib.cpp
  )

list(APPEND PCMUTILS_SRC
  fdk-aac-src/libPCMutils/src/limiter.cpp
  fdk-aac-src/libPCMutils/src/pcm_utils.cpp
  fdk-aac-src/libPCMutils/src/pcmdmx_lib.cpp
  )

list(APPEND SACDEC_SRC
  fdk-aac-src/libSACdec/src/sac_bitdec.cpp
  fdk-aac-src/libSACdec/src/sac_calcM1andM2.cpp
  fdk-aac-src/libSACdec/src/sac_dec.cpp
  fdk-aac-src/libSACdec/src/sac_dec_conceal.cpp
  fdk-aac-src/libSACdec/src/sac_dec_lib.cpp
  fdk-aac-src/libSACdec/src/sac_process.cpp
  fdk-aac-src/libSACdec/src/sac_qmf.cpp
  fdk-aac-src/libSACdec/src/sac_reshapeBBEnv.cpp
  fdk-aac-src/libSACdec/src/sac_rom.cpp
  fdk-aac-src/libSACdec/src/sac_smoothing.cpp
  fdk-aac-src/libSACdec/src/sac_stp.cpp
  fdk-aac-src/libSACdec/src/sac_tsd.cpp
  )

list(APPEND SACENC_SRC
  fdk-aac-src/libSACenc/src/sacenc_bitstream.cpp
  fdk-aac-src/libSACenc/src/sacenc_delay.cpp
  fdk-aac-src/libSACenc/src/sacenc_dmx_tdom_enh.cpp
  fdk-aac-src/libSACenc/src/sacenc_filter.cpp
  fdk-aac-src/libSACenc/src/sacenc_framewindowing.cpp
  fdk-aac-src/libSACenc/src/sacenc_huff_tab.cpp
  fdk-aac-src/libSACenc/src/sacenc_lib.cpp
  fdk-aac-src/libSACenc/src/sacenc_nlc_enc.cpp
  fdk-aac-src/libSACenc/src/sacenc_onsetdetect.cpp
  fdk-aac-src/libSACenc/src/sacenc_paramextract.cpp
  fdk-aac-src/libSACenc/src/sacenc_staticgain.cpp
  fdk-aac-src/libSACenc/src/sacenc_tree.cpp
  fdk-aac-src/libSACenc/src/sacenc_vectorfunctions.cpp
  )

list(APPEND SBRDEC_SRC
  fdk-aac-src/libSBRdec/src/HFgen_preFlat.cpp
  fdk-aac-src/libSBRdec/src/env_calc.cpp
  fdk-aac-src/libSBRdec/src/env_dec.cpp
  fdk-aac-src/libSBRdec/src/env_extr.cpp
  fdk-aac-src/libSBRdec/src/hbe.cpp
  fdk-aac-src/libSBRdec/src/huff_dec.cpp
  fdk-aac-src/libSBRdec/src/lpp_tran.cpp
  fdk-aac-src/libSBRdec/src/psbitdec.cpp
  fdk-aac-src/libSBRdec/src/psdec.cpp
  fdk-aac-src/libSBRdec/src/psdec_drm.cpp
  fdk-aac-src/libSBRdec/src/psdecrom_drm.cpp
  fdk-aac-src/libSBRdec/src/pvc_dec.cpp
  fdk-aac-src/libSBRdec/src/sbr_crc.cpp
  fdk-aac-src/libSBRdec/src/sbr_deb.cpp
  fdk-aac-src/libSBRdec/src/sbr_dec.cpp
  fdk-aac-src/libSBRdec/src/sbr_ram.cpp
  fdk-aac-src/libSBRdec/src/sbr_rom.cpp
  fdk-aac-src/libSBRdec/src/sbrdec_drc.cpp
  fdk-aac-src/libSBRdec/src/sbrdec_freq_sca.cpp
  fdk-aac-src/libSBRdec/src/sbrdecoder.cpp
  )

list(APPEND SBRENC_SRC
  fdk-aac-src/libSBRenc/src/bit_sbr.cpp
  fdk-aac-src/libSBRenc/src/code_env.cpp
  fdk-aac-src/libSBRenc/src/env_bit.cpp
  fdk-aac-src/libSBRenc/src/env_est.cpp
  fdk-aac-src/libSBRenc/src/fram_gen.cpp
  fdk-aac-src/libSBRenc/src/invf_est.cpp
  fdk-aac-src/libSBRenc/src/mh_det.cpp
  fdk-aac-src/libSBRenc/src/nf_est.cpp
  fdk-aac-src/libSBRenc/src/ps_bitenc.cpp
  fdk-aac-src/libSBRenc/src/ps_encode.cpp
  fdk-aac-src/libSBRenc/src/ps_main.cpp
  fdk-aac-src/libSBRenc/src/resampler.cpp
  fdk-aac-src/libSBRenc/src/sbr_encoder.cpp
  fdk-aac-src/libSBRenc/src/sbr_misc.cpp
  fdk-aac-src/libSBRenc/src/sbrenc_freq_sca.cpp
  fdk-aac-src/libSBRenc/src/sbrenc_ram.cpp
  fdk-aac-src/libSBRenc/src/sbrenc_rom.cpp
  fdk-aac-src/libSBRenc/src/ton_corr.cpp
  fdk-aac-src/libSBRenc/src/tran_det.cpp
  )

list(APPEND SYS_SRC
  fdk-aac-src/libSYS/src/genericStds.cpp
  fdk-aac-src/libSYS/src/syslib_channelMapDescr.cpp
  )

add_library(fdk-aac STATIC
  ${AACDEC_SRC}
  ${AACENC_SRC}
  ${ARITHCODING_SRC}
  ${DRCDEC_SRC}
  ${FDK_SRC}
  ${MPEGTPDEC_SRC}
  ${MPEGTPENC_SRC}
  ${PCMUTILS_SRC}
  ${SACDEC_SRC}
  ${SACENC_SRC}
  ${SBRDEC_SRC}
  ${SBRENC_SRC}
  ${SYS_SRC}
  )

target_link_libraries(fdk-aac)

target_include_directories(fdk-aac PRIVATE
  fdk-aac-src/libAACdec/include
  fdk-aac-src/libAACenc/include
  fdk-aac-src/libArithCoding/include
  fdk-aac-src/libDRCdec/include
  fdk-aac-src/libFDK/include
  fdk-aac-src/libMpegTPDec/include
  fdk-aac-src/libMpegTPEnc/include
  fdk-aac-src/libPCMutils/include
  fdk-aac-src/libSACdec/include
  fdk-aac-src/libSACenc/include
  fdk-aac-src/libSBRdec/include
  fdk-aac-src/libSBRenc/include
  fdk-aac-src/libSYS/include
  )
