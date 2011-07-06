#include "Server.h"

Server::Server(boost::asio::io_service& io_service, int port, ImageFactory &imFactory) : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)), imFactory(imFactory)
{
	start_accept();
}

Server::~Server(void)
{
}

void Server::start_accept()
{
	// Create new connection and listen to it
	boost::shared_ptr<TCPConnection> new_connection = TCPConnection::create(acceptor_.io_service());		
	
	acceptor_.async_accept(new_connection->socket(), boost::bind(&Server::handle_accept, this, 
		new_connection, boost::asio::placeholders::error));
	
}

void Server::handle_accept(boost::shared_ptr<TCPConnection> new_connection,const boost::system::error_code& error)
{
	if (!error)
	{
		// Get next image....
		GenericImage image;
		imFactory.getImage(image);

		// Serialize GenericImage data to serve as a header to the client...
		std::ostringstream outStream;
		//boost::archive::binary_oarchive outArchive(outStream);
		boost::archive::text_oarchive outArchive(outStream);
		outArchive << image;

		std::cout << "Image header:" << outStream.str() << std::endl;
		std::cout << "Image has " << image.getSize() << " bytes..." << std::endl;
		new_connection->setImageData(outStream.str(), (char*)image.getImageBuffer(),image.getSize());
		
		// Send image through it's connection...
		new_connection->start();
		
		// Accept new client again...
		start_accept();
	}
	else
	{
		throw std::exception("Error in Server::handle_accept"); 
	}
}
