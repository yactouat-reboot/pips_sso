#include "http_dev_service_http_handler.h"

using namespace web::http;

void HttpDevServiceHTTPHandler::handle_get_requests(http_request req) {
    auto path = req.request_uri().path();
    if(path == U("/oidc-redirect")) {
        handle_oidc_redirect(req);
    } else if (path == U("/protected")) { 
        handle_protected_get(req);
    } else {
        req.reply(status_codes::NotFound, U("Not found"));
    }
}

// routes
void HttpDevServiceHTTPHandler::handle_oidc_redirect(http_request req) {
    // extract the authorization code from the URL query parameters
    auto http_get_vars = uri::split_query(req.request_uri().query());
    auto code = http_get_vars.at(U("code"));
    req.reply(status_codes::OK, U("Received authorization code: ") + code);
    // TODO get the user's intended redirect URI
    // TODO OIDC: Proceed with exchanging the authorization code for an ID token
}
void HttpDevServiceHTTPHandler::handle_protected_get(http_request req) {
    // get Bearer token from request headers
    auto auth_header = req.headers()[U("Authorization")];
    req.reply(status_codes::OK, U("this is the provided token: ") + auth_header);
    // TODO: Verify the token. This usually involves making a request to the OIDC provider
    // TODO: Depending on the outcome of the token verification, adjust the response
}