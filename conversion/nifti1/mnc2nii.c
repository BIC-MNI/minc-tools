#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <minc2.h>
#include <ParseArgv.h>

#include "restructure.h"

#include "nifti1_io.h"

#include "nifti1_local.h"       /* Our local definitions */

/* This list is in the order in which dimension lengths and sample
 * widths are stored in the NIfTI-1 structure.
 */
static const char *dimnames[MAX_NII_DIMS] = {
    MIvector_dimension,
    MItime,
    MIzspace,
    MIyspace,
    MIxspace,
    NULL,
    NULL,
    NULL
};


void test_xform(mat44 m, int i, int j, int k)
{
    double x, y, z;
    
    x = m.m[DIM_X][DIM_I] * i + m.m[DIM_X][DIM_J] * j + m.m[DIM_X][DIM_K] * k 
        + m.m[DIM_X][3];
    y = m.m[DIM_Y][DIM_I] * i + m.m[DIM_Y][DIM_J] * j + m.m[DIM_Y][DIM_K] * k 
        + m.m[DIM_Y][3];
    z = m.m[DIM_Z][DIM_I] * i + m.m[DIM_Z][DIM_J] * j + m.m[DIM_Z][DIM_K] * k 
        + m.m[DIM_Z][3];

    printf("%d %d %d => ", i, j, k);
    printf("%f %f %f\n", x, y, z);
}

static int usage(void)
{
    static const char msg[] = {
        "mnc2nii: Convert MINC files to NIfTI-1 format\n"
        "usage: mnc2nii [-q] [filetype] [datatype] filename.mnc [filename.nii]\n"
    };
    fprintf(stderr, "%s", msg);
    return (-1);
}


/* Explicitly set all of the fields of the NIfTI I/O header structure to
 * something reasonable. Right now this is overkill since a simple memset() 
 * would do the same job, but I want this function to help me keep track
 * of all of the header fields and to allow me to easily override a default
 * if it becomes useful.
 */
void
init_nifti_header(nifti_image *nii_ptr)
{
    int i, j;

    nii_ptr->ndim = 0;

    nii_ptr->nx = nii_ptr->ny = nii_ptr->nz = nii_ptr->nt = nii_ptr->nu = 
        nii_ptr->nv = nii_ptr->nw = 0;

    for (i = 0; i < MAX_NII_DIMS; i++) {
        /* Fix suggested by Hyun-Pil Kim (hpkim@ihanyang.ac.kr):
           Use 1 as the default, not zero */
        nii_ptr->dim[i] = 1;
    }

    nii_ptr->nvox = 0;
    nii_ptr->nbyper = 0;
    nii_ptr->datatype = DT_UNKNOWN;

    nii_ptr->dx = nii_ptr->dy = nii_ptr->dz = nii_ptr->dt = nii_ptr->du = 
        nii_ptr->dv = nii_ptr->dw = 0.0;
    
    for (i = 0; i < MAX_NII_DIMS; i++) {
        nii_ptr->pixdim[i] = 0.0;
    }

    nii_ptr->scl_slope = 0.0;
    nii_ptr->scl_inter = 0.0;
    nii_ptr->cal_min = 0.0;
    nii_ptr->cal_max = 0.0;

    nii_ptr->qform_code = NIFTI_XFORM_UNKNOWN;
    nii_ptr->sform_code = NIFTI_XFORM_UNKNOWN;

    nii_ptr->freq_dim = 0;
    nii_ptr->phase_dim = 0;
    nii_ptr->slice_dim = 0;

    nii_ptr->slice_code = 0;
    nii_ptr->slice_start = 0;
    nii_ptr->slice_end = 0;
    nii_ptr->slice_duration = 0.0;

    nii_ptr->quatern_b = 0.0;
    nii_ptr->quatern_c = 0.0;
    nii_ptr->quatern_d = 0.0;
    nii_ptr->qoffset_x = 0.0;
    nii_ptr->qoffset_y = 0.0;
    nii_ptr->qoffset_z = 0.0;
    nii_ptr->qfac = 0.0;

    nii_ptr->toffset = 0.0;

    nii_ptr->xyz_units = NIFTI_UNITS_MM; /* Default spatial units */
    nii_ptr->time_units = NIFTI_UNITS_SEC; /* Default time units */

    nii_ptr->nifti_type = FT_ANALYZE;
    nii_ptr->intent_code = 0;
    nii_ptr->intent_p1 = 0.0;
    nii_ptr->intent_p2 = 0.0;
    nii_ptr->intent_p3 = 0.0;
    memset(nii_ptr->intent_name, 0, sizeof (nii_ptr->intent_name));

    memset(nii_ptr->descrip, 0, sizeof (nii_ptr->descrip));

    memset(nii_ptr->aux_file, 0, sizeof (nii_ptr->aux_file));
    
    nii_ptr->fname = NULL;
    nii_ptr->iname = NULL;
    nii_ptr->iname_offset = 0;
    nii_ptr->swapsize = 0;
    nii_ptr->byteorder = 1; /* default order (LSB) */
    nii_ptr->data = NULL;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            nii_ptr->qto_xyz.m[i][j] = 0.0;
            nii_ptr->qto_ijk.m[i][j] = 0.0;
            nii_ptr->sto_xyz.m[i][j] = 0.0;
            nii_ptr->sto_ijk.m[i][j] = 0.0;
        }
    }
    
    nii_ptr->num_ext = 0;
}    

/* Calculate the first power of two greater than or equal to "value".
 */
double nearest_power_of_two(double value)
{
    double r = 1.0;
    while (r < value) {
        r *= 2.0;
    }
    return r;
}

int
main(int argc, char **argv)
{
    /* NIFTI stuff */
    nifti_image *nii_ptr;
    nifti_image nii_rec;
    int nii_dimids[MAX_NII_DIMS];
    int nii_dir[MAX_NII_DIMS];
    int nii_map[MAX_NII_DIMS];
    unsigned long nii_lens[MAX_NII_DIMS];
    int nii_ndims;
    static int nifti_filetype;
    static int nifti_datatype;
    static int nifti_signed = 1;

    /* MINC stuff */
    int mnc_fd;                 /* MINC file descriptor */
    nc_type mnc_type;           /* MINC data type as read */
    int mnc_ndims;              /* MINC image dimension count */
    int mnc_dimids[MAX_VAR_DIMS]; /* MINC image dimension identifiers */
    long mnc_dlen;              /* MINC dimension length value */
    double mnc_dstep;           /* MINC dimension step value */
    int mnc_icv;                /* MINC image conversion variable */
    int mnc_vid;                /* MINC Image variable ID */
    long mnc_start[MAX_VAR_DIMS]; /* MINC data starts */
    long mnc_count[MAX_VAR_DIMS]; /* MINC data counts */
    int mnc_signed;             /* MINC if output voxels are signed */
    double real_range[2];       /* MINC real range (min, max) */
    double input_valid_range[2]; /* MINC valid range (min, max) */
    double output_valid_range[2]; /* Valid range of output data. */
    double nifti_slope;         /* Slope to be applied to output voxels. */
    double nifti_inter;         /* Intercept to be applied to output voxels. */
    double total_valid_range;   /* Overall valid range (max - min). */
    double total_real_range;    /* Overall real range (max - min). */

    /* Other stuff */
    char out_str[1024];         /* Big string for filename */
    char att_str[1024];         /* Big string for attribute values */
    int i;                      /* Generic loop counter the first */
    int j;                      /* Generic loop counter the second */
    char *str_ptr;              /* Generic ASCIZ string pointer */
    int r;                      /* Result code. */
    static int vflag = 0;       /* Verbose flag (default is quiet) */

    static ArgvInfo argTable[] = {
        {NULL, ARGV_HELP, NULL, NULL,
         "Output voxel data type specification"},
        {"-byte", ARGV_CONSTANT, (char *)DT_INT8, (char *)&nifti_datatype,
         "Write voxel data in 8-bit signed integer format."},
        {"-short", ARGV_CONSTANT, (char *)DT_INT16, (char *)&nifti_datatype,
         "Write voxel data in 16-bit signed integer format."},
        {"-int", ARGV_CONSTANT, (char *)DT_INT32, (char *)&nifti_datatype,
         "Write voxel data in 32-bit signed integer format."},
        {"-float", ARGV_CONSTANT, (char *)DT_FLOAT32, (char *)&nifti_datatype,
         "Write voxel data in 32-bit floating point format."},
        {"-double", ARGV_CONSTANT, (char *)DT_FLOAT64, (char *)&nifti_datatype,
         "Write voxel data in 64-bit floating point format."},
        {"-signed", ARGV_CONSTANT, (char *)1, (char *)&nifti_signed,
         "Write integer voxel data in signed format."},
        {"-unsigned", ARGV_CONSTANT, (char *)0, (char *)&nifti_signed,
         "Write integer voxel data in unsigned format."},
        {NULL, ARGV_HELP, NULL, NULL,
         "Output file format specification"},
        {"-dual", ARGV_CONSTANT, (char *)FT_NIFTI_DUAL, 
         (char *)&nifti_filetype,
         "Write NIfTI-1 two-file format (.img and .hdr)"},
        {"-ASCII", ARGV_CONSTANT, (char *)FT_NIFTI_ASCII, 
         (char *)&nifti_filetype,
         "Write NIfTI-1 ASCII header format (.nia)"},
        {"-nii", ARGV_CONSTANT, (char *)FT_NIFTI_SINGLE, 
         (char *)&nifti_filetype,
         "Write NIfTI-1 one-file format (.nii)"},
        {"-analyze", ARGV_CONSTANT, (char *)FT_ANALYZE, 
         (char *)&nifti_filetype,
         "Write an Analyze two-file format file (.img and .hdr)"},
        {NULL, ARGV_HELP, NULL, NULL,
         "Other options"},
        {"-quiet", ARGV_CONSTANT, (char *)0, 
         (char *)&vflag,
         "Quiet operation"},
        {"-verbose", ARGV_CONSTANT, (char *)1, 
         (char *)&vflag,
         "Quiet operation"},
        {NULL, ARGV_END, NULL, NULL, NULL}
    };

    set_ncopts(0);                 /* Clear global netCDF error reporting flag */

    /* Default NIfTI file type is "NII", single binary file
     */
    nifti_filetype = FT_UNSPECIFIED;
    nifti_datatype = DT_UNKNOWN;

    if (ParseArgv(&argc, argv, argTable, 0) || (argc < 2)) {
        fprintf(stderr, "Too few arguments\n");
        return usage();
    }

    if (!nifti_signed) {
        switch (nifti_datatype) {
        case DT_INT8:
            nifti_datatype = DT_UINT8;
            break;
        case DT_INT16:
            nifti_datatype = DT_UINT16;
            break;
        case DT_INT32:
            nifti_datatype = DT_UINT32;
            break;
        }
    }
    switch (nifti_datatype){
    case DT_INT8:
    case DT_UINT8:
        mnc_type = NC_BYTE;
        break;
    case DT_INT16:
    case DT_UINT16:
        mnc_type = NC_SHORT;
        break;
    case DT_INT32:
    case DT_UINT32:
        mnc_type = NC_INT;
        break;
    case DT_FLOAT32:
        mnc_type = NC_FLOAT;
        break;
    case DT_FLOAT64:
        mnc_type = NC_DOUBLE;
        break;
    }

    if (argc == 2) {
        strcpy(out_str, argv[1]);
        str_ptr = strrchr(out_str, '.');
        if (str_ptr != NULL && !strcmp(str_ptr, ".mnc")) {
            *str_ptr = '\0';
        }
    }
    else if (argc == 3) {
        strcpy(out_str, argv[2]);
        str_ptr = strrchr(out_str, '.');
        if (str_ptr != NULL) {
            /* See if a recognized file extension was specified.  If so,
             * we trim it off and set the output file type if none was
             * specified.  If the extension is not recognized, assume
             * that we will form the filename by just adding the right
             * extension for the selected output format.
             */
            if (!strcmp(str_ptr, ".nii")) {
                if (nifti_filetype == FT_UNSPECIFIED) {
                    nifti_filetype = FT_NIFTI_SINGLE;
                }
                *str_ptr = '\0';
            }
            else if (!strcmp(str_ptr, ".img") || 
                     !strcmp(str_ptr, ".hdr")) {
                if (nifti_filetype == FT_UNSPECIFIED) {
                    nifti_filetype = FT_NIFTI_DUAL;
                }
                *str_ptr = '\0';
            }
            else if (!strcmp(str_ptr, ".nia")) {
                if (nifti_filetype == FT_UNSPECIFIED) {
                    nifti_filetype = FT_NIFTI_ASCII;
                }
                *str_ptr = '\0';
            }
        }
    }
    else {
        fprintf(stderr, "Filename argument required\n");
        return usage();
    }

    /* Open the MINC file.  It needs to exist.
     */
    mnc_fd = miopen(argv[1], NC_NOWRITE);
    if (mnc_fd < 0) {
        fprintf(stderr, "Can't find input file '%s'\n", argv[1]);
        return (-1);
    }

    /* Find the MINC image variable.  If we can't find it, there is no
     * further processing possible...
     */
    mnc_vid = ncvarid(mnc_fd, MIimage);
    if (mnc_vid < 0) {
        fprintf(stderr, "Can't locate the image variable (mnc_vid=%d)\n", mnc_vid);
        return (-1);
    }

    /* Find out about the MINC image variable - specifically, how many
     * dimensions, and which dimensions.
     */
    r = ncvarinq(mnc_fd, mnc_vid, NULL, NULL, &mnc_ndims, mnc_dimids, NULL);
    if (r < 0) {
        fprintf(stderr, "Can't read information from image variable\n");
        return (-1);
    }
    if (mnc_ndims > MAX_NII_DIMS) {
        fprintf(stderr, "NIfTI-1 files may contain at most %d dimensions\n", 
                MAX_NII_DIMS);
        return (-1);
    }

    /* Initialize the NIfTI structure 
     */
    nii_ptr = &nii_rec;

    init_nifti_header(nii_ptr);

    /* For now we just use the mnc2nii command line as the description
     * field.  Probably we should use something better, perhaps a
     * combination of some other standard MINC fields that might
     * provide more information.
     */
    str_ptr = nii_ptr->descrip;
    for (i = 0; i < argc; i++) {
        char *arg_ptr = argv[i];

        if ((str_ptr - nii_ptr->descrip) >= MAX_NII_DESCRIP) {
            break;
        }

        if (i != 0) {
            *str_ptr++ = ' ';
        }

        while (*arg_ptr != '\0' && 
               (str_ptr - nii_ptr->descrip) < MAX_NII_DESCRIP) {
            *str_ptr++ = *arg_ptr++;
        }
        *str_ptr = '\0';
    }

    nii_ptr->fname = malloc(strlen(out_str) + 4 + 1);
    nii_ptr->iname = malloc(strlen(out_str) + 4 + 1);
    strcpy(nii_ptr->fname, out_str);
    strcpy(nii_ptr->iname, out_str);

    switch (nifti_filetype) {
    case FT_ANALYZE:
        strcat(nii_ptr->fname, ".hdr");
        strcat(nii_ptr->iname, ".img");
        break;
    case FT_NIFTI_SINGLE:
        strcat(nii_ptr->fname, ".nii");
        strcat(nii_ptr->iname, ".nii");
        break;
    case FT_NIFTI_DUAL:
        strcat(nii_ptr->fname, ".hdr");
        strcat(nii_ptr->iname, ".img");
        break;
    case FT_NIFTI_ASCII:
        strcat(nii_ptr->fname, ".nia");
        strcat(nii_ptr->iname, ".nia");
        break;
    default:
        fprintf(stderr, "Unknown output file type %d\n", nifti_filetype);
        return (-1);
    }

    /* Get real voxel range for the input file.
     */
    miget_image_range(mnc_fd, real_range);

    /* Get the actual valid voxel value range.
     */
    miget_valid_range(mnc_fd, mnc_vid, input_valid_range);

    /* Find the default range for the output type. Our output file
     * will use the full legal range of the output type if it is
     * an integer.
     */

    if (nifti_datatype == DT_UNKNOWN) {
        nii_ptr->datatype = DT_FLOAT32; /* Default */
        mnc_type = NC_FLOAT;
        mnc_signed = 1;
    }
    else {
        nii_ptr->datatype = nifti_datatype;
        mnc_signed = nifti_signed;
    }

    if (vflag) {
        fprintf(stderr, "MINC type %d signed %d\n", mnc_type, mnc_signed);
    }

    miget_default_range(mnc_type, mnc_signed, output_valid_range);

    total_valid_range = input_valid_range[1] - input_valid_range[0];
    total_real_range = real_range[1] - real_range[0];

    if ((output_valid_range[1] - output_valid_range[0]) > total_valid_range) {
        /* Empirically, forcing the valid range to be the nearest power
         * of two greater than the existing valid range seems to improve
         * the behavior of the conversion. This is at least in part because
         * of the limited precision of the NIfTI-1 voxel scaling fields.
         */
        double new_range = nearest_power_of_two(total_valid_range);
        if (new_range - 1.0 >= total_valid_range) {
            new_range -= 1.0;
        }

        if (output_valid_range[1] > total_valid_range) {
            output_valid_range[0] = 0;
            output_valid_range[1] = new_range;
        }
        else {
            output_valid_range[1] = output_valid_range[0] + new_range;
        }
    }
    else {
        /* The new range can't fully represent the input range. Use the 
         * full available range, and warn the user that they may have a
         * problem.
         */
        printf("WARNING: Range of input exceeds range of output format.\n");
    }

    if (vflag) {
        printf("Real range: %f %f Input valid range: %f %f Output valid range: %f %f\n",
               real_range[0], real_range[1],
               input_valid_range[0], input_valid_range[1],
               output_valid_range[0], output_valid_range[1]);
    }

    /* If the output type is not floating point, we may need to scale the
     * voxel values.
     */

    if (mnc_type != NC_FLOAT && mnc_type != NC_DOUBLE) {

        /* Figure out how to map pixel values into the range of the 
         * output datatype.
         */
        nifti_slope = ((real_range[1] - real_range[0]) / 
                       (output_valid_range[1] - output_valid_range[0]));

        if (nifti_slope == 0.0) {
            nifti_slope = 1.0;
        }
        nifti_inter = real_range[0] - (output_valid_range[0] * nifti_slope);

        /* One problem with NIfTI-1 is the limited precision of the 
         * scl_slope and scl_inter fields (they are just 32-bits). So
         * we look for possible issues and warn about that here.
         */
        if (nifti_inter != (float) nifti_inter || 
            nifti_slope != (float) nifti_slope) {
            double epsilon_i = nifti_inter - (float) nifti_inter;
            double epsilon_s = nifti_slope - (float) nifti_slope;

            /* If the loss in precision is more than one part per thousand
             * of the real range, flag this as a problem!
             */
            if ((epsilon_i > total_real_range / 1.0e3) ||
                (epsilon_s > total_real_range / 1.0e3)) {
                fprintf(stderr, "ERROR: Slope and intercept cannot be represented in the NIfTI-1 header.\n");
                fprintf(stderr, "      slope %f (%f), intercept %f (%f)\n", 
                        nifti_slope, (float) nifti_slope,
                        nifti_inter, (float) nifti_inter);
                return (-1);
            }
        }
    }
    else {
        nifti_slope = 0.0;
    }

    nii_ptr->scl_slope = nifti_slope;
    nii_ptr->scl_inter = nifti_inter;

    nii_ptr->nvox = 1;          /* Initial value for voxel count */

    /* Find all of the dimensions of the MINC file, in the order they 
     * will be listed in the NIfTI-1/Analyze file.  We use this to build
     * a map for restructuring the data according to the normal rules
     * of NIfTI-1.
     */
    nii_ndims = 0;
    for (i = 0; i < MAX_NII_DIMS; i++) {
        if (dimnames[i] == NULL) {
            nii_dimids[nii_ndims] = -1;
            continue;
        }

        nii_dimids[nii_ndims] = ncdimid(mnc_fd, dimnames[i]);
        if (nii_dimids[nii_ndims] == -1) {
            continue;
        }

        /* Make sure the dimension is actually used to define the image.
         */
        for (j = 0; j < mnc_ndims; j++) {
            if (nii_dimids[nii_ndims] == mnc_dimids[j]) {
                nii_map[nii_ndims] = j;
                break;
            }
        }

        if (j < mnc_ndims) {
            mnc_dlen = 1;
            mnc_dstep = 0;

            ncdiminq(mnc_fd, nii_dimids[nii_ndims], NULL, &mnc_dlen);
            ncattget(mnc_fd, ncvarid(mnc_fd, dimnames[i]), MIstep, &mnc_dstep);

            if (mnc_dstep < 0) {
                nii_dir[nii_ndims] = -1;
                mnc_dstep = -mnc_dstep;
            }
            else {
                nii_dir[nii_ndims] = 1;
            }

            nii_lens[nii_ndims] = mnc_dlen;
            nii_ndims++;
        }

        nii_ptr->dim[dimmap[i]] = (int) mnc_dlen;
        nii_ptr->nvox *= mnc_dlen;

        nii_ptr->pixdim[dimmap[i]] = (float) mnc_dstep;
    }

    /* Here we do some "post-processing" of the results. Make certain that
     * the nt value is never zero, and make certain that ndim is set to
     * 4 if there is a time dimension and 5 if there is a vector dimension
     */

    if (nii_ptr->dim[3] > 1 && nii_ndims < 4) {
        nii_ndims = 4;
    }

    if (nii_ptr->dim[4] > 1) {
        nii_ptr->intent_code = NIFTI_INTENT_VECTOR;
        nii_ndims = 5;
    }

    nii_ptr->ndim = nii_ndims; /* Total number of dimensions in file */
    nii_ptr->nx = nii_ptr->dim[0];
    nii_ptr->ny = nii_ptr->dim[1];
    nii_ptr->nz = nii_ptr->dim[2];
    nii_ptr->nt = nii_ptr->dim[3];
    nii_ptr->nu = nii_ptr->dim[4];

    nii_ptr->dx = nii_ptr->pixdim[0];
    nii_ptr->dy = nii_ptr->pixdim[1];
    nii_ptr->dz = nii_ptr->pixdim[2];
    nii_ptr->dt = nii_ptr->pixdim[3];
    nii_ptr->du = 1; /* MINC files don't define a sample size for a vector_dimension */

    nii_ptr->nifti_type = nifti_filetype;

    /* Load the direction_cosines and start values into the NIfTI-1 
     * sform structure.
     *
     */
    for (i = 0; i < MAX_SPACE_DIMS; i++) {
        int id = ncvarid(mnc_fd, mnc_spatial_names[i]);
        double start;
        double step;
        double dircos[MAX_SPACE_DIMS];
        int tmp;

        if (id < 0) {
            continue;
        }

        /* Set default values */
        start = 0.0;
        step = 1.0;
        dircos[DIM_X] = dircos[DIM_Y] = dircos[DIM_Z] = 0.0;
        dircos[i] = 1.0;

        miattget(mnc_fd, id, MIstart, NC_DOUBLE, 1, &start, &tmp);
        miattget(mnc_fd, id, MIstep, NC_DOUBLE, 1, &step, &tmp);
        miattget(mnc_fd, id, MIdirection_cosines, NC_DOUBLE, MAX_SPACE_DIMS, 
                 dircos, &tmp);
        ncdiminq(mnc_fd, ncdimid(mnc_fd, mnc_spatial_names[i]), NULL, 
                 &mnc_dlen);

        if (step < 0) {
            step = -step;
            start = start - step * (mnc_dlen - 1);
        }

        nii_ptr->sto_xyz.m[0][i] = step * dircos[0];
        nii_ptr->sto_xyz.m[1][i] = step * dircos[1];
        nii_ptr->sto_xyz.m[2][i] = step * dircos[2];

        nii_ptr->sto_xyz.m[0][3] += start * dircos[0];
        nii_ptr->sto_xyz.m[1][3] += start * dircos[1];
        nii_ptr->sto_xyz.m[2][3] += start * dircos[2];

        miattgetstr(mnc_fd, id, MIspacetype, sizeof(att_str), att_str);

        /* Try to set the S-transform code correctly.
         */
        if (!strcmp(att_str, MI_TALAIRACH)) {
            nii_ptr->sform_code = NIFTI_XFORM_TALAIRACH;
        }
        else if (!strcmp(att_str, MI_CALLOSAL)) {
            /* TODO: Not clear what do do here... */
            nii_ptr->sform_code = NIFTI_XFORM_SCANNER_ANAT;
        }
        else {                  /* MI_NATIVE or unknown */
            nii_ptr->sform_code = NIFTI_XFORM_SCANNER_ANAT;
        }
    }

    /* So the last row is right... */
    nii_ptr->sto_xyz.m[3][0] = 0.0;
    nii_ptr->sto_xyz.m[3][1] = 0.0;
    nii_ptr->sto_xyz.m[3][2] = 0.0;
    nii_ptr->sto_xyz.m[3][3] = 1.0;

    nii_ptr->sto_ijk = nifti_mat44_inverse(nii_ptr->sto_xyz);

    nifti_datatype_sizes(nii_ptr->datatype, 
                         &nii_ptr->nbyper, &nii_ptr->swapsize);


    if (vflag) {
        nifti_image_infodump(nii_ptr);
    }

    /* Now load the actual MINC data. */

    nii_ptr->data = malloc(nii_ptr->nbyper * nii_ptr->nvox);
    if (nii_ptr->data == NULL) {
        fprintf(stderr, "Out of memory.\n");
        return (-1);
    }

    mnc_icv = miicv_create();
    miicv_setint(mnc_icv, MI_ICV_TYPE, mnc_type);
    miicv_setstr(mnc_icv, MI_ICV_SIGN, (mnc_signed) ? MI_SIGNED : MI_UNSIGNED);
    miicv_setdbl(mnc_icv, MI_ICV_VALID_MAX, output_valid_range[1]);
    miicv_setdbl(mnc_icv, MI_ICV_VALID_MIN, output_valid_range[0]);
    miicv_setdbl(mnc_icv, MI_ICV_IMAGE_MAX, real_range[1]);
    miicv_setdbl(mnc_icv, MI_ICV_IMAGE_MIN, real_range[0]);
    miicv_setdbl(mnc_icv, MI_ICV_DO_NORM, TRUE);
    miicv_setdbl(mnc_icv, MI_ICV_USER_NORM, TRUE);

    miicv_attach(mnc_icv, mnc_fd, mnc_vid);

    /* Read in the entire hyperslab from the file.
     */
    for (i = 0; i < mnc_ndims; i++) {
        ncdiminq(mnc_fd, mnc_dimids[i], NULL, &mnc_count[i]);
        mnc_start[i] = 0;
    }

    r = miicv_get(mnc_icv, mnc_start, mnc_count, nii_ptr->data);
    if (r < 0) {
        fprintf(stderr, "Read error\n");
        return (-1);
    }

    /* Shut down the MINC stuff now that it has done its work. 
     */
    miicv_detach(mnc_icv);
    miicv_free(mnc_icv);
    miclose(mnc_fd);

    if (vflag) {
        /* Debugging stuff - just to check the contents of these arrays.
         */
        for (i = 0; i < nii_ndims; i++) {
            printf("%d: %ld %d %d\n", 
                   i, nii_lens[i], nii_map[i], nii_dir[i]);
        }
        printf("bytes per voxel %d\n", nii_ptr->nbyper);
        printf("# of voxels %ld\n", nii_ptr->nvox);
    }

    /* Rearrange the data to correspond to the NIfTI dimension ordering.
     */
    restructure_array(nii_ndims,
                      nii_ptr->data,
                      nii_lens,
                      nii_ptr->nbyper,
                      nii_map,
                      nii_dir);

    if (vflag) {
        /* More debugging stuff - check coordinate transform.
         */
        test_xform(nii_ptr->sto_xyz, 0, 0, 0);
        test_xform(nii_ptr->sto_xyz, 10, 0, 0);
        test_xform(nii_ptr->sto_xyz, 0, 10, 0);
        test_xform(nii_ptr->sto_xyz, 0, 0, 10);
        test_xform(nii_ptr->sto_xyz, 10, 10, 10);
    }

    if (vflag) {
        fprintf(stdout, "Writing NIfTI-1 file...");
    }
    nifti_image_write(nii_ptr);
    if (vflag) {
        fprintf(stdout, "done.\n");
    }

    return (0);
}
