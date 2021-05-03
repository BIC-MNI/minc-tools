/* ----------------------------- MNI Header -----------------------------------
@NAME       : dicom_to_minc.h
@DESCRIPTION: Header file for dicom_to_minc.h
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 28, 1997 (Peter Neelin)
@MODIFIED   : 
 * $Log: dicom_to_minc.h,v $
 * Revision 1.10  2010-11-23 23:30:50  claude
 * dcm2mnc: fixed seg fault bug (Claude) and added b-matrix (Ilana)
 *
 * Revision 1.9  2007-06-08 20:28:57  ilana
 * added several fields to mincheader (dicom elements and found in ASCONV header)
 *
 * Revision 1.8  2006/04/09 15:33:10  bert
 * Add flags and fields for DTI
 *
 * Revision 1.7  2005/11/04 22:25:34  bert
 * Change COORDINATE_EPSILON again, now set to 0.005
 *
 * Revision 1.6  2005/10/26 23:43:55  bert
 * Fix COORDINATE_EPSILON at 0.0001
 *
 * Revision 1.5  2005/07/14 19:00:30  bert
 * Changes ported from 1.X branch
 *
 * Revision 1.4.2.2  2005/06/20 22:00:26  bert
 * Simplify Image_Data structure, add is_siemens_mosaic() function declaration
 *
 * Revision 1.4.2.1  2005/05/12 21:16:47  bert
 * Initial checkin
 *
 * Revision 1.4  2005/04/28 17:10:22  bert
 * Added width information to General_Info and File_Info structures
 *
 * Revision 1.3  2005/04/20 23:14:32  bert
 * Remove unnecessary fields, add copy_spi_to_acr() function definition
 *
 * Revision 1.2  2005/03/02 18:25:13  bert
 * Add Mri_Names and Volume_Names
 *
 * Revision 1.1  2005/02/17 16:38:10  bert
 * Initial checkin, revised DICOM to MINC converter
 *
 * Revision 1.1.1.1  2003/08/15 19:52:55  leili
 * Leili's dicom server for sonata
 *
 * Revision 1.6  2002/04/26 03:27:03  rhoge
 * fixed MrProt problem - replaced fixed lenght char array with malloc
 *
 * Revision 1.5  2002/04/08 17:26:34  rhoge
 * added additional sequence info to minc header
 *
 * Revision 1.4  2002/03/27 18:57:50  rhoge
 * added diffusion b value
 *
 * Revision 1.3  2002/03/19 13:13:57  rhoge
 * initial working mosaic support - I think time is scrambled though.
 *
 * Revision 1.2  2001/12/31 18:27:21  rhoge
 * modifications for dicomreader processing of Numaris 4 dicom files - at
 * this point code compiles without warning, but does not deal with
 * mosaiced files.  Also will probably not work at this time for Numaris
 * 3 .ima files.  dicomserver may also not be functional...
 *
 * Revision 1.1.1.1  2000/11/30 02:13:15  rhoge
 * imported sources to CVS repository on amoeba
 *
 * Revision 6.1  1999/10/29 17:51:59  neelin
 * Fixed Log keyword
 *
 * Revision 6.0  1997/09/12 13:24:27  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:25:26  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:06:20  neelin
 * Release of minc version 0.4
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

#include <minc.h>

/* General constants */
#define SECONDS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define SECONDS_PER_HOUR (MINUTES_PER_HOUR*SECONDS_PER_MINUTE)
#define HOURS_PER_DAY 24
#define SECONDS_PER_DAY (HOURS_PER_DAY*SECONDS_PER_HOUR)
#define MS_PER_SECOND 1000
#define COORDINATE_EPSILON (0.005) /* bert- relaxed from 1.0e-5 */
#define DICOM_SECONDS_PER_HOUR 10000.0
#define DICOM_SECONDS_PER_MINUTE 100.0

/* Default value for ncopts */
#define NCOPTS_DEFAULT NC_VERBOSE

/* MINC variable for dicom elements */
#define DICOM_ROOT_VAR "dicom_groups"

/* Possible MRI dimensions */
typedef enum { SLICE = 0, ECHO, TIME, PHASE, CHEM_SHIFT, MRI_NDIMS } Mri_Index;

extern const char *Mri_Names[MRI_NDIMS];

/* World dimensions */
typedef enum { XCOORD = 0, YCOORD, ZCOORD, WORLD_NDIMS } World_Index;

extern const char *World_Names[WORLD_NDIMS];

/* Volume dimensions */
typedef enum { VSLICE = 0, VROW, VCOLUMN, VOL_NDIMS } Volume_Index;

extern const char *Volume_Names[VOL_NDIMS];

/* Orientations */
typedef enum {TRANSVERSE = 0, SAGITTAL, CORONAL, NUM_ORIENTATIONS} Orientation;

/* For dicom_to_minc(), these codes specify whether we are dealing
 * with a "normal" DICOM sequence, a Siemens mosaic sequence, or a
 * DICOM multiframe sequence.
 */
#define SUBIMAGE_TYPE_NONE 0
#define SUBIMAGE_TYPE_MOSAIC 1
#define SUBIMAGE_TYPE_MULTIFRAME 2

/* Structure for general info about files */
typedef struct {
    int initialized;
    int num_files;
    int subimage_type;
    double study_id;
    int acq_id;                 /* Time of scan */
    int rec_num;
    string_t image_type_string;
    int nrows;
    int ncolumns;
    int default_index[MRI_NDIMS]; /* Index for dimensions with size == 1 */
    int cur_size[MRI_NDIMS]; /* Size of dimension across these files */
    int max_size[MRI_NDIMS]; /* Size of dimension across acquisition */
    int size_isset[MRI_NDIMS]; /* If non-zero, found a max_size value. */
    int *indices[MRI_NDIMS]; /* List of indices found for each dimension.
                                Only allocated when size > 1 */ /*  */
    int search_start[MRI_NDIMS]; /* Indices into lists for starting searches */
    double *coordinates[MRI_NDIMS]; /* Array indicating coordinate of each
                                       index in indices array */
    double *widths[MRI_NDIMS];  /* Array indicating width of each index in
                                   indices array */
    int image_index[MRI_NDIMS];  /* Mapping from MRI dim to output image dim */
    World_Index slice_world;
    World_Index row_world;
    World_Index column_world;
    double step[WORLD_NDIMS];
    double start[WORLD_NDIMS];
    double dircos[WORLD_NDIMS][WORLD_NDIMS];
    nc_type datatype;           /* netCDF (and therefore MINC) datatype */
    int is_signed;              /* TRUE of 2's compliment data */
    double pixel_min;
    double pixel_max;
    string_t units;
    double window_min;
    double window_max;
    int num_mosaic_rows;
    int num_mosaic_cols;
    int num_slices_in_file;
    int sub_image_rows;
    int sub_image_columns;
    int fix_multi_echo_fMRI;
    int found_philips_MRLabels;
    int missing_scale;
    struct {
        string_t name;
        string_t identification;
        string_t birth_date;
        string_t age;
        string_t sex;
        string_t reg_date;
        string_t reg_time;
        double weight;
	string_t position; /*added ilana*/
    } patient;
    struct {
        string_t start_time;
        string_t modality;
        string_t manufacturer;
        string_t model;
        double  field_value;
        string_t software_version;
        string_t serial_no;
        string_t calibration_date;
	string_t calibration_time;
        string_t institution;
        string_t station_id;
        string_t referring_physician;
        string_t performing_physician;
        string_t operator;
        string_t procedure;
        string_t study_id;
        string_t acquisition_id;
    } study;
    struct {
        string_t scan_seq;
	string_t acquisition_time; /*added by ilana*/
	string_t image_time; /*added by ilana*/
	string_t series_time; /*added by ilana*/
        string_t protocol_name;
	string_t series_description; /*added by ilana*/
        string_t receive_coil;
        string_t transmit_coil;
        double rep_time;
        double slice_thickness;
        double num_slices;
	string_t slice_order;
        double echo_time;
	double echo_train_length; /*added by ilana*/
        double echo_number;
        double inv_time;
	double delay_in_TR; /*added by ilana*/
        double b_value;
        double flip_angle;
        double num_avg;
        double num_dyn_scans;
        double imaging_freq;
        string_t imaged_nucl;
        double  win_center;
        double  win_width;
        double  num_phase_enc_steps;
        double  percent_sampling;
        double  percent_phase_fov;
        double  pixel_bandwidth;
        string_t phase_enc_dir;
        string_t mr_acq_type;
        string_t image_type;
        double  sar;
        string_t comments;
        short dti;              /* TRUE if we think this is DTI */
    } acq;
    struct Pet_info {
        string_t radionuclide;
        string_t tracer;
        double radionuclide_halflife;
        double positron_fraction;
        string_t dose_units;
        string_t injection_route;
        double injection_dose;
        double injection_volume;
        string_t injection_time;
        int injection_year;
        int injection_month;
        int injection_day;
        int injection_hour;
        int injection_minute;
        double injection_seconds;
    } pet;
    Acr_Group group_list;
} General_Info;

#define B_MATRIX_COUNT 6

/* Structure for file-specific info */
typedef struct {
   int valid;
   const char *name;
   Acr_Group group_list;
   int bits_alloc;
   int bits_stored;
   int index[MRI_NDIMS];
   double pixel_max;
   double pixel_min;
   double slice_max;
   double slice_min;
   double window_max;
   double window_min;
   double coordinate[MRI_NDIMS];
   double width[MRI_NDIMS];     /* Sample width along each MRI dimension */
   double b_value;              /* DTI b-value, if present */
   double grad_direction[WORLD_NDIMS]; /* DTI gradient direction */
   double b_matrix[B_MATRIX_COUNT]; 
   int tpos_id; /* Temporal position identifier (for GE slice order) */
   double trigger_time; /* Trigger time (for GE slice order) */
   double RTIA_timer; /* RTIA timer (for GE slice order) */
   double slice_location; /* Slice location (for GE slice order) */
   double rep_time; /* Repetition time (for time positions) */
} File_Info;

/* Structure for storing the actual image data */
typedef struct {
    unsigned short *data;
} Image_Data;

/* function definitions */
extern int dicom_to_minc(int num_files, 
                         const char **file_list, 
                         const char *minc_file, 
                         int clobber,
                         const char *file_prefix, 
                         const char **output_file_name);
extern Acr_Group read_std_dicom(const char *filename, int max_group);
extern Acr_Group read_numa4_dicom(const char *filename, int max_group);
extern int search_list(int value, const int *list_ptr, int list_length, 
                       int start_index);
extern Acr_Group copy_spi_to_acr(Acr_Group group_list);

extern int is_siemens_mosaic(Acr_Group group_list);
