% can do this mid-music to change:
%   \set Staff.midiInstrument = #"pizzicato strings" 


#(set-global-staff-size 17)
\paper {
  indent = 3.0\cm  % space for instrumentName
  short-indent = 1.5\cm  % space for shortInstrumentName
}

#(define my-instrument-equalizer-alist '())

#(set! my-instrument-equalizer-alist
  (append
    '(
         ("flute" . (0 . 0.3))
         ("oboe" . (0 . 0.7))
         ("clarinet" . (0 . 0.7))
         ("bassoon" . (0.3 . 0.8))
         ("french horn" . (0.1 . 0.7))
         ("trumpet" . (0.1 . 0.8))
         ("timpani" . (0.2 . 0.9))
         ("violin" . (0.2 . 0.6))
         ("viola" . (0.1 . 0.7))
         ("cello" . (0.2 . 0.8))
         ("orchestral harp" . (0.2 . 0.6))
         ("contrabass" . (0.2 . 0.8))
      )
    my-instrument-equalizer-alist))

#(define (my-instrument-equalizer s)
  (let ((entry (assoc s my-instrument-equalizer-alist)))
    (if entry
      (cdr entry))))

\score {
  <<
    \set Score.instrumentEqualizer = #my-instrument-equalizer
    \new StaffGroup = "StaffGroup_woodwinds" <<
      \new Staff = "Staff_flute" {
        \set Staff.instrumentName = #"Flute"
        \set Staff.midiInstrument = #"flute"
        \set Staff.midiPanPosition = #0.2
        % shortInstrumentName, midiInstrument, etc.
        % may be set here as well
        \fluteMusic
      }
      \new Staff = "Staff_clarinet" {
        \set Staff.instrumentName = #"Clarinet"
        \set Staff.midiInstrument = #"clarinet"
        \set Staff.midiPanPosition = #0.3
        \clarinetMusic
      }
    >>
    \new StaffGroup = "StaffGroup_brass" <<
      \new Staff = "Staff_hornI" {
        \set Staff.instrumentName = #"Horn"
        \set Staff.midiInstrument = #"french horn"
        \set Staff.midiPanPosition = #0.5
        \hornMusic
      }
      \new Staff = "Staff_trumpet" {
        \set Staff.instrumentName = #"Trumpet"
        \set Staff.midiInstrument = #"trumpet"
        \set Staff.midiPanPosition = #0.4
        \trumpetMusic
      }
    >>
    \new Staff = "Staff_harp" <<
      \set Staff.instrumentName = #"Harp"
      \set Staff.midiInstrument = #"orchestral harp"
      \set Staff.midiPanPosition = #-0.7
      \harpMusic
    >>
    \new PianoStaff <<
      \set PianoStaff.instrumentName = #"Piano"
      \new Staff { \partcombine \pianoRHMusic \pianoLHMusic }
      \set Staff.midiPanPosition = #-0.8
    >>
    \new ChoirStaff = "ChoirStaff_choir" <<
      \new Staff = "Staff_soprano" {
        \set Staff.instrumentName = #"Soprano"
        \set Staff.midiInstrument = #"soprano sax"
        \new Voice = "soprano"
        \set Staff.midiPanPosition = #0
        \sopranoMusic
      }
      \new Staff = "Staff_mezzo" {
        \set Staff.instrumentName = #"Mezzo"
        \set Staff.midiInstrument = #"harmonica"
        \new Voice = "mezzo"
        \set Staff.midiPanPosition = #0.05
        \mezzoMusic
      }
      \new Staff = "Staff_bass" {
        \set Staff.instrumentName = #"Bass"
        \set Staff.midiInstrument = #"bassoon"
        \new Voice = "bass"
        \set Staff.midiPanPosition = #-0.05
        \vbassMusic
      }
    >>
    \new StaffGroup = "StaffGroup_strings" <<
      \new GrandStaff = "GrandStaff_violins" <<
        \new Staff = "Staff_violinI" {
          \set Staff.instrumentName = #"Violin I"
          \set Staff.midiPanPosition = #-0.4
          \set Staff.midiInstrument = #"violin"
          \violinIMusic
        }
        \new Staff = "Staff_violinII" {
          \set Staff.instrumentName = #"Violin II"
          \set Staff.midiPanPosition = #-0.3
          \set Staff.midiInstrument = #"violin"
          \violinIIMusic
        }
      >>
      \new Staff = "Staff_viola" {
        \set Staff.instrumentName = #"Viola"
        \set Staff.midiPanPosition = #0.5
        \set Staff.midiInstrument = #"viola"
        \violaMusic
      }
      \new Staff = "Staff_cello" {
        \set Staff.instrumentName = #"Cello"
        \set Staff.midiPanPosition = #0.6
        \set Staff.midiInstrument = #"cello"
        \celloMusic
      }
      \new Staff = "Staff_bass" {
        \set Staff.instrumentName = #"Double Bass"
        \set Staff.midiPanPosition = #0.7
        \set Staff.midiInstrument = #"contrabass"
        \bassMusic
      }
    >>
  >>
  \midi {
    \tempo 4 = 120
  }
}

