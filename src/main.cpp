#include "http_dev_service.h"

int main() {

    // constructing the base URL of the server
    utility::string_t port = U("8080");
    utility::string_t base_url = U("http://localhost:");
    base_url.append(port);

    // constructing the server
    HttpDevService service(base_url);
    service.start();

    std::string line;
    std::getline(std::cin, line); // Wait for input to stop the server

    service.stop();

    return 0;
}
