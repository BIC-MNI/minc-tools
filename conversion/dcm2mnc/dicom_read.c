/* ----------------------------- MNI Header -----------------------------------
   @NAME       : dicom_read.c
   @DESCRIPTION: Code to read siemens dicom files and get info from them.
   @METHOD     :
   @GLOBALS    :
   @CALLS      :
   @CREATED    : January 28, 1997 (Peter Neelin)
   @MODIFIED   :
   * $Log: dicom_read.c,v $
   * Revision 1.29  2010-11-23 23:30:50  claude
   * dcm2mnc: fixed seg fault bug (Claude) and added b-matrix (Ilana)
   *
   * Revision 1.28  2008-08-12 05:00:23  rotor
   *  * large number of changes from Claude (64 bit and updates)
   *
   * Revision 1.27  2008/01/13 09:38:54  stever
   * Avoid compiler warnings about functions and variables that are defined
   * but not used.  Remove some such functions and variables,
   * conditionalize some, and move static declarations out of header files
   * into C files.
   *
   * Revision 1.26  2008/01/11 07:17:07  stever
   * Remove unused variables.
   *
   * Revision 1.25  2007/12/09 19:52:15  alex
   * Fixed error in echo_time extraction
   *
   * Revision 1.24  2007/06/08 20:28:57  ilana
   * added several fields to mincheader (dicom elements and found in ASCONV header)
   *
   * Revision 1.23  2007/05/30 15:17:34  ilana
   * fix so that diffusion images all written into 1 4d volume, gradient directions and bvalues are written to mincheader, some fixes for TIM diffusion images
   *
   * Revision 1.22  2006/04/09 15:38:02  bert
   * Add support for standard DTI fields
   *
   * Revision 1.21  2005/12/05 16:50:08  bert
   * Deal with weird XMedCon images
   *
   * Revision 1.20  2005/10/26 23:43:35  bert
   * Latest attempt at getting slice spacing consistent
   *
   * Revision 1.19  2005/09/16 22:13:33  bert
   * Fix slice spacing handling
   *
   * Revision 1.18  2005/08/26 21:25:54  bert
   * Latest changes ported from 1.0 branch
   *
   * Revision 1.16.2.5  2005/08/26 03:50:16  bert
   * Use ACR_Number_of_temporal_positions for number of time slices
   *
   * Revision 1.16.2.4  2005/08/18 16:38:43  bert
   * Minor updates for dealing w/older numaris data
   *
   * Revision 1.16.2.3  2005/06/20 22:03:01  bert
   * Add functions for traversing DICOM sequences and recursively hunting for needed fields.
   *
   * Revision 1.16.2.2  2005/06/02 17:04:26  bert
   * 1. Fix handling of signed values for pixel minimum and maximum, 2. Set found_dircos[] when we adopt the default direction cosines for files with null direction cosines
   *
   * Revision 1.16.2.1  2005/05/12 21:16:47  bert
   * Initial checkin
   *
   * Revision 1.16  2005/05/09 15:30:32  bert
   * Don't allow a rescale slope value of zero
   *
   * Revision 1.15  2005/04/28 17:17:57  bert
   * Set and update new width information fields in a manner analogous to the coordinate fields in the General_Info and File_Info structures
   *
   * Revision 1.14  2005/04/20 23:14:04  bert
   * Remove most SPI_ references
   *
   * Revision 1.13  2005/04/20 17:47:38  bert
   * Fairly major restructuring, added init_general_info() function
   *
   * Revision 1.12  2005/04/18 21:43:04  bert
   * Properly set default minimum and maximum values based on the pixel representation
   *
   * Revision 1.11  2005/04/18 21:01:51  bert
   * Set signed/unsigned flag correctly
   *
   * Revision 1.10  2005/04/18 20:43:25  bert
   * Added some additional debugging information for image position and orientation
   *
   * Revision 1.9  2005/04/18 16:22:13  bert
   * Don't allow non-slice MRI dimensions to grow arbitrarily
   *
   * Revision 1.8  2005/04/05 21:49:52  bert
   * Use rescale slope and intercept to determine the proper slice minimum and maximum
   *
   * Revision 1.7  2005/03/29 20:21:44  bert
   * Fix use of slice spacing; fully check for position information if possible, otherwise create a reasonable position from the slice index
   *
   * Revision 1.6  2005/03/14 23:29:35  bert
   * Support basic dynamic PET fields.  Also allocate indices and coordinates arrays for all dimensions, even those we won't use.
   *
   * Revision 1.5  2005/03/13 19:37:42  bert
   * Try to use slice location for coordinate when all else fails, also added one debugging message and a check for PET modality
   *
   * Revision 1.4  2005/03/03 20:11:00  bert
   * Consider patient_id and patient_name when sorting into series.  Fix handling of missing direction cosines
   *
   * Revision 1.3  2005/03/03 18:59:15  bert
   * Fix handling of image position so that we work with the older field (0020, 0030) as well as the new (0020, 0032)
   *
   * Revision 1.2  2005/03/02 20:18:09  bert
   * Latest fixes and tweaks
   *
   * Revision 1.1  2005/02/17 16:38:10  bert
   * Initial checkin, revised DICOM to MINC converter
   *
   * Revision 1.1.1.1  2003/08/15 19:52:55  leili
   * Leili's dicom server for sonata
   *
   * Revision 1.12  2002/05/01 21:29:34  rhoge
   * removed MrProt from minc header - encountered files with large strings,
   * causing seg faults
   *
   * Revision 1.11  2002/04/26 03:27:03  rhoge
   * fixed MrProt problem - replaced fixed lenght char array with malloc
   *
   * Revision 1.10  2002/04/08 17:26:34  rhoge
   * added additional sequence info to minc header
   *
   * Revision 1.9  2002/03/27 18:57:50  rhoge
   * added diffusion b value
   *
   * Revision 1.8  2002/03/19 13:13:56  rhoge
   * initial working mosaic support - I think time is scrambled though.
   *
   * Revision 1.7  2001/12/31 18:27:21  rhoge
   * modifications for dicomreader processing of Numaris 4 dicom files - at
   * this point code compiles without warning, but does not deal with
   * mosaiced files.  Also will probably not work at this time for Numaris
   * 3 .ima files.  dicomserver may also not be functional...
   *
   * Revision 1.6  2000/12/14 21:33:13  rhoge
   * code modifications to restore measurement loop support that was broken
   * by changes to support acqusition loop scanning
   *
   * Revision 1.5  2000/12/13 13:25:36  rhoge
   * removed dummy printf from convert_time_to_seconds - turns out that
   * buggy behaviour was an optimization problem
   *
   * Revision 1.4  2000/12/12 19:27:52  rhoge
   * changed atof(acr_find_string) back to acr_find_double after realizing
   * that these functions assume string representation
   *
   * Revision 1.3  2000/12/12 14:43:22  rhoge
   * fixed syntax error (dangling comment symbol) from removal of debug
   * printfs - compiles now
   *
   * Revision 1.2  2000/12/11 20:01:44  rhoge
   * fixed code for frame time computation - ACR vals are strings.  Also
   * inserted dummy fprintf statement in convert_time_to_seconds as this
   * seems to salvage Linux problems
   *
   * Revision 1.1.1.1  2000/11/30 02:13:15  rhoge
   * imported sources to CVS repository on amoeba
   * -now always use ACR_Series for run number (seemed to be
   *  problems with test introduced in 6.1)
   * -added code to detect use of acquisition loop and also to handle
   *  `corrected' siemens acq loop scans
   * -changed code to use registration time instead of scan time for
   *  session id
   * -got rid of extraneous acquisition id digit
   * -added technical information about data acquisition
   *
   * Revision 6.2  1999/10/29 17:51:58  neelin
   * Fixed Log keyword
   *
   * Revision 6.1  1999/08/05 20:00:34  neelin
   * Get acquisition id from series or study element, depending on the
   * version of the Siemens software.
   *
   * Revision 6.0  1997/09/12  13:24:27  neelin
   * Release of minc version 0.6
   *
   * Revision 5.1  1997/09/10  19:36:13  neelin
   * Small fix to set default direction cosines when they are absent from the
   * dicom data.
   *
   * Revision 5.0  1997/08/21  13:25:26  neelin
   * Release of minc version 0.5
   *
   * Revision 4.1  1997/06/13  12:51:21  neelin
   * Changed definition of time index and acquisition id to match change
   * in Siemens dicom software.
   *
   * Revision 4.0  1997/05/07  20:06:20  neelin
   * Release of minc version 0.4
   *
   * Revision 1.2  1997/03/11  13:10:48  neelin
   * Working version of dicomserver.
   *
   * Revision 1.1  1997/03/04  20:56:47  neelin
   * Initial revision
   *
   @COPYRIGHT  :
   Copyright 1997 Peter Neelin, McConnell Brain Imaging Centre,
   Montreal Neurological Institute, McGill University.
   Permission to use, copy, modify, and distribute this
   software and its documentation for any purpose and without
   fee is hereby granted, provided that the above copyright
   notice appear in all copies.  The author and McGill University
   make no representations about the suitability of this
   software for any purpose.  It is provided "as is" without
   express or implied warranty.
   ---------------------------------------------------------------------------- */

#include "dcm2mnc.h"

#define IMAGE_NDIMS 2

static void get_intensity_info(Acr_Group group_list,
                               File_Info *file_info,
                                General_Info *general_info);
static void get_coordinate_info(Acr_Group group_list, File_Info *file_info,
                                Orientation *orientation,
                                World_Index volume_to_world[VOL_NDIMS],
                                const int sizes[VOL_NDIMS],
                                double dircos[VOL_NDIMS][WORLD_NDIMS],
                                double steps[VOL_NDIMS],
                                double starts[VOL_NDIMS],
                                double coordinate[WORLD_NDIMS]);
static void get_general_header_info(Acr_Group group_list,
                                    General_Info *general_info);
//static void convert_numa3_coordinate(double coordinate[WORLD_NDIMS]);
static void get_identification_info(Acr_Group group_list,
                                    double *study_id, int *acq_id,
                                    int *rec_num, int *image_type);

static void get_pet_info(Acr_Group group_list, struct Pet_info *pet_ptr);
static void do_gradient_transformation(Acr_Group group_list, double bvecs_new[3]);
static int irnd(double x)
{
    if (x > 0.0) {
        x += 0.5;
    }
    else {
        x -= 0.5;
    }
    return (int) floor(x);
}

/* The patient name field often contains '^' as an official separator
   character. Sometimes these are present in unusual places, or the
   placement of the characters may change within a sequence. 

   We handle this by removing any trailing space or '^' characters here.
*/
static void fix_patient_name(char *patient_name)
{
  int n = strlen(patient_name) - 1;
  int c;
  while (n >= 0 && ((c = patient_name[n]) == '^' || c == ' ')) {
    patient_name[n--] = 0;
  }
}


int
is_numaris3(Acr_Group group_list)
{
    return (strstr(acr_find_string(group_list, ACR_Manufacturer, ""),
                   "SIEMENS") != NULL &&
            strstr(acr_find_string(group_list, ACR_Software_versions, ""),
                   "VB33") != NULL);
}


int
get_subimage_count(const Acr_Group group_list)
{
  int result = acr_find_int(group_list, ACR_Number_of_frames, 0);
  if (result <= 0) {
    result = acr_find_int(group_list, EXT_Slices_in_file, 1);
  }
  return result;
}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : init_general_info
   @INPUT      : fi_ptr - file-specific info
                 group_list - input data
                 volume_to_world - correspondence of volume to world dimensions
                 spatial_sizes - 3D voxel counts
                 dircos - direction cosines
                 steps - width of each voxel for each spatial dimension
                 starts - starting position for each spatial dimension
                 coordinate -
   @OUTPUT     : gi_ptr - general information about files in this series


   @RETURNS    : (nothing)
   @DESCRIPTION: Initializes a "General_Info" structure based upon several
                 bits of information, including a previously initialized
                 File_Info structure (fi_ptr) and the DICOM group_list.
                 Broken out from the get_file_info() function to help
                 simplify that function.
   @METHOD     :
   @GLOBALS    :
   @CALLS      :
   @CREATED    : April 19, 2005 (Bert Vincent)
   @MODIFIED   :
   ---------------------------------------------------------------------------- */
static void
init_general_info(General_Info *gi_ptr, /* OUT */
                  const File_Info *fi_ptr, /* IN */
                  const Acr_Group group_list, /* IN */
                  const World_Index volume_to_world[VOL_NDIMS], /* IN */
                  const int spatial_sizes[VOL_NDIMS], /* IN */
                  double dircos[VOL_NDIMS][WORLD_NDIMS], /* IN */
                  const double steps[VOL_NDIMS], /* IN */
                  const double starts[VOL_NDIMS], /* IN */
                  double study_id, /* IN */
                  int acq_id,   /* IN */
                  int rec_num)  /* IN */
{
    int ivalue;                 /* For pixel representation value. */
    World_Index iworld; /* World coordinate index (XCOORD, YCOORD...) */
    World_Index jworld;         /* World coordinate index */
    Volume_Index ivolume; /* Voxel coordinate index (VROW, VCOLUMN...) */

    // Get row and columns sizes
    gi_ptr->nrows = spatial_sizes[VROW];
    gi_ptr->ncolumns = spatial_sizes[VCOLUMN];

    // Save the study, acquisition, reconstruction and image type
    //   identifiers
    gi_ptr->study_id = study_id;
    gi_ptr->acq_id = acq_id;
    gi_ptr->rec_num = rec_num;

    strcpy(gi_ptr->image_type_string, acr_find_string(group_list,
                                                      ACR_Image_type,
                                                      ""));

    /* Get spatial coordinate information */
    gi_ptr->slice_world = volume_to_world[VSLICE];
    gi_ptr->row_world = volume_to_world[VROW];
    gi_ptr->column_world = volume_to_world[VCOLUMN];
    for (ivolume = 0; ivolume < VOL_NDIMS; ivolume++) {
        iworld = volume_to_world[ivolume];
        gi_ptr->step[iworld] = steps[ivolume];
        gi_ptr->start[iworld] = starts[ivolume];
        for (jworld = 0; jworld < WORLD_NDIMS; jworld++) {
            gi_ptr->dircos[iworld][jworld] = dircos[ivolume][jworld];
        }

        if (G.Debug) {
            printf("%2d. %s axis length %d step %.3f, start %.3f, cosines %.3f,%.3f,%.3f\n",
                   ivolume,
                   World_Names[iworld],
                   spatial_sizes[ivolume],
                   gi_ptr->step[iworld],
                   gi_ptr->start[iworld],
                   gi_ptr->dircos[iworld][XCOORD],
                   gi_ptr->dircos[iworld][YCOORD],
                   gi_ptr->dircos[iworld][ZCOORD]
                   );
        }
    }

    /* Set data type and range */
    if (fi_ptr->bits_alloc <= 8) {
        gi_ptr->datatype = NC_BYTE;
    }
    else {
        gi_ptr->datatype = NC_SHORT;
    }

    /* bert- modify code to correctly read the pixel
     * representation if available and use that to set the
     * signed/unsigned flag.
     */

    ivalue = acr_find_int(group_list, ACR_Pixel_representation, -1);
    if (ivalue == ACR_PIXEL_REP_UNSIGNED) {
        gi_ptr->is_signed = 0;
    }
    else if (ivalue == ACR_PIXEL_REP_SIGNED) {
        gi_ptr->is_signed = 1;
    }
    else {
        if (ivalue != -1) {
            printf("WARNING: Unknown pixel representation value %d\n",
                   ivalue);
        }
        gi_ptr->is_signed = ((gi_ptr->datatype == NC_SHORT) &&
                             (fi_ptr->bits_stored < 16));
    }

    gi_ptr->pixel_min = fi_ptr->pixel_min;
    gi_ptr->pixel_max = fi_ptr->pixel_max;

    /* Save display window info */
    gi_ptr->window_min = fi_ptr->window_min;
    gi_ptr->window_max = fi_ptr->window_max;

    /* Get the rest of the header information */
    get_general_header_info(group_list, gi_ptr);

    /* Copy the group list */
    gi_ptr->group_list = acr_copy_group_list(group_list);

        // note that number of slices will be wrong here for mosaics
        // we add some other mosaic-relevant fields...

    gi_ptr->num_mosaic_rows =
        acr_find_int(group_list, EXT_Mosaic_rows, 1);
    gi_ptr->num_mosaic_cols =
        acr_find_int(group_list, EXT_Mosaic_columns, 1);
    gi_ptr->num_slices_in_file =
        acr_find_int(group_list, EXT_Slices_in_file, 1);
    gi_ptr->sub_image_rows =
        (int)acr_find_short(group_list, EXT_Sub_image_rows,
                            acr_find_short(group_list, ACR_Rows, 0));
    gi_ptr->sub_image_columns =
        (int)acr_find_short(group_list, EXT_Sub_image_columns,
                            acr_find_short(group_list, ACR_Rows, 0));

    /* Set initialized flag */
    gi_ptr->initialized = TRUE;

    if (G.Debug) {
        printf("Pixel minimum %.10f maximum %.10f\n",
               gi_ptr->pixel_min, gi_ptr->pixel_max);
        printf("Window minimum %.10f maximum %.10f\n",
               gi_ptr->window_min, gi_ptr->window_max);
    }
}

void
get_axis_lengths(const Acr_Group group_list, General_Info *gi_ptr, const File_Info *fi_ptr)
{
  Acr_Element_Id mri_total_list[MRI_NDIMS];
  int imri;
  int index;
  // Initialize array for MRI dimension lengths
  //
  mri_total_list[SLICE] = ACR_Images_in_acquisition;
  mri_total_list[ECHO] = ACR_Echo_train_length;
  mri_total_list[TIME] = ACR_Acquisitions_in_series;
  mri_total_list[PHASE] = NULL;
  mri_total_list[CHEM_SHIFT] = NULL;

  /* Get dimension information
   */
  for (imri = 0; imri < MRI_NDIMS; imri++) {

    /* Get sizes along "MRI" dimensions...
     */
    gi_ptr->cur_size[imri] = 1;

    if (mri_total_list[imri] != NULL) {
      int def_val = 1;

      if (imri == SLICE) {
        /* Look for the standard slice count fields first. We
         * start with the (0054, 0081) first, and if that fails
         * we retry with (0020, 1002).
         */
        def_val = acr_find_int(group_list, ACR_Number_of_slices, 0);

        /* If we have a multiframe image (MOSAIC), use the number of
         * frames. This may not be present in some MOSAIC files.
         */
        if (def_val == 0) {
          def_val = acr_find_int(group_list, ACR_Number_of_frames, 0);
        }
        if (def_val == 0) {
          def_val = acr_find_int(group_list, mri_total_list[imri], 0);
        }

        if ((def_val == 0 && G.n_distinct_coordinates > 1 && gi_ptr->num_files % G.n_distinct_coordinates == 0)
        || (def_val * acr_find_int(group_list, ACR_Number_of_temporal_positions, -1) != gi_ptr->num_files && def_val > 0
        && G.n_distinct_coordinates > 1 && acr_find_int(group_list, ACR_Number_of_temporal_positions, -1) > 0) ) {
            def_val = G.n_distinct_coordinates;
        }
        
        if (def_val == 0) {
          def_val = gi_ptr->num_files;
        }
        gi_ptr->max_size[imri] = def_val;
      }
      else if (imri == TIME) {
        /* Look for the official time slice count field first.
         */
        def_val = acr_find_int(group_list,
                               ACR_Number_of_temporal_positions,
                               -1);
        if (G.Debug > 1)
          printf("(0020,0105)=%d\n", def_val);

        /* Some Philips scans DO NOT PROPERLY SET the (0020,0105) value.
         * Here's a first try at a workaround:
         */
        if ((def_val == 1 || gi_ptr->num_files % (2 * def_val * gi_ptr->max_size[SLICE]) == 0) &&
            gi_ptr->num_files % gi_ptr->max_size[SLICE] == 0 &&
            gi_ptr->num_files > gi_ptr->max_size[SLICE]) {
          /* Extra paranoia - only ignore this if the manufacturer
           * checks out.
           */
          char *mfg_str = acr_find_string(group_list, ACR_Manufacturer, "");
          if (strstr(mfg_str, "Philips") != NULL)
            def_val = -1;        /* Ignore the field. */
        }

        /* Now look in the equally official but different field.
         */
        if (def_val < 0) {
          def_val = acr_find_int(group_list,
                                 ACR_Number_of_time_slices,
                                 def_val);
          if (G.Debug > 1)
            printf("(0054,0101)=%d\n", def_val);
        }

        if (def_val < 0) {
          /* Sometimes need to look further for time count (ex:segmented FLASH)
           */
          def_val = acr_find_int(group_list,
                                 ACR_Cardiac_number_of_images,
                                 def_val);
          if (G.Debug > 1)
            printf("(0018,1090)=%d\n", def_val);
        }

        if (def_val <= 0) {
          def_val = acr_find_int(group_list,
                                 mri_total_list[imri],
                                 def_val);
          if (G.n_distinct_coordinates > 0 && gi_ptr->num_files > 1 && 
              def_val > gi_ptr->num_files/G.n_distinct_coordinates) {
              def_val = -1;
          }
        }

        if (def_val < 0 || ( def_val == 0 && G.file_type == N4DCM ) ||
        (def_val == 1 && !strcmp(acr_find_string(group_list, ACR_Modality, ""), "PT")) ) {
          /* As our last resort, we try to determine this by seeing how many
             files we have, and dividing by the number of slices.
          */
          if (gi_ptr->max_size[SLICE] > 1) {
            int num_per_file = get_subimage_count(group_list);
            int num_images = gi_ptr->num_files * num_per_file;
            if ((num_images % gi_ptr->max_size[SLICE]) == 0) {
              def_val = num_images / gi_ptr->max_size[SLICE];
            }
          }
        }
        else if (gi_ptr->found_philips_MRLabels == 1) {
              def_val *= 2;
        }
        gi_ptr->max_size[imri] = def_val;
      }
      else {
        int number_of_echoes = acr_find_int(group_list, ACR_Number_of_Echoes, -1);
        int locations_in_acquisitions = acr_find_int(group_list, SPI_Locations_in_Acquisitions, -1);
        int image_count = acr_find_int(group_list, GEMS_Images_in_series, -1);

        if (number_of_echoes > 1 && locations_in_acquisitions > 0 && image_count > 1
            && ( (G.n_files_total/number_of_echoes) == gi_ptr->num_files
            || (image_count/number_of_echoes) == gi_ptr->num_files )
            && imri == ECHO && gi_ptr->max_size[imri] < number_of_echoes) {
                gi_ptr->max_size[imri] = number_of_echoes;
                gi_ptr->fix_multi_echo_fMRI = 1;
        }
        else {
                gi_ptr->max_size[imri] = acr_find_int(group_list,
                                              mri_total_list[imri],
                                              def_val);
        }
      }
    }
    else {
      gi_ptr->max_size[imri] = 0;
    }

    /* We alway specify a maximum size of one for an unknown dimension,
     * but we want to keep track of those for which no valid value
     * was actually discovered (bert).
     */
    if (gi_ptr->max_size[imri] < 1) {
      gi_ptr->max_size[imri] = 1;
      gi_ptr->size_isset[imri] = 0;
    }
    else {
      gi_ptr->size_isset[imri] = 1;
    }

    /* Check for 3D partitions for slice dimensions */
    if (imri == SLICE) {
      /* Get number of 3D partitions for working out number of
       * slices
       */
      int number_of_3D_partitions =
        acr_find_int(group_list, SPI_Number_of_3D_raw_partitions_nominal, 1);
      if (number_of_3D_partitions < 1 || (G.n_distinct_coordinates == gi_ptr->num_files
      && number_of_3D_partitions > 1) ){
        number_of_3D_partitions = gi_ptr->num_files;
      }
      /* Do this only if there is a plausible reason to think we need it.
       */
      if (gi_ptr->size_isset[imri]) {
        gi_ptr->max_size[imri] *= number_of_3D_partitions;
      }
    }

    gi_ptr->image_index[imri] = -1;

    /* Allocate space for index and coordinate arrays.
     * Set the first values.
     */

    gi_ptr->indices[imri] = malloc(gi_ptr->max_size[imri] * sizeof(int));

    gi_ptr->coordinates[imri] =
      malloc(gi_ptr->max_size[imri] * sizeof(double));

    gi_ptr->widths[imri] =
      malloc(gi_ptr->max_size[imri] * sizeof(double));

    for (index = 0; index < gi_ptr->max_size[imri]; index++) {
      gi_ptr->indices[imri][index] = -1;
      gi_ptr->coordinates[imri][index] = 0;
      gi_ptr->widths[imri][index] = 0; /* default */
    }
    gi_ptr->search_start[imri] = 0;

    if (G.Debug) {
      printf("%2d. %s axis length %d\n",
             imri, Mri_Names[imri], gi_ptr->max_size[imri]);
    }
  } /* Loop over dimensions */
}

void
get_file_indices(const Acr_Group group_list, const General_Info *gi_ptr, File_Info *fi_ptr)
{
  Acr_Element_Id mri_index_list[MRI_NDIMS];
  int imri;

  // Array of elements for mri dimensions
  mri_index_list[SLICE] = SPI_Current_slice_number;
  mri_index_list[ECHO] = ACR_Echo_number;
  // dicom time element may be different on old systems
  mri_index_list[TIME] = ACR_Temporal_position_identifier;
  mri_index_list[PHASE] = NULL;
  mri_index_list[CHEM_SHIFT] = NULL;

  /* Get indices for image in current file
   */
  for (imri = 0; imri < MRI_NDIMS; imri++) {
    if (mri_index_list[imri] != NULL) {
      int tmp_index = acr_find_int(group_list, mri_index_list[imri], -1);

      if (imri == SLICE) {
        if (tmp_index < 0) {
          /* Non-MOSAIC only? */
          tmp_index = acr_find_int(group_list, ACR_Image, -1);
        }
        if (tmp_index < 0) {
          tmp_index = acr_find_int(group_list, ACR_PET_Image_index, -1);
        }
        /* Perform this range check only if absolutely needed.
         */
        if (tmp_index >= 1 &&
            gi_ptr->max_size[SLICE] > 1 &&
            gi_ptr->max_size[TIME] > 1) {
          tmp_index = ((tmp_index - 1) % gi_ptr->max_size[SLICE]) + 1;
        }
      }

      /* Use alternative index fields for time, if needed.
       */
      if (imri == TIME) {

          /* Force the use of another dicomtag if ACR_Temporal_position_identifier
           * is missing in a subset of slices
           */
          if (G.bogus_temporal_id && !G.bogus_PET_index) {
              tmp_index = -1;
          }

          if (tmp_index < 0) {

              if (G.bogus_PET_index) {
                  if (!G.bogus_frame_reference_time) {
                      tmp_index = (int)acr_find_double(group_list, ACR_Frame_reference_time, -1.0);
                  }
                  else {
                      tmp_index = (int)acr_find_double(group_list, ACR_Acquisition_time, -1.0);
                  }
              }

          int range_check = 1;
          /* Only use the acquisition number if we've determined it is
           * not constant.
           */
          if (G.min_acq_num != G.max_acq_num) {
            tmp_index = acr_find_int(group_list, ACR_Acquisition, -1);
            /* If the acquisition number corresponds to the time axis,
             * we DON'T want to perform the range check below.
             */
            if (G.max_acq_num == gi_ptr->max_size[TIME] ||
                G.max_acq_num == gi_ptr->max_size[TIME] - 1 ||
                G.max_acq_num - G.min_acq_num == gi_ptr->max_size[TIME]-1) {
              range_check = 0;
            }
          }

          if (tmp_index < 0 && !G.bogus_PET_index) {
            /* Yet another alternative field from some PET scans.
             * Generally needs to be range-checked.
             */
            tmp_index = acr_find_int(group_list, ACR_PET_Image_index, -1);
          }

          if (tmp_index < 0) {
            /* If all else fails, try this. Will almost certainly need
             * to be range-checked.
             */
            tmp_index = acr_find_int(group_list, ACR_Image, -1);
            /* With multiple echos, ACR_Image can go higher than the number of slices
             * In these cases we have to range_check by number of slices x number of echos
             */
            range_check = gi_ptr->max_size[ECHO];
          }

          /* Force the index to remain inside the established range of the
           * dimension, if known. Often these values will vary from 1-N
           * where N is the total number of slices in an acquisition.
           * We do NOT perform this check if we're dealing with
           * a mosaic or multiframe image.
           */

          if (range_check &&
              tmp_index >= 1 &&
              gi_ptr->max_size[SLICE] > 1 &&
              (gi_ptr->max_size[TIME] > 1 || (gi_ptr->max_size[ECHO] > 1 && range_check > 1)) &&
              gi_ptr->subimage_type == SUBIMAGE_TYPE_NONE) {
            /* Keep the index inside the proper range, if known.
             */
            tmp_index = ((tmp_index - 1) / (gi_ptr->max_size[SLICE] * range_check)) + 1;
            //printf("D: %d ", tmp_index);
          }
        }
      }
      if (tmp_index < 0) {
        tmp_index = 1;
      }

      if (imri == TIME) {
          if (gi_ptr->found_philips_MRLabels == 1) {
            /* Make sure LABEL is the first volume */
            if (strstr(acr_find_string(group_list, PMS_MRImageLabelType, ""), "CONTROL") != NULL) {                  
                  tmp_index *= 2;
              }
              else {
                  tmp_index += (tmp_index-1);
              }
          }
      }

      fi_ptr->index[imri] = tmp_index;
    }
    else {
      fi_ptr->index[imri] = 1;
    }
    //printf("%s: %d ", Mri_Names[imri], fi_ptr->index[imri]);
  }
  //printf("\n");
}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : get_file_info
   @INPUT      : group_list - input data
   @OUTPUT     : file_inf_p - file-specific info
   general_info - general information about files
   @RETURNS    : (nothing)
   @DESCRIPTION: Routine to extract information from a group list
   @METHOD     :
   @GLOBALS    :
   @CALLS      :
   @CREATED    : November 25, 1993 (Peter Neelin)
   @MODIFIED   : Modified Feb. 2000 by Rick Hoge to handle Acquisition loop mode
   : if assume_acq_loop is FALSE the other added variables don't
   : matter
   ---------------------------------------------------------------------------- */
void
get_file_info(Acr_Group group_list, File_Info *fi_ptr, General_Info *gi_ptr, const char *file_name)
{
    Mri_Index imri;             /* MRI index (SLICE, ECHO, TIME, PHASE...) */
    int nrows;                  /* Row count in this file */
    int ncolumns;               /* Column count in this file */
    int spatial_sizes[VOL_NDIMS]; /* Voxel coordinate extents */
    double study_id;            /* Study identifier */
    int acq_id;                 /* Acquisition identifier */
    int rec_num;                /* ? Seems to be a dummy */
    int cur_index;              /* Index of slice(s) in current file */
    int index;                  /* General index value */
    Orientation orientation;    /* TRANSVERSE, SAGITTAL, or CORONAL */
    World_Index volume_to_world[VOL_NDIMS]; /* Maps voxel to world indices */
    double coordinate[WORLD_NDIMS]; /* Slice coordinates */
    double dircos[VOL_NDIMS][WORLD_NDIMS]; /* Direction cosines */
    double steps[VOL_NDIMS];    /* Step (spacing) coordinates */
    double starts[VOL_NDIMS];   /* Start (origin) coordinates */
    Acr_Element element;

    /* Get image dimensions
     */
    nrows = (int)acr_find_short(group_list, ACR_Rows, 0);
    ncolumns = (int)acr_find_short(group_list, ACR_Columns, 0);

    spatial_sizes[VROW] = nrows;
    spatial_sizes[VCOLUMN] = ncolumns;

    spatial_sizes[VSLICE] = acr_find_int(group_list, ACR_Images_in_acquisition,
                                         1);

    /* Initialize fields in the File_Info structure.
     */
    for (imri = 0; imri < MRI_NDIMS; imri++) {
      fi_ptr->index[imri] = 0;
      fi_ptr->width[imri] = 0.0;
      fi_ptr->coordinate[imri] = 0.0;
    }

    /* Get intensity information
     */
    get_intensity_info(group_list, fi_ptr, gi_ptr);

    /* Check for necessary values not found
     */
    if ((nrows <= 0) || (ncolumns <= 0) ||
        (fi_ptr->bits_stored <= 0) ||
        (fi_ptr->bits_alloc <= 0)) {
        if (G.Debug) {
            printf("ERROR: Needed values missing, marking '%s' invalid\n", file_name);
        }
        fi_ptr->valid = FALSE;
        return;
    }

    /* Get study, acq, rec, image type id's
     */
    get_identification_info(group_list, &study_id, &acq_id, &rec_num, NULL);

    if (!gi_ptr->initialized) {
      get_axis_lengths(group_list, gi_ptr, fi_ptr);
      spatial_sizes[VSLICE] = gi_ptr->max_size[SLICE];
    }

    get_file_indices(group_list, gi_ptr, fi_ptr);

    if (!gi_ptr->initialized) {
      for (imri = 0; imri < MRI_NDIMS; imri++) {
        gi_ptr->default_index[imri] = fi_ptr->index[imri];
      }
    }

    /* Get coordinate information. This needs to be done before we call
     * init_general_info().
     */
    get_coordinate_info(group_list, fi_ptr, &orientation, volume_to_world,
                        spatial_sizes, dircos, steps, starts, coordinate);

    /*
     * Use the coordinate information rather than the slice or time
     * position derived above.  This seems to be much more reliable with
     * IMA files, but not for real DICOM.
     */
    if ( G.file_type == IMA ||
       ( G.file_type == N4DCM && !strcmp(acr_find_string(group_list, ACR_Modality, ""), "MR") && (strstr(acr_find_string(group_list, ACR_Manufacturer, ""), "SIEMENS") == NULL) ) ||
       ( G.file_type == N4DCM && !strcmp(acr_find_string(group_list, ACR_Modality, ""), "PT") && ( strstr(acr_find_string(group_list, ACR_Manufacturer, ""), "CPS") != NULL) ) ||
       G.min_acq_num != G.max_acq_num && !is_siemens_mosaic(group_list)) {
       fi_ptr->index[SLICE] = irnd(fi_ptr->coordinate[SLICE] * 100.0);
    }

    /* Set up general info on first pass
     */
    if (!gi_ptr->initialized) {
        init_general_info(gi_ptr,
                          fi_ptr,
                          group_list,
                          volume_to_world,
                          spatial_sizes,
                          dircos,
                          steps,
                          starts,
                          study_id,
                          acq_id,
                          rec_num);

        for (imri = 0; imri < MRI_NDIMS; imri++) {
          gi_ptr->indices[imri][0] = fi_ptr->index[imri];
          gi_ptr->coordinates[imri][0] = fi_ptr->coordinate[imri];
          gi_ptr->widths[imri][0] = fi_ptr->width[imri];

          printf("First %s at %d,%f,%f\n", Mri_Names[imri],
                 fi_ptr->index[imri],
                 fi_ptr->coordinate[imri],
                 fi_ptr->width[imri]);
        }
    }
    /* Update general info and validate file on later passes
     */
    else {

        /* Check for consistent pixel minimum and maximum. */
        if ((gi_ptr->pixel_max != fi_ptr->pixel_max) ||
            (gi_ptr->pixel_min != fi_ptr->pixel_min)) {
            printf("WARNING: Inconsistent pixel minimum and maximum\n");
            printf("    %f %f, %f %f\n",
                   gi_ptr->pixel_min, gi_ptr->pixel_max,
                   fi_ptr->pixel_min, fi_ptr->pixel_max);
#if 0
            if (gi_ptr->pixel_max < fi_ptr->pixel_max) {
                gi_ptr->pixel_max = fi_ptr->pixel_max;
            }
            if (gi_ptr->pixel_min > fi_ptr->pixel_min) {
                gi_ptr->pixel_min = fi_ptr->pixel_min;
            }
#endif
        }

        /* Check for consistent data type */
        if (((gi_ptr->datatype == NC_BYTE) && (fi_ptr->bits_alloc > 8)) ||
            ((gi_ptr->datatype == NC_SHORT) && (fi_ptr->bits_alloc <= 8))) {
            printf("Inconsistent datatype, marking invalid\n");
            fi_ptr->valid = FALSE;
            return;
        }

        /* Check row and columns sizes */
        if ((nrows != gi_ptr->nrows) && (ncolumns != gi_ptr->ncolumns))  {
            printf("Mismatched rows/columns, marking invalid\n");
            fi_ptr->valid = FALSE;
            return;
        }

        /* Check study and acquisition id's */
        if ((gi_ptr->study_id != study_id) || (gi_ptr->acq_id != acq_id)) {
            printf("Mismatched acquisition/study, marking invalid\n");
            fi_ptr->valid = FALSE;
            return;
        }

        // Update display window info
        if (gi_ptr->window_min > fi_ptr->window_min)
          gi_ptr->window_min = fi_ptr->window_min;
        if (gi_ptr->window_max < fi_ptr->window_max)
          gi_ptr->window_max = fi_ptr->window_max;


        /* Look to see if indices have changed */
        for (imri = 0; imri < MRI_NDIMS; imri++) {
          /* If a dimension is known to have a maximum size of one
           * or less, we do NOT allow it to grow in any way.  An
           * exception is made for the slice and time dimensions,
           * however, since it appears that it is common for them to
           * be unspecified and can be guessed only by the number of
           * distinct locations discovered.
           */
          if (imri != SLICE && gi_ptr->max_size[imri] <= 1 && !(imri == TIME &&
                !((G.file_type == N4DCM) && (gi_ptr->num_slices_in_file > 1)))) {
            if (/* G.Debug && */ fi_ptr->index[imri] > 1) {
              printf("Warning: merging extra indices on %s axis: ",
                     Mri_Names[imri]);
              printf("  %d %d\n", gi_ptr->max_size[imri],
                     fi_ptr->index[imri]);
            }
            continue;
          }

          /* Get current index */
          cur_index = fi_ptr->index[imri];

          /* Check whether this index is in the list.
           */
          if (gi_ptr->cur_size[imri] == 1) {
            index = ((cur_index == gi_ptr->default_index[imri]) ? 0 : -1);
          }
          else {
            /* Search list of indices for 'cur_index'.  Search is
               started at search_start[] and has maximum length of
               cur_size[imri].
            */
            index = search_list(cur_index,
                                gi_ptr->indices[imri],
                                gi_ptr->cur_size[imri],
                                gi_ptr->search_start[imri]);
          }

          /* If it is not, then add it */
          if (index < 0 && gi_ptr->coordinates[SLICE][0] != fi_ptr->coordinate[SLICE]) {

                int number_of_fMRI_frames = 0;
                if (gi_ptr->fix_multi_echo_fMRI > 0 ) {
                       number_of_fMRI_frames = gi_ptr->num_files/acr_find_int(group_list, SPI_Locations_in_Acquisitions, -1);
                   }

            if (G.Debug >= HI_LOGGING) {
              printf("Need to add index %d to %s list, %d/%d\n",
                     cur_index, Mri_Names[imri],
                     gi_ptr->cur_size[imri],
                     gi_ptr->max_size[imri]);
            }

            /* Check whether we can add a new index */
            if (gi_ptr->cur_size[imri] >= gi_ptr->max_size[imri]) {
                if (imri == TIME && gi_ptr->fix_multi_echo_fMRI > 0 && number_of_fMRI_frames > 0) {
                    if (gi_ptr->max_size[TIME] < number_of_fMRI_frames) {
                        gi_ptr->max_size[imri]++;
                    }
                }
                else {
                    gi_ptr->max_size[imri]++;
                }

              gi_ptr->indices[imri] =
                realloc(gi_ptr->indices[imri],
                        gi_ptr->max_size[imri] * sizeof(int));

              gi_ptr->coordinates[imri] =
                realloc(gi_ptr->coordinates[imri],
                        gi_ptr->max_size[imri] * sizeof(double));

              gi_ptr->widths[imri] =
                realloc(gi_ptr->widths[imri],
                        gi_ptr->max_size[imri] * sizeof(double));
            }


            /* Add the index and coordinate to the lists */
            index = gi_ptr->cur_size[imri];
            gi_ptr->search_start[imri] = index;
            gi_ptr->indices[imri][index] = cur_index;
            gi_ptr->coordinates[imri][index] = fi_ptr->coordinate[imri];
            gi_ptr->widths[imri][index] = fi_ptr->width[imri];

           if (imri == SLICE && G.n_distinct_coordinates > 0){
               if (gi_ptr->cur_size[imri] < G.n_distinct_coordinates) {
                   gi_ptr->cur_size[imri]++;
               }
           }
           else if (imri == TIME && gi_ptr->fix_multi_echo_fMRI > 0 && number_of_fMRI_frames > 0) {
               if (gi_ptr->cur_size[imri] < number_of_fMRI_frames) {
                   gi_ptr->cur_size[imri]++;
               }
           }
           else {
               gi_ptr->cur_size[imri]++;
           }

            if (G.Debug) {
              printf("Added %s coordinate %f at %d %f %f %f\n",
                     Mri_Names[imri], gi_ptr->coordinates[imri][index],
                     cur_index, coordinate[0], coordinate[1], coordinate[2]);
            }
          }
          else if (G.Debug) {
            /*
            printf("%s: %s index %d is already on the list.\n",
                   file_name,
                   Mri_Names[imri],
                   cur_index);
            printf("Existing coordinate: %f, file coordinate: %f\n",
                   gi_ptr->coordinates[imri][index],
                   fi_ptr->coordinate[imri]);
            */
          }
        }              /* Loop over Mri_Index */


        //ilana debug
        //int test = fi_ptr->coordinate[TIME];
        //print ("*******Time coordinate IS: %i\n\n",test);
    }

    /* Get DTI information if available.
     */
    fi_ptr->b_value = (double)acr_find_double(group_list, ACR_Diffusion_b_value, 0);

    /* Determine the series is DTI if at least one b-value is found to be above 0 */
    gi_ptr->acq.dti = (gi_ptr->acq.dti == 1 || fi_ptr->b_value > 0);

    /* Get GEMS fMRI information if available.
     */
    fi_ptr->tpos_id = (int)acr_find_int(group_list, ACR_Temporal_position_identifier, -1);
    fi_ptr->trigger_time = (double)acr_find_double(group_list, ACR_Trigger_time, -1.0);
    fi_ptr->slice_location = (double)acr_find_double(group_list, ACR_Slice_location, 0.0);
    fi_ptr->RTIA_timer = (double)acr_find_double(group_list, GEMS_RTIA_timer, -1.0);
    fi_ptr->rep_time = (double)acr_find_double(group_list, ACR_Repetition_time, 0.0);

    element = acr_find_group_element(group_list,
                                     ACR_Diffusion_gradient_orientation);

    Acr_Double grad_direction[WORLD_NDIMS];
    if (element == NULL ||
        acr_get_element_double_array(element, WORLD_NDIMS, grad_direction) != WORLD_NDIMS) {
        /* Attempt to get gradient directions from GEMS, otherwise this should be 0 for the b=0 images*/
        grad_direction[XCOORD] = acr_find_double(group_list, GEMS_Diffusion_direction_x, 0);
        grad_direction[YCOORD] = acr_find_double(group_list, GEMS_Diffusion_direction_y, 0);
        grad_direction[ZCOORD] = acr_find_double(group_list, GEMS_Diffusion_direction_z, 0);
    }

    if (strstr(acr_find_string(group_list, ACR_Manufacturer, ""), "SIEMENS") != NULL || strstr(acr_find_string(group_list, ACR_Manufacturer, ""), "Siemens") != NULL || strstr(acr_find_string(group_list, ACR_Manufacturer, ""), "Philips") != NULL) {
        double bvec_corrected[3];
        do_gradient_transformation(group_list, bvec_corrected);
        grad_direction[XCOORD] = bvec_corrected[XCOORD];
        grad_direction[YCOORD] = bvec_corrected[YCOORD];
        grad_direction[ZCOORD] = bvec_corrected[ZCOORD];
    }

    fi_ptr->grad_direction[XCOORD] = (double)grad_direction[XCOORD];
    fi_ptr->grad_direction[YCOORD] = (double)grad_direction[YCOORD];
    fi_ptr->grad_direction[ZCOORD] = (double)grad_direction[ZCOORD];

    /* Want to add B-matrix if it exists, the problem is that
       the b=0 image does not even have the field while
       the other directions might (AND b=0 is ofen the first image!).
       Initialize to all 0s and decide later whether to include in header?
    */
    if (gi_ptr->acq.dti) {
        element = acr_find_group_element(group_list, SPI_B_matrix);
        if (element == NULL ||
            acr_get_element_double_array(element, B_MATRIX_COUNT,
                                         fi_ptr->b_matrix) != B_MATRIX_COUNT) {
            int i;
            for (i = 0; i < B_MATRIX_COUNT; i++) {
              fi_ptr->b_matrix[i] = 0; /* bmatrix for b=0 should be 0*/
            }
        }
    }

    // If we get to here, then we have a valid file
    fi_ptr->valid = TRUE;
    return;
}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : get_identification_info
   @INPUT      : group_list - input data
   @OUTPUT     : study_id
   acq_id
   rec_num
   image_type
   @RETURNS    : (nothing)
   @DESCRIPTION: Routine to get image identification information.
   @METHOD     :
   @GLOBALS    :
   @CALLS      :
   @CREATED    : February 28, 1997 (Peter Neelin)
   @MODIFIED   :
   ---------------------------------------------------------------------------- */
static void
get_identification_info(Acr_Group group_list,
                        double *study_id, int *acq_id,
                        int *rec_num, int *image_type)
{
    int number_of_frames;
    int number_of_averages;

    if (study_id != NULL) {
        double sID = (float)acr_find_int(group_list, ACR_Study, 0);
        if (sID > 0) {
            /* Use numeric studyID (0020,0010) by default if provided */
            *study_id = sID;
        }
        else {
        // generate a study ID number from date & time:  yyyymmdd.hhmmss
        // (should be unique enough for our application)
            *study_id = (((float)acr_find_int(group_list, ACR_Study_date, 0)) +
                    ((float)acr_find_int(group_list, ACR_Study_time, 0))/1e6);
        }
    }
    if (acq_id != NULL) {

        *acq_id = acr_find_int(group_list, ACR_Series, 0);

        number_of_frames =
            acr_find_int(group_list, ACR_Acquisitions_in_series, 1);

        number_of_averages =
            acr_find_int(group_list, ACR_Nr_of_averages, 1);

        /* Determine if measurement loop was used (rhoge) -

        if so, we replace the different series numbers with
        ACR_Study_time, which is the same for all frames in a run.
        This will aid in grouping the files later on.

        Criteria used for identification of meast loop:

        1) more than one dynamic scan

        2) number of dynamic scans NOT equal to nominal number of signal
        averages (if they're equal, we assume acquisition loop scan)

        WARNING:  it is possible that someone might use the
        measurement loop do serial scans which each have multiple signal
        averages.  If NSA = the number of measts. in this case, then
        the scan will not be recognized as a Meast loop scan and the
        different frames will be placed in different series.  To fix
        this, we'd really need to look at the series numbers of
        future and past files.  It's also unlikely to happen but I'm
        sure it will...

        This also means we should NOT correct the number of signal
        averages on the sending side */


        /*      if ((number_of_frames > 1) || (*acq_id == 0)) { (orig test) */

        if ( (G.file_type == N3DCM) &&
             (number_of_frames > 1) &&
             (number_of_frames != number_of_averages)) {

            *acq_id = acr_find_int(group_list, ACR_Study_time, 0);

        }
    }
    if (rec_num != NULL)
        *rec_num = 0;
    if (image_type != NULL)
        *image_type = 0;
}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : get_intensity_info
   @INPUT      : group_list - input data
   @OUTPUT     : fi_ptr - file-specific info
   @RETURNS    : (nothing)
   @DESCRIPTION: Routine to get intensity information from a group list
   @METHOD     :
   @GLOBALS    :
   @CALLS      :
   @CREATED    : February 28, 1997 (Peter Neelin)
   @MODIFIED   :
   ---------------------------------------------------------------------------- */
static void
get_intensity_info(Acr_Group group_list, File_Info *fi_ptr, General_Info *gi_ptr)
{
    double window_centre, window_width;
    double rescale_intercept_public, rescale_slope_public, rescale_slope_real; 
    double rescale_intercept_real, Philips_private_scale_slope, Philips_private_scale_intercept;
    double rescale_slope, rescale_intercept;
    int ivalue;                 /* 0000 for unsigned, 0001 for signed */
    int imin, imax;             /* Default minimum and maximum values */

    /* Get pixel storage information */
    fi_ptr->bits_alloc = (int)acr_find_short(group_list, ACR_Bits_allocated, 0);
    fi_ptr->bits_stored = (int)acr_find_short(group_list, ACR_Bits_stored, 0);

    /* bert- properly set the minimum and maximum pixel values depending
     * on whether or not this file specifies signed pixel values.
     */
    ivalue = acr_find_int(group_list, ACR_Pixel_representation, -1);

    if (ivalue == ACR_PIXEL_REP_SIGNED) {
        imin = -(1 << (fi_ptr->bits_stored - 1));
        imax = (1 << (fi_ptr->bits_stored - 1)) - 1;
    }
    else {
        imin = 0;
        imax = (1 << fi_ptr->bits_stored) - 1;
    }

    if (G.useMinMax) {
        int pmin;
        int pmax;

        /* Get pixel value information
         * I think this might wrongly assume that the ACR values min/max
         * apply to the whole volume - it actually appears that they apply
         * to the current slice.
         */

        pmin = (int)acr_find_short(group_list, ACR_Smallest_pixel_value, imin);
        pmax = (int)acr_find_short(group_list, ACR_Largest_pixel_value, imax);

        /* Hack to convert to signed representation if indicated - if
         * bit 15 is set, we have to "promote" to 2's complement by
         * sign-extending the result before converting it to a double.
         * Perhaps this sort of thing needs to be pushed down into the
         * ACR-NEMA library somehow, so that the representations of
         * short values are automatically converted properly?
         */
        if (ivalue == ACR_PIXEL_REP_SIGNED) {
            if (pmin & 0x8000) {
                pmin -= 0x10000;
            }
            if (pmax & 0x8000) {
                pmax -= 0x10000;
            }
        }

        fi_ptr->pixel_min = (double) pmin;
        fi_ptr->pixel_max = (double) pmax;
    }
    else {
        /* for now, use bits_stored to determine dynamic range
         * DICOM info on largest pixel applies to first slice,
         * not whole volume - this caused problems (roundoff?)
         * in Siemens Numaris 4 scans
         */
        fi_ptr->pixel_min = imin;
        fi_ptr->pixel_max = imax;
    }

    /* retrieving two pair of public tags RS, RI and one pair of private tags SI, SS, according to PMC3998685
     * setting default value if they don't exist
     */

    /* public real world value of RS and RI*/
    rescale_intercept_real = (double)acr_find_double(group_list, ACR_RealWorldValue_Intercept, 0);
    rescale_slope_real = (double)acr_find_double(group_list, ACR_RealWorldValue_Slope, 0);
    /* public RS and RI */
    rescale_slope_public = (double)acr_find_double(group_list, ACR_Rescale_slope, 0);
    rescale_intercept_public = (double)acr_find_double(group_list, ACR_Rescale_intercept, 0);
    /* private SS and SI */
    Philips_private_scale_slope = acr_find_double(group_list, PMS_Private_Rescale_slope, 0.0);
    Philips_private_scale_intercept = acr_find_double(group_list, PMS_Private_Rescale_intercept, 0.0);

    /* if real world value is there, we ignore other tags, we use the real world public values for slope and intercept */
    rescale_slope = rescale_slope_real;
    rescale_intercept = rescale_intercept_real;
    /* if other pair of public tags and private tags are there, we use RS RI SS in the equation of PMC3998685*/
    if(rescale_slope == 0 && rescale_slope_public != 0 && Philips_private_scale_slope != 0 &&
        strstr(acr_find_string(group_list, ACR_Manufacturer, ""), "Philips") != NULL){

        rescale_slope = rescale_slope_public / (Philips_private_scale_slope * rescale_slope_public);
        rescale_intercept = rescale_intercept_public / (Philips_private_scale_slope * rescale_slope_public);

    }
    /* if only that pair of public tags is there, we use these public values RS RI 
    instead of the real world public values for slope and intercept */
    else if(rescale_slope == 0 && rescale_slope_public != 0){

        rescale_slope = rescale_slope_public;
        rescale_intercept = rescale_intercept_public;

    }
    /* if only that pair of private tags is there, we use SI and SS with equation in PMC3998685*/
    else if(rescale_slope == 0 && Philips_private_scale_slope != 0 &&
        strstr(acr_find_string(group_list, ACR_Manufacturer, ""), "Philips") != NULL){

        rescale_slope = 1 / Philips_private_scale_slope;
        rescale_intercept = -1 * (Philips_private_scale_intercept / Philips_private_scale_slope);

    }
    /* if there is no tag present, we set the default value and give warning message */
    else if (rescale_slope == 0){
        rescale_slope = 1;
        rescale_intercept = 0;
        gi_ptr->missing_scale = 1;
    }
    fi_ptr->slice_min = fi_ptr->pixel_min * rescale_slope + rescale_intercept;
    fi_ptr->slice_max = fi_ptr->pixel_max * rescale_slope + rescale_intercept;

    /* Get window min and max */
    window_centre = (fi_ptr->slice_max + fi_ptr->slice_min) / 2.0;
    window_width  = fi_ptr->slice_max - fi_ptr->slice_min;
    window_centre =
        (double)acr_find_double(group_list, ACR_Window_centre, window_centre);
    window_width =
        (double)acr_find_double(group_list, ACR_Window_width, window_width);
    fi_ptr->window_min = window_centre - window_width / 2.0;
    fi_ptr->window_max = window_centre + window_width / 2.0;

}

/* Function to recursively search an element list for a specific
 * element, skipping a specified number of occurrences before
 * returning.  This is only called by acr_recurse_for_element().
 */
Acr_Element
acr_recursive_search(Acr_Element el_lst, int *nskip, Acr_Element_Id srch_id)
{
    Acr_Element el_ret = NULL;
    Acr_Element el_tmp;

    for (el_tmp = el_lst; el_tmp != NULL;
         el_tmp = acr_get_element_next(el_tmp)) {

        /* If we find what we're looking for, return it.
         */
        if (acr_get_element_group(el_tmp) == srch_id->group_id &&
            acr_get_element_element(el_tmp) == srch_id->element_id) {
            if (*nskip <= 0) {
                el_ret = el_tmp;
                break;
            }
            else {
                --(*nskip);
            }
        }
        /* See if we need to recurse.
         */
        if (acr_element_is_sequence(el_tmp)) {
            el_lst = (Acr_Element) acr_get_element_data(el_tmp);
            el_ret = acr_recursive_search(el_lst, nskip, srch_id);
            if (el_ret != NULL) {
                break;
            }
        }
    }
    return (el_ret);
}

/* acr_recurse_for_element()
 *
 * Function to search a group list for a particular element.  Unlike other
 * functions along these lines, this function will recursively descend into
 * compound datatypes (DICOM sequences) to hunt for instances of a particular
 * element.
 *
 * The search proceeds in two stages: The first is to search for
 * a particular sequence object.  This must be found or else the search is
 * called off.  Once the expected sequences is found, the function will
 * recursively search all of the substructure of that sequence for the
 * requested subelement.  The "nskip" parameter tells the function to ignore
 * the first "nskip" matches that it locates.
 */
Acr_Element
acr_recurse_for_element(Acr_Group group_list,
                        int nskip,
                        Acr_Element_Id seq_id,
                        Acr_Element_Id srch_id)
{
    Acr_Element el_seq;

    /* Hunt for the necessary sequence object.
     */
    el_seq = acr_find_group_element(group_list, seq_id);
    if (el_seq == NULL || !acr_element_is_sequence(el_seq)) {
        /* If not found, or not a sequence, abort the search.
         */
        return (NULL);
    }

    /* Otherwise proceed to "stage 2" and hunt for the requested subelement.
     */
    return acr_recursive_search((Acr_Element) acr_get_element_data(el_seq),
                                &nskip, srch_id);
}

int
dicom_read_position(Acr_Group group_list, int index, double coordinate[3])
{
    Acr_Element element = NULL;
    int result;
    int nskip = 0;

    if (index >= 0) {
      element = acr_recurse_for_element(group_list, index,
                                        ACR_Perframe_func_groups_seq,
                                        ACR_Plane_position_seq);
      if (element != NULL &&
          (element = acr_recursive_search( element, &nskip, ACR_Image_position_patient)) != NULL) {
        result = DICOM_POSITION_LOCAL;
      }
      else {
        /* Try to read a unique element from the sequences.  If this
         * succeeds, we need to flag this fact so that the higher-level
         * processing can adapt accordingly.
         */
        element = acr_recurse_for_element(group_list, index,
                                          ACR_Perframe_func_groups_seq,
                                          ACR_Image_position_patient);
      }
    }
    if (element != NULL) {
      result = DICOM_POSITION_LOCAL; /* Found a slice-specific position */
    }
    else {
      result = DICOM_POSITION_GLOBAL; /* Found a global position */

      element = acr_find_group_element(group_list,
                                       ACR_Image_position_patient);

      if (element == NULL) {
        /* bert-look for field in weird XMedCon location
         */
        element = acr_recurse_for_element(group_list, 0,
                                          ACR_Detector_information_seq,
                                          ACR_Image_position_patient);
      }
      
      if (element == NULL) {
        element = acr_find_group_element(group_list,
                                         ACR_Image_position_patient_old);
      }
    }

    if (element == NULL) {
        printf("WARNING: Failed to find image position (%d)\n", index);
    }
    else {
        if (acr_get_element_numeric_array(element,
                                          WORLD_NDIMS,
                                          coordinate) == WORLD_NDIMS) {
            return (result);
        }

        if (G.Debug) {
            printf("WARNING: Failed to read image position ('%s')\n",
                   (char *)acr_get_element_string(element));
        }
    }
    return DICOM_POSITION_NONE;
}

int
dicom_read_orientation(Acr_Group group_list, double orientation[6])
{
    Acr_Element element;
    int result;

    /* read in row/col vectors:
     */

    /* Look for single global header with slices all in same file. */

    element = acr_recurse_for_element(group_list, 0,
                                      ACR_Shared_func_groups_seq,
                                      ACR_Image_orientation_patient);

    /* Look for single local header with slices all in same file,
       with one header per slice (Philips Intera). */

    if (element == NULL) {
        element = acr_recurse_for_element(group_list, 0,
                                          ACR_Perframe_func_groups_seq,
                                          ACR_Image_orientation_patient);
    }

    /* bert - deal with weird XMedCon images...
     */
    if (element == NULL) {
        element = acr_recurse_for_element(group_list, 0,
                                          ACR_Detector_information_seq,
                                          ACR_Image_orientation_patient);
    }

    /* Look for header with slices in separate files. */
    if (element == NULL) {
        element = acr_find_group_element(group_list,
                                         ACR_Image_orientation_patient);
    }
    if (element == NULL) {
        /* If we failed to find the newer, better patient orientation
         * information, try to use the obsolete information if present.
         */
        element = acr_find_group_element(group_list,
                                         ACR_Image_orientation_patient_old);
    }
    if (element == NULL) {
        printf("WARNING: Failed to find image orientation!\n");
        return (0);
    }
    else if ((result = acr_get_element_numeric_array(element, 6,
                                                     orientation)) != 6) {
        printf("WARNING: Failed to read image orientation! (%d, '%s')\n",
               result, (char *)acr_get_element_string(element));
        return (0);
    }
    /* Swap row and column if we encounter HFDR PT */
    if (strstr(acr_find_string(group_list, ACR_Patient_position, ""), "HFDR") != NULL && !strcmp(acr_find_string(group_list, ACR_Modality, ""), "PT") && result == 6) {
        double row = orientation[0], col = orientation[1];
        orientation[0] = col;
        orientation[1] = row;
    }
    return (1);
}

/*
 * Read the pixel size, an array of 2 floating point numbers, from the
 * DICOM group list.
 */
int
dicom_read_pixel_size(Acr_Group group_list, double pixel_size[IMAGE_NDIMS])
{
    Acr_Element element;
    int result = 0;
    int i;

    for (i = 0; i < IMAGE_NDIMS; i++) {
        pixel_size[i] = -DBL_MAX;
    }

    /* Look for single global header with slices all in same file. */

    element = acr_recurse_for_element(group_list, 0,
                                      ACR_Shared_func_groups_seq,
                                      ACR_Pixel_size);

    /* Look for single local header with slices all in same file,
       with one header per slice (Philips Intera). */

    if (element == NULL) {
        element = acr_recurse_for_element(group_list, 0,
                                          ACR_Perframe_func_groups_seq,
                                          ACR_Pixel_size);
    }

    /* Look for header with slices in separate files. */

    if (element == NULL) {
        element = acr_find_group_element(group_list, ACR_Pixel_size);
    }

    if (element == NULL) {
        printf("WARNING: Can't find pixel size element\n");
    } else {
        /* Extract sizes from header string if found. */
        if (acr_get_element_numeric_array(element, IMAGE_NDIMS,

                                          pixel_size) != IMAGE_NDIMS) {
            printf("WARNING: Can't read pixel size element\n");
        }
        else {
            result = 1;
        }
    }

    /* If the values are still uninitialized, set them to some reasonable
     * defaults.
     */
    if (pixel_size[0] == -DBL_MAX) {
        pixel_size[0] = 1.0;    /* Assume 1mm spacing */
    }

    if (pixel_size[1] == -DBL_MAX) {
        pixel_size[1] = pixel_size[0]; /* Assume uniform sample grid. */
    }
    return (result);
}

/* Figure out the slice thickness.  It could be from either one of
 * two possible places in the file.
 *
 * This code has changed several times, and there may be no single
 * correct way of deriving the true slice spacing from the official
 * DICOM slice thickness and slice spacing fields.  My best guess is
 * to look for both fields, and to adopt the slice spacing when both
 * fields are set.
 */
double get_slice_separation(Acr_Group group_list){
    double sep = 1.0;
    double slice_thickness = (double)acr_find_double(group_list, ACR_Slice_thickness, 0);
    double slice_spacing = (double)acr_find_double(group_list, ACR_Spacing_between_slices, 0);

    if (slice_thickness == 0.0) {
        /* No slice thickness value found. */
        if (slice_spacing == 0.0) {
            if (G.Debug >= HI_LOGGING) {
                printf("Using default slice thickness of 1.0\n");
            }
        }
        else {
            if (G.Debug >= HI_LOGGING) {
                printf("Using (0018,0088) for slice thickness\n");
            }
            sep =  slice_spacing;
        }
    }
    else if (slice_spacing == 0.0) {
        /* No slice spacing value found. */
        if (G.Debug >= HI_LOGGING) {
            printf("Using (0018,0050) for slice thickness\n");
        }
        sep = slice_thickness;
    }
    else {
        /* Both fields are set.  I choose the slice spacing rather
         * than the slice thickness in this case. However, I believe
         * there is some evidence that this can cause problems in rare
         * cases.
         */
        if (G.Debug && !NEARLY_EQUAL(slice_thickness, slice_spacing)) {
            printf("Tags (0018,0050) and (0018,0088) have different values: ");
            printf(" (0018,0050) = %.10f, (0018,0088) = %.10f\nUsing (0018,0088) as slice step.\n",
                   slice_thickness, slice_spacing);
        }
        sep =  slice_spacing;
    }
    return sep;
}


void calculate_dircos(double RowColVec[6], double dircos[VOL_NDIMS][WORLD_NDIMS],
                      int do_coordinate_conversion){

        dircos[VCOLUMN][XCOORD] = RowColVec[0];
        dircos[VCOLUMN][YCOORD] = RowColVec[1];
        dircos[VCOLUMN][ZCOORD] = RowColVec[2];

        dircos[VROW][XCOORD] = RowColVec[3];
        dircos[VROW][YCOORD] = RowColVec[4];
        dircos[VROW][ZCOORD] = RowColVec[5];

        if(do_coordinate_conversion){
            convert_dicom_coordinate(dircos[VROW]);
            convert_dicom_coordinate(dircos[VCOLUMN]);
        }

        /* slice direction unit vector is cross product of row,
           col vectors:
         */
        dircos[VSLICE][XCOORD] =
            dircos[VCOLUMN][YCOORD] * dircos[VROW][ZCOORD] -
            dircos[VCOLUMN][ZCOORD] * dircos[VROW][YCOORD];

        dircos[VSLICE][YCOORD] =
            dircos[VCOLUMN][ZCOORD] * dircos[VROW][XCOORD] -
            dircos[VCOLUMN][XCOORD] * dircos[VROW][ZCOORD];

        dircos[VSLICE][ZCOORD] =
            dircos[VCOLUMN][XCOORD] * dircos[VROW][YCOORD] -
            dircos[VCOLUMN][YCOORD] * dircos[VROW][XCOORD];
}


/* ----------------------------- MNI Header -----------------------------------
   @NAME       : get_coordinate_info
   @INPUT      : group_list - input data
   sizes - size of each spatial dimension
   @OUTPUT     : fi_ptr - file-specific info
   volume_to_world - volume index to world coordinate index mapping
   dircos - direction cosines for spatial dimensions
   steps - step sizes for spatial dimensions
   starts - start positions for spatial dimensions (for a slice)
   coordinate - coordinate of centre of slice
   @RETURNS    : (nothing)
   @DESCRIPTION: Routine to get coordinate information for a slice from
   a group list
   @METHOD     :
   @GLOBALS    :
   @CALLS      :
   @CREATED    : February 28, 1997 (Peter Neelin)
   @MODIFIED   :
   ---------------------------------------------------------------------------- */

static void
get_coordinate_info(Acr_Group group_list,
                    File_Info *fi_ptr,
                    Orientation *orientation,
                    World_Index volume_to_world[VOL_NDIMS],
                    const int sizes[VOL_NDIMS],
                    double dircos[VOL_NDIMS][WORLD_NDIMS],
                    double steps[VOL_NDIMS],
                    double starts[VOL_NDIMS],
                    double coordinate[WORLD_NDIMS])
{
    Volume_Index ivolume;
    World_Index iworld;
    int found_dircos[VOL_NDIMS];
    int found_coordinate;
    double frame_time;
    double start_time;
    double magnitude;
    double largest;
    double psize[IMAGE_NDIMS];
    double slice_thickness, slice_spacing;

    double RowColVec[6]; /* row/column unit vectors in  dicom element */

    const Orientation orientation_list[WORLD_NDIMS] = {
        SAGITTAL, CORONAL, TRANSVERSE
    };

    if (G.Debug >= HI_LOGGING) {
        printf("get_coordinate_info\n");
    }

    /* Initialize a few things... */
    for (ivolume = 0; ivolume < VOL_NDIMS; ivolume++) {
        found_dircos[ivolume] = FALSE;
    }
    found_coordinate = FALSE;

#if 0
    /* TODO: For now this appears to be necessary.  In cases I don't fully
     * understand, the Siemens Numaris 3 DICOM image orientation does not
     * give the correct direction cosines, so we use the nonstandard Siemens
     * fields instead.  Someday I should figure out the relation (if any)
     * between the standard fields and these fields, and try to normalize
     * this mess.
     *
     * We only attempt this for files that are clearly marked as SIEMENS
     * files, with a version string that looks like VB33 (VB33D, VB33G, etc.)
     * Later versions do not seem to use these fields.
     */
    if (is_numaris3(group_list)) {
        Acr_Element_Id dircos_elid[VOL_NDIMS];

        /* Set direction cosine element ids. Note that the reversal of
         * rows and columns is intentional - their idea of the meaning
         * of theses labels is different from ours. (Their row vector
         * points along the row and not along the row dimension.)
         */

        dircos_elid[VSLICE] = SPI_Image_normal;
        dircos_elid[VROW] = SPI_Image_column;
        dircos_elid[VCOLUMN] = SPI_Image_row;

        /* Get direction cosines
         */
        for (ivolume = 0; ivolume < VOL_NDIMS; ivolume++) {
            element = acr_find_group_element(group_list, dircos_elid[ivolume]);
            if (element == NULL) {
                continue;
            }
            if (acr_get_element_numeric_array(element, WORLD_NDIMS,
                                              dircos[ivolume]) != WORLD_NDIMS) {
                continue;
            }
            /* negate the X and Z coordinates
             */
            convert_numa3_coordinate(dircos[ivolume]);
            found_dircos[ivolume] = TRUE;
        }
    }
#endif

    /* If we did not find the Siemens proprietary image vectors, try
     * the DICOM standard image position.
     */
    if (!found_dircos[VCOLUMN] || !found_dircos[VROW] || !found_dircos[VSLICE]) {
        if (dicom_read_orientation(group_list, RowColVec)) {
            calculate_dircos(RowColVec, dircos, 1);
            found_dircos[VCOLUMN] = TRUE;
            found_dircos[VROW] = TRUE;
            found_dircos[VSLICE] = TRUE;
        }
    }

    if (G.Debug >= HI_LOGGING) {
        printf("dircos %f %f %f %f %f %f %f %f %f\n",
               dircos[VSLICE][XCOORD],
               dircos[VSLICE][YCOORD],
               dircos[VSLICE][ZCOORD],
               dircos[VROW][XCOORD],
               dircos[VROW][YCOORD],
               dircos[VROW][ZCOORD],
               dircos[VCOLUMN][XCOORD],
               dircos[VCOLUMN][YCOORD],
               dircos[VCOLUMN][ZCOORD]);
    }

    /* Normalize the direction cosines
     */
    for (ivolume = 0; ivolume < VOL_NDIMS; ivolume++) {
        magnitude = 0.0;
        for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
            magnitude += dircos[ivolume][iworld] * dircos[ivolume][iworld];
        }
        if (magnitude <= 0) {
            found_dircos[ivolume] = FALSE;
            continue;
        }
        magnitude = sqrt(magnitude);
        for (iworld=0; iworld < WORLD_NDIMS; iworld++) {
            dircos[ivolume][iworld] /= magnitude;
        }
    }

    /* If we don't find direction cosines, then assume transverse volume
     */
    if (!found_dircos[VSLICE] ||
        !found_dircos[VROW] ||
        !found_dircos[VCOLUMN]) {

        if (G.Debug) {
            printf("Using default direction cosines\n");
        }

        for (ivolume = 0; ivolume < VOL_NDIMS; ivolume++) {
            for (iworld = 0; iworld < WORLD_NDIMS; iworld++) {
                dircos[ivolume][iworld] =
                    ((ivolume == (WORLD_NDIMS-iworld-1)) ? -1.0 : 0.0);
                found_dircos[iworld] = TRUE;
            }
        }
    }

    /* Figure out volume index to world index mapping and sign of direction
     * cosines - the code below figures out the primary direction in x,y,z
     * of each volume coordinate (row,col,slice)
     */
    for (ivolume = 0; ivolume < VOL_NDIMS; ivolume++) {
        largest = -1.0;
        for (iworld = 0; iworld < WORLD_NDIMS; iworld++) {
            magnitude = dircos[ivolume][iworld];
            if (magnitude < 0.0) magnitude = -magnitude;
            if (magnitude > largest) {
                largest = magnitude;
                volume_to_world[ivolume] = iworld;
            }
        }
    }

    if (G.Debug >= HI_LOGGING) {
        printf(" Volume_to_world slice=%s row=%s column=%s\n",
               World_Names[volume_to_world[VSLICE]],
               World_Names[volume_to_world[VROW]],
               World_Names[volume_to_world[VCOLUMN]]);
    }

    /* Get orientation (depends on primary direction of slice normal)
     */
    *orientation = orientation_list[volume_to_world[VSLICE]];
    if (G.Debug >= HI_LOGGING) {
        printf(" Orientation is %s\n",
               (*orientation == SAGITTAL) ? "SAGITTAL" :
               (*orientation == CORONAL) ? "CORONAL" : "TRANSVERSE");
    }

    int curr_dim, prev_dim;
    for (ivolume = 0; ivolume < VOL_NDIMS; ivolume++) {
        curr_dim = volume_to_world[ivolume];
        if ( ivolume > 0 && prev_dim == curr_dim) {
            /* By default use these settings, orientation likely unreliable if it's messed up this way */
            volume_to_world[VSLICE] = ZCOORD;
            volume_to_world[VROW] = YCOORD;
            volume_to_world[VCOLUMN] = XCOORD;
            break;
        }
        prev_dim = curr_dim;
    }

    /* Get step information
     */

    dicom_read_pixel_size(group_list, psize);

    steps[VCOLUMN] = psize[0];
    steps[VROW] = psize[1];     /* anisotropic resolution? */

    steps[VSLICE] = get_slice_separation(group_list);

    /* Make sure that direction cosines point the right way (dot
     * product of direction cosine and axis is positive) and that step
     * has proper sign.
     */
    for (ivolume = 0; ivolume < VOL_NDIMS; ivolume++) {
        iworld = volume_to_world[ivolume];
        if (dircos[ivolume][iworld] < 0.0) {
            if (G.Debug >= HI_LOGGING) {
                printf("Swapping direction of %s %s\n",
                       Volume_Names[ivolume],
                       World_Names[iworld]);
            }
            steps[ivolume] *= -1.0;
            for (iworld = 0; iworld < WORLD_NDIMS; iworld++) {
                dircos[ivolume][iworld] *= -1.0;
            }
        }
    }

    /* Find 3D coordinate of slice - ACR_Image_position_patient gives
     * the *corner* of the slice!
     *
     * Start by assuming that we didn't find it.
     */
    found_coordinate = FALSE;
    for (iworld = 0; iworld < WORLD_NDIMS; iworld++) {
        coordinate[iworld] = 0.0;
    }

    if (G.opts & OPTS_NO_LOCATION) {
        /* If the coordinates are untrustworthy, just generate something
         * reasonable for the slice coordinate.  Ignore the rest.
         */
        coordinate[volume_to_world[VSLICE]] =
            (steps[VSLICE] * fi_ptr->index[SLICE]);
        found_coordinate = TRUE;
    }
    else {
        found_coordinate = dicom_read_position(group_list, 
                                               -1, /* ?? */
                                               coordinate);
        if (!found_coordinate) {
            /* Last gasp - try to interpret the slice location as our slice
             * position.  It might work.
             */
            if (!found_coordinate) {
                coordinate[volume_to_world[VSLICE]] =
                    (double)acr_find_double(group_list, ACR_Slice_location, 1.0);
            }

            found_coordinate = TRUE;
        }
    }

    convert_dicom_coordinate(coordinate);

    /* Work out start positions in volume coordinates
     */
    for (ivolume=0; ivolume < VOL_NDIMS; ivolume++) {

        if (found_coordinate &&
            found_dircos[VSLICE] &&
            found_dircos[VROW] &&
            found_dircos[VCOLUMN]) {
            starts[ivolume] =
                coordinate[XCOORD] * dircos[ivolume][XCOORD] +
                coordinate[YCOORD] * dircos[ivolume][YCOORD] +
                coordinate[ZCOORD] * dircos[ivolume][ZCOORD];
        }
        else {
            starts[ivolume] = 0.0;
        }
    }

    if (G.Debug >= HI_LOGGING) {
        printf(" coordinate %f %f %f, start %f %f %f\n",
               coordinate[XCOORD], coordinate[YCOORD], coordinate[ZCOORD],
               starts[VROW], starts[VCOLUMN], starts[VSLICE]);
    }

    /* Find position along each dimension
     */
    fi_ptr->coordinate[SLICE] = starts[VSLICE];
    fi_ptr->coordinate[ECHO] =
        (double)acr_find_double(group_list, ACR_Echo_time, 0.0) / MS_PER_SECOND;


    /* Get the dimension width for time, if available.  The units are in
     * milliseconds in DICOM, whereas we use seconds in MINC.
     */
    fi_ptr->width[TIME] = (double)acr_find_double(group_list,
                                                  ACR_Actual_frame_duration,
                                                  0.0) / MS_PER_SECOND;

    /* PET scan times (bert)
     */
    frame_time = acr_find_double(group_list, ACR_Trigger_time, -1.0);
    if (frame_time >= 0.0 && !G.bogus_trigger_time) {
      frame_time /= 1000.0;
      start_time = 0;
    }
    else {
      start_time = (double)acr_find_double(group_list, ACR_Frame_reference_time, -1.0);
      frame_time = (double)acr_find_double(group_list, ACR_Actual_frame_duration, -1.0);
      if (start_time >= 0.0 && frame_time > 0.0) {
        if (G.adjust_frame_time) {
            double new_time = (start_time - frame_time / 2.0) / 1000.0;
            if (new_time < -1.0) {
              fprintf(stderr, "WARNING: adjusting the frame time gives a negative time.\n");
              fprintf(stderr, "         %g %g\n", start_time, frame_time);
            }
            frame_time = new_time;
        }
        else {
            frame_time = start_time / 1000.0; /* Convert msec to seconds. */
        }
      }
      else {
        /* Try to use the repetition time if it is legit. For now we
         * make this test very restrictive.
         */
        double tr = acr_find_double(group_list, ACR_Repetition_time, -1.0);
        char *str_seq = acr_find_string(group_list, ACR_Scanning_sequence, "");
        char *str_var = acr_find_string(group_list, ACR_Sequence_variant, "");
        if (tr > 0.0 && !strcmp(str_seq, "EP") && !strcmp(str_var, "SK")) {
          frame_time = (fi_ptr->index[TIME] - 1) * tr / 1000.0;
        }
        else {
          /* time section (rhoge)
           * now assume that time has been fixed when file was read
           */
          start_time = (double)acr_find_double(group_list, ACR_Series_time, 0.0);
          frame_time = (double)acr_find_double(group_list, ACR_Acquisition_time, 0.0);
          if (frame_time == 0.0) {
            /* See if the acquisition datetime contains anything useful. If so
             * we will use it.
             */
            char *dt_str = acr_find_string(group_list, ACR_Acquisition_datetime, "");
            if (dt_str[0] != 0) {
              /* If we got the string, it is of the format:
               *     YYYYMMDDHHMMSS.FFFFFF
               * We skip the first eight characters because we don't
               * care about the date.
               */
              frame_time = atof(dt_str + 8);
              /* TODO: We would like a better way of getting the start time,
               * but there don't seem to be other fields that are 100% reliable.
               */
              start_time = 0;
            }
          }

          start_time = convert_time_to_seconds(start_time);
          frame_time = convert_time_to_seconds(frame_time) - start_time;

          /* check for case where scan starts right before midnight,
           * but frame is after midnight
           */
          if (start_time < 600.0 && frame_time < 0.0) {
            frame_time += SECONDS_PER_DAY;
          }
        }
      }
    }
    fi_ptr->coordinate[TIME] = frame_time;

    /* end of time section */

    fi_ptr->coordinate[PHASE] = 0.0;
    fi_ptr->coordinate[CHEM_SHIFT] = 0.0;

}

#if 0
/* ----------------------------- MNI Header -----------------------------------
   @NAME       : convert_numa3_coordinate
   @INPUT      : coordinate
   @OUTPUT     : coordinate
   @RETURNS    : (nothing)
   @DESCRIPTION: Routine to convert a coordinate to the correct orientation
   @METHOD     :
   @GLOBALS    :
   @CALLS      :
   @CREATED    : February 28, 1997 (Peter Neelin)
   @MODIFIED   : made version specific to Numaris 3 SPI data (rhoge)
   ---------------------------------------------------------------------------- */
static void
convert_numa3_coordinate(double coordinate[WORLD_NDIMS])
{
    coordinate[XCOORD] = -coordinate[XCOORD];
    coordinate[ZCOORD] = -coordinate[ZCOORD];
}
#endif

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : convert_dicom_coordinate
   @INPUT      : coordinate
   @OUTPUT     : coordinate
   @RETURNS    : (nothing)
   @DESCRIPTION: Routine to convert a coordinate to the correct orientation
   @METHOD     :
   @GLOBALS    :
   @CALLS      :
   @CREATED    : February 28, 1997 (Peter Neelin)
   @MODIFIED   : made new dicom version (rhoge)
   ---------------------------------------------------------------------------- */
void
convert_dicom_coordinate(double coordinate[WORLD_NDIMS])
{
    /* Allow the user to override this, if only for debugging purposes...
     */
    if (G.opts & OPTS_KEEP_COORD) {
        return;
    }

    coordinate[XCOORD] = -coordinate[XCOORD];
    coordinate[YCOORD] = -coordinate[YCOORD];
}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : get_general_header_info
   @INPUT      : group_list - input data
   @OUTPUT     : gi_ptr - general information about files
   @RETURNS    : (nothing)
   @DESCRIPTION: Routine to extract general header information from a group list
   @METHOD     :
   @GLOBALS    :
   @CALLS      :
   @CREATED    : February 28, 1997 (Peter Neelin)
   @MODIFIED   :
   ---------------------------------------------------------------------------- */

static void
get_string_field(char *out_str, Acr_Group group_list,
                 Acr_Element_Id element_id)
{
   strncpy(out_str, acr_find_string(group_list, element_id, ""), STRING_T_LEN);
}

void
get_general_header_info(Acr_Group group_list, General_Info *gi_ptr)
{
    int length;
    char *string;

    if (G.Debug) {
        printf("SOP Class UID: %s\n",
               acr_find_string(group_list, ACR_SOP_Class_UID, ""));
        printf("Images in acquisition: %d\n",
               acr_find_int(group_list, ACR_Images_in_acquisition, -1));
        printf("Acquisitions in series: %d\n",
               acr_find_int(group_list, ACR_Acquisitions_in_series, -1));
        printf("3D raw partitions: %d\n",
               acr_find_int(group_list, SPI_Number_of_3D_raw_partitions_nominal, -1));
    }
    /* Get intensity units */
    strncpy(gi_ptr->units, "", STRING_T_LEN);

    /* Get patient info */
    get_string_field(gi_ptr->patient.name, group_list, ACR_Patient_name);
    fix_patient_name(gi_ptr->patient.name);

    get_string_field(gi_ptr->patient.identification,
                     group_list, ACR_Patient_identification);
    get_string_field(gi_ptr->patient.birth_date,
                     group_list, ACR_Patient_birth_date);

    get_string_field(gi_ptr->patient.age,
                     group_list, ACR_Patient_age);

    string = (char*)acr_find_string(group_list, ACR_Patient_sex, "");
    if (*string == 'M')
        strncpy(gi_ptr->patient.sex, MI_MALE, STRING_T_LEN);
    else if (*string == 'F')
        strncpy(gi_ptr->patient.sex, MI_FEMALE, STRING_T_LEN);
    else if (*string == 'O')
        strncpy(gi_ptr->patient.sex, MI_OTHER, STRING_T_LEN);
    else
        strncpy(gi_ptr->patient.sex, "", STRING_T_LEN);

    gi_ptr->patient.weight =
        (double)acr_find_double(group_list, ACR_Patient_weight, -DBL_MAX);

    /* added by rhoge - registration timing info */
    get_string_field(gi_ptr->patient.reg_date,
                     group_list, ACR_Study_date);

    get_string_field(gi_ptr->patient.reg_time,
                     group_list, ACR_Study_time);

    get_string_field(gi_ptr->patient.position, /*position of patient added by ilana*/
                     group_list, ACR_Patient_position);

    /* Get study info */

    /*some more timing info added by ilana*/
    get_string_field(gi_ptr->acq.series_time, group_list, ACR_Series_time);

    get_string_field(gi_ptr->study.start_time,
                     group_list, ACR_Study_date);

    length = strlen(gi_ptr->study.start_time);
    gi_ptr->study.start_time[length] = ' ';
    length++;
    strncpy(&gi_ptr->study.start_time[length],
            acr_find_string(group_list, ACR_Study_time, ""), STRING_T_LEN - length);
    string = (char*)acr_find_string(group_list, ACR_Modality, "");
    if (strcmp(string, ACR_MODALITY_MR) == 0)
        strncpy(gi_ptr->study.modality, MI_MRI, STRING_T_LEN);
    else if (strcmp(string, ACR_MODALITY_PT) == 0)
        strncpy(gi_ptr->study.modality, MI_PET, STRING_T_LEN);
    get_string_field(gi_ptr->study.manufacturer,
                     group_list, ACR_Manufacturer);
    get_string_field(gi_ptr->study.model,
                     group_list, ACR_Manufacturer_model);
    gi_ptr->study.field_value =
        (double)acr_find_double(group_list, ACR_Magnetic_field_strength, -DBL_MAX);
    get_string_field(gi_ptr->study.software_version,
                     group_list, ACR_Software_versions);
    get_string_field(gi_ptr->study.serial_no,
                     group_list, ACR_Device_serial_number);
    get_string_field(gi_ptr->study.calibration_date,
                     group_list, ACR_Calibration_date);
    get_string_field(gi_ptr->study.calibration_time, /*add time as well ilana*/
                     group_list, ACR_Calibration_time);
    get_string_field(gi_ptr->study.institution,
                     group_list, ACR_Institution_id);
    get_string_field(gi_ptr->study.station_id,
                     group_list, ACR_Station_id);
    get_string_field(gi_ptr->study.referring_physician,
                     group_list, ACR_Referring_physician);
    get_string_field(gi_ptr->study.performing_physician,
                     group_list, ACR_Performing_physician);
    get_string_field(gi_ptr->study.operator,
                     group_list, ACR_Operators_name);
    get_string_field(gi_ptr->study.procedure,
                     group_list, ACR_Procedure_description);
    sprintf(gi_ptr->study.study_id, "%.6f",gi_ptr->study_id);

    /* Acquisition id modified by rhoge to get rid of first digit that
       is not required for identification of run */
    /*   sprintf(gi_ptr->study.acquisition_id, "%d_%d",
         acr_find_int(group_list, ACR_Series, 0), gi_ptr->acq_id); */
    sprintf(gi_ptr->study.acquisition_id, "%d", gi_ptr->acq_id);



    /* Get acquisition information */

    get_string_field(gi_ptr->acq.acquisition_time, group_list, ACR_Acquisition_time); /*add acquisition start time ilana*/
    get_string_field(gi_ptr->acq.image_time, group_list, ACR_Image_time);
    get_string_field(gi_ptr->acq.scan_seq, group_list, ACR_Sequence_name);
    get_string_field(gi_ptr->acq.protocol_name, group_list, ACR_Protocol_name);
    get_string_field(gi_ptr->acq.series_description,         /*add series description ilana*/
                     group_list, ACR_Series_description);
    get_string_field(gi_ptr->acq.receive_coil, group_list,
                     ACR_Receive_coil_name);
    get_string_field(gi_ptr->acq.transmit_coil, group_list,
                     ACR_Transmit_coil_name);
    get_string_field(gi_ptr->acq.slice_order, group_list,
		     EXT_Slice_order);
      /*0x1 means ASCENDING
	0x2 means DESCENDING
	0x4 means INTERLEAVED*/
    if(!strcmp(gi_ptr->acq.slice_order,"0x1 "))
	strncpy(gi_ptr->acq.slice_order, "ascending", STRING_T_LEN);
    else if(!strcmp(gi_ptr->acq.slice_order,"0x2 "))
	strncpy(gi_ptr->acq.slice_order, "descending", STRING_T_LEN);
    else if(!strcmp(gi_ptr->acq.slice_order,"0x4 "))
	strncpy(gi_ptr->acq.slice_order, "interleaved", STRING_T_LEN);

    gi_ptr->acq.rep_time =
        (double)acr_find_double(group_list, ACR_Repetition_time, -DBL_MAX);
    if (gi_ptr->acq.rep_time != -DBL_MAX)
        gi_ptr->acq.rep_time /= 1000.0;

    gi_ptr->acq.echo_time =
        (double)acr_find_double(group_list, ACR_Echo_time, -DBL_MAX);
    if (gi_ptr->acq.echo_time != -DBL_MAX)
        gi_ptr->acq.echo_time /= 1000.0;

    gi_ptr->acq.echo_train_length =
        (double)acr_find_double(group_list, ACR_Echo_train_length, -DBL_MAX); /*added echo train length ilana*/

    gi_ptr->acq.echo_number =
        (double)acr_find_double(group_list, ACR_Echo_number, -DBL_MAX);

    gi_ptr->acq.inv_time =
        (double)acr_find_double(group_list, ACR_Inversion_time, -DBL_MAX);
    if (gi_ptr->acq.inv_time != -DBL_MAX)
        gi_ptr->acq.inv_time /= 1000.0;
    gi_ptr->acq.delay_in_TR =
        (double)acr_find_double(group_list, EXT_Delay_in_TR, -DBL_MAX);  /*added delay in TR ilana*/
    if (gi_ptr->acq.delay_in_TR != -DBL_MAX)
        gi_ptr->acq.delay_in_TR /= 1000000.0; /*write in seconds*/
    gi_ptr->acq.b_value =
        (double)acr_find_double(group_list, EXT_Diffusion_b_value, -DBL_MAX);
    gi_ptr->acq.flip_angle =
        (double)acr_find_double(group_list, ACR_Flip_angle, -DBL_MAX);
    gi_ptr->acq.slice_thickness =
        (double)acr_find_double(group_list, ACR_Slice_thickness, -DBL_MAX);
    gi_ptr->acq.num_slices =
        (double)acr_find_double(group_list, ACR_Images_in_acquisition, -DBL_MAX);
    gi_ptr->acq.num_dyn_scans =
        (double)acr_find_double(group_list, ACR_Acquisitions_in_series, -DBL_MAX);
    gi_ptr->acq.num_avg =
        (double)acr_find_double(group_list, ACR_Nr_of_averages, -DBL_MAX);
    gi_ptr->acq.imaging_freq =
        (double)acr_find_double(group_list, ACR_Imaging_frequency, -DBL_MAX);
    if (gi_ptr->acq.imaging_freq != -DBL_MAX)
        gi_ptr->acq.imaging_freq *= 1e6;
    get_string_field(gi_ptr->acq.imaged_nucl,
                     group_list, ACR_Imaged_nucleus);
    gi_ptr->acq.win_center =
        (double)acr_find_double(group_list, ACR_Window_centre, -DBL_MAX);
    gi_ptr->acq.win_width =
        (double)acr_find_double(group_list, ACR_Window_width, -DBL_MAX);

    gi_ptr->acq.num_phase_enc_steps =
        (double)acr_find_double(group_list, ACR_Number_of_phase_encoding_steps, -DBL_MAX);
    gi_ptr->acq.percent_sampling =
		    (double)acr_find_double(group_list, ACR_Percent_sampling, -DBL_MAX); /*don't need to multiply by 100 ilana*/

    gi_ptr->acq.percent_phase_fov =
		    (double)acr_find_double(group_list, ACR_Percent_phase_field_of_view, -DBL_MAX); /*don't need to multiply by 100 ilana*/

    gi_ptr->acq.pixel_bandwidth =
        (double)acr_find_double(group_list, ACR_Pixel_bandwidth, -DBL_MAX);

    gi_ptr->acq.sar = (double)acr_find_double(group_list, ACR_SAR, -DBL_MAX);

    get_string_field(gi_ptr->acq.mr_acq_type,
                     group_list, ACR_MR_acquisition_type);

    get_string_field(gi_ptr->acq.image_type, group_list, ACR_Image_type);
    if (G.Debug) {
        if (strstr(gi_ptr->acq.image_type, "MOSAIC") != NULL) {
            printf("This appears to be a Mosaic image\n");
        }
    }

    get_string_field(gi_ptr->acq.phase_enc_dir,
                     group_list, ACR_Phase_encoding_direction);

    /*Add image comments*/
    /*strncpy(gi_ptr->acq.comments, "", STRING_T_LEN);*/
    get_string_field(gi_ptr->acq.comments,
                     group_list, ACR_Image_comments);

    string = (char*)acr_find_string(group_list, ACR_Acquisition_contrast, "");
    if (string[0] == 0) {
      /* In at least one case, Philips makes you look inside of a proprietary
       * sequence field in order to find this value. I have no idea why.
       */
      Acr_Element el = acr_recurse_for_element(group_list, 0,
                                               PMS_Acquisition_parameters_seq,
                                               ACR_Acquisition_contrast);
      if (el != NULL)
        string = acr_get_element_string(el);
      else
        string = acr_find_string(group_list, ACR_Image_type, "");
    }
    gi_ptr->acq.dti = (strstr(string, "DIFFUSION") != NULL);

    get_pet_info(group_list, &gi_ptr->pet);
}

/** Get GE Medical systems private PET information fields.
 */
static void
get_gems_pet_info(Acr_Group group_list, struct Pet_info *pet_ptr)
{
  string_t temp;

  get_string_field(temp, group_list, GEMS_Pet_private_creator_id);
  if (strcmp(temp, "GEMS_PETD_01") != 0)
    return;

  printf("\nUsing GE-specific PET information.\n");
  get_string_field(pet_ptr->tracer, group_list, GEMS_Pet_tracer_name);
  get_string_field(pet_ptr->radionuclide, group_list, GEMS_Pet_radionuclide_name);
  get_string_field(pet_ptr->injection_time, group_list, GEMS_Pet_admin_datetime);
  pet_ptr->radionuclide_halflife = acr_find_double(group_list,
                                                   GEMS_Pet_halflife, -1.0);
  pet_ptr->positron_fraction = acr_find_double(group_list,
                                               GEMS_Pet_positron_fraction, -1.0);
  /* TODO: Dose and route information does not seem to be present?? */
}

/* Convert DTI vectors from scanner coordinates to image frame of reference */
static void do_gradient_transformation(Acr_Group group_list, double bvecs_new[3]) {

    double orient[6], sum = 0.0;
    Acr_Double old_bvec[WORLD_NDIMS];
    int wdim, vdim, vol_ndim = VOL_NDIMS, v;

    Acr_Element element = acr_find_group_element(group_list, ACR_Diffusion_gradient_orientation);

    if (element == NULL || acr_get_element_double_array(element, WORLD_NDIMS, old_bvec) != WORLD_NDIMS) {
        old_bvec[XCOORD] = 0.0;
        old_bvec[YCOORD] = 0.0;
        old_bvec[ZCOORD] = 0.0;
    }

    /* Get 6 orientations from 0020,0037 */
    dicom_read_orientation(group_list, orient);

    double orientation_vector[VOL_NDIMS][WORLD_NDIMS];

    orientation_vector[VCOLUMN][XCOORD] = orient[0];
    orientation_vector[VCOLUMN][YCOORD] = orient[1];
    orientation_vector[VCOLUMN][ZCOORD] = orient[2];
    orientation_vector[VROW][XCOORD] = orient[3];
    orientation_vector[VROW][YCOORD] = orient[4];
    orientation_vector[VROW][ZCOORD] = orient[5];

    orientation_vector[VSLICE][XCOORD] = orientation_vector[VCOLUMN][YCOORD] * orientation_vector[VROW][ZCOORD] -
                              orientation_vector[VCOLUMN][ZCOORD] * orientation_vector[VROW][YCOORD];
    orientation_vector[VSLICE][YCOORD] = orientation_vector[VCOLUMN][ZCOORD] * orientation_vector[VROW][XCOORD] -
                              orientation_vector[VCOLUMN][XCOORD] * orientation_vector[VROW][ZCOORD];
    orientation_vector[VSLICE][ZCOORD] = orientation_vector[VCOLUMN][XCOORD] * orientation_vector[VROW][YCOORD] -
                              orientation_vector[VCOLUMN][YCOORD] * orientation_vector[VROW][XCOORD];

    /*  undoing the bit in copy_spi_to_acr() */
    if (strstr(acr_find_string(group_list, ACR_Manufacturer, ""), "SIEMENS") != NULL || strstr(acr_find_string(group_list, ACR_Manufacturer, ""), "Siemens") != NULL){
        old_bvec[ZCOORD] = -1*old_bvec[ZCOORD];
    }
    /* Compute new bvec by computing the dot product of bvecs and orientation vectors */
    for (vdim = 0; vdim < VOL_NDIMS; vdim++) {
             vol_ndim--;
             for (wdim = 0; wdim < WORLD_NDIMS; wdim++) {
                sum += orientation_vector[vol_ndim][wdim] * old_bvec[wdim];
                 if (wdim == WORLD_NDIMS-1) {
                    bvecs_new[vdim] = sum;
                    /* Reset sum */
                    sum = 0.0;
                 }
             }
      }

      /* Do vector length normalization */
      double vector_length = sqrt((bvecs_new[0]*bvecs_new[0])+(bvecs_new[1]*bvecs_new[1])+(bvecs_new[2]*bvecs_new[2]));
      double bvn[3]; bvn[0] = bvecs_new[0]; bvn[1] = bvecs_new[1]; bvn[2] = bvecs_new[2];
      /* Avoid division by zero */
      if (vector_length != 0) {
          for (v = 0; v < WORLD_NDIMS; v++) {
              bvecs_new[v] = bvn[v]/vector_length;
          }
      }

    if (bvecs_new[YCOORD] != 0) {
        bvecs_new[YCOORD] = -1*bvecs_new[YCOORD];
    }

    /* Compute determinant of the orientation and check if spatial reflection is needed  */
    if (is_siemens_mosaic(group_list)) {
        int orientation = acr_find_int(group_list, EXT_Slice_orientation, -1);

        double det = (orient[0]*orient[4]*G.sliceNormV3)-(orient[0]*orient[5]*G.sliceNormV2)-(orient[1]*orient[3]*G.sliceNormV3)+(orient[1]*orient[5]*G.sliceNormV1)+(orient[2]*orient[3]*G.sliceNormV2)-(orient[2]*orient[4]*G.sliceNormV1);
        /* Do spatial reflection only when we obtain a negative determinant for siemens mosaic */
        if (det < 0) {
            if (bvecs_new[YCOORD] != 0) {
                bvecs_new[YCOORD] = -1*bvecs_new[YCOORD];
            }
        }
    }
}

static void
get_standard_pet_info(Acr_Group group_list, Acr_Element sequence,
                      struct Pet_info *pet_ptr)
{
  Acr_Element element;
  int n;

  n = 0;
  element = acr_recursive_search( sequence, &n, ACR_Radiopharmaceutical);
  if (element != NULL) {
    strncpy(pet_ptr->tracer, acr_get_element_string(element),
            STRING_T_LEN);
  }
  else {
    pet_ptr->tracer[0] = 0;
  }

  n = 0;
  element = acr_recursive_search(sequence, &n,
                                 ACR_Radiopharmaceutical_start_datetime);
  if (element != NULL) {
    strncpy(pet_ptr->injection_time, acr_get_element_string(element),
            STRING_T_LEN);
  }
  else {
    n = 0;
    element = acr_recursive_search(sequence, &n,
                                   ACR_Radiopharmaceutical_start_time);
    if (element != NULL) {
      string_t time_string;
      string_t date_string;
      get_string_field(date_string, group_list, ACR_Acquisition_date);
      strncpy(time_string, acr_get_element_string(element), STRING_T_LEN);
      sprintf(pet_ptr->injection_time, "%s%s", 
              date_string, time_string);
    }
  }

  n = 0;
  element = acr_recursive_search(sequence, &n, ACR_Radionuclide_half_life);
  if (element != NULL) {
    pet_ptr->radionuclide_halflife = acr_get_element_numeric(element);
  }
  else {
    pet_ptr->radionuclide_halflife = -1;
  }

  n = 0;
  element = acr_recursive_search(sequence, &n, ACR_Radionuclide_total_dose);
  if (element != NULL) {
    pet_ptr->injection_dose = acr_get_element_numeric(element);
  }
  else {
    pet_ptr->injection_dose = -1;
  }

  n = 0;
  element = acr_recursive_search(sequence, &n, ACR_Radiopharmaceutical_volume);
  if (element != NULL) {
    pet_ptr->injection_volume = acr_get_element_numeric(element);
  }
  else {
    pet_ptr->injection_volume = -1;
  }

  n = 0;
  element = acr_recursive_search(sequence, &n,
                                 ACR_Radionuclide_positron_fraction);
  if (element != NULL) {
    pet_ptr->positron_fraction = acr_get_element_numeric(element);
  }
  else {
    pet_ptr->positron_fraction = -1;
  }

  n = 0;
  element = acr_recursive_search(sequence, &n,
                                 ACR_Radiopharmaceutical_route);
  if (element != NULL) {
    strncpy(pet_ptr->injection_route, acr_get_element_string(element),
            STRING_T_LEN);
  }
  else {
    pet_ptr->injection_route[0] = 0;
  }

  pet_ptr->radionuclide[0] = 0;

  n = 0;
  element = acr_recursive_search(sequence, &n, ACR_Radionuclide_code_sequence);
  if (element != NULL) {
    Acr_Element el2;

    n = 0;
    el2 = acr_recursive_search((Acr_Element) acr_get_element_data(element),
                               &n, ACR_Code_meaning);
    if (el2 != NULL) {
      strncpy(pet_ptr->radionuclide, acr_get_element_string(el2),
              STRING_T_LEN);
    }
  }
}

static void
get_pet_info(Acr_Group group_list, struct Pet_info *pet_ptr)
{
  Acr_Element sequence;

  get_gems_pet_info(group_list, pet_ptr);

  sequence = acr_find_group_element(group_list,
                       ACR_Radiopharmaceutical_information_sequence);

  if (sequence != NULL && acr_element_is_sequence(sequence)) {
    get_standard_pet_info(group_list,
                          (Acr_Element)acr_get_element_data(sequence),
                          pet_ptr);
  }

  if (pet_ptr->injection_time[0] != 0) {
    string_t temp;

    /* Convert DICOM datetime field to individual elements.
     */
    strncpy(temp, &pet_ptr->injection_time[0], 4);
    temp[4] = 0;
    pet_ptr->injection_year = atoi(temp);
    strncpy(temp, &pet_ptr->injection_time[4], 2);
    temp[2] = 0;
    pet_ptr->injection_month = atoi(temp);
    strncpy(temp, &pet_ptr->injection_time[6], 2);
    temp[2] = 0;
    pet_ptr->injection_day = atoi(temp);
    strncpy(temp, &pet_ptr->injection_time[8], 2);
    temp[2] = 0;
    pet_ptr->injection_hour = atoi(temp);
    strncpy(temp, &pet_ptr->injection_time[10], 2);
    temp[2] = 0;
    pet_ptr->injection_minute = atoi(temp);

    strncpy(temp, &pet_ptr->injection_time[12], sizeof(temp));
    pet_ptr->injection_seconds = atof(temp);
  }
}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : convert_time_to_seconds
   @INPUT      : dicom_time
   @OUTPUT     : (none)
   @RETURNS    : real time in seconds from beginning of day
   @DESCRIPTION: Routine to convert dicom seconds (decimal hhmmss.xxxxx) to
   real seconds since the start of day.
   @METHOD     :
   @GLOBALS    :
   @CALLS      :
   @CREATED    : February 28, 1997 (Peter Neelin)
   @MODIFIED   :
   ---------------------------------------------------------------------------- */
extern double
convert_time_to_seconds(double dicom_time)
{

    /* Variables */
    double hh, mm, ss;

    /* Get the components of the time */

    hh = (int) (dicom_time / DICOM_SECONDS_PER_HOUR);
    dicom_time -= hh * DICOM_SECONDS_PER_HOUR;
    mm = (int) (dicom_time / DICOM_SECONDS_PER_MINUTE);
    dicom_time -= mm * DICOM_SECONDS_PER_MINUTE;
    ss = dicom_time;

    /* Work out the number of seconds */

    return (hh * (double)SECONDS_PER_HOUR) + (mm * (double)SECONDS_PER_MINUTE) + ss;
}

/* ----------------------------- MNI Header -----------------------------------
   @NAME       : convert_seconds_to_time
   @INPUT      : real time in seconds from beginning of day
   @OUTPUT     : (none)
   @RETURNS    : dicom_time
   @DESCRIPTION: Routine to convert real seconds since start of day to dicom seconds (decimal hhmmss.xxxxx). Reverses the transform made by convert_time_to_seconds
   @METHOD     :
   @GLOBALS    :
   @CALLS      :
   @CREATED    : December 10, 2020 (Massine Yahia)
   @MODIFIED   :
   ---------------------------------------------------------------------------- */
extern double
convert_seconds_to_time(double seconds)
{
    /* Constants */

    /* Variables */
    double ss;
    int hh, mm;
    /* Get the components of the time */

    hh = (seconds / SECONDS_PER_HOUR);
    seconds -= hh * SECONDS_PER_HOUR;
    mm = (seconds / SECONDS_PER_MINUTE);
    seconds -= mm * SECONDS_PER_MINUTE;
    ss = seconds;

    /* Work out the number of seconds */

    return (hh * DICOM_SECONDS_PER_HOUR) + (mm * DICOM_SECONDS_PER_MINUTE) + ss;
}

#if JPEG_FOUND
#include <jpeglib.h>
unsigned char *
dicom_jpeg_decompress(unsigned char *jpg_buffer, int jpg_size)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  unsigned char *dicom_ptr;
  int dicom_len;
  int width, height, pixel_size;
  int row_stride;
  int rc;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  
  jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);

  rc = jpeg_read_header(&cinfo, TRUE);
  if (rc != 1) {
    printf("ERROR reading JPEG header.");
    return NULL;
  }

  if (!jpeg_start_decompress(&cinfo)) {
    printf("ERROR starting JPEG decompression.\n");
    return NULL;
  }
	
  width = cinfo.output_width;
  height = cinfo.output_height;
  pixel_size = cinfo.output_components;
  row_stride = width * pixel_size;
  dicom_len = width * height;
  dicom_ptr = malloc(dicom_len*pixel_size);
  if (dicom_ptr == NULL) {
    printf("ERROR allocating memory\n");
    return NULL;
  }

  while (cinfo.output_scanline < cinfo.output_height) {
    unsigned char *buffer_array[1];
    buffer_array[0] = dicom_ptr + cinfo.output_scanline * row_stride;

    rc = jpeg_read_scanlines(&cinfo, buffer_array, 1);
    printf("jpeg_read_scanlines: %d\n", rc);
  }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  return dicom_ptr;
}
#endif

#ifdef OPENJPEG_FOUND

#include <openjpeg.h>


/*copied from gdcmJPEG2000Codec.cxx*/

typedef struct {
  char *mem;
  char *cur;
  size_t len;
} myfile;

void openjpeg_error_callback(const char* msg, void* dummy)
{
  printf( "OpenJPEG Error: %s", msg );
}

#ifdef OPENJPEG2_FOUND

OPJ_SIZE_T opj_read_from_memory(void * p_buffer, OPJ_SIZE_T p_nb_bytes, myfile* p_file)
{
  OPJ_SIZE_T l_nb_read;
  if( p_file->cur + p_nb_bytes <= p_file->mem + p_file->len )
    {
    l_nb_read = 1*p_nb_bytes;
    }
  else
    {
    l_nb_read = (OPJ_SIZE_T)(p_file->mem + p_file->len - p_file->cur);
    }
  memcpy(p_buffer,p_file->cur,l_nb_read);
  p_file->cur += l_nb_read;
  return l_nb_read ? l_nb_read : ((OPJ_SIZE_T)-1);
}

OPJ_SIZE_T opj_write_from_memory (void * p_buffer, OPJ_SIZE_T p_nb_bytes, myfile* p_file)
{
  OPJ_SIZE_T l_nb_write;
  l_nb_write = 1*p_nb_bytes;
  memcpy(p_file->cur,p_buffer,l_nb_write);
  p_file->cur += l_nb_write;
  p_file->len += l_nb_write;
  return l_nb_write;
}

OPJ_OFF_T opj_skip_from_memory (OPJ_OFF_T p_nb_bytes, myfile * p_file)
{
  if( p_file->cur + p_nb_bytes <= p_file->mem + p_file->len )
    {
    p_file->cur += p_nb_bytes;
    return p_nb_bytes;
    }

  p_file->cur = p_file->mem + p_file->len;
  return -1;
}

OPJ_BOOL opj_seek_from_memory (OPJ_OFF_T p_nb_bytes, myfile * p_file)
{
  if( (size_t)p_nb_bytes <= p_file->len )
    {
    p_file->cur = p_file->mem + p_nb_bytes;
    return OPJ_TRUE;
    }

  p_file->cur = p_file->mem + p_file->len;
  return OPJ_FALSE;
}

opj_stream_t* OPJ_CALLCONV opj_stream_create_memory_stream (myfile* p_mem, OPJ_SIZE_T p_size, int p_is_read_stream)
{
  opj_stream_t* l_stream = NULL;
  if (! p_mem)
  {
    return NULL;
  }
  l_stream = opj_stream_create(p_size,p_is_read_stream);
  if(! l_stream)
  {
    return NULL;
  }
  opj_stream_set_user_data(l_stream,p_mem,NULL);
  opj_stream_set_read_function(l_stream,(opj_stream_read_fn) opj_read_from_memory);
  opj_stream_set_write_function(l_stream, (opj_stream_write_fn) opj_write_from_memory);
  opj_stream_set_skip_function(l_stream, (opj_stream_skip_fn) opj_skip_from_memory);
  opj_stream_set_seek_function(l_stream, (opj_stream_seek_fn) opj_seek_from_memory);
  opj_stream_set_user_data_length(l_stream, p_mem->len /* p_size*/); 
  return l_stream;
}

#endif

unsigned char *
dicom_opj_decompress(unsigned char *jpg_buffer, int jpg_size)
{
  opj_dparameters_t dparameters;
  
#ifdef OPENJPEG1_FOUND  
  opj_cio_t *cio=NULL;
  opj_dinfo_t *dinfo=NULL;
#else  
  opj_stream_t *cio=NULL;
  opj_codec_t *dinfo=NULL;
  #define CODEC_JP2 OPJ_CODEC_JP2
  #define CODEC_J2K OPJ_CODEC_J2K
  OPJ_UINT32 *s[2],fl;
#endif
  opj_image_t *image=NULL;
  unsigned char *dicom_ptr;
  int dicom_len;
  int width, height, pixel_size;
  int i;
  const char jp2magic[] = "\x00\x00\x00\x0C\x6A\x50\x20\x20\x0D\x0A\x87\x0A";
  myfile mysrc;
  myfile *fsrc = &mysrc;
  
#define J2K_CFMT 0
#define JP2_CFMT 1
#define JPT_CFMT 2
#define PGX_DFMT 11


  opj_set_default_decoder_parameters(&dparameters);
    
  if( memcmp( jpg_buffer, jp2magic, sizeof(jp2magic) ) == 0 )
  {
    /* JPEG-2000 compressed image data ... sigh */
    printf( "J2K start like JPEG-2000 compressed image data instead of codestream" );
    dparameters.decod_format = JP2_CFMT;
  }
  else
  {
    /* JPEG-2000 codestream */
    dparameters.decod_format = J2K_CFMT;
  }  
  dparameters.cod_format = PGX_DFMT;
  
  /* get a decoder handle */
  switch(dparameters.decod_format)
  {
  case J2K_CFMT:
    dinfo = opj_create_decompress(CODEC_J2K);
    break;
  case JP2_CFMT:
    dinfo = opj_create_decompress(CODEC_JP2);
    break;
  }  
  
  if (dinfo  == NULL) {
    printf("ERROR: OpenJPEG initialization failed.\n");
    return NULL;
  }
  /*code to make sure the last byte is signature*/
/*  while( jpg_size > 0 && jpg_buffer[jpg_size-1] != 0xd9 )
  {
    jpg_size--;
  }*/

  fsrc->mem = fsrc->cur = (char*)jpg_buffer;
  fsrc->len = jpg_size;
  
  #ifdef OPENJPEG2_FOUND  

  fl = jpg_size - 100;
  s[0] = &fl;
  s[1] = NULL;  
  opj_set_error_handler(dinfo, openjpeg_error_callback, s);
  #endif
  
  #ifdef OPENJPEG1_FOUND
  cio = opj_cio_open((opj_common_ptr) dinfo, jpg_buffer, jpg_size);
  if (cio == NULL) {
    printf("ERROR: OpenJPEG initialization failed.\n");
    return NULL;
  }
  #else
  cio = opj_stream_create_memory_stream(fsrc, OPJ_J2K_STREAM_CHUNK_SIZE, 1);
  if (cio == NULL) {
    printf("ERROR: OpenJPEG initialization failed.\n");
    opj_destroy_codec(dinfo);
    return NULL;
  }
  #endif
  
#ifdef OPENJPEG2_FOUND
  if(!opj_setup_decoder(dinfo, &dparameters) )
  {
    printf("ERROR: OpenJPEG decoder initialization failed.\n");
    opj_destroy_codec(dinfo);
    opj_stream_destroy(cio);
    return NULL;
  }
#else
  opj_setup_decoder(dinfo, &dparameters);
#endif

  
  
  #ifdef OPENJPEG1_FOUND
  if ((image = opj_decode(dinfo, cio)) == NULL) {
    printf("ERROR: OpenJPEG decoder failed.\n");
    opj_destroy_decompress(dinfo);
    return NULL;
  }
  #else
  if(!opj_read_header(cio, dinfo,&image))
  {
    printf("ERROR: OpenJPEG header decoding failed.\n");
    opj_destroy_codec(dinfo);
    opj_stream_destroy(cio);
    return NULL;
  }
  
  if(!opj_decode(dinfo, cio, image))
  {
    printf("ERROR: OpenJPEG decoder failed.\n");
    opj_destroy_codec(dinfo);
    opj_stream_destroy(cio);
    return NULL;
  }
  
  if(!opj_end_decompress(dinfo,cio))
  {
    printf("ERROR: OpenJPEG decoder failed.\n");
    opj_destroy_codec(dinfo);
    opj_stream_destroy(cio);
    return NULL;
  }
  #endif
  
  width  = image->x1 - image->x0;
  height = image->y1 - image->y0;
  pixel_size = (image->comps[0].prec + (8-1)) / 8;
  dicom_len = width * height;
  /* Make a one-row-high sample array that will go away when done with image */
  dicom_ptr = malloc(dicom_len*pixel_size);
  switch (pixel_size) {
  case 1:
    for (i = 0; i < dicom_len; i++) {
      dicom_ptr[i] = image->comps[0].data[i];
    }
    break;
  case 2:
    for (i = 0; i < dicom_len; i++) {
      ((short *)dicom_ptr)[i] = image->comps[0].data[i];
    }
    break;
  case 4:
    for (i = 0; i < dicom_len; i++) {
      ((int *)dicom_ptr)[i] = image->comps[0].data[i];
    }
    break;
  default:
    break;
  }
  #ifdef OPENJPEG2_FOUND
  opj_stream_destroy(cio);
  if (dinfo) opj_destroy_codec(dinfo);
  #else
  opj_cio_close(cio);
  if (dinfo) opj_destroy_decompress(dinfo);
  #endif
  
  opj_image_destroy(image);
  
  return dicom_ptr;
}
#endif


/* ----------------------------- MNI Header -----------------------------------
   @NAME       : get_dicom_image_data
   @INPUT      : group_list - input data
   @OUTPUT     : image - image data structure (user must free data)
   @RETURNS    : (nothing)
   @DESCRIPTION: Routine to get an image from a group list
   @METHOD     :
   @GLOBALS    :
   @CALLS      :
   @CREATED    : November 25, 1993 (Peter Neelin)
   @MODIFIED   :
   ---------------------------------------------------------------------------- */
void
get_dicom_image_data(Acr_Group group_list, Image_Data *image)
{

    /* Variables */
    Acr_Element element;
    int nrows, ncolumns;
    int bits_alloc;
    int image_group;
    void *data = NULL;
    long imagepix, ipix;
    struct Acr_Element_Id elid;
    nc_type datatype;
    void *decoded_data = NULL;
    int encoded_length;
    int dimX, dimY, bits, frames;

    /* Get the image information */
    bits_alloc = (int)acr_find_short(group_list, ACR_Bits_allocated, 0);
    nrows = (int)acr_find_short(group_list, ACR_Rows, 0);
    ncolumns = (int)acr_find_short(group_list, ACR_Columns, 0);
    image_group = (int)acr_find_short(group_list, ACR_Image_location, ACR_IMAGE_GID);

    /* Figure out type */
    if (bits_alloc > CHAR_BIT)
        datatype = NC_SHORT;
    else
        datatype = NC_BYTE;

    /* Set image info */
    imagepix = nrows * ncolumns;
    image->data = (unsigned short *) malloc(imagepix * sizeof(short));
    CHKMEM(image->data);

    /* Get image pointer */
    elid.group_id = image_group;
    elid.element_id = ACR_IMAGE_EID;
    element = acr_find_group_element(group_list, &elid);
    if (element == NULL) {
        memset(image->data, 0, imagepix * sizeof(short));
        return;
    }
    if (acr_element_is_sequence(element)) {
      /* Assume the compressed data is in the second element of the sequence. */
      element = (Acr_Element) acr_get_element_data(element);
      element = acr_get_element_next(element);
      data = acr_get_element_data(element);
      encoded_length = acr_get_element_length(element);
      
#include "jpg_0xc3.h"
      decoded_data = decode_jpeg_sof_0xc3(data, encoded_length, 0,
                                          &dimX, &dimY, &bits, &frames);
#if OPENJPEG_FOUND
      if (decoded_data == NULL) {
        decoded_data = dicom_opj_decompress(data, encoded_length);
      }
#endif
#if JPEG_FOUND
      if (decoded_data == NULL) {
        decoded_data = dicom_jpeg_decompress(data, encoded_length);
      }
#endif
      if (decoded_data == NULL) {
        printf("ERROR: JPEG decoding failed.\n");
        exit(-1);
      }
      else {
        data = decoded_data;
      }
    }
    else {
      data = acr_get_element_data(element);
    }

    /* Convert the data according to type */

    /* Look for byte data */
    if (datatype == NC_BYTE) {
        for (ipix=0; ipix < imagepix; ipix++) {
            image->data[ipix] = *((unsigned char *) data + ipix);
        }
    }
    else {

        /* Look for unpacked short data */
        if (bits_alloc == nctypelen(datatype) * CHAR_BIT) {
            acr_get_short(acr_get_element_byte_order(element),
                          nrows*ncolumns, data, image->data);
        }

        /* Fill with zeros in any other case */
        else {
            memset(image->data, 0, imagepix * sizeof(short));
        }
    }

    if (decoded_data != NULL) {
      free(decoded_data);
    }
    return;
}


/* ----------------------------- MNI Header -----------------------------------
   @NAME       : parse_dicom_groups
   @INPUT      : group_list - list of acr-nema groups that make up object
   @OUTPUT     : data_info - information about data object
   @RETURNS    : (nothing)
   @DESCRIPTION: Routine to parse dicom object
   @METHOD     :
   @GLOBALS    :
   @CALLS      :
   @CREATED    : June 2001 (Rick Hoge)
   @MODIFIED   :
   ---------------------------------------------------------------------------- */

#define IDEFAULT (-1)

void
parse_dicom_groups(Acr_Group group_list, Data_Object_Info *di_ptr)
{
    Acr_Element element;
    Acr_Short AcqMat[4];
    Acr_Short freq_rows;
    Acr_Short freq_cols;

    /* Get info to construct unique identifiers for study, series/acq
     * for file processing
     */
    get_identification_info(group_list,
                            &(di_ptr->study_id), &(di_ptr->acq_id),
                            &(di_ptr->rec_num), &(di_ptr->image_type));

    /* Get number of echos, echo number, number of dynamic scans and
     * dynamic_scan_number
     */

    di_ptr->num_echoes = acr_find_int(group_list,
                                      ACR_Echo_train_length,
                                      IDEFAULT);

    di_ptr->echo_number = acr_find_int(group_list,
                                       ACR_Echo_number,
                                       IDEFAULT);
    /* if we expect multiple echoes from the echo train length,
     * but we have no echo number in the tags,
     * try to use the echo time as the echo number...
     */
    if(di_ptr->echo_number == IDEFAULT && di_ptr->num_echoes > 0){
        di_ptr->echo_number = acr_find_double(group_list,
                                              ACR_Echo_time,
                                              IDEFAULT);
    }


    di_ptr->num_dyn_scans = acr_find_int(group_list,
                                         ACR_Acquisitions_in_series,
                                         IDEFAULT);

    di_ptr->dyn_scan_number = acr_find_int(group_list,
                                           ACR_Acquisition,
                                           IDEFAULT);

    di_ptr->global_image_number = acr_find_int(group_list,
                                               ACR_Image,
                                               IDEFAULT);

    di_ptr->pet_image_index = acr_find_int(group_list,
                                               ACR_PET_Image_index,
                                               IDEFAULT);

    /* rhoge:
       new info added to di_ptr by rhoge: nominal number of slices;
       this is used in detection of a stream of files with the same
       acquisition ID number in which there are more files than
       slices.  If the number of signal averages is greater than one,
       we will assume that this means the acquisition loop was used for
       dynamic scanning.

       WARNINGS:  the same thing may need to be done with `number of
       partitions' for it to work with 3D scans  */

    di_ptr->num_slices_nominal = acr_find_int(group_list,
                                              ACR_Images_in_acquisition,
                                              IDEFAULT);

    di_ptr->slice_number = acr_find_int(group_list,
                                        SPI_Current_slice_number,
                                        IDEFAULT);

    di_ptr->slice_location = (double)acr_find_double(group_list,
                                                     ACR_Slice_location,
                                                     0.0);

    di_ptr->coord_found = dicom_read_position(group_list, 0, di_ptr->coord);

    /* identification info needed to generate unique session id
     * for file names
     */
    di_ptr->study_date = acr_find_int(group_list, ACR_Study_date,
                                      IDEFAULT);

    di_ptr->study_time = acr_find_int(group_list, ACR_Study_time,
                                      IDEFAULT);

    di_ptr->scanner_serialno = acr_find_int(group_list,
                                            ACR_Device_serial_number,
                                            IDEFAULT);

    /* identification info needed to determine if mosaics used
     */

    element = acr_find_group_element(group_list, ACR_Acquisition_matrix);

    if (element != NULL) {
        /* The acquisition matrix contains four elements:
           AcqMat[0] -> number of rows in frequency space.
           AcqMat[1] -> number of columns in frequency space.
           AcqMat[2] -> number of rows in phase space.
           AcqMat[3] -> number of columns in phase space.
        */
        acr_get_element_short_array(element, 4, AcqMat);

        freq_rows = AcqMat[0];
        freq_cols = AcqMat[1];

        /* rows in acq matrix is larger of freq rows and freq columns:
         */
        di_ptr->acq_rows = ( freq_rows > freq_cols ? freq_rows : freq_cols );

        /* all images are square, at this time
         */
        di_ptr->acq_cols = di_ptr->acq_rows;
    }
    else {
        di_ptr->acq_rows = IDEFAULT;
        di_ptr->acq_cols = IDEFAULT;
    }

    di_ptr->rec_rows = acr_find_int(group_list, ACR_Rows, IDEFAULT);
    di_ptr->rec_cols = acr_find_int(group_list, ACR_Columns, IDEFAULT);

    di_ptr->num_mosaic_rows = acr_find_int(group_list, EXT_Mosaic_rows,
                                           IDEFAULT);
    di_ptr->num_mosaic_cols = acr_find_int(group_list, EXT_Mosaic_columns,
                                           IDEFAULT);
    di_ptr->num_slices_in_file = acr_find_int(group_list, EXT_Slices_in_file,
                                              IDEFAULT);

    di_ptr->tpos_id = acr_find_int(group_list, ACR_Temporal_position_identifier,
                                   IDEFAULT);

    /* sequence, protocol names (useful for debugging):
     */

    get_string_field(di_ptr->sequence_name, group_list, ACR_Sequence_name);
    get_string_field(di_ptr->protocol_name, group_list, ACR_Protocol_name);
    get_string_field(di_ptr->patient_name, group_list, ACR_Patient_name);
    fix_patient_name(di_ptr->patient_name);
    get_string_field(di_ptr->patient_id, group_list, ACR_Patient_identification);
    get_string_field(di_ptr->image_type_string, group_list, ACR_Image_type);
}


