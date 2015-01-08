#include "NBID.h"

boost::uuids::uuid NBID::s_namespaceBase; // boost initializes to {6ba7b810-9dad-11d1-80b4-00c04fd430c8}
boost::uuids::name_generator NBID::s_uuidGen(NBID::s_namespaceBase);

NBID::NBID(const std::string& uuidGenName)
	: m_boostUUID(s_uuidGen(uuidGenName))
{
}

NBID::~NBID()
{
}