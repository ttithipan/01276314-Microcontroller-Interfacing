# Microcontroller Labs — organized copy

Reorganized copy of the `microcon` folder. The original files are untouched;
this tree is the cleaned-up version for verification.

## Layout

Each lab gets one folder. A lab that has several checkpoints keeps them as
subfolders (e.g. `lab04-1` is checkpoint 1 of Lab 4), and the assignment text
lives under `docs/`.

```
lab03/
  docs/       Lab 3 assignment (2026-Lab-03-UART)
  lab03-1/    exp1 Hello World
  lab03-2/    exp2 Transmit 'A'
  lab03-3/    exp3 Echo
  lab03-4/    exp4 Menu + LEDs
lab04/
  docs/       Lab 4 assignment (2026-Lab-04-NVIC)
  lab04-1/    checkpoint 1
  lab04-2/    checkpoint 2
  lab04-31/   checkpoint 3 (attempt 1)
  lab04-32/   checkpoint 3 (attempt 2)
  lab04-4/    checkpoint 4
lab05/
  docs/       Lab 5 assignment (2026-Lab-05-ADC)
  lab05-3/
  lab05-4/
  lab05-special/
  lab05-03/
lab06/docs/   2026-Lab-06-Timer   (no code yet)
lab07/docs/   2026-Lab-07-PWM     (no code yet)
lab08/docs/   2026-Lab-08-LCD     (no code yet)
```

## Notes

- `lab03` was converted from a single project that swapped `main.c` via
  `swap.sh` into four independent STM32CubeIDE projects (`lab03-1`..`lab03-4`),
  one per experiment. The `swap.sh`/`experiments/` scaffolding was removed and
  the assignment moved to `lab03/docs/`. Each child is fully self-contained
  (own `Core/`, `Drivers/`, `.ioc`, `.cproject`, `.launch`, project name).
- Dropped in this copy:
  - `lab03_backup/`
  - `Debug/` and `Release/` build outputs inside each STM32 project
    (regenerated on build; source under `Core/`, `Drivers/`, `.ioc`, `.project`
    are kept)
  - `.DS_Store`
- Every other file matches the original 1:1.
