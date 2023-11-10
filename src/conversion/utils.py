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
