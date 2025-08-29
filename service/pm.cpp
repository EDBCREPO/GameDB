#include <nodepp/nodepp.h>
#include <nodepp/popen.h>
#include <nodepp/ws.h>

using namespace nodepp;

void onMain(){

    process::add([=](){

        auto list = ptr_t<string_t>({ "frontend", "backend" , "nginx" });

        for( auto x: list ){ for( auto y=os::cpus(); y-->0; ){

            popen::async( regex::format( "./build/${0} ?CPU=${1}", x, y ) )
            .onData([=]( string_t data ){
                conio::log( regex::format( "<${0}> ${1}", x, data ));
            });

        }}

    return -1; });

}
