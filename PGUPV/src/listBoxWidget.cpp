
#include "listBoxWidget.h"

#include <guipg.h>

using PGUPV::ListBoxWidget;


bool PGUPV::renderListBox(uint32_t id, const std::string & label, int *i, const char **ptr, size_t nelems, int numItems)
{
	return GUILib::ListBox(id, label, i, ptr, nelems, numItems);
}
