# Tulip

## What is this?

``Tulip`` is a software for guitar tablatures ``typesetting``. This can run on ``Linux``, ``FreeBSD`` and ``Windows``.

This software works based on a ``DSL`` designed taking in consideration my own preferences and necessities on this subject.

You can follow reading the [user's manual](https://github.com/rafael-santiago/blob/master/doc/MANUAL.md) in order to known more about.

## How to clone this repo?

Use the ordinary ``git-clone`` command:

```
emmettbrown@DELOREAN:~/internet/github.com/src# git clone https://github.com/rafael-santiago/tulip tulip
```

This repository has some submodule so after clonning it is important to initialize them in the following way:

```
emmettbrown@DELOREAN:~/internet/github.com/src# cd tulip
emmettbrown@DELOREAN:~/internet/github.com/src/tulip# git submodule update --init
```

All done!

## How to build this software?

This software uses [Hefesto](https://github.com/rafael-santiago/hefesto) as its build system, so after following the steps
to put ``Hefesto`` to work on your system you should move to the ``src`` sub-directory and invoke ``Hefesto`` from there.
Something like this:

```
emmmetbrown@DELOREAN:~/internet/github.com/src/tulip# cd src
emmettbrown@DELOREAN:~/internet/github.com/src/tulip/src# hefesto
emmettbrown@DELOREAN:~/internet/github.com/src/tulip/src# ls ../bin
tulip*
emmettbrown@DELOREAN:~/internet/github.com/src/tulip/src# echo EUREKA
EUREKA
*** system TIP: You should read the documentation before returning naked and ashamed from the outdoors.
emmettbrown@DELOREAN:~/internet/github.com/src/tulip/src#
```

Yes, the ``Tulip``'s binary should be inside ``../bin`` sub-directory and you should visit the sub-directory ``doc`` before
starting.
