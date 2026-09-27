# Project proposal: TapLock

Date: _date of submission_
Repository: https://github.com/f1f2f0e0e4e0ede8ff0e/taplock

## Idea

I want to build a small RFID lock with an Arduino UNO. A card or key fob is touched to an RC522 reader and the
Arduino checks its UID against a list of allowed cards. For an allowed card a servo turns to the open position
for five seconds and a buzzer gives a short high beep. An unknown card gets a long low beep and the servo does
not move. After three wrong cards in a row the reader ignores all cards for 30 seconds.

Every attempt is sent to the computer over the serial port as a CSV line, and a Python script saves it to a
file with the date and time. That file is the data for the experiments.

## Goals

1. Read the UID of the cards and key fobs with the RC522 over SPI.
2. Compare it with the list of allowed cards and move the servo only for those.
3. Give a different sound for accepted and refused cards, and block the reader after three wrong cards.
4. Save every attempt to a file and use the reader for a few measurements.

## Components

All from the laboratory inventory:

| Part | Model |
|---|---|
| Board | KS0001 UNO R3 |
| RFID reader | KS0067 RC522 RFID Module, with cards and key fobs |
| Lock | KS0209 Micro Servo 9G |
| Buzzer | KS0019 Passive Buzzer Alarm Module |
| Wiring | Breadboard, jumper wires, USB cable |

## Plan

| When | Work | Deadline |
|---|---|---|
| until 1 October | Choose the idea, check the parts in the inventory, write this proposal | Proposal, 1 October |
| first half of October | Wokwi: read a UID first, then add the list check, the servo, the buzzer and the lockout | |
| second half of October | Show the Wokwi design to the teacher and collect the parts | Wokwi prototype, 1 November |
| November | Build it on the breadboard, test each part on its own first, register the real cards | |
| early December | Measurements and analysis | |
| until 20 December | Report, photos, demo video, GitHub Pages | Final project, 20 December |

## Experiments

The measurements are about the reader:

- from how far a card and a key fob are read, since their antennas are different sizes;
- how often the card is read when it is tilted to 0, 30, 60 and 90 degrees;
- whether the card is still read inside a metal card holder, behind a phone, inside a notebook and with a second
  RFID card on top of it.

I repeat each condition ten times and compare the averages. In the report I also want to explain why
checking only the UID is weak, since a UID can be copied, and what reading a key-protected sector of the
card would change.

## Risks

| Risk | What I do about it |
|---|---|
| The RC522 runs on 3.3 V and 5 V can damage it | Check the power wire twice before plugging in the USB cable |
| The module or the cards might not work | Test them first with a small UID sketch; the inventory has a second module |
| The servo may pull too much current from USB and reset the board | If it happens, give the servo its own 5 V supply with a common ground |
| Long jumper wires can disturb SPI | Keep the reader wires short |
| Some cards may have 7-byte UIDs | Check the UID length on the first day and extend the comparison if needed |

## What I need

The parts listed above with the cards and key fobs from the RC522 kit, after the Wokwi design is approved.
