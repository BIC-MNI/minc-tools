#!/usr/bin/env python3
"""Generate NIfTI-1 fixtures with spectral "time" units for nii2mnc.

Standard library only. Each file is float32, dim = 4 2 2 2 3, pixdim[4] =
2.5, toffset = 100, clean axis-aligned RAS s-form, and xyzt_units = mm | U
where U is one of the spectral codes in nifti1.h:

    nii_units_hz.nii     NIFTI_UNITS_HZ   (32)
    nii_units_ppm.nii    NIFTI_UNITS_PPM  (40)
    nii_units_rads.nii   NIFTI_UNITS_RADS (48)

    python3 make_nii_spectral_fixtures.py
"""
import os
import struct

HERE = os.path.dirname(os.path.abspath(__file__))
DT_FLOAT32 = 16
NIFTI_UNITS_MM = 2


def write(name, time_code):
    hdr = bytearray(348)
    struct.pack_into('<i', hdr, 0, 348)
    struct.pack_into('<8h', hdr, 40, 4, 2, 2, 2, 3, 1, 1, 1)
    struct.pack_into('<h', hdr, 70, DT_FLOAT32)
    struct.pack_into('<h', hdr, 72, 32)
    struct.pack_into('<8f', hdr, 76, 1.0, 1.0, 1.0, 1.0, 2.5, 0.0, 0.0, 0.0)
    struct.pack_into('<f', hdr, 108, 352.0)
    struct.pack_into('<f', hdr, 112, 0.0)               # scl_slope: unused
    struct.pack_into('<B', hdr, 123, NIFTI_UNITS_MM | time_code)
    struct.pack_into('<f', hdr, 136, 100.0)             # toffset
    struct.pack_into('<h', hdr, 254, 1)                 # sform_code
    struct.pack_into('<4f', hdr, 280, 1.0, 0.0, 0.0, 0.0)
    struct.pack_into('<4f', hdr, 296, 0.0, 1.0, 0.0, 0.0)
    struct.pack_into('<4f', hdr, 312, 0.0, 0.0, 1.0, 0.0)
    struct.pack_into('<4s', hdr, 344, b'n+1\x00')
    with open(os.path.join(HERE, name), 'wb') as f:
        f.write(hdr)
        f.write(b'\x00\x00\x00\x00')
        f.write(struct.pack('<24f', *[float(i) for i in range(24)]))
    print("wrote", name)


def main():
    write('nii_units_hz.nii', 32)
    write('nii_units_ppm.nii', 40)
    write('nii_units_rads.nii', 48)


if __name__ == '__main__':
    main()
