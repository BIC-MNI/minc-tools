/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincinfo
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to dump minc header information to standard output.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 19, 1993 (Peter Neelin)
@MODIFIED   : 
 * $Log: mincinfo.c,v $
 * Revision 6.13  2009-04-29 13:58:46  rotor
 *  * fixed a stack smash in mincinfo.c
 *
 * Revision 6.12  2008/01/17 02:33:02  rotor
 *  * removed all rcsids
 *  * removed a bunch of ^L's that somehow crept in
 *  * removed old (and outdated) BUGS file
 *
 * Revision 6.11  2008/01/13 04:30:28  stever
 * Add braces around static initializers.
 *
 * Revision 6.10  2008/01/12 19:08:15  stever
 * Add __attribute__ ((unused)) to all rcsid variables.
 *
 * Revision 6.9  2007/12/11 12:43:01  rotor
 *  * added static to all global variables in main programs to avoid linking
 *       problems with libraries (compress in mincconvert and libz for example)
 *
 * Revision 6.8  2006/07/28 18:20:53  baghdadi
 * *** empty log message ***
 *
 * Revision 6.7  2006/07/28 17:51:01  baghdadi
 * Added option to print version of file
 * must use -minc_version -image_info
 *
 * Revision 6.6  2005/09/14 04:31:17  bert
 * include config.h
 *
 * Revision 6.5  2004/11/01 22:38:38  bert
 * Eliminate all references to minc_def.h
 *
 * Revision 6.4  2001/10/31 19:40:21  neelin
 * Fixed bug in printing of sign for default output - this was introduced
 * in the change to miget_datatype.
 *
 * Revision 6.3  2001/08/16 16:41:35  neelin
 * Added library functions to handle reading of datatype, sign and valid range,
 * plus writing of valid range and setting of default ranges. These functions
 * properly handle differences between valid_range type and image type. Such
 * difference can cause valid data to appear as invalid when double to float
 * conversion causes rounding in the wrong direction (out of range).
 * Modified voxel_loop, volume_io and programs to use these functions.
 *
 * Revision 6.2  2000/04/25 18:12:05  neelin
 * Added modified version of patch from Steve Robbins to allow use on
 * multiple input files.
 *
 * Revision 6.1  1999/10/19 14:45:24  neelin
 * Fixed Log subsitutions for CVS
 *
 * Revision 6.0  1997/09/12 13:23:35  neelin
 * Release of minc version 0.6
 *
 * Revision 5.0  1997/08/21  13:24:36  neelin
 * Release of minc version 0.5
 *
 * Revision 4.0  1997/05/07  20:00:38  neelin
 * Release of minc version 0.4
 *
 * Revision 3.1  1995/10/04  19:05:25  neelin
 * Fixed default_min for signed long.
 *
 * Revision 3.0  1995/05/15  19:31:31  neelin
 * Release of minc version 0.3
 *
 * Revision 2.3  1995/02/08  19:31:47  neelin
 * Moved ARGSUSED statements for irix 5 lint.
 *
 * Revision 2.2  1995/02/01  15:29:31  neelin
 * Fixed call of miexpand_file.
 *
 * Revision 2.1  95/01/23  13:47:46  neelin
 * Changed ncopen, ncclose to miopen, miclose. Added miexpand_file to get
 * header only when appropriate.
 * 
 * Revision 2.0  94/09/28  10:34:04  neelin
 * Release of minc version 0.2
 * 
 * Revision 1.10  94/09/28  10:34:00  neelin
 * Pre-release
 * 
 * Revision 1.9  93/08/11  15:45:53  neelin
 * Functions called by ParseArgv must check that nextArg is not NULL.
 * 
 * Revision 1.8  93/08/11  15:22:19  neelin
 * Added RCS logging in source.
 * 
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <minc.h>
#include <ParseArgv.h>

/* Constants */
#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif
#define MAX_NUM_OPTIONS 100
#define MAX_STRING_LEN  256
#define MAX_LS_COLUMNS  32
#define DEFAULT_LS_FORMAT "dims,size,steps,field,protocol,series"

/* Name of program, for error reporting */
static char* exec_name;

static char *type_names[] = {
   NULL, "byte", "char", "short", "long", "float", "double"
};

/* Types */
typedef enum  {
   ENDLIST, IMAGE_INFO, DIMNAMES, VARNAMES,
   DIMLENGTH, VARTYPE, VARDIMS, VARATTS, VARVALUES,
   ATTTYPE, ATTVALUE, MINC_VERSION
} Option_code;
typedef struct {
   Option_code code;
   char *value;
} Option_type;

/* -ls / -csv / -json support types */
typedef struct {
   char var_name[MAX_NC_NAME];
   char att_name[MAX_NC_NAME];
   char fallback_var[MAX_NC_NAME]; /* try if primary not found; "" = none */
   char fallback_att[MAX_NC_NAME];
   char value[MAX_STRING_LEN];
   int  found;
} AttrValue;

typedef struct {
   char      filename[MAX_STRING_LEN];
   int       ndims;
   char      dim_names[MAX_VAR_DIMS][MAX_NC_NAME];
   long      dim_lengths[MAX_VAR_DIMS];
   double    dim_steps[MAX_VAR_DIMS];
   int       has_step[MAX_VAR_DIMS];
   int       nattrs;
   AttrValue *attrs;
   int       is_valid;
} FileInfo;

typedef enum { COL_DIMS, COL_SIZE, COL_STEPS, COL_FIELD, COL_ATTR } ColumnType;

typedef struct {
   ColumnType type;
   char       short_name[64];
   char       long_name[64];
   char       var_name[MAX_NC_NAME];
   char       att_name[MAX_NC_NAME];
   char       fallback_var[MAX_NC_NAME]; /* "" = no fallback */
   char       fallback_att[MAX_NC_NAME];
   int        attr_index;
   int        width;
} Column;

/* Practical upper bound on unique dimension names across all input files.
 * Real MINC files have at most ~5 dims (vector, time, z, y, x). */
#define MAX_UNIQUE_DIMS 16
/* Effective (expanded) column: COL_SIZE and COL_STEPS each expand to one
 * column per dimension; all other column types pass through unchanged. */
#define MAX_EFF_COLS (MAX_LS_COLUMNS + MAX_UNIQUE_DIMS * 2)
typedef struct {
   ColumnType orig_type;
   int        orig_idx;       /* index into cols[] */
   char       dimname[64];   /* dimension name (SIZE/STEPS only); always short */
   char       short_name[64];
   char       long_name[96]; /* "dimname_step" fits in 96 chars */
} EffCol;

/* Macros */
#define REPORT_ERROR \
   {int s;if ((s=report_error())!=EXIT_SUCCESS) return s; goto error_label;}
#define CHK_ERR(code) if ((code) == MI_ERROR) {REPORT_ERROR}
#define RTN_ERR(code) if ((code) == MI_ERROR) {return MI_ERROR;}

/* Function prototypes */
static int process_file( char* filename, int header_only );
static int get_option(char *dst, char *key, char *nextarg);
static int report_error(void);
static int get_attname(int mincid, char *string, int *varid, char *name);
static int print_image_info(char *filename, int mincid);
static void parse_ls_format(const char *format_str, Column *cols, int *ncols);
static void gather_ls_info(const char *filename, FileInfo *info,
                           Column *cols, int ncols);
static void get_column_value(FileInfo *info, Column *col,
                             char *buf, int maxlen, int compact);
static int  collect_all_dims(FileInfo *infos, int nfiles,
                              char all_dims[][MAX_NC_NAME]);
static int  find_dim_index(FileInfo *info, const char *dimname);
static void dim_abbrev(const char *dimname, char *abbrev, int maxlen);
static int  build_eff_cols(Column *cols, int ncols,
                            FileInfo *infos, int nfiles,
                            EffCol eff[], int max_eff);
static void get_eff_col_value(FileInfo *info, EffCol *ec, Column *orig_col,
                               char *buf, int maxlen, int compact);
static void print_ls_table(FileInfo *infos, int nfiles,
                           Column *cols, int ncols);
static void print_ls_csv(FileInfo *infos, int nfiles,
                         Column *cols, int ncols);
static void print_ls_json(FileInfo *infos, int nfiles,
                          Column *cols, int ncols);
/* Variables used for argument parsing */
static char *error_string = NULL;
static Option_type option_list[MAX_NUM_OPTIONS] = { { ENDLIST, "" } };
static int length_option_list = 0;
/* Variables for ls/csv/json mode */
static int   do_ls  = 0;
static int   do_csv = 0;
static int   do_json = 0;
static char *ls_format_string = NULL;

/* Argument table */
ArgvInfo argTable[] = {
   {"-image_info", ARGV_FUNC, (char *) get_option, (char *) IMAGE_INFO,
       "Print out the default information about the images."},
   {"-dimnames", ARGV_FUNC, (char *) get_option, (char *) DIMNAMES,
       "Print the names of the dimensions in the file."},
   {"-varnames", ARGV_FUNC, (char *) get_option, (char *) VARNAMES,
       "Print the names of the variables in the file."},
   {"-dimlength", ARGV_FUNC, (char *) get_option, (char *) DIMLENGTH,
       "Print the length of the specified dimension."},
   {"-vartype", ARGV_FUNC, (char *) get_option, (char *) VARTYPE,
       "Print the type of the specified variable."},
   {"-vardims", ARGV_FUNC, (char *) get_option, (char *) VARDIMS,
       "Print the dimension names for the specified variable."},
   {"-varatts", ARGV_FUNC, (char *) get_option, (char *) VARATTS,
       "Print the attribute names for the specified variable."},
   {"-varvalues", ARGV_FUNC, (char *) get_option, (char *) VARVALUES,
       "Print the values for the specified variable."},
   {"-atttype", ARGV_FUNC, (char *) get_option, (char *) ATTTYPE,
       "Print the type of the specified attribute (variable:attribute)."},
   {"-attvalue", ARGV_FUNC, (char *) get_option, (char *) ATTVALUE,
       "Print the value(s) of the specified attribute (variable:attribute)."},
   {"-error_string", ARGV_STRING, (char *) 0, (char *) &error_string,
       "Error to print on stdout (default = exit with error status)."},
   {"-minc_version", ARGV_FUNC, (char *) get_option, (char *) MINC_VERSION,
    "Print the minc file version, netcdf or hdf5."},
   {"-ls", ARGV_CONSTANT, (char *) 1, (char *) &do_ls,
    "Print compact multi-file summary in column format."},
   {"-csv", ARGV_CONSTANT, (char *) 1, (char *) &do_csv,
    "Print compact multi-file summary in CSV format."},
   {"-json", ARGV_CONSTANT, (char *) 1, (char *) &do_json,
    "Print compact multi-file summary in JSON format."},
   {"-F", ARGV_STRING, (char *) 0, (char *) &ls_format_string,
    "Columns for -ls/-csv/-json (dims,size,steps,var:att,...)."},
   {"-format", ARGV_STRING, (char *) 0, (char *) &ls_format_string,
    "Same as -F."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};

/* Main program */

int main(int argc, char *argv[])
{
   int ioption, ifile, header_only;
   int ret_value = EXIT_SUCCESS;

   exec_name = argv[0];

   /* Check arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || (argc < 2)) {
      (void) fprintf(stderr,
                     "\nUsage: %s [<options>] <mincfile> [<mincfile> ...]\n",
                     exec_name);
      (void) fprintf(stderr,
                       "       %s -help\n\n", exec_name);
      exit(EXIT_FAILURE);
   }

   /* Handle ls/csv/json modes: collect info for all files then output */
   if (do_ls || do_csv || do_json) {
      const char *fmt = ls_format_string
                      ? ls_format_string
                      : getenv("MINCINFO_LS_FORMAT");
      Column cols[MAX_LS_COLUMNS];
      int ncols = 0;
      int nfiles = argc - 1;
      FileInfo *infos;
      int i;

      if (!fmt) fmt = DEFAULT_LS_FORMAT;
      parse_ls_format(fmt, cols, &ncols);

      infos = malloc(nfiles * sizeof(FileInfo));
      for (i = 0; i < nfiles; i++)
         gather_ls_info(argv[i + 1], &infos[i], cols, ncols);

      if (do_json)
         print_ls_json(infos, nfiles, cols, ncols);
      else if (do_csv)
         print_ls_csv(infos, nfiles, cols, ncols);
      else
         print_ls_table(infos, nfiles, cols, ncols);

      for (i = 0; i < nfiles; i++) {
         if (infos[i].attrs) free(infos[i].attrs);
      }
      free(infos);
      return EXIT_SUCCESS;
   }

   /* Check for no print options */
   if (option_list[0].code == ENDLIST) {
      option_list[0].code = IMAGE_INFO;
      option_list[0].value = NULL;
      length_option_list = 1;
   }

   /* Set error handling */
   if (error_string == NULL)
      set_ncopts(NC_VERBOSE | NC_FATAL);
   else
      set_ncopts(0);

   /* Loop through print options, checking whether we need variable data */
   header_only = TRUE;
   for (ioption=0; ioption < length_option_list; ioption++) {
      if (option_list[ioption].code == VARVALUES)
         header_only = FALSE;
   }

   for (ifile=1; ifile < argc; ++ifile) {
      if (process_file( argv[ifile], header_only ) != EXIT_SUCCESS)
         ret_value = EXIT_FAILURE;
      if (argc > 2)
         printf("\n\n");
   }
   return ret_value;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : print_image_version
@INPUT      : (boolean)
@OUTPUT     : (none)
@RETURNS    : Exit status.
@DESCRIPTION: Prints out image netcdf or hdf5
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Apr 6, 2005 (Leila Baghdadi)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int print_image_version(int Is_MINC2_File)
{
   if (Is_MINC2_File) {
     (void) printf("Version: 2 (HDF5)\n");
   }
   else {
     (void) printf("Version: 1 (netCDF)\n");
     
   }
    return EXIT_SUCCESS;
}

/* =========================================================================
 * -ls / -csv / -json implementation
 * ========================================================================= */

static void parse_ls_format(const char *format_str, Column *cols, int *ncols)
{
   char buf[4096];
   char *tok;
   int attr_index = 0;

   *ncols = 0;
   strncpy(buf, format_str, sizeof(buf)-1);
   buf[sizeof(buf)-1] = '\0';

   tok = strtok(buf, ",");
   while (tok != NULL && *ncols < MAX_LS_COLUMNS) {
      Column *c = &cols[*ncols];
      memset(c, 0, sizeof(*c));

      if (strcmp(tok, "dims") == 0) {
         c->type = COL_DIMS;
         strncpy(c->short_name, "dims", sizeof(c->short_name)-1);
         strncpy(c->long_name,  "dims", sizeof(c->long_name)-1);
         (*ncols)++;
      } else if (strcmp(tok, "size") == 0) {
         c->type = COL_SIZE;
         strncpy(c->short_name, "size", sizeof(c->short_name)-1);
         strncpy(c->long_name,  "size", sizeof(c->long_name)-1);
         (*ncols)++;
      } else if (strcmp(tok, "steps") == 0) {
         c->type = COL_STEPS;
         strncpy(c->short_name, "steps", sizeof(c->short_name)-1);
         strncpy(c->long_name,  "steps", sizeof(c->long_name)-1);
         (*ncols)++;
      } else if (strcmp(tok, "field") == 0) {
         c->type = COL_FIELD;
         strncpy(c->var_name,   "study",            sizeof(c->var_name)-1);
         strncpy(c->att_name,   "field_value",      sizeof(c->att_name)-1);
         strncpy(c->short_name, "field",            sizeof(c->short_name)-1);
         strncpy(c->long_name,  "study:field_value",sizeof(c->long_name)-1);
         c->attr_index = attr_index++;
         (*ncols)++;
      } else if (strcmp(tok, "protocol") == 0) {
         c->type = COL_ATTR;
         strncpy(c->var_name,      "acquisition",          sizeof(c->var_name)-1);
         strncpy(c->att_name,      "protocol",             sizeof(c->att_name)-1);
         strncpy(c->fallback_var,  "acquisition",          sizeof(c->fallback_var)-1);
         strncpy(c->fallback_att,  "protocol_name",        sizeof(c->fallback_att)-1);
         strncpy(c->short_name,    "protocol",             sizeof(c->short_name)-1);
         strncpy(c->long_name,     "acquisition:protocol", sizeof(c->long_name)-1);
         c->attr_index = attr_index++;
         (*ncols)++;
      } else if (strcmp(tok, "series") == 0) {
         c->type = COL_ATTR;
         strncpy(c->var_name,      "acquisition",
                 sizeof(c->var_name)-1);
         strncpy(c->att_name,      "series_description",
                 sizeof(c->att_name)-1);
         /* no fallback for series_description */
         strncpy(c->short_name,    "series_description",
                 sizeof(c->short_name)-1);
         strncpy(c->long_name,     "acquisition:series_description",
                 sizeof(c->long_name)-1);
         c->attr_index = attr_index++;
         (*ncols)++;
      } else {
         /* Must be var:att form */
         char *colon = strchr(tok, ':');
         if (colon != NULL && colon != tok && *(colon+1) != '\0') {
            int vlen = (int)(colon - tok);
            c->type = COL_ATTR;
            if (vlen >= MAX_NC_NAME) vlen = MAX_NC_NAME-1;
            strncpy(c->var_name, tok, vlen);
            c->var_name[vlen] = '\0';
            strncpy(c->att_name,   colon+1, sizeof(c->att_name)-1);
            strncpy(c->short_name, colon+1, sizeof(c->short_name)-1);
            strncpy(c->long_name,  tok,     sizeof(c->long_name)-1);
            c->att_name[sizeof(c->att_name)-1]     = '\0';
            c->short_name[sizeof(c->short_name)-1] = '\0';
            c->long_name[sizeof(c->long_name)-1]   = '\0';
            c->attr_index = attr_index++;
            (*ncols)++;
         } else {
            (void) fprintf(stderr,
                           "mincinfo: unknown format column '%s' "
                           "(use dims, size, steps, field, protocol, series, "
                           "or variable:attribute)\n",
                           tok);
         }
      }
      tok = strtok(NULL, ",");
   }
}

static void gather_ls_info(const char *filename, FileInfo *info,
                           Column *cols, int ncols)
{
   char *tempfile;
   int mincid, imgid, varid;
   int created_tempfile;
   int oldncopts;
   int ndims, dims[MAX_VAR_DIMS];
   int i, c;
   int nattrs = 0;

   memset(info, 0, sizeof(*info));
   strncpy(info->filename, filename, MAX_STRING_LEN-1);
   info->filename[MAX_STRING_LEN-1] = '\0';

   /* Count COL_ATTR and COL_FIELD columns */
   for (c = 0; c < ncols; c++) {
      if ((cols[c].type == COL_ATTR || cols[c].type == COL_FIELD) &&
          cols[c].attr_index >= nattrs)
         nattrs = cols[c].attr_index + 1;
   }
   info->nattrs = nattrs;
   if (nattrs > 0) {
      info->attrs = malloc(nattrs * sizeof(AttrValue));
      memset(info->attrs, 0, nattrs * sizeof(AttrValue));
      for (c = 0; c < ncols; c++) {
         if (cols[c].type == COL_ATTR || cols[c].type == COL_FIELD) {
            int idx = cols[c].attr_index;
            strncpy(info->attrs[idx].var_name,     cols[c].var_name,
                    MAX_NC_NAME-1);
            strncpy(info->attrs[idx].att_name,     cols[c].att_name,
                    MAX_NC_NAME-1);
            strncpy(info->attrs[idx].fallback_var, cols[c].fallback_var,
                    MAX_NC_NAME-1);
            strncpy(info->attrs[idx].fallback_att, cols[c].fallback_att,
                    MAX_NC_NAME-1);
         }
      }
   } else {
      info->attrs = NULL;
   }

   tempfile = miexpand_file(filename, NULL, TRUE, &created_tempfile);
   if (tempfile == NULL) {
      info->is_valid = 0;
      return;
   }

   oldncopts = get_ncopts();
   set_ncopts(0);

   mincid = miopen(tempfile, NC_NOWRITE);
   if (created_tempfile) remove(tempfile);
   free(tempfile);

   if (mincid == MI_ERROR) {
      set_ncopts(oldncopts);
      info->is_valid = 0;
      return;
   }

   /* Get image variable dimensions */
   imgid = ncvarid(mincid, MIimage);
   if (imgid != MI_ERROR &&
       ncvarinq(mincid, imgid, NULL, NULL, &ndims, dims, NULL) != MI_ERROR) {
      info->ndims = ndims;
      for (i = 0; i < ndims; i++) {
         ncdiminq(mincid, dims[i], info->dim_names[i], &info->dim_lengths[i]);
         varid = ncvarid(mincid, info->dim_names[i]);
         if (varid != MI_ERROR &&
             miattget1(mincid, varid, MIstep, NC_DOUBLE,
                       &info->dim_steps[i]) != MI_ERROR) {
            info->has_step[i] = 1;
         }
      }
   }

   /* Read requested attributes (both string and numeric), with fallback */
   for (i = 0; i < nattrs; i++) {
      nc_type datatype;
      int att_length;
      const char *try_var = info->attrs[i].var_name;
      const char *try_att = info->attrs[i].att_name;
      int pass;

      for (pass = 0; pass < 2; pass++) {
         if (pass == 1) {
            if (info->attrs[i].fallback_var[0] == '\0') break;
            try_var = info->attrs[i].fallback_var;
            try_att = info->attrs[i].fallback_att;
         }
         varid = ncvarid(mincid, try_var);
         if (varid == MI_ERROR) continue;
         if (ncattinq(mincid, varid, try_att,
                      &datatype, &att_length) == MI_ERROR) continue;

         if (datatype == NC_CHAR) {
            if (miattgetstr(mincid, varid, try_att,
                            MAX_STRING_LEN-1, info->attrs[i].value) != NULL)
               info->attrs[i].found = 1;
         } else {
            double *dvals = malloc(att_length * sizeof(double));
            if (dvals &&
                miattget(mincid, varid, try_att,
                         NC_DOUBLE, att_length, dvals, NULL) != MI_ERROR) {
               int j, pos = 0;
               for (j = 0; j < att_length && pos < MAX_STRING_LEN-1; j++) {
                  if (j > 0 && pos < MAX_STRING_LEN-2)
                     info->attrs[i].value[pos++] = ' ';
                  pos += snprintf(info->attrs[i].value + pos,
                                  MAX_STRING_LEN - pos - 1,
                                  "%g", dvals[j]);
               }
               info->attrs[i].value[MAX_STRING_LEN-1] = '\0';
               info->attrs[i].found = 1;
            }
            if (dvals) free(dvals);
         }
         if (info->attrs[i].found) break; /* don't try fallback */
      } /* end pass loop */
   } /* end attr loop */

   miclose(mincid);
   set_ncopts(oldncopts);
   info->is_valid = 1;
}

static void format_field_strength(const char *raw, char *buf, int maxlen)
{
   char tmp[MAX_STRING_LEN];
   char *endp;
   double val;
   int len;

   strncpy(tmp, raw, sizeof(tmp)-1);
   tmp[sizeof(tmp)-1] = '\0';
   len = (int)strlen(tmp);
   if (len > 0 && (tmp[len-1] == 'T' || tmp[len-1] == 't'))
      tmp[len-1] = '\0';

   val = strtod(tmp, &endp);
   if (endp != tmp && *endp == '\0')
      snprintf(buf, maxlen, "%.4gT", val);
   else
      strncpy(buf, raw, maxlen-1);
   buf[maxlen-1] = '\0';
}

static void get_column_value(FileInfo *info, Column *col,
                             char *buf, int maxlen, int compact)
{
   int i, pos;

   if (!info->is_valid) {
      if (col->type == COL_ATTR || col->type == COL_FIELD) {
         strncpy(buf, compact ? "-" : "", maxlen-1);
      } else {
         strncpy(buf, "ERROR", maxlen-1);
      }
      buf[maxlen-1] = '\0';
      return;
   }

   switch (col->type) {
   case COL_DIMS:
      snprintf(buf, maxlen, "%dD", info->ndims);
      break;
   case COL_SIZE:
      pos = 0;
      for (i = 0; i < info->ndims; i++) {
         if (i > 0 && pos < maxlen-1) buf[pos++] = 'x';
         pos += snprintf(buf+pos, maxlen-pos, "%ld", info->dim_lengths[i]);
      }
      if (info->ndims == 0) strncpy(buf, compact ? "-" : "", maxlen-1);
      buf[maxlen-1] = '\0';
      break;
   case COL_STEPS:
      pos = 0;
      for (i = 0; i < info->ndims; i++) {
         if (info->has_step[i]) {
            double v = info->dim_steps[i];
            if (pos > 0 && pos < maxlen-1) buf[pos++] = 'x';
            if (fabs(v) >= DBL_MAX / 2.0) {
               buf[pos++] = '?';
            } else if (compact) {
               pos += snprintf(buf+pos, maxlen-pos, "%.4g", fabs(v));
            } else {
               pos += snprintf(buf+pos, maxlen-pos, "%g", v);
            }
         }
      }
      if (pos == 0) strncpy(buf, compact ? "-" : "", maxlen-1);
      buf[maxlen-1] = '\0';
      break;
   case COL_FIELD:
      if (col->attr_index < info->nattrs &&
          info->attrs[col->attr_index].found) {
         if (compact)
            format_field_strength(info->attrs[col->attr_index].value,
                                  buf, maxlen);
         else
            strncpy(buf, info->attrs[col->attr_index].value, maxlen-1);
      } else {
         strncpy(buf, compact ? "-" : "", maxlen-1);
      }
      buf[maxlen-1] = '\0';
      break;
   case COL_ATTR:
      if (col->attr_index < info->nattrs &&
          info->attrs[col->attr_index].found) {
         strncpy(buf, info->attrs[col->attr_index].value, maxlen-1);
      } else {
         strncpy(buf, compact ? "-" : "", maxlen-1);
      }
      buf[maxlen-1] = '\0';
      break;
   }
}

static void print_dashes(int n)
{
   int i;
   for (i = 0; i < n; i++) putchar('-');
}

/* ---- Effective-column helpers ---- */

/* all_dims must have room for MAX_UNIQUE_DIMS entries, each MAX_NC_NAME chars */
static int collect_all_dims(FileInfo *infos, int nfiles,
                            char all_dims[][MAX_NC_NAME])
{
   int n = 0, f, d, k;
   for (f = 0; f < nfiles; f++) {
      for (d = 0; d < infos[f].ndims; d++) {
         int found = 0;
         for (k = 0; k < n; k++) {
            if (strcmp(all_dims[k], infos[f].dim_names[d]) == 0) {
               found = 1; break;
            }
         }
         if (!found && n < MAX_UNIQUE_DIMS) {
            strncpy(all_dims[n], infos[f].dim_names[d], MAX_NC_NAME-1);
            all_dims[n][MAX_NC_NAME-1] = '\0';
            n++;
         }
      }
   }
   return n;
}

static int find_dim_index(FileInfo *info, const char *dimname)
{
   int i;
   for (i = 0; i < info->ndims; i++)
      if (strcmp(info->dim_names[i], dimname) == 0)
         return i;
   return -1;
}

static void dim_abbrev(const char *dimname, char *abbrev, int maxlen)
{
   if      (strcmp(dimname, "xspace") == 0)           strncpy(abbrev, "x",  maxlen-1);
   else if (strcmp(dimname, "yspace") == 0)           strncpy(abbrev, "y",  maxlen-1);
   else if (strcmp(dimname, "zspace") == 0)           strncpy(abbrev, "z",  maxlen-1);
   else if (strcmp(dimname, "time")   == 0)           strncpy(abbrev, "t",  maxlen-1);
   else if (strcmp(dimname, "vector_dimension") == 0) strncpy(abbrev, "v",  maxlen-1);
   else                                               strncpy(abbrev, dimname, maxlen-1);
   abbrev[maxlen-1] = '\0';
}

/* Expand COL_SIZE and COL_STEPS into one EffCol per dimension.
 * All other column types pass through as a single EffCol. */
static int build_eff_cols(Column *cols, int ncols,
                          FileInfo *infos, int nfiles,
                          EffCol eff[], int max_eff)
{
   char all_dims[MAX_UNIQUE_DIMS][MAX_NC_NAME];
   int  n_all_dims = 0;
   int  n = 0, c, d, needs_dims = 0;
   char abbrev[64];

   for (c = 0; c < ncols; c++)
      if (cols[c].type == COL_SIZE || cols[c].type == COL_STEPS) {
         needs_dims = 1; break;
      }
   if (needs_dims)
      n_all_dims = collect_all_dims(infos, nfiles, all_dims);

   for (c = 0; c < ncols && n < max_eff; c++) {
      if (cols[c].type == COL_SIZE) {
         for (d = 0; d < n_all_dims && n < max_eff; d++) {
            eff[n].orig_type = COL_SIZE;
            eff[n].orig_idx  = c;
            strncpy(eff[n].dimname, all_dims[d], sizeof(eff[n].dimname)-1);
            eff[n].dimname[sizeof(eff[n].dimname)-1] = '\0';
            dim_abbrev(all_dims[d], abbrev, sizeof(abbrev));
            strncpy(eff[n].short_name, abbrev, sizeof(eff[n].short_name)-1);
            eff[n].short_name[sizeof(eff[n].short_name)-1] = '\0';
            strncpy(eff[n].long_name, all_dims[d], sizeof(eff[n].long_name)-1);
            eff[n].long_name[sizeof(eff[n].long_name)-1] = '\0';
            n++;
         }
      } else if (cols[c].type == COL_STEPS) {
         for (d = 0; d < n_all_dims && n < max_eff; d++) {
            eff[n].orig_type = COL_STEPS;
            eff[n].orig_idx  = c;
            strncpy(eff[n].dimname, all_dims[d], sizeof(eff[n].dimname)-1);
            eff[n].dimname[sizeof(eff[n].dimname)-1] = '\0';
            dim_abbrev(all_dims[d], abbrev, sizeof(abbrev));
            snprintf(eff[n].short_name, sizeof(eff[n].short_name), "d%s", abbrev);
            snprintf(eff[n].long_name,  sizeof(eff[n].long_name),
                     "%s_step", all_dims[d]);
            n++;
         }
      } else {
         eff[n].orig_type = cols[c].type;
         eff[n].orig_idx  = c;
         eff[n].dimname[0] = '\0';
         strncpy(eff[n].short_name, cols[c].short_name, sizeof(eff[n].short_name)-1);
         eff[n].short_name[sizeof(eff[n].short_name)-1] = '\0';
         strncpy(eff[n].long_name,  cols[c].long_name,  sizeof(eff[n].long_name)-1);
         eff[n].long_name[sizeof(eff[n].long_name)-1] = '\0';
         n++;
      }
   }
   return n;
}

/* Get the value of a single effective column for one file. */
static void get_eff_col_value(FileInfo *info, EffCol *ec, Column *orig_col,
                              char *buf, int maxlen, int compact)
{
   if (ec->orig_type == COL_SIZE) {
      int idx = find_dim_index(info, ec->dimname);
      if (idx >= 0)
         snprintf(buf, maxlen, "%ld", info->dim_lengths[idx]);
      else
         strncpy(buf, compact ? "-" : "", maxlen-1);
   } else if (ec->orig_type == COL_STEPS) {
      int idx = find_dim_index(info, ec->dimname);
      if (idx >= 0 && info->has_step[idx]) {
         double v = info->dim_steps[idx];
         if (fabs(v) >= DBL_MAX / 2.0)
            strncpy(buf, "?", maxlen-1);
         else if (compact)
            snprintf(buf, maxlen, "%.4g", fabs(v));
         else
            snprintf(buf, maxlen, "%g", v);
      } else {
         strncpy(buf, compact ? "-" : "", maxlen-1);
      }
   } else {
      get_column_value(info, orig_col, buf, maxlen, compact);
   }
   buf[maxlen-1] = '\0';
}

static void print_ls_table(FileInfo *infos, int nfiles,
                           Column *cols, int ncols)
{
   EffCol eff_cols[MAX_EFF_COLS];
   int    n_eff;
   char **values;
   int   *col_widths;
   int    file_width;
   int    f, c;

   if (nfiles == 0) return;

   n_eff = build_eff_cols(cols, ncols, infos, nfiles, eff_cols, MAX_EFF_COLS);

   values = malloc(nfiles * n_eff * sizeof(char *));
   col_widths = malloc(n_eff * sizeof(int));

   /* Seed widths from header labels */
   for (c = 0; c < n_eff; c++)
      col_widths[c] = (int)strlen(eff_cols[c].short_name);
   file_width = (int)strlen("file");

   /* Pass 1: fill values and compute widths */
   for (f = 0; f < nfiles; f++) {
      int flen = (int)strlen(infos[f].filename);
      if (flen > file_width) file_width = flen;
      for (c = 0; c < n_eff; c++) {
         char *v = malloc(MAX_STRING_LEN);
         get_eff_col_value(&infos[f], &eff_cols[c],
                           &cols[eff_cols[c].orig_idx], v, MAX_STRING_LEN, 1);
         values[f * n_eff + c] = v;
         int vlen = (int)strlen(v);
         if (vlen > col_widths[c]) col_widths[c] = vlen;
      }
   }

   /* Header */
   printf("%-*s", file_width, "file");
   for (c = 0; c < n_eff; c++)
      printf("  %-*s", col_widths[c], eff_cols[c].short_name);
   printf("\n");

   /* Dashes */
   print_dashes(file_width);
   for (c = 0; c < n_eff; c++) {
      printf("  ");
      print_dashes(col_widths[c]);
   }
   printf("\n");

   /* Rows */
   for (f = 0; f < nfiles; f++) {
      printf("%-*s", file_width, infos[f].filename);
      for (c = 0; c < n_eff; c++) {
         if (c < n_eff-1)
            printf("  %-*s", col_widths[c], values[f * n_eff + c]);
         else
            printf("  %s", values[f * n_eff + c]);
      }
      printf("\n");
   }

   for (f = 0; f < nfiles; f++)
      for (c = 0; c < n_eff; c++)
         free(values[f * n_eff + c]);
   free(values);
   free(col_widths);
}

/* CSV-quote a string: wrap in "" if it contains , or " and double internal " */
static void csv_print_field(const char *s)
{
   const char *p;
   int needs_quote = 0;

   for (p = s; *p; p++) {
      if (*p == ',' || *p == '"' || *p == '\n') { needs_quote = 1; break; }
   }
   if (!needs_quote) {
      printf("%s", s);
      return;
   }
   putchar('"');
   for (p = s; *p; p++) {
      if (*p == '"') putchar('"');
      putchar(*p);
   }
   putchar('"');
}

static void print_ls_csv(FileInfo *infos, int nfiles,
                         Column *cols, int ncols)
{
   EffCol eff_cols[MAX_EFF_COLS];
   int    n_eff = build_eff_cols(cols, ncols, infos, nfiles,
                                 eff_cols, MAX_EFF_COLS);
   char buf[MAX_STRING_LEN];
   int f, c;

   /* Header */
   printf("file");
   for (c = 0; c < n_eff; c++) {
      putchar(',');
      printf("%s", eff_cols[c].long_name);
   }
   printf("\n");

   /* Rows */
   for (f = 0; f < nfiles; f++) {
      csv_print_field(infos[f].filename);
      for (c = 0; c < n_eff; c++) {
         putchar(',');
         get_eff_col_value(&infos[f], &eff_cols[c],
                           &cols[eff_cols[c].orig_idx], buf, sizeof(buf), 0);
         csv_print_field(buf);
      }
      printf("\n");
   }
}

/* Print a JSON string with escaping */
static void json_print_string(const char *s)
{
   const unsigned char *p;
   putchar('"');
   for (p = (const unsigned char *)s; *p; p++) {
      if (*p == '"')       { putchar('\\'); putchar('"'); }
      else if (*p == '\\') { putchar('\\'); putchar('\\'); }
      else if (*p == '\n') { putchar('\\'); putchar('n'); }
      else if (*p == '\r') { putchar('\\'); putchar('r'); }
      else if (*p == '\t') { putchar('\\'); putchar('t'); }
      else if (*p < 0x20)  { printf("\\u%04x", (unsigned)*p); }
      else                  putchar(*p);
   }
   putchar('"');
}

static void print_ls_json(FileInfo *infos, int nfiles,
                          Column *cols, int ncols)
{
   char buf[MAX_STRING_LEN];
   int f, c, i;

   printf("[\n");
   for (f = 0; f < nfiles; f++) {
      FileInfo *info = &infos[f];
      printf("  {\n");

      /* filename */
      printf("    \"filename\": ");
      json_print_string(info->filename);
      printf(",\n");

      /* ndims */
      if (info->is_valid)
         printf("    \"ndims\": %d,\n", info->ndims);
      else
         printf("    \"ndims\": null,\n");

      /* dimensions object keyed by name */
      printf("    \"dimensions\": {");
      if (info->is_valid) {
         for (i = 0; i < info->ndims; i++) {
            if (i > 0) printf(", ");
            json_print_string(info->dim_names[i]);
            printf(": {\"length\": %ld", info->dim_lengths[i]);
            if (info->has_step[i] &&
                fabs(info->dim_steps[i]) < DBL_MAX / 2.0)
               printf(", \"step\": %g", info->dim_steps[i]);
            else
               printf(", \"step\": null");
            printf("}");
         }
      }
      printf("},\n");

      /* attributes object: COL_ATTR columns from format string */
      printf("    \"attributes\": {");
      {
         int first_attr = 1;
         for (c = 0; c < ncols; c++) {
            if (cols[c].type == COL_ATTR) {
               if (!first_attr) printf(", ");
               json_print_string(cols[c].long_name);
               printf(": ");
               if (info->is_valid && cols[c].attr_index < info->nattrs &&
                   info->attrs[cols[c].attr_index].found) {
                  json_print_string(info->attrs[cols[c].attr_index].value);
               } else {
                  printf("null");
               }
               first_attr = 0;
            }
         }
      }
      printf("}\n");

      if (f < nfiles-1)
         printf("  },\n");
      else
         printf("  }\n");
   }
   printf("]\n");
}

/* =========================================================================
 * End of -ls / -csv / -json implementation
 * ========================================================================= */

/* ----------------------------- MNI Header -----------------------------------
@NAME       : process_file
@INPUT      : filename
              header_only - TRUE if only header needs to be expanded
@OUTPUT     : (none)
@RETURNS    : Exit value for program
@DESCRIPTION: Runs mincinfo on one file
@METHOD     : (Adapted from old main of mincinfo.)
@GLOBALS    : 
@CALLS      : 
@CREATED    : April 25, 2000 (Steve Robbins)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int process_file( char* filename, int header_only )
{
   int mincid, varid, dimid;
   int ndims, dims[MAX_VAR_DIMS];
   nc_type datatype;
   long length, var_length, row_length;
   long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS];
   int att_length;
   int idim, iatt, natts, option;
   int nvars, ivar, ival;
   char *string;
   char name[MAX_NC_NAME];
   char *cdata;
   double *ddata;
   int created_tempfile;
   char *tempfile;
   int Is_MINC2_File=0;

   /* Expand file */
   tempfile = miexpand_file(filename, NULL, header_only, &created_tempfile);
   if (tempfile == NULL) {
      (void) fprintf(stderr, "%s: Error expanding file \"%s\"\n",
                     exec_name, filename);
      return EXIT_FAILURE;
   }

   /* Open the file */
   mincid = miopen(tempfile, NC_NOWRITE);
   if (created_tempfile) {
      (void) remove(tempfile);
   }
   if (mincid == MI_ERROR) {
      (void) fprintf(stderr, "%s: Error opening file \"%s\"\n",
                     exec_name, tempfile);
      return EXIT_FAILURE;
   }

   free(tempfile);

   /* check whether the file is Version 2 */
#ifdef MINC2
   if (MI2_ISH5OBJ(mincid)) {
     Is_MINC2_File = 1;
   }
#endif

   /* Loop through print options */
   for (option=0; option < length_option_list; option++) {
      string = option_list[option].value;
      switch (option_list[option].code) {
      case IMAGE_INFO:
         CHK_ERR(print_image_info(filename, mincid));
         break;
      case DIMNAMES:
         CHK_ERR(ncinquire(mincid, &ndims, NULL, NULL, NULL));
         for (idim=0; idim<ndims; idim++) {
            CHK_ERR(ncdiminq(mincid, idim, name, NULL));
            (void) printf("%s ", name);
         }
         (void) printf("\n");
         break;
      case VARNAMES:
         CHK_ERR(ncinquire(mincid, NULL, &nvars, NULL, NULL));
         for (ivar=0; ivar<nvars; ivar++) {
            CHK_ERR(ncvarinq(mincid, ivar, name, NULL, NULL, NULL, NULL));
            (void) printf("%s ", name);
         }
         (void) printf("\n");
         break;
      case DIMLENGTH:
         CHK_ERR(dimid = ncdimid(mincid, string));
         CHK_ERR(ncdiminq(mincid, dimid, NULL, &length));
         (void) printf("%d\n", (int) length);
         break;
      case VARTYPE:
         CHK_ERR(varid = ncvarid(mincid, string));
         CHK_ERR(ncvarinq(mincid, varid, NULL, &datatype, NULL, NULL, NULL));
         (void) printf("%s\n", type_names[datatype]);
         break;
      case VARDIMS:
         CHK_ERR(varid = ncvarid(mincid, string));
         CHK_ERR(ncvarinq(mincid, varid, NULL, NULL, &ndims, dims, NULL));
         for (idim=0; idim<ndims; idim++) {
            CHK_ERR(ncdiminq(mincid, dims[idim], name, NULL));
            (void) printf("%s ", name);
         }
         (void) printf("\n");
         break;
      case VARATTS:
         if (*string=='\0') {
            varid = NC_GLOBAL;
            CHK_ERR(ncinquire(mincid, NULL, NULL, &natts, NULL));
         }
         else {
            CHK_ERR(varid = ncvarid(mincid, string));
            CHK_ERR(ncvarinq(mincid, varid, NULL, NULL, NULL, NULL, &natts));
         }
         for (iatt=0; iatt<natts; iatt++) {
            CHK_ERR(ncattname(mincid, varid, iatt, name));
            (void) printf("%s ", name);
         }
         (void) printf("\n");
         break;
      case VARVALUES:
         CHK_ERR(varid = ncvarid(mincid, string));
         CHK_ERR(ncvarinq(mincid, varid, NULL, &datatype, &ndims, dims, NULL));
         var_length = 1;
         for (idim=0; idim<ndims; idim++) {
            CHK_ERR(ncdiminq(mincid, dims[idim], NULL, &length));
            start[idim] = 0;
            count[idim] = length;
            var_length *= length;
            if (idim==ndims-1)
               row_length = length;
         }
         if (datatype==NC_CHAR) {
            cdata = malloc(var_length*sizeof(char));
            CHK_ERR(ncvarget(mincid, varid, start, count, cdata));
            for (ival=0; ival<var_length; ival++) {
               (void) putchar((int) cdata[ival]);
               if (((ival+1) % row_length) == 0)
                  (void) putchar((int)'\n');
            }
            free(cdata);
         }
         else {
            ddata = malloc(var_length*sizeof(double));
            CHK_ERR(mivarget(mincid, varid, start, count, 
                             NC_DOUBLE, NULL, ddata));
            for (ival=0; ival<var_length; ival++) {
               (void) printf("%.20g\n", ddata[ival]);
            }
            free(ddata);
         }
         break;
      case ATTTYPE:
         CHK_ERR(get_attname(mincid, string, &varid, name));
         CHK_ERR(ncattinq(mincid, varid, name, &datatype, NULL));
         (void) printf("%s\n", type_names[datatype]);
         break;
      case ATTVALUE:
         CHK_ERR(get_attname(mincid, string, &varid, name));
         CHK_ERR(ncattinq(mincid, varid, name, &datatype, &att_length));
         if (datatype == NC_CHAR) {
            cdata = malloc((att_length+1)*sizeof(char));
            if (miattgetstr(mincid, varid, name, att_length+1, cdata)==NULL)
               {REPORT_ERROR}
            (void) printf("%s\n", cdata);
            free(cdata);
         }
         else {
            ddata = malloc(att_length * sizeof(double));
            CHK_ERR(miattget(mincid, varid, name, NC_DOUBLE, att_length,
                             ddata, NULL));
            for (iatt=0; iatt<att_length; iatt++) {
               (void) printf("%.20g ", ddata[iatt]);
            }
            (void) printf("\n");
            free(ddata);
         }
         break;
      case MINC_VERSION:
	CHK_ERR(print_image_version(Is_MINC2_File));
	break;
      default:
         (void) fprintf(stderr, "%s: Program bug!\n", exec_name);
         return EXIT_FAILURE;
      }
   error_label: ;
   }

   /* Close the file */
   (void) miclose(mincid);

   return EXIT_SUCCESS;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_option
@INPUT      : dst - client data passed by ParseArgv
              key - matching key in argv
              nextarg - argument following key in argv
@OUTPUT     : (none)
@RETURNS    : TRUE if nextarg is used, FALSE otherwise.
@DESCRIPTION: Gets command line options for information to print.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 19, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int get_option(char *dst, char *key, char *nextarg)
     /* ARGSUSED */
{
   Option_code code;
   int return_value;

   /* Check number of options */
   if (length_option_list >= MAX_NUM_OPTIONS-1) {
      (void) fprintf(stderr, "Too many options - maximum is %d.\n", 
                     MAX_NUM_OPTIONS - 1);
      exit(EXIT_FAILURE);
   }

   /* Save option */
   code  = (Option_code) dst;
   option_list[length_option_list].code = code;
   if ((code == IMAGE_INFO) |
       (code == MINC_VERSION) || 
       (code == DIMNAMES) ||
       (code == VARNAMES)){
      option_list[length_option_list].value = NULL;
      return_value = FALSE;
   }
   else {
      /* Check for following argument */
      if (nextarg == NULL) {
         (void) fprintf(stderr, 
                        "\"%s\" option requires an additional argument\n",
                        key);
         return FALSE;
      }
      option_list[length_option_list].value = nextarg;
      return_value = TRUE;
   }
   length_option_list++;
   option_list[length_option_list].code = ENDLIST;

   return return_value;
   
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : report_error
@INPUT      : (none)
@OUTPUT     : (none)
@RETURNS    : Exit status.
@DESCRIPTION: Prints out the error message
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 19, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int report_error(void)
{
   if (error_string == NULL) {
      (void) fprintf(stderr, "Error reading file.\n");
      return EXIT_FAILURE;
   }
   else {
      (void) fprintf(stdout, "%s\n", error_string);
      return EXIT_SUCCESS;
   }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_attname
@INPUT      : mincid - id of minc file
              string - string giving varname:attname
@OUTPUT     : varid - pointer to variale id
              name - name of attribute
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Gets variable id and attribute name from a string of the
              form "varname:attname"
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 19, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int get_attname(int mincid, char *string, int *varid, char *name)
{

#define ATT_SEP_CHAR ':'

   char *attname, varname[MAX_NC_NAME];
   int i;

   /* Get the variable name */
   for (i=0; (i < MAX_NC_NAME) && (string[i] != ATT_SEP_CHAR) 
           && (string[i] != '\0'); i++) {
      varname[i] = string[i];
   }
   if (string[i] != ATT_SEP_CHAR) {
      if (error_string == NULL) {
         (void) fprintf(stderr, "Invalid attribute name '%s'\n",
                        string);
      }
      return MI_ERROR;
   }
   varname[i] = '\0';
   attname = &string[i+1];
   
   /* Get varid and name */
   if (varname[0] == '\0') {
      *varid = NC_GLOBAL;
   }
   else if ((*varid = ncvarid(mincid, varname)) == MI_ERROR) {
      return MI_ERROR;
   }
   (void) strncpy(name, attname, MAX_NC_NAME-1);
   name[MAX_NC_NAME-1] = '\0';

   return MI_NOERROR;

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : print_image_info
@INPUT      : mincid - id of minc file
@OUTPUT     : (none)
@RETURNS    : MI_ERROR if an error occurs
@DESCRIPTION: Prints information about image data in file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : May 19, 1993 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
static int print_image_info(char *filename, int mincid)
{
   int imgid, ndims, dim[MAX_VAR_DIMS], varid;
   nc_type datatype;
   double valid_range[2];
   char *sign_type[] = {MI_UNSIGNED, MI_SIGNED};
   int sign_index;
   int is_signed;
   long length;
   int idim;
   char name[MAX_NC_NAME];
   int oldncopts;
   double dim_start, dim_step;

   /* Get information about variable */
   RTN_ERR(imgid = ncvarid(mincid, MIimage));
   RTN_ERR(ncvarinq(mincid, imgid, NULL, NULL, &ndims, dim, NULL));
   RTN_ERR(miget_datatype(mincid, imgid, &datatype, &is_signed));
   RTN_ERR(miget_valid_range(mincid, imgid, valid_range));

   /* Get sign index */
   sign_index = (is_signed ? 1 : 0);

   /* Write out image info line */
   (void) printf("file: %s\n", filename);
   (void) printf("image: %s %s %.20g to %.20g\n", 
                 sign_type[sign_index], type_names[datatype],
                 valid_range[0], valid_range[1]);

   /* Write out dimension names */
   (void) printf("image dimensions:");
   for (idim=0; idim<ndims; idim++) {
      RTN_ERR(ncdiminq(mincid, dim[idim], name, NULL));
      (void) printf(" %s", name);
   }
   (void) printf("\n");

   /* Write out dimension info */
   oldncopts =get_ncopts();
   set_ncopts(0);
   (void) printf("    %-20s %8s %12s %12s\n", "dimension name", "length",
                 "step", "start");
   (void) printf("    %-20s %8s %12s %12s\n", "--------------", "------",
                 "----", "-----");
   for (idim=0; idim<ndims; idim++) {
      (void) ncdiminq(mincid, dim[idim], name, &length);
      (void) printf("    %-20s %8d", name, (int) length);
      varid = ncvarid(mincid, name);
      if (miattget1(mincid, varid, MIstep, NC_DOUBLE, &dim_step)!=MI_ERROR)
         (void) printf(" %12g", dim_step);
      else 
         (void) printf(" %12s", "unknown");
      if (miattget1(mincid, varid, MIstart, NC_DOUBLE, &dim_start)!=MI_ERROR)
         (void) printf(" %12g", dim_start);
      else 
         (void) printf(" %12s", "unknown");
      (void) printf("\n");
   }
   set_ncopts(oldncopts);

   return MI_NOERROR;

}

