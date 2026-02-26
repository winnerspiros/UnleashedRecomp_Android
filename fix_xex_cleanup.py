
file_path = 'tools/XenonRecomp/XenonUtils/xex.cpp'

with open(file_path, 'r') as f:
    lines = f.readlines()

new_lines = []
for line in lines:
    # Fix std::unique_ptr errors
    # compressBuffer -> compressBuffer.get() in lzxDecompress
    if 'lzxDecompress(compressBuffer' in line and 'compressBuffer.get()' not in line:
        line = line.replace('lzxDecompress(compressBuffer', 'lzxDecompress(compressBuffer.get()')

    # d = compressBuffer -> d = compressBuffer.get()
    if 'd = compressBuffer;' in line:
        line = line.replace('d = compressBuffer;', 'd = compressBuffer.get();')

    # reinterpret_cast<IMAGE_DOS_HEADER*>(image.data) -> image.data.get()
    if 'reinterpret_cast<IMAGE_DOS_HEADER*>(image.data)' in line:
        line = line.replace('image.data', 'image.data.get()')

    # auto* destData = result; -> result.get()
    if 'auto* destData = result;' in line:
        line = line.replace('result;', 'result.get();')

    # Fix remaining be<T> casts
    # (uint32_t)blocks[i].dataSize -> explicit cast helps but maybe .get() was better if available?
    # The previous attempt used casts. If they failed with "invalid cast", it means implicit conversion is missing.
    # be<T> has operator T().
    # If explicit cast fails, then implicit conversion is definitely not working.
    # But be<T> is defined in xbox.h.
    # The issue might be that I modified xbox.h and commented out stuff, maybe I broke something?
    # I verified xbox.h content, be<T> looks fine.

    # Let's try .get() again for be<T> members, as it is a method.
    # I need to revert casts to .get() where appropriate.

    # Reverting some casts to .get() calls for be<T>
    if '(uint32_t)blocks[i].dataSize' in line:
        line = line.replace('(uint32_t)blocks[i].dataSize', 'blocks[i].dataSize.get()')
    if '(uint32_t)blocks[i].zeroSize' in line:
        line = line.replace('(uint32_t)blocks[i].zeroSize', 'blocks[i].zeroSize.get()')
    if '(uint16_t)fileFormatInfo->compressionType' in line:
        line = line.replace('(uint16_t)fileFormatInfo->compressionType', 'fileFormatInfo->compressionType.get()')
    if '(uint32_t)fileFormatInfo->infoSize' in line:
        line = line.replace('(uint32_t)fileFormatInfo->infoSize', 'fileFormatInfo->infoSize.get()')
    if '(uint32_t)header->headerSize' in line:
        line = line.replace('(uint32_t)header->headerSize', 'header->headerSize.get()')
    if '(uint32_t)blocks->blockSize' in line:
        line = line.replace('(uint32_t)blocks->blockSize', 'blocks->blockSize.get()')

    # Fix the assignments that were failing
    # image.base = (uint32_t)*reinterpret_cast...
    # The error was "cannot convert const be to size_t".
    # Explicit cast to uint32_t then implicit to size_t should work IF operator uint32_t exists.
    # If operator T is problematic, .get() is safer.
    if 'image.base = (uint32_t)*reinterpret_cast' in line:
        line = line.replace('(uint32_t)*reinterpret_cast', '(*reinterpret_cast').replace('xex2BaseAddressPtr);', 'xex2BaseAddressPtr)).get();')

    if 'image.entry_point = (uint32_t)*reinterpret_cast' in line:
        line = line.replace('(uint32_t)*reinterpret_cast', '(*reinterpret_cast').replace('xex2EntryPointPtr);', 'xex2EntryPointPtr)).get();')

    if '(uint32_t)imports->numImports' in line:
        line = line.replace('(uint32_t)imports->numImports', 'imports->numImports.get()')

    if '(uint32_t)imports->sizeOfStringTable' in line:
        line = line.replace('(uint32_t)imports->sizeOfStringTable', 'imports->sizeOfStringTable.get()')

    if '(uint32_t)((const Xex2FileNormalCompressionInfo*)' in line:
        line = line.replace('(uint32_t)((const Xex2FileNormalCompressionInfo*)', '((const Xex2FileNormalCompressionInfo*)').replace('->windowSize', '->windowSize.get()')

    new_lines.append(line)

with open(file_path, 'w') as f:
    f.writelines(new_lines)

print("Modified xex.cpp (round 4 - fixing unique_ptr and reverting to .get())")
