#include <nodepp/nodepp.h>
#include <nginx/http.h>
#include <nodepp/os.h>

using namespace nodepp;

void onMain() {

    auto app = nginx::http::add();

    for( auto x=os::cpus(); x-->0; ){
    app.add( "pipe", regex::format( "/api/${0}", x ), object_t({
      { "href", regex::format( "http://localhost:80${0}1", x ) }
    }));}

    app.add( "pipe", nullptr, object_t({
        { "href"   , "http://localhost:8002/" },
        { "timeout", 10000 } /*--------------*/
    }));

    app.listen( "0.0.0.0", 8000, []( ... ){
        console::log( "Listening -> http://localhost:8000" );
    });

}
