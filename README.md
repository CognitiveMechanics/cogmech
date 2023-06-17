
# CogMech

Cogmech is an experimental programming language.

## To install

First, you'll need to install CMake and make on your system. CMake has installation instructions here: 
[https://cmake.org/install/](https://cmake.org/install/).

Note that on MacOS you also have the option of installing with Homebrew:

```bash
brew install cmake
```

Then in the root directory, you can run:

```bash
cmake .
cmake --build .
sudo make install
```

If all worked correctly you should see usage of the command line tool by running:

```bash
cogmech
```

## Hello World

Now to create our first program. Create a file called `hello.cogm` with the following contents:

```cogm
: "Hello, World"
```

We can run the file with the command:

```bash
cogmech exec hello.cogm
```

When run, you should get the following output:

```
"Hello, World"
```

## Entities & Atoms

The basic objects within the language are referred to as *entities*. The simplest entities are known as *atoms*, and are
specified by enclosing a name for the atom in quotes. In our `hello.cogm` example above, `"Hello World"` is not actually
a string of characters as it might appear to be, but the representation of a single atom.

## Printing Output

Entities on their own are *expressions*, but are not valid statements in the language. So the following program will 
give an error:

```cogm
"Hello, World"
```

The simplest statement is the print statement. A print statement is defined by beginning a line with a colon `:`, 
followed by an expression, as in our `hello.cogm` example:

```cogm
: "Hello, World"
```

## Comments

We can add line comments by adding the sequence `//`. Once that sequence is encoutnered, all text until the end of the 
line is considered part fo the comment and will be ignored by the interpreter.

```cogm
// This is a comment on its own line.
: "We're testing out comments" // This is a comment after a statement 
```

## Integers

The language also has builtin support for simple integer entities. These are positive only and are specified by their 
base-10 representation.

```
: 0 // prints: 0
: 100 // prints: 100
```

## Compound Entities

*Compound entities* are structures that are made up of other entities. They enclosed in angle brackets `<` and `>`. 
`<"dark", "chocolate">`, for example. 

We say a compund entity is *composed of* its *components* (child entities).

A compound entity can be made up of 1 or more components. For instance, `<"bitter", "dark", "chocolate">`, is also
a valid compound entity.

The order of the components does not matter. `<"dark", "chocolate">` and `<"chocolate", "dark">` have the exact same 
meaning. They can also be nested as we will see below.

## Symbols

In CogMech all entities are immutable, so there are no variables. But you can assign an entity to a *symbol* with
the following syntax.

```cogm
sym := <"some", structure">
: sym
```

## Extraction

There are a small number of structural operations we can perform on entities built into the language. The first is called
*extract*.

Very often we will use componds structures in a way that is variously termed an *associative array*, *dictionary*,
*map*, *hash*, etc. In other language. This is essentially a set of key-value pairs.

```c++
ex_map :=
  <
    <"key1", "value1">,
    <"key2", "value2">
  >
```

In order to read one of the values from `ex_map` we can use the extract operation. We use subscripting (array-indexing, etc.)
syntax with square brackets to read the value with `"key1"`.

```c++
: ex_map["key1"] // prints: "value1"
```

We can also use a symbol as the key:

```
ex_key := "key2"
: ex_map[ex_key] // prints: "value2"
```

When a key doesn't exist, *extract* will return a special `null` entity:

```cogm
: ex_map["nonexistent_key"] // prints: null
```

## Transclusion

The operation *transclude* is the opposite of *extract*. Given an entity, a key, and a value, it will either update
the existing pair that contains the key with the value, or if none is found it will create a new pair.

The operation is represented by the letter `T` and takes argmuents in the format `T(subject, key, value)`.

```cogm
ex_map_1 := T(ex_map, "key1", "updated_value") 

// ex_map_1 is:
// <
//   <"key1", "updated_value">,
//   <"key2", "value2">
// >

// ex_map is unchanged!


ex_map_2 := T(ex_map_1, "key3", "new_value")

// ex_map_2 is:
// <
//   <"key1", "updated_value">,
//   <"key2", "value2">,
//   <"key3", "new_value">
// >

```

## Matching

The language also includes an operation *match* to determine if two entities have similar structure.

The operation is denotes by `%` and takes arguments in the format `%(match, against)`. 

Match returns a special entity `true` if the first argument `match` contains equivalent structural pieces to `against`,
otherwise it returns `null`. In other words, `against` must have a structure that is a subset of `match`.

Illustrated by example:

```cogm
: %(                       "cake",                        "cake" )  // true
: %(                       "cake",                   "chocolate" )  // null
: %(        <"chocolate", "cake">,         <"chocolate", "cake"> )  // true
: %(        <"chocolate", "cake">,         <"cake", "chocolate"> )  // true
: %(        <"chocolate", "cake">,  <"big", "chocolate", "cake"> )  // null
: %( <"big", "chocolate", "cake">,         <"chocolate", "cake"> )  // true
```

## Proxy

Sometimes, we are looking to match a particular structure without actually caring about the specific entity that is
in the structure. In that case, we can use the *proxy*, represented by exmpty square brackets `[]`. Any entity
that is matches against the proxy will return `true`.

```cogm
: %(                "cake",                [] )  // true
: %(                "cake",                [] )  // true
: %(                    [],                [] )  // true
: %( <"chocolate", "cake">,                [] )  // true
: %( <"chocolate", "cake">,              <[]> )  // true
: %( <"chocolate", "cake">,          <[], []> )  // true
: %( <"chocolate", "cake">, <"chocolate", []> )  // true
: %( <"chocolate", "cake">,    <"cherry", []> )  // null
```

## Dot Proxy

Sometimes we want to match the proxy specifically, and only the proxy. For that purpose, we can use the *dot proxy*,
represented by `[*]`. The dot proxy also matches itself.

```cogm
: %(                [],                 [*] )  // true
: %(            "cake",                 [*] )  // null
: %( <"chocolate", []>,  <"chocolate", [*]> )  // true
```

## Dot

The *dot* operator recursively traverses an entity and replaces all instances of `[]` with `[*]`. For example:

```cogm
box := 
  <
    <"contents", []>
  >

: *box

// prints: 
// <
//  <"contents", [*]>
// >
```

This is useful for when you need to match an entity *exactly*, including proxies.

## Integer Comparisons

An integer matches another integer if it is equal to or greater than that integer. For example:

```cogm
: %(0, 0)  // true
: %(1, 0)  // true
: %(0, 1)  // null
: %(1, 1)  // true
: %(20, 1) // true
```

If you need to match an integer exactly, you can prefix it with the dot operator `*`:

```cogm
: %(0, *0)  // true
: %(1, *0)  // null
: %(0, *1)  // null
: %(1, *1)  // true
: %(20, *1) // null
```

## The Book

The language is based on the system outlined in the book *Cognitive Mechanics*. Reading the book is not necessary, but 
if you are interested it is available below:

- [Free PDF](https://www.cognitivemechanics.org/_files/ugd/23a803_3b740cc5152f4924ae9144f20c4cd958.pdf?index=true)
- [Paperback](https://www.amazon.com/Cognitive-Mechanics-Operations-Ryan-Williams/dp/B09W79N2LG)



