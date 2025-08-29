#include <nodepp/nodepp.h>
#include <express/http.h>

using namespace nodepp;

void onMain() {

    auto app = express::http::add();

    app.USE( express::http::file( "./view" ) );

    app.listen( "localhost", 8002, [=]( socket_t ){
        console::log( "-> http://localhost:8002" );
    });

}