#!/usr/bin/env python3
"""Generate float32 NIfTI-1 fixtures with scl_slope/scl_inter for nii2mnc.

Standard library only. Each file is a 4x3x2 float32 volume with a clean
axis-aligned RAS s-form. The real value of a voxel is
stored * scl_slope + scl_inter (NIfTI-1 applies scaling to float data too).

    nii_scl_float32.nii        stored 0..23, slope  2, inter 10 -> 10..56,   sum  792
    nii_scl_float32_neg.nii    stored 0..23, slope -2, inter 10 -> -36..10,  sum -312
    nii_scl_float32_const.nii  stored 3,     slope  2, inter  1 -> 7,        sum  168

    python3 make_nii_float_scl_fixtures.py
"""
import os
import struct

HERE = os.path.dirname(os.path.abspath(__file__))
DT_FLOAT32 = 16


def write(name, values, slope, inter):
    nx, ny, nz = 4, 3, 2
    hdr = bytearray(348)
    struct.pack_into('<i', hdr, 0, 348)
    struct.pack_into('<8h', hdr, 40, 3, nx, ny, nz, 1, 1, 1, 1)
    struct.pack_into('<h', hdr, 70, DT_FLOAT32)
    struct.pack_into('<h', hdr, 72, 32)
    struct.pack_into('<8f', hdr, 76, 1.0, 2.0, 3.0, 4.0, 1.0, 0.0, 0.0, 0.0)
    struct.pack_into('<f', hdr, 108, 352.0)
    struct.pack_into('<f', hdr, 112, slope)
    struct.pack_into('<f', hdr, 116, inter)
    struct.pack_into('<b', hdr, 123, 2)                 # mm
    struct.pack_into('<h', hdr, 254, 1)                 # sform_code
    struct.pack_into('<4f', hdr, 280, 2.0, 0.0, 0.0, -10.0)
    struct.pack_into('<4f', hdr, 296, 0.0, 3.0, 0.0, -20.0)
    struct.pack_into('<4f', hdr, 312, 0.0, 0.0, 4.0, -30.0)
    struct.pack_into('<4s', hdr, 344, b'n+1\x00')
    with open(os.path.join(HERE, name), 'wb') as f:
        f.write(hdr)
        f.write(b'\x00\x00\x00\x00')
        f.write(struct.pack('<%df' % len(values), *values))
    real = [v * slope + inter for v in values]
    print("wrote", name, "min", min(real), "max", max(real), "sum", sum(real))


def main():
    ramp = [float(i) for i in range(24)]
    write('nii_scl_float32.nii', ramp, 2.0, 10.0)
    write('nii_scl_float32_neg.nii', ramp, -2.0, 10.0)
    write('nii_scl_float32_const.nii', [3.0] * 24, 2.0, 1.0)


if __name__ == '__main__':
    main()
