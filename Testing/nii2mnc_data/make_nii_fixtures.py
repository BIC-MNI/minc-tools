#!/usr/bin/env python3
"""Generate small NIfTI-1 fixtures for the nii2mnc dimension-naming tests.

Dependency-free (numpy + struct only) so it runs without nibabel. The emitted
.nii files are committed and consumed by nii2mnc-dimnames-test.sh; rerun this
script if you need to regenerate them:

    python3 make_nii_fixtures.py

Each fixture exercises the mapping from NIfTI file axes to MINC spatial axes
(xspace/yspace/zspace) in nii2mnc. The "zeroslice" fixtures reproduce the
real-world bug where dcm2niix could not determine the slice direction and wrote
an s-form whose 3rd column (the k / slice direction) is all zeros: the naive
per-axis argmax then assigns "xspace" twice and drops "zspace".
"""
import os
import struct
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))

# NIfTI-1 single-file (.nii) header field offsets we populate.
def write_nii(path, data, srows, pixdim_spatial, dt=1.0):
    """data: ndarray (x,y,z) or (x,y,z,t), float32.
       srows: 3x4 s-form rows (srow_x, srow_y, srow_z).
       pixdim_spatial: (dx,dy,dz)."""
    data = np.asarray(data, dtype=np.float32)
    ndim = data.ndim
    nx, ny, nz = data.shape[0], data.shape[1], data.shape[2]
    nt = data.shape[3] if ndim == 4 else 1

    hdr = bytearray(348)
    struct.pack_into('<i', hdr, 0, 348)                 # sizeof_hdr
    dim = [ndim, nx, ny, nz, nt, 1, 1, 1]
    struct.pack_into('<8h', hdr, 40, *dim)              # dim[8]
    struct.pack_into('<h', hdr, 70, 16)                 # datatype = FLOAT32
    struct.pack_into('<h', hdr, 72, 32)                 # bitpix
    dx, dy, dz = pixdim_spatial
    pixdim = [1.0, dx, dy, dz, dt, 0.0, 0.0, 0.0]       # pixdim[0]=qfac
    struct.pack_into('<8f', hdr, 76, *pixdim)
    struct.pack_into('<f', hdr, 108, 352.0)            # vox_offset
    struct.pack_into('<f', hdr, 112, 1.0)              # scl_slope
    struct.pack_into('<f', hdr, 116, 0.0)              # scl_inter
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
    print("wrote", os.path.relpath(path, HERE), data.shape)


def ramp(shape):
    return np.arange(np.prod(shape), dtype=np.float32).reshape(shape, order='F')


def main():
    nx, ny, nz, nt = 4, 5, 6, 3

    # 1) Clean axis-aligned 3D RAS volume: file x->x, y->y, z->z.
    write_nii(os.path.join(HERE, 'nii_3d_ras.nii'),
              ramp((nx, ny, nz)),
              srows=[(2.0, 0.0, 0.0, -10.0),
                     (0.0, 3.0, 0.0, -20.0),
                     (0.0, 0.0, 4.0, -30.0)],
              pixdim_spatial=(2.0, 3.0, 4.0))

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
              ramp((nx, ny, nz, nt)),
              srows=[(2.0, 0.0, 0.0, -10.0),
                     (0.0, 3.0, 0.0, -20.0),
                     (0.0, 0.0, 4.0, -30.0)],
              pixdim_spatial=(2.0, 3.0, 4.0))

    # 4) BUG TRIGGER, 3D: s-form 3rd column (slice direction) all zeros.
    write_nii(os.path.join(HERE, 'nii_3d_zeroslice.nii'),
              ramp((nx, ny, nz)),
              srows=[(2.0, 0.0, 0.0, -10.0),
                     (0.0, 3.0, 0.0, -20.0),
                     (0.0, 0.0, 0.0, -30.0)],  # column k = [0,0,0]
              pixdim_spatial=(2.0, 3.0, 4.0))

    # 5) BUG TRIGGER, 4D: same degenerate slice column, with time.
    write_nii(os.path.join(HERE, 'nii_4d_zeroslice.nii'),
              ramp((nx, ny, nz, nt)),
              srows=[(2.0, 0.0, 0.0, -10.0),
                     (0.0, 3.0, 0.0, -20.0),
                     (0.0, 0.0, 0.0, -30.0)],  # column k = [0,0,0]
              pixdim_spatial=(2.0, 3.0, 4.0))


if __name__ == '__main__':
    main()
