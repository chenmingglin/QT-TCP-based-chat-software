#pragma once
#include <qstring.h>
class User
{
public:
	User() : m_id(0), m_name(""),m_psd(""), m_ip("") {}
	User(int id, QString name) : m_id(id), m_name(name),m_psd(""), m_ip("") {}
	User(const User &other) : m_id(other.m_id), m_name(other.m_name), m_psd(other.m_psd), m_ip(other.m_ip) {}
	User& operator=(const User& other) { m_id = other.m_id; m_name = other.m_name; m_psd = other.m_psd; m_ip = other.m_ip; return *this; };

	int id() const { return m_id; }
	QString name() const{ return m_name; }
	QString psd() const { return m_psd; }
	QString ip() const { return m_ip; }
	void set_id(int id) { m_id = id; }
	void set_name(const QString& name) { m_name = name; }
	void set_psd(const QString& psd) { m_psd = psd; }
	void set_ip(const QString& ip) { m_ip = ip; }
private:
	int m_id;
	QString m_name;
	QString m_psd;
	QString m_ip;
};
