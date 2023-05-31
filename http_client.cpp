# include <cpprest/filestream.h>
# include <cpprest/http_client.h>

using namespace concurrency::streams; // async streams
using namespace utility; // common utils like string conversion
using namespace web; // common features like URIs
using namespace web::http; // common HTTP functionalities
using namespace web::http::client;

/**
 * @brief The entry point of an HTTP client program
 * @param argc The number of arguments (should be 3: the program, the URL, and the resource without the `/`)
 * @param argv A pointer to an array of char pointers (the array of arguments)
 * @return program exist code
 */
int main(int argc, char* argv[]) {

    // checking that the program is called correctly
    if (argc < 2 || argc > 3) {
        std::cerr << "Wrong Usage, here is how you should use the client: " << argv[0] << " <url> <resourcePath>\n";
        return 1;
    }
    std::string url = argv[1];
    std::string resourcePath = argc == 3 ? argv[2] : "";

    /**
     * creating a HTTP client;
     * `U("http://google.com")` is the constructor's argument;
     *  - `U()` is a macro that makes sure that the string of the URL is correctly interpreted across platforms
     *  - once we have instantiated the client, "http://google.com" will be its base URL to make requests
    */
    http_client client(U(url));

    // Build request URI and start the request.
    uri_builder builder(U("/"));
    builder.append_path(U(resourcePath), true);

    // Send the HTTP GET asynchronous request, and process the response.
    client.request(methods::GET, builder.to_string())
        .then([](http_response response)
        {
            // get response headers
            printf("Received response status code:%u\n", response.status_code());
            // extract response body as string
            return response.extract_string();
        })
        .then([](string_t body)
        {
            printf("Received response body:%s\n", body.c_str());
        })
        // wait for the asynchronous operation to complete and block the main thread while waiting for completion
        .wait();

    return 0;

}

