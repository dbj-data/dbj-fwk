# dbj-fwk
Non Ambitious Windows Server Side App FWK

&copy; 2020 by dbj@dbj.org

> Purpose is to encapsulate and decouple SEH handling

## Usage

This is VS static lib project. Currently it is to be used from console apps. 
To use it you just need to provide implementation of one function (and link the lib of course):

```cpp
// user code starts here
// this is called from dbj*fwk
// framework is where SE handling  and 
// minidump creation is implemented
extern "C" int program (int argc , char ** argv ) 
{
  return 42;
}
```
Yes, that might seem as a bad idea; `main()` is inside this library. But read the features bellow.

And you still have the total control, but only of your program. 
What comes before is infrastructure you do need but you do not want to get involved with.

`nanoclib.h` does have some usefull macros and things inside, witch optionaly you can use. Not mandatory.


## Features 

- Windows code only
- on SE raised and caught **anywhere** this will
		- catch the SE
		- do the __finally block
		- create the minidump on the local machine
		- inform the console app user where is this miniidump and how is it called
- Visual Studio (2019) can open and use the minidump files
		- part of that is VS native debugging, allowing you to pinpoint the exact issue
		- In case you missed it: 
		- SEH is not C++ exceptions
		- SEG is present and works always in any Windows executable
		- SEH catches **everything**: stack abuse, division with zero and a such
		- SEH work with C code too

## The Roadmap

- Non Console WIN32 executables
		- Services
				- COM servers
		- WIN32 Apps
- logging
		- very important
		- decouple the logging component so that users might use their own

Following is undecided as it compromises the stability 

		- user definable finally block callbacks
		- user definable catch block callbacks

## PS

Please respect the [license](.LICENSE.MD)
