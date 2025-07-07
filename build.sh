#!/bin/bash
set -e

# Colors for better output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Required package versions
MIN_CMAKE_VERSION="3.14"
MIN_GCC_VERSION="8.0.0"
MIN_SQLITE_VERSION="3.22.0"
MIN_BOOST_VERSION="1.70.0"

# Directory where the script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Function to check if a command exists
command_exists() {
  command -v "$1" >/dev/null 2>&1
}

# Function to compare versions
version_greater_equal() {
    printf '%s\n%s\n' "$2" "$1" | sort -V -C
}

# Function to display section headers
section() {
    echo -e "${BLUE}=== $1 ===${NC}"
}

# Function to install packages based on the Linux distribution
install_packages() {
    local distro=""
    local packages=()
    
    # Detect distribution
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        distro=$ID
    fi
    
    echo -e "${YELLOW}Detected distribution: $distro${NC}"
    
    case "$distro" in
        ubuntu|debian|pop|linuxmint|elementary)
            sudo apt-get update
            
            # Add missing packages to the list
            for pkg in $@; do
                case $pkg in
                    cmake)
                        if ! command_exists cmake; then
                            packages+=("cmake")
                        fi
                        ;;
                    gcc)
                        if ! command_exists g++; then
                            packages+=("g++")
                        fi
                        ;;
                    sqlite3)
                        if ! command_exists sqlite3; then
                            packages+=("sqlite3" "libsqlite3-dev")
                        fi
                        ;;
                    ncurses)
                        # Check if ncurses is installed
                        if ! ldconfig -p | grep -q libncurses; then
                            packages+=("libncurses-dev")
                        fi
                        ;;
                    boost)
                        # Check if boost is installed
                        if ! ldconfig -p | grep -q libboost; then
                            packages+=("libboost-all-dev")
                        fi
                        ;;
                    openssl)
                        # Check if OpenSSL is installed
                        if ! ldconfig -p | grep -q libssl; then
                            packages+=("libssl-dev")
                        fi
                        ;;
                esac
            done
            
            # Install the packages if any are needed
            if [ ${#packages[@]} -gt 0 ]; then
                echo -e "${YELLOW}Installing packages: ${packages[@]}${NC}"
                sudo apt-get install -y "${packages[@]}"
            fi
            ;;
            
        fedora|rhel|centos|rocky|almalinux)
            sudo dnf check-update || true
            
            # Add missing packages to the list
            for pkg in $@; do
                case $pkg in
                    cmake)
                        if ! command_exists cmake; then
                            packages+=("cmake")
                        fi
                        ;;
                    gcc)
                        if ! command_exists g++; then
                            packages+=("gcc-c++")
                        fi
                        ;;
                    sqlite3)
                        if ! command_exists sqlite3; then
                            packages+=("sqlite" "sqlite-devel")
                        fi
                        ;;
                    ncurses)
                        # Check if ncurses is installed
                        if ! ldconfig -p | grep -q libncurses; then
                            packages+=("ncurses-devel")
                        fi
                        ;;
                    boost)
                        # Check if boost is installed
                        if ! ldconfig -p | grep -q libboost; then
                            packages+=("boost-devel")
                        fi
                        ;;
                    openssl)
                        # Check if OpenSSL is installed
                        if ! ldconfig -p | grep -q libssl; then
                            packages+=("openssl-devel")
                        fi
                        ;;
                esac
            done
            
            # Install the packages if any are needed
            if [ ${#packages[@]} -gt 0 ]; then
                echo -e "${YELLOW}Installing packages: ${packages[@]}${NC}"
                sudo dnf install -y "${packages[@]}"
            fi
            ;;
            
        *)
            echo -e "${RED}Unsupported distribution: $distro${NC}"
            echo "This script supports Ubuntu, Debian, Fedora, RHEL, CentOS, and Rocky Linux."
            echo "Please install the required dependencies manually:"
            echo "  - CMake (version $MIN_CMAKE_VERSION or higher)"
            echo "  - GCC/G++ (version $MIN_GCC_VERSION or higher)"
            echo "  - SQLite3 (version $MIN_SQLITE_VERSION or higher)"
            echo "  - NCurses development libraries"
            echo "  - Boost development libraries (version $MIN_BOOST_VERSION or higher)"
            echo "  - OpenSSL development libraries"
            exit 1
            ;;
    esac
}

# Function to check dependencies and their versions
check_dependencies() {
    section "Checking Dependencies"
    
    local all_dependencies_met=true
    
    # Check CMake
    if command_exists cmake; then
        local cmake_version=$(cmake --version | head -n1 | awk '{print $3}')
        if version_greater_equal "$cmake_version" "$MIN_CMAKE_VERSION"; then
            echo -e "${GREEN}CMake version $cmake_version is installed (minimum required: $MIN_CMAKE_VERSION)${NC}"
        else
            echo -e "${RED}CMake version $cmake_version is installed, but version $MIN_CMAKE_VERSION or higher is required${NC}"
            all_dependencies_met=false
        fi
    else
        echo -e "${RED}CMake is not installed${NC}"
        all_dependencies_met=false
    fi
    
    # Check G++
    if command_exists g++; then
        local gcc_version=$(g++ -dumpversion)
        if version_greater_equal "$gcc_version" "$MIN_GCC_VERSION"; then
            echo -e "${GREEN}G++ version $gcc_version is installed (minimum required: $MIN_GCC_VERSION)${NC}"
        else
            echo -e "${RED}G++ version $gcc_version is installed, but version $MIN_GCC_VERSION or higher is required${NC}"
            all_dependencies_met=false
        fi
    else
        echo -e "${RED}G++ is not installed${NC}"
        all_dependencies_met=false
    fi
    
    # Check SQLite3
    if command_exists sqlite3; then
        local sqlite_version=$(sqlite3 --version | awk '{print $1}')
        if version_greater_equal "$sqlite_version" "$MIN_SQLITE_VERSION"; then
            echo -e "${GREEN}SQLite3 version $sqlite_version is installed (minimum required: $MIN_SQLITE_VERSION)${NC}"
        else
            echo -e "${RED}SQLite3 version $sqlite_version is installed, but version $MIN_SQLITE_VERSION or higher is required${NC}"
            all_dependencies_met=false
        fi
    else
        echo -e "${RED}SQLite3 is not installed${NC}"
        all_dependencies_met=false
    fi
    
    # Check Boost
    if ldconfig -p | grep -q libboost_system; then
        # We can't easily check Boost version from the command line
        # This would require compiling a test program, which is complex for a script
        echo -e "${GREEN}Boost libraries are installed${NC}"
    else
        echo -e "${RED}Boost libraries are not installed${NC}"
        all_dependencies_met=false
    fi
    
    # Check NCurses
    if ldconfig -p | grep -q libncurses; then
        echo -e "${GREEN}NCurses libraries are installed${NC}"
    else
        echo -e "${RED}NCurses libraries are not installed${NC}"
        all_dependencies_met=false
    fi
    
    # Check OpenSSL
    if ldconfig -p | grep -q libssl; then
        echo -e "${GREEN}OpenSSL libraries are installed${NC}"
    else
        echo -e "${RED}OpenSSL libraries are not installed${NC}"
        all_dependencies_met=false
    fi
    
    if [ "$all_dependencies_met" = false ]; then
        section "Installing Missing Dependencies"
        install_packages cmake gcc sqlite3 ncurses boost openssl
        
        # Re-check after installation
        echo -e "\n${YELLOW}Checking dependencies after installation...${NC}"
        check_dependencies
    fi
}

build_project() {
    section "Building Project"
    
    # Create build directory if it doesn't exist
    mkdir -p "$SCRIPT_DIR/build"
    cd "$SCRIPT_DIR/build"
    
    # Configure with CMake
    echo -e "${YELLOW}Configuring with CMake...${NC}"
    cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_CLIENT=ON -DBUILD_SERVER=ON
    
    # Build the project
    echo -e "${YELLOW}Building the project...${NC}"
    cmake --build . -- -j$(nproc)
    
    # Copy configuration if it doesn't exist in the build directory
    if [ ! -d "config" ]; then
        echo "Copying configuration files..."
        mkdir -p config
        cp -r ../config/* config/
    fi
 
 # Create necessary directories
 mkdir -p data logs certs
    # Check if the binaries were created
    if [ -f "$SCRIPT_DIR/build/bin/chat_server" ] && [ -f "$SCRIPT_DIR/build/bin/chat_client" ]; then
        echo -e "${GREEN}Build successful!${NC}"
        echo -e "\nYou can run the server with:"
        echo -e "${YELLOW}  $SCRIPT_DIR/build/bin/chat_server <port>${NC}"
        echo -e "\nYou can run the client with:"
        echo -e "${YELLOW}  $SCRIPT_DIR/build/bin/chat_client <host> <port>${NC}"
    else
        echo -e "${RED}Build failed. Please check the error messages above.${NC}"
        exit 1
    fi
}

# Main execution

echo -e "${BLUE}=====================================${NC}"
echo -e "${BLUE}= C++ Real-Time Chat App Setup Tool =${NC}"
echo -e "${BLUE}=====================================${NC}"
echo ""

# Check for root permissions
if [ "$EUID" -eq 0 ]; then
    echo -e "${RED}Please do not run this script as root or with sudo.${NC}"
    echo "The script will prompt for sudo password when needed."
    exit 1
fi

# Check and install dependencies
check_dependencies

# Build the project
build_project

echo -e "\n${GREEN}Setup completed successfully!${NC}"
exit 0
EOF

chmod +x chat_app/setup_build.sh

echo "Created setup_build.sh script for the project"