# JSEngineWrapper

##A simple to use JavaScript Engine Wrapper for v8 and SpiderMonkey under the MIT license

JavaScript is a great language and there are several, fast JIT based JavaScript engines under heavy development. Things are very dynamic with complex APIs being changed at whim. The reasons for me to create a C++ wrapper were:

* To be able to choose the right engine for the given platform. Is one engine not yet available on Windows 10 or does not compile right now under iOS ? No problem, just switch to another one.

* Speed. The engines are in heavy competition regarding speed, just switch to the one which is fastest right now for a given task.

* API changes. If APIs change (and they do a lot) just take a new version of this wrapper and thats it. You dont have to change your entire source code.

* The API of this wrapper is straight-forward and easy to remember.

##How to build

You need to include the basic "jswrapper.hpp" file. This file will include all relevant headers depending if either __JSWRAPPER_SM or __JSWRAPPER_V8 is set. So if you want to work with v8, do

```c++
#define __JSWRAPPER_V8
#include "jswrapper.hpp
```

The wrapper directory needs to be in the include path. Add the source files to the project which end with _sm.cpp or _v8.cpp depending on which engine you use (plus the ones who dont have a specific ending).

##Features

The wrapper supports a lot of features. You can access JavaScript variables and functions, expose your C++ functions and classes to JavaScript as well as access typed arrays (or arrays). The wrapper was designed to be simple and small but flexible enough to allow most of the functionality an embedder would need. If the wrapper is missing a feature you need, just let me know.

##How it works

###Hello World

```c++

#include "jswrapper.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    JSWrapper jsWrapper( argv[0] );
        if ( !jsWrapper.m_isValid ) {
        printf("Unable to Initialize JavaScript Wrapper.\n");
        return 1;
    }

    JSWrapperData data;

    jsWrapper.execute( "'Hello' + ', World!'", &data );
    std::cout << data.toString() << std::endl;
}
```

After initializing the wrapper we define a JSWrapperData class, this class is used to hold any kind of JavaScript based data. Than we execute the hello world script, the result is written to the data structure and read out.

###Accessing JavaScript Objects

```c++
jsWrapper.execute( "var e=13; var a={ b : function( value ) { return value * 2; } };" );

JSWrapperObject *global=jsWrapper.globalObject(); // Get the global object

JSWrapperData eData, aData;
global->get( "e", &eData );
global->get( "a", &aData );

std::cout << eData.toNumber() << std::endl;

// Get the b function from the global a class

if ( aData.isObject() ) {
    JSWrapperData bFuncData, rcData;
    
    aData.object()-get( "b", &bFuncData );

    JSWrapperArguments args; // Create the arguments to pass to b
    args.append( 23.1 );

    bFuncData->call( &args, global, &rcData );
    std::cout << rcData.toNumber() << std::endl; // 46.2
}
delete global; // Cleanup

```

Here we read global variables from the JavaScript context and call a function inside a JavaScript object. Calling a JavaScript function also expects an object to define the context the function should work in, in this case we used the global object.

###Accessing Arrays


```c++
jsWrapper.execute( "var a=[ 1, 2, 3, 4 ];" );

JSWrapperData aData;
aData.object()-get( "a", &aData );

if ( aData.isObject() && aData.object()->isArray() ) {

    int length; 
    aData.object()->getArrayLength( &length );
    for ( int i=0; i < length; i < length ) {
        JSWrapperData data;
        aData.object()->getArrayElement( i, &data );
        // --- Do Something with the array element
    }
}
```

###Accessing Typed Arrays

You can check if an object is a typed array via isTypedArray() and can get the length and data via

```c++
void getAsFloat32Array( float **ptr, unsigned int *length );
void getAsUint8Array( unsigned char **ptr, unsigned int *length );
```

etc.

###Register a C++ function

```c++

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

    //JSWrapperData data; data.setNumber( 10 ); Just as an example
    //JSWRAPPER_FUNCTION_SETRC( data )

JSWRAPPER_FUNCTION_END


int main(int argc, char** argv)
{
    JSWrapper jsWrapper( argv[0] );
    if ( !jsWrapper.m_isValid ) {
        printf("Unable to Initialize JavaScript Wrapper.\n");
        return 1;
    }

    JSWrapperObject *global=jsWrapper.globalObject();
    global->registerFunction( "print", print );

    jsWrapper.execute( print( "'Hello' + ', World!'" ) );
    delete global;
}

```



