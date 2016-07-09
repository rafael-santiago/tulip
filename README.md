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

## Using syntax highlighting

I am a ``mcedit`` user. Due to it by now only syntax highlighting for this editor is implemented and can be found under the
``sub-path etc/tulip.syntax``.

### Adding tulip's syntax highlighting to the mcedit

Copy the file ``etc/tulip.syntax`` to the mcedit's syntax directory (It depends on your editor installation details):

```
emmetbrown@DELOREAN:~/internet/github.com/src/tulip# cp etc/tulip.syntax /usr/share/mc/syntax/
```

After doing it you should tell to ``mcedit`` about your intentions of highlighting tulip's code. It should be done adding
a pattern into the ``mcedit's Syntax`` file (It depends on your editor installation details). For instance, my ``Linux distro``
brings it under the path ``/etc/mc/Syntax``. Follows what should be added to this editor's configuration file:

```
file ..\*\\.(tlp)$ Tulip\sscript
include tulip.syntax
```

Now everything using extension ``.tlp`` will be highlighted:

![mcedit-after-before](https://github.com/rafael-santiago/tulip/blob/master/etc/mcedit-high.gif)

Done!

### Adding tulip's syntax highlighting to the notepad++

For doing it we need to access the menu option ``Language | Define your language...`` and click on ``Export...`` button. Choose the file ``etc/TLP.xml``
under this repo's root. After it all files having ``.tlp`` extension will be colorized as follows:

![npp-after-before](https://github.com/rafael-santiago/tulip/blob/master/etc/npp-high.gif)

Done!
