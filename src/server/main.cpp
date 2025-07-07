#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <filesystem>

#include "server/chat_server.h"
#include "common/config_loader.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    try {
        std::cout << "C++ Real-Time Chat Server Starting..." << std::endl;
      
       // Determine config file path
       std::string config_path = "config/server_config.env";
       
       // Override config path from command line if provided
       if (argc > 1) {
           config_path = argv[1];
       }
       
       // Check if config file exists
       if (!fs::exists(config_path)) {
           std::cerr << "Error: Configuration file not found: " << config_path << std::endl;
           std::cerr << "Usage: " << argv[0] << " [config_file_path]" << std::endl;
           return 1;
       }
       
       // Load configuration
       chat::ConfigLoader config(config_path);
       std::cout << "Loading configuration from " << config_path << std::endl;
       config.dumpConfig();
       
      // Get server configuration
       int port = config.getServerPort();
       int thread_pool_size = config.getInt("THREAD_POOL_SIZE", 0);
       int autosave_interval = config.getAutosaveInterval();
       std::string db_path = config.getString("DATABASE_PATH", "data/chat.db");
       
       // Ensure the database directory exists
      fs::path db_dir = fs::path(db_path).parent_path();
       if (!db_dir.empty() && !fs::exists(db_dir)) {
           std::cout << "Creating database directory: " << db_dir << std::endl;
           fs::create_directories(db_dir);
       }
       
       // Ensure logs directory exists
       std::string log_file = config.getString("LOG_FILE", "logs/server.log");
       fs::path log_dir = fs::path(log_file).parent_path();
       if (!log_dir.empty() && !fs::exists(log_dir)) {
           std::cout << "Creating log directory: " << log_dir << std::endl;
           fs::create_directories(log_dir);
       }
        
        // Create io_context
        boost::asio::io_context io_context;
        
        // Create and start the server
        chat::ChatServer server(io_context, port);
 
       std::cout << "Starting server on port " << port << std::endl;
        server.start();

         
       // Determine thread pool size if set to auto-detect
       if (thread_pool_size <= 0) {
           thread_pool_size = std::thread::hardware_concurrency();
           if (thread_pool_size == 0) {
               thread_pool_size = 4; // Default if hardware_concurrency returns 0
           }
       }
       std::cout << "Using thread pool size: " << thread_pool_size << std::endl;
       
       threads.reserve(thread_pool_size);

       for (int i = 0; i < thread_pool_size; ++i) {
            threads.emplace_back([&io_context]() {
                io_context.run();
            });
        }

        std::cout << "Server is running. Press Ctrl+C to stop." << std::endl;
       
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
       
       std::cout << "Server stopped." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}