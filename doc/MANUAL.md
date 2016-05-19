# Tulip user's manual

----

**Abstract**: ``Tulip`` is an application which implements a ``Language`` and some ``processors`` in order to generate
``Guitar`` tablatures. This document shows the main aspects of the implemented ``Language`` and also aspects that envolves
the output production a.k.a the ``processors``. Here you can find something about my motivations on writing this
software. If you are in a rush try to jump to "The Tulip system" section.

----

## My motivations

I have been playing (or at least trying) guitar since my twelve years old, during these years I had only two teachers.
When I started my studies the ``Standard Music Notation`` was unknown to me. The curious thing about these two teachers
was the choice of the same alternative music notation based in numbers. I said "curious" because since I learned this I have
not met anyone abroad whom know this same "numeric notation".

In fact this kind of notation is quite old. In my formal studies, with my first teacher, I used a book from ``1966``
written by a man called ``Fernando Azevedo``. This book was rather known by people here called "seresteiros",
a kind of bohemians signers which produced a significant collection of Brasilians popular Songs, however, nowadays
everyone tends to take it as a little bit old fashioned, specially when talking about harmonies. Even so I could learned
important things related with the basics of the guitar playing from this *papyrus* and even today I use these learnings.

I could say that the curious numeric notation is what I use the most. This numeric notation is handy and allows you
to transcribe your ideas to the paper very quickly. In my opinion when you are transcribing things specially "by ear"
a kind of notation that does not consume you on a *"pencil-time"* is rather useful because your ideas and mind need to flow
more freely.

Okay, you do not know this kind of notation, right? Until now...

### The numeric notation

As I said I could not be able to find anything about this in English or any other Language besides Portuguese. I am afraid
of this is used only by brazilians or portugueses but I am not so sure about (take a look in this [discussion on the Reddit](https://www.reddit.com/r/musictheory/comments/4hqic2/what_is_the_name_of_this_kind_of_notation/)).

This notation is the main idea behind ``Tulip``'s ``DSL`` so if you want to master ``Tulip`` you must master it before.

The numeric notation gives one number to each string. The ``first string`` is numbered as ``1``, the ``second`` as ``2``,
the ``third`` as ``3``, the ``fourth`` as ``4``, the ``fifth`` as ``5`` and the ``sixth`` (guess what?) as ``6``.

The another part of the notation is about combining the fret number with the string number. Opened strings are followed by ``0``.

So a note can be depicted as: ``<string-number><fret-number>``. These notes should be separated by a single ``dash``.
Piece of cake, right?

However in the fret number case we have some particularities. Some people when hits the 10th fret just numbered it as
``10`` and others numbered it as ``00``, fret number eleven as ``01``, fiften as ``05`` and so on. I use this second one and
as a effect the ``Tulip`` implements this.

When we have intervals or chords we should write these numbers in a way of fractions. A kind of this: "53/42/30/21/10".
The hammer-ons, pull-offs can be noted using a simple arc between the parts.

Let's take a look at the **Table 1** in order to review your new knowledges about a new simple notation that you just have been
learned.

**Table 1**: Notes from the fretbord to numbers.

|          **Fretboard**               | **Numeric cipher** |
|:------------------------------------:|:------------------:|
|          1st string open             |         10         |
|          2st string open             |         20         |
|          3rd string open             |         30         |
|          4th string open             |         40         |
|          5th string open             |         50         |
|          6th string open             |         60         |
|          1st string fret 1           |         11         |
|          2st string fret 6           |         26         |
|          3rd string fret 10          |        300         |
|          4th string fret 15          |        405         |
|          5th string fret 20          |        510         |
|          6th string fret 13          |        603         |

Maybe the usage of ``00`` for fret ten can confuse you at the beginning but with practice your brain will start thinking
in this weird way.

Beware that if you search the term ``Numeric cipher`` you will reach subjects dealing with ``Cryptography``.
I am saying it because I am hooked on this too but here I am translating freely from Portuguese what in this case means
``Cifra Numérica``. The term "Numeric Cipher" deals with ``Music`` here in my country.

### "The paper stack blues"

I like to transcribe/adapt songs as exercise and for fun issues too. I am in this "bussiness" since my
teenagerhood. As a result during these years I produced huge stacks of paper. One of my motivations to create ``Tulip``
(still in ``2005``) was a effort in order to stop with this mess. The thurth is that I did not stop pushing papers to
my dirty huge stack but I would say that I stopped with my sloppiness. Today I am stacking ``notebooks`` ;)

Even today I use pencil and paper and after I create a ``Tulip code`` for this. This is my basic workflow on it. The fact is
that transcribe my drafts to ``Tulip``'s ``DSL`` is pretty confortable besides the fact that I can versioning it and control
the tablature improvements. If you transcribe/adapt things you should know that we are constantly evoluting your work.
Because we are always evoluting and learning.

Contradicting myself in the picture below you can see a part of an adaptation from an ``Albert King``'s solo on a
napkin that I did recently these days:

![Sample of my mess direct from my desk](https://github.com/rafael-santiago/tulip/blob/master/etc/draft.jpg)

As you can see I like to use arrows to express bending cases and the symbol ``~`` to vibrato cases and also the way how the
chords are expressed in this kind of notation. But in ``Tulip`` these things are a little bit more structured. Nevertheless
this is the base for the Language discussed here.

### Understanding my versioning

I use this software since ``2005`` when I had written its first version. Here in this code repository you have a
reincarnation of this idea. The main differences that I would highlight are some changes in the ``DSL``,
the processors addition and a new user's manual. The code was re-written from the scratch too.

Since ``2005`` I had done two major improvements on this project. So the first version (``v1``) brought the basic language
definition and its text output producing. After I added a tiny ``MIDI`` processor which was able to filter the main ideas from
a input song and translating it as a ``Tulip code``. I called it ``v2``. What I could learned from ``v2`` is that parse ``MIDI``
files is painful, people do not follow any standard, we have sub-standards inside the main standard and it is such a mess.
The current version is ``v3``. This version brings a ``postscript`` and ``PDF`` processor in addition to the standard
``plain-text`` processor. The ``MIDI`` processor is still not integrated in ``v3``. Maybe ``v4``.

Here the main idea of versioning is:

>We have got some important change? Yes, ``v(n + 1)``.

Here is a nice slogan:

>"Keep it simple stupid"

I must confess that I can see beauty in this way of thinking.

## The Tulip system

``Tulip`` is a idea/software which can be splitted and understood on a two main parts:

- The ``DSL`` that you use to express your ``Musical intentions`` (commonly previous transcribed using "numeric ciphering").
- The processor that you use to produce a ``tablature`` output from your ``"tlp notation"``.

### Transcribing songs using Tulip's DSL

For this part I am considering that you have a previous knowledge about the "numeric cipher" otherwise take a look
at the previous section called ``The numeric notation``. A previous knowledge about tablatures is essential too.

The presented ``DSL`` has strong influences from ``TeX/LaTeX`` specially when talking about recursion in order to compose
combined playing techniques. Do not worrying if you do not know anything about ``TeX/LaTeX`` or ``typesetting``. It is not
needed here.

From now on, sometimes I will refer to the implemented ``DSL`` as ``tlp``.

### Using Tulip's processors
