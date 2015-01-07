/**
 * \file
 *
 * \date   25 Nov 2014
 * \author Prem Shankar Kumar (\@meprem)
 */

#include "zap_request.hpp"
#include "message.hpp"
#include "socket.hpp"
#include "z85.hpp"

#if (ZMQ_VERSION_MAJOR > 3)

namespace zmqpp
{

/*!
 * Receive a ZAP valid request from the handler socket
 */
zap_request::zap_request(socket& handler, bool logging) :
  zap_socket(handler),
  verbose(logging)
{
    message msg;
    zap_socket.receive(msg);

    if(0 == msg.parts())
        return;     // Interrupted

    // Get all standard frames off the handler socket
    version   = msg.get(0);
    sequence  = msg.get(1);
    domain    = msg.get(2);
    address   = msg.get(3);
    identity  = msg.get(4);
    mechanism = msg.get(5); 

    // If the version is wrong, we're linked with a bogus libzmq, so die
    assert(version == "1.0");

    // Get mechanism-specific frames
    if("PLAIN" == mechanism) {
        username = msg.get(6); 
        password = msg.get(7);             

    } else if("CURVE" == mechanism) {
        client_key = z85::encode(msg.get(6)); 

    } else if("GSSAPI" == mechanism) {
        principal = msg.get(6);
    }

    if (verbose) {
        std::cout << "auth: ZAP request mechanism=" << mechanism 
            <<" ipaddress=" << address << std::endl;
    }
}

/*! 
 * Send a ZAP reply to the handler socket
 */
void zap_request::reply (std::string status_code, std::string status_text) {
    if (verbose) {
        std::cout << "auth: ZAP reply status_code=" << status_code
            << " status_text=" << status_text << std::endl;
    }

    message reply;
    reply << version << sequence << status_code << status_text << "" << "";
    
    zap_socket.send(reply);
}    

}

#endif
