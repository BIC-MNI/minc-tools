/* ----------------------------- MNI Header -----------------------------------
@NAME       : dcm2mnc.c
@DESCRIPTION: Program to convert dicom files to minc
@GLOBALS    : 
@CREATED    : June 2001 (Rick Hoge)
@MODIFIED   : 
 * $Log: dcm2mnc.c,v $
 * Revision 1.24  2010-11-23 23:30:50  claude
 * dcm2mnc: fixed seg fault bug (Claude) and added b-matrix (Ilana)
 *
 * Revision 1.23  2008-08-12 05:00:23  rotor
 *  * large number of changes from Claude (64 bit and updates)
 *
 * Revision 1.22  2008/01/17 02:33:01  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 1.21  2008/01/12 19:08:14  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 1.20  2007/05/30 15:17:34  ilana
 * fix so that diffusion images all written into 1 4d volume, gradient directions and bvalues are written to mincheader, some fixes for TIM diffusion images
 *
 * Revision 1.19  2005/11/11 18:42:54  bert
 * Latest fixes to dcm2mnc
 *
 * Revision 1.18  2005/11/04 22:25:04  bert
 * Update version string to 2.0.07
 *
 * Revision 1.17  2005/08/26 21:25:54  bert
 * Latest changes ported from 1.0 branch
 *
 * Revision 1.14.2.10  2005/08/18 18:17:36  bert
 * Add -usecoordinates option
 *
 * Revision 1.14.2.9  2005/07/22 20:03:16  bert
 * Minor change to consider sequence name when finding series boundaries
 *
 * Revision 1.14.2.8  2005/07/12 16:01:14  bert
 * Sort on filename if all else fails
 *
 * Revision 1.14.2.7  2005/06/20 22:03:31  bert
 * Add simple test for binary files to avoid choking on text files
 *
 * Revision 1.14.2.6  2005/06/09 20:48:36  bert
 * Remove obsolete -descr option, add shiny new -fname and -dname options.  Also don't explicitly include math.h
 *
 * Revision 1.14.2.5  2005/06/02 18:35:32  bert
 * Change version to 2.0.06
 *
 * Revision 1.14.2.4  2005/05/16 22:39:56  bert
 * Insert conditionals to make the file build properly under Windows
 *
 * Revision 1.14.2.3  2005/05/16 19:55:50  bert
 * Fix usage of G.command_line
 *
 * Revision 1.14.2.2  2005/05/16 19:45:52  bert
 * Minor fix to argument structure, to reflect correct usage of ARGV_STRING items
 *
 * Revision 1.14.2.1  2005/05/12 21:16:47  bert
 * Initial checkin
 *
 * Revision 1.14  2005/05/09 15:32:02  bert
 * Change version to 2.0.05
 *
 * Revision 1.13  2005/04/29 23:09:36  bert
 * Add support for -stdin option to read file list from standard input
 *
 * Revision 1.12  2005/04/26 23:49:24  bert
 * Update version
 *
 * Revision 1.11  2005/04/18 16:38:42  bert
 * Fix up file type detection code
 *
 * Revision 1.10  2005/04/06 13:26:41  bert
 * Fix listing option
 *
 * Revision 1.9  2005/04/05 21:52:24  bert
 * Add -minmax option to enable use of explicit DICOM pixel min/max information, and updated version number
 *
 * Revision 1.8  2005/03/18 19:10:31  bert
 * Scan coordinate and location information for validity before relying on it
 *
 * Revision 1.7  2005/03/15 17:03:34  bert
 * Yet another directory expansion fix (sigh)
 *
 * Revision 1.6  2005/03/14 22:51:33  bert
 * Actually get the directory expansion working properly...
 *
 * Revision 1.5  2005/03/14 22:25:41  bert
 * If a directory is specified on the file list, expand it internally.  This gets around shell limitations.
 *
 * Revision 1.4  2005/03/03 20:10:14  bert
 * Consider patient_id and patient_name when sorting into series
 *
 * Revision 1.3  2005/03/03 18:59:15  bert
 * Fix handling of image position so that we work with the older field (0020, 0030) as well as the new (0020, 0032)
 *
 * Revision 1.2  2005/03/02 18:23:32  bert
 * Added mosaic sequence and bitwise options
 *
 * Revision 1.1  2005/02/17 16:38:09  bert
 * Initial checkin, revised DICOM to MINC converter
 *
 * Revision 1.1.1.1  2003/08/15 19:52:55  leili
 * Leili's dicom server for sonata
 *
 * Revision 1.5  2002/04/26 12:02:50  rhoge
 * updated usage statement for new forking defaults
 *
 * Revision 1.4  2002/04/26 11:32:48  rhoge
 * made forking default
 *
 * Revision 1.3  2002/03/23 13:17:53  rhoge
 * added support for Bourget network pushed dicom files, cleaned up
 * file check and read_numa4_dicom vr check/assignment
 *
 * Revision 1.2  2002/03/22 19:19:36  rhoge
 * Numerous fixes -
 * - handle Numaris 4 Dicom patient name
 * - option to cleanup input files
 * - command option
 * - list-only option
 * - debug mode
 * - user supplied name, idstr
 * - anonymization
 *
 * Revision 1.1  2002/03/22 03:50:02  rhoge
 * new name for standalone dicom to minc converter
 *
 * Revision 1.3  2002/03/22 00:38:08  rhoge
 * Added progress bar, wait for children at end, updated feedback statements
 *
 * Revision 1.2  2002/03/19 13:13:56  rhoge
 * initial working mosaic support - I think time is scrambled though.
 *
 * Revision 1.1  2001/12/31 17:26:21  rhoge
 * adding file to repository- compiles without warning and converts non-mosaic
 * Numa 4 files. 
 * Will probably not work for Numa 3 files yet.
 *
---------------------------------------------------------------------------- */

#define GLOBAL_ELEMENT_DEFINITION /* To define elements */
#include "dcm2mnc.h"

#include <sys/stat.h>
#if HAVE_DIRENT_H
#include <dirent.h>
#endif
#include <ParseArgv.h>

/* Function Prototypes */
static int dcm_sort_function(const void *entry1, const void *entry2);
static int use_the_files(int num_files, 
                         Data_Object_Info *data_info[],
                         const char *out_dir);
static void usage(void);
static int collect_files(char *directory,
                          char ***file_list,
                          int num_files,
                          struct stat st);
static void free_list(int num_files, 
                      char **file_list, 
                      Data_Object_Info **file_info_list);
static int check_file_type_consistency(int num_files, char **file_list);


struct globals G;

#define VERSION_STRING PACKAGE_VERSION " built " __DATE__ " " __TIME__

#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & _S_IFMT) == _S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(x) (((x) & _S_IFMT) == _S_IFREG)
#endif

ArgvInfo argTable[] = {
    {NULL, ARGV_VERINFO, VERSION_STRING, NULL, NULL },
    {"-clobber", ARGV_CONSTANT, (char *) TRUE, (char *) &G.clobber,
     "Overwrite output files"},
    {"-list", ARGV_CONSTANT, (char *) TRUE, (char *) &G.List,
     "Print list of series (don't create files)"},
    {"-anon", ARGV_CONSTANT, (char *) TRUE, (char *) &G.Anon,
     "Exclude subject name from file header"},
#if HAVE_POPEN
    {"-cmd", ARGV_STRING, (char *) 1, (char *) &G.command_line, 
     "Apply <command> to output files (e.g. gzip)"},
#endif
    {"-verbose", ARGV_CONSTANT, (char *) LO_LOGGING, (char *) &G.Debug,
     "Print debugging information"},
    {"-debug", ARGV_CONSTANT, (char *) HI_LOGGING, (char *) &G.Debug,
     "Print lots of debugging information"},
    {"-nosplitecho", ARGV_CONSTANT, (char *) FALSE, (char *) &G.splitEcho,
     "Combine all echoes into a single file."},
    {"-splitdynamic", ARGV_CONSTANT, (char *) TRUE, (char *)&G.splitDynScan,
     "Split dynamic scans into a separate files."},
    {"-opts", ARGV_INT, (char *) 1, (char *) &G.opts, 
     "Set debugging options"},

    {"-descending", 
     ARGV_CONSTANT,
     (char *) MOSAIC_SEQ_DESCENDING,
     (char *) &G.mosaic_seq,
     "Mosaic sequence is in descending slice order."},

    {"-ascending", 
     ARGV_CONSTANT,
     (char *) MOSAIC_SEQ_ASCENDING, 
     (char *) &G.mosaic_seq,
     "Mosaic sequence is in ascending slice order."},

    {"-interleaved", 
     ARGV_CONSTANT,
     (char *) MOSAIC_SEQ_INTERLEAVED, 
     (char *) &G.mosaic_seq,
     "Mosaic sequence is in interleaved slice order."},

     {"-num_partitions",
     ARGV_INT,
     (char *) NULL,
     (char *) &G.num_partitions,
     "Number of mosaic partitions to use if missing in the dicomtags."},

    {"-minmax", 
     ARGV_CONSTANT, 
     (char *)TRUE, 
     (char *) &G.useMinMax,
     "Honor DICOM pixel minimum and pixel maximum values."},

    {"-stdin",
     ARGV_CONSTANT,
     (char *)TRUE,
     (char *)&G.use_stdin,
     "Read file list from standard input."},

    {"-fname",
     ARGV_STRING,
     (char *)1,
     (char *)&G.filename_format,
     "Set format for output file name."},

    {"-dname",
     ARGV_STRING,
     (char *)1,
     (char *)&G.dirname_format,
     "Set format for output directory name."},

    {"-usecoordinates",
     ARGV_CONSTANT,
     (char *) TRUE,
     (char *) &G.prefer_coords,
     "Derive step value from coordinates rather than slice spacing."},

    {"-ignoreemptytime",
     ARGV_CONSTANT,
     (char *) TRUE,
     (char *) &G.ignore_empty_time,
     "Ignore empty timeframes."},

    {"-abort",
     ARGV_CONSTANT,
     (char *) TRUE,
     (char *) &G.abort_on_error,
     "Stop processing immediately if a file is not parsed properly."},

    {"-adjust_frame_time",
     ARGV_CONSTANT,
     (char *) TRUE,
     (char *) &G.adjust_frame_time,
     "Try to set frame times to the beginning of the frame."},

    {"-accept_period",
     ARGV_CONSTANT,
     (char *) FALSE,
     (char *) &G.ignore_leading_dot,
     "Don't ignore filenames that begin with an initial period ('.')."},

    {"-1", ARGV_CONSTANT, (char *) 1, (char *) &G.file_format,
     "Force MINC 1.0 (NetCDF) format files."},

    {NULL, ARGV_END, NULL, NULL, NULL}

};

int 
main(int argc, char *argv[])
{
    int ifile;
    Acr_Group group_list;
    char **file_list;           /* List of file names */
    Data_Object_Info **file_info_list;
    int num_file_args;          /* Number of files on command line */
    int num_files;              /* Total number of files */
    string_t out_dir;           /* Output directory */
    string_t message;           /* Generic message */
    int num_files_ok;           /* Actual number of DICOM/IMA files */
    struct stat st;
    int length;
    int exit_status;

    G.mosaic_seq = MOSAIC_SEQ_UNKNOWN; /* Assume ascending by default. */
    G.splitDynScan = FALSE;     /* Don't split dynamic scans by default */
    G.splitEcho = TRUE;         /* Do split by echo by default */
    G.use_stdin = FALSE;        /* Do not read file list from stdin */
    G.filename_format = NULL;
    G.dirname_format = NULL;

    G.minc_history = time_stamp(argc, argv); /* Create minc history string */
    G.prefer_coords = FALSE;
    G.abort_on_error = FALSE;
    G.adjust_frame_time = FALSE;
    G.ignore_leading_dot = TRUE;
    G.file_format = 2;          /* default file format */
    G.pname = argv[0];          /* get program name */
    
    /* Get the input parameters and file names.
     */
    if (ParseArgv(&argc, argv, argTable, 0)) {
        usage();
    }

    if (argc < 2) {
        usage();
    }

    if (G.List) {
        num_file_args = argc - 1; /* Assume no directory given. */
    }
    else {
        num_file_args = argc - 2; /* Assume last arg is directory. */

        strcpy(out_dir, argv[argc - 1]); 

        /* make sure path ends with slash 
         */
        length = strlen(out_dir);
        if (out_dir[length - 1] != '/') {
            out_dir[length++] = '/';
            out_dir[length++] = '\0';
        }

        if (stat(out_dir, &st) != 0 || !S_ISDIR(st.st_mode)) {
            fprintf(stderr, "The final argument, '%s', is not a directory\n", 
                    out_dir);
            exit(EXIT_FAILURE);
        }
    }

    /* Get space for file lists */
    /* Allocate the array of pointers used to implement the
     * list of filenames.
     */
    file_list = malloc(1 * sizeof(char *));
    CHKMEM(file_list);

    /* Go through the list of files, expanding directories where they
     * are encountered...
     */
    num_files = 0;
    for (ifile = 0 ; ifile < num_file_args; ifile++) {
#if HAVE_DIRENT_H
        if (stat(argv[ifile + 1], &st) == 0 && S_ISDIR(st.st_mode)) {
            num_files = collect_files(argv[ifile + 1], &file_list, num_files, st);
        }
        else {
            file_list = realloc(file_list, (num_files + 1) * sizeof(char *));
            file_list[num_files++] = strdup(argv[ifile + 1]);
        }
#else
        file_list = realloc(file_list, (num_files + 1) * sizeof(char *));
        file_list[num_files++] = strdup(argv[ifile + 1]);
#endif
    }

    if (G.use_stdin) {
        char linebuf[1024];
        char *p;

        while (fgets(linebuf, sizeof(linebuf), stdin) != NULL) {
            /* Strip off newline at end of string.
             */
            for (p = linebuf; *p != '\0'; p++) {
                if (*p == '\n') {
                    *p = '\0';
                }
            }
            if (strlen(linebuf) != 0) {
                file_list = realloc(file_list,
                                    (num_files + 1) * sizeof(char *));
                file_list[num_files++] = strdup(linebuf);
            }
        }
    }

    file_info_list = malloc(num_files * sizeof(*file_info_list));
    CHKMEM(file_info_list);

    /* figure out what kind of files we have -
     * supported types are:
     *
     *  IMA (Siemens .ima format - Numaris 3.5)
     *  N4DCM (Siemens DICOM - Numaris 4)
     *
     * if not all same type, return an error 
     *
     * we start by assuming N4DCM with no offset - we find that the
     * file is IMA or has an offset (the 128 byte + DICM offset seen
     * on Syngo CD's and exports) then the appropriate flag will be
     * set.
     */
    printf("Checking file types...\n");

    if (check_file_type_consistency(num_files, file_list) < 0) {
        exit(EXIT_FAILURE);
    }

    /* Now loop over all files, getting basic info on each
     */

    num_files_ok = 0;
    int j;
    int bogus_PET_index[num_files];
    double count_reference_times, count_trigger_times = 0.0;
    for (ifile = 0; ifile < num_files; ifile++) {
        char *cur_fname_ptr = file_list[ifile];

        if (!G.Debug) {
            sprintf(message, "Parsing %d files", num_files);
            progress(ifile, num_files, message);
        }

        if (G.file_type == IMA) {
            group_list = siemens_to_dicom(cur_fname_ptr, ACR_IMAGE_GID - 1);
        }
        else {
            /* read up to but not including pixel data
             */
            group_list = read_numa4_dicom(cur_fname_ptr, ACR_IMAGE_GID - 1);
        } 

        if (group_list == NULL) {
            /* This file appears to be invalid - it is probably a dicomdir
             * file or some other stray junk in the directory.
             */
            printf("Skipping file %s, which is not in the expected format.\n",
                   cur_fname_ptr);
            free(cur_fname_ptr);
        }
        else {
            /* Copy it back to the (possibly earlier) position in the real
             * file list.
             */

            /* Set this global variable if a time position ID is missing in a
             * slice to make sure to use another dicomtag to identify the time
             * position of all the frames
             */
            if ((int)acr_find_int(group_list, ACR_Temporal_position_identifier, -1) < 0) {
                G.bogus_temporal_id = 1;
            }

            count_reference_times += (double)acr_find_double(group_list, ACR_Frame_reference_time, 0.0);
            count_trigger_times += (double)acr_find_double(group_list, ACR_Trigger_time, 0.0);

            bogus_PET_index[ifile] = acr_find_int(group_list, ACR_PET_Image_index, -1);
            if (bogus_PET_index[ifile] >= 0) {
                for (j = 0; j < ifile; j++) {
                    if (bogus_PET_index[ifile] == bogus_PET_index[j]) {
                        G.bogus_PET_index = 1;
                        break;
                    }
                }
            }

            file_list[num_files_ok] = cur_fname_ptr;

            /* allocate space for the current entry to file_info_list 
             */
            file_info_list[num_files_ok] = malloc(sizeof(*file_info_list[0]));
            CHKMEM(file_info_list[num_files_ok]);
            file_info_list[num_files_ok]->file_index = num_files_ok;

            parse_dicom_groups(group_list, file_info_list[num_files_ok]);

            /* put the file name into the info list
             */
            file_info_list[num_files_ok]->file_name = strdup(file_list[num_files_ok]);

            /* Delete the group list now that we're done with it
             */
            acr_delete_group_list(group_list);
            num_files_ok++;
        }
    } /* end of loop over files to get basic info */

        if (!count_reference_times) {
            G.bogus_frame_reference_time = 1;
        }

        if (!count_trigger_times) {
            G.bogus_trigger_time = 1;
        }

    if (G.Debug) {
        printf("Using %d files\n", num_files_ok);
    }

    num_files = num_files_ok;

    printf("Sorting %d files...   ", num_files);

    /* sort the files into series based on acquisition number
     */
    qsort(file_info_list, num_files, sizeof(file_info_list[0]),
          dcm_sort_function);

    /* If DEBUG, print a list of all files.
     */
    if (G.Debug) {
        printf("\n");
        for (ifile = 0; ifile < num_files; ifile++) {
            Data_Object_Info *info = file_info_list[ifile];
            char *fname;

            if ((ifile % 16) == 0) {
                printf("%-4s %-32.32s %-15s %8s %8s %4s %4s %4s %4s %4s %4s %4s %4s %4s %5s %16s %16s\n",
                       "num",
                       "filename",
                       "studyid",
                       "serialno",
                       "acq",
                       "nec",
                       "iec",
                       "ndy",
                       "idy",
                       "nsl",
                       "isl",
                       "acol",
                       "rcol",
                       "mrow",
                       "img#",
                       "seq",
                       "pro");
            }
            /* Print out info about file.  Truncate the name if necessary.
             */
            fname = info->file_name;
            if (strlen(fname) > 32) {
                fname += strlen(fname) - 32;
            }

            printf("%4d %-32.32s %14.6f %8d %8d %4d %4f %4d %4d %4d %4d %4d %4d %4d %5d %16s %16s\n",
                   ifile,
                   fname,
                   info->study_id,
                   info->scanner_serialno,
                   info->acq_id,
                   info->num_echoes,
                   info->echo_number,
                   info->num_dyn_scans,
                   info->dyn_scan_number,
                   info->num_slices_nominal,
                   info->slice_number,
                   info->acq_cols,
                   info->rec_cols,
                   info->num_mosaic_rows,
                   info->global_image_number,
                   info->sequence_name,
                   info->protocol_name);
        }
    }

    printf("Done sorting files.\n");

    /* Loop over files, processing by acquisition */ 

    if (G.List) {
        printf("Listing files by series...\n");
    }
    else {
        printf("Processing files, one series at a time...\n");
    }

    exit_status = use_the_files(num_files, file_info_list, out_dir);

    if (G.List) {
        printf("Done listing files.\n");
    }
    else {
        printf("Done processing files.\n");
    }

    free_list(num_files, file_list, file_info_list);

    free(file_list);
    free(file_info_list);

    
    exit(exit_status);
}

static int
collect_files(char *directory,
          char ***file_list,
          int num_files,
          struct stat st)
{
    DIR *dp;
    struct dirent *np;
    char *tmp_str;
    int length;

    if (G.Debug) {
        printf("Expanding directory '%s'\n", directory);
    }

    length = strlen(directory);

    dp = opendir(directory);
    if (dp != NULL) {
        while ((np = readdir(dp)) != NULL) {
            /* Ignore files with names beginning with '.' */
            if (np->d_name[0] == '.' && G.ignore_leading_dot)
                continue;
            /* Generate the full path to the file.
             */
            tmp_str = malloc(length + strlen(np->d_name) + 2);
            strcpy(tmp_str, directory);
            if (tmp_str[length-1] != '/') {
                tmp_str[length] = '/';
                tmp_str[length+1] = '\0';
            }
            strcat(&tmp_str[length], np->d_name);
            if (stat(tmp_str, &st) == 0 && S_ISDIR(st.st_mode)) {
                num_files = collect_files(tmp_str, &file_list[0], num_files, st);
                free(tmp_str);
            }
            else if (stat(tmp_str, &st) == 0 && S_ISREG(st.st_mode)) {
                *file_list = realloc(*file_list,
                                    (num_files + 1) * sizeof(char *));
                file_list[0][num_files++] = tmp_str;
            }
            else {
                free(tmp_str);
            }
        }
        closedir(dp);
    }
    else {
        fprintf(stderr, "Error opening directory '%s'\n", directory);
    }

    return num_files;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : free_list
@INPUT      : num_files - number of files in list
              file_list - array of file names
@OUTPUT     : (none)
@RETURNS    : (nothing)
@DESCRIPTION: Frees up things pointed to in pointer arrays. Does not free
              the arrays themselves.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : November 22, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static void 
free_list(int num_files, 
          char **file_list, 
          Data_Object_Info **file_info_list)
{
    int i;

    for (i = 0; i < num_files; i++) {
        if (file_list[i] != NULL) {
            free(file_list[i]);
        }
        if (file_info_list[i] != NULL) {
            free(file_info_list[i]->file_name);
            free(file_info_list[i]);
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : dcm_sort_function
@INPUT      : entry1
              entry2
@OUTPUT     : (none)
@RETURNS    : -1, 0, 1 for lt, eq, gt
@DESCRIPTION: Function to compare two dcm series numbers
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : June 2001 (Rick Hoge)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int 
dcm_sort_function(const void *entry1, const void *entry2)
{
    const Data_Object_Info **file_info_list1 = 
      (const Data_Object_Info **) (uintptr_t) entry1;
    const Data_Object_Info **file_info_list2 = 
      (const Data_Object_Info **) (uintptr_t) entry2;

    // make a sort-able session ID number:  date.time
    double session1 = (*file_info_list1)->study_date +
        (*file_info_list1)->study_time / 1e6;
    double session2 = (*file_info_list2)->study_date +
        (*file_info_list2)->study_time / 1e6;

    // series index
    int series1 = (*file_info_list1)->acq_id;
    int series2 = (*file_info_list2)->acq_id;

    // frame index
    int frame1 = (*file_info_list1)->dyn_scan_number;
    int frame2 = (*file_info_list2)->dyn_scan_number;

    // echo index
    double echo1 = (*file_info_list1)->echo_number;
    double echo2 = (*file_info_list2)->echo_number;

    // image index
    int image1 = (*file_info_list1)->global_image_number;
    int image2 = (*file_info_list2)->global_image_number;

    if (G.bogus_PET_index) {
        image1 = (*file_info_list1)->pet_image_index;
        image2 = (*file_info_list2)->pet_image_index;
    }

    int slice1 = (*file_info_list1)->slice_number;
    int slice2 = (*file_info_list2)->slice_number;

    int ncmp;

    if (session1 < session2) return -1;
    else if (session1 > session2) return 1;
    else if (series1 < series2) return -1;
    else if (series1 > series2) return 1;
    else if ((ncmp = strcmp((*file_info_list1)->protocol_name,
                            (*file_info_list2)->protocol_name)) != 0) {
      return ncmp;
    }
    else if (frame1 < frame2) return -1;
    else if (frame1 > frame2) return 1;
    else if (echo1  < echo2)  return -1;
    else if (echo1  > echo2)  return 1;
    else if (image1 < image2) return -1;
    else if (image1 > image2) return 1;
    else if (slice1 < slice2) return -1;
    else if (slice1 > slice2) return 1;
    /* Last chance - if all else is equal, sort by the file names. 
     */
    else return strcmp((*file_info_list1)->file_name,
                       (*file_info_list2)->file_name);
}

static void 
usage(void)
{
    fprintf(stderr, "\nUsage: %s [options] file1 file2 file3 ... destdir\n",
            G.pname);
    fprintf(stderr, "\n");
    fprintf(stderr,"Files are named according to the following convention:\n\n");
    fprintf(stderr,"  Directory: lastname_firstname_yyyymmdd_hhmmss/\n");
    fprintf(stderr,"  Files:     lastname_firstname_yyyymmdd_hhmmss_series_modality.mnc\n\n");

    exit(EXIT_FAILURE);
}

static int
use_the_files(int num_files, 
              Data_Object_Info *di_ptr[],
              const char *out_dir)
{
    int ifile;
    int acq_num_files;
    const char **acq_file_list;
    int *used_file;
    int *acq_file_index;
    double cur_study_id;
    int cur_acq_id;
    int cur_rec_num;
    int cur_image_type;
    double cur_echo_number;
    int cur_dyn_scan_number;
    string_t cur_patient_name;
    string_t cur_patient_id;
    string_t cur_sequence_name;
    string_t cur_protocol_name;
    string_t cur_image_type_string;
    int exit_status;
    const char *output_file_name;
    string_t file_prefix;
    string_t string;
    FILE *fp;
    int trust_location;
    int trust_coord;
    int user_opts;              /* Options as set by user. We may override.. */
    int number_of_series_converted = 0, failed = 0;

    if (out_dir != NULL) {    /* if an output directory name has been 
                               * provided on the command line
                               */
        if (G.Debug) {
            printf("Using directory '%s'\n", out_dir);
        }
        strcpy(file_prefix, out_dir);
    }
    else {
        file_prefix[0] = '\0';
    }

    if (G.Debug) {                /* debugging */
        printf("file_prefix:  [%s]\n", file_prefix);
    }

    /* Allocate space for acquisition file list.
     */
    acq_file_list = malloc(num_files * sizeof(*acq_file_list));
    CHKMEM(acq_file_list);

    acq_file_index = malloc(num_files * sizeof(*acq_file_index));
    CHKMEM(acq_file_index);

    used_file = malloc(num_files * sizeof(*used_file));
    CHKMEM(used_file);

    for (ifile = 0; ifile < num_files; ifile++) {
        used_file[ifile] = FALSE;
    }

    for (;;) {

        /* Loop through files, looking for an acquisition
         * 
         * file groups should already have been sorted into acquisitions
         * in calling program 
         *
         * this code is in a `forever' loop because we loop over multiple
         * acquisitions until all of the files are used up.
         */

        acq_num_files = 0;
        G.n_files_total = num_files;

        for (ifile = 0; ifile < num_files; ifile++) {

            /* If already marked used (can this happen???), we've already
             * written the file to an output somewhere.
             */
            if (used_file[ifile]) {
                continue;
            }

            if (acq_num_files == 0) {
	 
                /* found first file: set all current attributes like
                 * study id, acq id, rec num(?), image type, echo
                 * number, dyn scan number, flag for multiple echoes,
                 * flag for multiple time points the flag input file
                 * as `used' 
                 */ 
	 
                cur_study_id = di_ptr[ifile]->study_id;
                cur_acq_id = di_ptr[ifile]->acq_id;
                cur_rec_num = di_ptr[ifile]->rec_num;
                cur_image_type = di_ptr[ifile]->image_type;
                cur_echo_number = di_ptr[ifile]->echo_number;
                cur_dyn_scan_number = di_ptr[ifile]->dyn_scan_number;

                strcpy(cur_patient_name, di_ptr[ifile]->patient_name);
                strcpy(cur_patient_id, di_ptr[ifile]->patient_id);
                strcpy(cur_sequence_name, di_ptr[ifile]->sequence_name);
                strcpy(cur_protocol_name, di_ptr[ifile]->protocol_name);
                strcpy(cur_image_type_string, di_ptr[ifile]->image_type_string);

                used_file[ifile] = TRUE;
            }
            /* otherwise check if attributes of the new input file match those
             * of the current output context and flag input file as `used' 
             */
            else if ((di_ptr[ifile]->study_id == cur_study_id) &&
                     (di_ptr[ifile]->acq_id == cur_acq_id) &&
                     (di_ptr[ifile]->rec_num == cur_rec_num) &&
                     (di_ptr[ifile]->image_type == cur_image_type) &&
                     (di_ptr[ifile]->echo_number == cur_echo_number ||
                      !G.splitEcho) &&
                     (di_ptr[ifile]->dyn_scan_number == cur_dyn_scan_number ||
                      !G.splitDynScan) &&
                     !strcmp(cur_protocol_name, di_ptr[ifile]->protocol_name) &&
                     !strcmp(cur_image_type_string, di_ptr[ifile]->image_type_string) &&
                     !strcmp(cur_patient_name, di_ptr[ifile]->patient_name) &&
                     !strcmp(cur_patient_id, di_ptr[ifile]->patient_id)) {

                used_file[ifile] = TRUE;
            }
            if (used_file[ifile]) {
	 
                /* if input file is flagged as `used', then add its index
                   to the list of files for this acquisition (and increment
                   counter) */

                acq_file_list[acq_num_files] = di_ptr[ifile]->file_name;
                acq_file_index[acq_num_files] = ifile;
                acq_num_files++;
            }
        }

        /* If no files were added to this acquisition, it implies that
         * all files have been processed.
         */
        if (acq_num_files == 0) {
            break;              /* All done!!! */
        }
       
        /* Use the files for this acquisition
         */
     
        /* Print out the file names if we are debugging.
         */
        if (G.Debug || G.List) {
            printf("\nSeries %4d %20s %20s (%4d files):\n",
                   cur_acq_id,
                   cur_patient_name,
                   di_ptr[acq_file_index[0]]->protocol_name,
                   acq_num_files);
            for (ifile = 0; ifile < acq_num_files; ifile++) {
                printf("     %s\n", di_ptr[acq_file_index[ifile]]->file_name);
            }
            if (G.List) {
                continue;
            }
        }

        /* Do some sanity checks on the acquisition.  In particular, we 
         * verify that the coordinate and/or slice location information
         * looks reliable.
         */
        trust_location = 1;
        trust_coord = 1;

        for (ifile = 0; ifile < acq_num_files; ifile++) {
            int jfile;
            int ix = acq_file_index[ifile];

            if (!di_ptr[ix]->coord_found) {
                trust_coord = 0;
            }

            for (jfile = ifile + 1; jfile < acq_num_files; jfile++) {
                int jx = acq_file_index[jfile];

                if (NEARLY_EQUAL(di_ptr[ix]->slice_location,
                                 di_ptr[jx]->slice_location)) {
                    trust_location = 0;
                }
            }
        }

        /* See how many coordinate points there are. */
        G.n_distinct_coordinates = 0;
        G.n_slices_nominal = 0;
        if (trust_coord) {
#define N_HASH 1013
          struct coord_set {
            struct coord_set *link;
            double coord[WORLD_NDIMS];
          } *coord_set[N_HASH];
          int n_elements = 0;
          struct coord_set *cs_ptr;
          int n_slices_nominal = di_ptr[acq_file_index[0]]->num_slices_nominal;

          for (ifile = 0; ifile < N_HASH; ifile++) {
            coord_set[ifile] = NULL;
          }

          for (ifile = 0; ifile < acq_num_files; ifile++) {
            int ix = acq_file_index[ifile];
            int hash = ((unsigned) floor(di_ptr[ix]->coord[XCOORD] * 100) +
                        (unsigned) floor(di_ptr[ix]->coord[YCOORD] * 100) +
                        (unsigned) floor(di_ptr[ix]->coord[ZCOORD] * 100)) % N_HASH;
            int found = 0;

            if (coord_set[hash] != NULL) {
              for (cs_ptr = coord_set[hash]; cs_ptr != NULL; cs_ptr = cs_ptr->link) {
                if (cs_ptr->coord[XCOORD] == di_ptr[ix]->coord[XCOORD] &&
                    cs_ptr->coord[YCOORD] == di_ptr[ix]->coord[YCOORD] &&
                    cs_ptr->coord[ZCOORD] == di_ptr[ix]->coord[ZCOORD]) {
                  found = 1;
                  break;
                }
              }
            }
            if (!found) {
              cs_ptr = malloc(sizeof(struct coord_set));
              cs_ptr->coord[XCOORD] = di_ptr[ix]->coord[XCOORD];
              cs_ptr->coord[YCOORD] = di_ptr[ix]->coord[YCOORD];
              cs_ptr->coord[ZCOORD] = di_ptr[ix]->coord[ZCOORD];
              cs_ptr->link = coord_set[hash];
              coord_set[hash] = cs_ptr;
              n_elements++;
            }
          }

          for (ifile = 0; ifile < N_HASH; ifile++) {
            while ( (cs_ptr = coord_set[ifile]) != NULL ) {
              coord_set[ifile] = cs_ptr->link;
              free(cs_ptr);
            }
          }

          printf("INFO: Number of distinct coordinates: %d\n", n_elements );
          if (n_elements > 1) {
            G.n_distinct_coordinates = n_elements;
            if ( n_slices_nominal % G.n_distinct_coordinates == 0 &&
                 n_slices_nominal > G.n_distinct_coordinates) {
                G.n_slices_nominal = 1;
            }
          }
        }

        /* We also check whether the acquisition number (0x0020, 0x0012) 
         * and image number (0x0020, 0x0013) are informative or not. 
         * They are sometimes absent, constant, or otherwise strange.
         * Determining this ahead of time helps us make good decisions
         * about how to treat these fields later.
         * We also check the temporal position identifier, as it can also
         * be useless.
         */
        G.max_acq_num = INT_MIN;
        G.min_acq_num = INT_MAX;
        G.max_img_num = INT_MIN;
        G.min_img_num = INT_MAX;
        G.max_tpos_id = INT_MIN;
        G.min_tpos_id = INT_MAX;

        for (ifile = 0; ifile < acq_num_files; ifile++) {
          int ix = acq_file_index[ifile];

          if (di_ptr[ix]->dyn_scan_number < G.min_acq_num) {
            G.min_acq_num = di_ptr[ix]->dyn_scan_number;
          }
          if (di_ptr[ix]->dyn_scan_number > G.max_acq_num) {
            G.max_acq_num = di_ptr[ix]->dyn_scan_number;
          }

          if (di_ptr[ix]->global_image_number < G.min_img_num) {
            G.min_img_num = di_ptr[ix]->global_image_number;
          }
          if (di_ptr[ix]->global_image_number > G.max_img_num) {
            G.max_img_num = di_ptr[ix]->global_image_number;
          }

          if (di_ptr[ix]->tpos_id < G.min_tpos_id) {
            G.min_tpos_id = di_ptr[ix]->tpos_id;
          }
          if (di_ptr[ix]->tpos_id > G.max_tpos_id) {
            G.max_tpos_id = di_ptr[ix]->tpos_id;
          }
        }

        user_opts = G.opts;

        if (!trust_coord) {
            printf("WARNING: Image coordinates absent or incomplete.\n");
            if (!trust_location) {
                printf("WARNING: Slice location is untrustworthy.\n");
                G.opts |= OPTS_NO_LOCATION;
            }
        }

        if (G.Debug >= HI_LOGGING) {
            if (G.min_acq_num != G.max_acq_num) {
                int ix = acq_file_index[0];
                if (G.max_acq_num == di_ptr[ix]->num_dyn_scans) {
                    /* Acquisition number is per scan (e.g. time).
                    */
                    printf("WARNING: Acquisition number is per scan.\n");
                }
                else if (G.max_acq_num == di_ptr[ix]->num_slices_nominal * di_ptr[ix]->num_dyn_scans) {
                    printf("WARNING: Acquisition number is global.\n");
                }
            }
            if (G.min_img_num != G.max_img_num) {
                int ix = acq_file_index[0];
                if (G.max_img_num == di_ptr[ix]->num_slices_nominal) {
                    printf("WARNING: Image number is per slice.\n");
                }
                else if (G.max_img_num == di_ptr[ix]->num_slices_nominal * di_ptr[ix]->num_dyn_scans) {
                    printf("WARNING: Image number is global.\n");
                }
            }
            if (G.min_img_num < 0 || G.min_img_num > 1) {
                printf("WARNING: Minimum image number is %d\n", G.min_img_num);
            }
        }
        
        printf("INFO: Acquisition number ranges from %d to %d\n", G.min_acq_num, G.max_acq_num);
        printf("INFO: Image number ranges from %d to %d\n", G.min_img_num, G.max_img_num);
        printf("INFO: Temporal position id ranges from %d to %d.\n", G.min_tpos_id, G.max_tpos_id);

        /* Create minc file
         */
        G.n_files = acq_num_files;
        exit_status = dicom_to_minc(acq_num_files, 
                                    acq_file_list, 
                                    NULL,
                                    G.clobber, 
                                    file_prefix, 
                                    &output_file_name);
        number_of_series_converted++;
        failed += exit_status;
        G.opts = user_opts;
       
        if (exit_status != EXIT_SUCCESS) 
            continue;

        /* Print log message */
        if (G.Debug) {
            printf("Created minc file %s.\n", output_file_name);
        }

#if HAVE_POPEN       
        /* Invoke a command on the file (if requested) and get the 
         * returned file name 
         */
        if (G.command_line != NULL && *G.command_line != '\0') {
            sprintf(string, "%s %s", G.command_line, output_file_name);
            printf("-Applying command '%s' to output file...  ", 
                   G.command_line);
            fflush(stdout);
            if ((fp = popen(string, "r")) != NULL) {
                char pipe_output_name[256];
                fscanf(fp, "%s", pipe_output_name);
                if (pclose(fp) != EXIT_SUCCESS) {
                    fprintf(stderr, 
                            "Error executing command\n   \"%s\"\n",
                            string);
                }
                else if (G.Debug) {
                    printf("Executed command \"%s\",\nproducing file %s.\n",
                           string, pipe_output_name);
                }
            }
            else {
                fprintf(stderr, "Error executing command \"%s\"\n", string);
            }
            printf("Done.\n");
        }
#endif /* HAVE_POPEN */
    }
   
    /* Free acquisition file list */
    free(acq_file_list);
    free(acq_file_index);
    free(used_file);
    /* Non-zero exit only if ALL DICOM series failed conversion */
    if (number_of_series_converted > failed) {
        exit_status = EXIT_SUCCESS;
        if (G.Debug && failed > 0) {
            printf("WARNING: %d out of %d DICOM series failed conversion.\n", failed, number_of_series_converted);
        }
    }
    return exit_status;
}

static int
is_cdexport_file(const char *fullname)
{
    FILE *fp;
    char tst_str[DICM_MAGIC_SIZE+1];
    int result = 0;

    if ((fp = fopen(fullname, "rb")) == NULL) {
        fprintf(stderr, "Error opening file %s!\n", fullname);
    }
    else {
        fseek(fp, DICM_MAGIC_OFFS, SEEK_SET);
        fread(tst_str, 1, DICM_MAGIC_SIZE, fp);
        tst_str[DICM_MAGIC_SIZE] = '\0';

        if (!strcmp(tst_str, DICM_MAGIC_STR)) {
            result = 1;
        }
        fclose(fp);
    }
    return (result);
}

static int
is_ima_file(const char *fullname)
{
    FILE *fp;
    char mfg_str[IMA_MAGIC_SIZE+1];
    int result = 0;

    if ((fp = fopen(fullname, "rb")) == NULL) {
        fprintf(stderr, "Error opening file %s!\n", fullname);
    }
    else {
        fseek(fp, IMA_MAGIC_OFFS, SEEK_SET);
        fread(mfg_str, 1, IMA_MAGIC_SIZE, fp);

        /* We only deal with Siemens IMA files - not sure any other kinds 
         * exist, frankly.
         */
        if (!memcmp(mfg_str, IMA_MAGIC_STR, IMA_MAGIC_SIZE)) {
            result = 1;
        }
        fclose(fp);
    }
    return (result);
}

/* _very_ limited test for "binary-ness" of a file.  This is just to keep
 * text files and other junk present in a directory from screwing up our
 * file type detection.
 */
static int
is_binary_file(const char *fullname)
{
    FILE *fp;
    int result = 0;
    int i;

    if ((fp = fopen(fullname, "rb")) == NULL) {
        fprintf(stderr, "Error opening file %s!\n", fullname);
    }
    else {
        /* This is an extremely trivial test for binary-ness.  Basically, we
         * look at the first 512 bytes, and if there aren't lots of unprintable
         * characters, we assume it is not a binary file.
         */
        for (i = 0; i < 128; i++) {
            int cc = getc(fp);
            if (cc == -1) {
                result = 0;     /* too short!! */
                break;
            }
            if (!isprint(cc) && cc != '\n' && cc != '\r') {
                result++;
            }
        }
        fclose(fp);
    }
    return (result > 3);        /* Binary if more than 3 unprintables */
}


static int
check_file_type_consistency(int num_files, char *file_list[])
{
    int i;
    const char *fn_ptr;
    int n4_offset = 0;

    for (i = 0; i < num_files; i++) {

        fn_ptr = file_list[i];

        /* Numaris 4 DICOM CD/Export file? if so, bytes 128-131 will 
         * contain the string `DICM' with no null termination.
         */

        if (is_cdexport_file(fn_ptr)) {
            if (G.file_type == UNDEF) {
                G.file_type = N4DCM;
                n4_offset = 1; 
                printf("File %s appears to be DICOM (CD/Export).\n",
                       fn_ptr);
            }
            else if (G.file_type != N4DCM || n4_offset != 1) {
                printf("MISMATCH: File %s appears to be DICOM (CD/Export).\n",
                       fn_ptr);
                return (-1);
            }
        } 
        else if (is_ima_file(fn_ptr)) {
            if (G.file_type == UNDEF) {
                G.file_type = IMA;
                printf("File %s appears to be Siemens IMA.\n", fn_ptr);
            }
            else if (G.file_type != IMA) {
                printf("MISMATCH: File %s appears to be Siemens IMA.\n", 
                       fn_ptr);
                return (-1);
            }
        }
        else if (is_binary_file(fn_ptr)) {
            if (G.file_type == UNDEF) {
                G.file_type = N4DCM;
                n4_offset = 0; 
                printf("File %s appears to be standard DICOM.\n", fn_ptr);
            }
            else if (G.file_type != N4DCM || n4_offset != 0) {
                printf("MISMATCH: File %s appears to be standard DICOM.\n",
                       fn_ptr);
                return (-1);
            }
        }
    }
    return (0);
}

/* compare two floating-point numbers */
int fcmp(double x, double y, double delta) 
{
    return ((fabs(x - y) / ((x == 0.0) ? 1.0 : fabs(x))) < delta);
}

