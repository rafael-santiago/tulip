# Tulip

## What is this?

``Tulip`` is a software for guitar tablatures ``typesetting``. This can run on ``Linux``, ``FreeBSD`` and ``Windows``.

This software works based on a ``DSL`` designed taking in consideration my own preferences and necessities on this subject.

You can follow reading the [user's manual](https://github.com/rafael-santiago/tulip/blob/master/doc/MANUAL.md) in order to known more about.

## How to clone this repo?

Use the ordinary ``git-clone`` command:

```
emmettbrown@DELOREAN:~/internet/github.com/src# git clone https://github.com/rafael-santiago/tulip tulip
```

This repository has some submodules so after clonning it is important to initialize them in the following way:

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
emmettbrown@DELOREAN:~/internet/github.com/src/tulip/src# _
```

Yes, the ``Tulip``'s binary should be inside ``../bin`` sub-directory and you should visit the sub-directory [``doc``](https://github.com/rafael-santiago/tulip/tree/master/doc) before
starting.

## Can I install this software using its own build scripts?

Yes, you can. For doing this you should call ``hefesto`` passing the option ``--install`` (being under ``src`` sub-directory,
of course).

Actually the following commands are what you should execute. Being you inside the root project's directory:

```
emmetbrown@DELOREAN:~/internet/github.com/src/tulip# cd src
emmetbrown@DELOREAN:~/internet/github.com/src/tulip/src# hefesto --install
(just follow the simple steps answering the questions or canceling them to abort the process)
(...)
```

For uninstalling, you should do almost the same. You should pass the ``--uninstall`` option instead of the previous one.
