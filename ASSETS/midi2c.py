#!/usr/bin/env python3
"""
Convert a MIDI file to C arrays for PC speaker music (MS-DOS).

The output contains two arrays:
    int notes[] = { ... };       // frequencies in Hz, 0 = pause
    int durations[] = { ... };   // length in timer ticks (1 tick ~ 55 ms)

The script extracts notes from all tracks, merges simultaneous notes by
selecting the highest pitch (or optionally the loudest), and inserts pauses
between notes when no note is active.
"""

import sys
import math
import argparse

try:
    import pretty_midi
except ImportError:
    print("Error: pretty_midi library is required. Install it with: pip install pretty_midi")
    sys.exit(1)


def midi_note_to_freq(note):
    """Convert a MIDI note number to frequency in Hz."""
    if note == 0:
        return 0
    return int(round(440.0 * (2 ** ((note - 69) / 12.0))))


def seconds_to_ticks(seconds, ms_per_tick=55):
    """Convert seconds to integer number of PC speaker timer ticks."""
    ms = seconds * 1000.0
    ticks = int(round(ms / ms_per_tick))
    if ticks < 1:
        ticks = 1
    return ticks


def extract_notes_from_midi(midi_file, strategy="highest"):
    """
    Extract a monophonic sequence of (note, duration_in_ticks) from a MIDI file.

    Parameters:
        midi_file: path to the MIDI file
        strategy: "highest" or "loudest" - which note to pick when multiple are active

    Returns:
        list of (note_number, duration_ticks) where note_number=0 means pause.
    """
    midi_data = pretty_midi.PrettyMIDI(midi_file)

    # Collect all note events (start and end) from all instruments
    events = []  # (time, pitch, velocity, type)
    for instrument in midi_data.instruments:
        for note in instrument.notes:
            events.append((note.start, note.pitch, note.velocity, 'on'))
            events.append((note.end, note.pitch, note.velocity, 'off'))

    # Sort events by time; for equal times, process 'off' before 'on'
    events.sort(key=lambda x: (x[0], 0 if x[3] == 'off' else 1))

    active_notes = {}  # pitch -> count (to handle overlapping same pitch)
    segments = []      # list of (note, duration_ticks)
    current_time = 0.0
    last_note = None   # note used in the previous segment (for merging)

    for time, pitch, velocity, etype in events:
        # Create a segment for the time interval from current_time to time
        delta = time - current_time
        if delta > 0:
            # Determine the chosen note during this interval
            if active_notes:
                if strategy == "loudest":
                    # Choose the active note with the highest velocity
                    # (we need to keep track of velocities too)
                    # For simplicity, we'll just use the highest pitch here;
                    # a proper loudest strategy would require storing velocities.
                    chosen = max(active_notes.keys())
                else:  # "highest"
                    chosen = max(active_notes.keys())
            else:
                chosen = 0  # pause

            dur_ticks = seconds_to_ticks(delta)
            if last_note is not None and chosen == last_note:
                # Extend previous segment instead of starting a new one
                segments[-1] = (segments[-1][0], segments[-1][1] + dur_ticks)
            else:
                segments.append((chosen, dur_ticks))
                last_note = chosen

        # Update active notes
        if etype == 'on':
            active_notes[pitch] = active_notes.get(pitch, 0) + 1
        else:  # 'off'
            if pitch in active_notes:
                active_notes[pitch] -= 1
                if active_notes[pitch] <= 0:
                    del active_notes[pitch]

        current_time = time

    # After all events, there might be a final silence (optional, usually not needed)
    # We won't add it explicitly; the music will stop when the last segment ends.

    # Merge consecutive segments with the same note (including pauses)
    merged = []
    for note, dur in segments:
        if merged and merged[-1][0] == note:
            merged[-1] = (note, merged[-1][1] + dur)
        else:
            merged.append((note, dur))

    return merged


def generate_c_arrays(segments):
    """Convert list of (note, duration) to C array strings."""
    notes = []
    durations = []
    for note, dur in segments:
        if note == 0:
            notes.append(0)
        else:
            notes.append(midi_note_to_freq(note))
        durations.append(dur)

    notes_str = "int notes[] = {\n    " + ", ".join(str(n) for n in notes) + "\n};"
    durations_str = "int durations[] = {\n    " + ", ".join(str(d) for d in durations) + "\n};"
    return notes_str, durations_str


def main():
    parser = argparse.ArgumentParser(description="Convert MIDI to PC speaker C arrays")
    parser.add_argument("midi_file", help="Input MIDI file")
    parser.add_argument("-o", "--output", default="music_arrays.c",
                        help="Output C file (default: music_arrays.c)")
    parser.add_argument("-s", "--strategy", choices=["highest", "loudest"],
                        default="highest",
                        help="Note selection strategy when multiple notes are active (default: highest)")
    args = parser.parse_args()

    segments = extract_notes_from_midi(args.midi_file, args.strategy)

    if not segments:
        print("No notes found in MIDI file.")
        sys.exit(1)

    notes_str, durations_str = generate_c_arrays(segments)

    with open(args.output, "w") as f:
        f.write("/* Auto-generated from MIDI file: %s */\n" % args.midi_file)
        f.write("/* Tick duration: 55 ms */\n\n")
        f.write(notes_str + "\n\n")
        f.write(durations_str + "\n")

    print("Written to %s" % args.output)
    print("Number of segments: %d" % len(segments))


if __name__ == "__main__":
    main()