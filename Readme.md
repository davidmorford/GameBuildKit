
# BuildKit for Games

BuildKit for Games is an experimental project that brings BuildKit, NavigationKit and Cocos2D into a single structure designed to share static libraries and resources with multiple game applications. Please see the readme for BuildKit to get started.

[BuildKit Readme](http://github.com/davidmorford/BuildKit/blob/master/Readme.md)

## Requirement Notes

* BuildKit, and by extension BuildKit for Games, uses the LLVM/Clang 1.5 compiler and flags to enable modern runtime ABI features. This will certainly cause issues if you libraries that use C++ or Objective-C++. To change this behaviour  to LLVM GCC 4.2 or GCC 4.2, change the value of GCC_VERSION and comment out OTHER_CFLAGS in the master xcconfig file for projects in $(ROOT)/Configurations/Project.xcconfig

* BuildKit for Games libraries, projects and templates will most likely target the unified SDK, iOS 4.2, which is still in beta and under NDA.


## Cocos2D and Chipmunk Libraries

* Cocos2D and Chipmunk static library projects based on cocos2d-iphone-0.99.5-beta3 sources pulled from github project.

* The only major change to libraries is making the headers BuildKit compatible so one can do the following:

** #import <Cocos2D/Cocos2D.h>
** #import <Chipmunk/Chipmunk.h>

* Cocos2D library headers have not yet been converted to <Cocos2D/*.h> style yet but will be very, very, very soon.


## BuildKit for Games Xcode Project Template

The template, based on the NavigatorKit universal application Xcode project template, is not setup for a universal-based iOS game application yet. Stay tuned for updates…

As with BuildKit, BuildKit for Games has an out of the box Xcode project template. The template uses NavigatorKit in hopes of using URL-based application architecture design to build a cohesive set of application URLs and URL mapped objects, scenes and actions. This template includes project references to the Cocos2D, Chipmunk and NavigatorKit projects as a convenience that rebuilds changed library projects. BuildKit uses xcconfig based setting paths for headers and libraries and for linker paths.

 
## History

### October 3, 2010

* Create from latest NavigatorKit project structure and include Cocos2D, Chipmunk and minimal Xcode project template.


## License and Copyright

BuildKit for Games is BSD licensed. The full text of the GameBuildKit license and licenses for the multiple included libraries are located in Documents/Licenses.md
