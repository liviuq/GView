#include "Multimedia.hpp"

namespace GView::GenericPlugins::Droppper::Multimedia
{
/*
* https://en.wikipedia.org/wiki/Magic_number_(programming)
* PDF files start with "PDF-" (hex 50 44 46 2D).
* 
* A good example from GNU:
* https://web.archive.org/web/20141010035745/http://gnupdf.org/Introduction_to_PDF
*/
constexpr uint32 PDF_MAGIC_START = 0x46445025;
constexpr std::string_view PDF_MAGIC_END{ "%%EOF" };

const std::string_view PDF::GetName() const
{
    return "PDF";
}

Category PDF::GetCategory() const
{
    return Category::Multimedia;
}

Subcategory PDF::GetSubcategory() const
{
    return Subcategory::PDF;
}

const std::string_view PDF::GetOutputExtension() const
{
    return "pdf";
}

Priority PDF::GetPriority() const
{
    return Priority::Binary;
}

bool PDF::ShouldGroupInOneFile() const
{
    return false;
}

/* bool PDF::Check(uint64 offset, DataCache& file, BufferView precachedBuffer, Finding& finding)
{
    CHECK(IsMagicU32(precachedBuffer, PDF_MAGIC_START), false, "PDF magic not found");

    finding.start = offset;
    finding.end   = offset + sizeof(PDF_MAGIC_START) + 5;
    auto pos      = finding.end;
    auto found    = false;

    while (true) {
        auto buffer = file.CopyToBuffer(pos, sizeof(uint64), true);
        CHECKBK(buffer.IsValid(), "Buffer is invalid");

    }

    // https://stackoverflow.com/questions/17279712/what-is-the-smallest-possible-valid-pdf
    CHECK(finding.end - finding.start >= 70, false, "File size too small for PDF");
    
    return true;
}*/


bool PDF::Check(uint64_t offset, DataCache& file, BufferView cachedBuffer, Finding& finding)
{
   
    if (!IsMagicU32(cachedBuffer, PDF_MAGIC_START)) {
        return false;
    }

    finding.start = offset;
    finding.end   = offset + sizeof(PDF_MAGIC_START);

    if (cachedBuffer.GetLength() > 7) {
        char majorVersion = cachedBuffer.GetData()[5];
        char minorVersion = cachedBuffer.GetData()[7];

        if (majorVersion < '1' || majorVersion > '2' || minorVersion < '0' || minorVersion > '9') {
            return false;
        }
    }

    uint64_t searchOffset = offset;
    auto chunk            = file.Get(searchOffset, file.GetCacheSize() / 8, false);
    bool foundXref        = false;
    bool foundTrailer     = false;
    bool foundStartxref   = false;

    while (chunk.GetLength() > 0) {
        for (size_t idx = 0; idx <= chunk.GetLength() - PDF_MAGIC_END.size(); ++idx) {
            if (memcmp(chunk.GetData() + idx, PDF_MAGIC_END.data(), PDF_MAGIC_END.size()) == 0) {
                
                size_t checkRegionStart = (idx > 200) ? idx - 200 : 0;

                for (size_t subIdx = checkRegionStart; subIdx < idx; ++subIdx) {
                    if (!foundXref && memcmp(chunk.GetData() + subIdx, "xref", 4) == 0) {
                        foundXref = true;
                    }
                    if (!foundTrailer && memcmp(chunk.GetData() + subIdx, "trailer", 7) == 0) {
                        foundTrailer = true;
                    }
                    if (!foundStartxref && memcmp(chunk.GetData() + subIdx, "startxref", 9) == 0) {
                        foundStartxref = true;
                    }
                }

                
                if (!foundXref || !foundTrailer || !foundStartxref) {
                    return false;
                }

                finding.end    = searchOffset + idx + PDF_MAGIC_END.size();
                finding.result = Result::Buffer;

                return true;
            }
        }

        searchOffset += chunk.GetLength() - PDF_MAGIC_END.size();
        chunk = file.Get(searchOffset, file.GetCacheSize() / 8, false);
    }

    if ((finding.end - finding.start) < 70) {
        return false;
    }

    return false; 
}

} // namespace GView::GenericPlugins::Droppper::Multimedia
