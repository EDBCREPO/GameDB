#include <nodepp/nodepp.h>
#include <express/http.h>

using namespace nodepp;

void onMain() {

    auto app = express::http::add();

    app.USE([=]( express_http_t cli, function_t<void> next ){
        cli.header( "Cross-Origin-Opener-Policy"  , "same-origin"  );
        cli.header( "Cross-Origin-Embedder-Policy", "require-corp" );
    next(); });

    app.USE( express::http::file( "./view" ) );

    app.listen( "localhost", 8002, [=]( socket_t ){
        console::log( "-> http://localhost:8002" );
    });

}