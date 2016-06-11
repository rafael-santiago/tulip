# Tulip user's manual

----

**Abstract**: ``Tulip`` is an application which implements a ``Language`` and some ``processors`` in order to generate
``Guitar`` tablatures. This document shows the main aspects of the implemented ``Language`` and also aspects that involves
the output production a.k.a the ``processors``. Here you can find something about my motivations on writing this
software. If you are in a rush, I suggest you jump to "The Tulip system" section.

----

## My motivations

I have been playing (or at least trying) the guitar since my twelve years old, during these years I had only two teachers.
When I started my studies the ``Standard Music Notation`` was unknown to me. The curious thing about these two teachers
was the choice for the same alternative music notation based on numbers. I said "curious" because since I learned this I have
not met anyone abroad whom know this same "numeric notation".

In fact this kind of notation is quite old. In my formal studies, with my first teacher, I used a book from ``1966``
written by a man called ``Fernando Azevedo``. This book was rather known by people here called "seresteiros".
A kind of bohemians signers which produced a significant collection of Brasilian popular Songs. However, nowadays
everyone tends to take it a little bit old fashioned, specially when talking about harmonies. Even so I could learned
important things related with the basics of the guitar playing from that *papyrus* and even today I use these learnings.

I could say that the curious numeric notation is what I use the most. This numeric notation is handy and allows you
to transcribe your ideas to the paper very quickly. In my opinion when you are transcribing things, specially by ear,
a kind of notation that does not consume you on a *"pencil-time"* is rather useful because your ideas and mind need to flow
more freely.

Okay, you do not know this kind of notation, right? Until now...

### The numeric notation

As I said I could not be able to find anything about this in English or any other Language besides Portuguese. I am afraid
of this is used only by brazilians or portugueses but I am not so sure about (take a look in this [discussion on the Reddit](https://www.reddit.com/r/musictheory/comments/4hqic2/what_is_the_name_of_this_kind_of_notation/)).

This notation is the main idea behind ``Tulip's DSL`` so if you want to master ``Tulip`` you must master it before.

The numeric notation gives one number to each string. The ``first string`` is numbered as ``1``, the ``second`` as ``2``,
the ``third`` as ``3``, the ``fourth`` as ``4``, the ``fifth`` as ``5`` and the ``sixth`` (guess what?) as ``6``.

The another part of the notation is about combining the fret number with the string number. Opened strings are followed by ``0``.

So a note can be depicted as: ``<string-number><fret-number>``. These notes should be separated by a single ``dash``.
Piece of cake, right?

However, in the fret number case we have some particularities. Some people when hit the 10th fret just numbered it as
``10`` and others numbered it as ``00``, fret number eleven as ``01``, fiften as ``05`` and so on. I use this second one and
as an effect this is what ``Tulip`` implements.

When we have intervals or chords we should be written down using stacked numbers. A kind of this: "53/42/30/21/10".
The hammer-ons, pull-offs can be noted using a simple arc between the parts.

Let's take a look at the **Table 1** in order to review your new knowledges about a new simple notation that you just have been
learned ;)

**Table 1**: Notes from the fretboard to numbers.

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

Maybe the usage of ``00`` for tenth fret can confuse you at the beginning but with practice your brain will start thinking
in this weird way.

Beware that if you search the term ``Numeric cipher`` you will reach subjects dealing with ``Cryptography``.
I am saying it because I am hooked on this subject too but here I am translating freely from Portuguese what in this case means
``Cifra Numérica``... The term "Numeric Cipher" deals with ``Music`` here in my country.

### "The paper stack blues"

I like to transcribe/adapt songs as an exercise and for fun issues too. I am in this "business" since my
teenagerhood. As a result during these years I have been producing huge stacks of paper. One of my motivations to create
``Tulip`` (still in ``2005``) was an effort in order to stop with this mess. The thurth is that I did not stop pushing papers
to my endless little stack but I would say that I stopped with my sloppiness. Now I am stacking ``notebooks`` ;)

Even today I use pencil and paper and then I create a ``Tulip code`` for. This is my basic workflow on it. Truly
translating my drafts to the ``Tulip's DSL`` is pretty confortable besides the fact that I can add some versioning on the
produced code, managing the transcription improvements. If you transcribe/adapt things you should know that we are constantly
evoluting our work, because we are always learning and evoluting ourselves.

Contradicting myself, in the **Figure 1**, you can see a part of an adaptation from an ``Albert King``'s solo on a
napkin that I recently did these days.

**Figure 1**: A short solo transcription written down on a napkin.

![Sample of my mess direct from my desk](https://github.com/rafael-santiago/tulip/blob/master/etc/draft.jpg)

According to the **Figure 1** is possible to see that I like to use arrows to express bending cases and tilde
to vibrato cases and also the way how the chords are expressed in this kind of notation. However, on ``Tulip``
these things are a little bit more structured. Nevertheless, this is the base for the Language discussed here.

### Understanding my versioning

I use this software since ``2005`` when I had written its first version. Here in this code repository you have a
reincarnation of this idea. The main differences that I would highlight are some changes in the ``DSL``,
the new processors addition and a new user's manual. The code was re-written from the scratch too.

Since ``2005`` I had done two major improvements on this project. So the first version (``v1``) brought the basic Language
definition and its text output producing. After I added a tiny ``MIDI`` processor which was able to filter the main ideas from
an input song translating it as a ``Tulip code``. I called it ``v2``. What I could learned from ``v2`` is that parse ``MIDI``
files is painful. It is astonishing how people do not follow any standard... We have several sub-standards (depending on the
manufacturers) inside the main standard and it is such a mess. Now the current version is ``v3``. This version brings a
``postscript`` and ``PDF`` processor in addition to the standard ``plain-text`` processor. The ``MIDI`` processor is still
not integrated in ``v3``. Maybe ``v4``.

Then, here the main versioning idea is:

>We have got some important change? Yes, ``v(n + 1)``.

Here is a nice slogan:

>"Keep it simple stupid"

I must confess that I can see beauty on this way of thinking.

## The Tulip system

``Tulip`` is an idea/software which can be splitted and understood on a two main parts:

- The ``DSL`` that you use to express your ``Musical intentions`` (maybe previously transcribed using "numeric ciphering").
- The processor that you use to produce a ``tablature`` output from your ``"tlp notation"``.

### Transcribing songs using Tulip's DSL

For this part I am considering that you have a previous knowledge about the "numeric cipher" otherwise take a look
at the previous section called ``The numeric notation``. A previous knowledge about tablatures is essential too.

The presented ``DSL`` has strong influences from ``TeX/LaTeX`` specially when talking about recursion in order to compose
combined playing techniques. Do not worrying if you do not know anything about ``TeX/LaTeX`` or even ``typesetting``. It is not
needed here.

From now on, sometimes I will refer to the implemented ``DSL`` as ``tlp``.

Perhaps it will be read by people that have some experience with computer programming and by people that do not.
Regardless, I am sure that these both kinds of people have one aspect in common: some level of knowledge about music, specially
guitar playing. Thinking about this, I decided to introduce the ``DSL`` concepts showing real examples of some well-known songs.
I find be in touching with real world transcriptions, talking about ``Music`` will certainly deviate us from a terrible boredom.

All songs showed here is the way which I am currently playing. I am saying it because I like to use an ``alternate`` tunning
instead of the ``standard``. For the following code samples ``is important`` you considering that all is done in a
guitar tunned ``two steps down``. Why? I am crazy, just accept this.

>Last thing: all here is about approximations and adaptations. Do not expect note-by-note things. Let's free ourselves from
obligations and have some fun with.

The ``guitar`` is a fascinating musical instrument not because is loud or distorted, well, this sometimes can make the things
cooler. The most fascinating thing about the guitar is that this instrument is a range of sounds. A note is a note but it
depends on how you attack (if you attack) this note, if you slide it or hammer it on or pull it off. For instance, the approach
that you use to perform a bend can actually change your tone. Etc, etc. For this reason, I like to say that the guitar is a
musical instrument driven by mood ;)

Express this details is quite hard even using our natural language. However, on the guitar we got several techniques that
isolated do not have much sense, but ``when combined`` can produce interesting sounds. The ``tlp`` use ``tags`` in order to
indicate some isolated techniques. The nice thing about this tags is that we can put one inside another to produce these
techniques combinations. These recursions makes the implemented ``DSL`` very powerfull in terms of expressivity.

A ``tag`` is a word sequence with special meaning preceded by a single ``dash``. The **Table 2** lists the supported
tags until now.


**Table 2**: The tlp tags for guitar techniques.

|           **Tag**                   |           **Guitar technique**                 |
|:-----------------------------------:|:----------------------------------------------:|
|          ``.mute``                  |              Palm mute                         |
|          ``.letring``               |              Let ring                          |
|          ``.strum``                 |              Strum                             |
|          ``.tremolopicking``        |              Tremolo picking                   |
|          ``.vibratowbar``           |              Vibrato with bar                  |
|          ``.trill``                 |              Trill                             |


All ``tlp tag`` must be followed by a code listing. A code listing is a variable sequence enclosed by ``{ ... }``.
As said before, between the brackets we can have other tags, notes and separators (to be continued, hold on).
As a result our musical intentions coded.

There is another important thing that is expressed as a tag: chords/intervals. But this is not a specific guitar
technique. Inside a ``chord tag`` can exist only notes and single separators (to be continued, hold on). The
important thing to know by now is that the ``chord tag`` is represented by ``.chord``.

Separators are important things to link the notes and in ``Tulip`` we have several, take a look at the **Table 3**.

### Using Tulip's processors
