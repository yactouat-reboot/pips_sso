#pragma once

#include "http_dev_service_http_handler.h"

class HttpDevService {
    public:
        HttpDevService(utility::string_t url);
        virtual ~HttpDevService() = default;

        /// @brief starts the HTTP server
        void start();

        /// @brief stops the HTTP server
        void stop();

    private:
        /// @brief the base URL of the HTTP server
        utility::string_t base_url;

        /// @brief the handler for the HTTP server
        HttpDevServiceHTTPHandler m_handler;

        /// @brief the HTTP server (coming from the cpprestsdk library)
        web::http::experimental::listener::http_listener m_listener;

};