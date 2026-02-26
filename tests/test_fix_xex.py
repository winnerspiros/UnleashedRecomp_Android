import pytest
from fix_xex import fix_content

def test_fix_content_file_format_info():
    """Test replacement of getOptHeaderPtr for FILE_FORMAT_INFO."""
    original = 'const auto* fileFormatInfo = reinterpret_cast<const Xex2OptFileFormatInfo*>(getOptHeaderPtr(data, XEX_HEADER_FILE_FORMAT_INFO));'
    expected = 'const auto* fileFormatInfo = reinterpret_cast<const Xex2OptFileFormatInfo*>(getOptHeaderPtr(data, dataSize, XEX_HEADER_FILE_FORMAT_INFO));'
    assert fix_content(original) == expected

def test_fix_content_image_base_address():
    """Test replacement of getOptHeaderPtr for IMAGE_BASE_ADDRESS."""
    original = 'const void* xex2BaseAddressPtr = getOptHeaderPtr(data, XEX_HEADER_IMAGE_BASE_ADDRESS);'
    expected = 'const void* xex2BaseAddressPtr = getOptHeaderPtr(data, dataSize, XEX_HEADER_IMAGE_BASE_ADDRESS);'
    assert fix_content(original) == expected

def test_fix_content_entry_point():
    """Test replacement of getOptHeaderPtr for ENTRY_POINT."""
    original = 'const void* xex2EntryPointPtr = getOptHeaderPtr(data, XEX_HEADER_ENTRY_POINT);'
    expected = 'const void* xex2EntryPointPtr = getOptHeaderPtr(data, dataSize, XEX_HEADER_ENTRY_POINT);'
    assert fix_content(original) == expected

def test_fix_content_import_libraries():
    """Test replacement of getOptHeaderPtr for IMPORT_LIBRARIES."""
    original = 'auto* imports = reinterpret_cast<const Xex2ImportHeader*>(getOptHeaderPtr(data, XEX_HEADER_IMPORT_LIBRARIES));'
    expected = 'auto* imports = reinterpret_cast<const Xex2ImportHeader*>(getOptHeaderPtr(data, dataSize, XEX_HEADER_IMPORT_LIBRARIES));'
    assert fix_content(original) == expected

def test_fix_content_no_change():
    """Test that content without target strings remains unchanged."""
    original = 'This is a test string without any targets.'
    assert fix_content(original) == original

def test_fix_content_already_fixed():
    """Test that already fixed content remains unchanged (idempotency)."""
    original = 'getOptHeaderPtr(data, dataSize, XEX_HEADER_FILE_FORMAT_INFO)'
    assert fix_content(original) == original

def test_fix_content_multiple_occurrences():
    """Test replacement of multiple occurrences."""
    original = (
        'getOptHeaderPtr(data, XEX_HEADER_FILE_FORMAT_INFO)\n'
        'getOptHeaderPtr(data, XEX_HEADER_ENTRY_POINT)'
    )
    expected = (
        'getOptHeaderPtr(data, dataSize, XEX_HEADER_FILE_FORMAT_INFO)\n'
        'getOptHeaderPtr(data, dataSize, XEX_HEADER_ENTRY_POINT)'
    )
    assert fix_content(original) == expected

def test_fix_content_mixed_context():
    """Test replacement within a larger code block."""
    original = """
    Image Xex2LoadImage(const uint8_t* data, size_t dataSize)
    {
        auto* header = reinterpret_cast<const Xex2Header*>(data);
        const auto* fileFormatInfo = reinterpret_cast<const Xex2OptFileFormatInfo*>(getOptHeaderPtr(data, XEX_HEADER_FILE_FORMAT_INFO));

        // ... code ...

        const void* xex2EntryPointPtr = getOptHeaderPtr(data, XEX_HEADER_ENTRY_POINT);
        return image;
    }
    """
    expected = """
    Image Xex2LoadImage(const uint8_t* data, size_t dataSize)
    {
        auto* header = reinterpret_cast<const Xex2Header*>(data);
        const auto* fileFormatInfo = reinterpret_cast<const Xex2OptFileFormatInfo*>(getOptHeaderPtr(data, dataSize, XEX_HEADER_FILE_FORMAT_INFO));

        // ... code ...

        const void* xex2EntryPointPtr = getOptHeaderPtr(data, dataSize, XEX_HEADER_ENTRY_POINT);
        return image;
    }
    """
    assert fix_content(original) == expected
