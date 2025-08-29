#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace game {

auto db = redis::add("db://localhost:6379");

struct player_t {
    string_t /*------------*/ name;
    string_t /*------------*/ game;
    string_t /*----------*/ server;
    ptr_t<ws_t> /*-------*/ socket;
    string_t /*-------*/ attribute;
};

struct server_t {
    string_t /*--------------*/ name;
    map_t<string_t,player_t*> player;
    string_t /*---------*/ attribute;
};

map_t<string_t,ptr_t<player_t>> tm;

struct game_t {
    string_t /*--------------------*/ name;
    map_t<string_t,ptr_t<server_t>> server;
};

map_t<string_t,ptr_t<game_t>> gm;

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace game {

    string_t normalize( string_t data ) { return regex::replace( data, R"([<'">])", "" ); }

    /*.......................................................................*/

    bool is_server( game_t* game, string_t server_id ){
        if( game == nullptr   ){ return false; }
        if( server_id.empty() ){ return false; }
        return game->server.has( server_id );
    }

    bool is_game( string_t game_id ){
        if( game_id.empty() ){ return false; }
        return gm.has( game_id );
    }

    /*.......................................................................*/

    game_t* get_game( string_t game_id ) {
        if( game_id.empty()  ){ throw except_t( "invalid gameID" ); }
        if( !gm.has(game_id) ){
            game_t game; game.name = game_id;
            gm[game_id] = type::bind( game );
        }   return gm[game_id].get();
    }

    player_t* get_player( string_t token ) {
        if( !jwt::verify( token, SECRET ) )
          { throw except_t( "invalid player's token" ); }

        auto tms = TIME_DAYS(2) - process::now();
        auto obj = json::parse( jwt::decode( token ) );
        auto stm = string::to_ulong( obj["stamp"].as<string_t>() );

        if( stm <= tms )
          { throw except_t( "expired player's token" ); }

        return tm[ obj["player_id"].as<string_t>() ].get();
    }

    server_t* get_server( game_t* game, string_t server_id ) {
        if( !is_server( game, server_id ) )
          { throw except_t( "invalid serverID" ); }
        return game->server[ server_id ].get();
    }

    server_t* get_server( player_t* player ) {
        auto game = get_game( player->game );
        if( !is_server( game, player->server ) )
          { throw except_t( "invalid serverID" ); }
        return game->server[ player->server ].get();
    }

    /*.......................................................................*/

    object_t all_server( player_t* player, uint offset, uint limit, string_t filter ){
        if( player==nullptr ){ throw except_t( "invalid player" ); }
        auto game = get_game( player->game ); object_t out;
        auto list = array_t<object_t>(); uint off=0, lim=0;

        auto x = game->server.raw().first(); while( x != nullptr ){
        auto y = x->next; auto z = x->data.second;

            if( off>=offset && lim>=limit ){ break; }

            if( !filter.empty() && !regex::test( z->attribute, filter ) )
              { x=y; continue; /*-------------------------------------*/ }

            if( off < offset ){ ++off; x=y; continue; }

            auto w = z->player.raw().first();
            array_t<object_t> players; while( w != nullptr ){
            auto y = w->next; auto z = w->data.second;

                players.push( object_t({
                    { "name", z->name   }, { "game", z->game },
                    { "serv", z->server }, { "attr", z->attribute }
                }) );

            }

            list.push( object_t({
                { "plyr", players },
                { "name", z->name },
                { "attr", z->attribute }
            }) );

            if( lim < limit  ){ ++lim; x=y; continue; }

        break; }

        out["size"] = game->server.size(); out["list"] = list;

    return out; }

    object_t all_server( player_t* player ) {
        if( player==nullptr ){ throw except_t( "invalid player" ); }
        array_t<object_t> players; object_t out;
        auto serv = get_server( player ); 

        auto x = serv->player.raw().first(); while( x != nullptr ){
        auto y = x->next; auto z = x->data.second;

            players.push( object_t({
                { "name", z->name   }, { "game", z->game },
                { "serv", z->server }, { "attr", z->attribute }
            }) );

        }

        return object_t({
            { "plyr", players },
            { "name", serv->name },
            { "attr", serv->attribute }
        });

    }

    /*.......................................................................*/

    void disconnect_server( player_t* player, server_t* server ) {
        if( player==nullptr ){ throw except_t( "invalid player" ); }
        if( server==nullptr ){ throw except_t( "invalid server" ); }
        
        server->player.erase( player->name );

        if( server->player.size() == 0 ){
            auto game = get_game( player->game );
            game->server.erase  ( player->server );    
        }

    }

    void disconnect_game( player_t* player, string_t game_id ) {
        if( player==nullptr ){ throw except_t( "invalid player" ); }
        if( game_id.empty() ){ throw except_t( "invalid gameID" ); }
    
        auto game = get_game( game_id ); if( !player->server.empty() ){
            if( is_server( game, player->server ) ){
                auto server = get_server( game, player->server );
                disconnect_server( player, server );
            }
        }

        if( game->server.empty() ){ gm.erase( game->name ); }

        player->game.clear(); player->server.clear();
    }

    /*.......................................................................*/

    void disconnect_server( player_t* player ) {
        auto game   = get_game  ( player->game ); /*---*/
        auto server = get_server( game, player->server );
        disconnect_server( player, server );
    }

    void disconnect_game( player_t* player ) {
         disconnect_game( player, player->game );
    }

    /*.......................................................................*/

    string_t create_player( ws_t cli ) {

        auto sha = crypto::hash::SHA1(); sha.update( string::to_string( process::now() ) );
        /*----------------------------*/ sha.update( string::to_string( cli.get_fd() ) );
        /*----------------------------*/ sha.update( cli.get_peername() );
        /*----------------------------*/ sha.update( SECRET + CPU );

        player_t item; item.socket = type::bind( cli );
        /*----------*/ item.name   = sha.get();
        tm[sha.get()]= type::bind( item );

        return jwt::encode( json::stringify( object_t({
             { "stamp"    , string::to_string( process::now() ) },
             { "player_id", sha.get() } /*---------------------*/
        })), SECRET );

    }

    server_t* create_server( player_t* player, string_t attribute ) {
        if( player == nullptr    ){ throw except_t( "invalid player" ); }
        if( player->game.empty() ){ throw except_t( "invalid gameID" ); }

        if( !player->server.empty() ){
            auto game   = get_game  ( player->game );
            auto server = get_server( game, player->server );
            disconnect_server( player, server );
        }

        auto sha = crypto::hash::SHA1(); sha.update( string::to_string( process::now() ) );
        /*----------------------------*/ sha.update( player->server );
        /*----------------------------*/ sha.update( player->name );
        /*----------------------------*/ sha.update( SECRET + CPU );

        auto game = get_game( player->game );
        auto serv = server_t(); serv.name = sha.get();
        game->server[ sha.get() ] = type::bind( serv );
        
    return game->server[ sha.get() ].get(); }

    /*.......................................................................*/

    void broadcast( player_t* player, string_t method, string_t path, string_t message ) {
         if( player == nullptr      ){ throw except_t( "invalid player" ); }
         if( player->game  .empty() ){ throw except_t( "invalid gameID" ); }
         if( player->server.empty() ){ throw except_t( "invalid servID" ); }

         auto game = get_game  ( player->game );
         auto serv = get_server( game, player->server );

         auto x = serv->player.raw().first(); while( x!= nullptr ){
         auto y = x->next; auto z = x->data.second;

            if( z->socket->get_fd() == player->socket->get_fd() )
              { x=y; continue; /*----------------------------*/ }

            apify::add( *z->socket ).emit( method, path, message );

         x=y; }

    }

    /*.......................................................................*/

    void connect_game( player_t* player, string_t game_id, string_t attribute ) {
         if( player == nullptr ){ throw except_t( "invalid player" ); }
         if( game_id.empty() )  { throw except_t( "invalid gameID" ); }
         disconnect_game( player, game_id ); player->attribute = attribute;
         auto game=get_game( game_id ); /**/ player->game = game->name;
    }

    void connect_server( player_t* player, string_t server_id ) {
         if( player == nullptr )   { throw except_t( "invalid player" ); }
         if( player->game.empty() ){ throw except_t( "invalid gameID" ); }
         if( server_id.empty() )   { throw except_t( "invalid servID" ); }

         if( !player->server.empty() )
           { disconnect_server( player, get_server( player ) ); }

         auto game = get_game  ( player->game );
         auto srv  = get_server( game, server_id );

         srv->player[ player->name ] = player;
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/
