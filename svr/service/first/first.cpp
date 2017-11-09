#include <iostream>
#include <PacketFactory.h>
#include <logging.h>
#include <Server.h>

using namespace std;

int main()
{
    Logger::getLogger().setLogLevel(Logger::LDEBUG);

    DEBUG("---first server app start at %ld", time(NULL));

    PacketFactorySingleton::get_mutable_instance().SetNumPacketTypes(TEST_PACKET_NUM_TYPES);
    
    Server server;

    while(true)
    {
        server.Tick();
    }

	return 0;
}
