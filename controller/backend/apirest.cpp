#pragma once

namespace controller { express_tcp_t apirest_router(){
    auto app = express::http::add();

    app.ALL([=]( express_http_t cli ){

        cli.header( "Access-Control-Allow-Methods", "GET" );
        cli.header( "Access-Control-Allow-Origin" , "*" );
        cli.send  ( "[ APIFY ONLY - HTTP BLOCKED ]" );

    });

return app; }}
