---
date: "2023-05-30"
slug: "building-a-sso-in-cpp"
title: "Building a SSO in C++"
---

So, I want to learn C++, and I mean business. I somehow want to build my own SSO service for my apps, that will be a harsh but perfect introduction to that language. 

My development environment will be VS Code in Ubuntu.

First, let's dust off by reviewing some notions about SSO.

## what is SSO ?

- _Single Sign-On_ is an authentication scheme that allows a user to sign in to various related, yet independent, apps using a single ID and password.
- The 3 parties involved in an SSO scheme are:
    - the client (the user trying to authenticate)
    - the relying party (the service the client tries to access)
    - the identity provider (the service that knows the user's credentials and that can authenticate them)
- OIDC is based on _OAuth 2.0_, the OIDC workflow can be represented as follows:

    ```mermaid
    sequenceDiagram
        participant User
        participant Service
        participant OP as OpenID Connect Provider
        User-->>Service: Attempts to access an auth protected resource
        Service-->>User: Redirects to OP with an authentication request
        User-->>OP: Login
        OP-->>User: Asks for user consent to share specific data with the service
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

## What C++ pointers are

In C++, a pointer is a variable that holds the memory address of another variable. The basic flow of pointers works as follows:

```cpp
int x = 1
int* p; // this is the pointer, it's declared to point to an integer
p = &x; // assign the address of x to p, the `&` operator is used to get the address of the variable

std::cout << "value of x: " << x << std::endl;
std::cout << "address of x: " << &x << std::endl;
std::cout << "value of p: " << p << std::endl; // outputs the same address than x
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

## What we will use

- To build our solution, we will need some kind of way to send HTTP requests and process JSON responses without too much overhead.
- For this, we are going to use the Microsoft C++ REST SDK (_Casablanca_), you can find the repo [here](https://github.com/microsoft/cpprestsdk).
- To install this lib, you'll need to run => `sudo apt update && sudo apt upgrade -y && apt-get install build-essential libcpprest-dev`.
- Let's try to write[an HTTP client](./http_client.cpp) for starters.