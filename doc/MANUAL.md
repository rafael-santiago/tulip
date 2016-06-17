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

In the code shown above is using the ``separation bar`` or ``sep bar`` as I like to call. This is denoted
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
- If we have a tag, the tag and what this tag brings wrapped will be repeated.
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

I also use some kind of hybrid picking but its out of a tablature's scope. Again, the colors is up to you.

We got new ``tlp`` stuff in the presented base pattern. The first is the "fret abstraction". The guitar is not a science
this is an art due to it several times we face imprecise things. Sometimes slides have destination but can be done from any
source. For doing it you must use a note that only indicates the string number and replaces the fret region by a suggestive
question mark. Then, we have ``6?``, ``5?``, ``4?``, ``3?``, ``2?`` and ``1?`` as possible abstractions.

In the previous code we can see a "note" which acts as a ``string jump`` (in terms of code), we use a string jump to indicate
to the ``Tulip`` where we want to put the next separator that complements this "jump". A jump is denoted by
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
(what in programming we call ``string``). If you want to add a line break use ``\n`` sequence. For tabulations use ``\t``.

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
repeat this "highlighted" part using ``.repeat{Name of this Part}``. The code output is:

    Song: O Pato - (adapted from Stan Getz's version)
    Transcribed by: Rafael Santiago

             lr.................. lr......................

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

Now let's see a cool song from one of my favorite contemporary singers: ``Imelda May``. This song came from her
second album. It is called ``Feel me``. For sure that my transcription is pretty bad wrong but I like to play in this way.
In the way of a ``frustrated piano player``, ha-ha...

This song starts with a nice "piano's riff", I approximate it as follows:

        60--.chord{401-302-202-102}-.chord{305-205}-.chord{302-202}-.chord{304-204}-.chord{302-202}--
        502-504-402-404-.chord{302-202}-

The base is a mix of fingering and strumming, I think that the better way is listen to it and try to figure out:

        .strum{57-.chord{57-49-39-28-17}----67-.chord{67-59-47-38-200-17}----401-.chord{401-302-202-102}---.chord{400-301-200-101}---|-
         57-.chord{57-49-39-28-17}----67-.chord{67-59-47-38-200-17}----400-.chord{400-301-200-101}---60-.chord{500-48-39-28}---}




### Using Tulip's processors
