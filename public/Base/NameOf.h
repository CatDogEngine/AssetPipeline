#pragma once

#include <nameof.hpp>

#include <optional>

namespace nameof
{

template <typename E>
[[nodiscard]] constexpr auto enum_count() noexcept
{
	return nameof::detail::count_v<std::decay_t<E>>;
}

template<class E>
[[nodiscard]] inline std::optional<E> enumof_name(const char* name) noexcept
{
	using D = std::decay_t<E>;
	static_assert(detail::nameof_enum_supported<D>::value, "nameof unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
	static_assert(detail::count_v<D> > 0, "nameof requires enum implementation and valid max and min.");

	for (std::size_t i = 0; i < detail::count_v<D>; ++i)
	{
		if (detail::names_v<D>[i].compare(name))
		{
			return detail::enum_value<D>(i);
		}
	}

	return std::nullopt;
}

}