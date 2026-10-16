#!/usr/bin/env python3
"""Generate single-slice NIfTI-1 fixtures for the nii2mnc tests.

Standard library only. Each file is float32 with a clean axis-aligned RAS
s-form: steps 2, 3, 4 mm, first voxel at (-10, -20, 20) mm, so the slice is
at z = 20 mm.

    nii_slice_3d.nii    dim = 3 4 3 1      (x, y, one z slice)
    nii_slice_2d.nii    dim = 2 4 3        (no z dimension)
    nii_slice_time.nii  dim = 4 4 3 1 3    (one z slice, 3 time points)

    python3 make_nii_slice_fixtures.py
"""
import os
import struct

HERE = os.path.dirname(os.path.abspath(__file__))
DT_FLOAT32 = 16


def write(name, dims):
    full = list(dims) + [1] * (7 - len(dims))
    hdr = bytearray(348)
    struct.pack_into('<i', hdr, 0, 348)
    struct.pack_into('<8h', hdr, 40, len(dims), *full)
    struct.pack_into('<h', hdr, 70, DT_FLOAT32)
    struct.pack_into('<h', hdr, 72, 32)
    struct.pack_into('<8f', hdr, 76, 1.0, 2.0, 3.0, 4.0, 1.5, 0.0, 0.0, 0.0)
    struct.pack_into('<f', hdr, 108, 352.0)
    struct.pack_into('<f', hdr, 112, 0.0)               # scl_slope: unused
    struct.pack_into('<b', hdr, 123, 2 | 8)             # mm | sec
    struct.pack_into('<h', hdr, 254, 1)                 # sform_code
    struct.pack_into('<4f', hdr, 280, 2.0, 0.0, 0.0, -10.0)
    struct.pack_into('<4f', hdr, 296, 0.0, 3.0, 0.0, -20.0)
    struct.pack_into('<4f', hdr, 312, 0.0, 0.0, 4.0, 20.0)
    struct.pack_into('<4s', hdr, 344, b'n+1\x00')
    n = 1
    for d in full:
        n *= d
    with open(os.path.join(HERE, name), 'wb') as f:
        f.write(hdr)
        f.write(b'\x00\x00\x00\x00')
        f.write(struct.pack('<%df' % n, *[float(i) for i in range(n)]))
    print("wrote", name, dims)


def main():
    write('nii_slice_3d.nii', [4, 3, 1])
    write('nii_slice_2d.nii', [4, 3])
    write('nii_slice_time.nii', [4, 3, 1, 3])


if __name__ == '__main__':
    main()
