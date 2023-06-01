#pragma once

// TODO find better alternatives than Casablanca for C++ REST API ? (consider https://github.com/CrowCpp/Crow or https://github.com/drogonframework/drogon)
#include <cpprest/http_listener.h>

/// @brief this class holds the logic for handling HTTP requests for the dev HTTP service that must be auth protected
class HttpDevServiceHTTPHandler {
public:
    /**
     * the default keyword is used to specify that the compiler should generate the default implementation of the constructor or destructor;
     * in this case, the default implementation is sufficient, 
     * so the default keyword is used to avoid having to write out the implementation explicitly
    */
    HttpDevServiceHTTPHandler() = default;
    virtual ~HttpDevServiceHTTPHandler() = default;

    /// @brief handles incoming GET HTTP requests, acts as a router
    /// @param req
    void handle_get_requests(web::http::http_request req);

private:
    /// @brief handles receiving the authorization code from the OIDC provider
    /// @param req 
    void handle_oidc_redirect(web::http::http_request req);

    /// @brief handles receiving a GET request to the protected endpoint
    /// @param req
    void handle_protected_get(web::http::http_request req);
};