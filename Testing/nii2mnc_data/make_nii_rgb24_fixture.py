#!/usr/bin/env python3
"""Generate the NIfTI-1 RGB24 fixture for the nii2mnc tests.

Standard library only (make_nii_fixtures.py needs numpy). Writes
nii_dt_rgb24.nii: a 4x3x2 volume, datatype DT_RGB24 (128, bitpix 24), clean
axis-aligned RAS s-form, components interleaved per voxel (RGBRGB..., x
fastest). For voxel index i (column-major): R = i%5, G = i%5 + 5, B = 20,
so over all components min 0, max 20, sum 692.

    python3 make_nii_rgb24_fixture.py
"""
import os
import struct

HERE = os.path.dirname(os.path.abspath(__file__))
DT_RGB24 = 128


def main():
    nx, ny, nz = 4, 3, 2
    hdr = bytearray(348)
    struct.pack_into('<i', hdr, 0, 348)
    struct.pack_into('<8h', hdr, 40, 3, nx, ny, nz, 1, 1, 1, 1)
    struct.pack_into('<h', hdr, 70, DT_RGB24)
    struct.pack_into('<h', hdr, 72, 24)
    struct.pack_into('<8f', hdr, 76, 1.0, 2.0, 3.0, 4.0, 1.0, 0.0, 0.0, 0.0)
    struct.pack_into('<f', hdr, 108, 352.0)
    struct.pack_into('<f', hdr, 112, 0.0)               # scl_slope: unused
    struct.pack_into('<b', hdr, 123, 2 | 8)             # mm | sec
    struct.pack_into('<h', hdr, 252, 0)                 # qform_code
    struct.pack_into('<h', hdr, 254, 1)                 # sform_code
    struct.pack_into('<4f', hdr, 280, 2.0, 0.0, 0.0, -10.0)
    struct.pack_into('<4f', hdr, 296, 0.0, 3.0, 0.0, -20.0)
    struct.pack_into('<4f', hdr, 312, 0.0, 0.0, 4.0, -30.0)
    struct.pack_into('<4s', hdr, 344, b'n+1\x00')

    raw = bytearray()
    total = 0
    for i in range(nx * ny * nz):
        rgb = (i % 5, i % 5 + 5, 20)
        raw += bytes(rgb)
        total += sum(rgb)

    path = os.path.join(HERE, 'nii_dt_rgb24.nii')
    with open(path, 'wb') as f:
        f.write(hdr)
        f.write(b'\x00\x00\x00\x00')
        f.write(raw)
    print("wrote", os.path.relpath(path, HERE), (nx, ny, nz, 3),
          "min", min(raw), "max", max(raw), "sum", total)


if __name__ == '__main__':
    main()
