#include <gtest/gtest.h>
#include "common/config_loader.h"
#include <sstream>
#include <fstream>

using namespace chat;

// Test fixture
class ConfigLoaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary config file for testing
        std::ofstream config_file("test_config.env");
        config_file << "# Test configuration\n";
        config_file << "SERVER_PORT=9000\n";
        config_file << "MAX_CONNECTIONS=50\n";
        config_file << "AUTOSAVE_INTERVAL=120 # in seconds\n";
        config_file << "ENABLE_SSL=true\n";
        config_file << "LOG_LEVEL=DEBUG\n";
        config_file << "THREAD_POOL_SIZE=8\n";
        config_file << "DATABASE_PATH=test/data.db\n";
        config_file.close();
    }
    
    void TearDown() override {
        // Clean up the temporary file
        std::remove("test_config.env");
    }
};

// Test basic loading and value retrieval
TEST_F(ConfigLoaderTest, LoadAndGetValues) {
    ConfigLoader config("test_config.env");
    
    // Test integer values
    EXPECT_EQ(config.getServerPort(), 9000);
    EXPECT_EQ(config.getInt("MAX_CONNECTIONS"), 50);
    EXPECT_EQ(config.getInt("NONEXISTENT", 42), 42); // Default value
    
    // Test string values
    EXPECT_EQ(config.getString("DATABASE_PATH"), "test/data.db");
    EXPECT_EQ(config.getString("NONEXISTENT", "default"), "default");
    
    // Test boolean values
    EXPECT_TRUE(config.getBool("ENABLE_SSL"));
    EXPECT_FALSE(config.getBool("NONEXISTENT", false));
    
    // Test specific getters
    EXPECT_EQ(config.getAutosaveInterval(), 120);
    EXPECT_EQ(config.getLogLevel(), LogLevel::DEBUG);
}

// Test presence check
TEST_F(ConfigLoaderTest, HasKey) {
    ConfigLoader config("test_config.env");
    
    EXPECT_TRUE(config.hasKey("SERVER_PORT"));
    EXPECT_TRUE(config.hasKey("LOG_LEVEL"));
    EXPECT_FALSE(config.hasKey("NONEXISTENT"));
}

// Test dumping configuration
TEST_F(ConfigLoaderTest, DumpConfig) {
    ConfigLoader config("test_config.env");
    
    std::stringstream ss;
    config.dumpConfig(ss);
    
    std::string dump = ss.str();
    EXPECT_TRUE(dump.find("SERVER_PORT = 9000") != std::string::npos);
    EXPECT_TRUE(dump.find("MAX_CONNECTIONS = 50") != std::string::npos);
    EXPECT_TRUE(dump.find("LOG_LEVEL = DEBUG") != std::string::npos);
}