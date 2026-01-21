#include "winsimple.h"


int main()
{

	
	
	
	ws::Network::Socket socket;
	ws::Network::Host host("itch.io");
	ws::Network::Server server(host,80); 
	socket.connectToServer(server);
	
	//Send a request to server to get info
	socket.sendData("HEAD / HTTP/1.0\r\n\r\n");
	
	std::string buffer = socket.getData();
	std::cout << buffer << "\n";
	
	
	
	
    
	system("pause");
	return 0;
}