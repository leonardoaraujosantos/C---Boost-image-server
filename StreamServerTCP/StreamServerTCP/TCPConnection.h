#pragma once
#define BOOST_ASIO_NO_NOMINMAX
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

class TCPConnection : public boost::enable_shared_from_this<TCPConnection>
{
public:		
	static boost::shared_ptr<TCPConnection> create(boost::asio::io_service &io_service);
	boost::asio::ip::tcp::socket& socket();
	void start();
	void setImageData(std::string header, char *data, int size);
private:
	TCPConnection(boost::asio::io_service &io_service);
	void handleWrite(const boost::system::error_code &error, size_t bytes_transfered);
	void printOptions();
	boost::asio::ip::tcp::socket socket_;
	char *ImageData_;
	std::string ImageHeader_;
	int size_;	
};
