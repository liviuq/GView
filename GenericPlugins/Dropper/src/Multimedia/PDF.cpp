#include "Multimedia.hpp"

namespace GView::GenericPlugins::Droppper::Multimedia
{

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
    return true;
}

} // namespace GView::GenericPlugins::Droppper::Multimedia
