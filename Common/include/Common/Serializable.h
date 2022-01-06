#pragma once

#include <memory>

namespace Common {
	/// <summary>
	/// Class that all models will inherit from for serialization
	/// </summary>
	class Serializable
	{
	public:
		virtual size_t BufferSize() const = 0;
	};
}

