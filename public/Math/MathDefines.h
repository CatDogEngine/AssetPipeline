#pragma once

namespace cdtools
{

constexpr double SmallNumberTolerance = 1.e-8f;

// Define explicit vector types to create vector instances.
// So that we can specialize behaviors for different vector types.
// For example, Point is (0, 0, 0, 1) and Direction is (0, 0, 0, 0) by default.
enum class VectorType : uint8_t
{
	Generic = 0,
	Point,
	Direction,
	Color,
	U8Color,
	UV,
	VertexWeight,
};

}