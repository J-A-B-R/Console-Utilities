Console Utilities
=================

Console Utilities are a set of lightweight Win32 console programs written in pure C that offer funtionality and console management not found on Windows.

For detailed operation, check the in-command help with option `/?`.
The help included on each command is in english and spanish. If the Windows locale is a spanish variant, it'll show the spanish help, otherwise it'll show the english help.


Installation
------------
You can download or clone the project and compile it for yourself or you can download a precompiled bundle. Whatever the method you choose,
put the executables in a directory included in the PATH environment variable.

###Bundles:
* [Console Utilities v1.0.1 32 bits](https://rapidshare.com/files/2846596904/Console-Utilities_x86_1.0.1.zip)

* [Console Utilities v1.0.1 64 bits](https://rapidshare.com/files/2824059969/Console-Utilities_x64_1.0.1.zip)


Commands included so far
------------------------

- **`CCOLOR`**: Gets or sets the current foreground and background console colors.

- **`EECHO`**: Displays a message enhanced with escaped characters with several functions.

- **`ELEV`**: If the UAC is enabled, launchs a program elevated, but using the existing console, instead of spawning a new one.

	Inpired by [The Code Project](http://www.codeproject.com/) article [Vista UAC: The Definitive Guide](http://www.codeproject.com/Articles/19165/Vista-UAC-The-Definitive-Guide).


Commands on roadmap
-------------------
- **`COLORTABLE`**: Changes the console color palette with new colors.

- **`NSCRB`**: Launchs a program in a new screen buffer on the same console and reverts to the previous screen buffer on exiting.

- New escaped characters and improvements to **`EECHO`**.


Personal background
-------------------
I started this little project as a fun summer entertainment to detox of .NET, OO, lambdas, ORMs, repository patterns, async programming, SOLID concepts and whatnot.

So here I am, back to basics facing the arid Win32 API that I've almost forgotten, armed only with the ancient but beloved C89 and willing to learn one thing or two.


History
-------
### 1.0
&ensp;Initial release.


License
-------
Copyright 2013 Juan Burgos.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
