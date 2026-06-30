#!/usr/bin/env python3
"""Generate small NIfTI-1 fixtures for the nii2mnc tests.

Dependency-free (numpy + struct only) so it runs without nibabel. The emitted
.nii files are committed and consumed by the nii2mnc test scripts; rerun this
script if you need to regenerate them:

    python3 make_nii_fixtures.py

Two families of fixtures are produced:

1. Dimension-naming (nii2mnc-dimnames-test.sh): exercise the mapping from NIfTI
   file axes to MINC spatial axes (xspace/yspace/zspace). The "zeroslice"
   fixtures reproduce the real-world bug where dcm2niix could not determine the
   slice direction and wrote an s-form whose 3rd column (the k / slice
   direction) is all zeros: the naive per-axis argmax then assigns "xspace"
   twice and drops "zspace".

2. Data-type / intensity (nii2mnc-intensity-test.sh): one clean axis-aligned
   volume per NIfTI voxel datatype, plus one scaled fixture, all filled with a
   known integer ramp so the real min/max/sum are predictable. These verify
   that each datatype is read correctly and intensities (including
   scl_slope/scl_inter scaling) are preserved.
"""
import os
import struct
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))

# NIfTI-1 datatype codes (see nifti1.h).
DT_UINT8, DT_INT16, DT_INT32 = 2, 4, 8
DT_FLOAT32, DT_FLOAT64 = 16, 64
DT_INT8, DT_UINT16, DT_UINT32 = 256, 512, 768

# datatype code -> (numpy dtype, bitpix)
_DT = {
    DT_UINT8:   (np.uint8,   8),
    DT_INT8:    (np.int8,    8),
    DT_INT16:   (np.int16,   16),
    DT_UINT16:  (np.uint16,  16),
    DT_INT32:   (np.int32,   32),
    DT_UINT32:  (np.uint32,  32),
    DT_FLOAT32: (np.float32, 32),
    DT_FLOAT64: (np.float64, 64),
}


def write_nii(path, data, srows, pixdim_spatial, dt_time=1.0,
              datatype=DT_FLOAT32, scl_slope=1.0, scl_inter=0.0):
    """data: ndarray (x,y,z) or (x,y,z,t).
       srows: 3x4 s-form rows (srow_x, srow_y, srow_z).
       pixdim_spatial: (dx,dy,dz).
       datatype: NIfTI-1 datatype code; data is cast to the matching numpy type.
       scl_slope/scl_inter: stored on disk so real = stored*slope + inter."""
    np_dtype, bitpix = _DT[datatype]
    data = np.asarray(data, dtype=np_dtype)
    ndim = data.ndim
    nx, ny, nz = data.shape[0], data.shape[1], data.shape[2]
    nt = data.shape[3] if ndim == 4 else 1

    hdr = bytearray(348)
    struct.pack_into('<i', hdr, 0, 348)                 # sizeof_hdr
    dim = [ndim, nx, ny, nz, nt, 1, 1, 1]
    struct.pack_into('<8h', hdr, 40, *dim)              # dim[8]
    struct.pack_into('<h', hdr, 70, datatype)           # datatype
    struct.pack_into('<h', hdr, 72, bitpix)             # bitpix
    dx, dy, dz = pixdim_spatial
    pixdim = [1.0, dx, dy, dz, dt_time, 0.0, 0.0, 0.0]  # pixdim[0]=qfac
    struct.pack_into('<8f', hdr, 76, *pixdim)
    struct.pack_into('<f', hdr, 108, 352.0)            # vox_offset
    struct.pack_into('<f', hdr, 112, scl_slope)        # scl_slope
    struct.pack_into('<f', hdr, 116, scl_inter)        # scl_inter
    struct.pack_into('<b', hdr, 123, 2 | 8)            # xyzt_units: mm | sec
    struct.pack_into('<h', hdr, 252, 0)                # qform_code = UNKNOWN
    struct.pack_into('<h', hdr, 254, 1)                # sform_code = SCANNER_ANAT
    struct.pack_into('<4f', hdr, 280, *srows[0])       # srow_x
    struct.pack_into('<4f', hdr, 296, *srows[1])       # srow_y
    struct.pack_into('<4f', hdr, 312, *srows[2])       # srow_z
    struct.pack_into('<4s', hdr, 344, b'n+1\x00')      # magic

    # NIfTI on-disk order is column-major over (x,y,z,t): x fastest.
    raw = np.asfortranarray(data).tobytes(order='F')
    with open(path, 'wb') as f:
        f.write(hdr)
        f.write(b'\x00\x00\x00\x00')                   # extension flags
        f.write(raw)
    print("wrote", os.path.relpath(path, HERE), data.shape, "dt", datatype)


def ramp(shape):
    return np.arange(np.prod(shape), dtype=np.float32).reshape(shape, order='F')


# Clean axis-aligned RAS s-form shared by the data-type fixtures.
_RAS_SROWS = [(2.0, 0.0, 0.0, -10.0),
              (0.0, 3.0, 0.0, -20.0),
              (0.0, 0.0, 4.0, -30.0)]
_RAS_PIXDIM = (2.0, 3.0, 4.0)


def main():
    nx, ny, nz, nt = 4, 5, 6, 3

    # ---- Family 1: dimension-naming fixtures (FLOAT32, slope 1) -------------

    # 1) Clean axis-aligned 3D RAS volume: file x->x, y->y, z->z.
    write_nii(os.path.join(HERE, 'nii_3d_ras.nii'),
              ramp((nx, ny, nz)), srows=_RAS_SROWS, pixdim_spatial=_RAS_PIXDIM)

    # 2) Clean permuted/oblique 3D: file x->z, y->x, z->y (still a bijection).
    #    Guards that the fix is a no-op for valid non-identity orientations.
    write_nii(os.path.join(HERE, 'nii_3d_permuted.nii'),
              ramp((nx, ny, nz)),
              srows=[(0.0, 2.0, 0.0, -10.0),   # row x: dominant in file-axis 1
                     (0.0, 0.0, 3.0, -20.0),   # row y: dominant in file-axis 2
                     (4.0, 0.0, 0.0, -30.0)],  # row z: dominant in file-axis 0
              pixdim_spatial=(4.0, 2.0, 3.0))

    # 3) Clean 4D (time) volume.
    write_nii(os.path.join(HERE, 'nii_4d_normal.nii'),
              ramp((nx, ny, nz, nt)), srows=_RAS_SROWS, pixdim_spatial=_RAS_PIXDIM)

    # 4) BUG TRIGGER, 3D: s-form 3rd column (slice direction) all zeros.
    write_nii(os.path.join(HERE, 'nii_3d_zeroslice.nii'),
              ramp((nx, ny, nz)),
              srows=[(2.0, 0.0, 0.0, -10.0),
                     (0.0, 3.0, 0.0, -20.0),
                     (0.0, 0.0, 0.0, -30.0)],  # column k = [0,0,0]
              pixdim_spatial=_RAS_PIXDIM)

    # 5) BUG TRIGGER, 4D: same degenerate slice column, with time.
    write_nii(os.path.join(HERE, 'nii_4d_zeroslice.nii'),
              ramp((nx, ny, nz, nt)),
              srows=[(2.0, 0.0, 0.0, -10.0),
                     (0.0, 3.0, 0.0, -20.0),
                     (0.0, 0.0, 0.0, -30.0)],  # column k = [0,0,0]
              pixdim_spatial=_RAS_PIXDIM)

    # ---- Family 2: data-type / intensity fixtures --------------------------
    #
    # All use the clean RAS geometry above with 120 voxels (4x5x6) holding the
    # ramp v = i % 5, so for every datatype:
    #     min = 0, max = 4, sum = 24 cycles * (0+1+2+3+4) = 240.
    # Expected (real) min/max/sum consumed by nii2mnc-intensity-test.sh:
    #     nii_dt_*        -> min 0  max 4   sum 240
    #     nii_scl_int16   -> real = stored*2 - 10:
    #                        min -10  max -2  sum 2*240 - 10*120 = -720
    flat = (np.arange(nx * ny * nz) % 5)
    base = flat.reshape((nx, ny, nz), order='F')

    for name, code in [('nii_dt_uint8',  DT_UINT8),
                       ('nii_dt_int8',   DT_INT8),
                       ('nii_dt_int16',  DT_INT16),
                       ('nii_dt_uint16', DT_UINT16),
                       ('nii_dt_int32',  DT_INT32),
                       ('nii_dt_float32', DT_FLOAT32),
                       ('nii_dt_float64', DT_FLOAT64)]:
        write_nii(os.path.join(HERE, name + '.nii'),
                  base, srows=_RAS_SROWS, pixdim_spatial=_RAS_PIXDIM,
                  datatype=code)

    # Scaled INT16: stored ramp, real = stored*2 - 10.
    write_nii(os.path.join(HERE, 'nii_scl_int16.nii'),
              base, srows=_RAS_SROWS, pixdim_spatial=_RAS_PIXDIM,
              datatype=DT_INT16, scl_slope=2.0, scl_inter=-10.0)


if __name__ == '__main__':
    main()
