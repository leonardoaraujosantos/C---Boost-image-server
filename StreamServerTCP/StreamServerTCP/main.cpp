#include <iostream>

#include "Server.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int ac, char* av[])
{
	int use_video;
	int server_port;
	std::string image_dir;
	std::string image_extension;	
	int debugMode;
	int equalizeHist;
	std::string ROI;

	try
	{
		// Parse program options...
		po::options_description desc("Allowed options");
		desc.add_options()
            ("help,h", "produce help message")
			("port,p",  po::value<int>(&server_port)->default_value(8666), "UDP server port")
			("video,v",  po::value<int>(&use_video)->default_value(0), "Use camera flag") 
			("debug,g",  po::value<int>(&debugMode)->default_value(0), "Debug mode") 
			("equalize,q",  po::value<int>(&equalizeHist)->default_value(0), "Equalize histogram")
			("ROI,r", po::value<std::string>(&ROI)->default_value(""),"Apply ROI on output image.")
			("directory,d", po::value<std::string>(&image_dir)->default_value("."),"Image directory.")
			("extension,e", po::value<std::string>(&image_extension)->default_value(".jpg"),"Image extension.")
        ;

		po::variables_map vm;        
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);
		
		if (vm.count("help")) 
		{
			std::cout << desc << std::endl;
            return 1;
        }
		if (vm.count("video")) 
		{            
			std::cout << "Video mode set to " << vm["video"].as<int>() << std::endl;
        }
		if (vm.count("port")) 
		{            
			std::cout << "UDP port number set to " << vm["port"].as<int>() << std::endl;
        }
		if (vm.count("directory"))
        {            
			std::cout << "Image directory: " << vm["directory"].as<std::string>() << std::endl;			
        }
		if (vm.count("extension"))
        {            
			std::cout << "Image extension: " << vm["extension"].as<std::string>() << std::endl;			
        }

		ImageFactory imFactory(use_video,image_dir,image_extension,debugMode,ROI,equalizeHist);
		boost::asio::io_service io_service;
		Server server(io_service,server_port,imFactory);
		io_service.run();	 
	}
	catch(std::exception &e)
	{		
		std::cerr << e.what() << std::endl;
	}
	std::cout << "Image server closing" << std::endl;
		
	return 0;

}