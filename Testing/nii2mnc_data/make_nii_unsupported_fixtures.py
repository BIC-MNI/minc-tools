#!/usr/bin/env python3
"""Generate NIfTI-1 fixtures with datatypes that nii2mnc does not support.

Standard library only. Each file is a 2x2x2 volume with a clean axis-aligned
RAS s-form:

    nii_dt_complex64.nii   DT_COMPLEX64 (32), bitpix 64
    nii_dt_int64.nii       DT_INT64 (1024),   bitpix 64

    python3 make_nii_unsupported_fixtures.py
"""
import os
import struct

HERE = os.path.dirname(os.path.abspath(__file__))


def write(name, datatype, bitpix, data):
    hdr = bytearray(348)
    struct.pack_into('<i', hdr, 0, 348)
    struct.pack_into('<8h', hdr, 40, 3, 2, 2, 2, 1, 1, 1, 1)
    struct.pack_into('<h', hdr, 70, datatype)
    struct.pack_into('<h', hdr, 72, bitpix)
    struct.pack_into('<8f', hdr, 76, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0)
    struct.pack_into('<f', hdr, 108, 352.0)
    struct.pack_into('<f', hdr, 112, 1.0)               # scl_slope
    struct.pack_into('<b', hdr, 123, 2)                 # mm
    struct.pack_into('<h', hdr, 254, 1)                 # sform_code
    struct.pack_into('<4f', hdr, 280, 1.0, 0.0, 0.0, 0.0)
    struct.pack_into('<4f', hdr, 296, 0.0, 1.0, 0.0, 0.0)
    struct.pack_into('<4f', hdr, 312, 0.0, 0.0, 1.0, 0.0)
    struct.pack_into('<4s', hdr, 344, b'n+1\x00')
    with open(os.path.join(HERE, name), 'wb') as f:
        f.write(hdr)
        f.write(b'\x00\x00\x00\x00')
        f.write(data)
    print("wrote", name)


def main():
    write('nii_dt_complex64.nii', 32, 64,
          struct.pack('<16f', *[float(i) for i in range(16)]))
    write('nii_dt_int64.nii', 1024, 64, struct.pack('<8q', *range(8)))


if __name__ == '__main__':
    main()
