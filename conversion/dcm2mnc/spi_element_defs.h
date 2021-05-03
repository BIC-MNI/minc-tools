/* ----------------------------- MNI Header -----------------------------------
@NAME       : spi_element_defs.h
@DESCRIPTION: Element definitions for Siemens "Standard Product Interconnect" 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 23, 1993 (Peter Neelin)
@MODIFIED   : 
@COPYRIGHT  :
              Copyright 1993 Peter Neelin, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

/* Element id's for SPI */
/* Most of this information is available at David Clunie's medical imaging 
 * website (www.dclunie.com).
 */
GLOBAL_ELEMENT(SPI_PMS_grp19_tag                      , 0x0019, 0x0010, CS);

GLOBAL_ELEMENT(SPI_PMS_field_of_view                  , 0x0019, 0x1000, DS);

GLOBAL_ELEMENT(SPI_PMS_cc_angle                       , 0x0019, 0x1005, DS);
GLOBAL_ELEMENT(SPI_PMS_ap_angle                       , 0x0019, 0x1006, DS);
GLOBAL_ELEMENT(SPI_PMS_lr_angle                       , 0x0019, 0x1007, DS);

/* 0 undefined, 1 head first, 2 feet first */
GLOBAL_ELEMENT(SPI_PMS_patient_position               , 0x0019, 0x1008, CS);

/* 0 undefined, 1 supine, 2 prone, 3 left decubitus, 4 right decubitus */
GLOBAL_ELEMENT(SPI_PMS_patient_orientation            , 0x0019, 0x1009, CS);

/* 0 undefined 1 transverse 2 sagittal 3 coronal */
GLOBAL_ELEMENT(SPI_PMS_slice_orientation              , 0x0019, 0x100a, CS);

/* caudal-cranial position (MINC Z) */
GLOBAL_ELEMENT(SPI_PMS_cc_position                    , 0x0019, 0x100b, DS);

/* anterior-posterior position (MINC Y) */
GLOBAL_ELEMENT(SPI_PMS_ap_position                    , 0x0019, 0x100c, DS);

/* left-right position (MINC X) */
GLOBAL_ELEMENT(SPI_PMS_lr_position                    , 0x0019, 0x100d, DS);

GLOBAL_ELEMENT(SPI_PMS_slice_count                    , 0x0019, 0x100f, IS);

GLOBAL_ELEMENT(SPI_PMS_flip_angle                     , 0x0019, 0x101a, DS);

/* caudal-cranial position (MINC X) */
GLOBAL_ELEMENT(SPI_PMS_lr_position2                   , 0x0019, 0x110b, DS);

/* anterior-posterior position (MINC Z) */
GLOBAL_ELEMENT(SPI_PMS_cc_position2                   , 0x0019, 0x110c, DS);

/* left-right position (MINC Z) */
GLOBAL_ELEMENT(SPI_PMS_ap_position2                   , 0x0019, 0x110d, DS);

/* These are the actual Siemens values. */
GLOBAL_ELEMENT(SPI_Private_creator                    , 0x0019, 0x0010, LO);
GLOBAL_ELEMENT(SPI_Image_header_type                  , 0x0019, 0x1008, CS);
GLOBAL_ELEMENT(SPI_Diffusion_b_value                  , 0x0019, 0x100c, IS);
GLOBAL_ELEMENT(SPI_Diffusion_gradient_direction       , 0x0019, 0x100e, FD);
GLOBAL_ELEMENT(SPI_B_matrix                           , 0x0019, 0x1027, FD); /*added by ilana, seems to exits for software >= VB*/

GLOBAL_ELEMENT(SPI_Number_of_data_bytes               , 0x0019, 0x1060, IS);
GLOBAL_ELEMENT(SPI_Fourier_lines_nominal              , 0x0019, 0x1220, IS);
GLOBAL_ELEMENT(SPI_Fourier_lines_after_zero           , 0x0019, 0x1226, IS);
GLOBAL_ELEMENT(SPI_First_measured_fourier_line        , 0x0019, 0x1228, IS);
GLOBAL_ELEMENT(SPI_Acquisition_columns                , 0x0019, 0x1230, LO);
GLOBAL_ELEMENT(SPI_Reconstruction_columns             , 0x0019, 0x1231, LO);
GLOBAL_ELEMENT(SPI_Number_of_averages                 , 0x0019, 0x1250, IS);
GLOBAL_ELEMENT(SPI_Flip_angle                         , 0x0019, 0x1260, DS);
GLOBAL_ELEMENT(SPI_Number_of_prescans                 , 0x0019, 0x1270, IS);
GLOBAL_ELEMENT(SPI_Saturation_regions                 , 0x0019, 0x1290, IS);
GLOBAL_ELEMENT(SPI_Magnetic_field_strength            , 0x0019, 0x1412, DS);
GLOBAL_ELEMENT(SPI_Base_raw_matrix_size               , 0x0019, 0x14d4, IS);

GLOBAL_ELEMENT(SPI_Private_creator_0021               , 0x0021, 0x0011, LO);
GLOBAL_ELEMENT(SPI_ICE_Dims                           , 0x0021, 0x1106, LO);
GLOBAL_ELEMENT(SPI_Field_of_view                      , 0x0021, 0x1120, DS);
GLOBAL_ELEMENT(SPI_Image_magnification_factor         , 0x0021, 0x1122, DS);
GLOBAL_ELEMENT(SPI_View_direction                     , 0x0021, 0x1130, CS);
GLOBAL_ELEMENT(SPI_Rest_direction                     , 0x0021, 0x1132, CS);
GLOBAL_ELEMENT(SPI_Image_position                     , 0x0021, 0x1160, DS);
GLOBAL_ELEMENT(SPI_Image_normal                       , 0x0021, 0x1161, DS);
GLOBAL_ELEMENT(SPI_Image_distance                     , 0x0021, 0x1163, DS);
GLOBAL_ELEMENT(SPI_Image_row                          , 0x0021, 0x116a, DS);
GLOBAL_ELEMENT(SPI_Image_column                       , 0x0021, 0x116b, DS);
GLOBAL_ELEMENT(SPI_Patient_orientation_set1           , 0x0021, 0x1170, CS);
GLOBAL_ELEMENT(SPI_Patient_orientation_set2           , 0x0021, 0x1171, CS);
GLOBAL_ELEMENT(SPI_Study_name                         , 0x0021, 0x1180, CS);
GLOBAL_ELEMENT(SPI_Study_type                         , 0x0021, 0x1182, CS);
GLOBAL_ELEMENT(SPI_Number_of_3D_raw_partitions_nominal, 0x0021, 0x1330, IS);
GLOBAL_ELEMENT(SPI_Number_of_3d_raw_part_cur          , 0x0021, 0x1331, IS);
GLOBAL_ELEMENT(SPI_Number_of_3D_image_partitions      , 0x0021, 0x1334, IS);
GLOBAL_ELEMENT(SPI_Actual_3D_partition_number         , 0x0021, 0x1336, IS);
GLOBAL_ELEMENT(SPI_Slab_thickness                     , 0x0021, 0x1339, DS);
GLOBAL_ELEMENT(SPI_Number_of_slices_nominal           , 0x0021, 0x1340, IS);
GLOBAL_ELEMENT(SPI_Number_of_slices_cur               , 0x0021, 0x1341, IS);
GLOBAL_ELEMENT(SPI_Current_slice_number               , 0x0021, 0x1342, IS);
GLOBAL_ELEMENT(SPI_Order_of_slices                    , 0x0021, 0x134f, IS);
GLOBAL_ELEMENT(SPI_Number_of_echoes                   , 0x0021, 0x1370, IS);
GLOBAL_ELEMENT(SPI_Locations_in_Acquisitions          , 0x0021, 0x104f, SS);

/* These are the two large fields in Siemens files in which many parameters
 * are dumped. The ASCCONV block is located in the second (0029, 1020), but
 * many of the DTI parameters are available in the first.
 */
GLOBAL_ELEMENT(SPI_Private_creator_0029               , 0x0029, 0x0010, LO);
GLOBAL_ELEMENT(SPI_Protocol2                          , 0x0029, 0x1010, CS);
GLOBAL_ELEMENT(SPI_Protocol                           , 0x0029, 0x1020, CS);


