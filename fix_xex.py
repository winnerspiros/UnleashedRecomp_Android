
file_path = 'tools/XenonRecomp/XenonUtils/xex.cpp'

with open(file_path, 'r') as f:
    lines = f.readlines()

new_lines = []
for line in lines:
    # Fix be<T> conversion issues by adding explicit cast or .get()

    # Line 271: image.base = *reinterpret_cast<const be<uint32_t>*>(xex2BaseAddressPtr);
    if 'image.base = *reinterpret_cast<const be<uint32_t>*>(xex2BaseAddressPtr);' in line:
        line = line.replace('image.base = *reinterpret_cast<const be<uint32_t>*>(xex2BaseAddressPtr);',
                            'image.base = (*reinterpret_cast<const be<uint32_t>*>(xex2BaseAddressPtr)).get();')

    # Line 276: image.entry_point = *reinterpret_cast<const be<uint32_t>*>(xex2EntryPointPtr);
    if 'image.entry_point = *reinterpret_cast<const be<uint32_t>*>(xex2EntryPointPtr);' in line:
        line = line.replace('image.entry_point = *reinterpret_cast<const be<uint32_t>*>(xex2EntryPointPtr);',
                            'image.entry_point = (*reinterpret_cast<const be<uint32_t>*>(xex2EntryPointPtr)).get();')

    # Line 303: for (size_t i = 0; i < imports->numImports; i++)
    if 'for (size_t i = 0; i < imports->numImports; i++)' in line:
        line = line.replace('i < imports->numImports', 'i < imports->numImports.get()')

    # Line 311: auto* library = (Xex2ImportLibrary*)(((char*)imports) + sizeof(Xex2ImportHeader) + imports->sizeOfStringTable);
    if 'auto* library = (Xex2ImportLibrary*)(((char*)imports) + sizeof(Xex2ImportHeader) + imports->sizeOfStringTable);' in line:
        line = line.replace('imports->sizeOfStringTable', 'imports->sizeOfStringTable.get()')

    # Line 339: image.symbols.insert({ name->second, descriptors[im].firstThunk, sizeof(thunk), Symbol_Function });
    # Fix brace initialization failure by using explicit constructor call or fixing arguments
    if 'image.symbols.insert({ name->second, descriptors[im].firstThunk, sizeof(thunk), Symbol_Function });' in line:
        # Symbol struct has: std::string name; size_t address; size_t size; SymbolType type;
        # descriptors[im].firstThunk is be<uint32_t>. Explicit cast needed?
        line = line.replace('descriptors[im].firstThunk', 'descriptors[im].firstThunk.get()')

    new_lines.append(line)

with open(file_path, 'w') as f:
    f.writelines(new_lines)

print("Modified xex.cpp")
