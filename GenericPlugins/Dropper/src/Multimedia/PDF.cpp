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
constexpr uint32 PDF_MAGIC_START = 0x2D464450;

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

bool PDF::Check(uint64 offset, DataCache& file, BufferView precachedBuffer, Finding& finding)
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
}

} // namespace GView::GenericPlugins::Droppper::Multimedia
