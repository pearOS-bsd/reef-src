// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// SceneKit overlay, the symbols apps import (GarageBand, Keynote, Pages, Rotato). Swift calling convention on x86-64: floating point arguments in
// xmm0.., integer ones in rdi, rsi, rdx; SCNVector3 and SCNVector4 (CGFloat = Double) are returned in xmm0..xmm3.
// The vector initialisers are the conversions; what needs objects (bridging, geometry sources, bounding volumes, matrices) returns zero for now.
.text
.macro SYM name
.globl \name
\name:
.endm

// SCNVector3.init(_: CGFloat, _: CGFloat, _: CGFloat) and init(_: Double, _: Double, _: Double): the values are already the result
SYM _$sSo10SCNVector3V8SceneKitEyAB12CoreGraphics7CGFloatV_A2FtcfC
SYM _$sSo10SCNVector3V8SceneKitEyABSd_S2dtcfC
SYM _$sSo10SCNVector4V8SceneKitEyAB12CoreGraphics7CGFloatV_A3FtcfC
    ret
// init(_: Float, _: Float, _: Float)
SYM _$sSo10SCNVector3V8SceneKitEyABSf_S2ftcfC
    cvtss2sd %xmm0, %xmm0
    cvtss2sd %xmm1, %xmm1
    cvtss2sd %xmm2, %xmm2
    ret
// init(_: Int, _: Int, _: Int)
SYM _$sSo10SCNVector3V8SceneKitEyABSi_S2itcfC
    cvtsi2sdq %rdi, %xmm0
    cvtsi2sdq %rsi, %xmm1
    cvtsi2sdq %rdx, %xmm2
    ret
// init(_: SIMD3<Float>)
SYM _$sSo10SCNVector3V8SceneKitEyABs5SIMD3VySfGcfC
    movaps %xmm0, %xmm1
    shufps $0x55, %xmm1, %xmm1
    movaps %xmm0, %xmm2
    shufps $0xAA, %xmm2, %xmm2
    cvtss2sd %xmm0, %xmm0
    cvtss2sd %xmm1, %xmm1
    cvtss2sd %xmm2, %xmm2
    ret
// the rest: zero
SYM _$sSo10SCNVector3V8SceneKitE19_bridgeToObjectiveCSo7NSValueCyF
SYM _$sSo13CATransform3DV8SceneKitEyABSo13simd_float4x4acfC
SYM _$sSo13simd_float4x4a8SceneKitEyABSo13CATransform3DVcfC
SYM _$sSo17SCNBoundingVolumeP8SceneKitE11boundingBoxSo10SCNVector3V3min_AF3maxtvg
SYM _$sSo17SCNBoundingVolumeP8SceneKitE14boundingSphereSo10SCNVector3V6center_Sf6radiustvg
SYM _$sSo17SCNGeometrySourceC8SceneKitE18textureCoordinatesABSaySo7CGPointVG_tcfC
SYM _$sSo17SCNGeometrySourceC8SceneKitE7normalsABSaySo10SCNVector3VG_tcfC
SYM _$sSo17SCNGeometrySourceC8SceneKitE8verticesABSaySo10SCNVector3VG_tcfC
SYM _$sSo18SCNGeometryElementC8SceneKitE7indices13primitiveTypeABSayxG_So0a9PrimitiveG0Vtcs17FixedWidthIntegerRzlufC
    xorl %eax, %eax
    xorps %xmm0, %xmm0
    xorps %xmm1, %xmm1
    xorps %xmm2, %xmm2
    xorps %xmm3, %xmm3
    ret
