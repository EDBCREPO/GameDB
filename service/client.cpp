#include <nodepp/nodepp.h>
#include <apify/apify.h>
#include <nodepp/ws.h>

using namespace nodepp;

void onMain() {

    auto cli = ws::client( "ws://localhost:8000/api/0" );
    auto app = apify::add<ws_t>();
    auto tkn = string_t();

    app.on( "CONNECT", "/:tkn", [=]( apify_t<ws_t> cli ){
        tkn = cli.params[ "tkn" ];
    });

    cli.onConnect([=]( ws_t cli ){

        cli.onData([=]( string_t data ){
            app.next( cli, data );
        });

        cli.onDrain([=](){
            console::log( "disconnected" );
        }); console::log( "connected" );

    });

}
