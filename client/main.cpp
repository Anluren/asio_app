#include <iostream>
#include <asio.hpp>
#include <string>
#include <sstream>

using asio::ip::tcp;

void print_usage() {
    std::cout << "Usage:\n"
              << "  WRITE <key> <value>\n"
              << "  READ <key>\n";
}

int main(int argc, char* argv[]) {
    asio::io_context io_context;
    tcp::socket socket(io_context);
    try {
        socket.connect(tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), 12345));
    } catch (const std::exception& e) {
        std::cerr << "Failed to connect: " << e.what() << std::endl;
        return 1;
    }
    std::string line;
    print_usage();
    while (std::cout << "> " && std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string cmd, key, value;
        iss >> cmd >> key;
        if (cmd == "WRITE") {
            iss >> value;
            if (key.empty() || value.empty()) {
                std::cout << "Invalid WRITE. Usage: WRITE <key> <value>\n";
                continue;
            }
            std::ostringstream req;
            req << "WRITE " << key.size() << " " << value.size() << "\n" << key << value;
            asio::write(socket, asio::buffer(req.str()));
        } else if (cmd == "READ") {
            if (key.empty()) {
                std::cout << "Invalid READ. Usage: READ <key>\n";
                continue;
            }
            std::ostringstream req;
            req << "READ " << key.size() << "\n" << key;
            asio::write(socket, asio::buffer(req.str()));
        } else {
            std::cout << "Unknown command.\n";
            continue;
        }
        asio::streambuf resp_buf;
        asio::read_until(socket, resp_buf, '\n');
        std::istream resp_stream(&resp_buf);
        std::string resp_line;
        std::getline(resp_stream, resp_line);
        if (resp_line.rfind("OK ", 0) == 0) {
            int vlen = 0;
            std::istringstream vss(resp_line.substr(3));
            vss >> vlen;
            std::string value(vlen, '\0');
            if (vlen > 0) {
                asio::read(socket, asio::buffer(&value[0], vlen));
            }
            std::cout << "Value: " << value << std::endl;
        } else {
            std::cout << resp_line << std::endl;
        }
    }
    return 0;
}
