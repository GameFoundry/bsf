#pragma once

#include "BsPrerequisitesUtil.h"

namespace BansheeEngine
{
	/**
	 * @brief	Generates a new hash for the provided type using the default standard
	 * 			hasher and combines it with a previous hash.
	 */
	template <class T>
	inline void hash_combine(std::size_t& seed, const T& v)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
	}

	/**
	 * @brief	Generates an MD5 hash string for the provided source string.
	 */
	String md5(const WString& source);

	/**
	 * @brief	Generates an MD5 hash string for the provided source string.
	 */
	String md5(const String& source);
}