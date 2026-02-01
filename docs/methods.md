# Pre-defined methods

## Various

### `function exit()`

 - exits

## clipboard

### `function clipboard_isText() returns bool`

 - checks if clipboard content is text

### `function clipboard_read() returns string`

 - reads clipboard as text
 - returns empty string if clipboard is binary

## `function clipboard_write(string message) returns bool`

 - writes `message` to clipboard
 - returns `true` on success, `false` on error (`message` too long, clipboard could not be opened)

## Logger

- log string format
  - [year-month-day hour-minute-second-ms] : [message]

### `function setLog(string)`

- sets log file path. Recognises relative and absolute path

### `function log(string)`

- logs a message. 
- Will throw an error if no log file is initialised.

## Console

Logger can be tied to console via CLI, but there is an exposed console accessor as well

### `function print(any message)`

- uses implicit conversion to string
- prints `message` into console
- does not throw an error if copycleaner is run without a console, function doesn't do anything

## Alert

### `function showAlertOK (any title, any message)`

- opens a dialogue as alert with title and message and one "OK" buttom

### `function showAlert returns boolean (any title, any message)`

- opens a dialogue as alert with title and message and an OK and a "Cancel" button. 
- Returns true if OK is pressed.

### `function showAlertYesNoCancel returns int (any title, any message)`

- opens a dialogue as alert with title and message and a "Yes", "No" and "Cancel" button
- Returns 0 for Yes, 1 for No, 2 for Cancel