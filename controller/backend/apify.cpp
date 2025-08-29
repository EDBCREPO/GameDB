#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { apify_host_t<ws_t> apify_router(){
    apify_host_t<ws_t> app;

    /*.......................................................................*/

    app.on( "CONNECT", nullptr, [=]( apify_t<ws_t> cli ){
    auto pid = regex::format( "/${0}", cli.params["pid"] ) ; try {

        auto player = game::get_player( cli.params["tkn"] );
        game::connect_game( player, cli.params["gid"], cli.message );

        cli.emit( "DONE", pid, json::stringify( object_t({
            { "game" , game::get_game( cli.params["gid"] ) },
            { "stamp", string::to_string( process::now() ) }
        }) ));

    } catch( except_t err ) {

        cli.emit( "FAIL", pid, json::stringify( object_t({
            { "msg"  , err.data() }, /*-------------------*/
            { "stamp", string::to_string( process::now() ) },
        }) ));

    } });

    app.on( "PING", nullptr, [=]( apify_t<ws_t> cli ){
    auto pid=regex::format( "/${0}", cli.params["pid"] ) ; try {

        cli.emit( "DONE", pid, json::format( object_t({
            { "player", game::get_player ( cli.params["tkn"] ) },
            { "stamp" , string::to_string( process::now() ) },
            { "rand"  , string::to_string( rand() ) }
        }) ));

    } catch(...) {

        cli.emit( "FAIL", pid, json::stringify( object_t({
            { "stamp", string::to_string( process::now() ) },
            { "msg"  , "player not found" }
        }) ));

    } });

    app.on( "DISCONNECT", nullptr, [=]( apify_t<ws_t> cli ){
    auto pid=regex::format( "/${0}", cli.params["pid"] ) ; try {

        auto player = game::get_player( cli.params["tkn"] );
        game::disconnect_game( player ); /*---------------*/

        cli.emit( "DONE", pid, json::stringify( object_t({
            { "stamp", string::to_string( process::now() ) }
        }) ));

    } catch( except_t err ) {

        cli.emit( "FAIL", pid, json::stringify( object_t({
            { "msg"  , err.data() }, /*-------------------*/
            { "stamp", string::to_string( process::now() ) }
        }) ));

    } });

    /*.......................................................................*/

    app.on( "SERVER", "/all/:offset/:limit", [=]( apify_t<ws_t> cli ){
    auto pid=regex::format( "/${0}", cli.params["pid"] ); try {

        auto player = game::get_player( cli.params["tkn"] );
        auto limit  = string::to_uint( cli.params["limit"] );
        auto offset = string::to_uint( cli.params["offset"] );

        cli.emit( "DONE", pid, json::stringify( object_t({
            { "list" , game::all_server ( player, offset, limit, cli.message ) },
            { "stamp", string::to_string( process::now() ) },
        }) ));

    } catch( except_t err ) {

        cli.emit( "FAIL", pid, json::stringify( object_t({
            { "msg"  , err.data() }, /*-------------------*/
            { "stamp", string::to_string( process::now() ) }
        }) ));

    } });

    app.on( "SERVER", "/get/:sid", [=]( apify_t<ws_t> cli ){
    auto pid=regex::format( "/${0}", cli.params["pid"] ); try {

        auto player = game::get_player( cli.params["tkn"] );

        cli.emit( "DONE", pid, json::stringify( object_t({
            { "stamp", string::to_string( process::now() ) },
            { "list" , game::all_server ( player ) }
        }) ));

    } catch( except_t err ) {

        cli.emit( "FAIL", pid, json::stringify( object_t({
            { "msg"  , err.data() }, /*-------------------*/
            { "stamp", string::to_string( process::now() ) }
        }) ));

    } });

    app.on( "SERVER", "/join/:sid", [=]( apify_t<ws_t> cli ){
    auto pid =regex::format( "/${0}", cli.params["pid"] ); try {

        auto player = game::get_player( cli.params["tkn"] );
        game::connect_server( player, cli.params["sid"] );

        cli.emit( "DONE", pid, json::stringify( object_t({
            { "stamp", string::to_string( process::now() ) }
        }) ));

    } catch( except_t err ) {

        cli.emit( "FAIL", pid, json::stringify( object_t({
            { "msg"  , err.data() }, /*-------------------*/
            { "stamp", string::to_string( process::now() ) }
        }) ));

    } });

    app.on( "SERVER", "/unjoin", [=]( apify_t<ws_t> cli ){
    auto pid =regex::format( "/${0}", cli.params["pid"] ); try {

        auto player = game::get_player( cli.params["tkn"] );
        game::disconnect_server( player );

        cli.emit( "DONE", pid, json::stringify( object_t({
            { "stamp", string::to_string( process::now() ) }
        }) ));

    } catch( except_t err ) {

        cli.emit( "FAIL", pid, json::stringify( object_t({
            { "msg"  , err.data() }, /*-------------------*/
            { "stamp", string::to_string( process::now() ) }
        }) ));

    } });

    app.on( "SERVER", "/new", [=]( apify_t<ws_t> cli ){
    auto pid =regex::format( "/${0}", cli.params["pid"] ); try {

        auto player = game::get_player( cli.params["tkn"] );
        auto serv   = game::create_server( player, cli.message );

        auto n_path = regex::format( "/${0}/${1}/join/${2}",
            cli.params["tkn"], cli.params["pid"],
            server->name
        );

        app.emit( cli, "SERVER", n_path, cli.message );

    } catch( except_t err ) {

        cli.emit( "FAIL", pid, json::stringify( object_t({
            { "msg"  , err.data() }, /*-------------------*/
            { "stamp", string::to_string( process::now() ) }
        }) ));

    } });

    /*.......................................................................*/

    app.on( "CHAT", nullptr, [=]( apify_t<ws_t> cli ){
    auto pid=regex::format( "/${0}", cli.params["pid"] ); try {

        auto player = game::get_player( cli.params["tkn"] );
        game::broadcast( player, cli.method, cli.path, cli.message );

        cli.emit( "DONE", pid, json::stringify( object_t({
            { "stamp", string::to_string( process::now() ) }
        }) ));

    } catch( except_t err ) {

        cli.emit( "FAIL", pid, json::stringify( object_t({
            { "msg"  , err.data() }, /*-------------------*/
            { "stamp", string::to_string( process::now() ) }
        }) ));

    } });

    app.on( "GAME", nullptr, [=]( apify_t<ws_t> cli ){
    auto pid=regex::format( "/${0}", cli.params["pid"] ); try {

        auto player = game::get_player( cli.params["tkn"] );
        game::broadcast( player, cli.method, cli.path, cli.message );

        cli.emit( "DONE", pid, json::stringify( object_t({
            { "stamp", string::to_string( process::now() ) }
        }) ));

    } catch( except_t err ) {

        cli.emit( "FAIL", pid, json::stringify( object_t({
            { "msg"  , err.data() }, /*-------------------*/
            { "stamp", string::to_string( process::now() ) }
        }) ));

    } });

    /*.......................................................................*/

return app; }}

/*────────────────────────────────────────────────────────────────────────────*/
