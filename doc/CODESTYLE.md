# Code style

Try to read the code and adapt yourself.

I personally like some aspects present in ``Pike's style``. Take a look in it.

Anyway, these are the main things to highlight...

The indentation is done using 8 spaces.

The include guards use the entire path until the file extension. Please, do not use underscore to start this include guard.
The include guard should be started by the project name, in this case "TULIP". Yes, always capital letters.
Supposing that we got a header file under the path ``dsl/compiler/compiler.h``, the guard would be:

```
#ifndef TULIP_DSL_COMPILER_COMPILER_H
#define TULIP_DSL_COMPILER_COMPILER_H 1
```

Always define it as ``1``.

It is important to add the copyright disclaimer into the header and implementation files.

Try to initialize your variables even if this variable is being used (at least until now) just after inside a for, in order
to iterate over some thing:

```
    char *p = NULL;
    for (p = buf; *p != 0; p++) {
        //All right.
    }
```

Use ``0`` as string delimiter.

``function_name_should_be_in_this_way()`` but try to avoid excessive verbosity. Do not be a fop:

![the-macaroni-by-philip-dawe-1773](https://github.com/rafael-santiago/tulip/blob/master/etc/the-macaroni.jpg)

With the variables you should try the same.

``Free`` anything that you ``Alloc``.

Do not be afraid of pointers. This is an ``C`` project. If you are afraid of pointers because someone told you that they are
evil, take in consideration that undefined references inside someones mind about how to properly use pointers can be the real
problem.

Do not use things without "logic meaning" in logical expressions which tests if this thing is equals to ``1``. Look:

```
    int delta = 1;

    if (delta) {
        (...)
    }
```

It is better:

```
    int has_delta = 1;

    if (has_delta) {
        (...)
    }
```

or

```
    int delta = 1;

    if (delta == 1) {
        (...)
    }
```

The same for pointers:

```
    char *str = NULL;

    if (str) {
        (...)
    }
```

Better:

```
    char *str = NULL;

    if (str != NULL) {
        (...)
    }
```

Always use NULL to indicate the "nullity" of some memory address.

Do not reciclate a structure just because it has one memory slice that you can use to save your sloppiness. If really
necessary, creates a new one.

Public structures should be post-fixed with ``_ctx``. Generally, this would be implemented under the sub-module ``base/types.*``, ``base/ctx.*``.

For pointers, used for iterating over some thing use ``p`` or ending the variable with ``p``:

```
    tulip_single_note_ctx *np = NULL, *p = NULL, *curr_note_p = NULL;
```

You can use macros, however, write it and use it as a person with knowledge for using it.

Use parenthesis to exactly communicate your intentions about the expression.

Try to use ``ANSI C`` the ``Posix``'s part relevant for us. Maybe someone, somewhere, over some unknown rainbow wants
to compile it. Let's make life easier for these people as much as we can.

Use blank spaces to separate the things logically.

```
    int i = 0;
    for (i=0; i<10; i++) {
        (...)
    }
    if (i == 0) {
        (...)
    }
    while (i-- > 0) {
        (...)
    }
    if ((a & mask) == 0 && (b & mask) == 0
                   && (cc & mask) == 0 && (dd & mask) == 0) {
        (...)
    }
    c(r(a(p)));
```

For sure that you hate reading books with poor typesetting. The main reason for reading is getting some information.
What a nice thing when the text disposition guides you on it. Follows a better way:

```
    int i = 0;

    for (i = 0; i < 10; i++) {
        (...)
    }

    if (i == 0) {
        (...)
    }

    while (i-- > 0) {
        (...)
    }

    if ((a  & mask) == 0 && (b  & mask) == 0 &&
        (cc & mask) == 0 && (dd & mask) == 0) {
        (...)
    }
```

## Comments

This is important when it is important. The format should be:

```
    //  INFO(YourNameOrA.k.a): blah..
```

for warnings:

```
    //  WARN(YourNameOrA.k.a): blah..
```

for todo's:

```
    //  TODO(YourNameOrA.k.a): blah..
```

## Tests

You should write tests. However, try not be so pedantic. This is boring. For the tests writing [cutest](https://github.com/rafael-santiago/cutest) is being used.

If we are a little bit [fop](https://github.com/rafael-santiago/tulip/blob/master/etc/the-macaroni.jpg) maybe you can faint when see that all tests are being written inside ``tests/main.c``.
However, this is good to find and correct the issues without get bored. Another thing: this is not about to write a ``Software Illiad``.
Here the goal is to make a ``simple-sharp-useful-not bloated-software``. Maybe you should visit the site [suckless.org](http://suckless.org).

## Code constructions

### If / else

```
    if (<expr>) {
        (...)
    } [else {]
        (...)
    }]
```

Please, do not use:

```
    if (<expr>)
        (...)
    [else
        (...)
    ]
```


### While / Do...While

```
    while (<expr>) {
        (...)
    }
```

```
    do {
        (...)
    } while (<expr>);
```

Please, do not use:

```
    while (<expr>)
        (...)
```

### For

```
    for (i = 0; i < n; i++) {
        (...)
    }
```

Please, do not use:

```
    for (i = 0; i < n; ++i) [{
        (...)
    }]
```

This is bad too:

```
    for (i = 0; i < n; i++)
        (...);
```

Acceptable:

```
    for (i = 0; i < n; i);
```

or

```
    for (i = 0; i < n; i)
        ;
```

### Switches

```
    switch (s) {
        case 0:
            break;

        case 1:
            break;

        default:
            break;
    }
```

