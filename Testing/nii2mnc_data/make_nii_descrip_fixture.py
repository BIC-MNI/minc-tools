#!/usr/bin/env python3
"""Generate a NIfTI-1 fixture with a descrip string for the nii2mnc tests.

Standard library only. Writes nii_descrip.nii: a 2x2x2 float32 volume,
clean axis-aligned RAS s-form, descrip = "TE=34;Time=140149.418;phase=1"
(the kind of text dcm2niix writes), zero padded to 80 bytes.

    python3 make_nii_descrip_fixture.py
"""
import os
import struct

HERE = os.path.dirname(os.path.abspath(__file__))
DT_FLOAT32 = 16
DESCRIP = b'TE=34;Time=140149.418;phase=1'


def main():
    hdr = bytearray(348)
    struct.pack_into('<i', hdr, 0, 348)
    struct.pack_into('<8h', hdr, 40, 3, 2, 2, 2, 1, 1, 1, 1)
    struct.pack_into('<h', hdr, 70, DT_FLOAT32)
    struct.pack_into('<h', hdr, 72, 32)
    struct.pack_into('<8f', hdr, 76, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0)
    struct.pack_into('<f', hdr, 108, 352.0)
    struct.pack_into('<f', hdr, 112, 0.0)               # scl_slope: unused
    struct.pack_into('<b', hdr, 123, 2)                 # mm
    struct.pack_into('<80s', hdr, 148, DESCRIP)
    struct.pack_into('<h', hdr, 254, 1)                 # sform_code
    struct.pack_into('<4f', hdr, 280, 1.0, 0.0, 0.0, 0.0)
    struct.pack_into('<4f', hdr, 296, 0.0, 1.0, 0.0, 0.0)
    struct.pack_into('<4f', hdr, 312, 0.0, 0.0, 1.0, 0.0)
    struct.pack_into('<4s', hdr, 344, b'n+1\x00')
    path = os.path.join(HERE, 'nii_descrip.nii')
    with open(path, 'wb') as f:
        f.write(hdr)
        f.write(b'\x00\x00\x00\x00')
        f.write(struct.pack('<8f', *[float(i) for i in range(8)]))
    print("wrote", os.path.relpath(path, HERE))


if __name__ == '__main__':
    main()
