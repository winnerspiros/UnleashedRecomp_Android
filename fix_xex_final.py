
file_path = 'tools/XenonRecomp/XenonUtils/xex.cpp'

with open(file_path, 'r') as f:
    lines = f.readlines()

new_lines = []
for line in lines:
    # Remove .get() calls I added, assuming I will fix them differently or revert
    # Actually, I should probably try to cast to type instead of .get() if .get() is missing?
    # But .get() IS there in the file I read.

    # Wait, the previous errors "no member named get" are very strange if get() is visible.
    # Is it possible that 'be' is specialized somewhere without get()?
    # Or maybe some macro interference?

    # I will try to use explicit cast instead of .get()
    # (uint32_t)(...)

    line = line.replace('.get()', '') # Revert .get()

    # Now add explicit casts to uint32_t for be<uint32_t> usage
    if 'image.base = *reinterpret_cast<const be<uint32_t>*>(xex2BaseAddressPtr);' in line:
         line = line.replace('image.base = *reinterpret_cast<const be<uint32_t>*>(xex2BaseAddressPtr);',
                             'image.base = (uint32_t)*reinterpret_cast<const be<uint32_t>*>(xex2BaseAddressPtr);')

    if 'image.entry_point = *reinterpret_cast<const be<uint32_t>*>(xex2EntryPointPtr);' in line:
         line = line.replace('image.entry_point = *reinterpret_cast<const be<uint32_t>*>(xex2EntryPointPtr);',
                             'image.entry_point = (uint32_t)*reinterpret_cast<const be<uint32_t>*>(xex2EntryPointPtr);')

    if 'for (size_t i = 0; i < imports->numImports; i++)' in line:
         line = line.replace('imports->numImports', '(uint32_t)imports->numImports')

    if 'auto* library = (Xex2ImportLibrary*)(((char*)imports) + sizeof(Xex2ImportHeader) + imports->sizeOfStringTable);' in line:
         line = line.replace('imports->sizeOfStringTable', '(uint32_t)imports->sizeOfStringTable')

    if '((const Xex2FileNormalCompressionInfo*)(fileFormatInfo + 1))->windowSize' in line:
         line = line.replace('((const Xex2FileNormalCompressionInfo*)(fileFormatInfo + 1))->windowSize', '(uint32_t)((const Xex2FileNormalCompressionInfo*)(fileFormatInfo + 1))->windowSize')

    if 'image.symbols.insert({ name->second, descriptors[im].firstThunk, sizeof(thunk), Symbol_Function });' in line:
         # use Symbol constructor but with cast
         line = '                        image.symbols.insert(Symbol(name->second, (uint32_t)descriptors[im].firstThunk, sizeof(thunk), Symbol_Function));\n'

    # Also handle the other errors I saw in previous logs:
    # compressionType comparison: be<short> vs enum. Cast to short?
    if 'if (fileFormatInfo->compressionType == XEX_COMPRESSION_NONE)' in line:
        line = line.replace('fileFormatInfo->compressionType', '(uint16_t)fileFormatInfo->compressionType')

    if 'else if (fileFormatInfo->compressionType == XEX_COMPRESSION_BASIC)' in line:
        line = line.replace('fileFormatInfo->compressionType', '(uint16_t)fileFormatInfo->compressionType')

    if 'else if (fileFormatInfo->compressionType == XEX_COMPRESSION_NORMAL)' in line:
        line = line.replace('fileFormatInfo->compressionType', '(uint16_t)fileFormatInfo->compressionType')

    # infoSize / sizeof
    if '(fileFormatInfo->infoSize / sizeof(Xex2FileBasicCompressionInfo))' in line:
        line = line.replace('fileFormatInfo->infoSize', '(uint32_t)fileFormatInfo->infoSize')

    # imageSize += ...
    if 'imageSize += blocks[i].dataSize + blocks[i].zeroSize;' in line:
        line = line.replace('blocks[i].dataSize', '(uint32_t)blocks[i].dataSize').replace('blocks[i].zeroSize', '(uint32_t)blocks[i].zeroSize')

    # memcpy dataSize
    if 'memcpy(destData, srcData, blocks[i].dataSize);' in line:
        line = line.replace('blocks[i].dataSize', '(uint32_t)blocks[i].dataSize')

    # srcData +=
    if 'srcData += blocks[i].dataSize;' in line:
        line = line.replace('blocks[i].dataSize', '(uint32_t)blocks[i].dataSize')

    # destData +=
    if 'destData += blocks[i].dataSize;' in line:
        line = line.replace('blocks[i].dataSize', '(uint32_t)blocks[i].dataSize')

    # memset zeroSize
    if 'memset(destData, 0, blocks[i].zeroSize);' in line:
        line = line.replace('blocks[i].zeroSize', '(uint32_t)blocks[i].zeroSize')

    # destData += zeroSize
    if 'destData += blocks[i].zeroSize;' in line:
        line = line.replace('blocks[i].zeroSize', '(uint32_t)blocks[i].zeroSize')

    # headerSize
    if 'const uint32_t headerSize = header->headerSize;' in line:
        line = line.replace('header->headerSize', '(uint32_t)header->headerSize')

    # while (blocks->blockSize)
    if 'while (blocks->blockSize)' in line:
        line = line.replace('blocks->blockSize', '(uint32_t)blocks->blockSize')

    # pNext = p + blocks->blockSize
    if 'const uint8_t* pNext = p + blocks->blockSize;' in line:
        line = line.replace('blocks->blockSize', '(uint32_t)blocks->blockSize')

    # processBytes
    if 's.processBytes(p, blocks->blockSize);' in line:
        line = line.replace('blocks->blockSize', '(uint32_t)blocks->blockSize')

    new_lines.append(line)

with open(file_path, 'w') as f:
    f.writelines(new_lines)

print("Modified xex.cpp (round 3 - using explicit casts)")
