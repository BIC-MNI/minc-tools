/* _GNU_SOURCE enables mkdtemp() and nftw() on glibc regardless of _XOPEN_SOURCE */
#define _GNU_SOURCE

#include "archive_support.h"

#ifdef HAVE_LIBARCHIVE

#include <archive.h>
#include <archive_entry.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>   /* strcasecmp */
#include <sys/stat.h>
#include <unistd.h>

/* --------------------------------------------------------------------------
 * is_archive_file()
 * -------------------------------------------------------------------------- */
int is_archive_file(const char *path)
{
    size_t len;
    if (!path) return 0;
    len = strlen(path);

    /* .zip */
    if (len >= 4 && strcasecmp(path + len - 4, ".zip") == 0)
        return 1;
    /* .tgz */
    if (len >= 4 && strcasecmp(path + len - 4, ".tgz") == 0)
        return 1;
    /* .tar.gz */
    if (len >= 7 && strcasecmp(path + len - 7, ".tar.gz") == 0)
        return 1;
    /* .tar.bz2 */
    if (len >= 8 && strcasecmp(path + len - 8, ".tar.bz2") == 0)
        return 1;

    return 0;
}

/* --------------------------------------------------------------------------
 * extract_archive_to_tmpdir()
 * Uses the libarchive "copy data" pattern recommended in the libarchive wiki.
 * -------------------------------------------------------------------------- */

static int copy_data(struct archive *ar, struct archive *aw)
{
    int r;
    const void *buff;
    size_t size;
    la_int64_t offset;

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF)
            return ARCHIVE_OK;
        if (r < ARCHIVE_OK)
            return r;
        r = (int)archive_write_data_block(aw, buff, size, offset);
        if (r < ARCHIVE_OK) {
            fprintf(stderr, "dcm2mnc: archive write error: %s\n",
                    archive_error_string(aw));
            return r;
        }
    }
}

int extract_archive_to_tmpdir(const char *archive_path, char **tmpdir_out)
{
    struct archive *a      = NULL;
    struct archive *disk   = NULL;
    struct archive_entry *entry;
    char tmpdir_template[] = "/tmp/dcm2mnc_XXXXXX";
    char *tmpdir           = NULL;
    int   flags, r;
    int   ret = -1;

    /* Create temp directory */
    tmpdir = mkdtemp(tmpdir_template);
    if (!tmpdir) {
        perror("dcm2mnc: mkdtemp");
        return -1;
    }
    *tmpdir_out = strdup(tmpdir);
    if (!*tmpdir_out) {
        fprintf(stderr, "dcm2mnc: out of memory\n");
        return -1;
    }

    /* Open archive for reading */
    a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_filter_all(a);

    r = archive_read_open_filename(a, archive_path, 10240);
    if (r != ARCHIVE_OK) {
        fprintf(stderr, "dcm2mnc: cannot open archive %s: %s\n",
                archive_path, archive_error_string(a));
        goto cleanup;
    }

    /* Open disk writer */
    flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM |
            ARCHIVE_EXTRACT_SECURE_NODOTDOT;
    disk = archive_write_disk_new();
    archive_write_disk_set_options(disk, flags);
    archive_write_disk_set_standard_lookup(disk);

    /* Extract loop */
    for (;;) {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF)
            break;
        if (r < ARCHIVE_WARN) {
            fprintf(stderr, "dcm2mnc: archive read error: %s\n",
                    archive_error_string(a));
            goto cleanup;
        }

        /* Rebase pathname into our tmpdir */
        {
            char dest[4096];
            const char *orig = archive_entry_pathname(entry);
            snprintf(dest, sizeof(dest), "%s/%s", *tmpdir_out, orig);
            archive_entry_set_pathname(entry, dest);
        }

        r = archive_write_header(disk, entry);
        if (r < ARCHIVE_OK) {
            fprintf(stderr, "dcm2mnc: archive_write_header: %s\n",
                    archive_error_string(disk));
        } else if (archive_entry_size(entry) > 0) {
            r = copy_data(a, disk);
            if (r < ARCHIVE_WARN) goto cleanup;
        }

        r = archive_write_finish_entry(disk);
        if (r < ARCHIVE_WARN) goto cleanup;
    }

    ret = 0;  /* success */

cleanup:
    if (a)    { archive_read_close(a);  archive_read_free(a);   }
    if (disk) { archive_write_close(disk); archive_write_free(disk); }
    if (ret != 0 && *tmpdir_out) {
        /* extraction failed — caller won't be calling cleanup_tmpdir */
        free(*tmpdir_out);
        *tmpdir_out = NULL;
    }
    return ret;
}

/* --------------------------------------------------------------------------
 * cleanup_tmpdir()
 * Uses nftw() to recursively remove all files and directories.
 * -------------------------------------------------------------------------- */

static int nftw_rm(const char *path,
                   const struct stat *sb,
                   int typeflag,
                   struct FTW *ftwbuf)
{
    (void)sb; (void)ftwbuf;
    if (typeflag == FTW_F || typeflag == FTW_SL)
        return unlink(path);
    else
        return rmdir(path);
}

void cleanup_tmpdir(char *tmpdir)
{
    if (!tmpdir) return;
    nftw(tmpdir, nftw_rm, 32, FTW_DEPTH | FTW_PHYS);
    free(tmpdir);
}

#endif /* HAVE_LIBARCHIVE */
