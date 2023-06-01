// TODO find better alternatives than Casablanca for C++ REST API ? (consider https://github.com/CrowCpp/Crow or https://github.com/drogonframework/drogon)

#include <cpprest/http_listener.h>
#include <cpprest/json.h>

using namespace web;
using namespace http;
using namespace http::experimental::listener;
using namespace utility;

class OIDCRedirectHandler {

    public:
        OIDCRedirectHandler() {}
        /**
         * - `url` is the the URL that the server (listener) will bind to
         * 
         * - `OIDCRedirectHandler(utility::string_t url): m_listener(url)`
         *  - here, the `:m_listener(url)` part is an initialization list; this C++ feature allows to initialize member variables when the object is created
        */
        OIDCRedirectHandler(utility::string_t url): m_listener(url) {
            /**
             * here:
             * - we set the server so that it handles GET requests
             * - we set a specific handler
             * - `std::placeholders::_1` represents the first argument passed to the function when it's called
             * 
             * In substance, we are saying "when an HTTP GET request comes in, call the `handle_get` method of this object to handle the request"
            */
            m_listener.support(methods::GET, std::bind(&OIDCRedirectHandler::handle_get, this, std::placeholders::_1));
        }
        /**
         * marking this method as `virtual` means it can be overriden by a derived class,
         * this allows for the appropriate method to be called based on the type of object at run time;
         * in this case, we have declared a virtual destructor for this class,
         * this means that an object of a derived class can be safely deleted through a pointer to the base class,
         * this is important to avoid memory leaks or undefined behavior
        */
        virtual ~OIDCRedirectHandler() {};

        pplx::task<void> close() {
            return m_listener.close();
        }
        pplx::task<void> open() {
            return m_listener.open();
        }

    protected:
        void handle_get(http_request req) {

            // get the path for the request we use the `auto` keyword for type inference
            auto path = uri::decode(req.relative_uri().path());

            if(path == U("/oidc-redirect")) {
                /*
                * extract the authorization code from the URL
                */
                auto http_get_vars = uri::split_query(req.request_uri().query());
                auto code = http_get_vars[U("code")]; // empty string if no query param called 'code'
                // TODO get the user's intended redirect URI

                // TODO OIDC: Proceed with exchanging the authorization code) for an ID token

                // Send a response to the client
                req.reply(status_codes::OK, U("Received authorization code: ") + code);
            } else if (path == U("/protected")) {
                // get Bearer token from request headers
                auto auth_header = req.headers()[U("Authorization")];
                req.reply(status_codes::OK, U("this is the provided token: ") + auth_header);
                // TODO: Verify the token. This usually involves making a request to the OIDC provider
                // TODO: Depending on the outcome of the token verification, adjust the response
            } else {
                req.reply(status_codes::NotFound, U("Not found"));
            }

        }

    private:
        // we prefix the name by `m_` here to specifiy that this is a class member variable
        http_listener m_listener;

};

int main() {
    utility::string_t port = U("8080");
    utility::string_t address = U("http://localhost:");
    address.append(port);

    OIDCRedirectHandler handler(address);
    handler.open().wait();

    /**
     * ucout is an output stream object in C++ that is used to write Unicode characters to the console. 
     * It is defined in the cpprest/details/basic_types.h header file of the Casablanca library.
    */
    ucout << utility::string_t(U("Listening for requests at: ")) << address << std::endl;

    std::string line;
    std::getline(std::cin, line); // Wait for input to stop the server

    handler.close().wait();

    return 0;
}

