# 🪞 Concerto Reflection

**Modern C++ reflection for the Concerto engine – package generation, runtime metadata & method invocation**

Concerto Reflection is a library and code‑generation tool that brings runtime reflection to your C++20 projects. Annotate your classes, enums and methods with simple macros, run the package generator to produce reflection metadata, and then explore your types at runtime: query namespaces, inspect member variables, or invoke methods dynamically.

---

## 📋 Table of Contents

1. [Installation](#installation)
2. [Quick Start](#quick-start)
3. [Usage](#usage)
4. [Examples & Documentation](#examples--documentation)
5. [Contributing](#contributing)
6. [License](#license)
7. [Topics](#topics)

---

## 🛠️ Installation

### Prerequisites

* **Xmake** – Concerto Reflection uses the [Xmake build system](https://xmake.io/).  Install Xmake using your package manager or from the official website.


### Clone & configure

```bash
git clone https://github.com/ConcertoEngine/ConcertoReflection.git
cd ConcertoReflection
```

### Configure build options

Concerto Reflection exposes several build options through Xmake.  You can enable unit tests and assertions when configuring the build.  For example:

```bash
xmake config -m debug \
    --tests=y        # build unit tests
```

### Build & install

Build the library and generator by running:

```bash
xmake                # compile the reflection library and package generator
xmake install -o <install-prefix>  # install headers and binaries
```

The build produces two important targets:

* **`concerto-reflection`** – a shared library containing the runtime reflection API.
* **`concerto-pkg-generator`** – a command‑line tool that parses your C++ source files and generates package metadata.

---

## 🚀 Quick Start

Concerto Reflection works in two phases: **package generation** and **runtime usage**.

### 1 – Annotate your types

Use the provided macros to mark your package, classes, enums and members.  For example, here is a simple package with a class and an enum used in the tests:

```cpp
#include <Concerto/Reflection/Defines.hpp>
#include <Concerto/Core/Types.hpp>
#include <Concerto/Core/FunctionRef.hpp>

struct CCT_PACKAGE("version = \"1.0\"", "description = \"Sample Package description\"", "serialize = [\"JSON\", \"YML\"]") MyPackage {};

namespace my::sample {
    enum class CCT_ENUM() Colour : cct::UInt32 { Red = 0, Green = 1, Blue = 2 };

    class CCT_CLASS("Test=\"test\"") MyBar : public cct::refl::Object {
    public:
        MyBar() : m_value(42) {}

        CCT_METHOD()
        cct::refl::Int32 UltimateQuestionOfLife(cct::refl::Int32 v1, cct::refl::Int32 v2, cct::refl::Int32 v3)
        {
            return m_value;
        }

        CCT_OBJECT(MyBar);
    private:
        CCT_MEMBER()
        cct::refl::Int32 m_value;
    };
}
```

The macros expand to register metadata about your types, methods and members. Attributes can be attached as key‑value pairs to any macro.

### 2 – Generate a package

Run the package generator on your source files.  It uses Clang to parse the code and produce a pair of `.gen.hpp`/`.gen.cpp` files that contain the reflection metadata:

xmake example:
```lua
target("MyApp")
    set_kind("binary")
    add_files("Src/**.cpp")
    add_headerfiles("Include/*.hpp")
    add_deps("concerto-reflection")
    add_rules("cct_cpp_reflect")
```

The `cct_cpp_reflect` rule automatically runs the package generator during the build, passing the relevant source and header files to it. The generator then creates `MyAppPackage.gen.hpp` and `MyAppPackage.gen.cpp` in your build directory and adds them to your target. These generated files provide a `CreateMyAppPackage()` function.

### 3 – Load your package at runtime

At runtime, load the generated package into a `cct::refl::PackageLoader` and register it with the global namespace:

```cpp
#include <ConcertoReflectionPackage.gen.hpp>
#include <Concerto/Reflection/PackageLoader.hpp>

int main()
{
    cct::refl::PackageLoader loader;
    loader.AddPackage(CreateConcertoReflectionPackage());
    loader.AddPackage(CreateMyAppPackage());
    // If you have multiple packages, call AddPackage for each one
    loader.LoadPackages();

    // Now you can query classes, namespaces and invoke methods
}
```

### 4 – Discover and invoke

Once loaded, you can query classes, namespaces and call functions at runtime.  For example, to find a class by name and invoke one of its methods:

```cpp
int main()
{
    cct::refl::PackageLoader loader;
    loader.AddPackage(CreateConcertoReflectionPackage());
    // If you have multiple packages, call AddPackage for each one
    loader.LoadPackages();

    const cct::refl::Class* myBarClass = cct::refl::GetClassByName("my::sample::MyBar");
    if (myBarClass)
    {
        // Create a default instance of the class
        std::unique_ptr<cct::refl::Object> instance = myBarClass->CreateDefaultObject();
        cct::refl::Object* memberVariable = myBarClass->GetMemberVariable("m_value");
        // Retrieve a method by name
        const cct::refl::Method* AnswerMethod = myBarClass->GetMethod("UltimateQuestionOfLife");
        if (AnswerMethod && instance && memberVariable)
        {
            // Invoke the method.  Invoke returns a Result<T, std::string>
            // containing either the result or an error message.
            auto result = AnswerMethod->Invoke<cct::refl::Int32>(*instance, cct::refl::Int32{}, cct::refl::Int32{}, cct::refl::Int32{});
            if (result.IsOk())
            {
                cct::Logger::Log("The UltimateQuestionOfLife is 42? Answer: {}", result.GetValue().Equals(*memberVariable) ? "Yes" : "No");
            }
        }
    }
}
```

The tests demonstrate retrieving a class, finding a method and invoking it , expecting the return value `42`.

---

## 🎛️ Usage

Concerto Reflection exposes a rich runtime API.  Here are some of the key capabilities:

### Global namespace and package loader

* **Package Loader** – manage the lifetime of reflection packages.  Use `PackageLoader::AddPackage` to register a generated package and `LoadPackages` to initialize namespaces and classes.
* **Global Namespace** – access all loaded classes and namespaces through `GlobalNamespace::Get()`.  You can query namespaces by qualified name and iterate over classes.

### Classes

* Retrieve a class by fully qualified name using `GetClassByName`.
* Inspect class properties: `GetName`, `GetBaseClass`, `GetMemberVariableCount` and `GetMethodCount`.
* Create objects dynamically: `Class::CreateDefaultObject()` returns a `std::unique_ptr<Object>`.
* Check inheritance relationships with `InheritsFrom`.

### Member variables

* Access a member variable by index or name with `GetMemberVariable`.
* Query the type of a member, check whether a member exists (`HasMemberVariable`).
* Retrieve custom attributes attached to a member via `GetAttribute`.

### Methods

* Obtain a method by index or name using `GetMethod`.
* Check for the existence of a method with `HasMethod`.
* Invoke a method dynamically via `Method::Invoke<T>(Object&, Args...)`.  The return value is wrapped in a `Result<T, std::string>` which either contains the result or an error string.

### Attributes & metadata

* All reflection entities (packages, classes, enums, methods, members) can carry key–value attributes.  Use `HasAttribute` and `GetAttribute` to inspect them.
* Built‑in types such as `Int32` are also registered with the reflection system, allowing you to treat fundamental types like any other class.


### Template class support

Concerto Reflection supports C++ template classes with full reflection metadata for each specialization.

#### Annotating template classes

Mark template classes with the standard reflection macros:

```cpp
template<typename T>
class CCT_CLASS() Container : public cct::refl::Object
{
public:
    Container() : m_value() {}

    CCT_MEMBER()
    T m_value;

    CCT_METHOD()
    void SetValue(T val) { m_value = val; }

    CCT_METHOD()
    T GetValue() const { return m_value; }

    CCT_OBJECT(Container);
};

// Explicitly instantiate the specializations you want to reflect
template class Container<int>;
template class Container<double>;
```

#### Accessing template specializations at runtime

Once loaded, you can retrieve specializations in two ways:

**Method 1: Using the TemplateClass API**

```cpp
const auto* containerClass = cct::refl::GetClassByName("cct::sample::Container");
if (containerClass && containerClass->IsTemplateClass())
{
    auto* templateClass = dynamic_cast<cct::refl::TemplateClass*>(
        const_cast<cct::refl::Class*>(containerClass));

    // Get a specific specialization by type string
    auto* intSpec = templateClass->GetSpecialization("int"sv);
    if (intSpec)
    {
        auto obj = intSpec->CreateDefaultObject();
        // Use the specialized instance...
    }
}
```

**Method 2: Direct retrieval by full name**

```cpp
const auto* intSpec = cct::refl::GetClassByName("cct::sample::Container<int>");
if (intSpec)
{
    auto obj = intSpec->CreateDefaultObject();
    // Use the specialized instance...
}
```

#### Multi-parameter templates

Templates with multiple parameters are fully supported:

```cpp
template<typename K, typename V>
class CCT_CLASS() Pair : public cct::refl::Object
{
public:
    CCT_MEMBER()
    K m_key;

    CCT_MEMBER()
    V m_value;

    CCT_OBJECT(Pair);
};

// Explicitly instantiate specializations
template class Pair<int, double>;
template class Pair<std::string, int>;
```

Then retrieve them:

```cpp
const auto* pairSpec = cct::refl::GetClassByName("cct::sample::Pair<int,double>");
```

#### How it works

The package generator:
1. Detects template class declarations and their explicit instantiations
2. Generates a base `TemplateClass` that describes the template parameters
3. For each explicit instantiation, generates a specialization class with the concrete type information
4. Registers all specializations in the reflection namespace for runtime discovery

---

### Generic class support (runtime-parameterized types)

Concerto Reflection also supports **generic classes** – classes whose member variables store runtime type parameters. Unlike C++ templates which are resolved at compile-time, generic classes allow you to work with polymorphic type parameters at runtime without requiring explicit specializations.

#### Key differences: Templates vs Generics

| Aspect | Template Class | Generic Class |
|--------|---|---|
| **Type Resolution** | Compile-time (instantiation-based) | Runtime (parameter-based) |
| **Specializations** | Explicit instantiations required | Single definition works for all type arguments |
| **Type Safety** | Full compile-time checking | Type safety via `const Class*` pointers |
| **Cross-DLL Compatibility** | Each specialization needs code generation | Single definition works across DLLs |
| **Memory Model** | Separate binary code per specialization | Shared implementation, parameterized data |
| **Use Cases** | Known type combinations at build time | Dynamic, unknown type combinations at runtime |

#### Annotating generic classes

Mark a class as generic using `CCT_GENERIC_CLASS()` and annotate type parameter fields with `CCT_GENERIC_TYPE()`:

```cpp
class CCT_CLASS() CCT_GENERIC_CLASS() GenericContainer : public cct::refl::Object
{
public:
    GenericContainer() : m_elementType(nullptr) {}

    CCT_MEMBER()
    CCT_GENERIC_TYPE()
    const cct::refl::Class* m_elementType;

    CCT_METHOD()
    const cct::refl::Class* GetElementType() const
    {
        return m_elementType;
    }

    CCT_OBJECT(GenericContainer);
};
```

Key points:
- Type parameter fields **must** be of type `const cct::refl::Class*`
- Use `CCT_GENERIC_TYPE()` annotation to mark type parameters
- The macro automatically includes `CCT_MEMBER()` annotation
- Provide getter methods to access type parameters

#### Using generic classes at runtime

Generic classes are instantiated with type arguments passed to `CreateDefaultObject()`:

```cpp
const auto* containerClass = cct::refl::GetClassByName("cct::sample::GenericContainer");
if (containerClass && containerClass->IsGenericClass())
{
    auto* genericClass = dynamic_cast<cct::refl::GenericClass*>(
        const_cast<cct::refl::Class*>(containerClass));

    // Get a type to use as argument
    const auto* int32Class = cct::refl::GetClassByName("cct::refl::Int32");

    // Create instance with type argument(s)
    std::vector<const cct::refl::Class*> typeArgs = { int32Class };
    auto obj = genericClass->CreateDefaultObject(
        std::span<const cct::refl::Class*>(typeArgs));

    if (obj)
    {
        // Access the type parameter
        auto container = dynamic_cast<YourGenericContainer*>(obj.get());
        const auto* elementType = container->GetElementType();
        // Use the element type information...
    }
}
```

#### Multi-parameter generics

Generic classes can have multiple type parameters:

```cpp
class CCT_CLASS() CCT_GENERIC_CLASS() GenericPair : public cct::refl::Object
{
public:
    GenericPair() : m_keyType(nullptr), m_valueType(nullptr) {}

    CCT_MEMBER()
    CCT_GENERIC_TYPE()
    const cct::refl::Class* m_keyType;

    CCT_MEMBER()
    CCT_GENERIC_TYPE()
    const cct::refl::Class* m_valueType;

    CCT_METHOD()
    const cct::refl::Class* GetKeyType() const { return m_keyType; }

    CCT_METHOD()
    const cct::refl::Class* GetValueType() const { return m_valueType; }

    CCT_OBJECT(GenericPair);
};
```

Instantiate with multiple type arguments:

```cpp
const auto* int32Class = cct::refl::GetClassByName("cct::refl::Int32");
const auto* int64Class = cct::refl::GetClassByName("cct::refl::Int64");

std::vector<const cct::refl::Class*> typeArgs = { int32Class, int64Class };
auto obj = genericClass->CreateDefaultObject(
    std::span<const cct::refl::Class*>(typeArgs));
```

#### How it works

The package generator:
1. Detects `CCT_GENERIC_CLASS()` annotations on class declarations
2. Identifies type parameter fields via `CCT_GENERIC_TYPE()` annotations
3. Generates code that stores type parameters in member variables during instantiation
4. Provides `CreateDefaultObject(span<const Class*>)` to pass type arguments at runtime

---
---

## 📚 Examples & Documentation

The generated files in `Tests/` (`ConcertoReflectionPackage.gen.hpp`, `ConcertoReflectionTestsPackage.gen.hpp`) show what the output of the package generator looks like and can be used as a reference.

---

## 🤝 Contributing

We welcome contributions to Concerto Reflection!

1. **Fork** this repository.
2. **Create a feature branch**: `git checkout -b feat/my-feature`.
3. **Commit your changes**: `git commit -am "Add my feature"`.
4. **Push** your branch: `git push origin feat/my-feature`.
5. **Open a pull request**.

If you plan on submitting significant changes, please open an issue first to discuss your ideas.  Contributions should follow the coding style used in the existing source code and include appropriate tests.

---

## 📜 License

Concerto Reflection is released under the **MIT License**.  You are free to use, modify and distribute this software within the terms of the license.

---

## 🔖 Topics

`c++20`, `reflection`, `runtime-reflection`, `code-generation`, `clang`, `llvm`, `xmake`, `game-development`, `metadata`, `package-generator`, `open-source`, `mit-license`
