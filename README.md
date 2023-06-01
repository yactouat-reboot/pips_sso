---
date: "2023-05-30"
slug: "building-a-sso-in-cpp"
title: "Building a SSO in C++"
---

So, I want to learn C++, and I mean business. I somehow want to build my own SSO service for my apps, that will be a harsh but perfect introduction to that language. 

My development environment will be VS Code in Ubuntu (whether WSL 2 or the real thing).

First, let's dust off by reviewing some notions about SSO.

## what is SSO ?

- _Single Sign-On_ is an authentication scheme that allows a user to sign in to various related, yet independent, apps using a single ID and password.
- The 3 parties involved in an SSO scheme are:
    - the client (the user trying to authenticate)
    - the relying party (the service the client tries to access)
    - the identity provider (the service that knows the user's credentials and that can authenticate them)
- OpenID Connect (OIDC) is a common architecture for SSO systems, in these systems, the requested service is responsible for redirecting the user's browser to an OIDC provider.
- OIDC is based on _OAuth 2.0_, the OIDC workflow can be represented as follows:

    ```mermaid
    sequenceDiagram
        participant User
        participant Service
        participant OP as OpenID Connect Provider
        User-->>Service: Attempts to access an auth protected resource
        Service-->>User: Redirects to OP with an authentication request
        User-->>OP: Login
        OP-->>User: Asks for user consent to share specific data with the requested service
        User-->>OP: Gives consent
        OP-->>User: Redirects back to Service with an authorization code (as a URL query param)
        User-->>Service: Delivers authorization code
        Service-->>OP: Sends authorization code
        OP-->>Service: Sends ID and access tokens
        Service-->>OP: Sends access token (user info request)
        OP-->>Service: Sends user info (user info response)
    ```

- You will often find SSO implementations using _OpenID Connect_ (OIDC) in the wild.
- In OIDC, the access token is used to allow a given service in the system to perform certain actions on behalf of the user.

Alright, now that we have the broad lines, let's try to write our first lines of SSO code with C++. However, understanding some basic notions will be necessary before proceeding...

## What is C++ ?

C++ is a cross platforms language that implements OOP. Use-cases of this language are basically anything:
- games
- high performance/low latency applications (such as financial apps)
- server applications
- GUIs
- etc.

## What C++ pointers are

In C++, a pointer is a variable that holds the memory address of another variable. The basic flow of pointers works as follows:

```cpp
int x = 1
int* p; // this is the pointer, it's declared to point to an integer
p = &x; // assign the address of x to p, the `&` operator is used to get the address of the variable

std::cout << "value of x: " << x << std::endl;
std::cout << "address of x: " << &x << std::endl;
std::cout << "value of p: " << p << std::endl; // outputs the same address than &x
std::cout << "value of *p: " << *p << std::endl; // outputs the same value than the value of x, this is called "de referencing" the pointer
```

Pointers are used for:
- dynamic memory allocation
- data structures such as linked lists or trees, for instance
- function pointers
- etc.

For instance, dynamic memory allocation is necessary when you need to allocate memory for variables at runtime, such as arrays that can grow or shrink. Let's have a look =>

```cpp
# include <iostream>

int main() {

    int n;
    std::cout << "hey user, enter a number of your choosing";
    std::cin >> n;

    // allocate memory for n integers dynamically
    int* arrPointer = new int[n];

    // do something on each index of the array
    for(int i = 0; i < n; i++) {
        // ...
        arrPointer[i] = "some processing result";
    }

    /**
     * deallocate the memory when we're done with the array,
     * `delete[]` releases dynamic memory that was previously allocated with the `new[]` operator;
     * ! it's crucial to match `new[]` and `delete[]` properly; for instance if you allocate a single element with `new`, then you deallocate it with a single `delete`
     * once you have deallocated the memory on a given pointer, you can not re use that pointer later in your program
     * */
    delete[] arrPointer;

    // exiting the program
    return 0;
}
```

## What macros are

- In C++, a macro is a fragment of code which is given a name; you can define macros using the `#define` preprocessor directive; once a macro is defined, the compiler replaces all occurrences of the macro in the code with its definition during the pre processing stage (before the actual compilation).
- See an example of a macro usage =>

```cpp
#define PI 3.14159

int main() {
    double radius = 5.0;
    double area = PI * radius * radius; // PI will be replaced by the defined value here
    return 0;
}
```

- Macros can also take arguments =>

```cpp
#define PI 3.14159
#define SQUARE(x) ((x) * (x))

int main() {
    double radius = 5.0;
    double area = PI * SQUARE(radius);
    return 0;
}
```

- Macros just do simple text substitution, they don't respect scopes or data types. They are notoriously hard to debug. So they are not considered as a very good practice.

## Lambda functions

- Lambda functions provide a convenient way to define an anonymous function object right at the location where it is invoked or passed as an argument to another function. Their syntax is as follows:

    ```cpp
    [capture-list](parameter-list) -> return-type {
        function-body
    }
    ```

    - the _capture list_ is used to capture variables (whether by value or by reference) from the enclosing scope in which the lambda is defined
    - the _parameter list_ is the list of parameters that the lambda function takes
    - the _return type_ is optional, as it can be deduced from the function body
    - the _function body_ is the actual code that the lambda function will execute

- Here is an example of a lambda function:

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    int multiplier = 10;

    // Lambda function to multiply each element by `multiplier`, which is captured from the surrounding scope
    std::for_each(numbers.begin(), numbers.end(), [multiplier](int &n) {
        n *= multiplier;
    });

    for(int n : numbers) {
        std::cout << n << " ";
    }

    return 0;
}
```

## What we will use

- To build our solution, we will need some kind of way to send HTTP requests and process JSON responses without too much overhead.
- For this, we are going to use the Microsoft C++ REST SDK (_Casablanca_), you can find the repo [here](https://github.com/microsoft/cpprestsdk).
- To install this lib, you'll need to run => `sudo apt update && sudo apt upgrade -y && sudo apt-get install build-essential libcpprest-dev`
- also make sure that you have `openssl` installed on your machine
- Let's try to write [an HTTP client](./http_client.cpp) for starters.

## Build the HTTP client: we need CMake

- Since we are using an external library, we are in a position in which trying to build the app' without the relevant tooling would cause too much overhead: e.g. we need to link the C++ REST SDK in our build system. To do that, we are going to use _CMake_ to build our app'.
- CMake is a cross-platform, open-source build system generator. It is used to manage the build process of software projects in a compiler-independent manner. The build process can be quite complex, especially for large projects that include many source files, have many dependencies, or need to be built on different platforms or with different compilers.
- When you use CMake, you create one or more `CMakeLists.txt` files that describe how your project should be built. These files include information such as:
  - which source files to compile
  - which libraries to link
  - where to find dependencies
  - how to organize the output binaries
  - any special compilation flag or options that are needed
- Once you've created your `CMakeLists.txt` files, you run CMake to generate the actual build system.
- Here is an example of a `CMakeLists.txt` file:

    ```cmake
    cmake_minimum_required(VERSION 3.10)
    project(MyProject)

    set(CMAKE_CXX_STANDARD 17)

    add_executable(MyProject main.cpp)
    ```

    ... this:
    - sets the minimum version of CMake that is required to build the project
    - names the project
    - sets the C++ standard to C++17
    - adds an executable target called `MyProject` that is built from the `main.cpp` source file

    ... you would run then run CMake to generate the build system:

    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

    ... this:
    - creates a `build` directory to store the build artifacts
    - generates makes files within that directory
    - compiles the project

## use CMake to build our HTTP client

- `sudo apt install cmake`
- create the [`CMakeLists.txt`](./CMakeLists.txt) file
- `cmake -S . -B ./build`
  - this will output a lot of files in the `./build` directory that can safely be git ignored
- `make -C build`, this should automatically make the executable marked as such in the `./build` directory
- you can now call the client with `./HttpClient`

## creating a webservice in C++

As we are trying to build an SSO system, we need 3 parties:
- the client
- the requested service
- the identity provider

Until now, we have built a test HTTP client, now let's start to create a [test HTTP server](./http_server.cpp) that will provide an example of a requested service. In this file, we have learned how to process GET requests and extracts the path and query parameters from them.