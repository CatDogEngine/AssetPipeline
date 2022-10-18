#pragma once

#include "VectorDerived.hpp"

namespace cdtools
{

// Generic vector types.
using Vec2f = VectorDerived<float, 2, VectorType::Generic>;
using Vec3f = VectorDerived<float, 3, VectorType::Generic>;
using Vec4f = VectorDerived<float, 4, VectorType::Generic>;
using Vec2 = VectorDerived<double, 2, VectorType::Generic>;
using Vec3 = VectorDerived<double, 3, VectorType::Generic>;
using Vec4 = VectorDerived<double, 4, VectorType::Generic>;

// More safe specific vector types than using generic types.
using Point = VectorDerived<float, 3, VectorType::Point>;;
using Direction = VectorDerived<float, 3, VectorType::Direction>;
using Color = VectorDerived<float, 4, VectorType::Color>;
using U8Color = VectorDerived<uint8_t, 4, VectorType::U8Color>;
using UV = VectorDerived<float, 2, VectorType::UV>;
using VertexWeight = VectorDerived<float, 4, VectorType::VertexWeight>;

}