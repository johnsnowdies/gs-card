#!/usr/bin/env python3
"""
Преобразует монофонический MIDI-файл в C-функцию с нотами и длительностями.

Пример использования:
    python midi2c.py -i melody.mid

Сгенерирует файл melody.c с функцией void music_melody().
"""

import os
import sys
import argparse
import math
from mido import MidiFile

def extract_notes_and_durations(mid_file):
    """
    Извлекает последовательность нот и пауз из монофонического MIDI-файла.
    Возвращает:
        notes: список MIDI-номеров нот (0 для паузы)
        durs:  список длительностей в тиках по 55 мс
        bpm:   вычисленный BPM (целое)
    """
    mid = MidiFile(mid_file)
    ppq = mid.ticks_per_beat

    # Определяем темп (по умолчанию 120 BPM -> 500000 мкс на четверть)
    tempo = 500000  # мкс на четверть
    for track in mid.tracks:
        for msg in track:
            if msg.type == 'set_tempo':
                tempo = msg.tempo
                break
        if tempo != 500000:
            break

    # Собираем все нотные события с абсолютным временем (в тиках)
    events = []
    for track in mid.tracks:
        abs_time = 0
        for msg in track:
            abs_time += msg.time
            if msg.type == 'note_on' or msg.type == 'note_off':
                is_off = (msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0))
                events.append((abs_time, 'off' if is_off else 'on', msg.note))

    if not events:
        raise ValueError("В MIDI-файле нет нотных событий.")

    events.sort(key=lambda x: x[0])

    # Проход по событиям для построения нот и пауз
    notes = []      # MIDI-номера (0 = пауза)
    durs = []       # длительности в тиках MIDI

    active_note = None
    note_start = 0
    last_time = 0   # время окончания последней ноты или паузы

    for abs_time, typ, note in events:
        if typ == 'on':
            if active_note is not None:
                # Аварийное завершение предыдущей ноты (не должно случаться в монофоническом файле)
                dur = abs_time - note_start
                if dur > 0:
                    notes.append(active_note)
                    durs.append(dur)
                active_note = note
                note_start = abs_time
                last_time = abs_time
            else:
                # Пауза перед нотой
                dur_pause = abs_time - last_time
                if dur_pause > 0:
                    notes.append(0)
                    durs.append(dur_pause)
                active_note = note
                note_start = abs_time
                last_time = abs_time

        elif typ == 'off':
            if active_note is not None and active_note == note:
                dur = abs_time - note_start
                if dur > 0:
                    notes.append(active_note)
                    durs.append(dur)
                active_note = None
                last_time = abs_time
            # else: игнорируем Note Off для неактивной ноты

    # Если осталась незавершённая нота, завершаем её в конце
    if active_note is not None:
        total_duration = max([e[0] for e in events])
        dur = total_duration - note_start
        if dur > 0:
            notes.append(active_note)
            durs.append(dur)
        active_note = None

    # Преобразуем длительности в тики по 55 мс
    # Время одного тика MIDI в секундах
    tick_sec = (tempo / 1_000_000.0) / ppq
    new_durs = []
    for d in durs:
        duration_sec = d * tick_sec
        dur_55 = int(round(duration_sec / 0.055))
        if dur_55 < 1:
            dur_55 = 1
        new_durs.append(dur_55)

    # Вычисляем BPM для комментария
    bpm = int(round(60_000_000 / tempo))

    return notes, new_durs, bpm


def generate_c_code(filename, notes, durs, bpm):
    """
    Генерирует C-код и сохраняет в файл.
    """
    # Имя функции: music_ + basename без расширения
    base = os.path.splitext(os.path.basename(filename))[0]
    # Очищаем от недопустимых символов (оставляем буквы, цифры, подчёркивание)
    func_name = ''.join(c for c in base if c.isalnum() or c == '_')
    if not func_name:
        func_name = "unknown"
    func_name = "music_" + func_name

    # Формируем строки массивов
    notes_str = ', '.join(str(n) for n in notes)
    durs_str = ', '.join(str(d) for d in durs)
    count = len(notes)

    code = f"""/*
 * Generated from {os.path.basename(filename)}
 * BPM: {bpm}
 */

void {func_name}()
{{
    static int notes[] = {{{notes_str}}};
    static int durs[]  = {{{durs_str}}};
    music_play(notes, durs, {count});
}}
"""
    return code


def main():
    parser = argparse.ArgumentParser(description="Преобразование монофонического MIDI в C-код.")
    parser.add_argument('-i', '--input', required=True, help="Входной MIDI-файл")
    parser.add_argument('-o', '--output', help="Выходной C-файл (по умолчанию <имя>.c)")
    args = parser.parse_args()

    if not os.path.isfile(args.input):
        print(f"Ошибка: файл '{args.input}' не найден.", file=sys.stderr)
        sys.exit(1)

    # Определяем выходной файл
    if args.output:
        output_file = args.output
    else:
        base = os.path.splitext(args.input)[0]
        output_file = base + '.c'

    try:
        notes, durs, bpm = extract_notes_and_durations(args.input)
    except Exception as e:
        print(f"Ошибка при обработке MIDI: {e}", file=sys.stderr)
        sys.exit(1)

    code = generate_c_code(args.input, notes, durs, bpm)

    with open(output_file, 'w') as f:
        f.write(code)

    print(f"Сгенерирован файл: {output_file}")


if __name__ == "__main__":
    main()