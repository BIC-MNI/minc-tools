#!/usr/bin/env python3
"""Generate the NIfTI-1 vector (dim[5]) fixture for the nii2mnc tests.

Standard library only. Writes nii_vector.nii: a 4x3x2 volume with a
3-component vector at each voxel (dim = 5 4 3 2 1 3, float32,
intent_code NIFTI_INTENT_VECTOR, clean axis-aligned RAS s-form). This is the
layout of an ITK/ANTs displacement field.

NIfTI stores dim[5] slowest: all voxels of component 0, then component 1,
then component 2. Component c of voxel n (x fastest) holds 100*c + n.

    python3 make_nii_vector_fixture.py
"""
import os
import struct

HERE = os.path.dirname(os.path.abspath(__file__))
DT_FLOAT32 = 16
NIFTI_INTENT_VECTOR = 1007


def main():
    nx, ny, nz, nc = 4, 3, 2, 3
    hdr = bytearray(348)
    struct.pack_into('<i', hdr, 0, 348)
    struct.pack_into('<8h', hdr, 40, 5, nx, ny, nz, 1, nc, 1, 1)
    struct.pack_into('<h', hdr, 68, NIFTI_INTENT_VECTOR)
    struct.pack_into('<h', hdr, 70, DT_FLOAT32)
    struct.pack_into('<h', hdr, 72, 32)
    struct.pack_into('<8f', hdr, 76, 1.0, 2.0, 3.0, 4.0, 1.0, 1.0, 1.0, 1.0)
    struct.pack_into('<f', hdr, 108, 352.0)
    struct.pack_into('<f', hdr, 112, 0.0)               # scl_slope: unused
    struct.pack_into('<b', hdr, 123, 2)                 # mm
    struct.pack_into('<h', hdr, 254, 1)                 # sform_code
    struct.pack_into('<4f', hdr, 280, 2.0, 0.0, 0.0, -10.0)
    struct.pack_into('<4f', hdr, 296, 0.0, 3.0, 0.0, -20.0)
    struct.pack_into('<4f', hdr, 312, 0.0, 0.0, 4.0, -30.0)
    struct.pack_into('<4s', hdr, 344, b'n+1\x00')

    n = nx * ny * nz
    values = [100.0 * c + v for c in range(nc) for v in range(n)]
    path = os.path.join(HERE, 'nii_vector.nii')
    with open(path, 'wb') as f:
        f.write(hdr)
        f.write(b'\x00\x00\x00\x00')
        f.write(struct.pack('<%df' % len(values), *values))
    print("wrote", os.path.relpath(path, HERE), (nx, ny, nz, 1, nc))


if __name__ == '__main__':
    main()
