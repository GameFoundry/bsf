#include "CmUUID.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

using namespace boost::uuids;

namespace CamelotEngine
{
	String UUIDGenerator::generateRandom()
	{
		static random_generator gen;

		uuid newUUID = gen();

		return to_string(newUUID);
	}
};