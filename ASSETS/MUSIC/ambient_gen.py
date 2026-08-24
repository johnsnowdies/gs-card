#!/usr/bin/env python3
"""
Генератор музыки для PC speaker в разных стилях.

Стили:
  arabian - восточная/арабская тема (гармонический минор, мелизмы)
  battle  - динамичная боевая тема (быстрый темп, арпеджио)
  creepy  - мрачная/жуткая атмосфера (диссонансы, медленный темп)

Выход: C-массивы notes[] и durations[].
"""

import random
import argparse

# ============ УТИЛИТЫ ============

def midi_to_freq(midi_note):
    """Перевод MIDI ноты в частоту (Гц)."""
    return int(round(440.0 * (2 ** ((midi_note - 69) / 12.0))))

def freq_to_tick_duration(seconds, ms_per_tick=55):
    """Перевод секунд в целое число тиков (минимум 1)."""
    ms = seconds * 1000.0
    ticks = int(round(ms / ms_per_tick))
    return max(1, ticks)

# ============ БАЗОВЫЕ КЛАССЫ ДЛЯ СТИЛЕЙ ============

class MusicGenerator:
    """Базовый класс генератора."""
    def __init__(self, seed=None, measures=16, notes_per_measure=4, pause_prob=0.05):
        if seed is not None:
            random.seed(seed)
        self.measures = measures
        self.notes_per_measure = notes_per_measure
        self.pause_prob = pause_prob
        self.notes = []
        self.durations = []
        self.prev_note = None

    def generate(self):
        raise NotImplementedError

    def add_note(self, freq, duration_ticks):
        self.notes.append(freq)
        self.durations.append(duration_ticks)
        if freq > 0:
            self.prev_note = freq

    def add_pause(self, duration_ticks):
        self.notes.append(0)
        self.durations.append(duration_ticks)

    def write_c_arrays(self, output_file):
        notes_str = "int notes[] = {\n    " + ", ".join(str(n) for n in self.notes) + "\n};"
        durations_str = "int durations[] = {\n    " + ", ".join(str(d) for d in self.durations) + "\n};"
        with open(output_file, "w") as f:
            f.write("/* Auto-generated music (style: {}) */\n".format(self.style_name))
            f.write("/* Tick = 55 ms */\n\n")
            f.write(notes_str + "\n\n")
            f.write(durations_str + "\n")
        print(f"Записано в {output_file}, всего событий: {len(self.notes)}")


# ============ ARABIAN ============

class ArabianGenerator(MusicGenerator):
    """Восточная тема: гармонический минор, характерные интервалы."""
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.style_name = "arabian"
        # Гармонический минор от A (A H C D E F G# A)
        # MIDI ноты: A4=69, B4=71, C5=72, D5=74, E5=76, F5=77, G#5=80, A5=81
        self.scale_midi = [69, 71, 72, 74, 76, 77, 80, 81]
        self.scale_freq = [midi_to_freq(m) for m in self.scale_midi]
        # Ритмы: часто используется пунктир (1.5 : 0.5) и длинные ноты
        self.duration_choices = [1, 1, 2, 3, 4, 6]  # 3 = 165 мс
        # Мелодические паттерны (ступени гаммы)
        self.motives = [
            [0, 1, 2, 3],  # восходящий тетрахорд
            [3, 2, 1, 0],  # нисходящий
            [0, 2, 1, 2],  # опевание
            [2, 0, 1, 0],
            [4, 3, 2, 3],
            [0, 1, 3, 2],  # с использованием увеличенной секунды (1->3)
            [3, 1, 2, 0],
            [0, 1, 2, 1, 3, 2, 1, 0]  # длинный нисходящий мотив
        ]

    def generate(self):
        for m in range(self.measures):
            # Выбираем аккорд? Для простоты используем тонику A (ступень 0)
            current_chord_root = 0  # A
            for i in range(self.notes_per_measure):
                if random.random() < self.pause_prob:
                    self.add_pause(random.choice([1, 2]))
                    continue
                # Выбор мотива с привязкой к текущей гармонии
                motive = random.choice(self.motives)
                # Транспонируем мотив: сдвигаем в пределах гаммы, сохраняя характер
                shift = random.choice([0, 0, 0, 1, -1, 2])
                note_indices = [(idx + shift) % len(self.scale_midi) for idx in motive]
                # Берём первую ноту мотива как основную, длительность берём из выбранных
                main_note_idx = note_indices[0]
                freq = self.scale_freq[main_note_idx]
                # Длительность: для восточной мелодии часто длинная первая нота
                if i == 0:
                    dur = random.choice([4, 6])
                else:
                    dur = random.choice(self.duration_choices)
                self.add_note(freq, dur)
                # Иногда добавляем мелизм: быстрая вторая нота (короткая)
                if random.random() < 0.4 and len(note_indices) > 1:
                    second_idx = note_indices[1]
                    second_freq = self.scale_freq[second_idx]
                    self.add_note(second_freq, 1)
                # Пауза между фразами (в конце такта)
                if i == self.notes_per_measure - 1 and random.random() < 0.5:
                    self.add_pause(random.choice([1, 2]))
        return self


# ============ BATTLE ============

class BattleGenerator(MusicGenerator):
    """Боевая тема: быстрый темп, чёткий ритм, арпеджио, минор."""
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.style_name = "battle"
        # Минорная пентатоника от E (E G A B D E)
        self.scale_midi = [64, 67, 69, 71, 74, 76]  # E4, G4, A4, B4, D5, E5
        self.scale_freq = [midi_to_freq(m) for m in self.scale_midi]
        # Ритм: преобладают короткие длительности (1-2 тика), иногда 4 для акцентов
        self.duration_choices = [1, 1, 1, 2, 2, 4]
        # Мотивы: арпеджио и маршевые фигуры
        self.motives = [
            [0, 2, 3, 2],   # E G A G
            [0, 1, 2, 3],   # E G A B
            [3, 2, 1, 0],   # B A G E
            [2, 3, 2, 0],   # A B A E
            [0, 2, 0, 2],   # повторение тоники
            [1, 3, 2, 1],
            [0, 0, 0, 1],   # ритмическая фигура на тонике
        ]

    def generate(self):
        for m in range(self.measures):
            # Чередуем аккорды: Em (тоника) и D (седьмая ступень)
            chord_root = 0 if m % 2 == 0 else 5  # 5 = D5 (нота D5 в масштабе индекс 5)
            for i in range(self.notes_per_measure):
                if random.random() < self.pause_prob:
                    self.add_pause(1)
                    continue
                motive = random.choice(self.motives)
                # Транспонируем, но не выходим за пределы гаммы
                shift = random.choice([0, 0, 1, -1])
                note_indices = []
                for idx in motive:
                    new_idx = (idx + shift) % len(self.scale_midi)
                    note_indices.append(new_idx)
                # Первая нота мотива - главная, остальные могут быть короче
                main_freq = self.scale_freq[note_indices[0]]
                # Длительность главной ноты
                dur = random.choice(self.duration_choices)
                self.add_note(main_freq, dur)
                # Иногда добавляем быстрые ноты из мотива (для динамики)
                if random.random() < 0.5 and len(note_indices) > 1:
                    # Добавляем несколько коротких нот
                    for j in range(1, min(len(note_indices), 3)):
                        extra_freq = self.scale_freq[note_indices[j]]
                        self.add_note(extra_freq, 1)
                # В конце такта иногда акцент
                if i == self.notes_per_measure - 1 and random.random() < 0.3:
                    self.add_note(self.scale_freq[0], 4)  # низкая тоника
        return self


# ============ CREEPY ============

class CreepyGenerator(MusicGenerator):
    """Крипота: диссонансы, медленный темп, низкие ноты, неожиданные паузы."""
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.style_name = "creepy"
        # Используем хроматическую гамму в низком регистре, с акцентом на уменьшённые интервалы
        # Набор MIDI нот: C3=48, C#3=49, D3=50, D#3=51, E3=52, F3=53, F#3=54, G3=55, G#3=56, A3=57, A#3=58, B3=59
        self.scale_midi = list(range(48, 60))  # хроматика от C3 до B3
        self.scale_freq = [midi_to_freq(m) for m in self.scale_midi]
        # Ритм: очень медленный, длительности 4, 8, 12, 16 тиков
        self.duration_choices = [4, 6, 8, 12, 16]
        # Диссонансные интервалы: тритон (6 полутонов), малая секунда (1), большая септима (11)
        # Создаём мотивы как последовательности смещений от тоники
        self.motives = [
            [0, 6, 5, 6],   # тритон
            [0, 1, 0, 2],   # малая секунда
            [0, 11, 10, 11],
            [0, 3, 1, 2],   # уменьшённая кварта + малая секунда
            [0, 7, 1, 6],   # квинта + малая секунда + тритон
            [0, 2, 3, 1],
            [0, 8, 7, 8],   # малая секста
        ]

    def generate(self):
        for m in range(self.measures):
            for i in range(self.notes_per_measure):
                # Частые паузы создают напряжение
                if random.random() < self.pause_prob * 3:
                    self.add_pause(random.choice([2, 4, 8]))
                    continue
                # Выбираем случайную "тоника" (базовую ноту)
                base_idx = random.choice([0, 2, 3, 5, 7, 8, 10])
                motive = random.choice(self.motives)
                # Транспонируем мотив от базовой ноты (без зацикливания, чтобы не вылезти за пределы)
                note_indices = []
                for offset in motive:
                    idx = base_idx + offset
                    if idx < 0 or idx >= len(self.scale_midi):
                        idx = base_idx  # fallback
                    note_indices.append(idx)
                # Первая нота - основная, длинная
                main_freq = self.scale_freq[note_indices[0]]
                dur = random.choice(self.duration_choices)
                self.add_note(main_freq, dur)
                # Иногда добавляем вторую ноту (диссонанс)
                if random.random() < 0.6 and len(note_indices) > 1:
                    second_freq = self.scale_freq[note_indices[1]]
                    self.add_note(second_freq, random.choice([2, 4]))
                # В конце такта иногда резкая пауза
                if i == self.notes_per_measure - 1 and random.random() < 0.5:
                    self.add_pause(random.choice([4, 8]))
        return self


# ============ MAIN ============

def main():
    parser = argparse.ArgumentParser(description="Генератор музыки для PC speaker")
    parser.add_argument("style", choices=["arabian", "battle", "creepy"],
                        help="Стиль музыки")
    parser.add_argument("-o", "--output", default="music.c",
                        help="Имя выходного C-файла")
    parser.add_argument("--seed", type=int, default=None,
                        help="Зерно случайности")
    parser.add_argument("--measures", type=int, default=16,
                        help="Количество тактов")
    parser.add_argument("--notes-per-measure", type=int, default=4,
                        help="Нот на такт")
    parser.add_argument("--pause-prob", type=float, default=0.05,
                        help="Вероятность паузы")
    args = parser.parse_args()

    kwargs = {
        "seed": args.seed,
        "measures": args.measures,
        "notes_per_measure": args.notes_per_measure,
        "pause_prob": args.pause_prob
    }

    if args.style == "arabian":
        gen = ArabianGenerator(**kwargs)
    elif args.style == "battle":
        gen = BattleGenerator(**kwargs)
    elif args.style == "creepy":
        gen = CreepyGenerator(**kwargs)
    else:
        raise ValueError("Unknown style")

    gen.generate()
    gen.write_c_arrays(args.output)


if __name__ == "__main__":
    main()
    