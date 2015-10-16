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

The wrapper directory needs to be in the include path. Add the source files to the project which end with _sm.cpp or _v8.cpp depending on which engine you use (plus the ones which dont have a specific ending).

##Features

The wrapper supports the most common features an embedder would need. 

* You can access JavaScript variables and functions
* Expose your C++ functions and classes to JavaScript
* Access Typed Arrays (or arrays). 

The wrapper was designed to be simple and small but flexible. If the wrapper is missing a feature you need, just let me know. The wrapper is always kept up to date to the latest versions of SpiderMonkey and v8 as it is used in Visual Graphics, a JavaScript based cross platform application platform.

##How it works

###Hello World

```c++

#include "jswrapper.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    JSWrapper jsWrapper( argv[0] );
        if ( !jsWrapper.isValid() ) {
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
        JSWrapperData data=args[i];

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
    if ( !jsWrapper.isValid() ) {
        printf("Unable to Initialize JavaScript Wrapper.\n");
        return 1;
    }

    JSWrapperObject *global=jsWrapper.globalObject();
    global->registerFunction( "print", print );

    jsWrapper.execute( print( "'Hello' + ', World!'" ) );
    delete global;
}

```

The JSWRAPPER_FUNCTION macro is used to define a function (can also be a class member function). It creates an JSWrapperArguments object which holds all arguments passed to the function. Using a macro allows us to implement JSWRAPPER_FUNCTION differently for each JavaScript engine, it has to be followed by a JSWRAPPER_FUNCTION_END macro which cleans things up.

Not used here, but also defined in the JSWRAPPER_FUNCTION is JSWrapperObject *thisObject, an object indicating the current scope the function is running in.

The print function parses the arguments passed to it from JavaScript and prints them out. You can set the return value of a function by passing a data class to JSWRAPPER_FUNCTION_SETRC().

###Register a C++ Class

```c++

class Dog
{
    public:    
    Dog( void ) { m_name="Unnamed"; }
    std::string m_name;
};

JSWRAPPER_CONSTRUCTOR( DogConstructor, "Dog" )
    // --- JSWrapperArguments args; is already defined

    Dog *dog=new Dog();
    JSWRAPPER_CONSTRUCTOR_SETCLASS( dog )

JSWRAPPER_CONSTRUCTOR_END

JSWRAPPER_FUNCTION( testFunc )

    Dog *dog=(Dog *) JSWRAPPER_FUNCTION_GETCLASS
    // Do something

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
    if ( !jsWrapper.isValid() ) {
        printf("Unable to Initialize JavaScript Wrapper.\n");
        return 1;
    }

    JSWrapperObject *global=jsWrapper.globalObject();

    JSWrapperClass *dogClass=global->createClass( "Dog", DogConstructor );
    dogClass->registerFunction( "testFunc", testFunc );
    dogClass->registerProperty( "name", GetDogProperty_name, SetDogProperty_name );
    dogClass->install();

    jsWrapper.execute( "var dog=new Dog(); dog.name=\"Snickers\"; print( dog.name ); dog.testFunc(); " );

    delete global;
    delete dogClass;
}

```

This example registers a Dog class together with a name property and a member functions called testFunc.

The first step is to register the name of the class together with the Constructor function into the object you want to register the class into, in this case the global object. The constructor function allocates the C++ class and associates it with the JavaScript class:

```c++
JSWrapperClass *dogClass=global->createClass( "Dog", DogConstructor );
```

Note that the constructor function uses the JSWRAPPER_CONSTRUCTOR macro which takes two arguments, the name of the function plus the name of the class it creates. It has to be followed by an JSWRAPPER_CONSTRUCTOR_END macro.

The definition of the testFunc member function is similar to the print function, to get a pointer to the C++ class use the JSWRAPPER_FUNCTION_GETCLASS macro. Receiving arguments and setting the return value works the same as in normal functions, i.e. see the print function. You register it via

```c++
dogClass->registerFunction( "testFunc", testFunc );
```

The get and set properties have their own set of macros, this was necessary as v8 and SM use different function arguments. Registration of the get and set function for get name properties is done like this:

```c++
dogClass->registerProperty( "name", GetDogProperty_name, SetDogProperty_name );
```

Once you have defined all functions and properties for the class you need to install the class:

```c++
dogClass->install();
```

After this the class is installed in JavaScript and ready to use as shown in the example.

###Notes

JSWrapperObjects, which are embedded inside a JSWrapperData and can be accessed via data.object(), get deleted together with the data object. If an object is passed via a function argument for example and you want to keep it for later use, you need to create a copy:

```c++

JSWrapperObject *myPersistentObject;

JSWRAPPER_FUNCTION( myFunction )
    // --- JSWrapperArguments args; is already defined
    // --- Argument 0 is an object we need to access later
    
    myPersistentObject=args[0].object()-copy();
```

myPersistentObject is protected, i.e. garbage collectors will not touch it until you delete the object. This is very useful for callback and objects which work async on the C++ side, like a calback function for an async network request.

###Todos

* Make JSWrapper multi-instance enabled for users who want to have several script contexts in their applications.
* Support debugging features of the Engines.

###Contact

For feature requests or questions please contact me at markusm@visualgraphics.tv.