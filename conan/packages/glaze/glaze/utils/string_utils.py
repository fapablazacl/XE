def is_capitalized(value: str) -> bool:
    if value == "":
        return False

    state = 0  # 0: initial uppercase run, 1: lowercase part

    for ch in value:
        if state == 0:
            if str.isupper(ch):
                state = 1
            else:
                return False
        elif state == 1:
            if str.isupper(ch):
                continue
            elif str.islower(ch):
                state = 2
            else:
                return False
        elif state == 2 and not str.islower(ch):
            return False

    return True


def split_capitalized(value: str) -> list[str]:
    entries = []

    current = ""
    current_i = 0
    for i in range(len(value)):
        ch = value[i]

        if is_capitalized(current) and not is_capitalized(current + ch):
            entries.append(current)
            current = ""
            current_i = i

        current += ch

    entries.append(value[current_i:])

    return entries


def camel_case(class_name: str) -> str:
    return "".join([item.title() for item in class_name.split(" ")])


def version_to_int(version: str) -> int:
    """Encode a "MAJOR.MINOR" GL version string as MAJOR*10 + MINOR.

    e.g. "1.0" -> 10, "3.3" -> 33, "4.6" -> 46. Matches the convention used by
    glcorearb.h-style ``GL_VERSION_X_Y`` macros and the GLAD loader. Raises
    ``ValueError`` on malformed input.
    """
    parts = version.split(".")
    if len(parts) != 2:
        raise ValueError(f"version must be 'MAJOR.MINOR', got {version!r}")
    major, minor = parts
    return int(major) * 10 + int(minor)
