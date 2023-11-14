# To help the conversions...

def count_parentheses(text):
    open_paren = 0
    close_paren = 0
    in_single_quote = False
    in_double_quote = False
    escape_char = False

    for char in text:
        # Skip if the current character is escaped
        if escape_char:
            escape_char = False
            continue

        # If it's the escape character and we're in a quote, skip the next character
        if char == '\\' and (in_single_quote or in_double_quote):
            escape_char = True
            continue

        # Toggle the single quote flag if we're not in double quotes
        if char == "'" and not in_double_quote:
            in_single_quote = not in_single_quote

        # Toggle the double quote flag if we're not in single quotes
        elif char == '"' and not in_single_quote:
            in_double_quote = not in_double_quote

        # Count the parentheses if we're not in any quote
        elif not in_single_quote and not in_double_quote:
            if char == '(':
                open_paren += 1
            elif char == ')':
                close_paren += 1

    return open_paren, close_paren

# Pass a string AFTER the opening parenthesis and it will parse the line
# until it finds the close, and will return the index immediately after it,
# or None if it can't find it!
def find_close_parenthesis(line):
    open_parens = 1 # Already matched one to get here!
    close_parens = 0
    index = 0
    in_single_quote = False
    in_double_quote = False
    escape_char = False

    while index < len(line):
        # Skip if the current character is escaped
        if escape_char:
            escape_char = False
        else:
            # If it's the escape character and we're in a quote, skip the next character
            if line[index] == '\\' and (in_single_quote or in_double_quote):
                escape_char = True
                continue

            # Toggle the single quote flag if we're not in double quotes
            if line[index] == "'" and not in_double_quote:
                in_single_quote = not in_single_quote

            # Toggle the double quote flag if we're not in single quotes
            elif line[index] == '"' and not in_single_quote:
                in_double_quote = not in_double_quote

            elif not in_single_quote and not in_double_quote:
                if line[index] == "(":
                    open_parens += 1
                if line[index] == ")":
                    close_parens += 1
        
        index += 1
        if open_parens == close_parens:
            break
    
    if index <= len(line):
        return index
    else:
        return None
