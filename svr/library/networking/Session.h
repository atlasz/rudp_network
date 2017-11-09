#pragma once
#include <singleton.h>
#include <varmacro.h>
#include <cstdint>
#include <unordered_map>
#include <Address.h>

class Session
{
public:
	Session();
	~Session();

	int Start();
	void Stop();
	void Timeout();
private:
	GETSETVAR(uint16_t, sid, 0);
	GETSETVAR(uint64_t, uid, 0);
	STRUCT_VAR(Address, peer);
};

class SessionManager
{
public:
	void Init();
	Session* ProduceSession(uint64_t uid);
	void DestroySession(uint16_t sid);
	Session* GetSession(uint16_t sid);
	Session* GetSessionByUid(uint64_t uid);
private:
	std::unordered_map<uint16_t, Session*> m_session_map;
	std::unordered_map<uint64_t, uint16_t> m_uid_sid_map;
private:
	static int sid_gen_;
};
typedef Singleton<SessionManager> SessionManagerSingleton;