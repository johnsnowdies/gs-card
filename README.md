# GS-CARD

A perfect way to execute your hyperspace jump quickly and safely (not guaranteed).

![Logo](https://github.com/johnsnowdies/gs-card/blob/574a5a96de226de081c2c8692de4733a60df5b44/docs/logo.png)

## What is this?

**GS-CARD** is a logistics strategy set in space. You take on the role of a cargo ship captain travelling through the **Saferi** galaxy, completing delivery contracts, avoiding pirates, customs, and dangerous objects. The entire interface is identical to MS-DOS programs from the 90s.

The game was written in **Turbo C 2.0** (1989) strictly following the C89 standard and runs on real DOS or in emulators.

## Build requirements
- **DOSBox version 0.74-3** or higher
- **Compiler:** Borland Turbo C 2.0
- Place Turbo C in the root directory (e.g., `C:\TURBOC` or mounted drive).
- **Build command:** `make build` (with optional `LANG=ru` or `LANG=en`).

## Makefile targets

| Target   | Description |
|----------|-------------|
| `make build LANG=ru/en` | Compile inside DOSBox, copy assets, clean intermediate files. |
| `make run`              | Launch DOSBox with the built `GSCARD.EXE`. |
| `make clean`            | Remove all build artifacts (except .gitkeep and bundles). |
| `make release`          | Build both language versions and create `.jsdos` bundles (`bundle-ru.jsdos`, `bundle-en.jsdos`). |
| `make fix`              | Convert source files to DOS format (CP866 + CRLF). |

All commands assume you have **DOSBox** installed and accessible in your `PATH`.

## What the 🚀

My friends and I were playing a tabletop role‑playing game about a distant dystopian space future. We used the first version of GS‑CARD as a galaxy map where the plot of this game unfolded.

Our story revolved around a ruined human empire, whose technology was terribly outdated but still useful. That's why I used MS‑DOS and the Turbo C compiler from 1989. I didn't want to mess with DOS extenders, so the entire codebase stays within the 640 KB conventional memory limit.