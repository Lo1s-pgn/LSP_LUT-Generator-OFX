# Vendored OpenFX 1.5.1 + Support (minimal)

Subset required to build **LSP - Simple LUT Generator** via CMake:

- C API headers (`include/ofx*.h`)
- OFX C++ Support headers (`Support/include/ofxs*.h`)
- Eight Support translation units in `Support/Library/` (listed in `cmake/LutGenCommon.cmake`) plus `ofxsSupportPrivate.h`

Upstream: [AcademySoftwareFoundation/openfx](https://github.com/AcademySoftwareFoundation/openfx) tag **OFX_Release_1.5.1**.

Override path with **`-DOFX_SDK_PATH=`** at CMake configure time to use another checkout.
