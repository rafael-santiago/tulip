# Tulip

## What is this?

``Tulip`` is a software for guitar tablatures ``typesetting``. This can run on ``Linux``, ``FreeBSD`` and ``Windows``.

This software works based on a ``DSL`` designed taking into consideration my own preferences and necessities on this subject.

You can follow reading the [user's manual](https://github.com/rafael-santiago/tulip/blob/master/doc/MANUAL.md) in order to know more about.

Until now you will be able to generate tablature outputs based on ``ASCII``, ``Postscript``, ``Encapsulated Postscript``, ``PDF``, ``Markdown``, ``SVG``, ``HTML/ASCII``, ``HTML/SVG``, ``JPEG``, ``PNG`` and ``MOBI`` (Kindle). You can
check an output sample in ``Postscript`` [here](https://github.com/rafael-santiago/tulip/blob/master/etc/tulip-output-sample.gif?raw=true).

## How can I clone this repo?

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

## How can I build this software?

This software uses [Hefesto](https://github.com/rafael-santiago/hefesto) as its build system, so after following the steps
to put ``Hefesto`` to work on your system you should move to the ``src`` sub-directory and invoke ``Hefesto`` from there.
Something like this:

```
emmmetbrown@DELOREAN:~/internet/github.com/src/tulip# cd src
emmettbrown@DELOREAN:~/internet/github.com/src/tulip/src# hefesto
(...)
emmettbrown@DELOREAN:~/internet/github.com/src/tulip/src# ls ../bin
tulip*
emmettbrown@DELOREAN:~/internet/github.com/src/tulip/src# echo EUREKA
EUREKA
*** system TIP: You should read the documentation before returning naked and ashamed from the outdoors.
emmettbrown@DELOREAN:~/internet/github.com/src/tulip/src# _
```

Yes, the ``Tulip``'s binary should be inside ``../bin`` sub-directory and you should visit the sub-directory [``doc``](https://github.com/rafael-santiago/tulip/tree/master/doc) before
starting.

### Okay, but I am not so habituated with software building...

Maybe people without experience in software development but still wanting to use this software can arrive here. If you do not
have an environment well-prepared for building the software (e.g.: git and GCC well-installed) you can use the pre-builts
located under the subdirectory ``prebuilts``. In this case, you will get only the executable file, the exportation of its
executable is up to you. The **Table 1** brings the ``MD5`` hashes of these packages after downloading the file which
fits to your environment is a good practice to check if your copy has the same related hash according to the **Table 1**.

**Table 1**: Pre-builts MD5 hashes.

|                **File**              |   **Platform**      |                    **MD5 hash**               |
|:------------------------------------:|:-------------------:|:---------------------------------------------:|
| [``prebuilt/tulip-bin-linux.zip``](https://github.com/rafael-santiago/tulip/raw/master/prebuilt/tulip-bin-linux.zip) | ``linux`` | ``b53e9922be61a346da00099998d61607`` |
| [``prebuilt/tulip-bin-freebsd.zip``](https://github.com/rafael-santiago/tulip/raw/master/prebuilt/tulip-bin-freebsd.zip) | ``freebsd`` | ``cb980ca497c5f67e3c0a3701b880039f`` |
| [``prebuilt/tulip-bin-windows.zip``](https://github.com/rafael-santiago/tulip/raw/master/prebuilt/tulip-bin-windows.zip) | ``windows`` | ``2ed188fd0a6be735327a287037e8c97e`` |

All executables listed in the **Table 1** are ``32-bit`` based.

After downloading the correspondent file you need to unzip it into a specific directory. Maybe you should create a new directory
for doing it. However, ``Linux`` and ``FreeBSD`` users should unzip it to the ``local/bin`` directory (or equivalent) and for
them no additional actions are necessary.

Now, for ``Windows`` users: once the executable unzipped into some place on your computer filesystem, you need to "export" this
executable, in order to be able to call this program from everywhere. Under ``Windows`` it should be done adding to
the ``PATH`` environment variable the directory path where the ``tulip.exe`` is laying on. Tip: if you do not know
how to do it, try to search for "how to set the PATH variable in Windows". Once it done, if you re-open a command prompt
the ``tulip.exe`` probably will be accessible.

## Can I install this software using its own build scripts?

Yes, you can. For doing it you should call ``hefesto`` passing the option ``--install`` (being under ``src`` sub-directory,
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

I am a ``mcedit`` user. Due to it a syntax highlighting configuration file for this editor can be found under the
``sub-path etc/tulip.syntax``.

### Adding tulip's syntax highlighting to the mcedit

Copy the file ``etc/tulip.syntax`` to the mcedit's syntax directory (It depends on your editor installation details):

```
emmetbrown@DELOREAN:~/internet/github.com/src/tulip# cp etc/tulip.syntax /usr/share/mc/syntax/
```

After doing it you should tell to ``mcedit`` about your intentions of highlighting tulip's code. It should be done adding
a pattern into the ``mcedit's Syntax`` file (It depends on your editor installation details). My ``Linux distro``
brings it under the path ``/etc/mc/Syntax``. Follows what should be added to this editor's configuration file:

```
file ..\*\\.(tlp)$ Tulip\sscript
include tulip.syntax
```

Now everything using extension ``.tlp`` will be highlighted:

![mcedit-after-before](https://github.com/rafael-santiago/tulip/blob/master/etc/mcedit-high.gif)

Done!

### Adding tulip's syntax highlighting to the notepad++

For doing it we need to access the menu option ``Language | Define your language...`` and click on ``Export...`` button.
Now choose the file ``etc/TLP.xml`` under this repo's root. After doing it all files having ``.tlp`` extension will be
colorized as follows:

![npp-after-before](https://github.com/rafael-santiago/tulip/blob/master/etc/npp-high.gif)

Done!
