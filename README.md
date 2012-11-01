gltoolkit
=========

Minimal toolkit to extract .po files from getlocalization.com


[GetLocalization.com API](https://www.getlocalization.com/)
-----------------------------------------------------------

Detailed information about the Get Localization API is available at the
[Get Localization Help Desk](http://support.getlocalization.com/entries/136183-api/).

This library uses only two different Webservice calls, both member of the
[frozen REST API](http://support.getlocalization.com/entries/20016086-get-localization-rest-api):


### Retrieve list of product languages

    http://www.getlocalization.com/api/languages/?product=<product>&type=xml

> If type is `xml`, the output data is in following XML format

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


### Retrieve translations in XML

    http://www.getlocalization.com/api/localized_strings/<product>/<iana-code>/

> The data output in XML format, the list of all information of a master string

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

























