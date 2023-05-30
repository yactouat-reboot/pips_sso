# include <cpprest/filestream.h>
# include <cpprest/http_client.h>

using namespace concurrency::streams; // async streams
using namespace utility; // common utils like string conversion
using namespace web; // common features like URIs
using namespace web::http; // common HTTP functionalities
using namespace web::http::client;

/**
 * @brief The entry point of an HTTP client program
 * @param argc The number of arguments
 * @param argv A pointer to an array of char pointers (the array of arguments)
 * @return program exist code
 */
int main(int argc, char* argv[]) {

    /**
     * creating a HTTP client;
     * `U("http://google.com")` is the constructor's argument;
     *  - `U()` is a macro
     *  - once we have instantiated the client, "http://google.com" will be its base URL to make requests
    */
    http_client client(U("http://google.com"));

}

