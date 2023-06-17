
# CogMech

Cogmech is an experimental programming language.

## To install

The language is currently tested on MacOS and Debian 11. It likely doesn't work on Windows without some tinkering.

First, you'll need to install CMake and make on your system. CMake has installation instructions here: 
[https://cmake.org/install/](https://cmake.org/install/).

Note that on MacOS you also have the option of installing with Homebrew:

```bash
brew install cmake
```

And it's also available within most Linux package managers.

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

```
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

## Examples

The code examples found in this tutorial can be found in the `cogm/exanmples/` directory.

## Entities & Atoms

The basic objects within the language are referred to as *entities*. The simplest entities are known as *atoms*, and are
specified by enclosing a name for the atom in quotes. In our `hello.cogm` example above, `"Hello World"` is not actually
a string of characters as it might appear to be, but the representation of a single atom.

## Printing Output

Entities on their own are *expressions*, but are not valid statements in the language. So the following program will 
give an error:

```
"Hello, World"
```

The simplest statement is the print statement. A print statement is defined by beginning a line with a colon `:`, 
followed by an expression, as in our `hello.cogm` example:

```
: "Hello, World"
```

## Comments

We can add line comments by adding the sequence `//`. Once that sequence is encoutnered, all text until the end of the 
line is considered part fo the comment and will be ignored by the interpreter.

```
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

```
sym := <"some", structure">
: sym
```

Symbols may must begin with an upper or lowercase letters and can continue with letters, numerals, and underscores `_`.
By convention, symbols are `snake_cased`.

## Extraction

There are a small number of structural operations we can perform on entities built into the language. The first is called
*extract*.

Very often we will use componds structures in a way that is variously termed an *associative array*, *dictionary*,
*map*, *hash*, etc. In other language. This is essentially a set of key-value pairs.

```
ex_map :=
  <
    <"key1", "value1">,
    <"key2", "value2">
  >
```

In order to read one of the values from `ex_map` we can use the extract operation. We use subscripting (array-indexing, etc.)
syntax with square brackets to read the value with `"key1"`.

```
: ex_map["key1"] // prints: "value1"
```

We can also use a symbol as the key:

```
ex_key := "key2"
: ex_map[ex_key] // prints: "value2"
```

When a key doesn't exist, *extract* will return a special `null` entity:

```
: ex_map["nonexistent_key"] // prints: null
```

## Transclusion

The operation *transclude* is the opposite of *extract*. Given an entity, a key, and a value, it will either update
the existing pair that contains the key with the value, or if none is found it will create a new pair.

The operation is represented by the letter `T` and takes argmuents in the format `T(subject, key, value)`.

```
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

```
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

```
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

```
: %(                [],                 [*] )  // true
: %(            "cake",                 [*] )  // null
: %( <"chocolate", []>,  <"chocolate", [*]> )  // true
```

## Dot

The *dot* operator recursively traverses an entity and replaces all instances of `[]` with `[*]`. For example:

```
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

```
: %(0, 0)  // true
: %(1, 0)  // true
: %(0, 1)  // null
: %(1, 1)  // true
: %(20, 1) // true
```

If you need to match an integer exactly, you can prefix it with the dot operator `*`:

```
: %(0, *0)  // true
: %(1, *0)  // null
: %(0, *1)  // null
: %(1, *1)  // true
: %(20, *1) // null
```

## Operation Definitions

You can define certain arrangements of operations as your own custom operations. Here are a couple examples:

```
dessert(flavor, type) ->
  <
    <"flavor", flavor>,
    <"dessert_type", type>,
    <"eaten", null>
  >

EatDessert(dessert) -> T(dessert, "eaten", true)

: dessert("chocolate", "cake")
: EatDessert(dessert("peach", "cobbler"))

// prints:
// <
//   <"flavor", "chocolate">,
//   <"dessert_type", "cake">,
//   <"eaten", null>
// >
// <
//   <"flavor", "peach">,
//   <"dessert_type", "cobbler">,
//   <"eaten", true>
// >
```

Operations may must begin with an upper or lowercase letters and can continue with letters, numerals, and underscores `_`.
By convention, operations that only create a compoound entity are `snake_cased`, while operations that apply
transformations to data are `PascalCased`.

## Relations

The primarily interesting point of the structure of the language is how its logic is enacted by a series of 
*relations*. A relation defines a conditionally applied transition between an entity that matches a certain *specifier*
and a transformed state.

Let's show an example:

```
uneaten_dessert := dessert([], [])
s_eat : uneaten_dessert => EatDessert(s_eat)
```

The parts of this statement are:

- `s_eat`, referred to as the *bound symbol*; this symbol represents the starting state of the entity that this relation is
  applied to.
- The *bound symbol* is followed by a colon `:` and another expression that is the *specifier*. The *specifier* represents 
  a pattern of a structure which an entity *must* match before this relation will be applied. This expression can be 
  either a symbol or a literal representation of some entity.
- The *specifier* is followed by a double arrow `=>` and then some *result expression* that represents the transformed 
  state after applying the relation. In the scope of the *result expression* the *bound symbol* refers to the initial
  entity the relation has been applied to.

## Evaluation

Relations are not applied to an entity until they are *evaluated*. Evaluation is signified by an operation `R`, which 
takes a single argument that is the entity to apply any relevant relations to.  From our example:

```
vanilla_pudding := dessert("vanilla", "pudding")

: "Vanilla pudding that sits around gets eaten"
: R(vanilla_pudding)

// prints:
// "Vanilla pudding that sits around gets eaten"
// <
//   <"flavor", "vanilla">,
//   <"dessert_type", "pudding">,
//   <"eaten", true>
// >
```

Note that although `vanilla_pudding` is not exactly the same as `dessert`, it does *match* `dessert`, and therefore
the relation is applied. In this alternative example, the relation will not be applied because the `"dessert_type"` is
missing.

```
snack(flavor, type, eaten) ->
  <
    <"flavor", flavor>,
    <"snack_type", type>,
    <"eaten", eaten>
  >

bbq_chips := snack("bbq", "chips", null)

: "We aren't eating snacks right now, only desserts"
: R(bbq_chips)

// no transformation occurs, prints:
// "We aren't eating snacks right now, only desserts"
// <
//   <"flavor", "bbq">,
//   <"snack_type", "chips">,
//   <"eaten", null>
// >
```

## Evaluation Order

When an evaluation is run, it does not stop after the first match. It continues to step through all applied relations 
until no further matching relations are found.

For example, if we create another relation that will convert all eaten desserts to a `"dessert_type"` of `"eaten"`:

```
eaten_dessert := T(dessert([], []), "eaten", true)
s_eaten : eaten_dessert => T(s_eaten, "dessert_type", "eaten")

: "Eaten desserts lose their type, but not their flavor"
: R(vanilla_pudding)

// prints:
// "Eaten desserts lose their type, but not their flavor"
// <
//   <"flavor", "vanilla">,
//   <"dessert_type", "eaten">,
//   <"eaten", true>
// >
```

**NOTE:** It is possible to create relations that result in circular evaluations. The behavior of these circumstances is 
currently undefined, and could possibly result in an infinite loop in your program.

## Including Files

There is an `@include` directive that can be used to include other cogm files. This behaves basically as if you 
simply inserted the source of the other file into your own.

```
@include "path/to/somefile.cogm"
```

## Debugging

There is also a `@trace` directive that will enable traces through the evaluation process. This can be helpful in 
debugging relations, but their current iteration could use improvement. The directive is specified as follows:

```
@trace 1 // The 1 indicates the trace level.
```

Currently, the three levels are:

- **0**: No tracing
- **1**: Essential trace logging
- **2**: Slightly more detiled trace logging

## Macros

The language also enables you to define macros with the doubke-colon `::` operator. A macro acts as a literal token 
substitution, so that everying from the beginning of the definition to the end of the line will be inserted into the list 
of evaluation tokens.

This can be useful for trying eperimental syntax (see `cogm/lib/alt.cogm`), and can also be useful in other areas.

Keep in mind the value of a macro does not need to be a valid expression, and may contain any list of tokens.

Here's an example that replaces the word `to` with the `=>` operator.

```
to :: =>
s : "begin" to "end" // this is now alternative syntax for a relation definition

: R("begin") // prints "end"
```

Sometimes the result of an operation or relation is complex and could be simplifed by etracting out a list of tokens,
but the tokens contain words that refer to parameters or bound symbols that are not defined in the outer context.

```
some_structure(first, second) -> <first, second>
op_result :: some_structure(a, b) // Notice `a` and `b` are not defined in this context

Op(a, b) -> op_result
: Op(1, 2) // prints: <1, 2>
```

## "Practical" Examples

Look to `cogm/lib/math.cogm` to see examples that perform arithmetical oeprations.

## The Book

The language is based on the system outlined in the book *Cognitive Mechanics*. Reading the book is not necessary, but 
if you are interested it is available below:

- [Free PDF](https://www.cognitivemechanics.org/_files/ugd/23a803_3b740cc5152f4924ae9144f20c4cd958.pdf?index=true)
- [Paperback](https://www.amazon.com/Cognitive-Mechanics-Operations-Ryan-Williams/dp/B09W79N2LG)



