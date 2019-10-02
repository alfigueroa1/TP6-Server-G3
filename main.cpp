#include <iostream>
#include <cstdio>
#include <chrono>
#include <ctime>
#include <cstring>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

typedef enum { INITIAL = 1, PATH, HOST, REST, END }stateTypes;
#define HELLO_PORT 80
#define RESPONSETRUE(path, length, date, date30) " HTTP/1.1 200 OK \x0D \x0A \
Date: " +(date)+ "\x0D \x0A \
Location : 127.0.0.1" +(path)+  "\x0D \x0A \
Cache - Control : max - age = 30  \x0D \x0A \
Expires : " +(date30)+ "  \x0D \x0A \
Content - Length : " +(length)+ " \x0D \x0A \
Content - Type : text / html; charset = iso - 8859 - 1  \x0D \x0A"

#define RESPONSEBAD(date, date30) "HTTP/1.1 404 Not Found \0x0D \0x0A \
Date : " +(date)+ " \0x0D \0x0A \
Cache - Control : public, max - age = 30 \0x0D \0x0A \
Expires : " +(date30)+ " \0x0D \0x0A \
Content - Length : 0 \0x0D \0x0A \
Content - Type : text / html; charset = iso - 8859 - 1 \0x0D \0x0A"

class server
{
public:
	server();
	void startConnection();
	void receiveMessage();
	bool parseMessage();
	void sendMessage();
	~server();

private:
	std::string buf;
	std::string path;
	std::string length;
	boost::asio::io_service* IO_handler;
	boost::asio::ip::tcp::acceptor* server_acceptor;
	boost::asio::ip::tcp::socket* socket_forServer;
};


void server::
startConnection()
{

	//Uncomment if non-blocking mode is desired
	//
	//When non-blocking mode is chosen accept operation
	//will fail with boost::asio::error::would_block
	//if there's no client inmediatelly connected when accept operation is performed.
	//Must comment blocking mode.
	//
	server_acceptor->non_blocking(true);
	boost::system::error_code error;
	do
	{
		server_acceptor->accept(*socket_forServer, error);
	} while ((error.value() == WSAEWOULDBLOCK));
	if (error)
		std::cout << "Error while trying to listen to " << HELLO_PORT << "Port " << error.message() << std::endl;

	//server_acceptor->accept(*socket_forServer);

	socket_forServer->non_blocking(true);
}

void server::
receiveMessage()
{

	size_t len;
	boost::system::error_code error;

	/*do
	{
		len = socket_forServer->read_some(boost::asio::buffer(buf, buf.max_size()), error);
		//para futuro, guardar buffer en archivo para leer cosas grandes
	} while ((error.value() == WSAEWOULDBLOCK));
	if (error)
		std::cout << "Error while trying to connect to server " << error.message() << std::endl;
	*/
	buf = "GET /path/filename/santiputo.html HTTP/1.1 /x0D /x0A Host: 127.0.0.1 /x0D /x0A asdadasdsofksdfok /x0D /x0A";
}

bool server::
parseMessage()		//El metodo valida el mensaje recibido del servidor. Principalmente guarda el path y verifica el uso correcto de instrucciones y conexion al host correcto.
{
	{
		bool ret = false;
		std::string validate[4] = { "GET ", "HTTP/1.1 \0x0D \0x0A","Host: 127.0.0.1 \0x0D \0x0A","\0x0D \0x0A \0x0D \0x0A" };
		char patht[200];
		int run = 1, i, p = 0;
		stateTypes state = INITIAL;
		size_t found = 0;
		while (run)
		{
			switch (state)
			{
			case INITIAL:
				found = buf.find(validate[0]);
				if (found != std::string::npos)
					state = PATH;
				else {
					ret = false;
					state = END;
				}
				break;
			case PATH:
				for (i = 4; buf[i] != ' '; i++, p++)
				{
					patht[p] = buf[i];
				}

				state = HOST;
				break;
			case HOST:
				found = buf.find(validate[3]);
				if (found != std::string::npos)
				{
					found = buf.find(validate[3]);
					if (found != std::string::npos) {
						state = END;
						ret = true;
					}
				}
				else {
					ret = false;
					state = END;
				}
				break;
			case END:
				run = 0;

				break;

			}
		}
		std::string pathc(patht, p);
		path = pathc;
			return ret;
	}
}

void server::
sendMessage()
{
	std::time_t time = std::time(nullptr);
	std::string date = std::asctime(std::localtime(&time));
	time = time + 30;
	std::string date30 = std::asctime(std::localtime(&time));

	std::string message = RESPONSETRUE(path, length, date, date30);
	size_t len;
	boost::system::error_code error;

	std::cout << message << std::endl;
	/*do
	{
		len = socket_forServer->write_some(boost::asio::buffer(buf, strlen(buf)), error);
	} while ((error.value() == WSAEWOULDBLOCK));
	if (error)
		std::cout << "Error while trying to connect to server " << error.message() << std::endl;*/
}


server::
server()
{
	IO_handler = new boost::asio::io_service();
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), HELLO_PORT);

	socket_forServer = new boost::asio::ip::tcp::socket(*IO_handler);
	server_acceptor = new boost::asio::ip::tcp::acceptor(*IO_handler, ep);
}

server::
~server()
{
	server_acceptor->close();
	socket_forServer->close();
	delete server_acceptor;
	delete socket_forServer;
	delete IO_handler;
}

int
main(int argc, char* argv[])
{
	
	server conquering;
	std::cout << std::endl << "Start Listening on port " << HELLO_PORT << std::endl;
	//conquering.startConnection();
	//std::cout << "Somebody connected to port " << HELLO_PORT << std::endl;
	//std::cout << "Press Enter to Send Message  " << std::endl;
	//getchar();
	conquering.receiveMessage();
	conquering.parseMessage();	//faltaria chequear el valor de retorno de esta, pero no lo hago porque creo que la deberia llamar receive.
	conquering.sendMessage();
	//Sleep(50); // Le damos 50ms para que llegue el mensaje antes de cerrar el socket.

	return 0;
}