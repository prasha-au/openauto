
# Macro Pad

A [Macro Keyboard](https://www.aliexpress.com/item/1005005771118023.html) serves as an input instead of the touch screen sending basic keypresses.

The utility to program it can be found [here](https://github.com/kriomant/ch57x-keyboard-tool?tab=readme-ov-file). The configuration file is as follows:



```yaml
# Normal keyboard orientation is when
# buttons are on the left side and knobs are on the right.
# However, you may want to use the keyboard in another orientation.
# To avoid remapping button positions in your head, just set it here.
# Possible values are:
#   (horizontal)
#   - 'normal': buttons on the left, knobs on the right
#   - 'upsidedown': buttons on the right, knobs on the left
#   (vertical)
#   - 'clockwise': buttons on the top, knobs on the bottom
#   - 'counterclockwise': buttons on the bottom, knobs on the top
orientation: counterclockwise

# Different keyboard models have different numbers of buttons and knobs.
# Set it here for proper handling.
# Count rows and columns with the keyboard in normal orientation (knobs on the right)
rows: 2
columns: 3
knobs: 1

# Layers are sets of alternative key mappings.
# The current layer is changed using a button on the side of the keyboard
# and displayed with LEDs on top (only for the moment of changing).
# All keyboards I saw had three layers, but I suppose other variants exist.
layers:
  - buttons:
      # Array of buttons.
      # In horizontal orientations it's `rows` rows `columns` buttons each.
      # In vertical: `columns` rows `rows` buttons each.
      # Each entry is either a sequence of 'chords' or a mouse event.
      # A chord is a combination of one key with optional modifiers,
      # like 'b', 'ctrl-alt-a' or 'win-rctrl-backspace'.
      # It can also be just modifiers without a key: 'ctrl-alt'.
      # You may combine up to 5 chords into a sequence using commas: 'ctrl-v,ctrl-c'.
      # Arbitrary scan codes (decimal) may be given like this: '<101>'.
      - ["down", "up"]
      - ["left", "right"]
      - ["j", "h"]
    knobs:
      # Knobs are listed from left to right if horizontal.
      # Knobs are listed from top to bottom if vertical.
      # Knobs can be rotated counter-clockwise (ccw) or clockwise (cw)
      # and pressed down.
      - ccw: "1"
        press: "enter"
        cw: "2"

```

