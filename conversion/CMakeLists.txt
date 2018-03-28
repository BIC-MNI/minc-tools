# CMakeFiles.txt for the MINC2 conversion progs
#
# Andrew Janke - a.janke@gmail.com

#LINK_DIRECTORIES()

LINK_LIBRARIES( ${LIBMINC_LIBRARIES})

ADD_DEFINITIONS(-DHAVE_CONFIG_H)

IF(JPEG_FOUND)
  INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR})
ENDIF(JPEG_FOUND)

IF(OPENJPEG_FOUND)
  INCLUDE_DIRECTORIES(${OPENJPEG_INCLUDE_DIR})
ENDIF(OPENJPEG_FOUND)


# conversion progs (and libraries)
INCLUDE_DIRECTORIES(Acr_nema)
ADD_LIBRARY(acr_nema STATIC 
   Acr_nema/acr_io.c
   Acr_nema/dicom_client_routines.c
   Acr_nema/dicom_network.c
   Acr_nema/element.c
   Acr_nema/file_io.c
   Acr_nema/globals.c
   Acr_nema/group.c
   Acr_nema/message.c
   Acr_nema/value_repr.c
   Acr_nema/dicom_tags.c)

ADD_EXECUTABLE(dcm2mnc
   dcm2mnc/dcm2mnc.c
   dcm2mnc/dicom_to_minc.c
   dcm2mnc/siemens_to_dicom.c
   dcm2mnc/dicom_read.c
   dcm2mnc/minc_file.c
   dcm2mnc/progress.c
   dcm2mnc/jpg_0xc3.c
   dcm2mnc/string_to_filename.c)
   
TARGET_LINK_LIBRARIES(dcm2mnc acr_nema)
  
IF(JPEG_FOUND)
  TARGET_LINK_LIBRARIES(dcm2mnc ${JPEG_LIBRARIES})
ENDIF(JPEG_FOUND)

IF(OPENJPEG_FOUND)
  # in case openjpeg was compiled with thread support....
  #SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${CMAKE_THREAD_LIBS_INIT}")
  TARGET_LINK_LIBRARIES(dcm2mnc ${OPENJPEG_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
  # ${CMAKE_THREAD_LIBS_INIT}
ENDIF(OPENJPEG_FOUND)

ADD_EXECUTABLE(ecattominc
   ecattominc/ecattominc.c
   ecattominc/insertblood.c
   ecattominc/ecat_file.c
   ecattominc/machine_indep.c
   )
ADD_EXECUTABLE(minctoecat
   minctoecat/minctoecat.c
   minctoecat/ecat_write.c
   minctoecat/machine_indep.c
   )

TARGET_LINK_LIBRARIES(minctoecat ${VOLUME_IO_LIBRARIES} ${LIBMINC_LIBRARIES} m)


IF(NIFTI_FOUND)
  INCLUDE_DIRECTORIES(${NIFTI_INCLUDE_DIR} ${ZNZ_INCLUDE_DIR})
  
  ADD_EXECUTABLE(mnc2nii
    nifti1/mnc2nii.c
    )
  TARGET_LINK_LIBRARIES(mnc2nii ${VOLUME_IO_LIBRARIES} ${LIBMINC_LIBRARIES} ${NIFTI_LIBRARY} ${ZNZ_LIBRARY} ${ZLIB_LIBRARY} m)

  ADD_EXECUTABLE(nii2mnc
    nifti1/nii2mnc.c
    )
  TARGET_LINK_LIBRARIES(nii2mnc ${VOLUME_IO_LIBRARIES} ${LIBMINC_LIBRARIES} ${NIFTI_LIBRARY} ${ZNZ_LIBRARY} ${ZLIB_LIBRARY} m)

  INSTALL(TARGETS
    mnc2nii
    nii2mnc
    DESTINATION bin)
  
  INSTALL_MAN_PAGES( ${CMAKE_INSTALL_PREFIX}/man
    nifti1/mnc2nii.man1
    nifti1/nii2mnc.man1
  )
ENDIF(NIFTI_FOUND)

ADD_EXECUTABLE(upet2mnc
   micropet/upet2mnc.c
   )

IF(BUILD_MINC2)

  ADD_EXECUTABLE(vff2mnc
    vff2mnc/vff2mnc.c
    )
  INSTALL(TARGETS
    vff2mnc
    DESTINATION bin)

ENDIF(BUILD_MINC2)


# install progs
INSTALL(TARGETS
   dcm2mnc
   ecattominc
   minctoecat
   upet2mnc
   DESTINATION bin)


INSTALL_MAN_PAGES( ${CMAKE_INSTALL_PREFIX}/man
  dcm2mnc/dcm2mnc.man1
  ecattominc/ecattominc.man1
  micropet/upet2mnc.man1
  minctoecat/minctoecat.man1
  vff2mnc/vff2mnc.man1
)
