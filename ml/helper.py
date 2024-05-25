import music21
import re

def abc_to_midi_string(abc_notation):
    try:
        # Attempt to parse the ABC notation
        stream = music21.converter.parse(abc_notation, format='abc')
        return abc_notation  # If parsing succeeds, return the original notation
    except Exception as e:
        print(f"Error parsing ABC notation: {e}")
        
        # Add default headers and note length if missing
        fixed_notation = add_defaults(abc_notation)
        print(f"Fixed ABC notation: {fixed_notation}")
        
        try:
            # Attempt to parse the fixed notation
            stream = music21.converter.parse(fixed_notation, format='abc')
            return fixed_notation  # If parsing succeeds, return the fixed notation
        except Exception as e:
            print(f"Error parsing fixed ABC notation: {e}")
            return None  # Return None if fixing fails
        

def add_defaults(abc):
    # Ensure X, T, M, L, and K fields are present
    if not re.search(r"X:\s*\d+", abc):
        abc = "X:1\n" + abc
    if not re.search(r"T:.*", abc):
        abc = re.sub(r"(X:\s*\d+\n)", r"\1T:Unknown\n", abc, count=1)
    if not re.search(r"M:\s*\d+/\d+", abc):
        abc = re.sub(r"(T:.*\n)", r"\1M:4/4\n", abc, count=1)
    if not re.search(r"L:\s*\d+/\d+", abc):
        abc = re.sub(r"(M:.*\n)", r"\1L:1/8\n", abc, count=1)
    if not re.search(r"K:.*", abc):
        abc = re.sub(r"(L:.*\n)", r"\1K:C\n", abc, count=1)
    return abc