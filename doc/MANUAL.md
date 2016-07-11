# Tulip user's manual

----

**Abstract**: ``Tulip`` is an application which implements a ``Language`` and some ``processors`` in order to generate
``Guitar`` tablatures. This document shows the main aspects of the implemented ``Language`` and also aspects that involves
the output production a.k.a the ``processors``. Here you can find something about my motivations on writing this
software. If you are in a rush, I suggest you jump to "The Tulip system" section.

----

## My motivations

I have been playing (or at least trying) the guitar since my twelve years old, during these years I had only two teachers.
When I started my studies the ``Standard Music Notation (Sheet Music)`` was unknown to me. The curious thing about these two teachers
was the choice for the same alternative music notation based on numbers. I said "curious" because since I learned this I have
not met anyone abroad whom know this same "numeric notation".

In fact this kind of notation is quite old. In my formal studies, with my first teacher, I used a book from ``1966``
written by a man called ``Fernando Azevedo``. This book was rather known by people here called "seresteiros".
A kind of bohemians singers which produced a significant collection of popular Brazilian's Songs. However, nowadays
everyone tends to take it a little bit old fashioned, specially when talking about harmonies. Even so I could learned
important things related with the basics of the guitar playing from that *papyrus* and even today I use these learnings.

I could say that the curious numeric notation is what I use the most. This numeric notation is handy and allows you
to transcribe your ideas to the paper very quickly. In my opinion when you are transcribing things, specially by ear,
a kind of notation that does not consume you on a *"pencil-time"* is rather useful because your ideas and mind need to flow
more freely.

Okay, you do not know this kind of notation, right? Until now...

### The numeric notation

As I said I could not be able to find anything about this in English or any other Language besides Portuguese. I am afraid
of this is used only by brazilians or portugueses but I am not so sure about (take a look at this [discussion on the Reddit](https://www.reddit.com/r/musictheory/comments/4hqic2/what_is_the_name_of_this_kind_of_notation/)).

This notation is the main idea behind ``Tulip's DSL`` so if you want to master ``Tulip`` you must master this notation before.

The numeric notation gives one number to each string. The ``first string`` is numbered as ``1``, the ``second`` as ``2``,
the ``third`` as ``3``, the ``fourth`` as ``4``, the ``fifth`` as ``5`` and the ``sixth`` (guess what?) as ``6``.

The another part of the notation is about combining the fret number with the string number. Opened strings are followed by ``0``.

So a note can be depicted as: ``<string-number><fret-number>``. These notes should be separated by a single ``dash``.
Piece of cake, right?

However, in the fret number case we have some particularities. Some people when hit the 10th fret just numbered it as
``10`` and others numbered it as ``00``, fret number eleven as ``01``, fifteen as ``05`` and so on. I use this second one and
as an effect this is what ``Tulip`` implements.

When we have intervals or chords it should be written down using stacked numbers. A kind of this: "53/42/30/21/10".
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
``Tulip`` (still in ``2005``) was an effort in order to stop with this mess. The truth is that I did not stop pushing papers
to my endless little stack but I would say that I stopped with my sloppiness. Now I am stacking ``notebooks`` ;)

Even today I use pencil and paper and then I create a ``Tulip code`` for. This is my basic workflow on it. Truly, to be
translating my drafts into the ``Tulip's DSL`` is pretty confortable besides the fact that I can add some versioning on it,
managing the transcription improvements. If you transcribe/adapt things you should know that we are constantly evoluting our
work, because we are always learning and evoluting ourselves.

Contradicting myself, in the **Figure 1**, you can see a part of an adaptation from an ``Albert King``'s solo on a
napkin that I recently did these days.

**Figure 1**: A short solo transcription written down on a napkin.

![Sample of my mess direct from my desk](https://github.com/rafael-santiago/tulip/blob/master/etc/draft.jpg)

According to the **Figure 1** is possible to see that I like to use arrows to express bending cases and tilde
to vibrato cases and also the way how the chords are expressed in this kind of notation. However, on ``Tulip``
these things are a little bit more structured. Nevertheless, this is the base for the Language discussed here.

### Understanding my versioning

I use this software since ``2005`` when I had written its first version. Here in this code repository you have a
reincarnation of the main idea. The major differences that I would highlight are some changes in the ``DSL``,
the new processors addition and a new user's manual. The code was re-written from the scratch too but I will
not annoy you with ``Compilers Theory``. Take it easy, by now you are safe.

Since ``2005`` I had done two major improvements on this project. So the first version (``v1``) brought the basic Language
definition and its text output producing. After I added a tiny ``MIDI`` processor which was able to filter the main ideas from
an input song translating it as a ``Tulip code``. I called it ``v2``. What I could learned from ``v2`` is that parse ``MIDI``
files is painful. It is astonishing how people do not follow any standard... We have several sub-standards (depending on the
manufacturers) inside the main standard and it is such a mess. Now the current version is ``v3``. This version brings a
``postscript`` and ``PDF`` processor in addition to the standard ``plain-text`` processor. The ``MIDI`` processor is still
not integrated in ``v3``. Maybe ``v4``.

Then, here is the main versioning idea:

>We have got some important change? Yes, ``v(n + 1)``.

In addition, a nice slogan:

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
Regardless, I am sure that these both kinds of people have one aspect in common: some level of ``musical knowledge``, specially
``guitar playing``. Thinking about this, I decided to introduce the ``DSL`` concepts showing real examples of some well-known songs.
I find be in touch with real world transcriptions, talking about ``Music`` will certainly deviate us from a terrible boredom.

All songs shown here are in the way which I am currently playing. I am saying it because I like to use an ``alternate``
tunning instead of the ``standard``. For the following code samples ``it is important`` you considering that all is done in a
guitar tunned ``two steps down``. Why? I am crazy, just accept this.

>Last thing: all here is about approximations and adaptations. Do not expect note-by-note things. Let's free ourselves from
obligations and have some fun with. I invite you...

The ``guitar`` is a fascinating musical instrument not because is loud or distorted, well, this feature sometimes can make the things
cooler, exciting. The most fascinating thing about the guitar is how it can be a range of sounds. A note is a note but it
depends on how you attack (if you attack) this note, if you slide it or hammer it on or pull it off. For instance, the approach
that you use to perform a bend can actually change your tone. The way how you hold the neck to "blow up" a vibrato.
Still, you can make it soft and sweet instead of angry. Etc, etc. For this reason, I like to say that the guitar is a musical
instrument driven by mood. Maybe any musical instrument could be but string instruments are pretty organic, you really gives
your body (maybe your soul, ha-ha) on it. What say about your precious calluses?

Express this techniques details is quite hard even using our natural language. However, on the guitar we got several
techniques that isolated do not have much sense but ``when combined`` can produce interesting sounds. The ``tlp`` uses ``tags``
in order to indicate some isolated techniques. The nice thing about these tags is that we can put one inside another to
produce a technique combination. These recursions make the implemented ``DSL`` very powerful in terms of expressivity
besides being instinctive for any guitar player. Well, maybe you can be wondering about how much possible
combinations can be made. Actually, your own playing will guide you for getting this answer. What can I say is that I did
care about this feature for letting the song writing the most natural for us while musicians.

A ``tag`` is a word sequence with a special meaning. It is preceded by a single ``period``. The **Table 2** lists the supported
tags until now.


**Table 2**: The tlp tags for guitar techniques.

|           **Tag**                   |           **Guitar technique**                 |
|:-----------------------------------:|:----------------------------------------------:|
|          ``.mute``                  |              Palm mute                         |
|          ``.letring``               |              Let ring                          |
|          ``.strum``                 |              Strumming                         |
|          ``.tremolopicking``        |              Tremolo picking                   |
|          ``.vibratowbar``           |              Vibrato with bar                  |
|          ``.trill``                 |              Trill                             |


All ``tlp tag`` must be followed by a code listing. A code listing is a variable sequence enclosed by ``{ ... }``.
As said before, between the brackets we can have other tags, notes and separators (to be continued, hold on).
As a result, our musical intentions will follow coded inside.

There is another important thing that is expressed as a tag: ``chords`` and ``intervals``. But this is not a specific guitar
technique. Inside a ``chord tag`` can exist only notes and single separators (to be continued, hold on). The
important thing to know by now is that the ``chord tag`` is represented by ``.chord``.

Separators are important building blocks to link the notes and in ``Tulip`` we have several, take a look at the **Table 3**.

**Table 3**: The tlp note separators.

|          **Separator**                 |           **On the guitar it means**            |
|:--------------------------------------:|:-----------------------------------------------:|
|              ``-``                     |        A single pause between the notes         |
|              ``~``                     |        A vibrato using the current note(s)      |
|              ``/``                     |        A slide-down using the current note(s)   |
|              ``\``                     |        A slide-up using the current note(s)     |
|              ``h``                     |        A hammer-on using the current note(s)    |
|              ``p``                     |        A pull-off using the current note(s)     |
|              ``b``                     |        A bend-on using the current note(s)      |
|              ``r``                     |        A release bend using the current note(s) |
|              ``*``                     |    A natural harmonic using the current note(s) |
|              ``v``                     | An articial harmonic using the current notes(s) |
|              ``T``                     |        A tapping using the current note(s)      |

Now we have basic informations about how to produce a ``Tulip code``. What about start with a song written by a genius?
Then, for starting let's pick up one famous song by ``Jimi Hendrix`` called ``Purple Haze``. I will show you
little by little this song (in the way that I play) and at the end we will have a complete code for generating
our guitar tablature. During this time, I will introduce you some techniques and concepts behind the ``Tulip's DSL``.

Let's go...

The ``Purple Haze's`` introduction has a pretty interesting thing. Originally, the introduction is made by
``bass`` and ``guitar`` playing different things together. So, the guitar does this (ok, here goes our first code):

        69-401--69-401--69-401--69-401

and the guitar (if acting like a bass) should do it:

        500-302--500-302--500-302--500-302

or still:

        63-45--63-45--63-45--63-45

These layers of sound produce a "thing" called ``tritone``. ``Tritones`` are commonly referenced as a way to add
tension or "frightening ambiences" for the listeners. In fact, if you do only the guitar's part coded above, you will
not be able to achieve this "tension". Actually, it will be sound pretty unexpressive. So, what about combining these
two worlds?

        69-.chord{500-401}--69-.chord{500-401}--69-.chord{500-401}--69-.chord{500-401}-

Better? I think so... Remembering that everything is taking in consideration my personal preference for tunning
the guitar ``two steps down``.

Now the cool riff. You should wet it with a ``Fuzz`` if you have one...

    400h402-300-201-400~~~~~-46/.letring{48-58}-63-45~~-|-
    400h402-300-201-400~~~~~-46/.letring{48-58}-63-45~~-|-
    400h402-300-201-400~~~~~-46/.letring{48-58}-63-66~~-|-
    58-500-48-400/402-400~~~~-400~~-400/402-300~-300~-300h302~~---300h301p300-402\-

Yes, it is a little bit different from ``Jimi`` and a crap when compared with what he did...

The code shown above is using the ``separation bars`` or ``sep bars`` as I like to call. This is denoted
by the ``pipe`` symbol. It is useful to separate things in logical blocks inside the tablature. Resulting visually as
a vertical breaking in the ``tab diagram``.

Note that in the previous code we have a repetion in the second "bar" (forget about sheets here,
ok? no time divisions, it is just about tablatures). I hate using copy and paste, it is boring. In ``Tulip`` there is a smart
symbol which you use to say: "hey guy, here you will repeat the same that I did just before."

For doing it you should use the ``at`` symbol. So, the lazy version would be:

    400h402-300-201-400~~~~~-.letring{46/48-58}-63-45~~-|-@
    400h402-300-201-400~~~~~-.letring{46/48-58}-63-66~~-|-
    58-500-48-400/402-400~~~~-400~~-400/402-300~-300~-300h302~~---300h301p300-402\-

The ``@`` in ``Tulip`` is known as ``once more``. The ``once more`` uses some rules for trying to be more smart when talking
about understanding the user intentions about...

>"...WTF this user really wants to do once more here?!"

Follows the basic rules:

- If we have a single note, only this note will be repeated.
- If we have a tag, the tag and what this tag brings wrapped inside will be repeated.
- If we have a sep bar before the once more, the ``once more`` will repeat everything inside the last logical block that you created including the ``sep bar``. By the way, this is what is happening in the ``Purple Haze's`` main riff previously shown.

There is another way to indicate a repetition avoiding annoying ourselves with re-typing again and again. It is a special ``control tag``
called ``times``. The ``.times`` tag should receive inside its code listing a ``number of times`` what indicates the repetion of something.
However, the ``.times`` tag must co-working with a ``sep bar`` because alone it tends to produce non-sense/dubious indications
into the output tablature. Then, using ``.times`` for our last code sample it would be:

    400h402-300-201-400~~~~~-46/.letring{48-58}-63-45~~-.times{2}-|-
    400h402-300-201-400~~~~~-46/.letring{48-58}-63-66~~-|-
    58-500-48-400/402-400~~~~-400~~-400/402-300~-300~-300h302~~---300h301p300-402\-

As a result we will get a kind of indication saying "2X" near from the tablature breaking.

Backing to our transcription, until now we already know how to do the ``introduction`` and the ``main riff``.

Let's see the verses.

The verses use a cool thing that people like to call ``Hendrix's chord`` (seventh sharp ninth chords).
We also use two major triads in a common shortened way (4th shape from CAGED). Doing some kind of thing with
them that some people like to call ``Motown's thing``, because it is rather used in ``R&B`` and ``Soul`` accompanying.
Come on, I am pretty sure that you are a serious music lover, further introductions related with ``Motown`` I judge
unnecessary here. Right?

Here goes the first part of the verses accompanying:

    .letring{5?/500-49}-.chord{500-49-300-201}.chord{5:~4:~3:~2:~}-|-
    .chord{500-49-300-201}.chord{5:~4:~3:~2:~}---.times{3}-|-
    .letring{4?/48-37}-.chord{48-37-28}.chord{2:p}.chord{48-37-26}--
    .letring{4?/400-39}-.chord{400-39-200}.chord{2:p}.chord{400-39-28}-

Beware that here I am not playing "that" open 6th string when it is played ``a half step down`` (by the way, the original tunning).
Here we could hold this note at the 3th fret but we need to sustain it to sound heavy and dude I sincerely hate capos.
I prefer to find my own way.

It is quite hard express accompanying patterns. Specially ``Hendrix's`` patterns, because he had used to avoid doing boring
"cowboy things", you know, up-and-down-up-and-down... In addition, I think that the most important thing to do with an
accompanying instruction is add your own ``mojo``... Let your *"mojo working"* here... I dislike dissect rhythm patterns
because I afraid of playing in a robot-way. I think that rhythm must be primitive. Not detailed understood. It is time
to celebrate your caveman's inheritance, no theories, listen to the beat and go for yourself letting your ancestors guide you ;)

Another nice thing that I like to do with this base is hold the triads root using my thumb at 6th instead of playing this root
using the 4th string. So, I slide it down until the root and dive into the shortened triad holding its root with my thumb.
I think maybe it is the base of what ``Hendrix`` does in this song record.

I also use some kind of hybrid picking but its out of a tablature's scope. Again, the colors is up to you.

We got new ``tlp`` stuff in the presented base pattern. The first is the "fret abstraction". The guitar is not a science
this is an art due to it several times we face imprecise things. Sometimes slides have destination but can be done from any
source. For doing it you must use a note that only indicates the string number and replaces the fret region by a suggestive
question mark. Then, we have ``6?``, ``5?``, ``4?``, ``3?``, ``2?`` and ``1?`` as possible abstractions.

In the previous code we can see a "note" which acts as a ``string jump`` (in terms of code), we use a string jump to indicate
to the ``Tulip`` where (which string) we want to put the next separator that complements this "jump". A jump is denoted by
``<string-number>:<separator>``. It is possible to do things like ``3:~``, ``2:p``, etc.

When there are pull-offs, hammer-ons or any single technique done using intervals and chords is important to build up
this song block using three or more chord tags in order to wrap these things. This guarantees a fancier typesetting.

Now, the second part from verses is related with a break done at the end of the last verse:

    .letring{4?/48-37}-.chord{48-37-28}.chord{2:p}.chord{48-37-26}--
    .letring{4?/400-39}-.chord{400-39-200}.chord{2:p}.chord{400-39-28}-|-
    .chord{500-49}.chord{5:\4:\}-

Anyway, as I said before this is just a little window about what ``Hendrix`` was able to do with. The truth is that
he naturally used to add new little random things at each turn, producing a richer accompanying. As a result what we feel while
listeners is that there is a secondary song evoluting inside the main. It can be a nice challenge for you and your creativity.

In terms of code, nothing new was presented above. The following is related to the bridge and then all verses are played again:

    .chord{500-302}.chord{5:/3:/}.chord{502-304}-.chord{500-302}-.chord{58-300}-|-
    400h402-300-201~~-400h402-300-201~~-

It brings a slide over eighth intervals and a little mention of the main riff's beginning.

Here goes my bad solo's aproximation... If you have a ``Octave Fuzz`` you should use it. The solo's entrance could be:

    402h404-302-205~~--402h402-302-205~~--305h307-206-108~~--305h307-206-108~~-|-

Engage your octave...

    203-.letring{205b-105-205~~-105-205b-205~~}-|-205h207-207~--205-207-205-307~~--307-309-307-305-307~~-307/309-305~~
    405/407-305-307-305-407-305~~--305-307-305-407-405~~--405-407-405-507-405~~~--402/405-302-304--
    304h305p304-405-302-304-405--304-405-304-405-304-405~~--402-405-302-304-305--106b-104h106p104-102-104-102-204-
    3?/304-203-.letring{204b-104-204~~~~}-

Maybe you will note a weird thing on bendings. I avoid indicating the destination notes. It is a personal preference
because I think that bends must be done singing the sound, muttering or thinking about what should be. It is weird
thinking: "Okay, now a will bend this string a half tone, a whole tone". But in ``Tulip`` you can write things like ``204b206``
there is no problem on it.

In order to get a complete transcription we need to the "exit solo". Follows:

    305h307-206-108~~~-.letring{63---------}-305h307-206-108~~-.letring{63---------}-|-
    206-208-209-208--206-307-305--.chord{209-106}.chord{2:b}-.chord{209-106}.chord{2:~}.chord{2:~}-|-
    .tremolopicking{1?/106~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~}-

Another great guitar player called ``Eric Gales`` does a nice ending using the eighth intervals from the bridge.
``Strumming`` more or less in this way:

    .strum{5?/.chord{500-302}----------------------5?/.chord{502-304}----------------------5?/.chord{58-300}-----------}

Go ahead repeating it again and again. The use of ``.strum`` is straightforward. It is a way to abstract the strumming
complications, saying only where to put the fingers and strum the notes following the current beat.

All done! Now we get a complete song.

>WARNING: Please do not take my assumptions as an absolute truth. This came from a lot of bad playing and preferences.
Then, do not try to learn how to play with me. Just learn how to use this ``software``!!!

A remark to Larry, Curly and Moe: To dump a whole song into a file without any logical layout is pretty dumb. The ``tlp``
has a nice control tag called ``.literal``. Inside this tag you can introduce a human-readable text enclosed by double-quotes
(what in programming we call ``string``). If you want to add a line break, use ``\n`` sequence. For tabulations, use ``\t``.

Another cool control sequence is the ``save-point`` denoted by a semicolon. When the ``Tulip's procesor`` find a semicolon all
typesetting done until that point (including your remarks to the reader, using the ``.literal`` tag) is saved and a new one
tablature diagram is used.

Still, there are two useful control tags for defining the ``song's title`` and the ``transcriber's name``. These tags are
respectively ``.song`` and ``.transcriber``. They receive a ``string`` as the ``.literal`` tag expects.

Then, here is our ``Purple Haze`` interpretation coded into ``Tulip's DSL``:

    .song{"Purple Haze (The Jimi Hendrix Experience / Are you experienced? [1967])"}

    .transcriber{"Rafael Santiago"}

    .literal{"Intro:\n"}

        69-.chord{500-401}--69-.chord{500-401}--69-.chord{500-401}--69-.chord{500-401}-|-;

    .literal{"Main riff:\n"}

        400h402-300-201-400~~~~~-.letring{46/48-58}-63-45~~-|-@
        400h402-300-201-400~~~~~-.letring{46/48-58}-63-66~~-|-
        58-500-48-400/402-400~~~~-400~~-400/402-300~-300~-300h302~~---300h301p300-402\-;

    .literal{"Verses (I):\n"}

        .letring{5?/500-49}-.chord{500-49-300-201}.chord{5:~4:~3:~2:~}-|-
        .chord{500-49-300-201}.chord{5:~4:~3:~2:~}---.times{3}-|-
        .letring{4?/48-37}-.chord{48-37-28}.chord{2:p}.chord{48-37-26}--
        .letring{4?/400-39}-.chord{400-39-200}.chord{2:p}.chord{400-39-28}-;


    .literal{"Verses (II):\n"}

        .letring{4?/48-37}-.chord{48-37-28}.chord{2:p}.chord{48-37-26}--
        .letring{4?/400-39}-.chord{400-39-200}.chord{2:p}.chord{400-39-28}-|-
        .chord{500-49}.chord{5:\4:\}-;

    .literal{"Bridge:\n"}

        .chord{500-302}.chord{5:/3:/}.chord{502-304}-.chord{500-302}-.chord{58-300}-|-
        400h402-300-201~~-400h402-300-201~~-;

    .literal{"Solo:\n"}

        402h404-302-205~~--402h402-302-205~~--305h307-206-108~~--305h307-206-108~~-|-
        203-.letring{205b-105-205~~-105-205b-205~~}-|-205h207-207~--205-207-205-307~~--307-309-307-305-307~~-307/309-305~~
        405/407-305-307-305-407-305~~--305-307-305-407-405~~--405-407-405-507-405~~~--402/405-302-304--
        304h305p304-405-302-304-405--304-405-304-405-304-405~~--402-405-302-304-305--106b-104h106p104-102-104-102-204-
        3?/304-203-.letring{204b-104-204~~~~}-;

    .literal{"Exit solo:\n"}

        305h307-206-108~~~-.letring{63---------}-305h307-206-108~~-.letring{63---------}-|-
        206-208-209-208--206-307-305--.chord{209-106}.chord{2:b}-.chord{209-106}.chord{2:~}.chord{2:~}-|-
        .tremolopicking{1?/106~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~}-;

    .literal{"Ending (outro):\n"}

        .strum{5?/.chord{500-302}----------------------5?/.chord{502-304}----------------------5?/.chord{58-300}-----------}


By now we will abstract how this code should be passed to the ``Tulip``. After processing we will get this output:

    Song: Purple Haze (The Jimi Hendrix Experience / Are you experienced? [1967])
    Transcribed by: Rafael Santiago

    Intro:


            |------------------------|
            |------------------------|
            |------------------------|
            |---11----11----11----11-|
            |---10----10----10----10-|
            |-9-----9-----9-----9----|

    Main riff:


                                  lr......                             lr......

            |-----------------------------------|------------------------------------|
            |----------11-----------------------|-----------11-----------------------|
            |-------10--------------------------|--------10--------------------------|
            |-10h12-------10~~~~~---6/8-----5~~-|--10h12-------10~~~~~---6/8-----5~~-|
            |---------------------------8-------|----------------------------8-------|
            |-----------------------------3-----|------------------------------3-----|

                                  lr......

            |-----------------------------------|----------------------------------------|
            |----------11-----------------------|----------------------------------------|
            |-------10--------------------------|--------------------------------10~-10~-|
            |-10h12-------10~~~~~---6/8---------|------8-10/12-10~~~~-10~~-10/12---------|
            |---------------------------8-------|-8-10-----------------------------------|
            |-----------------------------3-6~~-|----------------------------------------|

            |------------------------|
            |------------------------|
            |-10h12~~---10h11p10-----|
            |--------------------12\-|
            |------------------------|
            |------------------------|

    Verses (I):


              lr.......               lr......     lr.......

                                   3X
            |--------------|--------|-----------------------------|
            |----------11~-|-11~----|---------8p6-----------10p8--|
            |----------10~-|-10~----|-------7-7-7---------9--9-9--|
            |--------9--9~-|--9~----|---?/8---8-8----?/10---10-10-|
            |---?/10---10~-|-10~----|-----------------------------|
            |--------------|--------|-----------------------------|

    Verses (II):


              lr......     lr.......

            |-----------------------------|-----|
            |---------8p6-----------10p8--|-----|
            |-------7-7-7---------9--9-9--|-----|
            |---?/8---8-8----?/10---10-10-|-9-\-|
            |-----------------------------|-10\-|
            |-----------------------------|-----|

    Bridge:


            |-------------|-----------------------------|
            |-------------|----------11~~----------11~~-|
            |-12/14-12-10-|-------10------------10------|
            |-------------|-10h12---------10h12---------|
            |-10/12-10--8-|-----------------------------|
            |-------------|-----------------------------|

    Solo:


                                                                              lr...........

            |----------------------------------------18~~-----------18~~-|----------15-----|
            |----------15~~-----------15~~--------16-------------16------|-13---15b----15--|
            |-------12-------------12-------15h17----------15h17---------|-----------------|
            |-12h14----------12h12---------------------------------------|-----------------|
            |------------------------------------------------------------|-----------------|
            |------------------------------------------------------------|-----------------|

              lr...............

            |------15----------|------------------------------------------------------------|
            |---~~----15b-15~~-|-15h17-17~--15-17-15----------------------------------------|
            |------------------|---------------------17~~--17-19-17-15-17~~-17/19-15~~------|
            |------------------|------------------------------------------------------15/17-|
            |------------------|------------------------------------------------------------|
            |------------------|------------------------------------------------------------|

            |------------------------------------------------------------------------------|
            |------------------------------------------------------------------------------|
            |-15-17-15----15~~--15-17-15-----------------------------------12-14--14h15p14-|
            |----------17----------------17-15~~--15-17-15----15~~~--12/15-----------------|
            |----------------------------------------------17------------------------------|
            |------------------------------------------------------------------------------|

            |---------------------------------------------------16b-14h16p14-12-14-12----|
            |-------------------------------------------------------------------------14-|
            |----12-14-----14----14----14-------------12-14-15---------------------------|
            |-15-------15-----15----15----15~~--12-15------------------------------------|
            |----------------------------------------------------------------------------|
            |----------------------------------------------------------------------------|

                      lr..............

            |---------------14--------|
            |------13---14b----14~~~~-|
            |-?/14--------------------|
            |-------------------------|
            |-------------------------|
            |-------------------------|

    Exit solo:


                             lr..........               lr..........

            |----------18~~~-----------------------18~~--------------|--------------------|
            |-------16--------------------------16-------------------|-16-18-19-18--16----|
            |-15h17-----------------------15h17----------------------|-----------------17-|
            |--------------------------------------------------------|--------------------|
            |--------------------------------------------------------|--------------------|
            |------------------3--------------------------3----------|--------------------|

                             tp............................................

            |-----16--16---|---?/16~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~-|
            |-----19b-19~~-|-----------------------------------------------|
            |-15-----------|-----------------------------------------------|
            |--------------|-----------------------------------------------|
            |--------------|-----------------------------------------------|
            |--------------|-----------------------------------------------|

    Ending (outro):


              st...................................................................

            |----------------------------------------------------------------------|
            |----------------------------------------------------------------------|
            |-----12------------------------14------------------------10-----------|
            |----------------------------------------------------------------------|
            |---?/10----------------------?/12----------------------?/-8-----------|
            |----------------------------------------------------------------------|

Okay, now let's see a song which uses more single notes than intervals. For doing it I will take a nice interpretation
by ``Stan Getz`` of a famous ``Brazilian song`` called ``O Pato`` (a.k.a ``The duck``). Here goes my humble ``Stan Getz``
solo's adaptation for the guitar. I think that any brazilian child (at least from my childhood's time) should know this.
This is pretty simple but I would like to use it to introduce you two nice new ``tags``:

    .song{"O Pato - (adapted from Stan Getz's version)"}
    .transcriber{"Rafael Santiago"}

    .letring{5?/502-403-301-.chord{300-200}.chord{3:/2:/}.chord{301-201}}-|-

    .letring{.chord{502-401}-.chord{300-200}.chord{3:/2:/}.chord{301-201}-.chord{502-401}-
    .chord{300-200}.chord{3:/2:/}.chord{301-201}-----}-|-.part{Quack}

    401-403-301--301h303p301-403-401-|-.part{Walk}-

    .repeat{Quack}-.repeat{Walk}-.repeat{Quack}-

    .chord{401-204}-.chord{201-301}-303-301-403-|-4:/401-300-301-.times{3}-|-

    502-403-301-303-.chord{201-101}-.chord{401-204}-|-.chord{101-201}-
    .chord{401-204}--4?/403-301-303-201-303-|-

    402-303-201-303-301-402-301-303-201-303-301-402-301-303-201-101-101-101-|-

    303-204-101-104-101-.chord{401-204}-|-

    301-303-301-403-301-303-301-403-301-303-301-401\-|-

    301-303-403-301-303-301-201-303-301-403-301\-|-

    301-303-301-403-301-303-301-403-301-303-301-201-301-|-

    303-204-201-204-.chord{300-103}-.chord{101-38}-|-

    3:/303-204-101-204-4:/401-300-301~~-

In the previous code I am using the tags ``.part`` and ``.repeat``. This is useful when you got things that are repeated
in a non-linear way. All you should do is "highlight" the part with ``.part{Name of this Part}`` and further when desired
repeat this "highlighted" part using ``.repeat{Name of this Part}``. The code output after processing is:

    Song: O Pato - (adapted from Stan Getz's version)
    Transcribed by: Rafael Santiago

              lr.................

            |--------------------|--------------------------|--------------------------|
            |--------------10/11-|------10/11----10/11------|--------------------------|
            |-----------11-10/11-|------10/11----10/11------|-------11--11h13p11-------|
            |--------13----------|---11-------11------------|-11-13--------------13-11-|
            |---?/12-------------|---12-------12------------|--------------------------|
            |--------------------|--------------------------|--------------------------|

              lr......................                                  lr................

            |--------------------------|----------------------------|---------------------|
            |------10/11----10/11------|----------------------------|--------10/11----10--|
            |------10/11----10/11------|---------11--11h13p11-------|--------10/11----10--|
            |---11-------11------------|---11-13--------------13-11-|-----11-------11-----|
            |---12-------12------------|----------------------------|-----12-------12-----|
            |--------------------------|----------------------------|---------------------|

              lr........

                                                       3X
            |------------|-----------------|------------|-------------11----|-11----------|
            |---/11------|--14-11----------|------------|-------------11-14-|-11-14-------|
            |---/11------|-----11-13-11----|-----10-11--|-------11-13-------|-------------|
            |------------|--11----------13-|-/11--------|----13----------11-|----11--?/13-|
            |------------|-----------------|------------|-12----------------|-------------|
            |------------|-----------------|------------|-------------------|-------------|

            |-------------|----------------------------------------------11-11-11-|-------|
            |-------11----|-------11----------------11----------------11----------|----14-|
            |-11-13----13-|----13----13-11----11-13----13-11----11-13-------------|-13----|
            |-------------|-12-------------12----------------12-------------------|-------|
            |-------------|-------------------------------------------------------|-------|
            |-------------|-------------------------------------------------------|-------|

            |-11-14-11----|--------------------------------------|-------------------------|
            |----------14-|--------------------------------------|-------------------11----|
            |-------------|-11-13-11----11-13-11----11-13-11-----|-11-13----11-13-11----13-|
            |----------11-|----------13----------13----------11\-|-------13----------------|
            |-------------|--------------------------------------|-------------------------|
            |-------------|--------------------------------------|-------------------------|

            |-----------|----------------------------------------|-------------13-11-|-----|
            |-----------|----------------------------------11----|----14-11-14-------|-----|
            |-11----11\-|-11-13-11----11-13-11----11-13-11----11-|-13----------10-8--|-/13-|
            |----13-----|----------13----------13----------------|-------------------|-----|
            |-----------|----------------------------------------|-------------------|-----|
            |-----------|----------------------------------------|-------------------|-----|

            |----11----------------|
            |-14----14-------------|
            |--------------10-11~~-|
            |----------/11---------|
            |----------------------|
            |----------------------|

The rules about highlighting used in the ``.part`` tag are the same present in the ``once more``.

Now let's see a cool song by one of my favorite contemporary singers: ``Imelda May``. This song comes from her
second album. It is called ``Feel me``. It brings a great guitar's solo by an awesome guitar player called ``Darrel
Higham``. For sure that my transcription is pretty wrong but I like to play in this way (from the point of view of a
``frustrated piano player``). :O)

So... This song starts with a nice "piano's riff", I approximate it as follows:

    .literal{"Piano's riff:\n"}

            60--.chord{401-302-202-102}-.chord{305-205}-.chord{302-202}-.chord{304-204}-.chord{302-202}--
            502-504-402-404-.chord{302-202}-;

The base is a mix of fingering and strumming, I think that the better way is listen to it and try to figure out:

    .literal{"Base:\n"}

            .strum {
                57-.chord{57-49-39-28-17}----

                67-.chord{17-67-59-47-38-200}----

                401-.chord{401-302-202-102}---.chord{400-301-200-101}---|-

                57-.chord{57-49-39-28-17}----

                67-.chord{17-67-59-47-38-200}----

                400-.chord{400-301-200-101}---60-.chord{500-48-39-28}---
             }-;

Maybe the chorus, at first, can be a kind of complicated, because we get so many dynamics and several repetions. Again,
the better way is listen to it:

    .literal{"Chorus:\n"}

            .chord{404-304-204}-.chord{407-307-207}--
            .chord{404-304-204}-.chord{402-302-202}-.chord{406-306-206}-
            .chord{402-302-202}-.strum{.chord{501-400-301}-----}-
            .chord{406-306-206}-.chord{404-304-204}-.chord{402-302-202}-.times{2}-|-

            .strum{.chord{501-400-301}-----.chord{57-49-39-28-17}-----}.times{2}-|-

            .chord{404-304-204}-.chord{407-307-207}--
            .chord{404-304-204}-.chord{402-302-202}-.chord{406-306-206}-
            .chord{402-302-202}-.strum{.chord{501-400-301}---}|-

            .mute{.chord{67-59-49}--}.times{7}-|-

            .chord{404-304-204}-.chord{407-307-207}--
            .chord{404-304-204}-.chord{402-302-202}-.chord{406-306-206}-
            .chord{402-302-202}-.strum{.chord{501-400-301}-----}-
            .chord{406-306-206}-.chord{404-304-204}-.chord{402-302-202}-|-

            .chord{404-304-204}-.chord{407-307-207}--
            .chord{404-304-204}-.chord{402-302-202}-.chord{406-306-206}-
            .chord{402-302-202}-.strum{.chord{501-400-301}-----.chord{57-49-39-28-17}------}-;

Here goes my bad guitar solo's approximation. Add your improvements according to your own preferences and skills:

    .literal{"Solo:\n"}

            5:/57-57-57-57-|-

            67-600-57-59-600-57-59-47-49-37-39-27-28-100-|-100-18h100p18-17-28-39-|-

            402h403h404-.chord{302-202}.chord{3:b2:b}--404-502h504-402-404-302b-|-

            404-302-.letring{304b-205-304r}-302-.times{2}-|-

            404-.chord{302-202}.chord{3:b2:b}-404-|-304b-202-102-;.letring{205b-102--}.times{5}-|-

            205b----|-3:/306-207-105-107b---107r-105-107-207~~-|-

            304-202-102-205b-205-202-304-205-404-304-302-404-302-404-|-504-402-404-.chord{302-202}-|-

            404-402h403h404-.chord{302-202}.chord{3:b2:b}-.chord{302-202}.chord{3:b2:b}-404-|-

            27h28-17-102-102-100-18-17-28-28-17-28-|-27h28-17-100-100-18-17-28-39-39-28-39-|-

            47h49-37-39-28-39-37-49-28-28-39-|-;.letring{305b-207p202}-.times{4}-|-

            .chord{205-102}.chord{2:b1:b}-205r-202-205p202-304-302b-404\-;

Now the entire song's code listing:

    .song{"Feel me - (Imelda May / Love Tattoo [2007])"}
    .transcriber{"Rafael Santiago"}

    .literal{"Piano's riff:\n"}

            60--.chord{401-302-202-102}-.chord{305-205}-.chord{302-202}-.chord{304-204}-.chord{302-202}--
            502-504-402-404-.chord{302-202}-;

    .literal{"Base:\n"}

            .strum {
                57-.chord{57-49-39-28-17}----

                67-.chord{17-67-59-47-38-200}----

                401-.chord{401-302-202-102}---.chord{400-301-200-101}---|-

                57-.chord{57-49-39-28-17}----

                67-.chord{17-67-59-47-38-200}----

                400-.chord{400-301-200-101}---60-.chord{500-48-39-28}---
             }-;

    .literal{"Chorus:\n"}

                .chord{404-304-204}-.chord{407-307-207}--
                .chord{404-304-204}-.chord{402-302-202}-.chord{406-306-206}-
                .chord{402-302-202}-.strum{.chord{501-400-301}-----}-
                .chord{406-306-206}-.chord{404-304-204}-.chord{402-302-202}-.times{2}-|-

                .strum{.chord{501-400-301}-----.chord{57-49-39-28-17}-----}.times{2}-|-

                .chord{404-304-204}-.chord{407-307-207}--
                .chord{404-304-204}-.chord{402-302-202}-.chord{406-306-206}-
                .chord{402-302-202}-.strum{.chord{501-400-301}---}|-

                .mute{.chord{67-59-49}--}.times{7}-|-

                .chord{404-304-204}-.chord{407-307-207}--
                .chord{404-304-204}-.chord{402-302-202}-.chord{406-306-206}-
                .chord{402-302-202}-.strum{.chord{501-400-301}-----}-
                .chord{406-306-206}-.chord{404-304-204}-.chord{402-302-202}-|-

                .chord{404-304-204}-.chord{407-307-207}--
                .chord{404-304-204}-.chord{402-302-202}-.chord{406-306-206}-
                .chord{402-302-202}-.strum{.chord{501-400-301}-----.chord{57-49-39-28-17}------}-;

    .literal{"Solo:\n"}

                5:/57-57-57-57-|-

                67-600-57-59-600-57-59-47-49-37-39-27-28-100-|-100-18h100p18-17-28-39-|-

                402h403h404-.chord{302-202}.chord{3:b2:b}--404-502h504-402-404-302b-|-

                404-302-.letring{304b-205-304r}-302-.times{2}-|-

                404-.chord{302-202}.chord{3:b2:b}-404-|-304b-202-102-;.letring{205b-102--}.times{5}-|-

                205b----|-3:/306-207-105-107b---107r-105-107-207~~-|-

                304-202-102-205b-205-202-304-205-404-304-302-404-302-404-|-504-402-404-.chord{302-202}-|-

                404-402h403h404-.chord{302-202}.chord{3:b2:b}-.chord{302-202}.chord{3:b2:b}-404-|-

                27h28-17-102-102-100-18-17-28-28-17-28-|-27h28-17-100-100-18-17-28-39-39-28-39-|-

                47h49-37-39-28-39-37-49-28-28-39-|-;.letring{305b-207p202}-.times{4}-|-

                .chord{205-102}.chord{2:b1:b}-205r-202-205p202-304-302b-404\-;

Follows the output tablature:

    Song: Feel me - (Imelda May / Love Tattoo [2007])
    Transcribed by: Rafael Santiago

    Piano's riff:


            |----12-----------------------------|
            |----12-15-12-14-12--------------12-|
            |----12-15-12-14-12--------------12-|
            |----11--------------------12-14----|
            |--------------------12-14----------|
            |-0---------------------------------|

    Base:


              st............................ ..............................

            |-----7-------7-------12---11---|---7-------7-------11---------|
            |-----8------10-------12---10---|---8------10-------10-----8---|
            |-----9-------8-------12---11---|---9-------8-------11-----9---|
            |-----9-------7----11-11---10---|---9-------7----10-10-----8---|
            |---7-7-------9-----------------|-7-7-------9--------------10--|
            |----------7--7-----------------|--------7--7------------0-----|

    Chorus:


                                 st.......             st.............

                                                    2X                2X
            |----------------------------------------|----------7------|--------------------|
            |-14-17--14-12-16-12-----------16-14-12--|----------8------|-14-17--14-12-16-12-|
            |-14-17--14-12-16-12---11------16-14-12--|---11-----9------|-14-17--14-12-16-12-|
            |-14-17--14-12-16-12---10------16-14-12--|---10-----9------|-14-17--14-12-16-12-|
            |----------------------11----------------|---11-----7------|--------------------|
            |----------------------------------------|-----------------|--------------------|

              st.....  pm...                      st.......

                            7X
            |--------|-------|---------------------------------------|--------------------|
            |--------|-------|-14-17--14-12-16-12-----------16-14-12-|-14-17--14-12-16-12-|
            |---11---|-------|-14-17--14-12-16-12---11------16-14-12-|-14-17--14-12-16-12-|
            |---10---|---9---|-14-17--14-12-16-12---10------16-14-12-|-14-17--14-12-16-12-|
            |---11---|---9---|----------------------11---------------|--------------------|
            |--------|---7---|---------------------------------------|--------------------|

              st..............

            |----------7------|
            |----------8------|
            |---11-----9------|
            |---10-----9------|
            |---11-----7------|
            |-----------------|

    Solo:


            |----------|-----------------------------10-|-10-8h10p8-7-----|--------------|
            |----------|-------------------------7-8----|-------------8---|----------12b-|
            |----------|---------------------7-9--------|---------------9-|----------12b-|
            |----------|-----------------7-9------------|-----------------|-12h13h14-----|
            |-/7-7-7-7-|------7-9----7-9----------------|-----------------|--------------|
            |----------|-7-10-----10--------------------|-----------------|--------------|

                                         lr...........

                                                         2X
            |--------------------|------------------------|-----------|--------12-|
            |--------------------|-------------15---------|----12b----|-----12----|
            |----------------12b-|----12---14b----14r-12--|----12b----|-14b-------|
            |-14-------12-14-----|-14---------------------|-14-----14-|-----------|
            |----12h14-----------|------------------------|-----------|-----------|
            |--------------------|------------------------|-----------|-----------|

              lr........

                        5X
            |-------12---|--------|--------15-17b---17r-15-17------|-------12--------------|
            |---15b------|-15b----|-----17--------------------17~~-|----12----15b-15-12----|
            |------------|--------|-/16----------------------------|-14-----------------14-|
            |------------|--------|--------------------------------|-----------------------|
            |------------|--------|--------------------------------|-----------------------|
            |------------|--------|--------------------------------|-----------------------|

            |----------------------|-------------|------------------------|-----7-12-12-10-|
            |-15-------------------|----------12-|-------------12b-12b----|-7h8------------|
            |-------14-12----12----|----------12-|-------------12b-12b----|----------------|
            |----14-------14----14-|----12-14----|-14-12h13h14---------14-|----------------|
            |----------------------|-14----------|------------------------|----------------|
            |----------------------|-------------|------------------------|----------------|

            |-8-7-----7---|-----7-10-10-8-7-----------|-----------------------|
            |-----8-8---8-|-7h8-------------8-----8---|---------8-------8-8---|
            |-------------|-------------------9-9---9-|-----7-9---9-7-------9-|
            |-------------|---------------------------|-7h9-----------9-------|
            |-------------|---------------------------|-----------------------|
            |-------------|---------------------------|-----------------------|

              lr...........

                          4X
            |--------------|-12b-------------------------|
            |-------17p12--|-15b-15r-12-15p12------------|
            |---15b--------|------------------14-12b-----|
            |--------------|-------------------------14\-|
            |--------------|-----------------------------|
            |--------------|-----------------------------|

Into the ``Feel me``'s solo code you could see that I introduced some ``save-points``. I did it seeking
for a fancier typesetting. The ``Tulip`` tries to adjust the things by itself, however, sometimes you need to do it by
your own. Bear in mind that internally we got some heuristics for best fitting but they are just heuristics. It is impossible
handle all cases. Do not forget that you got control ``tags`` and ``separators`` to be in control. Your ``tab``, your rules.

Continuing... Here goes one of the most popular bass lines of the Universe written by ``Charles Mingus``
(``Haitian Fight Song``), screwed up in my lousy ``walking bass guitar`` way:

    .song{"Haitian Fight Song (Charles Mingus / The Clown [1957])"}
    .transcriber{"Rafael Santiago"}

    6?/67-.chord{47-37-27}--50-.chord{58-47-300}-.chord{59-48-301}-.times{3}-|-.part{walk}

    50-.chord{49-500-302}-.chord{59-48-301}-50-.chord{58-47-300}--.chord{59-48-301}-|-

    .repeat{walk}

    50-.chord{49-500-302}-.chord{59-48-301}-50-.chord{58-47-300}--.chord{59-48-301}-|-;

    .repeat{walk}

    .letring {
        500-49-302---49--500--602-55/59~~--
        .chord{48-39}.chord{4:~3:~}.chord{4:~3:~}.chord{4:~3:~}.chord{4:~3:~}-----
        6?/67~~~~----
    }

In terms of code it does not have anything special but it is a good way to adapt your mind about the usage of the ``.chord``
tag. I find it could be hard in the beginning. The song's theme it is up to you. Go ahead, listen to it and try to fill up with
something that you judge good enough. The tablature is what follows:

    Song: Haitian Fight Song (Charles Mingus / The Clown [1957])
    Transcribed by: Rafael Santiago

                             3X                                   3X
            |-----------------|------------------|-----------------|------------------|
            |-----7-----------|------------------|-----7-----------|------------------|
            |-----7----10-11--|---12-11---10--11-|-----7----10-11--|---12-11---10--11-|
            |-----7-----7--8--|----9--8----7---8-|-----7-----7--8--|----9--8----7---8-|
            |--------0--8--9--|-0-10--9-0--8---9-|--------0--8--9--|-0-10--9-0--8---9-|
            |-?/7-------------|------------------|-?/7-------------|------------------|

                                lr................................................

                             3X
            |-----------------|---------------------------------------------------|
            |-----7-----------|---------------------------------------------------|
            |-----7----10-11--|--------12--------------------9~~~~----------------|
            |-----7-----7--8--|------9------9----------------8~~~~----------------|
            |--------0--8--9--|---10-----------10-----5/9~~-----------------------|
            |-?/7-------------|--------------------12------------------?/7~~~~----|

Okay as we started to talk about ``jazz``, then let's pick up a nice ``jazz-funk`` by ``Grant Green``. The song is
``High Heeled Sneakers``. Originally, this song is an old ``blues`` song, recorded by a ``blues man`` called ``Tommy Tucker``.
``Grant Green`` picked up the verses melody and made up a dancing ``jazz-funk`` with this. I challenge you not to nod your
head or tap your feet while listening to it. By the way, there is an interesting thing that I want to introduce you using this
cool jazz theme:

    .song{"High Heeled Sneakers - Grant Green's version / Iron City [1972]"}
    .transcriber{"Rafael Santiago"}

        .literal{"# Theme\n"}

        .literal{"\tRepeat what follows twice...\n"}

        .chord{27-17}-.chord{27-17}-.chord{27-17}--.chord{28-18}/.chord{200-100}--
        .chord{501-400-301-202}--.chord{501-400-301-200}-.chord{501-400-301-202}-|-
        49-37h38p37-.chord{49-37-27}-200-17-200-.chord{27-17}--
        49-.chord{37-27}.chord{3:h}38-.chord{37-27}.chord{3:h}38-.chord{49-37}-;-

        .letring {
            .chord{27-37}-49-.chord{27-37}-49-.chord{27-37}-
        }-
        .chord{39-200}/.chord{300-201}\.chord{200-39}--.chord{37-200}-
        .chord{39-200}-.chord{37-200}-49-
        .letring {
            .chord{27-37}-49-.chord{27-37}-49-.chord{27-37}-
        }-;

        .literal{"\tThe theme's break\n"}

        .chord{49-202}-.chord{49-202}-.chord{49-202}-.chord{49-202}/.chord{401-204}--
        .chord{401-302}-.chord{401-301}.chord{3:h}302-.chord{403-302}~~~~~~-
        3?/302-201-102b---102r-100-202-200-301-;
        49-.chord{37-27}.chord{3:h}38p.chord{37-27}-.chord{49-37}-
        .letring {
            .chord{27-37}-49-.chord{27-37}-49-.chord{27-37}-
        }-
        .chord{39-200}/.chord{300-201}\.chord{200-39}--.chord{37-200}-
        .chord{39-200}-.chord{37-200}-49-
        .letring {
            .chord{27-37}-49-.chord{27-37}-49-.chord{27-37}-
        }-;

The nice stuff above are productions like: ``.chord{28-18}/.chord{200-100}``. When we have hammer-on's, pull-off's, slides, etc
between whole notes from a interval to another we can use this simpler construction instead of:
``chord{28-18}.chord{2:/1:/}.chord{200-100}``.

The output tablature result based on previous code would be:

    Song: High Heeled Sneakers - Grant Green's version / Iron City [1972]
    Transcribed by: Rafael Santiago

    # Theme

	    Repeat what follows twice...


            |-7-7-7--8/10------------|--------------7----7--------------|
            |-7-7-7--8/10--12--10-12-|---------7-10---10-7----7---7-----|
            |--------------11--11-11-|---7h8p7-7--------------7h8-7h8-7-|
            |--------------10--10-10-|-9-------9------------9---------9-|
            |--------------11--11-11-|----------------------------------|
            |------------------------|----------------------------------|

            lr..........                     lr..........

            |----------------------------------------------|
            |---7---7---7--10/11\10-10-10-10-----7---7---7-|
            |---7---7---7---9/10\9---7--9--7-----7---7---7-|
            |-----9---9----------------------9-----9---9---|
            |----------------------------------------------|
            |----------------------------------------------|

	    The theme's break


            |-------------------------------------------12b---12r-10----------|
            |-12-12-12-12/14-------------------------11--------------12-10----|
            |-----------------12-11h12-12~~~~~~-?/12-----------------------11-|
            |--9--9--9--9/11--11-11----13~~~~~~-------------------------------|
            |-----------------------------------------------------------------|
            |-----------------------------------------------------------------|

                        lr..........                     lr..........

            |--------------------------------------------------------|
            |---7---7-----7---7---7--10/11\10-10-10-10-----7---7---7-|
            |---7h8p7-7---7---7---7---9/10\9---7--9--7-----7---7---7-|
            |-9-------9-----9---9----------------------9-----9---9---|
            |--------------------------------------------------------|
            |--------------------------------------------------------|

There is another nice (at least I judge) thing related with ``chords`` introduced with the last sample song.
The ``Tulip`` when face chords made with notes mixing up frets less than fret #10 and greater or equal than fret #10,
it try to adjust the notes considering your explicit preferences, for example, I think the following typesetting ugly:

    |-10-----(...)
    |-9------(...)
    |-8------(...)
    |--------(...)
    |--------(...)
    |--------(...)

I prefer:

    |-10-----(...)
    |--9-----(...)
    |--8-----(...)
    |--------(...)
    |--------(...)
    |--------(...)

However, it is just about a point of view. With this in mind I have decided to implement a implicit rule (to be signaled by you)
for chords typesetting. This signal is done using the disposition of the notes. So when you use this ``.chord{100-29-38}``
you have this:

    |-10-----(...)
    |-9------(...)
    |-8------(...)
    |--------(...)
    |--------(...)
    |--------(...)

When you use this ``.chord{38-29-100}`` you have this:

    |-10-----(...)
    |--9-----(...)
    |--8-----(...)
    |--------(...)
    |--------(...)
    |--------(...)

The first (using smaller note cipherings before) always will need a leading separator to compensate and keep the next production
well separated from the current. In the ``High Heeled Sneakers`` we have a practical example. Furthermore, we have these
two typesetting rules co-working for a fancier output. Take a look:

    .chord{39-200}/.chord{300-201}\.chord{200-39}--.chord{37-200}-

The above code produces:

    |-------------|
    |-10/11\10-10-|
    |--9/10\9---7-|
    |-------------|
    |-------------|
    |-------------|

Now let's see some accompaniment pattern from an ``Albert King's`` song called ``Lovingest Woman In Town`` (from the
point of view of a frustrated ``horn section`` member... haha). I love this kind of ``funky`` blues that ``Albert King``
used to do so well. He got a killer horn section in his band. So, here is my (lousy) interpretation from a great moment
of these horns:

    .song{"Lovingest Woman In Town -  Albert King"}
    .transcriber{"Rafael Santiago"}

    .literal{"Base:\n"}

        .chord{56-46}-.chord{56-48-37}--
        .chord{56-48-37}-.chord{56-48-37}-
        .chord{56-46}-.chord{56-48-37}--
        .chord{49-500-300-201}-.chord{49-500-300-201}-.times{3}-|-
        .chord{49-500-300-201}-
        .chord{400-300}.chord{3:h}.chord{302}-.chord{300-201}-.chord{300-201}-
        .chord{400-300}.chord{3:h}.chord{302}-.chord{300-201}-.chord{300-201}-
        .chord{400-300}.chord{3:h}.chord{302}-.chord{300-201}-.chord{300-201}-
        .chord{400-300}.chord{3:h}.chord{302}-|-
        56-57-.chord{48-37}--
        .chord{58-47-38}-.chord{19-29}--
        .chord{58-47-38}-.chord{18-28}--
        48-.chord{37-26-16}--
        .chord{46-36}h.chord{48-37}-.chord{28-18}-.chord{26-16}--
        .chord{26-16}-.chord{400-302-201}.chord{3:p}.chord{400-300-201}.chord{3:h}.chord{400-302-201}-.times{2}-|-

        56-57-
        .chord{58-47-38-29}~
        .chord{58-47-38-29}~
        .chord{58-47-38-29}~
        .chord{58-47-38-29}~~-
        .letring{58-49-38-29--16-26--26-38--38-;-36h37-48}~~~-

        38b-26-16-29-26-38b-38r-36-48-|-

        .chord{36-25-16}@@@/.chord{308-207-108}-

        .chord{308-207-108}@@-

And, of course the resultant tablature:

    Song: Lovingest Woman In Town -  Albert King
    Transcribed by: Rafael Santiago

    Base:


                                  3X
            |----------------------|----------------------------------------------|----------|
            |---------------11-11--|-11-------11-11-------11-11-------11-11-------|----------|
            |---7--7-7---7--10-10--|-10-10h12-10-10-10h12-10-10-10h12-10-10-10h12-|-----7--8-|
            |-6-8--8-8-6-8---9--9--|--9-10----------10----------10----------10----|-----8--7-|
            |-6-6--6-6-6-6--10-10--|-10-------------------------------------------|-6-7----8-|
            |----------------------|----------------------------------------------|----------|

                                                                lr.....................

                                               2X
            |-9----8----6------8-6--6-----------|--------------------------6-----------|
            |-9----8----6------8-6--6-11-11-11--|-----9~9~9~9~~---------9----6--6------|
            |----8------7--6h7--------12p10h12--|-----8~8~8~8~~-------8-----------8--8-|
            |----7----8----6h8--------10-10-10--|-----7~7~7~7~~-----9------------------|
            |----8------------------------------|-6-7-8~8~8~8~~---8--------------------|
            |-----------------------------------|--------------------------------------|

              lr.......

            |------------------6---------------|-6--6--6--6/18-18--18--18-|
            |----------------6---9-6-----------|-5--5--5--5/17-17--17--17-|
            |----6h7------8b---------8b-8r-6---|-6--6--6--6/18-18--18--18-|
            |--------8~~~--------------------8-|--------------------------|
            |----------------------------------|--------------------------|
            |----------------------------------|--------------------------|

The ``solos`` and ``Woooooos`` is up to you! ;)

By the way I think that the recording that I used to transcribe has the horn section from ``Willie Dixon's`` band but I am
not so sure about this fact.

Now, going back to the rock tunes... Let's see a cool song from a cool band. The band is ``The Hellacopters`` and the song
is one of my favorite from them, ``Better than you``. The main thing that I like in the songs made by them is the simplicity
and the sense of what a rock n' roll song should be: ``simple, cool & fast``. In terms of ``guitars`` this band always had a quite nice
``two-guitar`` work. The guitar solos & riffs used to complement each other, continuing the talk... This song is a nice sample
of it. You can find out live versions from this song where he promoted interesting guitar battles. You should watch it.

Well, just starting, in the introduction we get a nice ``Chuck Berry-like`` lick/riff. The cool thing here is listening to one
guitar overlapping another one with the same idea but using a little different interval. I am not your teacher, so the theory
stops here...

    .chord{201-101}/.chord{203-103}-.chord{203-103}-.chord{203-103}-.times{2}-|-
    .chord{201-101}/.chord{203-103}-.chord{203-103}-;

    .chord{306-205}/.chord{307-206}-.chord{307-206}-.chord{307-206}-.times{2}-|-
    .chord{306-205}/.chord{307-206}-.chord{307-206}-;

    .chord{305-205}.chord{3:b2:b}-.chord{305-205}.chord{3:r2:r}-
    .chord{303-203}-.chord{305-205}-.chord{303-203}-405-;

Then, we are pushed by a badass ``base riff``. The problem here is that I transcribed this, with my lazy chords constructions...
I will let you figure out about the chords used here. Beware for not learning my bad practices... Anyway, the
``base riff`` using ``"Garfield hands"`` would be:

    .chord{50-40}-.chord{58-400-300}-.chord{58-400-300}-.chord{50-40}-
    .chord{58-48-38}-.chord{58-48-38}-.chord{58-48-38}-
    .chord{50-40}-.chord{58-48-300-201}-.chord{58-48-38-29}-
    .chord{58-400-300}-|-.chord{58-400-300}-.chord{58-400-300}-;

Still in the introduction we have a ``blues-rock`` lick, I like to do it in the following way:

    .chord{206-103}.chord{2:b}-.chord{206-103}.chord{2:r}-
    .chord{206-103}.chord{2:b}-.chord{206-103}.chord{2:r}-
    .chord{206-103}.chord{2:b}-.chord{206-103}.chord{2:r}-

    206/208-206-208-206-307-106~~-|-208-109p106-208-106-208-305-203-206p203-305-303-405~~-;

During the chorus you can take some breath but not be negligent about the ``palm-muting`` otherwise will sound pretty bad:

    .chord{50-40}-
    .mute{
        .chord{58-48-300-201}-------------------|-
        .chord{58-48-38-29}----------------
    }-|-;
    .chord{50-40}-.chord{58-400-300}-.chord{58-400-300}-.chord{50-40}-
    .chord{58-48-38}-.chord{58-48-38}-.chord{58-48-38}-
    .chord{50-40}-.chord{58-48-300-201}-.chord{58-48-38-29}-
    .chord{58-400-300}-|-.chord{58-400-300}-.chord{58-400-300}-;
    .chord{50-40}-
    .mute{.chord{58-48-300-201}-----------------}-|-
    .chord{50-40}-
    .mute{.chord{56-48-38}-----------------}-;

After the first turn we got another short pentatonic lick:

    .chord{206-103}.chord{2:b}-.chord{206-103}.chord{2:r}-
    .chord{206-103}.chord{2:b}-.chord{206-103}.chord{2:r}-
    .chord{206-103}.chord{2:b}-.chord{206-103}.chord{2:r}-

    4:/400-300-29/200-300-38-300~~-;

We get a break/bridge before the guitar solos. By the way, pretty nice:

    .letring{.chord{58-400-300}--------------}-.chord{58-400-300}-;

    500h502-.chord{400-300}-.chord{400-300}-502-.chord{400-300}-|-
    .chord{302-202}.chord{3:b2:b}-.chord{302-202}.chord{3:r2:r}-
    .chord{300-200}-.chord{38-28}-|-

    58h500-.chord{48-38}-.chord{48-38}-500-.chord{48-38}-|-
    500h502-400-402-.chord{300-200}~~-|-

    500h502-.chord{400-300}-.chord{400-300}-502-.chord{400-300}-|-
    .chord{302-202}.chord{3:b2:b}-.chord{302-202}.chord{3:r2:r}-
    .chord{300-200}-.chord{38-28}-|-

    58h500-.chord{48-38}-.chord{48-38}-500-.chord{48-38}-|-
    .chord{302-202}.chord{3:b2:b}-.chord{302-202}.chord{3:r2:r}-.chord{300-200}~~~~-;

The solos are divided into two but in the end is a interesting talk between the musicians:

    .chord{302-200}.chord{3:b}-.chord{200-100}-203-200-302-300-402-300-303-302-300~-|-
    /300-29-200-101b-|-101~~--101~~-|-101b102r101p18-101p18-29-300-|-307-206-208-106-109-108-106-
    108b110r108-106-108-106-101-103-;-309-208-.letring{210b-110-210r-110-210r-110-210r}-208~~-;

The second solo starts with a classical rockabilly stuff:

    3:/305-.chord{206-106}@@@@@@@-|-
    .chord{206-106}-.chord{205-105}-.chord{203-103}-.chord{306-206}-
    .chord{305-205}-.chord{303-203}-405-.chord{303-203}-305b-;
    .letring{203h206p203-305b}-.times{3}-|-206/208~~-|-307-208-106-208-108/110\108-|-
    .chord{305-203}.chord{3:b}-203-305-303-305-303-.times{2}-;-405-403-403h405-.letring{303b---101------}-;

I love the easy way we fell down of the solo falling into the ``calm part``. Here is essential "got your mojo working"...

    .letring{
        .chord{53-43-35-26}-26-35-.chord{53-43-33-24}-.chord{53-43-33-24}-.chord{53-43-33-24}
    }-
    .chord{33-23}-.chord{31-21}-.chord{43-32-21}-21-32-;
    .letring{
        .chord{53-43-35-26}-26-35-.chord{53-43-33-24}-.chord{53-43-33-24}-.chord{53-43-33-24}
    }-
     .chord{33-23}-.chord{31-21}-.chord{43-32-21}-53h56-43-;

This is a nice base for noodling your guitar all day long. Find for videos from this song with "battles"... You can not be sorry.

The end of the song is just a pentatonic lick using intervals:

    .chord{2:/1:/}.chord{203-103}-.chord{203-103}-
    .chord{201-101}-.chord{201-101}-.chord{201-101}-
    .chord{203-103}/.chord{208-108}-.chord{206-106}-.chord{203-103}-.chord{203-103}.chord{2:\1:\};

There is a little change/addition at the last chorus but it is pretty obvious so I will let you to figure out what is
happening there.

The entire ``tlp`` for my transcription for this ``badass`` song:

    .song{"Better than you / The Hellacopters [By the grace of God]"}
    .transcriber{"Rafael Santiago"}

    .literal{"Intro lick(a):\n"}
    .literal{"\t(Listen to the introduction to figure out how these things are placed)"}
        .chord{201-101}/.chord{203-103}-.chord{203-103}-.chord{203-103}-.times{2}-|-
        .chord{201-101}/.chord{203-103}-.chord{203-103}-;


        .chord{306-205}/.chord{307-206}-.chord{307-206}-.chord{307-206}-.times{2}-|-
        .chord{306-205}/.chord{307-206}-.chord{307-206}-;

        .chord{305-205}.chord{3:b2:b}-.chord{305-205}.chord{3:r2:r}-
        .chord{303-203}-.chord{305-205}-.chord{303-203}-405-;

    .literal{"Base riff:\n"}
        .chord{50-40}-.chord{58-400-300}-.chord{58-400-300}-.chord{50-40}-.chord{58-48-38}-.chord{58-48-38}-.chord{58-48-38}-
        .chord{50-40}-.chord{58-48-300-201}-.chord{58-48-38-29}-.chord{58-400-300}-|-.chord{58-400-300}-.chord{58-400-300}-;

    .literal{"Intro lick(b):\n"}
        .chord{206-103}.chord{2:b}-.chord{206-103}.chord{2:r}-
        .chord{206-103}.chord{2:b}-.chord{206-103}.chord{2:r}-
        .chord{206-103}.chord{2:b}-.chord{206-103}.chord{2:r}-

        206/208-206-208-206-307-106~~-|-208-109p106-208-106-208-305-203-206p203-305-303-405~~-;

    .literal{"Chorus:\n"}
        .chord{50-40}-
        .mute{
            .chord{58-48-300-201}-------------------|-
            .chord{58-48-38-29}----------------
        }-|-;
        .chord{50-40}-.chord{58-400-300}-.chord{58-400-300}-.chord{50-40}-.chord{58-48-38}-.chord{58-48-38}-.chord{58-48-38}-
        .chord{50-40}-.chord{58-48-300-201}-.chord{58-48-38-29}-.chord{58-400-300}-|-.chord{58-400-300}-.chord{58-400-300}-;
        .chord{50-40}-
        .mute{.chord{58-48-300-201}-----------------}-|-
        .chord{50-40}-
        .mute{.chord{56-48-38}-----------------}-;

    .literal{"Lick(c):\n"}
        .chord{206-103}.chord{2:b}-.chord{206-103}.chord{2:r}-
        .chord{206-103}.chord{2:b}-.chord{206-103}.chord{2:r}-
        .chord{206-103}.chord{2:b}-.chord{206-103}.chord{2:r}-

        4:/400-300-29/200-300-38-300~~-;

    .literal{"Pre-bridge:\n"}
        .letring{.chord{58-400-300}--------------}-.chord{58-400-300}-;

    .literal{"\t(This part should be repeated twice)\n"}
        500h502-.chord{400-300}-.chord{400-300}-502-.chord{400-300}-|-
        .chord{302-202}.chord{3:b2:b}-.chord{302-202}.chord{3:r2:r}-
        .chord{300-200}-.chord{38-28}-|-

        58h500-.chord{48-38}-.chord{48-38}-500-.chord{48-38}-|-
        500h502-400-402-.chord{300-200}~~-|-

        500h502-.chord{400-300}-.chord{400-300}-502-.chord{400-300}-|-
        .chord{302-202}.chord{3:b2:b}-.chord{302-202}.chord{3:r2:r}-
        .chord{300-200}-.chord{38-28}-|-

        58h500-.chord{48-38}-.chord{48-38}-500-.chord{48-38}-|-
        .chord{302-202}.chord{3:b2:b}-.chord{302-202}.chord{3:r2:r}-.chord{300-200}~~~~-;

    .literal{"Solo(i):\n"}
        .chord{302-200}.chord{3:b}-.chord{200-100}-203-200-302-300-402-300-303-302-300~-|-
        /300-29-200-101b-|-101~~--101~~-|-101b102r101p18-101p18-29-300-|-307-206-208-106-109-108-106-
        108b110r108-106-108-106-101-103-;-309-208-.letring{210b-110-210r-110-210r-110-210r}-208~~-;

    .literal{"Solo(ii):\n"}
        3:/305-.chord{206-106}@@@@@@@-|-
        .chord{206-106}-.chord{205-105}-.chord{203-103}-.chord{306-206}-
        .chord{305-205}-.chord{303-203}-405-.chord{303-203}-305b-;
        .letring{203h206p203-305b}-.times{3}-|-206/208~~-|-307-208-106-208-108/110\108-|-
        .chord{305-203}.chord{3:b}-203-305-303-305-303-.times{2}-;-405-403-403h405-.letring{303b---101------}-;

    .literal{"Clean:\n"}
        .letring{
            .chord{53-43-35-26}-26-35-.chord{53-43-33-24}-.chord{53-43-33-24}-.chord{53-43-33-24}
        }-
        .chord{33-23}-.chord{31-21}-.chord{43-32-21}-21-32-;
        .letring{
            .chord{53-43-35-26}-26-35-.chord{53-43-33-24}-.chord{53-43-33-24}-.chord{53-43-33-24}
        }-
        .chord{33-23}-.chord{31-21}-.chord{43-32-21}-53h56-43-;

    .literal{"Final lick:\n"}
        .chord{2:/1:/}.chord{203-103}-.chord{203-103}-
        .chord{201-101}-.chord{201-101}-.chord{201-101}-
        .chord{203-103}/.chord{208-108}-.chord{206-106}-.chord{203-103}-.chord{203-103}.chord{2:\1:\};

As ``Dewey Finn`` would say:

>>"Now rise your goblet of rock. It's toast to those who rock!"
>>>![school-of-rock-scene](https://github.com/rafael-santiago/tulip/blob/master/etc/school-of-rock.gif "A famous funny scene from the movie School of Rock")

Haha, here is the output tablature (two steps down, of course... I am crazy, do you remember?):

    Song: Better than you / The Hellacopters [By the grace of God]
    Transcribed by: Rafael Santiago

    Intro lick(a):

	    (Listen to the introduction to figure out how these things are placed)

                          2X
            |-11/13-13-13--|-11/13-13-|
            |-11/13-13-13--|-11/13-13-|
            |--------------|----------|
            |--------------|----------|
            |--------------|----------|
            |--------------|----------|

                          2X
            |--------------|----------|
            |-15/16-16-16--|-15/16-16-|
            |-16/17-17-17--|-16/17-17-|
            |--------------|----------|
            |--------------|----------|
            |--------------|----------|

            |---------------------|
            |-15b-15r-13-15-13----|
            |-15b-15r-13-15-13----|
            |------------------15-|
            |---------------------|
            |---------------------|

    Base riff:


            |---------------------------|-------|
            |-------------------11-9----|-------|
            |---10-10---8-8-8---10-8-10-|-10-10-|
            |-0-10-10-0-8-8-8-0--8-8-10-|-10-10-|
            |-0--8--8-0-8-8-8-0--8-8--8-|--8--8-|
            |---------------------------|-------|

    Intro lick(b):


            |-13--13--13--13--13--13--------------------16~~-|----19p16----16----------|
            |-16b-16r-16b-16r-16b-16r-16/18-16-18-16---------|-18-------18----18----13-|
            |----------------------------------------17------|-------------------15----|
            |------------------------------------------------|-------------------------|
            |------------------------------------------------|-------------------------|
            |------------------------------------------------|-------------------------|

            |------------------|
            |-16p13------------|
            |-------15-13------|
            |-------------15~~-|
            |------------------|
            |------------------|

    Chorus:


                pm..................... ..................

            |--------------------------|-------------------|
            |-----11-------------------|-9-----------------|
            |-----10-------------------|-8-----------------|
            |-0----8-------------------|-8-----------------|
            |-0----8-------------------|-8-----------------|
            |--------------------------|-------------------|

            |---------------------------|-------|
            |-------------------11-9----|-------|
            |---10-10---8-8-8---10-8-10-|-10-10-|
            |-0-10-10-0-8-8-8-0--8-8-10-|-10-10-|
            |-0--8--8-0-8-8-8-0--8-8--8-|--8--8-|
            |---------------------------|-------|

                pm...................     pm..................

            |-------------------------|-----------------------|
            |-----11------------------|-----------------------|
            |-----10------------------|-----8-----------------|
            |-0----8------------------|-0---8-----------------|
            |-0----8------------------|-0---6-----------------|
            |-------------------------|-----------------------|

    Lick(c):


            |-13--13--13--13--13--13------------------------|
            |-16b-16r-16b-16r-16b-16r--------9/10-----------|
            |-----------------------------10------10-8-10~~-|
            |-------------------------/10-------------------|
            |-----------------------------------------------|
            |-----------------------------------------------|

    Pre-bridge:


            lr................

            |-----------------------|
            |-----------------------|
            |---10---------------10-|
            |---10---------------10-|
            |----8----------------8-|
            |-----------------------|

	    (This part should be repeated twice)


            |-------------------|--------------|---------------|------------------|----------|
            |-------------------|-12b-12r-10-8-|---------------|-------------10~~-|----------|
            |-------10-10----10-|-12b-12r-10-8-|------8-8----8-|-------------10~~-|-------10-|
            |-------10-10----10-|--------------|------8-8----8-|-------10-12------|-------10-|
            |-10h12-------12----|--------------|-8h10-----10---|-10h12------------|-10h12----|
            |-------------------|--------------|---------------|------------------|----------|

            |----------|--------------|---------------|----------------|
            |----------|-12b-12r-10-8-|---------------|-12b-12r-10~~~~-|
            |-10----10-|-12b-12r-10-8-|------8-8----8-|-12b-12r-10~~~~-|
            |-10----10-|--------------|------8-8----8-|----------------|
            |----12----|--------------|-8h10-----10---|----------------|
            |----------|--------------|---------------|----------------|

    Solo(i):


            |-----10-----------------------------|----------11b-|-11~~--11~~-|-11b12r11p8-|
            |-10--10-13-10-----------------------|-----9-10-----|------------|------------|
            |-12b----------12-10----10-13-12-10~-|-/10----------|------------|------------|
            |--------------------12--------------|--------------|------------|------------|
            |------------------------------------|--------------|------------|------------|
            |------------------------------------|--------------|------------|------------|

            |-11p8------|----------16-19-18-16-18b20r18-16-18-16-11-13-|
            |------9----|----16-18-------------------------------------|
            |--------10-|-17-------------------------------------------|
            |-----------|----------------------------------------------|
            |-----------|----------------------------------------------|
            |-----------|----------------------------------------------|

                    lr.........................

            |-------------20-----20-----20----------|
            |----18---20b----20r----20r----20r-18~~-|
            |-19------------------------------------|
            |---------------------------------------|
            |---------------------------------------|
            |---------------------------------------|

    Solo(ii):


            |-----16--16--16--16--16--16--16--16-|-16-15-13--------------------|
            |-----16--16--16--16--16--16--16--16-|-16-15-13-16-15-13----13-----|
            |-/15--------------------------------|----------16-15-13----13-15b-|
            |------------------------------------|-------------------15--------|
            |------------------------------------|-----------------------------|
            |------------------------------------|-----------------------------|

              lr.............

                             3X                                                     2X
            |-----------------|---------|-------16----18/20\18-|--------------------|
            |---13h16p13------|-16/18~~-|----18----18----------|-13--13-------------|
            |------------15b--|---------|-17-------------------|-15b----15-13-15-13-|
            |-----------------|---------|----------------------|--------------------|
            |-----------------|---------|----------------------|--------------------|
            |-----------------|---------|----------------------|--------------------|

                          lr..............

            |---------------------11------|
            |-----------------------------|
            |---------------13b-----------|
            |-15-13-13h15-----------------|
            |-----------------------------|
            |-----------------------------|

    Clean:


              lr............

            |-------------------------|
            |---6-6---4-4-4-3-1-1-1---|
            |---5---5-3-3-3-3-1-2---2-|
            |---3-----3-3-3-----3-----|
            |---3-----3-3-3-----------|
            |-------------------------|

              lr............

            |---------------------------|
            |---6-6---4-4-4-3-1-1-------|
            |---5---5-3-3-3-3-1-2-------|
            |---3-----3-3-3-----3-----3-|
            |---3-----3-3-3-------3h6---|
            |---------------------------|

    Final lick:


            |-/13-13-11-11-11-13/18-16-13-13\-|
            |-/13-13-11-11-11-13/18-16-13-13\-|
            |---------------------------------|
            |---------------------------------|
            |---------------------------------|
            |---------------------------------|

If you have a military ``peaked cap`` and a alpine white ``SG Custom 1961``... go ahead... it is time to use it.

No, the most important advice is: play it loud, distorted and fast. Otherwise will not work. This is ``"blitzkrieg rock"`` kid! ;)

I want to show you a nice way for using the ``part`` and ``repeat`` tags. For doing it I will pick a ``Motörhead``'s song
called ``Whorehouse Blues``.

This song closes them album ``Inferno``. It is basically a twelve-bar blues (shuffle) played with 3 acoustic guitars (I think).
I love this song. I personally dislike a bit acoustic guitars, but this song is pretty cool. So, I like to understand
it as a "intro riff", "base", "solo", "base variation", "turnaround ending". I adjusted some things to fits better to
my tunning and preferences ;-P... If you want to use it good luck!

For the ``intro riff`` we have the following:

    53-53-41-53-|.part{intro&riff_1}-
    51-53-51-53-43-41-51-53-|.part{intro&riff_2}-
    33h35-.letring{24-35~~}-|.part{intro&riff_3}-;

The ``base`` constantly backs to the ``intro riff``. Then I found this way to denote it:

    .chord{53-43-35}-.chord{53-43-35}---.chord{53-43-37}---.chord{43-33-24}---.chord{53-43-37}-.chord{53-43-35}.times{2}-|-

    .repeat{intro&riff_1}
    .repeat{intro&riff_2}
    .repeat{intro&riff_3}-;

This differs a bit from the original way of playing the chorus. But this way mix up some "rockabilly" echoes into the chorus:

    63-63-51-52-.times{3}-|-41-53-51-63-|-

    .repeat{intro&riff_1}
    .repeat{intro&riff_2}
    .repeat{intro&riff_3}-;


Well, the solo is a far approximation ;)

    300/302-203-302-201-39-28-400-300-400-38~~-400~~-|-

    300b--201-.times{3}-|-38-400~~-|-

    .chord{500-400-302-203}-.chord{500-400-300-201}-.chord{500-400-302-203}-;

    .letring{105-206}~~.times{3}-|-

    3:/305-203-206-203-305-203-206-|-

    3:/305-203-206-105p103-206-203-305-203-206-;

    206~-203~~-|-

    207\206\205---203-205b-205r-203-|-

    203-205b-205b-205b-103b-|-

    3:/302-201-203-103b-|-101h102p101-203-201-302-203-|-201-302-300-402-300-402-38-400~~-|-

    3:/302-203-302-201-39-28-;-

    .chord{302-201-102}@@-|-

    .chord{202-102}\.chord{201-101}-.chord{19-29}-|-.chord{201-101}b-.chord{201-101}r-.chord{29-19}.times{2}-|-

    28-201b-18-201b-18-201b-18-18h101p18-201-28-201\200p29-300-38-300/301\300-38-300-400-400-38h39-.chord{18-28}\-|-

    .chord{302-201-102}@@-|-

    .chord{29-19}-.chord{202-102}-.chord{201-101}-.chord{29-19}-.chord{302-202}\.chord{301-201}---.chord{39-29}-;-

    307-206-307-206-307~-|-307~-307~-307~-307~-307~-;

    104b--104r--104b--104r--104~~~~~~~~~~-|-

    102-104-106-104-102-204-102-204-202-303-202-303~~~-|-

    401h403-204-301-303~~-|-

    305h307-206-307-208-307-|-

    307-206-203-|-206b---206r---|-206b---206r---|-

    206-203~~~~~-203~~~-203~~~-203~~~-203~~~-203~~~~~-203~~~~~-;

The base variation is pretty simple:

    .chord{53-43-35}-.chord{53-43-35}---.chord{53-43-37}---.chord{51-41-33-24}--.chord{51-41-33-24}-|-;

I like the ending in the following way:

    53/55-45-43-41-53-51-63-41-53-51-51h53-|-

    .letring{.chord{35-18}-------------------------------------}

The entire script for this ``badass`` blues:

    .song{"Whorehouse Blues - Motörhead [Inferno]"}
    .transcriber{"Rafael Santiago"}

    .literal{"<Intro & Riff>"}

        53-53-41-53-|.part{intro&riff_1}-
        51-53-51-53-43-41-51-53-|.part{intro&riff_2}-
        33h35-.letring{24-35~~}-|.part{intro&riff_3}-;

    .literal{"<Base>"}

    .literal{"\tBasically you should \"shuffle\" twice and back to the intro riff."}
    .literal{"\tThe ***whole base*** is done twice."}

        .chord{53-43-35}-.chord{53-43-35}---.chord{53-43-37}---.chord{43-33-24}---.chord{53-43-37}-.chord{53-43-35}.times{2}-|-

        .repeat{intro&riff_1}
        .repeat{intro&riff_2}
        .repeat{intro&riff_3}-;

    .literal{"<Chorus>"}

        63-63-51-52-.times{3}-|-41-53-51-63-|-

        .repeat{intro&riff_1}
        .repeat{intro&riff_2}
        .repeat{intro&riff_3}-;

    .literal{"<Solo>"}

        300/302-203-302-201-39-28-400-300-400-38~~-400~~-|-

        300b--201-.times{3}-|-38-400~~-|-

        .chord{500-400-302-203}-.chord{500-400-300-201}-.chord{500-400-302-203}-;

        .letring{105-206}~~.times{3}-|-

        3:/305-203-206-203-305-203-206-|-

        3:/305-203-206-105p103-206-203-305-203-206-;

        206~-203~~-|-

        207\206\205---203-205b-205r-203-|-

        203-205b-205b-205b-103b-|-

        3:/302-201-203-103b-|-101h102p101-203-201-302-203-|-201-302-300-402-300-402-38-400~~-|-

        3:/302-203-302-201-39-28-;-

        .chord{302-201-102}@@-|-

        .chord{202-102}\.chord{201-101}-.chord{19-29}-|-.chord{201-101}b-.chord{201-101}r-.chord{29-19}.times{2}-|-

        28-201b-18-201b-18-201b-18-18h101p18-201-28-201\200p29-300-38-300/301\300-38-300-400-400-38h39-.chord{18-28}\-|-

        .chord{302-201-102}@@-|-

        .chord{29-19}-.chord{202-102}-.chord{201-101}-.chord{29-19}-.chord{302-202}\.chord{301-201}---.chord{39-29}-;-

        .literal{"\tPick several times (listen to the song) the last note from this segment holding"}

        307-206-307-206-307~-|-307~-307~-307~-307~-307~-;

        104b--104r--104b--104r--104~~~~~~~~~~-|-

        102-104-106-104-102-204-102-204-202-303-202-303~~~-|-

        401h403-204-301-303~~-|-

        305h307-206-307-208-307-|-

        307-206-203-|-206b---206r---|-206b---206r---|-

        206-203~~~~~-203~~~-203~~~-203~~~-203~~~-203~~~~~-203~~~~~-;

    .literal{"Base variation"}

        .chord{53-43-35}-.chord{53-43-35}---.chord{53-43-37}---.chord{51-41-33-24}--.chord{51-41-33-24}-|-;

    .literal{"Ending turnaround"}

        53/55-45-43-41-53-51-63-41-53-51-51h53-|-

        .letring{.chord{35-18}-------------------------------------}


The resultant tablature is what follows:

    Song: Whorehouse Blues - Motörhead [Inferno]
    Transcribed by: Rafael Santiago

    <Intro & Riff>

                                              lr......

            |---------|-----------------|-------------|
            |---------|-----------------|-------4-----|
            |---------|-----------------|-3h5-----5~~-|
            |-----1---|---------3-1-----|-------------|
            |-3-3---3-|-1-3-1-3-----1-3-|-------------|
            |---------|-----------------|-------------|

    <Base>
	    Basically you should "shuffle" twice and back to the intro riff.
	    The ***whole base*** is done twice.

                                                                  lr......

                               2X
            |-------------------|---------|-----------------|-------------|
            |-----------4-------|---------|-----------------|-------4-----|
            |-5-5---7---3---7-5-|---------|-----------------|-3h5-----5~~-|
            |-3-3---3---3---3-3-|-----1---|---------3-1-----|-------------|
            |-3-3---3-------3-3-|-3-3---3-|-1-3-1-3-----1-3-|-------------|
            |-------------------|---------|-----------------|-------------|

    <Chorus>

                                                                   lr......

                      3X
            |----------|---------|---------|-----------------|-------------|
            |----------|---------|---------|-----------------|-------4-----|
            |----------|---------|---------|-----------------|-3h5-----5~~-|
            |----------|-1-------|-----1---|---------3-1-----|-------------|
            |-----1-2--|---3-1---|-3-3---3-|-1-3-1-3-----1-3-|-------------|
            |-3-3------|-------3-|---------|-----------------|-------------|

    <Solo>

                                                             3X
            |--------------------------------------|----------|--------|----------|
            |-------13----11---8-------------------|------11--|--------|-13-11-13-|
            |-10/12----12----9------10----8~~------|-10b------|-8------|-12-10-12-|
            |--------------------10----10-----10~~-|----------|---10~~-|-10-10-10-|
            |--------------------------------------|----------|--------|-10-10-10-|
            |--------------------------------------|----------|--------|----------|

              lr.......

                       3X
            |---15------|-----------------------|-----------15p13----------------|
            |------16~~-|-----13-16-13----13-16-|-----13-16-------16-13----13-16-|
            |-----------|-/15----------15-------|-/15-------------------15-------|
            |-----------|-----------------------|--------------------------------|
            |-----------|-----------------------|--------------------------------|
            |-----------|-----------------------|--------------------------------|

            |----------|--------------------------|----------------13b-|-----------13b-|
            |-16~-13~~-|-17\16\15---13-15b-15r-13-|-13-15b-15b-15b-----|-----11-13-----|
            |----------|--------------------------|--------------------|-/12-----------|
            |----------|--------------------------|--------------------|---------------|
            |----------|--------------------------|--------------------|---------------|
            |----------|--------------------------|--------------------|---------------|

            |-11h12p11-------------|--------------------------|------------------|
            |----------13-11----13-|-11-----------------------|-----13----11---8-|
            |----------------12----|----12-10----10----8------|-/12----12----9---|
            |----------------------|----------12----12---10~~-|------------------|
            |----------------------|--------------------------|------------------|
            |----------------------|--------------------------|------------------|

                                              2X
            |-12--12--12-|-12\11-9-|-11b-11r-9-|-------8-----8-----8-8h11p8-----------------|
            |-11--11--11-|-12\11-9-|-11b-11r-9-|-8-11b---11b---11b----------11-8-11\10p9----|
            |-12--12--12-|---------|-----------|-----------------------------------------10-|
            |------------|---------|-----------|--------------------------------------------|
            |------------|---------|-----------|--------------------------------------------|
            |------------|---------|-----------|--------------------------------------------|

            |---------------------------8\-|-12--12--12-|-9-12-11-9-----------|
            |---------------------------8\-|-11--11--11-|-9-12-11-9-12\11---9-|
            |-8-10/11\10-8-10-------8h9----|-12--12--12-|-----------12\11---9-|
            |-----------------10-10--------|------------|---------------------|
            |------------------------------|------------|---------------------|
            |------------------------------|------------|---------------------|

    	    Pick several times (listen to the song) the last note from this segment holding

            |-----------------|---------------------|
            |----16----16-----|---------------------|
            |-17----17----17~-|-17~-17~-17~-17~-17~-|
            |-----------------|---------------------|
            |-----------------|---------------------|
            |-----------------|---------------------|

            |-14b--14r--14b--14r--14~~~~~~~~~~-|-12-14-16-14-12----12-------------------|
            |----------------------------------|----------------14----14-12----12-------|
            |----------------------------------|----------------------------13----13~~~-|
            |----------------------------------|----------------------------------------|
            |----------------------------------|----------------------------------------|
            |----------------------------------|----------------------------------------|

            |------------------|-------------------|----------|-------------|-------------|
            |-------14---------|-------16----18----|----16-13-|-16b---16r---|-16b---16r---|
            |----------11-13~~-|-15h17----17----17-|-17-------|-------------|-------------|
            |-11h13------------|-------------------|----------|-------------|-------------|
            |------------------|-------------------|----------|-------------|-------------|
            |------------------|-------------------|----------|-------------|-------------|

            |----------------------------------------------------|
            |-16-13~~~~~-13~~~-13~~~-13~~~-13~~~-13~~~~~-13~~~~~-|
            |----------------------------------------------------|
            |----------------------------------------------------|
            |----------------------------------------------------|
            |----------------------------------------------------|

    Base variation

            |----------------|
            |-----------4--4-|
            |-5-5---7---3--3-|
            |-3-3---3---1--1-|
            |-3-3---3---1--1-|
            |----------------|

    Ending turnaround

                                          lr......................................

            |---------------------------|---8-------------------------------------|
            |---------------------------|-----------------------------------------|
            |---------------------------|---5-------------------------------------|
            |-----5-3-1-------1---------|-----------------------------------------|
            |-3/5-------3-1-----3-1-1h3-|-----------------------------------------|
            |---------------3-----------|-----------------------------------------|

Let's end with a ``standard rock`` called ``Louie Louie``, this song is pretty known. Maybe, in a galaxy far, far away, Aliens
can be listening to it loudly while getting crazy with virtuous ``air-guitar`` sessions.

For this last sample I will do something different. I will show you parts of the song previously written down using "numeric
cipher" and so I will let for the interested reader the task of comparing my drafts over the shown ``Tulip`` code. In the
end they will cracked the code of the code from the code. Or more or less it! ;)

The used version of ``Louie Louie`` is the ``Motörhead``'s alternate version (I love the guitar solo from this one).

My *shitty-drafts* for this song (do not mind I transcribed the solo first, sometimes, I am such a crazy person) follows
depicted in the **Figure 2**, **Figure 3**, **Figure 4**, **Figure 5** and **Figure 6**.

**Figure 2**: See? I told you...

![Louie Louie transcription personal draft part i](https://github.com/rafael-santiago/tulip/blob/master/etc/louie-louie-draft-0.jpg)

**Figure 3**: Here goes my guitar solo approximation.

![Louie Louie transcription personal draft part ii](https://github.com/rafael-santiago/tulip/blob/master/etc/louie-louie-draft-1.jpg)

**Figure 4**: Continuing the guitar solo.

![Louie Louie transcription personal draft part iii](https://github.com/rafael-santiago/tulip/blob/master/etc/louie-louie-draft-2.jpg)

**Figure 5**: Here you can see the intro riff and the chorus.

![Louie Louie transcription personal draft part iv](https://github.com/rafael-santiago/tulip/blob/master/etc/louie-louie-draft-3.jpg)

**Figure 6**: What you should do during the verses and a general idea for the ending lick.

![Louie Louie transcription personal draft part v](https://github.com/rafael-santiago/tulip/blob/master/etc/louie-louie-draft-4.jpg)

Here is the entire ``Tulip`` code for this song (two whole steps down, you already know why...):

    .song{"Louie Louie (Alternate Version) / Motorhead [Some EP which I can not rememeber by now, around 1979]"}
    .transcriber{"Rafael Santiago"}

    .literal{"[Riff intro]"}
        .literal{"\tRepeats it over and over..."}
        .chord{59-401-301}-|-.chord{69-401-501}-.chord{69-401-501}-.chord{69-401-501}-|-
        .chord{59-401-301}-.chord{59-401-301}-|-.chord{501-403-303}-.chord{501-403-303}-.chord{501-403-303}-|-
        .chord{59-401-301}-;

    .literal{"[Verses]"}
        .literal{"\tThere are some open intervals here. It is just a personal preference..."}
        .chord{60-50}-.chord{69-501-401}-|-.chord{69-501-401}-.chord{59-501-401}-|-
        .chord{50-40}-.chord{501-403-303}-.chord{501-403-303}-.chord{501-403-303}-|-
        .chord{59-401-301}-;

    .literal{"[Chorus]"}
        .literal{"\tHere you can also omit the open intervals."}
        .chord{60-50}-.chord{69-501-401}-.chord{69-501-401}-.chord{69-501-401}-|-
        .chord{59-401-301}-.chord{59-401-301}-|-.chord{50-40}-.chord{501-403-303}-.chord{501-403-303}-.chord{501-403-303}-|-
        .chord{59-401-301}-.chord{59-401-301}-;

    .literal{"[Solo]"}
        59-501-49-401-.chord{39-29}b-401-.times{2}-|-

        59-501-49-401-39-302~~~~-|-

        .chord{19-29}-.chord{202-302}-.chord{301-201}-.chord{39-29}-401-.chord{301-201}-
        .chord{29-39}-401-49-401-49-501-500-59-57-59-47-49-.chord{3:/2:/}.chord{303-202}~~-|-

        .letring {
            .chord{302-201}/.chord{303-202}-202~-303~-
            .chord{302-201}/.chord{303-202}-202-.chord{302-201}/.chord{303-202}-.chord{302-201}/.chord{303-202}
        }-.times{3}-|-

        3:/308-207-209b--209r-207-209-308-|-

        207-209b--209r-207-209-.chord{209-106}.chord{2:b}--207-308-207-209p207-308-207-308~~-|-/308-207-209-;-
        .letring {
            209b--109
        }-.times{10}-|-

        104b---104r-104p102-204-.times{3}-|-

        301/303-202-102-204~-|-

        3:/308-207-209-;-

        .letring {
            209b--111
        }-.times{70000}-;-

        109b----------110~~~~~~~~-|-
        111-109-106-.chord{209-106}.chord{2:b}-210-|-
        106-111-109-106-.chord{209-106}.chord{2:b}-106-.chord{209-106}.chord{2:b}-.chord{209-106}.chord{2:~}.chord{2:~}-;

        .literal{"[Final lick]"}
            .literal{"\tSomething to be developed over.\n"}
            59-501-49-401-39-301-302-200-202-204-102-104-104b----104r-104p102-204~~-


It is a simple and cool song. Sorry for my solo approximations, etc, etc... Follows the tab for it:

    Song: Louie Louie (Alternate Version) / Motorhead [Some EP which I can not rememeber by now, around 1979]
    Transcribed by: Rafael Santiago

    [Riff intro]
	    Repeats it over and over...

            |----|----------|-------|----------|----|
            |----|----------|-------|----------|----|
            |-11-|----------|-11-11-|-13-13-13-|-11-|
            |-11-|-11-11-11-|-11-11-|-13-13-13-|-11-|
            |--9-|-11-11-11-|--9--9-|-11-11-11-|--9-|
            |----|--9--9--9-|-------|----------|----|

    [Verses]
    	    There are some open intervals here. It is just a personal preference...

            |------|-------|------------|----|
            |------|-------|------------|----|
            |------|-------|---13-13-13-|-11-|
            |---11-|-11-11-|-0-13-13-13-|-11-|
            |-0-11-|-11-11-|-0-11-11-11-|--9-|
            |-0--9-|--9----|------------|----|

    [Chorus]
	    Here you can also omit the open intervals.

            |------------|-------|------------|-------|
            |------------|-------|------------|-------|
            |------------|-11-11-|---13-13-13-|-11-11-|
            |---11-11-11-|-11-11-|-0-13-13-13-|-11-11-|
            |-0-11-11-11-|--9--9-|-0-11-11-11-|--9--9-|
            |-0--9--9--9-|-------|------------|-------|

    [Solo]

                              2X
            |------------------|--------------------|-9-------------------------------------|
            |-----------9b-----|--------------------|-9-12-11-9----11-9---------------------|
            |-----------9b-----|-----------9-12~~~~-|---12-11-9----11-9---------------------|
            |------9-11----11--|------9-11----------|-----------11------11-9-11-9-----------|
            |-9-11-------------|-9-11---------------|-----------------------------11-10-9-7-|
            |------------------|--------------------|---------------------------------------|

                            lr....................................

                                                                 3X
            |-------------|---------------------------------------|-----------------------|
            |-------/12~~-|---11/12-12~-----11/12-12-11/12-11/12--|-----17-19b--19r-17-19-|
            |-------/13~~-|---12/13-----13~-12/13----12/13-12/13--|-/18-------------------|
            |---7-9-------|---------------------------------------|-----------------------|
            |-9-----------|---------------------------------------|-----------------------|
            |-------------|---------------------------------------|-----------------------|

            |----|-------------------16-----------------------------|-----------|
            |----|-17-19b--19r-17-19-19b--17----17-19p17----17------|-----17-19-|
            |-18-|---------------------------18----------18----18~~-|-/18-------|
            |----|--------------------------------------------------|-----------|
            |----|--------------------------------------------------|-----------|
            |----|--------------------------------------------------|-----------|

              lr.........

                        10X                   3X
            |--------19--|-14b---14r-14p12-----|----------12-----|-----------|
            |---19b------|-----------------14--|-------12----14~-|-----17-19-|
            |------------|---------------------|-11/13-----------|-/18-------|
            |------------|---------------------|-----------------|-----------|
            |------------|---------------------|-----------------|-----------|
            |------------|---------------------|-----------------|-----------|

              lr.........

                        70000X
            |--------21--|
            |---19b------|
            |------------|
            |------------|
            |------------|
            |------------|

            |-19b----------20~~~~~~~~-|-21-19-16-16-----|-16-21-19-16-16--16-16--16---|
            |-------------------------|----------19b-20-|-------------19b----19b-19~~-|
            |-------------------------|-----------------|-----------------------------|
            |-------------------------|-----------------|-----------------------------|
            |-------------------------|-----------------|-----------------------------|
            |-------------------------|-----------------|-----------------------------|

    [Final lick]
    	    Something to be developed over.


            |----------------------------12-14-14b----14r-14p12------|
            |-------------------10-12-14------------------------14~~-|
            |-----------9-11-12--------------------------------------|
            |------9-11----------------------------------------------|
            |-9-11---------------------------------------------------|
            |--------------------------------------------------------|

I think it is enough. With the presented samples you can easily master the ``Tulip`` if you really wants to.

Now you need to know how to process the code written inside the ``Tulip`` in order to produce your tabs.

Keep reading...

### How to verify a TLP file?

In order to generate a tablature from a ``tlp`` source code, this code needs to be correct. Any error can exist inside
of it, otherwise the tablature will not be generated. The way to verify it is calling ``Tulip`` with the following command line:

```
tulip-user@SOMEWHERE:~/over/the/rainbow# tulip --tlp=sample-song.tlp
```

This could be called ``the compilation command``.

In the file is ok nothing will be returned by the ``Tulip`` and it exits with zero code. Otherwise if found any error, this
error will be reported and the ``Tulip`` will exit with a non-zero code.

The above shown command only will compile your ``Tulip`` source code, no tablature will be maked.

This command tends to be useful only when you just want to know that any change in the code has no errors. In fact, the
output tablature can be easily re-assembled. By this fact, if you are intending to versioning your codes with some
``source control management tool``, obviously you do not need for versioning the tablatures, only the ``.tlp`` source files.

### Using Tulip's processors

The ``Tulip``'s processors are invoked through the option ``--out=<file>``. They must be used when you intends to generate
some tablature output from your input code. The following command should be used to generated a tablature:

```
tulip-user@SOMEWHERE:~/over/the/rainbow# tulip --tlp=sample-song.tlp --out=sample-song.txt
```

This could be called ``the typesetting command``.

According to the above command sample, the code ``sample-song.tlp`` will be compiled and if it has no errors the tablature
``sample-song.txt`` will be generated under the current directory.

The type of processors are chosen by the output extension, so a ``.txt`` output produces a tablature text-based.

If you chose a unsupported file extension for the output you are warned about. Until now we have the ``.txt`` processor,
the processors related with ``postscript (.ps)`` and ``portable document file (.pdf)`` are still being developed. You still
do not have any code related with them here in this public repository.

Then after all, the generation of a tablature file also involves the compilation of the source which will be used for this
generation.

### Changing the behavior of the processor

It is possible to change the behavior of the processor in some cases using explict options.

Fistly, let's see what behavoir's changes we can apply for a ``Tulip`` processor. It follows listed into the **Table 4**.
Inside the ``Tulip`` a processor is responsible to care about the tablature typesetting, so when you use an option present
in the **Table 4** you are directly influencing the output typesetting.

**Table 4**: Options for processor's behavior changing.
|       **Option**                 |           **Handy for**                                                                  |        **Setting sample**                                   |
|:--------------------------------:|-----------------------------------------------------------------------------------------:|:-----------------------------------------------------------:|
| ``fretboard-size``               | Expresses the size of the fretboard diagram in columns                                   | ``fretboard-size=320``                                      |
| ``fretboard-style``              | Sets one of the two available styles of fretboard                                        | ``fretboard-style=normal`` or ``fretboard-style=continuous``|
| ``close-tab-to-save``            | Sets/Unsets the option which flags the closement of a fretboard diagram before saving it | ``close-tab-to-save=no``                                    |
| ``indentation-deepness``         | Indicates the amount of spaces added in order to indent the tablature                    | ``indentation-deepness=2``                                  |
| ``include-tab-notation``         | Includes at the beginning of the tablature an used-techniques reference table            | ``include-tab-notation=yes``                                |
| ``cut-tab-on-the-last-note``     | Cuts the tablature output based on the effective last note put on it                     | ``cut-tab-on-the-last-note=no``                             |
| ``add-tunning-to-the-fretboard`` | Adds the tunning of each string in the fretboard diagram                                 | ``add-tunning-to-the-fretboard=yes``                        |
| ``tunning``                      | When you want to add it to the fretboard and you are using an alternate tunning          | ``tunning=D-G-D-G-B-d``                                     |

The ``tunning`` option must be expressed from the sixth string to the first. Using note ciphers separated by single dashes.

When the ``fretboard-style`` is set to ``continous`` the processor does not mind for closing the saved tab diagram (with bars)
unless it is the last one.

These options can be stored into a special file called ``.tulipprefs``. Each line of this file should be a valid option setting.
Follows a content sample of it:

```
fretboard-size=320
add-tunning-to-the-fretboard=yes
tunning=Eb-Ab-Db-Gb-Bb-Eb
fretboard-style=continuous
```

The file ``.tulipprefs`` should be created under the path where you intend to call the ``Tulip``.

By the way, if you are wanting to be tidy, using some ``SCM`` software, the ``.tulipprefs`` could be a relevant file for
versioning with them besides the own source codes, of course.

Another way to set this options is supplying them for the ``Tulip`` through the command line. In this case, you need to add
double dashes to these options as a prefix, take a look:

```
tulip-user@SOMEWHERE:~/over/the/rainbow# tulip --tlp=sample.tlp --out=sample.txt --fretboard-size=180 --tunning=E-A-D-G-B-e
```

An important thing to say here is that an option supplied by command line overwrites an option set into the ``.tulipprefs``.
Then, even with a ``.tulipprefs`` under the current directory, for a specific typesetting task, you still can change the
processor behavior using directly the command line.

## And so?

Well, I hope you enjoy using this software. Maybe at the beginning, seeing a bunch of numbers, mixed up with crazy tags
could make you a little bit dizzy. Bear in mind that once learned the numeric cipher and the possible control tags,
the ``Tulip`` could be not useful but can make the transcription process much more entertaining, because you will start
seeing it in a more programmatic way, instead of dummy clicks on a simulated fretboard. In addition, it can bring more
method to your transcription workflow. Because you can add some version control over it. Looks, here is about producing
well-structured codes instead of abstract clicks chained on a "luser" interface, so you can edit this plain-file codes
everywhere you want to. Then, what about be more free and tidy with your transcriptions?

However, do not avoid learning about the ``Standard Music Notation (Sheet Music)``. It is rather important for any ``Musician`` whom wants
to evolute more and more. In fact, the tablature is a quite limited transcription form it is nice only to know about what you
"previously know" or at least have a far idea about what it should be.

That's all folks. Enjoy!

*By now turn off this tedious flat-computer and go playing your gorgeous-interesting-curvilinear-guitar, hurry up! ;)*
