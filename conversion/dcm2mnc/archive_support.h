#ifndef ARCHIVE_SUPPORT_H
#define ARCHIVE_SUPPORT_H

#ifdef HAVE_LIBARCHIVE

/* Returns 1 if path ends with .zip / .tar.gz / .tar.bz2 / .tgz (case-insensitive),
 * 0 otherwise. */
int is_archive_file(const char *path);

/*
 * Extract all files from archive_path into a freshly created temp directory.
 * On success: *tmpdir_out is set to a malloc'd path to the temp dir (caller must
 *   eventually call cleanup_tmpdir()), returns 0.
 * On failure: prints an error to stderr, returns non-zero, *tmpdir_out is undefined.
 */
int extract_archive_to_tmpdir(const char *archive_path, char **tmpdir_out);

/* Recursively remove tmpdir and free the pointer. */
void cleanup_tmpdir(char *tmpdir);

#endif /* HAVE_LIBARCHIVE */

#endif /* ARCHIVE_SUPPORT_H */
