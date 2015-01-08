#pragma once

#include <string>

#include <boost\uuid\uuid.hpp>
#include <boost\uuid\uuid_io.hpp>
#include <boost\uuid\uuid_generators.hpp>

/*
* Name Based ID generator class which
* uses boost::uuids::uuid as the underlying UUID generator
*/
class NBID
{
public:
	explicit NBID(const std::string& uuidGenName);
	~NBID();

	bool operator<(const NBID& other) const { return (m_boostUUID < other.getBoostUUID()); }

	const boost::uuids::uuid& getBoostUUID() const { return m_boostUUID; }

	/* Returns string representation of UUID */
	std::string toString() const { return to_string(m_boostUUID); }

private:
	NBID(const NBID&);
	NBID(NBID*);
	
	static boost::uuids::uuid s_namespaceBase;
	static boost::uuids::name_generator s_uuidGen;

	const boost::uuids::uuid m_boostUUID;
};