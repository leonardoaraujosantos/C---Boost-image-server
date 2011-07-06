#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

// OpenCv Headers
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

// Header do objeto de imagem generica
#include "GenericImage.h"

using boost::asio::ip::tcp;

char *image_data;

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: client <host>" << std::endl;
      return 1;
    }

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(argv[1], "8666");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
      socket.close();
      socket.connect(*endpoint_iterator++, error);
    }
    if (error)
      throw boost::system::system_error(error);
    
	
	// Get first the header...
	std::string imageHeaderStr;
	boost::array<char, 512> buf;
	GenericImage imageHeader;	
	{
		boost::system::error_code error;
		size_t len = socket.read_some(boost::asio::buffer(buf), error);

		if (error == boost::asio::error::eof)
			return 0; // Connection closed cleanly by peer.
		else if (error)
			throw boost::system::system_error(error); // Some other error.
		
		 std::cout << "(Header...) Tamanho recebido:" << len << std::endl;
		 imageHeaderStr = std::string(buf.begin(), len);
		 std::cout << "Header:" << imageHeaderStr << std::endl;	

		 // Recover header serialized
		 std::istringstream inStream(imageHeaderStr);
		 boost::archive::text_iarchive inArchive(inStream);
		 inArchive >> imageHeader;

		 std::cout << "imageHeader.size=" << imageHeader.getSize() << std::endl;	
	}

	// Alocate image size
	char *image_data = new char[imageHeader.getSize()];
	int image_index = 0;

	for (;;)
    {
      //boost::array<char, 320 * 240 * 3> recv_buf;
	  char *recv_buf = new char[65000];
      boost::system::error_code error;

      size_t len = socket.read_some(boost::asio::buffer(recv_buf,65000), error);

      if (error == boost::asio::error::eof)
        break; // Connection closed cleanly by peer.
      else if (error)
        throw boost::system::system_error(error); // Some other error.

	  std::cout << "(Image data...) Tamanho recebido:" << len << std::endl;	

	  // Create image buffer from packages received...
	  for (int count = 0; count < len; count++)
	  {
		image_data[image_index] = recv_buf[count];
		image_index++;
	  }	
	  delete [] recv_buf;
    }
	// Create an OpenCV image...
	IplImage *imReceivedImage;
	imReceivedImage = cvCreateImage(cvSize(imageHeader.getWidth(),imageHeader.getHeight()),IPL_DEPTH_8U,imageHeader.getChannels());  //164, 123
	imReceivedImage->widthStep = imageHeader.getStep();
	imReceivedImage->nChannels = imageHeader.getChannels();
	imReceivedImage->imageData = (char*)image_data;

	// Display image
	while(1)
	{
	cvShowImage("ROI",imReceivedImage); 
	char c	= cvWaitKey(1);	
	if(c==27) break;
	}

	// Delete buffer	
	delete [] image_data;
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}