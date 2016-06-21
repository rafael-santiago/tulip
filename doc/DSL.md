# The Tulip Language

Well, I created this tiny ``DSL`` in ``2005`` based on my personal preferences about ``music transcribing`` but I have never
formalized anything about since then.

The Language is a mix of an ancient music notation with some tablature notation and still some tag recursion. The details
about the design and choices can be found in the [``User Manual``](https://github.com/rafael-santiago/tulip/blob/master/doc/MANUAL.md).

## The *BNF*

Basically here is the possible productions that you should be able to do. The following *BNF data* it is not so well
normalized but I think that it is possible to produce a parser or at least avoid reading the ``User Manual`` in order to learn
how to produce some ``Tulip code``, if you have some formal knowledge about compilers, of course. Otherwise do not waste your
time freaking out your brain, it is better to move on to the [User Manual](https://github.com/rafael-santiago/tulip/blob/master/doc/MANUAL.md)
and forget this.

        <tlp-code>                ::= (<note> | <sep> | <save-point> | <once-more> | <tag-announce> <tag-code>)(<tlp-code)* |
                                      (<tlp-code>)*
        <note>                    ::= <string-number> <fret-number>
        <string-number>           ::= "1" | "2" | "3" | "4" | "5" | "6"
        <fret-number>             ::= (<number> | ":"<sep>)
        <number>                  ::= (<digit>)+
        <digit>                   ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
        <sep>                     ::= ("-" | "h" | "p" | "~" | "/" | "\" | "b" | "r" | "*" | "v" | "T")+
        <save-point>              ::= (";")+
        <once-more>               ::= ("@")+
        <tag-announce>            ::= "."
        <tag-code>                ::= <mute-tag-code>           |
                                      <letring-tag-code>        |
                                      <chord-tag-code>          |
                                      <strum-tag-code>          |
                                      <tremolopicking-tag-code> |
                                      <vibratowbar-tag-code>    |
                                      <trill-tag-code>          |
                                      <tunning-tag-code>        |
                                      <literal-tag-code>        |
                                      <times-tag-code>          |
                                      <song-tag-code>           |
                                      <transcriber-tag-code>    |
                                      <part-tag-code>           |
                                      <repeat-tag-code>
        <mute-tag-code>           ::= "mute" <recursive-tag-stmt-list>
        <letring-tag-code>        ::= "letring" <recursive-tag-stmt-list>
        <strum-tag-code>          ::= "strum" <recursive-tag-stmt-list>
        <tremolopicking-tag-code> ::= "tremolopicking" <recursive-tag-stmt-list>
        <vibratowbar-tag-code>    ::= "vibratowbar" <recursive-tag-stmt-list>
        <trill-tag-code>          ::= "trill" <recursive-tag-stmt-list>
        <tunning-tag-code>        ::= "tunning" <tag-begin> <tunning-code> <tag-end>
        <tunning-code>            ::= <note-cipher> | "-" <note-cipher>
        <literal-tag-code>        ::= "literal" <tag-begin> <string> <tag-end>
        <string>                  ::= """ (<US-ASCII-SET> | <string-escape-codes>)+ """
        <string-escape-codes>     ::= ("\n" | "\r" | "\n" | "\t" | "\\" | "\"")+
        <chord-tag-code>          ::= "chord" <tag-begin> <chord-stmt-list> <tag-end>
        <times-tag-code>          ::= "times" <tag-begin> <number> <tag-end>
        <song-tag-code>           ::= "song" <tag-begin> <string> <tag-end>
        <transcriber-tag-code>    ::= "trancriber" <tag-begin> <string> <tag-end>
        <part-tag-code>           ::= "part" <tag-begin> (<US-ASCII-SET-EXCEPTING-{-}-">)+ <tag-end>
        <repeat-tag-code>         ::= "repeat" <tag-begin> (<US-ASCII-SET-EXCEPTING-{-}-">)+ <tag-end>
        <chord-stmt-list>         ::= (<note><sep>)+ | <note>
        <note-cipher>             ::= "C"  | "D"  | "E"  | "F"  | "G"  | "A"  | "B"  | "C#" | "D#" |
                                      "F#" | "G#" | "A#" | "Db" | "Eb" | "Gb" | "Ab" | "Bb" | "c"  |
                                      "d"  | "e"  | "f"  | "g"  | "a"  | "b"  | "c#" | "d#" | "f#" |
                                      "g#" | "db" | "eb" | "gb" | "ab" | "bb"
        <tag-begin>               ::= "{"
        <tag-end>                 ::= "}"
        <recursive-tag-stmt-list> ::= <tag-begin> <tlp-code> <tag-end>

As you can see it is a pretty compact Language. Parse this is very straightforward.

Excepting the tags ``chord``, ``tunning``, ``literal``, ``song``, ``transcriber`` and ``times`` any other should support fully
recursion. What means allow a more internal definition of ``<tlp-code>`` as the tag's production.

The production ``<note-cipher>`` does not disallow the usage of a lower-case cipher in order to express a lower string tunning.
Nevertheless I think that use this is ugly as hell.
