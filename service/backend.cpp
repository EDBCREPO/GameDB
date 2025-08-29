#define SECRET process::env::get( "SECRET" )
#define CPU    process::env::get( "CPU" )

/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/nodepp.h>
#include <nodepp/encoder.h>
#include <nodepp/crypto.h>
#include <express/http.h>
#include <apify/apify.h>
#include <redis/redis.h>
#include <nodepp/wait.h>
#include <nodepp/ws.h>
#include <jwt/jwt.h>

/*────────────────────────────────────────────────────────────────────────────*/

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

#include "../controller/backend/game.cpp"
#include "../controller/backend/apify.cpp"
#include "../controller/backend/apirest.cpp"

/*────────────────────────────────────────────────────────────────────────────*/

void onMain() { process::env::init( ".env" );

    auto app = express::http::add(); app.USE( controller::apirest_router() );
    auto cid = string::to_uint( process::env::get( "CPU" ) ); /*-----------*/
    auto wpp = apify::add<ws_t>(); /*--------------------------------------*/
    auto prt = string::to_uint( regex::format( "80${0}1", cid ) ); /*------*/

    /*........................................................................*/

    wpp.add( "/:pid/:tkn/:gid", controller::apify_router() );

    wpp.on([=]( apify_t<ws_t> cli ){
        cli.emit( "FAIL", nullptr, "invalid route" );
    });

    auto skt = app.listen( "localhost", prt, [=]( socket_t ){
         console::log( "-> http://localhost:8001" );
    });

    /*........................................................................*/

    ws::server( skt ); skt.onConnect([=]( ws_t cli ){

        cli.onData([=]( string_t data ){ wpp.next( cli, data ); });

        auto token = controller::game::create_player( cli );
        auto pid   = regex::format( "/${0}", token );

        apify::add( cli ).emit( "CONNECT", pid, nullptr );

        cli.onDrain([=](){
            wpp.emit( cli, "DISCONNECT", pid, nullptr );
        });

    });

}

/*────────────────────────────────────────────────────────────────────────────*/
