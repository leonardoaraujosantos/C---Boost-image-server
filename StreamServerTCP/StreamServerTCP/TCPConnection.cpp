#include "TCPConnection.h"

TCPConnection::TCPConnection(boost::asio::io_service &io_service) : socket_(io_service)
{
	
}


void TCPConnection::handleWrite(const boost::system::error_code &error, size_t bytes_transfered)
{	
	if (!error)
	{
		std::cout << "Sent " << bytes_transfered << " bytes" << std::endl;
	}
	else
	{
		std::cout << "Error  " << error << " in TCPConnection::handleWrite value=" << error.value() << std::endl;
	}
}

void TCPConnection::start()
{	
	boost::system::error_code ignored_error;
	// Print some debug options...
	printOptions();

	// Send image header (size,channels,step,width,height, etc...)
	boost::asio::write(socket_, boost::asio::buffer(ImageHeader_), boost::asio::transfer_all(), 
		ignored_error);
	std::cout << "(Header) ignored_error=" << ignored_error << " message:" << ignored_error.message() << std::endl;
	
	// Send image data	
	boost::asio::write(socket_, boost::asio::buffer(ImageData_,size_), boost::asio::transfer_all(), 
		ignored_error);
	std::cout << "(Image data) ignored_error=" << ignored_error << " message:" << ignored_error.message() << std::endl;
}

boost::asio::ip::tcp::socket& TCPConnection::socket()
{
	return socket_;
}

boost::shared_ptr<TCPConnection> TCPConnection::create(boost::asio::io_service &io_service)
{
	return boost::shared_ptr<TCPConnection>(new TCPConnection(io_service));
}

void TCPConnection::setImageData(std::string header, char *data, int size)
{
	ImageData_ = data;
	size_ = size;	
	ImageHeader_ = header;
}

void TCPConnection::printOptions()
{
	boost::asio::detail::socket_option::integer<SOL_SOCKET, SO_SNDBUF> cOptionSNDMessageSize; 
	socket_.get_option( cOptionSNDMessageSize );
	std::cout << "Socket option SO_SNDBUF =" << cOptionSNDMessageSize.value() << std::endl;

	boost::asio::detail::socket_option::integer<SOL_SOCKET, SO_RCVBUF> cOptionRCVMessageSize; 
	socket_.get_option( cOptionRCVMessageSize );
	std::cout << "Socket option SO_RCVBUF =" << cOptionRCVMessageSize.value() << std::endl;
}