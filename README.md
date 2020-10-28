# dbj-fwk
Non Ambitious Windows (Server Side) App FWK

&copy; 2020 by dbj@dbj.org

> Purpose is to encapsulate and decouple SEH handling

## Usage

This is VS static lib project. Currently it is to be used from console apps. 
To use it you just need to provide implementation of one function (and link the lib of course):

```cpp
// user code starts here
// this is called from dbj*fwk
// lib where SE handling  and 
// minidump creation is implemented
extern "C" int program (int argc , char ** argv ) 
{
  return 42;
}
```
Yes, that might seem as a bad idea; `main()` is inside this library. But read the features bellow.

You still have the total control, but only of your program. What comes before is the infrastructure you do need *and* you do not want to get involved with.

`nanoclib.h` does have some usefull macros and things inside; all optional. Not mandatory.


## Features 

- Windows code only
- on SE raised and caught **anywhere** this fwk will
	- catch the SE
		- do the `__finally` block
		- create the minidump on the local machine
		- inform the user where is this miniidump and how is it called
- Visual Studio (2019) opens and uses the minidump files
	- part of that experience is VS native debugging, allowing you to pinpoint the exact issue
	- In case you missed it: 
    	- SEH is not C++ exceptions
    	- SEH is present in any Windows executable
    	- SEH catches **everything** : stack abuse, division with zero and a such
        	- there are no C++ exceptions for that
    	- SEH works with C code too
    	- And "the 'gasp!' moment"
    		- MS STL is raising SE when 
		`_HAS_EXCEPTIONS == 0`
		- yes you think that right,
		in that scenario:
    		- MS STL is not using C++ exceptions
    		- `try`/`throw`/`catch` are forbiden 
    		- In that scenario MS STL is **not** ISO standard 
- DBJ_FWK_DISPLAY_INFO if defined, provokes guess what ;)

## The Roadmap

- Non Console WIN32 executables
  - Services
  - COM servers
  - WIN32 Apps
- logging is very important
  - decouple the logging component so that users might use their own

Following is undecided as it compromises the stability 

- user definable SEH finally callbacks
- user definable SEH catch callbacks

## PS

Please respect the [license](.LICENSE.MD)


[Serverside Systems](https://github.com/dbj-data) is part of [dbj.systems](.https://dbj.systems) group.

![serverside](./media/serversidelogo.png)
