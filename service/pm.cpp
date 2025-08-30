#include <nodepp/nodepp.h>
#include <nodepp/popen.h>
#include <nodepp/os.h>

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

void cb( string_t x, uint y ){

    auto pid = popen::async( regex::format( "./build/${0} ?CPU=${1}", x, y ) );

    pid.onDrain([=](){
        if( process::should_close() ){ return; }
        console::error( x, "<> closed" );
        cb( x, y );
    });
    
    pid.onData([=]( string_t data ){
        conio::log( x, "<>", data );
    });

}

/*────────────────────────────────────────────────────────────────────────────*/

void onMain(){

    ptr_t<string_t> pm ({ "frontend", "backend", "nginx" });

    for( auto &x: pm ){
    for( auto y=os::cpus(); y-->0; ){ cb( x, y ); }}

}

/*────────────────────────────────────────────────────────────────────────────*/