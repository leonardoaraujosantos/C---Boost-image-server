#pragma once

#include "TCPConnection.h"
#include "ImageFactory.h"

using boost::asio::ip::tcp;

class Server
{
public:
	Server(boost::asio::io_service& io_service, int port, ImageFactory &imFactory);
	~Server(void);
private:
	void start_accept();
	void handle_accept(boost::shared_ptr<TCPConnection> new_connection,const boost::system::error_code& error);
	tcp::acceptor acceptor_;

	// Class for get images from camera or directory
	ImageFactory &imFactory;	
};
