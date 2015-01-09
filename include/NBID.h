/*
The MIT License (MIT)

Copyright (c) 2015 Tayfun Kayhan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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