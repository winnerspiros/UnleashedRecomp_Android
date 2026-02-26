
file_path = 'tools/XenonRecomp/XenonUtils/xex.cpp'

with open(file_path, 'r') as f:
    lines = f.readlines()

new_lines = []
for line in lines:
    # Fix missed lzxDecompress call
    # ((const Xex2FileNormalCompressionInfo*)(fileFormatInfo + 1))->windowSize
    if '->windowSize' in line and '.get()' not in line:
        line = line.replace('->windowSize', '->windowSize.get()')

    # Fix Symbol insertion
    # image.symbols.insert({ name->second, descriptors[im].firstThunk.get(), sizeof(thunk), Symbol_Function });
    if 'image.symbols.insert({' in line:
        line = line.replace('image.symbols.insert({', 'image.symbols.insert(Symbol(')
        line = line.replace('});', '));')

    new_lines.append(line)

with open(file_path, 'w') as f:
    f.writelines(new_lines)

print("Modified xex.cpp (round 2)")
