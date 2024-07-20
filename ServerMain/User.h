#pragma once
#include <qstring.h>
class User
{
public:
	User() : m_id(0), m_name("") {}
	User(int id, QString name) : m_id(id), m_name(name) {}
	User(const User &other) : m_id(other.m_id), m_name(other.m_name) {}
	User& operator=(const User& other) { m_id = other.m_id; m_name = other.m_name; return *this; };

	int id() const { return m_id; }
	QString name() const{ return m_name; }
private:
	int m_id;
	QString m_name;
};
