#pragma once

#include <memory>

namespace Common {
	/// <summary>
	/// Class that all models will inherit from for serialization
	/// </summary>
	class Serializable
	{
	public:
		/// <summary>
		/// After this method, delete/free should be called!
		/// </summary>
		/// <returns></returns>
		virtual char* Serialize() = 0;
		virtual size_t BufferSize() = 0;
	};
}

