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

// --- C++ Class

class Dog
{
public:    
    Dog( void ) { m_name="Freddy"; }
    std::string m_name;
    int         m_age;
};

// ---

JSWRAPPER_CONSTRUCTOR( DogConstructor, "Dog" )
    // --- JSWrapperArguments args; is already defined

    std::cout << "Dog Constructor" << std::endl;

    Dog *dog=new Dog();
    JSWRAPPER_CONSTRUCTOR_SETCLASS( dog )

JSWRAPPER_CONSTRUCTOR_END

JSWRAPPER_FUNCTION( testFunc )

    Dog *dog=(Dog *) JSWRAPPER_FUNCTION_GETCLASS
    // --- Do Something

JSWRAPPER_FUNCTION_END

JSWRAPPER_GETPROPERTY( GetDogProperty_name )

    Dog *dog=(Dog *) JSWRAPPER_PROPERTY_GETCLASS

    JSWrapperData data; data.setString( dog->m_name );
    JSWRAPPER_GETPROPERTY_SETRC( data )

JSWRAPPER_GETPROPERTY_END

JSWRAPPER_SETPROPERTY( SetDogProperty_name )
    // --- JSWrapperData data; is already defined

    Dog *dog=(Dog *) JSWRAPPER_PROPERTY_GETCLASS
    dog->m_name=data.toString();

JSWRAPPER_SETPROPERTY_END

int main(int argc, char** argv)
{
    JSWrapper jsWrapper( argv[0] );
    if ( !jsWrapper.m_isValid ) {
        printf("Unable to Initialize JavaScript Wrapper.\n");
        return 1;
    }

    JSWrapperObject *global=jsWrapper.globalObject();

    // --- Register Class

    JSWrapperClass *dogClass=global->createClass( "Dog", DogConstructor );
    dogClass->registerFunction( "testFunc", testFunc );
    dogClass->registerProperty( "name", GetDogProperty_name, SetDogProperty_name );
    dogClass->install();

    jsWrapper.execute( "var dog=new Dog(); dog.name=\"Snickers\"; dog.testFunc();" );

    delete global;

    return 0;
}