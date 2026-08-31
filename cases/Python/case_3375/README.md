# GPIO output loopback test

This test verifies that the grabber's user-controlled GPIO output is correctly
reported on `KY_TTL_0` and on `KY_TTL_2`.  On every iteration it drives
`UserOutputValue` high and low, reads the line status after each change, and
fails if either line does not reflect the expected state.

## Prerequisites

- Vision Point API and its Python bindings must be installed and configured.
- A supported KAYA frame grabber must be connected to the host.
- Connect the GPIO lines so that the signal driven from `KY_TTL_0` can be
  observed on `KY_TTL_2` (for example, with the applicable GPIO loopback
  cable). Ensure the wiring and voltage levels match the grabber hardware
  documentation before running the test.

## Procedure

1. The test opens the selected grabber.
2. It configures `KY_TTL_0` as an output with `KY_USER_OUT_0` as its source.
3. For each trigger cycle, it sets `UserOutputValue` to `True` and verifies
   that both `KY_TTL_0` and `KY_TTL_2` report an active `LineStatus`.
4. It sets `UserOutputValue` to `False` and verifies that both lines report an
   inactive `LineStatus`.
5. The test closes the grabber and fails if any cycle had an unexpected line
   status.

## Running the test

From this directory, run:

```powershell
python .\case_3375.py --deviceIndex <index> --number_of_triggers <count>
```

Use the following command to list the available grabber indices:

```powershell
python .\case_3375.py --deviceList
```

`--number_of_triggers` defaults to `1000`. Use `--unattended` to select the
first detected grabber when no `--deviceIndex` is supplied.

## Pass criteria

The test passes when `KY_TTL_0` and `KY_TTL_2` both report `True` after every
high output transition and `False` after every low output transition. It fails
when one or more cycles report an unexpected status; the final `errorCount`
must be zero.
