#!/usr/bin/env python3
"""Generate the NIfTI-1 two-file (.hdr/.img) fixture for the nii2mnc tests.

Standard library only. Writes nii_pair.hdr and nii_pair.img: a 4x3x2 int16
volume (magic "ni1", vox_offset 0), clean axis-aligned RAS s-form, filled
with the ramp v = i (i = 0..23, x fastest), so the sum is 276.

    python3 make_nii_pair_fixture.py
"""
import os
import struct

HERE = os.path.dirname(os.path.abspath(__file__))
DT_INT16 = 4


def main():
    nx, ny, nz = 4, 3, 2
    hdr = bytearray(348)
    struct.pack_into('<i', hdr, 0, 348)
    struct.pack_into('<8h', hdr, 40, 3, nx, ny, nz, 1, 1, 1, 1)
    struct.pack_into('<h', hdr, 70, DT_INT16)
    struct.pack_into('<h', hdr, 72, 16)
    struct.pack_into('<8f', hdr, 76, 1.0, 2.0, 3.0, 4.0, 1.0, 0.0, 0.0, 0.0)
    struct.pack_into('<f', hdr, 108, 0.0)               # vox_offset
    struct.pack_into('<f', hdr, 112, 1.0)               # scl_slope
    struct.pack_into('<b', hdr, 123, 2)                 # mm
    struct.pack_into('<h', hdr, 254, 1)                 # sform_code
    struct.pack_into('<4f', hdr, 280, 2.0, 0.0, 0.0, -10.0)
    struct.pack_into('<4f', hdr, 296, 0.0, 3.0, 0.0, -20.0)
    struct.pack_into('<4f', hdr, 312, 0.0, 0.0, 4.0, -30.0)
    struct.pack_into('<4s', hdr, 344, b'ni1\x00')

    n = nx * ny * nz
    img = struct.pack('<%dh' % n, *range(n))

    with open(os.path.join(HERE, 'nii_pair.hdr'), 'wb') as f:
        f.write(hdr)
    with open(os.path.join(HERE, 'nii_pair.img'), 'wb') as f:
        f.write(img)
    print("wrote nii_pair.hdr/.img", (nx, ny, nz), "sum", sum(range(n)))


if __name__ == '__main__':
    main()
