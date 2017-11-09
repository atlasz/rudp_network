#include "Server.h"
#include <logging.h>

Server::Server()
{
	m_svrSocket = new Socket(8888, SOCKET_TYPE_IPV4);
}

Server::~Server()
{
   delete m_svrSocket;
}

// void Server::SendPackets( double time )
// {

// }

int Server::ReceivePackets()
{
	uint8_t msg[kMaxPkgLength];

    while (true) {
        bool busy = false;
        Address from;
        int n = m_svrSocket->ReceivePacket(from, msg, sizeof(msg));
        if(n > 0)
        {
    		 busy = true;

            uint8_t cmd = msg[0];
            DEBUG("client: (%d)|recv: %d|len: %d", from.GetAddress4(), cmd, n);
            switch (cmd) 
            {
                case RUDP_CMD_CONNECTION_START_REQ:
                    // ProcessStart(*socket, from, msg, n);
                	DEBUG("RUDP_CMD_CONNECTION_START_REQ");
                	ProcessStart(from, msg, n);
                    break;
                case RUDP_CMD_CONNECTION_STOP_REQ:
                    // ProcessStop(*socket, from, msg, n);
                    break;
                // case RUDP_CMD_CONN_ACK:
                //      ProcessConnAck(*socket, from, msg, n);
                //     break;
                case RUDP_CMD_HEARTBEAT:
                    // ProcessHeartbeat(*socket, from, msg, n);
                    break;
                case RUDP_CMD_DATA:
                    // ProcessDataNew(*socket, from, msg, n);
                    break;
                default:
                    ERROR("unknown cmd: %u", cmd);
                    break;
            }
        }

        if (!busy) {
            break;
        }
    }

    return 0;
}

void Server::Tick()
{
	ReceivePackets();
}

void Server::ProcessStart(const Address& from,uint8_t* buffer, int len)
{
	ConnectionStartReq startReq;
    ReadStream readStream( buffer, len );
    startReq.SerializeRead( readStream );

    DEBUG("ProcessStart: uid %llud sid %d ", startReq.uid, startReq.sid);

    bool need_create = true;
    auto* s = SessionManagerSingleton::get_mutable_instance().GetSessionByUid(startReq.uid);

    if(nullptr != s)
    {
    	//different device

    	//reconnect
    }

    if(nullptr == s || need_create)
    {
    	s = SessionManagerSingleton::get_mutable_instance().ProduceSession(startReq.uid);
    	if(nullptr == s)
    	{
    		ERROR("Produce session failed");
    	}
    }

    //set session data
    s->set_peer(from);
    s->Start();

    int BufferSize = 512;
    uint8_t data[BufferSize];

    ConnectionStartRsp rsp;
    rsp.cmd = RUDP_CMD_CONNECTION_START_RSP;
    rsp.sid = s->sid();

    WriteStream writeStream( data, BufferSize );

    rsp.SerializeWrite( writeStream );
 	writeStream.Flush();
 	
    m_svrSocket->SendPacket(s->peer(), writeStream.GetData(), writeStream.GetBytesProcessed());

   
}
