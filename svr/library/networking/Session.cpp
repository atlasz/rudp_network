#include "Session.h"

Session::Session()
{

}

Session::~Session()
{

}

int Session::Start()
{
	return 0;
}

void Session::Stop()
{

}

void Session::Timeout()
{

}




int SessionManager::sid_gen_ = 0;
void SessionManager::Init()
{

}

Session* SessionManager::ProduceSession(uint64_t uid)
{
	auto* curSession = GetSessionByUid(uid);
	if(nullptr != curSession)
	{
		//stop session
		DestroySession(curSession->sid());
	}

	auto* s = new Session();
	s->set_sid(++sid_gen_);
	s->set_uid(uid);

	m_session_map[s->sid()] = s;
	m_uid_sid_map[uid] = s->sid();
	return s;
}

void SessionManager::DestroySession(uint16_t sid)
{
	auto it = m_session_map.find(sid);
    if (it == m_session_map.end()) {
        return;
    }

    m_uid_sid_map.erase(it->second->uid());

    delete it->second;
    m_session_map.erase(it);
}

Session* SessionManager::GetSession(uint16_t sid)
{
	auto it = m_session_map.find(sid);
    if (it == m_session_map.end()) {
        return nullptr;
    }

    return it->second;
}

Session* SessionManager::GetSessionByUid(uint64_t uid)
{
	auto it = m_uid_sid_map.find(uid);
    if (it == m_uid_sid_map.end()) {
        return nullptr;
    }

    return GetSession(it->second);

}