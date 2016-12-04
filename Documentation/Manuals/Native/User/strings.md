Strings 						{#strings}
===============
Strings are represented with @ref bs::String "String" and @ref bs::WString "WString" types. These are wrappers for the standard C++ strings and have the same interface and behaviour.

Use the **String** type for strings containing only ASCII characters (limited character set). Use the **WString** (wide string) for strings containing more complex characters, as it supports all Unicode characters.

~~~~~~~~~~~~~{.cpp}
String simple = "SimpleString";
WString complex = "Š¶¤ÞÐ";
~~~~~~~~~~~~~

# Converting data types
You can convert most primitive data types to strings by using the @ref bs::toString "toString" or @ref bs::toWString "toWString" functions.

~~~~~~~~~~~~~{.cpp}
bool v1 = false;
int v2 = 244;

String str1 = toString(v1);
String str2 = toString(v2);
~~~~~~~~~~~~~

You can also do an opposite conversion, converting from a string to a primitive data type by calling one of the *parse* functions.

~~~~~~~~~~~~~{.cpp}
String str1 = "false";
String str2 = "244";

bool v1 = parseBool(str1, false);
int v2 = parseINT32(str2, 0);
~~~~~~~~~~~~~

If the system cannot properly parse the string, it will instead assign the default value provided.

# Manipulating strings
Various forms of string manipulations can be performed via @ref bs::StringUtil "StringUtil", including but not limited to: making a string upper or lower case, replacing string elements, matching string elements, splitting strings based on delimiters and more.

~~~~~~~~~~~~~{.cpp}
String string = "124,355,banana,954";

// Split string into entries separated by ,
Vector<String> entries = StringUtil::split(string, ",");

// Replace all occurrences of "banana" within the string, with "643"
string = StringUtil::replaceAll(string, "banana", "643");
~~~~~~~~~~~~~

# Formatting strings
Often you need to construct larger strings from other strings. Use @ref bs::StringUtil::format "StringUtil::format" to construct such strings by providing a template string, which contains special identifiers for inserting other strings. The identifiers are represented like "{0}, {1}" in the source string, where the number represents the position of the parameter that will be used for replacing the identifier.

~~~~~~~~~~~~~{.cpp}
String templateStr = "Hello, my name is {0}.";
String str = StringUtil::format(templateStr, "Banshee");

// str now contains the string "Hello, my name is Banshee."
~~~~~~~~~~~~~