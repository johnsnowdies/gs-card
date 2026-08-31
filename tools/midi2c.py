#!/usr/bin/env python3
"""
Convert a monophonic single-track MIDI file to a C function with note/duration arrays.
Durations are expressed in "ticks" of approximately 55 ms.

Usage:
    midi2c.py -i input.mid [-o output.c]

If -o is not given, output is written to input.c
"""

import sys
import os
import argparse
import math

try:
    import mido
except ImportError:
    print("Error: mido library is required. Install with: pip install mido", file=sys.stderr)
    sys.exit(1)

def midi_note_to_freq(note):
    """Convert MIDI note number to frequency in Hz (rounded to int)."""
    return int(round(440.0 * (2 ** ((note - 69) / 12.0))))

def parse_args():
    parser = argparse.ArgumentParser(description="Convert MIDI to C music_play arrays")
    parser.add_argument("-i", "--input", required=True, help="Input MIDI file")
    parser.add_argument("-o", "--output", help="Output C file (default: input.c)")
    return parser.parse_args()


def sanitize_name(name):
    """Convert file basename to a valid C identifier."""
    base = os.path.splitext(os.path.basename(name))[0]
    return "".join(ch if ch.isalnum() else "_" for ch in base)


def midi_ticks_to_55ms_units(delta_ticks, tempo_us_per_quarter, ticks_per_beat):
    """
    Convert a duration in MIDI ticks to integer number of 55 ms units.
    tempo_us_per_quarter: microseconds per quarter note
    ticks_per_beat: MIDI ticks per quarter note
    """
    if delta_ticks <= 0:
        return 0
    # Duration in milliseconds
    ms = (delta_ticks * tempo_us_per_quarter) / (ticks_per_beat * 1000.0)
    units = int(round(ms / 55.0))
    if units == 0 and ms > 0:
        units = 1  # ensure very short events are not lost
    return units


def main():
    args = parse_args()
    input_file = args.input
    output_file = args.output if args.output else os.path.splitext(input_file)[0] + ".c"

    # Load MIDI file
    try:
        mid = mido.MidiFile(input_file)
    except Exception as e:
        print(f"Error reading MIDI file: {e}", file=sys.stderr)
        sys.exit(1)

    if len(mid.tracks) != 1:
        print("Warning: expected exactly one track, using track 0", file=sys.stderr)
        track = mid.tracks[0]
    else:
        track = mid.tracks[0]

    ticks_per_beat = mid.ticks_per_beat
    if ticks_per_beat == 0:
        print("Error: SMPTE time division not supported", file=sys.stderr)
        sys.exit(1)

    # Collect absolute times and events
    abs_time = 0
    active_note = None  # (note_number, start_tick)
    notes = []          # list of dicts: start, end, note
    tempo_events = []   # list of (abs_tick, tempo_us_per_quarter)

    for msg in track:
        abs_time += msg.time
        if msg.type == 'set_tempo':
            tempo_events.append((abs_time, msg.tempo))
        elif msg.type == 'note_on':
            if msg.velocity > 0:
                if active_note is not None:
                    # monophonic assumption: finish previous note
                    print("Warning: overlapping notes detected, finishing previous note early", file=sys.stderr)
                    notes.append({'start': active_note[1], 'end': abs_time, 'note': active_note[0]})
                active_note = (msg.note, abs_time)
            else:
                # note_on with velocity 0 acts as note_off
                if active_note is not None and active_note[0] == msg.note:
                    notes.append({'start': active_note[1], 'end': abs_time, 'note': active_note[0]})
                    active_note = None
        elif msg.type == 'note_off':
            if active_note is not None and active_note[0] == msg.note:
                notes.append({'start': active_note[1], 'end': abs_time, 'note': active_note[0]})
                active_note = None

    # If a note is still active at end of track, close it at the last event time
    if active_note is not None:
        notes.append({'start': active_note[1], 'end': abs_time, 'note': active_note[0]})

    # Determine total track duration (max end time or last event time)
    total_duration_ticks = max([n['end'] for n in notes], default=abs_time)

    # Use the first tempo event if available, otherwise default 120 BPM (500000 us/quarter)
    if tempo_events:
        # Sort tempo events by time and take the first one
        tempo_events.sort(key=lambda x: x[0])
        tempo = tempo_events[0][1]
    else:
        tempo = 500000  # 120 BPM

    bpm = 60000000 / tempo

    # Build chronological list of (is_note, note_number_or_0, duration_ticks)
    events = []
    previous_end = 0
    # Sort notes by start time
    notes_sorted = sorted(notes, key=lambda x: x['start'])

    for n in notes_sorted:
        # Add pause before note if gap exists
        gap = n['start'] - previous_end
        if gap > 0:
            events.append((False, 0, gap))
        # Add the note
        dur = n['end'] - n['start']
        if dur > 0:
            events.append((True, n['note'], dur))
        previous_end = max(previous_end, n['end'])

    # Add trailing pause if any
    trailing_gap = total_duration_ticks - previous_end
    if trailing_gap > 0:
        events.append((False, 0, trailing_gap))

    # Convert durations to 55 ms units and build arrays
    notes_array = []
    durs_array = []
    for is_note, note_val, dur_ticks in events:
        units = midi_ticks_to_55ms_units(dur_ticks, tempo, ticks_per_beat)
        if units > 0:
            if is_note:
                notes_array.append(midi_note_to_freq(note_val))
            else:
                notes_array.append(0)  # пауза
            durs_array.append(units)

    if not notes_array:
        print("Warning: no musical content found", file=sys.stderr)
        notes_array = [0]
        durs_array = [1]

    # Generate C code
    func_name = "music_" + sanitize_name(os.path.basename(input_file))
    count = len(notes_array)

    c_code = []
    c_code.append(f"void {func_name}()")
    c_code.append("{")
    c_code.append(f"    /* BPM: {bpm:.2f} */")
    c_code.append("    static int notes[] = {" + ", ".join(str(x) for x in notes_array) + "};")
    c_code.append("    static int durs[]  = {" + ", ".join(str(x) for x in durs_array) + "};")
    c_code.append(f"    music_play(notes, durs, {count});")
    c_code.append("}")

    # Write output
    try:
        with open(output_file, 'w') as f:
            f.write("\n".join(c_code))
            f.write("\n")
        print(f"Generated {output_file}")
    except Exception as e:
        print(f"Error writing output: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()