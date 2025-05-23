﻿# sayaslicer

> [!WARNING]
> Currently a work-in-progress; nightly builds are available [here](https://github.com/SayakaIsBaka/sayaslicer/actions) (Windows, Linux and Mac, GitHub login required)
> Alternatively, you can download builds from [nightly.link](https://nightly.link/SayakaIsBaka/sayaslicer/workflows/main/master?preview) if you don't have a GitHub account

A cross-platform audio slicer with BMS-related features. Aims to be a drop-in replacement for [woslicerII](https://cerebralmuddystream.nekokan.dyndns.info/soft/woslicerII.zip) [(english version)](https://github.com/SayakaIsBaka/woslicerII-english) by [wosderge](https://cerebralmuddystream.nekokan.dyndns.info) with similar features and newly added ones, while also fixing some long-standing limitations and ergonomic issues.

## Features
- BMSE clipboard support (copy and paste, can be used with [BMHelper](https://excln.github.io/bmhelper.html))
- iBMSC clipboard export support (useful for markers with a resolution over 192th)
- File drag and drop (only on Windows)
- Offset setting
- Arbitrary snapping (from 1/1 to 1/192)
- Toggleable base-62 support for BMSE clipboard data copy
- Silent tail remover with configurable threshold
- Fadeout
- Partial BPM changes support (only for audio slicing, a bit hackish but it works)
- Load / save project
- Import markers from a MIDI file
- [Mid2BMS](https://mid2bms.net) renamer array file import
- Keysound list export
- Copy / paste markers
- Undo / redo
- Zero-crossing
- Multiple languages support (English, Japanese, Korean)
- Automatic update checker (disabled by default) and downloader

## Keyboard shortcuts
Keyboard shortcuts are the same as woslicerII except the keyboard layout is taken into account:
- `O`: open audio file
- `Z`: add slice marker
- `C`: clear all markers (but adds a marker at 0, similar behaviour to woslicerII)
- `Shift+C`: clear ALL markers (including marker at 0)
- `V`: copy markers as BMSE clipboard data
- `Shift+V`: copy markers as iBMSC clipboard data
- `B`: import markers from clipboard (using BMSE clipboard data)
- `K`: copy keysound list to clipboard
- `Shift+K`: append keysound list to BMS (if in the same folder as the audio file)
- `M`: export keysounds
- `P`: preview current keysound and move to the next one
- `Enter`: preview current keysound
- `LeftArrow / RightArrow`: move position cursor
- `Shift + LeftArrow / Shift + RightArrow`: move position cursor (jump to closest snap)
- `UpArrow / DownArrow`: set snapping
- `Ctrl+O`: load project
- `Ctrl+S`: save project
- `Home`: jump to the beginning of the waveform
- `End`: jump to the end of the waveform
- `Space`: enter / exit select mode
- `Ctrl+C`: copy selected markers
- `Ctrl+X`: cut selected markers
- `Ctrl+V`: paste previously copied markers
- `Del`: delete selected markers
- `Ctrl+Z`: undo action
- `Ctrl+Y`: redo action

Additionally, clicking on a marker in the table will jump the cursor to its position; right-clicking it will delete it.

## Important notes
- Zero-crossing markers can help with reducing clicks during keysound export; however, doing so also moves the markers, which may affect iBMSC clipboard data copying (as it supports an arbitrary maximum resolution). If you encounter issues with the BMS file after pasting iBMSC clipboard data into your editor and saving (such as missing keysounds or gamemode change), please use BMSE clipboard data instead, which automatically rounds all markers to the nearest 192th.

## Planned features
The following features are less likely to be implemented but might be done depending on whether I feel like it or if there's enough demand for it:
- Better BPM change support (BMSE clipboard support, import BPM changes from MIDI and proper grid display)

## Credits
sayaslicer uses the following libraries:
- [Dear ImGui](https://github.com/ocornut/imgui)
- [GLFW](https://github.com/glfw/glfw)
- [PortAudio](https://github.com/PortAudio/portaudio)
- [libsndfile](https://github.com/libsndfile/libsndfile)
- [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)
- [implot](https://github.com/epezent/implot)
- [tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/)
- [clip](https://github.com/dacap/clip)
- [ImGuiNotify](https://github.com/TyomaVader/ImGuiNotify)
- [cereal](https://github.com/USCiLab/cereal)
- [midifile](https://github.com/craigsapp/midifile)
- [i18n_keyval](https://github.com/stefandevai/i18n_keyval)
- [cpr](https://github.com/libcpr/cpr)
- [nlohmann_json](https://github.com/nlohmann/json)
- [imgui_markdown](https://github.com/juliettef/imgui_markdown)
- [libiconv](https://www.gnu.org/software/libiconv/)
- [Moonlight (ImGui theme)](https://github.com/Madam-Herta/Moonlight/)

Translations:
- Korean (한국어): [Sobrem](https://twitter.com/SobremMusic)
- Simplified Chinese (简体中文): [Zris](https://twitter.com/Zrisound)
- Spanish (Español): [MADizM*rsoff](https://github.com/Marsoff9898)
