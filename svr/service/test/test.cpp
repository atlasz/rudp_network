#include <iostream>
#include <Address.h>
#include <Socket.h>
#include <PacketFactory.h>

using namespace std;

int main()
{
	cout << " NOT Debug " << endl;

	#ifdef DEBUG
	cout << " Debug " << endl;
	#endif

	#ifdef SELF
	cout << " Self " << endl;
	#endif

    Address address;

    cout << "is valid: " << address.IsValid() << endl;

    Socket socket(8088, SOCKET_TYPE_IPV4);


    cout << "Socket is Error: " << socket.IsError() << endl;
    PacketFactorySingleton::get_mutable_instance().SetNumPacketTypes(TEST_PACKET_NUM_TYPES);
    
    int packetType = PacketFactorySingleton::get_mutable_instance().GetNumPacketTypes();

    cout << "PacketFactory GetNumPacketTypes: " << packetType << endl;
	return 0;
}