import re
import sys

def fix_content(content):
    """
    Applies specific string replacements to the content of xex.cpp.
    """
    replacements = [
        (
            r'getOptHeaderPtr\(data, XEX_HEADER_FILE_FORMAT_INFO\)',
            r'getOptHeaderPtr(data, dataSize, XEX_HEADER_FILE_FORMAT_INFO)'
        ),
        (
            r'getOptHeaderPtr\(data, XEX_HEADER_IMAGE_BASE_ADDRESS\)',
            r'getOptHeaderPtr(data, dataSize, XEX_HEADER_IMAGE_BASE_ADDRESS)'
        ),
        (
            r'getOptHeaderPtr\(data, XEX_HEADER_ENTRY_POINT\)',
            r'getOptHeaderPtr(data, dataSize, XEX_HEADER_ENTRY_POINT)'
        ),
        (
            r'getOptHeaderPtr\(data, XEX_HEADER_IMPORT_LIBRARIES\)',
            r'getOptHeaderPtr(data, dataSize, XEX_HEADER_IMPORT_LIBRARIES)'
        )
    ]

    new_content = content
    for pattern, replacement in replacements:
        new_content = re.sub(pattern, replacement, new_content)

    return new_content

def main():
    file_path = 'tools/XenonRecomp/XenonUtils/xex.cpp'
    try:
        with open(file_path, 'r') as f:
            content = f.read()

        new_content = fix_content(content)

        if new_content != content:
            with open(file_path, 'w') as f:
                f.write(new_content)
            print(f"Successfully patched {file_path}")
        else:
            print(f"No changes needed for {file_path}")

    except FileNotFoundError:
        print(f"Error: File {file_path} not found.")
        sys.exit(1)
    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()
