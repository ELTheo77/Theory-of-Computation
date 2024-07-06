#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include "dfa.hpp"
#include "nfa.hpp"
#include "cfg.hpp"
#include "pda.hpp"

using boost::asio::ip::tcp;
namespace fs = boost::filesystem;

class HttpServer {
public:
    HttpServer(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
        socket_(io_context) {
        startAccept();
    }

private:
    void startAccept() {
        acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
            if (!ec) {
                handleRequest();
            }
            startAccept();
            });
    }

    void handleRequest() {
        auto read_buffer = std::make_shared<boost::asio::streambuf>();
        boost::asio::async_read_until(socket_, *read_buffer, "\r\n\r\n",
            [this, read_buffer](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::istream stream(read_buffer.get());
                    std::string request;
                    std::getline(stream, request);

                    // Parse the request method and URL path
                    std::istringstream request_stream(request);
                    std::string method, path;
                    request_stream >> method >> path;

                    if (method == "GET") {
                        handleGetRequest(path);
                    }
                    else if (method == "POST") {
                        handlePostRequest(path, stream);
                    }
                    else {
                        handleNotFound();
                    }
                }
            });
    }

    void handleGetRequest(const std::string& path) {
        if (path == "/") {
            serveFile("index.html");
        }
        else {
            std::string file_path = "." + path;
            if (fs::exists(file_path) && fs::is_regular_file(file_path)) {
                serveFile(file_path);
            }
            else {
                handleNotFound();
            }
        }
    }

    void handlePostRequest(const std::string& path, std::istream& request_stream) {
        if (path == "/dfa") {
            handleDFAValidation(request_stream);
        }
        else if (path == "/nfa") {
            handleNFAConversion(request_stream);
        }
        else if (path == "/cfg") {
            handleCFGValidation(request_stream);
        }
        else if (path == "/pda") {
            handlePDAConversion(request_stream);
        }
        else {
            handleNotFound();
        }
    }

    void handleDFAValidation(std::istream& request_stream) {
        std::string request_body((std::istreambuf_iterator<char>(request_stream)), std::istreambuf_iterator<char>());
        std::istringstream iss(request_body);
        std::string line;
        std::string dfa_str;
        std::string input_str;

        while (std::getline(iss, line)) {
            if (line.find("dfaDefinition") != std::string::npos) {
                dfa_str = extractJsonValue(line);
                dfa_str.erase(dfa_str.find("\""));
            }
            if (line.find("inputString") != std::string::npos) {
                input_str = extractJsonValue(line);
                for (int i = 0; i < 4; i++)
                    input_str.erase(0, input_str.find("\"") + 1);
            }
        }

        bool is_valid_dfa = false;
        bool accepts_input = false;
        try {
            DFA dfa(dfa_str);
            is_valid_dfa = dfa.validate();
            accepts_input = is_valid_dfa && dfa.accepts(input_str);
        }
        catch (const std::exception& e) {
            std::cerr << "DFA validation error: " << e.what() << "\n";
        }

        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
        response += "{\"is_valid_dfa\": " + std::string(is_valid_dfa ? "true" : "false") + ", ";
        response += "\"accepts_input\": " + std::string(accepts_input ? "true" : "false") + "}";
        sendResponse(response);
    }

    void handleNFAConversion(std::istream& request_stream) {
        std::string request_body((std::istreambuf_iterator<char>(request_stream)), std::istreambuf_iterator<char>());
        std::istringstream iss(request_body);
        std::string line;
        std::string nfa_str = "";
        int flag = 0;

        while (std::getline(iss, line)) {
            if (line[0] == 'q') flag = 1;
            if (flag) {
                nfa_str = nfa_str + line + "\\n";
            }
        }

        nfa_str.erase(nfa_str.length() - 2);
        nfa_str += "-";

        std::string dfa_str;
        try {
            NFA nfa(nfa_str);
            DFA dfa(nfa.toDFA());
            dfa_str = dfa.toString();
            for (int i = 0; i < dfa_str.length(); i++) {
                if (dfa_str[i] == 'n') dfa_str[i] = '\n';
            }
            size_t index = 0;
            while (true) {
                index = dfa_str.find("\\", index);
                if (index == std::string::npos) break;
                dfa_str.replace(index, 1, "");
            }
            dfa_str.erase(dfa_str.find_last_of("0"));
            dfa_str.erase(dfa_str.find_last_of("0"));
        }
        catch (const std::exception& e) {
            std::cerr << "NFA to DFA conversion error: " << e.what() << "\n";
        }

        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
        response += "{\"dfa\": \"" + escapeJson(dfa_str) + "\"}";
        sendResponse(response);
    }

    void handleCFGValidation(std::istream& request_stream) {
        std::string request_body((std::istreambuf_iterator<char>(request_stream)), std::istreambuf_iterator<char>());
        std::istringstream iss(request_body);
        std::string cfg_str;
        std::string line;

        while (std::getline(iss, line)) {
            cfg_str = extractJsonValue(line);
        }

        bool is_valid_cfg = false;
        try {
            CFG cfg(cfg_str);
            is_valid_cfg = cfg.validate();
        }
        catch (const std::exception& e) {
            std::cerr << "CFG validation error: " << e.what() << "\n";
        }

        /*if (cfg_str.find(">") == std::string::npos){
            is_valid_cfg = 0;
        }*/
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
        response += "{\"is_valid_cfg\": " + std::to_string(is_valid_cfg) + "}";
        sendResponse(response);
    }

    void handlePDAConversion(std::istream& request_stream) {
        std::string request_body((std::istreambuf_iterator<char>(request_stream)), std::istreambuf_iterator<char>());
        std::istringstream iss(request_body);
        std::string line;
        std::string pda_str;

        while (std::getline(iss, line)) {
            pda_str += line + "\n";
        }

        std::string cfg_str;
        try {
            PDA pda(pda_str);
            CFG cfg = pda.toCFG();
            cfg_str = cfg.toString();
        }
        catch (const std::exception& e) {
            std::cerr << "PDA to CFG conversion error: " << e.what() << "\n";
        }

        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
        response += "{\"cfg\": \"" + escapeJson(cfg_str) + "\"}";
        sendResponse(response);
    }

    void handleNotFound() {
        serveFile("404.html");
    }

    void serveFile(const std::string& file_path) {
        std::ifstream file(file_path, std::ios::binary);
        if (file.is_open()) {
            std::string content_type = getContentType(file_path);
            std::ostringstream response_stream;
            response_stream << "HTTP/1.1 200 OK\r\n";
            response_stream << "Content-Type: " << content_type << "\r\n\r\n";
            response_stream << file.rdbuf();
            sendResponse(response_stream.str());
            file.close();
        }
        else {
            handleNotFound();
        }
    }

    std::string getContentType(const std::string& file_path) {
        std::string extension = fs::path(file_path).extension().string();
        std::unordered_map<std::string, std::string> content_types = {
            {".html", "text/html"},
            {".css", "text/css"},
            {".js", "application/javascript"},
            {".json", "application/json"},
            {".txt", "text/plain"}
        };
        auto it = content_types.find(extension);
        if (it != content_types.end()) {
            return it->second;
        }
        return "application/octet-stream";
    }

    std::string extractJsonValue(const std::string& line) {
        std::size_t start_pos = line.find(":") + 1;
        std::size_t end_pos = line.find_last_of("\"");
        return line.substr(start_pos + 1, end_pos - start_pos - 1);
    }

    std::string escapeJson(const std::string& str) {
        std::ostringstream escaped;
        for (char c : str) {
            switch (c) {
            case '\\': escaped << "\\\\"; break;
            case '"': escaped << "\\\""; break;
            case '\b': escaped << "\\b"; break;
            case '\f': escaped << "\\f"; break;
            case '\n': escaped << "\\n"; break;
            case '\r': escaped << "\\r"; break;
            case '\t': escaped << "\\t"; break;
            default: escaped << c; break;
            }
        }
        return escaped.str();
    }

    void sendResponse(const std::string& response) {
        boost::asio::async_write(socket_, boost::asio::buffer(response),
            [this](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    socket_.shutdown(tcp::socket::shutdown_both);
                    socket_.close();
                }
            });
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
};

int main() {
    try {
        boost::asio::io_context io_context;
        HttpServer server(io_context, 8080);
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}