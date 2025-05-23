#pragma once

/*
Efficient transform classes for floating point and alpha16 coordinate systems.
Note that component transforms are used because it's easy to skip calculations
for components that are not used. For example, if the rotation is 0 then no
expensive trig functions are needed. Same with scale and offset.

*/

#include "fl/lut.h"
#include "fl/ptr.h"
#include "fl/xymap.h"
#include "lib8tion/types.h"
#include "fl/math_macros.h"

namespace fl {

FASTLED_SMART_PTR(TransformFloatImpl);

using alpha16 =
    uint16_t; // fixed point representation of 0->1 in the range [0, 65535]

// This transform assumes the coordinates are in the range [0,65535].
struct Transform16 {
    // Make a transform that maps a rectangle to the given bounds from
    // (0,0) to (max_value,max_value), inclusive.
    static Transform16 ToBounds(alpha16 max_value);
    static Transform16 ToBounds(const point_xy<alpha16> &min,
                                const point_xy<alpha16> &max,
                                alpha16 rotation = 0);

    static Transform16 From(uint16_t width, uint16_t height) {
        point_xy<alpha16> min = point_xy<alpha16>(0, 0);
        point_xy<alpha16> max = point_xy<alpha16>(width, height);
        return Transform16::ToBounds(min, max);
    }

    // static Transform16 From(const XYMap &map) {
    //     return Transform16::ToBounds(map.getWidth(), map.getHeight());
    // }
    Transform16() = default;
    alpha16 scale_x = 0xffff;
    alpha16 scale_y = 0xffff;
    alpha16 offset_x = 0;
    alpha16 offset_y = 0;
    alpha16 rotation = 0;

    point_xy<alpha16> transform(const point_xy<alpha16> &xy) const;
};

// This transform assumes the coordinates are in the range [0,1].
struct TransformFloatImpl : public Referent {
    static TransformFloatImplPtr Identity() {
        TransformFloatImplPtr tx = TransformFloatImplPtr::New();
        return tx;
    }
    TransformFloatImpl() = default;
    float scale_x = 1.0f;
    float scale_y = 1.0f;
    float offset_x = 0.0f;
    float offset_y = 0.0f;
    float rotation = 0.0f; // rotation range is [0,1], not [0,2*PI]!
    float scale() const;
    void set_scale(float scale);
    point_xy_float transform(const point_xy_float &xy) const;
    bool is_identity() const;
};

// TransformFloat is a wrapper around the smart ptr. This version allows for
// easy use and fast / well behaved copy.
struct TransformFloat {
    TransformFloat() = default;
    float scale_x() const { return mImpl->scale_x; }
    float scale_y() const { return mImpl->scale_y; }
    float offset_x() const { return mImpl->offset_x; }
    float offset_y() const { return mImpl->offset_y; }
    // rotation range is [0,1], not [0,2*PI]!
    float rotation() const { return mImpl->rotation; }
    float scale() const {
        return MIN(scale_x(), scale_y());
    }
    void set_scale(float scale) {
        mImpl->set_scale(scale);
    }
    void set_scale_x(float scale) {
        mImpl->scale_x = scale;
    }
    void set_scale_y(float scale) {
        mImpl->scale_y = scale;
    }
    void set_offset_x(float offset) {
        mImpl->offset_x = offset;
    }
    void set_offset_y(float offset) {
        mImpl->offset_y = offset;
    }
    void set_rotation(float rotation) {
        mImpl->rotation = rotation;
    }
    point_xy_float transform(const point_xy_float &xy) const { return mImpl->transform(xy); }
    bool is_identity() const { return mImpl->is_identity(); }

 private:
    TransformFloatImplPtr mImpl = TransformFloatImpl::Identity();
};


} // namespace fl