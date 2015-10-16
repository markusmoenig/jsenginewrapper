/*
 * Copyright (c) 2015 Markus Moenig <markusm@visualgraphics.tv>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "jswrapper.hpp"

#include <iostream>

// --- Sample print function. Prints all arguments.

JSWRAPPER_FUNCTION( print )
    // --- JSWrapperArguments args; is already defined

    for ( int i=0; i < args.count(); ++i )
    {
        JSWrapperData data=args.at( i );

        if ( data.type() == JSWrapperData::Undefined )
            std::cout << "Undefined ";
        else
        if ( data.type() == JSWrapperData::Null )
            std::cout << "Null ";
        else            
        if ( data.type() == JSWrapperData::Number )
            std::cout << data.toNumber() << " ";
        else
        if ( data.type() == JSWrapperData::String )
            std::cout << data.toString() << " ";
        else
        if ( data.type() == JSWrapperData::Boolean )
            std::cout << data.toBoolean() << " ";
        else
        if ( data.type() == JSWrapperData::Object )
            std::cout << "[Object object] ";        
    }
 
    std::cout << std::endl;

    JSWrapperData data;
    JSWRAPPER_FUNCTION_SETRC( data )

JSWRAPPER_FUNCTION_END

int main(int argc, char** argv)
{
    JSWrapper jsWrapper( argv[0] );
    if ( !jsWrapper.m_isValid ) {
        printf("Unable to Initialize JavaScript Wrapper.\n");
        return 1;
    }

    JSWrapperObject *global=jsWrapper.globalObject(); // Get the global object
    global->registerFunction( "print", print );
    
    jsWrapper.execute( print( "'Hello' + ', World!'" ) );
    
    delete global;
    return 0;
}