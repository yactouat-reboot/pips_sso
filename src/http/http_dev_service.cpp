#include "http_dev_service.h"

// using an initialization list; this C++ feature allows to initialize member variables when the object is created
HttpDevService::HttpDevService(utility::string_t url): m_listener(url), base_url(url) {
}

void HttpDevService::start() {
    /**
     * Here:
     * - we set the server so that it handles GET requests
     * - we set a specific handler
     * - `std::placeholders::_1` represents the first argument passed to the function when it's called
     * 
     * In substance, we are saying "when an HTTP GET request comes in, call the `handle_get_requests` method of this object to handle the request"
    */
    m_listener.support(
        web::http::methods::GET, 
        std::bind(
            &HttpDevServiceHTTPHandler::handle_get_requests, 
            &m_handler, 
            std::placeholders::_1
        ));

    /**
     * ucout is an output stream object in C++ that is used to write Unicode characters to the console. 
     * It is defined in the cpprest/details/basic_types.h header file of the Casablanca library.
    */
    ucout << utility::string_t(U("Listening for requests at: ")) << base_url << std::endl;

    m_listener.open().wait();
}

void HttpDevService::stop() {
    m_listener.close().wait();
}


