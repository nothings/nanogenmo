% can do this mid-music to change:
%   \set Staff.midiInstrument = #"pizzicato strings" 


#(set-global-staff-size 17)
\paper {
  indent = 3.0\cm  % space for instrumentName
  short-indent = 1.5\cm  % space for shortInstrumentName
}

\score {
  <<
    \new StaffGroup = "StaffGroup_woodwinds" <<
      \new Staff = "Staff_flute" {
        \set Staff.instrumentName = #"Flute"
        \set Staff.midiInstrument = #"flute"
        \set Staff.shortInstrumentName = #"Fl."
        % shortInstrumentName, midiInstrument, etc.
        % may be set here as well
        \fluteMusic
      }
      \new Staff = "Staff_clarinet" {
        \set Staff.instrumentName = #"Clarinet"
        \set Staff.midiInstrument = #"clarinet"
        \set Staff.shortInstrumentName = #"Cl."
        \clarinetMusic
      }
    >>
    \new StaffGroup = "StaffGroup_brass" <<
      \new Staff = "Staff_hornI" {
        \set Staff.instrumentName = #"Horn"
        \set Staff.midiInstrument = #"french horn"
        \set Staff.shortInstrumentName = #"Hn."
        \hornMusic
      }
      \new Staff = "Staff_trumpet" {
        \set Staff.instrumentName = #"Trumpet"
        \set Staff.midiInstrument = #"trumpet"
        \set Staff.shortInstrumentName = #"Tpt."
        \trumpetMusic
      }
    >>
    \new Staff = "Staff_harp" <<
      \set Staff.instrumentName = #"Harp"
      \set Staff.midiInstrument = #"harp"
      \set Staff.shortInstrumentName = #"Hp."
      \harpMusic
    >>
    \new PianoStaff <<
      \set PianoStaff.instrumentName = #"Piano"
      \set PianoStaff.shortInstrumentName = #"Piano"
      \new Staff { \pianoRHMusic }
      \new Staff { \pianoLHMusic }
    >>
    \new ChoirStaff = "ChoirStaff_choir" <<

      \new Staff = "Staff_soprano" {
        \set Staff.instrumentName = #"Soprano"
        \set Staff.shortInstrumentName = #"Sop."
        \new Voice = "soprano"
        \sopranoMusic
      }
      \new Lyrics \lyricsto "soprano" { \sopranoLyrics }

      \new Staff = "Staff_mezzo" {
        \set Staff.instrumentName = #"Mezzo"
        \set Staff.shortInstrumentName = #"Mez."
        \new Voice = "mezzo"
        \mezzoMusic
      }
      \new Lyrics \lyricsto "mezzo" { \mezzoLyrics }

      \new Staff = "Staff_bass" {
        \set Staff.instrumentName = #"Bass"
        \set Staff.shortInstrumentName = #"B."
        \new Voice = "bass"
        \vbassMusic
      }
      \new Lyrics \lyricsto "bass" { \vbassLyrics }

    >>
    \new StaffGroup = "StaffGroup_strings" <<
      \new GrandStaff = "GrandStaff_violins" <<
        \new Staff = "Staff_violinI" {
          \set Staff.instrumentName = #"Violin I"
          \set Staff.shortInstrumentName = #"Vln.1"
          \violinIMusic
        }
        \new Staff = "Staff_violinII" {
          \set Staff.instrumentName = #"Violin II"
          \set Staff.shortInstrumentName = #"Vln.2"
          \violinIIMusic
        }
      >>
      \new Staff = "Staff_viola" {
        \set Staff.instrumentName = #"Viola"
        \set Staff.shortInstrumentName = #"Vla."
        \violaMusic
      }
      \new Staff = "Staff_cello" {
        \set Staff.instrumentName = #"Cello"
        \set Staff.shortInstrumentName = #"Vc."
        \celloMusic
      }
      \new Staff = "Staff_bass" {
        \set Staff.instrumentName = #"Double Bass"
        \set Staff.shortInstrumentName = #"D.B."
        \bassMusic
      }
    >>
  >>
  \layout {
     \context {
        \Staff \RemoveEmptyStaves
     }
  }
}
