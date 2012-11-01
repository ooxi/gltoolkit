gltoolkit
=========

Minimal toolkit to extract .po files from [GetLocalization.com](https://www.getlocalization.com/)
as part of a [CMake](http://www.cmake.org/) build process, or as standalone
task.

[![Build Status](https://secure.travis-ci.org/ooxi/gltoolkit.png)](http://travis-ci.org/ooxi/gltoolkit)


Building gltoolkit
------------------

Since gltoolkit uses CMake, building the standalone executable is fairly easy

    $ git clone https://github.com/ooxi/gltoolkit.git gltoolkit
    $ mkdir gltoolkit/build; cd gltoolkit
    $ git submodule init; git submodule update; cd build
    $ cmake -DCMAKE_BUILD_TYPE=Release -DCURL_STATICLIB=ON ..
    $ make && ./test-gltoolkit

If you need a debug build, specify `CMAKE_BUILD_TYPE` as `Debug` and rebuild.


GetLocalization.com API
-----------------------

Detailed information about the Get Localization API is available at the
[Get Localization Help Desk](http://support.getlocalization.com/entries/136183-api/).

This library uses only two different Webservice calls, both member of the
[frozen REST API](http://support.getlocalization.com/entries/20016086-get-localization-rest-api):


### Retrieve list of product languages

    http://www.getlocalization.com/api/languages/?product=<product>&type=xml

> If type is `xml`, the output data is in following XML format

```xml
<Languages>
    <Language>
        <Name>German</Name>
        <IanaCode>de</IanaCode>
    </Language>
    <Language>
        <Name>Russian</Name>
        <IanaCode>ru</IanaCode>
    </Language>
</Languages>
```


### Retrieve translations in XML

    http://www.getlocalization.com/api/localized_strings/<product>/<iana-code>/

> The data output in XML format, the list of all information of a master string

```xml
<GLStrings>
    <product>violetland</product>

    <GLString>
        <MasterString>Please wait...</MasterString>
        <LogicalString></LogicalString>
        <ContextInfo>../src/program.cpp:183 ../src/program.cpp:346</ContextInfo>
        <Translation>Bitte warten...</Translation>
    </GLString>
    <GLString>
        <MasterString>Try to survive as long as you can.</MasterString>
        <LogicalString></LogicalString>
        <ContextInfo>../src/program.cpp:205</ContextInfo>
        <Translation>Versuche so lange wie möglich zu überleben.</Translation>
    </GLString>
<GLStrings>
```

