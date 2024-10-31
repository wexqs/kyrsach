#include "Interface.h"
#include "Connector.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "Communicate.h"
#include "Errors.h"
#include "Logger.h"
namespace po = boost::program_options;
int Interface::comm_proc(int argc, const char** argv)
{
    bool flag_b = false;
    bool flag_l = false;
    bool flag_p = false;
    int PORT;
    std::string logfile;
    std::string basefile;
    try {
        po::options_description opts("Allowed options");
        opts.add_options()
        ("help,h", "Show help")
        ("basefile,b",
         po::value<std::string>()->default_value("/home/stud/kyrsach/test/auth.txt"),
         "option is string(path to file with database)") 
        ("logfile,l",                        
         po::value<std::string>()->default_value("/home/stud/kyrsach/test/log.txt"), 
         "option is string(path to file with logs)")
        ("PORT,p",                               
         po::value<int>(&PORT)->default_value(33333),
         "option is int (PORT for server)");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, opts), vm);
        po::notify(vm);
        if(vm.count("help")) {
            std::cout << opts << "\n";
            exit(0);
        }

        if(vm.count("basefile")) {
            if(vm["basefile"].as<std::string>() == "/home/stud/kyrsach/test/auth.txt") {
                flag_b = true;
            }
            basefile = vm["basefile"].as<std::string>();
        }
        if(vm.count("logfile")) {
            if(vm["logfile"].as<std::string>() == "/home/stud/kyrsach/test/log.txt") {
                flag_l = true;
            }
            logfile = vm["logfile"].as<std::string>();
        }

        if(vm.count("PORT")) {
            if(vm["PORT"].as<int>() == 33333) {
                flag_p = true;
            }
            PORT = vm["PORT"].as<int>();
    }
   	if(PORT < 1024 or PORT > 65535){
   		throw crit_err("Incorrect port");
   	}
	if(flag_b and flag_l and flag_p) {
        std::cout << "Server started with default parameters.Use -h for help"<<std::endl;
        }
    Logger l1(logfile);
        if(logfile != "/home/stud/log.txt") {
            l1.writelog("Path to logfile set value: "+logfile);
        } else {
            l1.writelog("Path to logfile set default value");
        }
    if(basefile != "/home/stud/server.txt") {
        l1.writelog("Path to basefile set value: " + basefile);
    } else {
        l1.writelog("Path to basefile set default value");
    }
    if(PORT != 33333) {
        l1.writelog("Port set not default value");
    } else {
        l1.writelog("Port set default value");
    }
    Connector c1;
    c1.connect(basefile);
    l1.writelog("Connect to database success!");
    l1.writelog("Server started");
    Communicate con;
    #if _UNITTEST_!=1
    con.connection(PORT,c1.get_data(),&l1);
	#endif
    }catch(crit_err& e){
    	#if _UNITTEST_ != 1
    	std::cerr << "error: " << e.what() << "\n";
        std::cerr << "Критическая ошибка" << std::endl;
        #endif
        #if _UNITTEST_ ==1
        std::cerr << "crit_err exception" << std::endl;
        #endif
    }catch(po::error& e) {
    	#if _UNITTEST_ != 1
        std::cerr << "error: " << e.what() << "\n";
        std::cerr << "Use -h for help\n";
        #endif
        #if _UNITTEST_ ==1
        std::cerr << "Boost errors" << std::endl;
        #endif
    }catch(std::exception& e) {
    	#if _UNITTEST_ != 1
        std::cerr << "error: " << e.what() << "\n";
        std::cerr << "Use -h for help\n";
        #endif
        #if _UNITTEST_ ==1
        std::cerr << "Exception" << std::endl;
        #endif
    } catch(...) {
        std::cerr << "Exception of unknown type!\n";
        std::cerr << "Use -h for help\n";
        std::terminate();
    }
    return 0;
}
