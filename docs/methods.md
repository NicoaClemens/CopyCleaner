# Pre-defined methods

## Various

### `function setEncoding(string encoding) returns bool`

 - Sets the encoding used for file/console/alert/clipboard read/write
 - returns `false` if `encoding` is not a supported encoding
 - Has to be called before files/console/alerts/clipboards are accessed

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

## Alert
