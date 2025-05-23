// Server main.cpp moved here
#include <iostream>
#include <asio.hpp>
#include <leveldb/db.h>
#include <memory>
#include <string>
#include <sstream>

using asio::ip::tcp;

constexpr int PORT = 12345;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, leveldb::DB* db)
        : socket_(std::move(socket)), db_(db) {}

    void start() {
        do_read();
    }

private:
    tcp::socket socket_;
    leveldb::DB* db_;
    std::array<char, 1024> buffer_;

    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(buffer_),
            [this, self](std::error_code ec, std::size_t length) {
                if (!ec) {
                    // Handle received data
                    std::string data(buffer_.data(), length);
                    std::cout << "Received: " << data << std::endl;

                    // Example: Write data to LevelDB
                    leveldb::WriteOptions write_options;
                    db_->Put(write_options, "key", data);

                    // Example: Read data from LevelDB
                    std::string value;
                    leveldb::ReadOptions read_options;
                    db_->Get(read_options, "key", &value);
                    std::cout << "Stored in DB: " << value << std::endl;

                    // Echo the data back to the client
                    do_write(data);
                }
            });
    }

    void do_write(const std::string& data) {
        auto self(shared_from_this());
        asio::async_write(socket_, asio::buffer(data),
            [this, self](std::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    // Start reading the next message
                    do_read();
                }
            });
    }
};

class Server {
public:
    Server(asio::io_context& io_context, short port, leveldb::DB* db)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), db_(db) {
        do_accept();
    }

private:
    tcp::acceptor acceptor_;
    leveldb::DB* db_;

    void do_accept() {
        acceptor_.async_accept(
            [this](std::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket), db_)->start();
                }

                do_accept();
            });
    }
};

int main() {
    try {
        leveldb::DB* db;
        leveldb::Options options;
        options.create_if_missing = true;

        // Open the database
        leveldb::Status status = leveldb::DB::Open(options, "mydb", &db);
        if (!status.ok()) {
            std::cerr << "Unable to open/create test.db: " << status.ToString() << std::endl;
            return 1;
        }

        asio::io_context io_context;
        Server server(io_context, PORT, db);

        std::cout << "Server is running on port " << PORT << std::endl;

        io_context.run();

        // Close the database
        delete db;
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
