#!/usr/bin/env python3
"""Generate a NIfTI-1 fixture with dim[6] > 1 for the nii2mnc tests.

Standard library only. Writes nii_dim6.nii: float32, dim = 6 2 2 2 1 1 2
(two blocks along dim[6]), values 0..15, clean axis-aligned RAS s-form.

    python3 make_nii_dim6_fixture.py
"""
import os
import struct

HERE = os.path.dirname(os.path.abspath(__file__))
DT_FLOAT32 = 16


def main():
    hdr = bytearray(348)
    struct.pack_into('<i', hdr, 0, 348)
    struct.pack_into('<8h', hdr, 40, 6, 2, 2, 2, 1, 1, 2, 1)
    struct.pack_into('<h', hdr, 70, DT_FLOAT32)
    struct.pack_into('<h', hdr, 72, 32)
    struct.pack_into('<8f', hdr, 76, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0)
    struct.pack_into('<f', hdr, 108, 352.0)
    struct.pack_into('<f', hdr, 112, 0.0)               # scl_slope: unused
    struct.pack_into('<b', hdr, 123, 2)                 # mm
    struct.pack_into('<h', hdr, 254, 1)                 # sform_code
    struct.pack_into('<4f', hdr, 280, 1.0, 0.0, 0.0, 0.0)
    struct.pack_into('<4f', hdr, 296, 0.0, 1.0, 0.0, 0.0)
    struct.pack_into('<4f', hdr, 312, 0.0, 0.0, 1.0, 0.0)
    struct.pack_into('<4s', hdr, 344, b'n+1\x00')
    path = os.path.join(HERE, 'nii_dim6.nii')
    with open(path, 'wb') as f:
        f.write(hdr)
        f.write(b'\x00\x00\x00\x00')
        f.write(struct.pack('<16f', *[float(i) for i in range(16)]))
    print("wrote", os.path.relpath(path, HERE))


if __name__ == '__main__':
    main()
